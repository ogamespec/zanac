/* apu.c - 2A03/2A07 audio processing unit.
 *
 * Reference material: Specs/03-apu-*.md (see apu.h).  One APU cycle is two CPU
 * cycles (03-apu-overview.md section 2), so the pulse/noise/DMC/frame-counter
 * machinery advances on every other CPU cycle, while the triangle frequency
 * counter is stepped once per CPU cycle (03-apu-triangle.md section 2).
 *
 * SPEC GAP APU-8: the specification does not state how ACLK1//ACLK2 align to
 * PHI1/PHI2, nor the APU-cycle parity at reset, so the APU phase here is
 * arbitrary-but-consistent.  See SPEC-GAPS.md.
 */
#include <string.h>

#include "nes.h"
#include "apu.h"

/* Length counter reload values (03-apu-pulse.md section 4, cross-checked
 * against the decimal table in 03-apu-noise.md section 4). */
const uint8_t apu_length_table[32] = {
    10, 254, 20,   2, 40,   4, 80,   6, 160,   8, 60,  10, 14, 12, 26, 14,
    12,  16, 24,  18, 48,  20, 96,  22, 192,  24, 72,  26, 16, 28, 32, 30
};

/* Duty sequences as the die decodes them (square.v SQUARE_Duty):
 *   in[0] = DC0 & DC1 & DC2, in[1] = DC1 & DC2, in[2] = DC2,
 *   in[3] = ~(DC1 & DC2)
 * The 3-bit counter is loaded with 0 by $4003/$4007 and then counts *down*,
 * so the table is indexed by the current counter value and the published
 * sequence is played as 0,7,6,5,4,3,2,1 - the community tables are the same
 * waveform rotated, which moves the phase after every $4003 write. */
static const uint8_t apu_duty[4][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 1 }, /* 12.5%          */
    { 0, 0, 0, 0, 0, 0, 1, 1 }, /* 25%            */
    { 0, 0, 0, 0, 1, 1, 1, 1 }, /* 50%            */
    { 1, 1, 1, 1, 1, 1, 0, 0 }  /* 25% negated    */
};

/* Noise period tables.  The published tables (03-apu-noise.md section 3) are
 * in CPU cycles, but noise.v clocks the noise frequency counter from ACLK4
 * (the APU cycle, i.e. every 2 CPU cycles), and the counter reloads after
 * exactly NNF APU cycles.  The hardware NNF values derived from the
 * NOISE_Decoder1/NOISE_Decoder2 PLA equal the published CPU-cycle periods
 * divided by two, so these tables are the APU-cycle periods.  Using the
 * CPU-cycle figures in an APU-cycle counter doubled every period (a full
 * octave low) and added one extra APU cycle per shift. */
static const uint16_t apu_noise_period_ntsc[16] = {
    2, 4, 8, 16, 32, 48, 64, 80, 101, 127, 190, 254, 381, 508, 1017, 2034
};
static const uint16_t apu_noise_period_pal[16] = {
    2, 4, 7, 15, 30, 44, 59, 74, 94, 118, 177, 236, 354, 472, 945, 1889
};

/* DMC rate tables: period in CPU cycles (03-apu-dmc.md section 3). */
static const uint16_t apu_dmc_period_ntsc[16] = {
    428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54
};
static const uint16_t apu_dmc_period_pal[16] = {
    398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118, 98, 78, 66, 50
};

/* The frame sequencer as the hardware actually divides it: one entry per
 * activation, holding the CPU cycle (counted from the sequence start) at which
 * it happens and the signals it emits.  The `at` values are NESdev's APU-cycle
 * table doubled into CPU cycles (APU_Frame_Counter): mode 0 activates at
 * 7457 / 14913 / 22371 / 29828 (interrupt) / 29829 (last clock) and restarts
 * one cycle later, so the interrupt period is exactly 29830 CPU cycles - which
 * is why the last interval is not a uniform 7457.  Mode 1 has no interrupt and
 * its fifth activation is at 37281. */
typedef struct {
    int  at;                    /* CPU cycles from the sequence start */
    bool quarter, half;
    uint8_t irq;                /* 0 = none, 1 = set, 2 = terminal set (a
                                 * terminal step clears the flag instead while
                                 * the interrupt is inhibited) */
    bool restart;               /* this activation restarts the sequence */
} ApuFrameEvent;

static const ApuFrameEvent apu_ev4_ntsc[] = {
    {  7457, true,  false, 0, false },
    { 14913, true,  true,  0, false },
    { 22371, true,  false, 0, false },
    { 29828, false, false, 1, false },
    { 29829, true,  true,  1, false },
    { 29830, false, false, 2, true  }
};
static const ApuFrameEvent apu_ev4_pal[] = {
    {  8313, true,  false, 0, false },
    { 16627, true,  true,  0, false },
    { 24939, true,  false, 0, false },
    { 33252, false, false, 1, false },
    { 33253, true,  true,  1, false },
    { 33254, false, false, 2, true  }
};
static const ApuFrameEvent apu_ev5_ntsc[] = {
    {  7457, true,  false, 0, false },
    { 14913, true,  true,  0, false },
    { 22371, true,  false, 0, false },
    { 29829, false, false, 0, false },
    { 37281, true,  true,  0, false },
    { 37282, false, false, 0, true  }
};
static const ApuFrameEvent apu_ev5_pal[] = {
    {  8313, true,  false, 0, false },
    { 16627, true,  true,  0, false },
    { 24939, true,  false, 0, false },
    { 33253, false, false, 0, false },
    { 41565, true,  true,  0, false },
    { 41566, false, false, 0, true  }
};

static const ApuFrameEvent *apu_frame_events(const Apu *apu)
{
    if (apu->nes->region == NES_REGION_NTSC)
        return apu->frame_mode ? apu_ev5_ntsc : apu_ev4_ntsc;
    return apu->frame_mode ? apu_ev5_pal : apu_ev4_pal;
}

static void apu_refresh_status(Apu *apu);
static void audio_capture(Apu *apu);

/* ------------------------------------------------------------ DAC tables -- */
/* Transfer curve of 03-apu-mixer.md section 4.3:
 *   R_int = 39k / g_A (AUX A), 78k / g_B (AUX B);  V = 5.0 * ExtRes/(R_int+ExtRes)
 * The full 218-entry AUX B table is not printed in the specification
 * (SPEC GAP MIX-1), only the formula, so the table is computed from it. */
static float apu_auxa[31];
static float apu_auxb[218];
static bool  apu_tables_ready = false;

static void apu_build_tables(void)
{
    int i;
    /* The transfer curve is built in millivolts so that it matches the
     * normalisation in apu_mix() (03-apu-mixer.md section 3 gives V_pin in mV:
     * AUX A full scale 357.14 mV, AUX B 1088.26 mV).  Building it in volts and
     * then dividing by the mV full-scale figure attenuated the whole mix by
     * 1000x - about -60 dB, i.e. inaudible. */
    const float vcc = 5000.0f, ext = 100.0f;
    for (i = 0; i < 31; i++) {
        float rint = (i == 0) ? 999999.0f : (39000.0f / (float)i);
        apu_auxa[i] = vcc * ext / (rint + ext);
    }
    for (i = 0; i < 218; i++) {
        float rint = (i == 0) ? 999999.0f : (78000.0f / (float)i);
        apu_auxb[i] = vcc * ext / (rint + ext);
    }
    apu_tables_ready = true;
}

/* ------------------------------------------------------------------ init -- */

void apu_init(Apu *apu, struct Nes *nes)
{
    memset(apu, 0, sizeof(*apu));
    apu->nes = nes;
    if (!apu_tables_ready) apu_build_tables();
    neslib_apu_reset(apu);
}

void neslib_apu_reset(Apu *apu)
{
    int c;
    for (c = 0; c < 2; c++) {
        ApuPulse *p = &apu->pulse[c];
        p->enabled = false;
        p->duty = 0;
        p->halt = false;
        p->constant_volume = false;
        p->volume = 0;
        p->timer = 0;
        p->timer_counter = 0;
        p->duty_counter = 0;
        p->length = 0;
        p->env_start = false;
        p->env_divider = 0;
        p->env_decay = 0;
        p->sweep_enable = false;
        p->sweep_period = 0;
        p->sweep_negate = false;
        p->sweep_shift = 0;
        p->sweep_divider = 0;
        p->sweep_reload = false;
        p->output = 0;
    }
    apu->triangle.enabled = false;
    apu->triangle.control = false;
    apu->triangle.linear_reload = 0;
    apu->triangle.reload_flag = false;
    apu->triangle.linear_counter = 0;
    apu->triangle.timer = 0;
    apu->triangle.timer_counter = 0;
    apu->triangle.length = 0;
    apu->triangle.seq = 0;      /* cleared by RES; output 15 (03-apu-triangle.md) */
    apu->triangle.output = 15;

    apu->noise.enabled = false;
    apu->noise.halt = false;
    apu->noise.constant_volume = false;
    apu->noise.volume = 0;
    apu->noise.period_index = 0;
    apu->noise.mode = false;
    apu->noise.timer_counter = 0;
    /* The random LFSR has no reset in the schematic; NESdev reports a power-on
     * seed of 1 (03-apu-noise.md Open questions), which is used here. */
    apu->noise.lfsr = 1;
    apu->noise.length = 0;
    apu->noise.env_start = false;
    apu->noise.env_divider = 0;
    apu->noise.env_decay = 0;
    apu->noise.output = 0;

    apu->dmc.enabled = false;
    apu->dmc.irq_enable = false;
    apu->dmc.loop = false;
    apu->dmc.rate_index = 0;
    apu->dmc.rate_counter = 0;
    /* The DMC's $4012/$4013 reload shadows have no reset in the die (plain
     * registers power up at 0), so a channel enabled before either is written
     * plays one byte from $C000 + 64*0 = $C000.  Leaving the shadows at 0 made
     * the emulator fetch from $0000 and, because $4013 = 0 means "0 bytes" in
     * the shadow instead of the hardware's 16*0+1 = 1 byte, refuse to fetch at
     * all ($4015 bit 4 stayed set and no IRQ was ever raised). */
    apu->dmc_sample_base = 0xC000;
    apu->dmc_sample_len = 1;
    apu->dmc.sample_addr = 0xC000;
    apu->dmc.sample_len = 0;
    apu->dmc.sample_buffer = 0;
    apu->dmc.buffer_full = false;
    apu->dmc.bits_remaining = 8;
    apu->dmc.shift_reg = 0;
    apu->dmc.output = 0;
    apu->dmc.irq_flag = false;
    apu->dmc.start_pending = false;
    apu->dmc.silence = true;

    apu->status = 0;
    apu->frame_irq = false;
    apu->frame_irq_line = false;
    apu->frame_irq_clear_pending = false;
    apu->frame_mode = 0;
    apu->frame_reset_count = 0;
    apu->frame_irq_inhibit = false;
    apu->frame_event = 0;
    apu->frame_cycles = apu_frame_events(apu)[0].at;
    apu->apu_cycles = 0;
    apu->phase = false;
    apu->out_latch = 0;
    apu->pad_out = false;
    apu->lock = false;

    /* Audio capture stays enabled across a reset (apu_audio_enable owns
     * audio_step); only the in-flight decimation state is restarted. */
    apu->audio_accum = 0.0;
    apu->audio_sum = 0.0;
    apu->audio_cycles = 0;
    apu->audio_hp_x = 0.0;
    apu->audio_hp_y = 0.0;
    apu->audio_head = 0;
    apu->audio_tail = 0;
}

/* ----------------------------------------------------------- length units -- */

static void length_clock(Apu *apu)
{
    int i;
    ApuPulse *p;
    for (i = 0; i < 2; i++) {
        p = &apu->pulse[i];
        if (p->enabled && !p->halt && p->length > 0)
            p->length--;
    }
    if (apu->triangle.enabled && !apu->triangle.control && apu->triangle.length > 0)
        apu->triangle.length--;
    if (apu->noise.enabled && !apu->noise.halt && apu->noise.length > 0)
        apu->noise.length--;
}

/* --------------------------------------------------------------- envelope -- */

static uint8_t env_clock(bool start, uint8_t *divider, uint8_t *decay,
                         bool loop, uint8_t period)
{
    if (start) {
        *decay = 15;
        *divider = period;
        return 15;
    }
    if (*divider == 0) {
        *divider = period;
        if (*decay > 0) (*decay)--;
        else if (loop) *decay = 15;
    } else {
        (*divider)--;
    }
    return *decay;
}

static void quarter_clock(Apu *apu)
{
    int i;
    for (i = 0; i < 2; i++) {
        ApuPulse *p = &apu->pulse[i];
        p->env_decay = env_clock(p->env_start, &p->env_divider, &p->env_decay,
                                 p->halt, p->volume);
        p->env_start = false;
    }
    apu->noise.env_decay = env_clock(apu->noise.env_start, &apu->noise.env_divider,
                                     &apu->noise.env_decay, apu->noise.halt,
                                     apu->noise.volume);
    apu->noise.env_start = false;

    /* Linear counter: reload if the flag is set (and skip the decrement),
     * otherwise decrement; then clear the flag when the control bit is 0
     * (03-apu-triangle.md section 3). */
    if (apu->triangle.reload_flag) {
        apu->triangle.linear_counter = apu->triangle.linear_reload;
    } else if (apu->triangle.linear_counter > 0) {
        apu->triangle.linear_counter--;
    }
    if (!apu->triangle.control)
        apu->triangle.reload_flag = false;
}

static void half_clock(Apu *apu)
{
    int i;
    length_clock(apu);

    for (i = 0; i < 2; i++) {
        ApuPulse *p = &apu->pulse[i];
        uint16_t target;
        bool overflow, underflow, nosq, suppressed;

        if (p->sweep_reload) {
            p->sweep_divider = p->sweep_period;
            p->sweep_reload = false;
        } else if (p->sweep_divider > 0) {
            p->sweep_divider--;
        }

        nosq = (p->length == 0);
        underflow = (p->timer & 0x7FC) == 0;   /* current period < 4 */
        target = p->timer >> p->sweep_shift;
        if (p->sweep_negate) {
            uint16_t sub = (uint16_t)(target + (i == 0 ? 1 : 0));
            overflow = false;
            if (p->timer >= sub) target = (uint16_t)(p->timer - sub);
            else target = 0;
        } else {
            uint32_t sum = (uint32_t)p->timer + target;
            overflow = sum > 0x7FF;
            target = (uint16_t)(sum & 0x7FF);
        }

        suppressed = !p->sweep_enable || nosq || p->sweep_shift == 0 ||
                     overflow || underflow || p->sweep_divider != 0;
        if (!suppressed)
            p->timer = target;
    }
}

/* ----------------------------------------------------------- frame counter -- */

static void frame_counter_clock(Apu *apu)
{
    const ApuFrameEvent *ev = apu_frame_events(apu);
    const ApuFrameEvent *e = &ev[apu->frame_event];

    if (e->quarter) quarter_clock(apu);
    if (e->half) half_clock(apu);

    /* clkgen.v wires mask_clear ($4017 bit 6) to int_ff's asynchronous reset,
     * so while the interrupt is inhibited the flag cannot be set at all: $4015
     * bit 6 reads 0 and the CPU /IRQ line never asserts while the inhibit is
     * held.  The emulator used to set the flag regardless and gate only the
     * /IRQ line, which made $4015 bit 6 read 1 for the two CPU cycles of the
     * IRQ activation in every frame.  An activation also cancels a pending
     * $4015-read clear. */
    if (e->irq) {
        apu->frame_irq_clear_pending = false;
        if (!apu->frame_irq_inhibit) {
            apu->frame_irq = true;
            apu->frame_irq_line = true;
        }
    }

    apu->frame_event++;
    if (e->restart) {
        /* The sequence restarts here; step 1 of the next sequence follows a
         * full first interval later, so the restart cycle itself is counted. */
        apu->frame_event = 0;
        apu->frame_cycles = ev[0].at;
        return;
    }
    apu->frame_cycles = ev[apu->frame_event].at - e->at;
}

/* ------------------------------------------------------------------- DMC -- */

static void dmc_note_stop(Apu *apu);

bool apu_dmc_wants_fetch(Apu *apu)
{
    ApuDmc *d = &apu->dmc;
    return d->enabled && !d->buffer_full && d->sample_len > 0;
}

uint16_t apu_dmc_fetch_addr(Apu *apu) { return apu->dmc.sample_addr; }

/* CPU cycles until the output unit ends the current cycle and empties the
 * sample buffer, i.e. until a reload DMA would be scheduled (NESdev "APU_DMC",
 * Output unit).  -1 when the channel cannot schedule one. */
int apu_dmc_reload_eta(Apu *apu)
{
    ApuDmc *d = &apu->dmc;
    uint16_t period = (apu->nes->region == NES_REGION_NTSC)
                        ? apu_dmc_period_ntsc[d->rate_index & 0x0F]
                        : apu_dmc_period_pal[d->rate_index & 0x0F];
    if (!d->enabled || d->silence || !d->buffer_full)
        return -1;
    /* The output cycle ends on the tick that brings the bits-remaining
     * counter to zero; the first tick is `rate_counter` cycles away and the
     * rest are one period apart. */
    return d->rate_counter + (int)(d->bits_remaining - 1) * (int)period;
}

static void dmc_note_stop(Apu *apu)
{
    apu->dmc_stopped = true;
    apu->dmc_stop_cycle = apu->nes->cpu_cycles;
    apu->dmc_stop_eta = apu_dmc_reload_eta(apu);
}

/* The output unit has played the last buffered byte of a non-looping sample:
 * stop the channel.  The IRQ flag is NOT set here - it was already set by the
 * fetch that exhausted the byte counter, one 8-bit buffer earlier
 * (03-apu-dmc.md sections 5 and 8, phenomenon 7). */
static void dmc_end(Apu *apu)
{
    ApuDmc *d = &apu->dmc;
    dmc_note_stop(apu);
    d->enabled = false;
    apu_refresh_status(apu);
}

void apu_dmc_fetch_complete(Apu *apu, uint8_t value)
{
    ApuDmc *d = &apu->dmc;
    d->sample_buffer = value;
    d->buffer_full = true;
    /* `silence` tracks the output unit's step flip-flop, which only BLOAD
     * (loading the byte into the shift register at a bit-counter boundary)
     * clears - fetching into the sample buffer does not start the output unit
     * (dpcm.v DPCM_SampleBufferControl). */
    /* Byte consumed: the counter decrements and the address advances
     * (03-apu-dmc.md section 5).  The channel is *not* disabled here: the
     * fetched bytes still have to play out, and $4015 bit 4 stays set until
     * they have (02-cpu-dma.md section 4). */
    if (d->sample_len > 0) d->sample_len--;
    d->sample_addr = (uint16_t)(0x8000 | ((d->sample_addr + 1) & 0x7FFF));
    /* The loop decision is taken when the counter expires, i.e. at fetch time:
     * disabling the loop flag after the last fetch still lets the already
     * scheduled next byte be fetched, which is the "next DMA at 432 cycles"
     * AccuracyCoin's DMASync counts on. */
    if (d->sample_len == 0) {
        if (d->loop) {
            d->sample_addr = apu->dmc_sample_base;
            d->sample_len = apu->dmc_sample_len;
        } else {
            if (d->irq_enable) {
                /* ED1: with LOOP clear, the bytes-remaining counter empty and
                 * the last byte latched into the buffer, the IRQ flag is set at
                 * once (03-apu-dmc.md phenomenon 7). */
                d->irq_flag = true;
            }
            /* ED1 also resets the enable flip-flop, so $4015 bit 4 reads 0 as
             * soon as the last byte has been fetched even though the output
             * unit keeps playing the buffered bits out
             * (03-apu-dmc.md section 8; AccuracyCoin "Delta Modulation
             * Channel" test I). */
            /* Playback stops implicitly when the last byte of a non-looping
             * sample is fetched; the DMC abort bug needs that moment
             * (NESdev "DMA", Bugs).  Note the stop before the enable is
             * cleared so the reload ETA is computed against a live channel. */
            dmc_note_stop(apu);
            if (d->enabled) {
                d->enabled = false;
                apu_refresh_status(apu);
            }
        }
    }
}

static void dmc_step(Apu *apu)
{
    ApuDmc *d = &apu->dmc;
    uint16_t period = (apu->nes->region == NES_REGION_NTSC)
                        ? apu_dmc_period_ntsc[d->rate_index & 0x0F]
                        : apu_dmc_period_pal[d->rate_index & 0x0F];

    /* The rate table is in CPU cycles (03-apu-dmc.md section 3), so the timer
     * is clocked once per CPU cycle: one bit per `period` cycles.  Both the
     * rate divider and the output unit's bits-remaining counter free-run,
     * whether or not the channel is enabled - that free-running phase is what
     * AccuracyCoin's DMA tests observe. */
    if (d->rate_counter > 0) {
        d->rate_counter--;
        if (d->rate_counter != 0)
            return;
    }
    d->rate_counter = period;

    /* Clock the output unit (03-apu-dmc.md section 6, "Output unit"). */
    if (!d->silence) {
        uint8_t bit = (uint8_t)(d->shift_reg & 1);
        d->shift_reg >>= 1;
        if (bit) {
            if (d->output < 126) d->output = (uint8_t)(d->output + 2);
        } else {
            if (d->output > 1) d->output = (uint8_t)(d->output - 2);
        }
    }
    if (d->bits_remaining > 0)
        d->bits_remaining--;

    if (d->bits_remaining == 0) {
        /* End of an output cycle: start the next one.  An empty sample buffer
         * silences the unit; a full one is copied into the shift register and
         * the buffer then requests the next fetch. */
        bool was_playing = !d->silence;
        d->bits_remaining = 8;
        if (d->buffer_full) {
            d->shift_reg = d->sample_buffer;
            d->buffer_full = false;
            d->silence = false;
        } else {
            d->silence = true;
            if (was_playing && d->sample_len == 0)
                dmc_end(apu);
        }
    }
}

/* ---------------------------------------------------------------- channels -- */

/* Recompute SQA/SQB from the current duty counter value.  The gate mirrors
 * square.v SQUARE_Output: the latch output is DUTY & ~SW_UVF & ~NOSQ &
 * ~SW_OVF, then LOCK clears it, and the volume/envelope nibble is passed
 * through.  Called both from pulse_step() and from a $4003/$4007 write, since
 * the hardware decode is combinational and a phase reset therefore shows the
 * counter's zero state immediately (square.v SQUARE_Duty). */
static void pulse_refresh_output(Apu *apu, int idx)
{
    ApuPulse *p = &apu->pulse[idx];
    bool nosq = (p->length == 0);
    bool underflow = (p->timer & 0x7FC) == 0;
    bool overflow = false;
    uint8_t vol;

    if (!p->sweep_negate) {
        uint16_t target = (uint16_t)(p->timer >> p->sweep_shift);
        if ((uint32_t)p->timer + target > 0x7FF) overflow = true;
    }
    vol = p->constant_volume ? p->volume : p->env_decay;
    if (apu_duty[p->duty & 3][p->duty_counter & 7] &&
        !nosq && !underflow && !overflow && !apu->lock)
        p->output = vol;
    else
        p->output = 0;
}

static void pulse_step(Apu *apu, int idx)
{
    ApuPulse *p = &apu->pulse[idx];

    if (p->timer_counter == 0) {
        p->timer_counter = p->timer;
        p->duty_counter = (uint8_t)((p->duty_counter - 1) & 7);
    } else {
        p->timer_counter--;
    }
    pulse_refresh_output(apu, idx);
}

static void triangle_step(Apu *apu)
{
    ApuTriangle *t = &apu->triangle;
    bool gate;
    if (t->timer_counter == 0) {
        t->timer_counter = t->timer;
        gate = t->linear_counter > 0 && t->length > 0 && !apu->lock;
        if (gate) t->seq = (uint8_t)((t->seq + 1) & 0x1F);
    } else {
        t->timer_counter--;
    }
    /* The 32-step sequence is 15,14,...,0,0,1,...,15
     * (03-apu-triangle.md section 2). */
    if (t->seq < 16) t->output = (uint8_t)(15 - t->seq);
    else             t->output = (uint8_t)(t->seq - 16);
}

static void noise_step(Apu *apu)
{
    ApuNoise *n = &apu->noise;
    uint16_t period = (apu->nes->region == NES_REGION_NTSC)
                        ? apu_noise_period_ntsc[n->period_index & 0x0F]
                        : apu_noise_period_pal[n->period_index & 0x0F];
    uint8_t vol;

    /* The frequency counter reloads itself when it has counted down to its
     * last state, so the shift lands exactly `period` APU cycles after the
     * previous one (noise.v NOISE_FreqLFSR: NFLOAD fires while the counter
     * holds its terminal value). */
    if (n->timer_counter > 0)
        n->timer_counter--;
    if (n->timer_counter == 0) {
        uint16_t tap;
        uint16_t feedback;
        n->timer_counter = period;
        /* Feedback: bit 0 XOR bit 1 (mode 0, 32767 steps) or bit 6 (mode 1,
         * 93 steps); all-zero is escaped (03-apu-noise.md section 4). */
        if (n->mode) tap = 6; else tap = 1;
        feedback = (uint16_t)((n->lfsr & 1) ^ ((n->lfsr >> tap) & 1));
        if ((n->lfsr & 0x7FFF) == 0) feedback = 1;
        n->lfsr = (uint16_t)((n->lfsr >> 1) | (feedback << 14));
        if ((n->lfsr & 0x7FFF) == 0) n->lfsr = 1;
    }

    /* noise.v NOISE_RandomLFSR: RNDOUT = (sout[0] | NORND) & ~LOCK and
     * RND_out = ~(~Vol | {4{RNDOUT}}), so the channel is silenced while bit 0
     * of the shift register is *set* - the opposite of the pulse channels'
     * DUTY polarity. */
    vol = n->constant_volume ? n->volume : n->env_decay;
    if ((n->lfsr & 1) != 0 || n->length == 0 || apu->lock)
        n->output = 0;
    else
        n->output = vol;
}

/* -------------------------------------------------------------- register -- */

static void apu_refresh_status(Apu *apu)
{
    apu->status = (uint8_t)((apu->pulse[0].enabled ? 0x01 : 0) |
                            (apu->pulse[1].enabled ? 0x02 : 0) |
                            (apu->triangle.enabled ? 0x04 : 0) |
                            (apu->noise.enabled ? 0x08 : 0) |
                            (apu->dmc.enabled ? 0x10 : 0));
}

void apu_write(Apu *apu, uint16_t addr, uint8_t value)
{
    switch (addr) {
    case 0x4000: case 0x4004: {
        ApuPulse *p = &apu->pulse[addr == 0x4000 ? 0 : 1];
        p->duty = (uint8_t)(value >> 6);
        p->halt = (value & 0x20) != 0;
        p->constant_volume = (value & 0x10) != 0;
        p->volume = (uint8_t)(value & 0x0F);
        break;
    }
    case 0x4001: case 0x4005: {
        ApuPulse *p = &apu->pulse[addr == 0x4001 ? 0 : 1];
        p->sweep_enable = (value & 0x80) != 0;
        p->sweep_period = (uint8_t)((value >> 4) & 7);
        p->sweep_negate = (value & 0x08) != 0;
        p->sweep_shift = (uint8_t)(value & 7);
        p->sweep_reload = true;
        break;
    }
    case 0x4002: apu->pulse[0].timer = (uint16_t)((apu->pulse[0].timer & 0x700) | value); break;
    case 0x4006: apu->pulse[1].timer = (uint16_t)((apu->pulse[1].timer & 0x700) | value); break;
    case 0x4003: case 0x4007: {
        ApuPulse *p = &apu->pulse[addr == 0x4003 ? 0 : 1];
        p->timer = (uint16_t)((p->timer & 0x0FF) | ((uint16_t)(value & 7) << 8));
        if (p->enabled) p->length = apu_length_table[value >> 3];
        p->env_start = true;
        p->duty_counter = 0;
        /* The duty decode is combinational, so the phase reset is audible on
         * the same cycle rather than at the next timer reload. */
        pulse_refresh_output(apu, addr == 0x4003 ? 0 : 1);
        break;
    }
    case 0x4008:
        apu->triangle.control = (value & 0x80) != 0;
        apu->triangle.linear_reload = (uint8_t)(value & 0x7F);
        break;
    case 0x400A:
        apu->triangle.timer = (uint16_t)((apu->triangle.timer & 0x700) | value);
        break;
    case 0x400B:
        apu->triangle.timer = (uint16_t)((apu->triangle.timer & 0x0FF) |
                                         ((uint16_t)(value & 7) << 8));
        if (apu->triangle.enabled) apu->triangle.length = apu_length_table[value >> 3];
        apu->triangle.reload_flag = true;
        break;
    case 0x400C:
        apu->noise.halt = (value & 0x20) != 0;
        apu->noise.constant_volume = (value & 0x10) != 0;
        apu->noise.volume = (uint8_t)(value & 0x0F);
        break;
    case 0x400E:
        apu->noise.mode = (value & 0x80) != 0;
        apu->noise.period_index = (uint8_t)(value & 0x0F);
        break;
    case 0x400F:
        if (apu->noise.enabled) apu->noise.length = apu_length_table[value >> 3];
        apu->noise.env_start = true;
        break;
    case 0x4010:
        apu->dmc.irq_enable = (value & 0x80) != 0;
        if (!apu->dmc.irq_enable) apu->dmc.irq_flag = false;
        apu->dmc.loop = (value & 0x40) != 0;
        apu->dmc.rate_index = (uint8_t)(value & 0x0F);
        break;
    case 0x4011:
        /* A direct load only rewrites the DAC register.  It must not restart
         * the output unit: dpcm.v gates DSTEP with the step flip-flop that
         * BLOAD (a byte arriving in the shift register) sets, so a $4011
         * write with no sample playing leaves the level alone.  Clearing
         * `silence` here made the idle output unit shift its stale shift
         * register out and walk the level down by 2 every bit period, which
         * corrupts every $4011-driven DAC level (dac_tnd0-3 write $4011
         * continuously and never start a DMC sample). */
        apu->dmc.output = (uint8_t)(value & 0x7F);
        break;
    case 0x4012:
        /* The write loads the sample-address *reload* register only; the
         * address counter itself is loaded from it when the sample is started
         * (DSLOAD) or when a looping sample wraps (03-apu-dmc.md section 4).
         * Updating the live counter here made the wrap happen one byte early,
         * which is what AccuracyCoin's "DMC DMA Bus Conflicts" template
         * measured. */
        apu->dmc_sample_base = (uint16_t)(0xC000 + ((uint16_t)value * 0x40));
        break;
    case 0x4013:
        /* Sample length = ($4013 x 16) + 1 bytes (02-cpu-dma.md section 4):
         * a written 0 still plays one byte.  Like $4012 this is a reload
         * register; the live counter only changes at DSLOAD or at the wrap. */
        apu->dmc_sample_len = (uint16_t)((uint16_t)value * 16 + 1);
        break;
    case 0x4015: {
        bool was_enabled = apu->dmc.enabled;
        int eta_before = apu_dmc_reload_eta(apu);
        apu->pulse[0].enabled = (value & 0x01) != 0;
        apu->pulse[1].enabled = (value & 0x02) != 0;
        apu->triangle.enabled = (value & 0x04) != 0;
        apu->noise.enabled = (value & 0x08) != 0;
        apu->dmc.enabled = (value & 0x10) != 0;
        if (!apu->pulse[0].enabled) apu->pulse[0].length = 0;
        if (!apu->pulse[1].enabled) apu->pulse[1].length = 0;
        if (!apu->triangle.enabled) apu->triangle.length = 0;
        if (!apu->noise.enabled) apu->noise.length = 0;
        if (apu->dmc.enabled && !was_enabled) {
            /* A $4015 bit-4 0 -> 1 transition performs DSLOAD: it reloads the
             * address and length counters and restarts playback.  A repeat
             * write of 1 while the channel is already enabled does not
             * retrigger and leaves the buffered byte alone
             * (03-apu-dmc.md section 8, AccuracyCoin "Delta Modulation
             * Channel" test 4). */
            apu->dmc.sample_addr = apu->dmc_sample_base;
            apu->dmc.sample_len = apu->dmc_sample_len;
            /* A byte already waiting in the sample buffer is *kept*: in dpcm.v
             * stop_ff (CTRL1) is set only by PCM and cleared only by BLOAD, and
             * start_set requires it clear, so a disable -> enable while a byte
             * waits issues no fetch at all - the stale byte plays first and the
             * next fetch, from the reloaded address, only happens once BLOAD has
             * emptied the buffer.  Discarding it here made the emulator fetch
             * `base` immediately and play [base][base+1]... instead of the
             * hardware's [stale][base][base+1]... */
        }
        if (!apu->dmc.enabled && was_enabled) {
            /* Playback stopped explicitly: the DMC abort bug needs to know
             * when this happened and how far away the next reload was
             * (NESdev "DMA", Bugs). */
            apu->dmc_stopped = true;
            apu->dmc_stop_cycle = apu->nes->cpu_cycles;
            apu->dmc_stop_eta = eta_before;
        }
        if (!apu->dmc.enabled) {
            apu->dmc.irq_flag = false;
        }
        /* Any write to $4015 clears the DMC IRQ flag. */
        apu->dmc.irq_flag = false;
        apu_refresh_status(apu);
        break;
    }
    case 0x4016:
        apu->out_latch = (uint8_t)(value & 7);
        break;
    case 0x4017:
        /* The mode and interrupt-inhibit flags apply at once, but the timer
         * reset only lands 3 or 4 CPU cycles after the write, depending on
         * where the write falls in the APU cycle (NESdev "APU Frame Counter":
         * a write during an APU cycle takes 3 cycles, one between APU cycles
         * takes 4).  The mode-1 immediate quarter/half clocks are emitted at
         * that same delayed point, not at the write itself, and each write
         * queues its own - two back-to-back writes clock the length counters
         * twice (AccuracyCoin "APU Length Counter" test 4). */
        apu->frame_irq_inhibit = (value & 0x40) != 0;
        if (apu->frame_irq_inhibit) {
            apu->frame_irq = false;
            apu->frame_irq_line = false;
            apu->frame_irq_clear_pending = false;
        }
        apu->frame_mode = (uint8_t)((value >> 7) & 1);
        {
            bool get = ((apu->nes->cpu_cycles & 1) != 0) == apu->nes->dma_get_parity;
            int delay = get ? 4 : 3;   /* 4 cycles for a write between APU cycles */
            if (apu->frame_reset_count <
                (int)(sizeof(apu->frame_reset) / sizeof(apu->frame_reset[0]))) {
                ApuFrameReset *fr = &apu->frame_reset[apu->frame_reset_count++];
                fr->delay = delay;
                fr->mode = apu->frame_mode;
            } else {
                /* More writes in flight than the hardware can hold apart: fold
                 * the oldest into the new one by keeping the new delay. */
                ApuFrameReset *fr = &apu->frame_reset[apu->frame_reset_count - 1];
                fr->delay = delay;
                fr->mode = apu->frame_mode;
            }
        }
        break;
    default:
        break;
    }
}

uint8_t apu_read(Apu *apu, uint16_t addr)
{
    switch (addr) {
    case 0x4015: {
        uint8_t v = 0;
        if (apu->pulse[0].length > 0) v |= 0x01;
        if (apu->pulse[1].length > 0) v |= 0x02;
        if (apu->triangle.length > 0) v |= 0x04;
        if (apu->noise.length > 0) v |= 0x08;
        if (apu->dmc.enabled) v |= 0x10;
        if (apu->frame_irq) v |= 0x40;
        if (apu->dmc.irq_flag) v |= 0x80;
        /* The frame interrupt flag is not cleared by the read itself but on
         * the next put -> get transition of the APU cycle (AccuracyCoin
         * "Frame Counter IRQ" tests 6 and 7): a read taken on a put cycle
         * sees the flag cleared by the very next read, one taken on a get
         * cycle does not. */
        apu->frame_irq_clear_pending = true;
        /* The CPU /IRQ line drops with the read, while the bit itself stays
         * visible until the next get cycle. */
        apu->frame_irq_line = false;
        return v;
    }
    case 0x4018: /* 2A03 test register: SQB SQA (DBG = 1 only) */
        return (uint8_t)((apu->pulse[1].output << 4) | apu->pulse[0].output);
    case 0x4019:
        return (uint8_t)((apu->noise.output << 4) | apu->triangle.output);
    case 0x401A:
        return (uint8_t)(apu->dmc.output & 0x7F);
    default:
        return 0;
    }
}

bool apu_irq(Apu *apu)
{
    return apu->frame_irq_line || (apu->dmc.irq_flag && apu->dmc.irq_enable);
}

/* ------------------------------------------------------------------- tick -- */

void apu_frame_tick(Apu *apu)
{
    bool get = ((apu->nes->cpu_cycles & 1) != 0) == apu->nes->dma_get_parity;
    int i, n;

    /* A pending $4015 read clears the frame interrupt flag on the put -> get
     * transition of the APU cycle, so a read taken on a put cycle is already
     * cleared by the next read while a get-cycle read still returns it. */
    if (apu->frame_irq_clear_pending && get) {
        apu->frame_irq_clear_pending = false;
        apu->frame_irq = false;
    }

    /* Advance the sequence before applying any reset queued in this cycle, so
     * that an event scheduled by a reset lands exactly `at` cycles after it
     * instead of one cycle early. */
    if (apu->frame_cycles > 0) {
        apu->frame_cycles--;
        if (apu->frame_cycles == 0)
            frame_counter_clock(apu);
    }

    /* In-flight $4017 writes: each restarts the sequencer (and, in 5-step
     * mode, emits a quarter and a half frame clock) 3 or 4 CPU cycles after
     * its write cycle.  In clkgen.v the immediate clock is the Z2 pulse in the
     * nACLK2 phase of the cycle *before* F1 reloads the LFSR, so it lands one
     * CPU cycle ahead of the restart. */
    n = 0;
    for (i = 0; i < apu->frame_reset_count; i++) {
        ApuFrameReset fr = apu->frame_reset[i];
        fr.delay--;
        if (fr.delay == 1 && fr.mode) {
            /* One CPU cycle before the restart: the Z2 pulse. */
            quarter_clock(apu);
            half_clock(apu);
        }
        if (fr.delay <= 0) {
            const ApuFrameEvent *ev = apu_frame_events(apu);
            apu->frame_event = 0;
            apu->frame_cycles = ev[0].at;
        } else {
            apu->frame_reset[n++] = fr;
        }
    }
    apu->frame_reset_count = n;
}

void apu_tick(Apu *apu)
{
    apu->phase = !apu->phase;

    /* The pad-side output latch follows the $4016 register only at ACLK2
     * (put) edges; a register bit that is high for part of a single put cycle
     * never reaches the controllers. */
    if ((((apu->nes->cpu_cycles & 1) != 0) == apu->nes->dma_get_parity))
        apu->pad_out = (apu->out_latch & 1) != 0;

    /* The triangle frequency counter is stepped once per CPU cycle
     * (03-apu-triangle.md section 2). */
    triangle_step(apu);

    /* The DMC bit timer is specified in CPU cycles and advances every CPU
     * cycle; the frame counter has its own start-of-cycle tick (apu_frame_tick)
     * so that its clocks are visible to a read on the same cycle.  The
     * pulse/noise timers count APU cycles and stay in the phase-gated half
     * below. */
    dmc_step(apu);

    if (!apu->phase) {
        /* only the second half advances the APU-cycle machinery */
        audio_capture(apu);
        return;
    }

    apu->apu_cycles++;
    pulse_step(apu, 0);
    pulse_step(apu, 1);
    noise_step(apu);
    audio_capture(apu);
}

/* ------------------------------------------------------------------- mix -- */

float apu_mix(Apu *apu)
{
    int g_a = apu->pulse[0].output + apu->pulse[1].output;
    int g_b = 4 * apu->triangle.output + 2 * apu->noise.output + apu->dmc.output;
    float va, vb;
    if (g_a > 30) g_a = 30;
    if (g_b > 217) g_b = 217;
    va = apu_auxa[g_a];
    vb = apu_auxb[g_b];
    /* AUX A and AUX B are two separate pins (dac.v DAC_Square / DAC_Others)
     * that the board sums through one network, so each contributes in
     * proportion to its own pin voltage.  The previous normalise-then-weight
     * version (va/357.14*0.45 + vb/1088.26*0.55) scaled AUX B down by 1088/357
     * a second time and left a single full-volume pulse channel ~2.5x louder
     * than a full-volume triangle; on hardware it is the triangle that is
     * louder (db_apu compares exactly this pair, and the community mixer model
     * gives 0.149 : 0.247 for it).  The result is unipolar 0..1: silence sits
     * at (0.5+0.5)/1445.4 and both channels at full scale reach 1.0. */
    return (va + vb) / (357.14f + 1088.26f);
}

/* ---------------------------------------------------- audio sample capture -- */

/* CPU clock of the region, used for the decimation ratio.  Must match the
 * figure the frontend uses for its own timing. */
static double apu_cpu_clock(const Apu *apu)
{
    return (apu->nes->region == NES_REGION_PAL) ? 1662607.0 : 1789772.7;
}

void apu_audio_enable(Apu *apu, double sample_rate)
{
    apu->audio_head = 0;
    apu->audio_tail = 0;
    apu->audio_accum = 0.0;
    apu->audio_sum = 0.0;
    apu->audio_cycles = 0;
    apu->audio_dropped = 0;
    apu->audio_hp_x = 0.0;
    apu->audio_hp_y = 0.0;
    apu->audio_step = (sample_rate > 0.0) ? apu_cpu_clock(apu) / sample_rate : 0.0;
    /* ~5 Hz one-pole high pass (the board's coupling capacitor). */
    apu->audio_hp_coef = (sample_rate > 0.0)
                            ? 1.0 - (2.0 * 3.14159265358979 * 5.0) / sample_rate
                            : 1.0;
}

int apu_audio_available(const Apu *apu)
{
    int n = apu->audio_tail - apu->audio_head;
    if (n < 0) n += (int)(sizeof(apu->audio_buf) / sizeof(apu->audio_buf[0]));
    return n;
}

int apu_audio_read(Apu *apu, float *dst, int max)
{
    int n = 0;
    int cap = (int)(sizeof(apu->audio_buf) / sizeof(apu->audio_buf[0]));
    while (n < max && apu->audio_head != apu->audio_tail) {
        dst[n++] = apu->audio_buf[apu->audio_head];
        apu->audio_head = (apu->audio_head + 1) % cap;
    }
    return n;
}

/* One CPU cycle of the mixer output.  The value is accumulated and, once a
 * whole output sample period has elapsed, emitted as its average - a box
 * filter over exactly one output sample (see apu.h).  This is what makes the
 * difference between "a 60 Hz staircase of frame-boundary mixer values" and
 * actual audio: the mixer is read at the CPU rate, not once per video frame. */
static void audio_capture(Apu *apu)
{
    double v;
    if (apu->audio_step <= 0.0)
        return;
    v = (double)apu_mix(apu);
    apu->audio_sum += v;
    apu->audio_cycles++;
    apu->audio_accum += 1.0;
    if (apu->audio_accum >= apu->audio_step) {
        float out = (float)(apu->audio_sum / (double)apu->audio_cycles);
        int cap = (int)(sizeof(apu->audio_buf) / sizeof(apu->audio_buf[0]));
        int next;
        /* The DAC pins are unipolar (silence sits at ~0.5 mV, full scale at
         * ~1.0), but the board AC-couples the audio output.  Removing the DC
         * with the same one-pole high pass keeps the signal centred so the
         * frontend does not hand a large DC offset to the sound card. */
        {
            double y = (double)out - apu->audio_hp_x + apu->audio_hp_coef * apu->audio_hp_y;
            apu->audio_hp_x = out;
            apu->audio_hp_y = y;
            out = (float)y;
        }
        next = (apu->audio_tail + 1) % cap;
        if (next == apu->audio_head) {
            /* Consumer too slow: drop the oldest sample rather than stall the
             * emulator. */
            apu->audio_head = (apu->audio_head + 1) % cap;
            apu->audio_dropped++;
        }
        apu->audio_buf[apu->audio_tail] = out;
        apu->audio_tail = next;
        apu->audio_accum -= apu->audio_step;
        apu->audio_sum = 0.0;
        apu->audio_cycles = 0;
    }
}
