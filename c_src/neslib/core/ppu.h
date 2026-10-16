/* ppu.h - RP2C02/RP2C07 picture processing unit.
 *
 * Reference material:
 *   Specs/04-ppu-overview.md   - buses, DB open bus, revision list
 *   Specs/04-ppu-registers.md  - $2000-$2007, v/t/x/w, reset window
 *   Specs/04-ppu-memory-map.md - address space, mirroring, palettes, OAM
 *   Specs/04-ppu-rendering.md  - per-dot fetch schedule, priority mux, sprite 0
 *   Specs/04-ppu-sprites.md    - OAM, evaluation, the overflow bug, the FIFO
 *   Specs/04-ppu-scrolling.md  - per-dot v/t update schedule
 *   Specs/04-ppu-palette.md    - palette RAM, emphasis, greyscale
 *   Specs/04-ppu-timing.md     - frame geometry, NMI/INT, odd-frame skip
 */
#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdbool.h>

struct Nes;
struct Cart;

/* One PPU open handle: there is exactly one PPU per Nes. */
typedef struct Ppu {
    struct Nes  *nes;
    struct Cart *cart;

    /* ---- CPU-visible registers ------------------------------------- */
    uint8_t  ctrl;        /* $2000 */
    uint8_t  mask;        /* $2001 */
    uint8_t  mask_pending;      /* $2001 value still inside its write latency */
    uint8_t  addr_q[4];        /* $2006 values still inside their write latency */
    int      addr_q_delay[4];
    int      addr_qn;
    int      mask_pending_delay;
    uint8_t  status;      /* $2002 flags: bit7 vblank, bit6 sprite0, bit5 overflow */
    bool     vblank_read_suppress; /* a $2002 read happened on this CPU cycle */
    bool     vblank_set_in_cycle;  /* /VSET falls on the first dot of this CPU cycle */
    bool     int_ff;               /* /INT latch: vblank flag delayed by 2 PPU clocks */
    int      int_delay;            /* PPU clocks until int_ff follows the vblank flag */
    uint8_t  oam_addr;    /* $2003 */
    uint8_t  read_buffer; /* $2007 read buffer */

    /* Internal scroll/address registers (04-ppu-registers.md section 10) */
    uint16_t v;           /* current VRAM address */
    uint16_t t;           /* scroll/address preload */
    uint8_t  fine_x;      /* x */
    uint8_t  w;           /* first/second write latch */
    uint8_t  ppu_bus;     /* DB open bus (04-ppu-overview.md section 3) */

    /* The DB is capacitive: each data bit decays towards 0 independently.
     * One 8-bit down-counter per bit of ppu_bus, decremented once per PPU dot;
     * a bit reads as set only while its counter is non-zero.  The decay time is
     * analogue and unspecified (04-ppu-registers.md section 4); the constant
     * below is about twenty frames, the middle of the range the AccuracyCoin
     * "PPU Register Open Bus" decay sub-test recommends. */
    uint8_t  bus_decay[8];

    /* ---- timing ---------------------------------------------------- */
    int      line;        /* V counter, 0..261 (NTSC) / 0..311 (PAL) */
    int      dot;         /* H counter, 0..340 */
    bool     frame_odd;   /* Even/Odd latch (04-ppu-timing.md section 8) */
    bool     odd_frame_skip; /* this is a short (odd) NTSC frame */
    uint64_t dot_count;
    bool     nmi_out;     /* /INT level, wired to the CPU /NMI */
    bool     nmi_prev;

    /* Reset register-clear window: writes to $2000/$2001/$2005/$2006 are
     * ignored for ~29658 NTSC CPU cycles after reset
     * (04-ppu-registers.md section 11). */
    bool     rc_active;
    uint64_t rc_until_cpu_cycle;

    /* ---- memory ---------------------------------------------------- */
    uint8_t  ciram[2048];   /* board VRAM (U4) */
    uint8_t  pal[32];       /* palette RAM, 6 bits per entry */
    uint8_t  oam[256];      /* primary OAM, 64 sprites x 4 bytes */
    uint8_t  oam2[32];      /* secondary OAM */
    uint8_t  oam2_addr;     /* the evaluator's own secondary-OAM address */
    uint8_t  oam2_seed;
    uint8_t  oam2_data;        /* byte the sprite fetch is reading from OAM2 */     /* corruption row: secondary-OAM address two dots after BLNK */
    uint8_t  oam_read;      /* the OAM data buffer the CPU sees at $2004 */

    /* ---- background pipeline (04-ppu-rendering.md sections 3-4) ----- */
    /* 16-bit pattern shift registers, bit 15 = the pixel being output.  The
     * attribute registers are also 16 bits and hold two bits per pixel
     * (lo = attribute bit 0, hi = attribute bit 1).  Both are reloaded for the
     * whole 16-pixel window whenever a tile's pattern word is loaded, which is
     * what the hardware's paired 8-bit registers produce. */
    uint16_t bg_sr_lo;   /* plane 0 (pattern colour bit 0) window */
    uint16_t bg_sr_hi;   /* plane 1 (pattern colour bit 1) window */
    uint16_t bg_at_lo;
    uint16_t bg_at_hi;
    uint8_t  nt_latch;      /* nametable byte being assembled */
    uint8_t  latch;
    uint8_t  addr_low;         /* low 8 address pins (data-bus sharing) */
    uint8_t  spr_latch;        /* sprite fetch's own address latch */
    uint8_t  spr_latch0;       /* OLD v low byte captured before dot 257's inc/copy */
    int      cpu_read_pending; /* dots until a $2007 read hits the bus */
    int      inc_pending;      /* dots until a $2007 read bumps v */
    int      cpu_read_active;  /* this dot performs that read */         /* octal latch driving the low address byte */
    uint8_t  at_latch;      /* attribute byte being assembled */
    uint8_t  at_quadrant;   /* quadrant selected when that byte was fetched */
    uint8_t  pat_lo;        /* pattern plane A staging latch */
    bool     bg_frozen;     /* both rendering bits were off: shifters are stale */
    uint8_t  fetch_phase;   /* 0..7 position inside the 8-dot tile period */

    /* ---- sprite evaluation (04-ppu-sprites.md sections 3-4) --------- */
    uint8_t  spr_count;         /* sprites copied into secondary OAM */
    uint8_t  spr_eval_n;        /* primary-OAM counter n (byte address) */
    uint8_t  spr_eval_b;        /* byte index inside the object being copied */
    uint8_t  spr_eval_m;        /* overflow-walk byte index m (0..3) */
    uint8_t  spr_eval_found;    /* sprites copied so far */
    uint8_t  spr_eval_state;    /* 0 search, 1 overflow walk, 2 done */
    bool     sprite0_on_line;   /* sprite at the evaluation start is on this line */
    uint8_t  eval_start;        /* OAMADDR sampled at the start of evaluation */
    /* Secondary-OAM address.  It walks the 32 secondary bytes during the
     * evaluation, is bumped twice per sprite while the lanes are loaded and
     * wraps at the end of the fetch window; once it wraps the "increment
     * frozen" flag blocks further bumps until rendering is enabled on dot 63,
     * 255 or 339 (04-ppu-sprites.md section 5.1). */
    uint8_t  oam_readbuf;       /* $2004 shadow of the internal OAM access */
    uint8_t  oam2_reg;          /* secondary-OAM address counter */
    bool     oam2_inc_frozen;   /* "increment frozen" latch for that counter */
    bool     spr_shift_hpos;    /* a lane X counter needs the odd-frame shift */

    /* ---- sprite pipeline (04-ppu-sprites.md section 5) -------------- */
    uint8_t  spr_lane_x[8];            /* X coordinate (the loaded value) */
    uint8_t  spr_lane_cnt[8];          /* X down-counter, armed at dot 339 */
    uint8_t  spr_lane_attr[8];
    uint8_t  spr_lane_pat[8][2];       /* {plane A, plane B}, hardware order */
    uint8_t  spr_lane_armed[8];
    uint8_t  spr_lane_done[8];
    uint8_t  spr_lane_shifted[8];      /* pixels emitted by this lane */
    uint8_t  spr_lane_flip[8];         /* horizontal-flip flag */
    uint8_t  spr_lane_is0[8];          /* this lane holds the evaluated "sprite 0" */

    /* ---- pixel output ---------------------------------------------- */
    uint8_t  pixel;         /* 5-bit palette index latched in the colour buffer */
    uint32_t framebuffer[NES_SCREEN_H][NES_SCREEN_W];

    /* ---- misc ------------------------------------------------------ */
    bool     rendering;     /* BLACK == 0 */
    bool     prev_rendering;/* the rendering bits at the previous PPU dot */
    bool     oam_corrupt;   /* an OAM row must be corrupted on this dot */
    int      corrupt_seed_delay; /* PPU dots until the corruption seed is sampled */
    bool     oam_corrupt_pending; /* an OAM row corrupts on the next rendered dot */
    bool     emph_r, emph_g, emph_b;
    bool     greyscale;
    uint64_t frame_dots;
} Ppu;

void ppu_init(Ppu *ppu, struct Nes *nes, struct Cart *cart);
void ppu_reset(Ppu *ppu);
void ppu_tick(Ppu *ppu);

/* Diagnostic snapshot: everything needed to reproduce the visible picture with
 * an independent renderer (used by --ppudump to compare against a reference
 * emulator).  chr covers the 8 KB pattern space, ciram the two 1 KB pages and
 * palette the 32 CRAM entries. */
typedef struct PpuDump {
    const uint32_t *framebuffer; /* 240 rows of 256 pixels, 0x00RRGGBB */
    const uint8_t  *palette;     /* 32 entries */
    const uint8_t  *oam;         /* 256 bytes */
    const uint8_t  *ciram;       /* 2048 bytes, page 0 then page 1 */
    struct Cart    *cart;        /* for ppu_chr_read */
    uint8_t  ctrl, mask, status, oam_addr;
    uint16_t v, t;
    uint8_t  fine_x;
    int      line, dot;
} PpuDump;

void ppu_dump_state(const Ppu *ppu, PpuDump *out);

/* CPU-side register access (address is the full $2000-$3FFF range). */
uint8_t ppu_read_register(Ppu *ppu, uint16_t addr);
void    ppu_write_register(Ppu *ppu, uint16_t addr, uint8_t value);

/* Force the OAM address to zero (dots 257-320 of every rendering scanline). */
void ppu_oam_addr_force_zero(Ppu *ppu);

/* VRAM access used by the cartridge. */
uint8_t ppu_vram_read(Ppu *ppu, uint16_t addr);
void    ppu_vram_write(Ppu *ppu, uint16_t addr, uint8_t value);
uint8_t ppu_chr_read(Ppu *ppu, uint16_t addr);
void    ppu_chr_write(Ppu *ppu, uint16_t addr, uint8_t value);

/* $4014 target: write one byte to OAMDATA. */
void ppu_oam_dma_write(Ppu *ppu, uint8_t value);

/* neslib addition: apply the queued PCLK-latency writes ($2001/$2006) at once.
 * The ported game code issues register writes from C, not from a cycle-timed
 * 6502, so the queues must be drained before the next access is interpreted;
 * see ../neslib.c. */
void ppu_flush_write_queues(Ppu *ppu);

/* Diagnostic: print the recent PPU register accesses to stderr. */
void ppu_dump_ring(void);

/* Region-derived NMI / RC timing, called when the region changes. */
void ppu_configure(Ppu *ppu, int region);

/* Exact frame length in dots for the current configuration. */
int  ppu_lines_per_frame(const Ppu *ppu);
bool ppu_vblank_line(const Ppu *ppu);

extern const uint32_t ppu_palette_rgb[64];

#endif /* PPU_H */
