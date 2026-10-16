/* apu.h - 2A03/2A07 audio processing unit.
 *
 * Reference material:
 *   Specs/03-apu-overview.md      - register map $4000-$401F, clocking
 *   Specs/03-apu-pulse.md         - pulse 1/2, envelope, sweep, length
 *   Specs/03-apu-triangle.md      - triangle and linear counter
 *   Specs/03-apu-noise.md         - noise LFSR and period tables
 *   Specs/03-apu-dmc.md           - DMC sample fetch and output
 *   Specs/03-apu-frame-counter.md - 4/5 step sequences and the frame IRQ
 *   Specs/03-apu-mixer.md         - AUX A / AUX B DAC transfer curves
 */
#ifndef APU_H
#define APU_H

#include <stdint.h>
#include <stdbool.h>

struct Nes;

/* Length counter reload values (03-apu-pulse.md / 03-apu-noise.md). */
extern const uint8_t apu_length_table[32];

typedef struct {
    bool     enabled;        /* $4015 bit */
    uint8_t  duty;           /* $4000 bits 7-6 */
    bool     halt;           /* $4000 bit 5 (length halt + envelope loop) */
    bool     constant_volume;/* $4000 bit 4 */
    uint8_t  volume;         /* $4000 bits 3-0 */
    uint16_t timer;          /* 11-bit period */
    uint16_t timer_counter;
    uint8_t  duty_counter;   /* 3-bit, counts down */
    uint8_t  length;         /* length counter */
    /* envelope */
    bool     env_start;
    uint8_t  env_divider;
    uint8_t  env_decay;
    /* sweep */
    bool     sweep_enable;
    uint8_t  sweep_period;
    bool     sweep_negate;
    uint8_t  sweep_shift;
    uint8_t  sweep_divider;
    bool     sweep_reload;
    uint8_t  output;         /* 0..15 */
} ApuPulse;

/* One in-flight $4017 write: the sequencer restart (and, in 5-step mode, the
 * immediate quarter/half clocks) lands 3 or 4 CPU cycles later.  Back-to-back
 * writes each get their own entry, so two writes produce two clocks. */
typedef struct {
    int     delay;           /* CPU cycles until this write takes effect */
    uint8_t mode;            /* $4017 bit 7 of the write */
} ApuFrameReset;

typedef struct {
    bool     enabled;
    bool     control;        /* $4008 bit 7 */
    uint8_t  linear_reload;  /* $4008 bits 6-0 */
    bool     reload_flag;
    uint8_t  linear_counter;
    uint16_t timer;
    uint16_t timer_counter;
    uint8_t  length;
    uint8_t  seq;            /* 5-bit up/down counter */
    uint8_t  output;         /* 0..15 */
} ApuTriangle;

typedef struct {
    bool     enabled;
    bool     halt;
    bool     constant_volume;
    uint8_t  volume;
    uint8_t  period_index;
    bool     mode;           /* false = white (tap 1), true = tonal (tap 6) */
    uint16_t timer_counter;
    uint16_t lfsr;           /* 15-bit */
    uint8_t  length;
    bool     env_start;
    uint8_t  env_divider;
    uint8_t  env_decay;
    uint8_t  output;         /* 0..15 */
} ApuNoise;

typedef struct {
    bool     enabled;
    bool     irq_enable;
    bool     loop;
    uint8_t  rate_index;
    uint16_t rate_counter;
    uint16_t sample_addr;    /* 15-bit */
    uint16_t sample_len;     /* 12-bit down counter (bytes*16) */
    uint8_t  sample_buffer;
    bool     buffer_full;
    uint8_t  bits_remaining; /* 8 -> 0 */
    uint8_t  shift_reg;
    uint8_t  output;         /* 0..127 */
    bool     irq_flag;
    bool     start_pending;
    bool     silence;
} ApuDmc;

typedef struct Apu {
    struct Nes *nes;

    ApuPulse    pulse[2];
    ApuTriangle triangle;
    ApuNoise    noise;
    ApuDmc      dmc;

    uint8_t  status;         /* $4015 write bits */
    bool     frame_irq;      /* $4015 read bit 6 (the visible flag) */
    bool     frame_irq_line; /* the CPU /IRQ line driven by the frame counter
                              * (only asserted while not inhibited) */
    bool     frame_irq_clear_pending; /* a $4015 read clears bit 6 on the
                                       * next put->get APU cycle transition */
    bool     frame_irq_inhibit; /* $4017 bit 6 */
    uint8_t  frame_mode;     /* $4017 bit 7: 0 = 4-step, 1 = 5-step */
    int      frame_event;    /* index into the event table */
    int      frame_cycles;   /* CPU cycles until the next event */
    ApuFrameReset frame_reset[4]; /* in-flight $4017 writes, oldest first */
    int      frame_reset_count;

    bool     dmc_stopped;    /* playback was stopped this CPU cycle */
    uint64_t dmc_stop_cycle; /* CPU cycle of that stop */
    int      dmc_stop_eta;   /* CPU cycles from the stop to the reload schedule */
    uint64_t apu_cycles;     /* APU cycles since power-on */
    bool     phase;          /* false = first half, true = second half of a CPU cycle */

    /* $4016 output register bit 0 and the pad-side strobe latch.  The pad
     * output only follows the register at ACLK2 edges, which is why a
     * one-cycle 1 -> 0 pulse can miss the controllers entirely
     * (AccuracyCoin "Controller Strobing" tests 3 and 4). */
    uint8_t  out_latch;
    bool     pad_out;

    /* ---- Audio capture -------------------------------------------------
     * apu_tick() mixes the channel outputs once per CPU cycle and decimates
     * that ~1.79 MHz stream to `audio_rate` with a box (averaging) filter.
     * Averaging over the whole output sample period is a first-order
     * anti-alias filter: it removes the worst of the decimation aliasing that
     * plain point sampling produces, at the cost of the sinc droop that any
     * box filter has.  The decimated samples are queued in a small ring buffer
     * that the frontend and the NES_WAV dumper drain.  `audio_step` is 0 while
     * capture is off, so apu_mix() is not even called. */
    double   audio_step;     /* CPU cycles per output sample (0 = disabled) */
    double   audio_accum;    /* CPU cycles accumulated toward the next sample */
    double   audio_sum;      /* sum of apu_mix() over the current interval */
    int      audio_cycles;   /* number of apu_mix() calls in that interval */
    double   audio_hp_coef;  /* one-pole DC blocker coefficient */
    double   audio_hp_x;     /* previous filter input */
    double   audio_hp_y;     /* previous filter output */
    float    audio_buf[32768];
    int      audio_head;     /* read position (== audio_tail when empty) */
    int      audio_tail;     /* write position */
    uint64_t audio_dropped;  /* samples lost to a full ring buffer */

    /* Test registers ($4018-$401A), 2A03 only. */
    bool     lock;

    /* DMC reload values captured from $4012/$4013 (DSLOAD sources). */
    uint16_t dmc_sample_base;
    uint16_t dmc_sample_len;
} Apu;

void apu_init(Apu *apu, struct Nes *nes);
void neslib_apu_reset(Apu *apu);
/* Advance one CPU cycle. */
void apu_tick(Apu *apu);
/* Frame-counter half of the APU tick, run at the *start* of a CPU cycle so
 * that a frame-counter clock is already visible to a read taken on the same
 * cycle (AccuracyCoin "Frame Counter 4-step" test 2). */
void apu_frame_tick(Apu *apu);
/* $4000-$4017 writes / $4015-$4017 reads. */
void    apu_write(Apu *apu, uint16_t addr, uint8_t value);
uint8_t apu_read(Apu *apu, uint16_t addr);

/* Current mixed output, in the range -1..+1 (03-apu-mixer.md section 4). */
float apu_mix(Apu *apu);

/* Start/stop decimated sample capture at `sample_rate` Hz.  A rate of 0 (or
 * less) disables it.  Channel emulation is unaffected either way. */
void apu_audio_enable(Apu *apu, double sample_rate);
/* Samples waiting in the ring buffer. */
int  apu_audio_available(const Apu *apu);
/* Copy at most `max` samples out; returns how many were copied. */
int  apu_audio_read(Apu *apu, float *dst, int max);

/* IRQ line: true when asserted. */
bool apu_irq(Apu *apu);

/* DMC DMA request: the caller performs the 3/4-cycle halt and returns the byte. */
bool apu_dmc_wants_fetch(Apu *apu);
/* CPU cycles from now until the output cycle that would request a reload DMA
 * (the moment the sample buffer is emptied and refilled).  -1 when no reload
 * can be scheduled. */
int  apu_dmc_reload_eta(Apu *apu);
uint16_t apu_dmc_fetch_addr(Apu *apu);
void apu_dmc_fetch_complete(Apu *apu, uint8_t value);

#endif /* APU_H */
