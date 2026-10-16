/* nes.c - system orchestration: clocks, frame loop, reset.
 *
 * Reference material:
 *   Specs/01-platform-overview.md section 2 - master clocks and CPU:PPU ratios
 *   Specs/01-platform-overview.md section 7 - reset and power-on
 *   Specs/02-cpu-dma.md                     - the CPU halt (RDY)
 */
#include <stdlib.h>
#include <string.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "cart.h"

const char *nes_region_name(NesRegion r)
{
    switch (r) {
        case NES_REGION_NTSC:  return "NTSC";
        case NES_REGION_PAL:   return "PAL";
        case NES_REGION_DENDY: return "Dendy";
        default:               return "?";
    }
}

Nes *nes_create(NesRegion region)
{
    Nes *nes = (Nes *)calloc(1, sizeof(Nes));
    if (!nes) return NULL;
    nes->region = region;
    nes->cpu = (Cpu *)calloc(1, sizeof(Cpu));
    nes->ppu = (Ppu *)calloc(1, sizeof(Ppu));
    nes->apu = (Apu *)calloc(1, sizeof(Apu));
    nes->cart = (Cart *)calloc(1, sizeof(Cart));
    if (!nes->cpu || !nes->ppu || !nes->apu || !nes->cart) {
        nes_destroy(nes);
        return NULL;
    }
    return nes;
}

void nes_destroy(Nes *nes)
{
    if (!nes) return;
    if (nes->cart) cart_free(nes->cart);
    free(nes->cart);
    free(nes->apu);
    free(nes->ppu);
    free(nes->cpu);
    free(nes);
}

void nes_power_on(Nes *nes)
{
    /* RAM contents at power-on are undefined (01-platform-overview.md
     * section 7).  A fixed, deterministic pattern is used so that a run is
     * reproducible; this is a deliberate choice, not a spec requirement. */
    uint32_t x = 0x1BADB002u;
    int i;
    for (i = 0; i < (int)sizeof(nes->ram); i++) {
        x ^= x << 13; x ^= x >> 17; x ^= x << 5;
        nes->ram[i] = (uint8_t)(x & 0xFF);
    }
    memset(nes->cart->prg_ram, 0, sizeof(nes->cart->prg_ram));

    nes->cpu_open_bus = 0xFF;
    nes->ext_bus = 0xFF;
    nes->cpu_cycles = 0;
    nes->frame_count = 0;
    nes->dma_get_parity = true;
    nes->cpu_halted = false;
    nes->cpu_halt_addr = 0;
    nes->oam_dma_pending = false;
    nes->oam_dma_active = false;
    nes->oam_dma_page = 0;
    nes->oam_dma_pos = 0;
    nes->oam_dma_want = 0;
    nes->oam_dma_buffer = 0;
    nes->dmc_phase = NES_DMC_IDLE;
    nes->dmc_attempt = false;
    nes->dmc_halt_on_get = false;
    nes->dmc_load_delay = 0;
    nes->dmc_dma_addr = 0;
    nes->dmc_dma_value = 0;
    nes->dmc_last_cycle = 0;

    /* PPU revision: 2C02G behaviour (OAMDATA readable, palette reads work). */
    nes->ppu_rev.oam_readable = true;
    nes->ppu_rev.palette_readable = true;
    nes->ppu_rev.oamaddr_corruption = true;

    input_init(&nes->input);
    cpu_init(nes->cpu, nes);
    ppu_init(nes->ppu, nes, nes->cart);
    apu_init(nes->apu, nes);
    nes_reset(nes);
}

void nes_reset(Nes *nes)
{
    cpu_reset(nes->cpu);
    ppu_reset(nes->ppu);
    neslib_apu_reset(nes->apu);
    nes->oam_dma_pending = false;
    nes->oam_dma_active = false;
    nes->cpu_halted = false;
    nes->dmc_phase = NES_DMC_IDLE;
    nes->dmc_attempt = false;
    nes->dmc_load_delay = 0;
    nes->dmc_halt_on_get = false;
}

void nes_set_region(Nes *nes, NesRegion region)
{
    nes->region = region;
    ppu_configure(nes->ppu, (int)region);
    neslib_apu_reset(nes->apu);
    ppu_reset(nes->ppu);
    cpu_reset(nes->cpu);
}

/* ------------------------------------------------------------- scheduling -- */

/* True when the given CPU cycle is an ACLK1 "get" slot (DMA can read on it).
 * The power-on CPU/APU alignment is not documented; AccuracyCoin detects the
 * parity at run time, so either choice is acceptable (02-cpu-dma.md section 3,
 * SPEC GAP DMA-2). */
static bool dma_cycle_is_get(Nes *nes, uint64_t cycle)
{
    return ((cycle & 1) != 0) == nes->dma_get_parity;
}

static bool dma_get_slot(Nes *nes)
{
    return dma_cycle_is_get(nes, nes->cpu_cycles);
}

/* Complete a DMC fetch: the register decoder sees {CPU_A[15:5], bus_A[4:0]},
 * so a fetch whose low five address bits select $4015/$4016/$4017 while the
 * halted core sits in $4000-$401F collides with that register
 * (02-cpu-dma.md section 5). */
static uint8_t dmc_bus_read(Nes *nes, uint16_t addr)
{
    uint8_t v = nes_cpu_read(nes, addr);
    if ((nes->cpu_halt_addr & 0xFFE0) == 0x4000) {
        uint16_t reg = (uint16_t)(0x4000 | (addr & 0x1F));
        if (reg == 0x4015) {
            /* The status read does not drive the data bus; it only clears the
             * frame IRQ flag.  The DMA byte is unaffected. */
            apu_read(nes->apu, 0x4015);
        } else if (reg == 0x4016 || reg == 0x4017) {
            /* The controller drives bits 4-0; bits 7-5 come from the sample
             * byte the DMA is reading. */
            uint8_t c = input_read(&nes->input, reg == 0x4016 ? 0 : 1,
                                   nes->cpu_open_bus);
            v = (uint8_t)((c & 0x1F) | (v & 0xE0));
        }
    }
    return v;
}

static void dma_hold_cpu(Nes *nes, uint16_t addr)
{
    nes->cpu_halted = true;
    nes->cpu_halt_addr = addr;
}

/* A read performed by the OAM DMA engine.  The APU/IO register block responds
 * only when the *6502* address bus sits inside $4000-$401F; the register index
 * itself comes from the address the OAM address bus drives (the registers
 * mirror every $20 bytes).  An OAM DMA out of page $40 with the core halted
 * elsewhere therefore reads open bus and, in particular, must not clear the
 * frame IRQ flag by touching $4015 (AccuracyCoin "APU Register Activation"
 * tests 4 and 5).  When the core *is* in $4000-$401F the register drives only
 * the bits it implements and the byte the DMA address selects supplies the
 * rest - the bus conflict of test 7. */
static uint8_t oam_bus_read(Nes *nes, uint16_t addr)
{
    uint8_t v;
    bool reg_range = (addr >= 0x4000 && addr < 0x4020);
    bool active = (nes->cpu_halt_addr & 0xFFE0) == 0x4000;

    if (reg_range) {
        if (!active)
            return nes->ext_bus;
        v = nes->ext_bus;
    } else {
        v = nes_cpu_read(nes, addr);
        if (!active)
            return v;
    }

    switch (addr & 0x1F) {
    case 0x15: {
        /* $4015 drives every bit except bit 5, which the external bus
         * supplies (03-apu-frame-counter.md section 2). */
        uint8_t s = apu_read(nes->apu, 0x4015);
        return (uint8_t)((s & 0xDF) | (v & 0x20));
    }
    case 0x16:
    case 0x17: {
        /* The controller drives bits 4-0, the address byte the rest. */
        uint8_t c = input_read(&nes->input, (addr & 1) ? 1 : 0, nes->ext_bus);
        return (uint8_t)((c & 0x1F) | (v & 0xE0));
    }
    default:
        /* $4000-$4014 are write-only and $4018-$401F unmapped: nothing
         * drives the bus. */
        return v;
    }
}

/* Release RDY once neither engine needs the core stopped. */
static void dma_release_cpu(Nes *nes)
{
    if (!nes->oam_dma_active && !nes->oam_dma_pending &&
        nes->dmc_phase == NES_DMC_IDLE)
        nes->cpu_halted = false;
}

/* /NMI is the PPU /INT output: it is low while INT_FF is set and $2000[7] is
 * enabled (04-ppu-timing.md section 7; 04-ppu-registers.md section 2).  The
 * level is evaluated after every PPU dot, not once per CPU cycle: a $2000[7]
 * write that enables the NMI and the RESCL that ends vblank can fall inside
 * the same CPU cycle, and that short pulse must still be seen by the CPU's
 * falling-edge latch (AccuracyCoin "NMI at VBlank end"). */
static void nes_update_nmi(Nes *nes)
{
    bool int_now = nes->ppu->int_ff && ((nes->ppu->ctrl & 0x80) != 0);
    cpu_set_nmi(nes->cpu, !int_now);
}

void nes_run_cpu_cycle(Nes *nes)
{
    Cpu *cpu = nes->cpu;
    bool get;
    bool cpu_read = false, cpu_write = false;
    uint16_t cpu_addr = 0;
    bool halt_now = false;
    bool oam_drive = false, oam_is_write = false;
    bool dmc_drive = false;

    /* Count every CPU cycle, including the cycles the CPU spends halted for
     * DMA.  The counter is the system's CPU-time base. */
    nes->cpu_cycles++;
    apu_frame_tick(nes->apu);
    input_write_strobe(&nes->input, nes->apu->pad_out ? 1 : 0);
    get = dma_get_slot(nes);
    /* A $2002 read only suppresses /VSET when the read and the set land on the
     * same PPU clock.  The core's read happens at the start of the CPU cycle,
     * so the suppression counts only when the set dot is that first dot. */
    nes->ppu->vblank_read_suppress = false;
    nes->ppu->vblank_set_in_cycle =
        ppu_vblank_line(nes->ppu) && nes->ppu->dot == 1;

    /* ---- DMC fetch scheduling ---------------------------------------- */
    if (nes->dmc_load_delay > 0)
        nes->dmc_load_delay--;
    if (nes->dmc_phase == NES_DMC_IDLE && nes->dmc_load_delay == 0 &&
        apu_dmc_wants_fetch(nes->apu)) {
        nes->dmc_phase = NES_DMC_WAIT_SLOT;
        nes->dmc_attempt = false;
        /* The halt attempt lands on the next slot of the scheduled kind: a get
         * for a load fetch, a put for a reload. */
        nes->dmc_target_cycle = (get == nes->dmc_halt_on_get)
                                    ? nes->cpu_cycles : nes->cpu_cycles + 1;
    }

    /* The explicit/implicit stop bug (NESdev "DMA", Bugs): if playback stops on
     * the 2nd or 3rd CPU cycle before a scheduled reload halt, the DMA starts
     * and is aborted after a single cycle. */
    if (nes->apu->dmc_stopped) {
        uint64_t stop = nes->apu->dmc_stop_cycle;
        uint64_t target = 0;
        nes->apu->dmc_stopped = false;
        if (nes->dmc_phase == NES_DMC_WAIT_SLOT && !nes->dmc_halt_on_get) {
            target = nes->dmc_target_cycle;
        } else if (nes->apu->dmc_stop_eta >= 0) {
            /* The buffer empties `eta` cycles after the stop, and the request
             * is seen on the following CPU cycle; the reload's halt attempt
             * then lands on the first put slot at or after that. */
            target = stop + (uint64_t)nes->apu->dmc_stop_eta;
            if (dma_cycle_is_get(nes, target))
                target++;   /* a reload halts on a put slot */
        }
        if (target) {
            int64_t delta = (int64_t)target - (int64_t)stop;
            if (delta == 2 || delta == 3) {
                /* Stopped on the 2nd or 3rd cycle before the halt: the DMA
                 * starts and is aborted after a single cycle. */
                nes->dmc_abort_cycle = target;
                nes->dmc_phase = NES_DMC_IDLE;
                nes->dmc_attempt = false;
            } else if (delta == 1) {
                /* The reload was already committed when playback stopped, so
                 * it still runs as an ordinary fetch. */
                nes->dmc_phase = NES_DMC_WAIT_SLOT;
                nes->dmc_halt_on_get = false;
                nes->dmc_attempt = false;
                nes->dmc_target_cycle = target;
            }
        }
    }

    /* ---- The cycle the core is about to execute ---------------------- */
    if (!nes->cpu_halted && cpu_peek_bus(cpu, &cpu_addr, &cpu_write))
        cpu_read = !cpu_write;

    /* ---- the aborted fetch: exactly one halt cycle ---- */
    if (nes->dmc_abort_hold) {
        /* The single aborted halt cycle is over: let the core continue. */
        nes->dmc_abort_hold = false;
        dma_release_cpu(nes);
    }
    if (nes->dmc_abort_cycle) {
        if (nes->cpu_cycles == nes->dmc_abort_cycle) {
            if (!nes->cpu_halted && cpu_read) {
                dma_hold_cpu(nes, cpu_addr);
                halt_now = true;
                nes->dmc_abort_hold = true;
            }
            /* A halt that lands on a write cycle does not happen at all. */
            nes->dmc_abort_cycle = 0;
        } else if (nes->cpu_cycles > nes->dmc_abort_cycle) {
            nes->dmc_abort_cycle = 0;
        }
    }

    /* ---- OAM DMA: accept the halt on a read cycle -------------------- */
    if (nes->oam_dma_pending && !nes->oam_dma_active) {
        if (!nes->cpu_halted) {
            if (cpu_read) {
                dma_hold_cpu(nes, cpu_addr);
                halt_now = true;
            }
            /* A write cycle is completed by the core; the DMA keeps waiting. */
        }
        if (nes->cpu_halted) {
            nes->oam_dma_pending = false;
            nes->oam_dma_active = true;
            nes->oam_dma_pos = 0;
            nes->oam_dma_want = 0;
        }
    }

    /* ---- DMC DMA: scheduled slot, then a retried halt ---------------- */
    if (nes->dmc_phase == NES_DMC_WAIT_SLOT) {
        if (!nes->dmc_attempt && get == nes->dmc_halt_on_get)
            nes->dmc_attempt = true;
        if (nes->dmc_attempt) {
            if (nes->cpu_halted) {
                /* The core is already stopped (an OAM transfer): the halt
                 * cycles coincide (NESdev "DMA", DMC DMA during OAM DMA). */
                nes->dmc_phase = NES_DMC_DUMMY;
            } else if (cpu_read) {
                dma_hold_cpu(nes, cpu_addr);
                halt_now = true;
                nes->dmc_phase = NES_DMC_DUMMY;
            }
            /* A write cycle is ignored and the halt is retried next cycle. */
        }
    } else if (nes->dmc_phase == NES_DMC_DUMMY) {
        /* The dummy cycle after the halt: the core repeats its read and no
         * transfer happens. */
        nes->dmc_phase = NES_DMC_NEED_GET;
    } else if (nes->dmc_phase == NES_DMC_NEED_GET) {
        if (get)
            dmc_drive = true;
    }

    /* ---- OAM transfers (a DMC get steals the slot) ------------------- */
    if (nes->oam_dma_active && !halt_now) {
        if (!dmc_drive) {
            if (nes->oam_dma_want == 0) {
                if (get) {
                    oam_drive = true;
                    oam_is_write = false;
                }
            } else if (!get) {
                oam_drive = true;
                oam_is_write = true;
            }
        }
    }

    /* ---- Execute the cycle ------------------------------------------- */
    if (dmc_drive) {
        uint16_t addr = apu_dmc_fetch_addr(nes->apu);
        uint8_t v = dmc_bus_read(nes, addr);
        nes->dmc_dma_addr = addr;
        nes->dmc_dma_value = v;
        /* The DMC engine owns the external data bus for this cycle; the byte
         * it reads never reaches the 2A03's internal bus (AccuracyCoin
         * "Internal Data Bus" test 2). */
        nes_set_ext_bus(nes, v);
        apu_dmc_fetch_complete(nes->apu, v);
        nes->cpu->dmc_dma_seen = true;
        nes->dmc_last_cycle = nes->cpu_cycles;
        nes->dmc_phase = NES_DMC_IDLE;
        nes->dmc_halt_on_get = false;
        dma_release_cpu(nes);
    } else if (oam_drive) {
        if (!oam_is_write) {
            uint8_t b = oam_bus_read(nes, (uint16_t)((nes->oam_dma_page << 8) |
                                                     nes->oam_dma_pos));
            nes->oam_dma_buffer = b;
            nes_set_ext_bus(nes, b);
            nes->oam_dma_want = 1;
        } else {
            ppu_oam_dma_write(nes->ppu, nes->oam_dma_buffer);
            /* The DMA buffer drives the external data bus during the put. */
            nes_set_open_bus(nes, nes->oam_dma_buffer);
            nes->oam_dma_pos++;
            nes->oam_dma_want = 0;
            if (nes->oam_dma_pos >= 256) {
                nes->oam_dma_active = false;
                dma_release_cpu(nes);
            }
        }
    } else if (nes->cpu_halted) {
        /* Repeat the read the core was stopped on, side effects included. */
        uint8_t v = nes_cpu_read(nes, nes->cpu_halt_addr);
        nes_set_open_bus(nes, v);
        cpu->total_cycles++;
    } else {
        cpu_tick(cpu);
    }

    /* NTSC/Dendy: three PPU dots per CPU cycle.  PAL: 16 dots per 5 CPU
     * cycles (01-platform-overview.md section 2).  /NMI is resampled after
     * every dot (see nes_update_nmi). */
    if (nes->region == NES_REGION_PAL) {
        static const int pal_dots[5] = { 3, 3, 4, 3, 3 };
        int i;
        int n = pal_dots[nes->cpu_cycles % 5];
        for (i = 0; i < n; i++) {
            ppu_tick(nes->ppu);
            nes_update_nmi(nes);
        }
    } else {
        ppu_tick(nes->ppu);
        nes_update_nmi(nes);
        ppu_tick(nes->ppu);
        nes_update_nmi(nes);
        ppu_tick(nes->ppu);
        nes_update_nmi(nes);
    }
    apu_tick(nes->apu);

    cpu_set_irq(nes->cpu, apu_irq(nes->apu) || cart_irq(nes->cart));
}

void nes_run_frame(Nes *nes)
{
    uint64_t start = nes->frame_count;
    /* Run until the PPU has completed a frame. */
    while (nes->frame_count == start)
        nes_run_cpu_cycle(nes);
}
