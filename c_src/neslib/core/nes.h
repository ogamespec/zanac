/* nes.h - Nespeculator core types.
 *
 * Implemented strictly from the specification set in Specs (see SPEC-GAPS.md for the items the
 * specification set does not define).
 *
 * Reference material used while writing this file:
 *   Specs/01-platform-overview.md  - memory maps, clocks, regions
 *   Specs/02-cpu-core.md           - programmer model, open bus, reset
 *   Specs/04-ppu-timing.md         - frame geometry, region tables
 */
#ifndef NES_H
#define NES_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#define NES_SCREEN_W 256
#define NES_SCREEN_H 240

/* Frame geometry (04-ppu-timing.md sections 2 and 7). */
#define PPU_DOTS_PER_LINE 341
#define PPU_LINES_NTSC    262
#define PPU_LINES_PAL     312

/* CPU:PPU clock ratio, expressed as PPU dots per 16 CPU cycles so that both
 * NTSC (48/16 = 3) and PAL (51.2 -> not integer) fit.  We keep the integer
 * ratio instead: NTSC 3 dots/cycle, PAL 16 dots per 5 CPU cycles. */
typedef enum {
    NES_REGION_NTSC = 0, /* RP2A03 + RP2C02, /12 and /4 */
    NES_REGION_PAL  = 1, /* RP2A07 + RP2C07, /16 and /5 */
    NES_REGION_DENDY = 2 /* UMC 6527P + 6538, /15 and /5 (01-platform-overview 2) */
} NesRegion;

/* PPU revision knobs that the specs make observable.  Modelled as a small
 * feature set rather than a full revision list (04-ppu-overview.md section 6). */
typedef struct {
    bool oam_readable;      /* /R4 present: 2C02G and later (04-ppu-registers 6) */
    bool palette_readable;  /* palette bypass on $2007: 2C02G/H and PAL parts */
    bool oamaddr_corruption;/* 2C02G $2003 write bug (04-ppu-registers 5) */
} PpuRevision;

struct Nes;
struct Cpu;
struct Ppu;
struct Apu;
struct Cart;

/* ------------------------------------------------------------------ CPU --- */
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "cart.h"
#include "input.h"

typedef struct Nes {
    NesRegion region;
    PpuRevision ppu_rev;

    struct Cpu  *cpu;
    struct Ppu  *ppu;
    struct Apu  *apu;
    struct Cart *cart;
    Input        input;

    /* Power-on / reset */
    uint64_t cpu_cycles;
    uint64_t frame_count;
    bool     reset_pending;

    /* The 2A03 has two data buses (AccuracyCoin "Internal Data Bus"): an
     * internal bus between the CPU core and the APU/IO registers, and the
     * external bus on the chip's pins.  A CPU read of $4015 drives only the
     * internal bus, while a DMC/OAM DMA read drives only the external one. */
    uint8_t  cpu_open_bus;  /* internal data bus (IDB) */
    uint8_t  ext_bus;       /* external data bus (EDB) */
    FILE    *buslog;   /* optional bus trace sink (NES_BUSLOG) */
    uint64_t buslog_from, buslog_to;  /* CPU-cycle window for the trace (to=0: all) */

    /* Deterministic power-on RAM pattern (the spec says RAM contents are
     * undefined; a fixed pattern keeps runs reproducible). */
    uint32_t power_on_seed;

    /* Internal 2 KB work RAM ($0000-$07FF, mirrored through $1FFF). */
    uint8_t  ram[2048];

    /* ---- DMA engines (02-cpu-dma.md) --------------------------------
     * The 2A03 contains an OAM DMA engine and the DMC fetch engine.  Both
     * share the core's bus through RDY: a halt is accepted only on a read
     * cycle, and while halted the core repeats that read (with all of its
     * register side effects) on every no-op cycle. */
    bool     dma_get_parity;   /* CPU-cycle parity of an ACLK1 "get" slot */
    bool     cpu_halted;       /* RDY low: the core repeats cpu_halt_addr */
    uint16_t cpu_halt_addr;

    /* OAM DMA ($4014) */
    bool     oam_dma_pending;  /* page written, waiting for a read cycle */
    bool     oam_dma_active;   /* transferring */
    uint8_t  oam_dma_page;
    uint16_t oam_dma_pos;      /* 0..255 bytes copied */
    uint8_t  oam_dma_want;     /* 0 = needs a get, 1 = needs a put */
    uint8_t  oam_dma_buffer;

    /* DMC fetch */
    int      dmc_phase;        /* NesDmcPhase */
    bool     dmc_attempt;      /* the halt attempt has begun (retries every cycle) */
    bool     dmc_halt_on_get;  /* load fetches schedule on a get, reloads on a put */
    int      dmc_load_delay;   /* CPU cycles before a load fetch may halt */
    uint16_t dmc_dma_addr;
    uint8_t  dmc_dma_value;
    uint64_t dmc_last_cycle;   /* CPU cycle of the most recent DMC fetch */
    uint64_t dmc_target_cycle; /* CPU cycle of the scheduled halt attempt */
    uint64_t dmc_abort_cycle;  /* != 0: a one-cycle aborted fetch lands here */
    bool     dmc_abort_hold;
    int      dmc_refetch_guard;   /* the aborted halt is in progress (release next cycle) */
} Nes;

/* DMC fetch state machine (nes.c): NESdev "DMA" - the scheduled slot, the
 * retried halt, the dummy cycle and the optional alignment cycle. */
enum {
    NES_DMC_IDLE = 0,     /* no fetch in progress */
    NES_DMC_WAIT_SLOT,    /* waiting for the scheduled slot, then a read cycle */
    NES_DMC_DUMMY,        /* the dummy cycle after the halt */
    NES_DMC_NEED_GET      /* waiting for a get slot to transfer the byte */
};

/* bus.c ------------------------------------------------------------------- */
uint8_t nes_cpu_read(Nes *nes, uint16_t addr);
void    nes_dma_tick(Nes *nes);
void    nes_cpu_write(Nes *nes, uint16_t addr, uint8_t value);
/* The CPU core reports the byte it drives on the data bus for the open-bus
 * latch, including dummy reads. */
void    nes_set_open_bus(Nes *nes, uint8_t value);
void    nes_set_ext_bus(Nes *nes, uint8_t value);

/* nes.c ------------------------------------------------------------------- */
Nes *nes_create(NesRegion region);
void nes_destroy(Nes *nes);
void nes_reset(Nes *nes);
void nes_power_on(Nes *nes);
/* Run one frame (until the PPU signals the start of vblank or frame end). */
void nes_run_frame(Nes *nes);
/* Run exactly one CPU cycle (PPU dots are advanced by the ratio). */
void nes_run_cpu_cycle(Nes *nes);
void nes_set_region(Nes *nes, NesRegion region);

extern const char *nes_region_name(NesRegion r);

#endif /* NES_H */
