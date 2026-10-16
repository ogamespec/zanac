/* ppu.c - RP2C02/RP2C07 picture processing unit (dot-accurate).
 *
 * Per-dot schedule, from Specs/04-ppu-rendering.md sections 2/7 and
 * Specs/04-ppu-scrolling.md section 4:
 *   dots 1-256   : 32 background tile periods, 8 dots each:
 *                    NT 1-2, AT 3-4, TA (plane A) 5-6, TB (plane B) 7-8
 *   dot 256      : one fine-Y increment (coarse Y only on a 7->0 wrap)
 *   dot 257      : the last coarse-X increment, then the horizontal t->v copy
 *   dots 257-320 : eight sprite fetch slots (8 dots each), OAMADDR forced to 0
 *   dots 321-336 : two-tile prefetch for the next line
 *   dots 337-340 : two dummy nametable fetches
 *   pre-render line: the vertical t->v copy runs on dots 280-304
 * Coarse X is incremented once per 8-dot tile period, i.e. at dots 8, 16, ...,
 * 256, plus the last one at 257 and two more at 329 and 337.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"

/* Decay time of one PPU open-bus data bit, in PPU dots (about twenty NTSC
 * frames).  See the comment on bus_decay in ppu.h. */
#define PPU_BUS_DECAY_DOTS (20u * 262u * 341u)

/* --------------------------------------------------------------- palette -- */
/* 2C02 RGB rendition.  04-ppu-palette.md section 3 fixes the twelve chroma
 * phases and four luma levels but states that "the exact level-per-entry
 * mapping is incomplete in ranks 1-2; emulators should use the canonical NES
 * palette", so the canonical 64-colour table is used.
 *
 * Canonical 64-colour table: index bits 5-4 select the luma row and bits 3-0
 * the chroma phase.  Chroma $D-$F are the black level in every row
 * (04-ppu-palette.md phenomenon 5), and $00/$10 are the two grey shades; the
 * 2C02 does not emit a luma level above the $2x/$3x row, so entries $20 and
 * $30 share the brightest white. */
const uint32_t ppu_palette_rgb[64] = {
    0x666666,0x002A88,0x1412A7,0x3B00A4,0x5C007E,0x6E0040,0x6C0600,0x561D00,
    0x333500,0x0B4800,0x005200,0x004F08,0x00404D,0x000000,0x000000,0x000000,
    0xADADAD,0x155FD9,0x4240FF,0x7527FE,0xA01ACC,0xB71E7B,0xB53120,0x994E00,
    0x6B6D00,0x388700,0x0C9300,0x008F32,0x007C8D,0x000000,0x000000,0x000000,
    0xFFFEFF,0x64B0FF,0x9290FF,0xC676FF,0xF36AFF,0xFE6ECC,0xFE8170,0xEA9E22,
    0xBCBE00,0x88D800,0x5CE430,0x45E082,0x48CDDE,0x4F4F4F,0x000000,0x000000,
    0xFFFEFF,0xC0DFFF,0xD3D2FF,0xE8C8FF,0xFBC2FF,0xFEC4EA,0xFECCC5,0xF7D8A5,
    0xE4E594,0xCFEF96,0xBDF4AB,0xB3F3CC,0xB5EBF2,0xB8B8B8,0x000000,0x000000
};

/* --------------------------------------------------------------- helpers -- */

static int ppu_lines(const Ppu *ppu)
{
    return ppu->nes->region == NES_REGION_NTSC ? PPU_LINES_NTSC : PPU_LINES_PAL;
}

int ppu_lines_per_frame(const Ppu *ppu) { return ppu_lines(ppu); }

void ppu_dump_state(const Ppu *ppu, PpuDump *out)
{
    out->framebuffer = (const uint32_t *)ppu->framebuffer;
    out->palette = ppu->pal;
    out->oam = ppu->oam;
    out->ciram = ppu->ciram;
    out->cart = ppu->nes->cart;
    out->ctrl = ppu->ctrl;
    out->mask = ppu->mask;
    out->status = ppu->status;
    out->oam_addr = ppu->oam_addr;
    out->v = ppu->v;
    out->t = ppu->t;
    out->fine_x = ppu->fine_x;
    out->line = ppu->line;
    out->dot = ppu->dot;
}

static bool ppu_region_pal(const Ppu *ppu)
{
    return ppu->nes->region != NES_REGION_NTSC;
}

static int ppu_prerender_line(const Ppu *ppu)
{
    return ppu->nes->region == NES_REGION_NTSC ? 261 : 311;
}

bool ppu_vblank_line(const Ppu *ppu)
{
    switch (ppu->nes->region) {
        case NES_REGION_DENDY: return ppu->line == 291;
        default:               return ppu->line == 241;
    }
}

/* PPU clocks between a $2001 write on the CPU bus and its effect on the PPU.
 * AccuracyCoin documents a 2..5 dot latency depending on the PPU revision and
 * the CPU/PPU clock alignment (its "BG Serial In" test slides a
 * disable/enable pair around the dot%8 == 7 pattern load and expects the
 * shifted-in '1's to survive). */
#ifndef PPU_MASK_WRITE_DELAY
#define PPU_MASK_WRITE_DELAY 3
#endif

/* Same latency for $2006 (see case 6). */
#ifndef PPU_ADDR_WRITE_DELAY
#define PPU_ADDR_WRITE_DELAY 4
#endif

/* Rendering is "active" for the purpose of $2007's combined increment only on
 * the visible scanlines (04-ppu-registers.md section 9). */
static bool ppu_rendering_active(const Ppu *ppu)
{
    return ppu->rendering && ppu->line < 240;
}

static uint16_t ciram_index(Ppu *ppu, uint16_t addr)
{
    uint16_t a = (uint16_t)((addr - 0x2000) & 0x0FFF);
    int bank;
    if (ppu->nes->cart && ppu->nes->cart->four_screen)
        return (uint16_t)(a & 0x0FFF);
    bank = cart_ciram_a10(ppu->nes->cart, addr);
    return (uint16_t)((bank << 10) | (a & 0x3FF));
}

/* ------------------------------------------------------- PPU open bus (DB) - */

/* Apply bits to the capacitive DB latch.  Every bit that is driven refreshes
 * its own decay counter; bits that are not driven keep both their value and
 * their remaining decay time (04-ppu-registers.md section 4). */
static void ppu_bus_apply(Ppu *ppu, uint8_t driven, uint8_t value)
{
    int i;
    ppu->ppu_bus = (uint8_t)((ppu->ppu_bus & (uint8_t)~driven) | (value & driven));
    for (i = 0; i < 8; i++) {
        if (driven & (1u << i))
            ppu->bus_decay[i] = 255;
    }
}

/* The value a floating read observes: set bits whose decay counter ran out are
 * gone.  Returns the decayed latch as well so callers can drive it back. */
static uint8_t ppu_bus_value(const Ppu *ppu)
{
    uint8_t v = 0;
    int i;
    for (i = 0; i < 8; i++) {
        if (ppu->bus_decay[i] && (ppu->ppu_bus & (1u << i)))
            v |= (uint8_t)(1u << i);
    }
    return v;
}

static void ppu_bus_tick(Ppu *ppu)
{
    int i;
    for (i = 0; i < 8; i++)
        if (ppu->bus_decay[i]) ppu->bus_decay[i]--;
}


/* ---------------------------------------------------- last-access ring --- */
/* A tiny ring of the most recent CPU accesses to the PPU registers, printed
 * when the frontend asks for it (NES_AUTODUMPPPU).  Used to see exactly what a
 * test ROM did without writing a multi-gigabyte bus log. */
#define PPU_RING 256
typedef struct PpuRingEnt {
    uint16_t addr; uint8_t val; uint8_t rd; uint16_t v; uint8_t buf; int line, dot;
} PpuRingEnt;
static PpuRingEnt ppu_ring[PPU_RING];
static int ppu_ring_pos;

static void ppu_ring_add(Ppu *ppu, uint16_t addr, uint8_t val, int rd)
{
    PpuRingEnt *e = &ppu_ring[ppu_ring_pos & (PPU_RING - 1)];
    e->addr = addr; e->val = val; e->rd = (uint8_t)rd;
    e->v = ppu->v; e->buf = ppu->read_buffer;
    e->line = ppu->line; e->dot = ppu->dot;
    ppu_ring_pos++;
}

void ppu_dump_ring(void)
{
    int i, n = ppu_ring_pos < PPU_RING ? ppu_ring_pos : PPU_RING;
    fprintf(stderr, "PPURING %d entries\n", n);
    for (i = 0; i < n; i++) {
        PpuRingEnt *e = &ppu_ring[(ppu_ring_pos - n + i) & (PPU_RING - 1)];
        fprintf(stderr, "  %s %04X %02X v=%04X buf=%02X [%d,%d]\n",
                e->rd ? "R" : "W", e->addr, e->val, e->v, e->buf, e->line, e->dot);
    }
}

/* ------------------------------------------------------------- VRAM access -- */uint8_t ppu_vram_read(Ppu *ppu, uint16_t addr)
{
    addr &= 0x3FFF;
    if (addr < 0x2000) {
        /* MMC3 counts scanlines from A12 rising edges, and the pattern table
         * fetches are the only thing that drives A12 (05-boards-and-io.md,
         * mapper register semantics are out of scope per the specification). */
        if (ppu->nes->cart)
            cart_ppu_a12(ppu->nes->cart, addr, ppu->rendering);
        return cart_chr_read(ppu->nes->cart, addr);
    }
    if (addr < 0x3F00) {
        /* $3000-$3EFF mirrors $2000-$2EFF: NESdev rank 4, but the spec's
         * programming notes instruct an emulator to provide the mirror
         * (04-ppu-memory-map.md section 1). */
        uint16_t a = (uint16_t)(addr & 0x2FFF);
        if (ppu->nes->cart && !cart_ciram_enabled(ppu->nes->cart))
            return 0x00;
        return ppu->ciram[ciram_index(ppu, a)];
    }
    {
        uint8_t idx = (uint8_t)(addr & 0x1F);
        /* $3F10/$3F14/$3F18/$3F1C alias $3F00/$3F04/$3F08/$3F0C: CRAM word rows
         * 0 and 4 are one cell (04-ppu-palette.md section 1.1). */
        if ((idx & 0x13) == 0x10)
            idx = (uint8_t)(idx & 0x0F);
        return ppu->pal[idx];
    }
}

void ppu_vram_write(Ppu *ppu, uint16_t addr, uint8_t value)
{
    addr &= 0x3FFF;
    if (addr < 0x2000) {
        cart_chr_write(ppu->nes->cart, addr, value);
        return;
    }
    if (addr < 0x3F00) {
        uint16_t a = (uint16_t)(addr & 0x2FFF);
        if (ppu->nes->cart && !cart_ciram_enabled(ppu->nes->cart))
            return;
        ppu->ciram[ciram_index(ppu, a)] = value;
        return;
    }
    {
        uint8_t idx = (uint8_t)(addr & 0x1F);
        if ((idx & 0x13) == 0x10)
            idx = (uint8_t)(idx & 0x0F);
        ppu->pal[idx] = (uint8_t)(value & 0x3F);
    }
}

uint8_t ppu_chr_read(Ppu *ppu, uint16_t addr)
{
    return cart_chr_read(ppu->nes->cart, (uint16_t)(addr & 0x1FFF));
}

void ppu_chr_write(Ppu *ppu, uint16_t addr, uint8_t value)
{
    cart_chr_write(ppu->nes->cart, (uint16_t)(addr & 0x1FFF), value);
}

/* ------------------------------------------------------------- init/reset -- */

void ppu_configure(Ppu *ppu, int region) { (void)region; }

void ppu_init(Ppu *ppu, struct Nes *nes, struct Cart *cart)
{
    memset(ppu, 0, sizeof(*ppu));
    ppu->nes = nes;
    ppu->cart = cart;
    /* OAM/CIRAM/palette contents at power-on are unspecified
     * (04-ppu-timing.md section 11).  $FF in OAM means "no sprites", which is
     * the harmless default for a program that never clears it. */
    memset(ppu->oam, 0xFF, sizeof(ppu->oam));
    memset(ppu->oam2, 0xFF, sizeof(ppu->oam2));
    ppu->line = ppu_prerender_line(ppu);
    ppu->dot = 0;
}

void ppu_reset(Ppu *ppu)
{
    ppu->ctrl = 0;
    ppu->mask = 0;
    ppu->t = 0;
    ppu->fine_x = 0;
    ppu->at_quadrant = 0;
    ppu->w = 0;
    ppu->read_buffer = 0;
    ppu->ppu_bus = 0;
    memset(ppu->bus_decay, 0, sizeof(ppu->bus_decay));
    ppu->line = ppu_prerender_line(ppu);
    ppu->dot = 0;
    ppu->nmi_out = false;
    ppu->greyscale = false;
    ppu->emph_r = ppu->emph_g = ppu->emph_b = false;
    ppu->rendering = false;
    ppu->rc_active = true;
    /* RC is released by RESCL; NESdev gives ~29658 NTSC / 33132 PAL CPU cycles
     * (04-ppu-registers.md section 11). */
    ppu->rc_until_cpu_cycle = ppu->nes->cpu_cycles +
        (ppu->nes->region == NES_REGION_NTSC ? 29658u : 33132u);
}

/* -------------------------------------------------------- register access -- */

static void ppu_write_oam(Ppu *ppu, uint8_t value)
{
    /* Writes while rendering do not reach OAM: the data is ignored and the
     * OAM address jumps by four instead of one, which the AccuracyCoin
     * "$2004 behavior" sub-tests 6, 7 and A pin down.  OAM is only really
     * written while BLNK is active, i.e. while the fetch schedule is
     * stopped (04-ppu-registers.md section 6). */
    if (ppu->rendering && (ppu->line < 240 || ppu->line == ppu_prerender_line(ppu))) {
        ppu->oam_addr = (uint8_t)((ppu->oam_addr + 4) & 0xFC);
        return;
    }
    /* Only the attribute byte (byte 2 of each sprite) has bits 4-2 missing
     * from the OAM array, so only it behaves as if ANDed with $E3; the Y,
     * tile and X bytes store all eight bits (04-ppu-sprites.md section 1,
     * AccuracyCoin "APU Register Activation" pre-requisite 6 writes a Y byte
     * and reads it back unchanged). */
    if ((ppu->oam_addr & 3) == 2)
        value = (uint8_t)(value & 0xE3);
    ppu->oam[ppu->oam_addr] = value;
    ppu->oam_addr++;
}

void ppu_oam_dma_write(Ppu *ppu, uint8_t value) { ppu_write_oam(ppu, value); }

/* neslib addition (see ppu.h): apply the PCLK-latency write queues at once.
 * The queued writes are applied in issue order, which preserves the effect of
 * the 6502 store sequence they replace. */
static void ppu_apply_mask(Ppu *ppu, uint8_t value);
static void ppu_apply_addr(Ppu *ppu, uint8_t value);

void ppu_flush_write_queues(Ppu *ppu)
{
    int q;
    for (q = 0; q < ppu->addr_qn; q++)
        ppu_apply_addr(ppu, ppu->addr_q[q]);
    ppu->addr_qn = 0;
    if (ppu->mask_pending_delay > 0) {
        ppu_apply_mask(ppu, ppu->mask_pending);
        ppu->mask_pending_delay = 0;
    }
}

void ppu_oam_addr_force_zero(Ppu *ppu) { ppu->oam_addr = 0x00; }

static void ppu_increment_v(Ppu *ppu)
{
    if (ppu->rendering && ppu->line < 240) {
        /* A $2007 access on a rendering scanline adds $1001 to v
         * (fine Y +1 and coarse X +1) regardless of $2000[2], with normal
         * wrapping (04-ppu-registers.md section 9; AccuracyCoin
         * "$2007 read w/ rendering" checks the exact value). */
        if ((ppu->v & 0x001F) == 31) { ppu->v &= (uint16_t)~0x001F; ppu->v ^= 0x0400; }
        else ppu->v++;
        if ((ppu->v & 0x7000) != 0x7000) {
            ppu->v = (uint16_t)(ppu->v + 0x1000);
        } else {
            uint16_t y;
            ppu->v &= (uint16_t)~0x7000;
            y = (uint16_t)((ppu->v & 0x03E0) >> 5);
            if (y == 29) { y = 0; ppu->v ^= 0x0800; }
            else if (y == 31) { y = 0; }
            else y++;
            ppu->v = (uint16_t)((ppu->v & ~0x03E0) | (y << 5));
        }
    } else {
        ppu->v = (uint16_t)(ppu->v + ((ppu->ctrl & 0x04) ? 32 : 1));
    }
}

uint8_t ppu_read_register(Ppu *ppu, uint16_t addr)
{
    uint8_t reg = (uint8_t)(addr & 7);
    switch (reg) {
    case 2: { /* PPUSTATUS */
        uint8_t v = (uint8_t)(ppu_bus_value(ppu) & 0x1F);
        v |= (uint8_t)(ppu->status & 0xE0);
        /* The vblank flag is latched at the start of the read, but the two
         * sprite flags are sampled about two PPU clocks later (when M2 goes
         * low), so a read that lands on the vblank-end clear reports the old
         * vblank bit together with the new sprite bits -- exactly the
         * observable the AccuracyCoin "$2002 flag timing" answer keys
         * describe. */
        {
            int pre = ppu_prerender_line(ppu);
            int l = ppu->line, d = ppu->dot, k;
            for (k = 0; k <= 2; k++) {
                if (l == pre && d == 1) {
                    v = (uint8_t)(v & ~0x60);
                    break;
                }
                if (++d >= 341) {
                    d = 0;
                    if (++l >= ppu_lines(ppu)) l = 0;
                }
            }
        }
        { uint8_t rv = v; ppu_ring_add(ppu, addr, rv, 1); }

        /* Reading clears the vblank latch and the shared w toggle; bits 6-5
         * are not cleared (04-ppu-registers.md section 4).  The read also
         * resets the internal vblank flip-flop with priority over the set, so
         * a read that lands on the /VSET dot suppresses the flag for the whole
         * frame (HDL/PPU/fsm.v VBlankInt: R2_EN resets int_ff and dominates
         * its set input; AccuracyCoin "VBlank beginning" tests this). */
        ppu->status &= (uint8_t)~0x80;
        ppu->int_ff = false;      /* R2_EN resets the /INT latch */
        ppu->int_delay = 0;
        ppu->vblank_read_suppress = true;
        ppu->w = 0;
        /* A $2002 read drives bits 7-5 onto the DB and leaves the low five bits
         * floating (04-ppu-registers.md section 4). */
        ppu_bus_apply(ppu, 0xE0, v);
        return v;
    }
    case 4: { /* OAMDATA */
        if (!ppu->nes->ppu_rev.oam_readable)
            return ppu_bus_value(ppu);
        {
            uint8_t v;
            /* The hardware samples $2004 at the *end* of the CPU read cycle,
             * so the access window is a dot ahead of where this read is
             * issued. */
            int od = ppu->dot + 1;
            if (od > 340) od -= 341;
            /* Reads while rendering expose the internal OAM access the sprite
             * logic is performing (04-ppu-registers.md section 6): the
             * secondary-OAM clear reads back $FF, the evaluation window
             * returns the evaluator's own data, and everything from the end of
             * the sprite fetch round to the start of the next line returns
             * secondary-OAM slot 0 ("$2004 Stress" keys $7F there). */
            if (ppu->rendering && ppu->line < 240) {
                if (od >= 65 && od <= 256) {
                    /* The evaluator is reading OAM at its own address, so the
                     * data bus carries that byte -- which is what $2004
                     * observes ("$2004 Stress" keys the address walk). */
                    v = ppu->oam[ppu->spr_eval_n];
                    ppu->oam_readbuf = v;
                }
                else if (od >= 257 && od <= 320) {
                    /* The sprite fetch drives the shared bus from secondary
                     * OAM, so $2004 reads back the byte the fetch has at its
                     * own secondary-OAM address ("Misaligned OAM2 Address"
                     * test 3 expects index 0 on dot 324). */
                    uint8_t off = (uint8_t)(od - 257);
                    uint8_t ph = (uint8_t)(off & 7);
                    uint8_t a;
                    if (ph >= 5)
                        a = (uint8_t)((off >> 3) * 4 + 3);
                    else
                        a = (uint8_t)((off >> 3) * 4 + ph);
                    if (ppu->oam2_inc_frozen)
                        a = ppu->oam2_reg;
                    v = ppu->oam2[a & 0x1F];
                }
                else if (od == 0 || od >= 321) {
                    /* After the sprite fetch the secondary-OAM address has
                     * wrapped back to slot 0, and that is what the data bus
                     * keeps presenting until the next line's clear. */
                    v = ppu->oam2[0];
                } else
                    v = 0xFF;   /* dots 1-64: secondary-OAM clear */
            } else {
                v = ppu->oam[ppu->oam_addr];
            }
            ppu_bus_apply(ppu, 0xFF, v);
            return v;
        }
    }
    case 7: { /* PPUDATA */
        uint16_t a = (uint16_t)(ppu->v & 0x3FFF);
        uint8_t result;
        if (ppu->rendering && ppu->line < 240)
            ppu->cpu_read_pending = 6;
        ppu_ring_add(ppu, addr, ppu->read_buffer, 1);
        if (a >= 0x3F00 && ppu->nes->ppu_rev.palette_readable) {
            /* Palette reads bypass the buffer, return the entry immediately and
             * carry PPU open bus in bits 7-6; greyscale is applied
             * (04-ppu-registers.md section 9). */
            /* The CRAM word rows 0 and 4 are one cell, so the read path needs
             * the same $3F10/$3F14/$3F18/$3F1C -> $3F00/$3F04/$3F08/$3F0C fold
             * the write path performs.  Without it a write to $3F00 is not
             * visible through its mirror ("Palette RAM Quirks" test 4). */
            uint8_t pidx = (uint8_t)(a & 0x1F);
            if ((pidx & 0x13) == 0x10)
                pidx = (uint8_t)(pidx & 0x0F);
            uint8_t pv = (uint8_t)(ppu->pal[pidx]);
            if (ppu->greyscale) pv = (uint8_t)(pv & 0x30);
            result = (uint8_t)((ppu_bus_value(ppu) & 0xC0) | pv);
            /* The buffer underneath is filled from the nametable mirror of the
             * palette address (04-ppu-registers.md section 9; AccuracyCoin
             * "PPU Read Buffer" test 7 writes $5A to $2F00 and reads it back
             * through the buffer after a $3F00 read). */
            ppu->read_buffer = ppu_vram_read(ppu, (uint16_t)(a & 0x2FFF));
        } else {
            result = ppu->read_buffer;
            ppu->read_buffer = ppu_vram_read(ppu, a);
        }
        ppu_bus_apply(ppu, 0xFF, result);
        /* During rendering the $2007 access is stretched by the background read
         * cadence: the PPU's own address latch is set up before the increment
         * of v lands, so the buffer is filled from the address v had *before*
         * the access (AccuracyCoin "$2007 Stress Test" keys the NT byte at the
         * pre-increment coarse X).  Outside rendering the bump is immediate. */
        if (ppu->rendering && ppu->line < 240)
            ppu->inc_pending = 6;
        else
            ppu_increment_v(ppu);
        return result;
    }
    default:
        /* $2000/$2001/$2003/$2005/$2006 have no read path (04-ppu-registers.md
         * phenomenon 1): return the DB open-bus value. */
        return ppu_bus_value(ppu);
    }
}

/* Apply a $2001 value once its write latency has elapsed (see case 1). */
static void ppu_apply_mask(Ppu *ppu, uint8_t value)
{
    /* When both rendering bits go to zero the background pipeline stops: the
     * shift registers keep their last contents, so a mid-line re-enable cannot
     * retroactively fetch the skipped tiles (04-ppu-rendering.md section 7;
     * AccuracyCoin "Rendering Flag Behavior" test 1). */
    if ((value & 0x18) == 0)
        ppu->bg_frozen = true;
    ppu->mask = value;
    ppu->greyscale = (value & 0x01) != 0;
    if (ppu_region_pal(ppu)) {
        ppu->emph_g = (value & 0x20) != 0;
        ppu->emph_r = (value & 0x40) != 0;
    } else {
        ppu->emph_r = (value & 0x20) != 0;
        ppu->emph_g = (value & 0x40) != 0;
    }
    ppu->emph_b = (value & 0x80) != 0;
    /* BLACK = ~(BGE | OBE): rendering counts as off only when both are 0. */
    ppu->rendering = (value & 0x18) != 0;
}




/* Apply a $2006 value once its write latency has elapsed (see case 6). */
static void ppu_apply_addr(Ppu *ppu, uint8_t value)
{
    if (!ppu->w) {
        ppu->t = (uint16_t)((ppu->t & 0x00FF) | ((uint16_t)(value & 0x3F) << 8));
        ppu->t = (uint16_t)(ppu->t & ~0x4000); /* fine Y bit 2 forced to 0 */
        ppu->w = 1;
    } else {
        ppu->t = (uint16_t)((ppu->t & 0xFF00) | value);
        ppu->v = ppu->t; /* the whole of t is copied into v immediately */
        ppu->w = 0;
    }
}

void ppu_write_register(Ppu *ppu, uint16_t addr, uint8_t value)
{
    uint8_t reg = (uint8_t)(addr & 7);
    /* PPU_TRACE=<n> logs the first n $2000/$2005/$2006 writes with the PPU's
     * line, dot and loopy-register state - the diagnostic that showed the port
     * and the original ROM set the scroll registers identically. */
    {   static int traced;
        const char *t = getenv("PPU_TRACE");
        if (t && (reg == 0 || reg == 5 || reg == 6 || reg == 7) && traced < atoi(t)) {
            traced++;
            fprintf(stderr, "PPUW f=%llu line=%3d dot=%3d reg=%d val=%02X  t=%04X v=%04X fine_x=%d\n",
                    (unsigned long long)ppu->nes->frame_count, ppu->line, ppu->dot, reg, value,
                    ppu->t, ppu->v, ppu->fine_x);
        }
    }
    ppu_bus_apply(ppu, 0xFF, value);
    if (reg == 3 || reg == 4 || reg == 5 || reg == 6 || reg == 7)
        ppu_ring_add(ppu, addr, value, 0);

    if (ppu->rc_active && ppu->nes->cpu_cycles < ppu->rc_until_cpu_cycle &&
        (reg == 0 || reg == 1 || reg == 5 || reg == 6))
        return; /* ignored-write window (04-ppu-registers.md section 11) */

    switch (reg) {
    case 0: /* PPUCTRL */
        ppu->ctrl = value;
        ppu->t = (uint16_t)((ppu->t & ~0x0C00) | ((uint16_t)(value & 3) << 10));
        break;
    case 1: /* PPUMASK */
        /* The CPU interface is not synchronised to PCLK: the value reaches the
         * PPU a few dots after the CPU cycle that wrote it, so queue it. */
        ppu->mask_pending = value;
        ppu->mask_pending_delay = PPU_MASK_WRITE_DELAY;
        break;
    case 3: /* OAMADDR */
        ppu->oam_addr = value;
        break;
    case 4: /* OAMDATA */
        ppu_write_oam(ppu, value);
        break;
    case 5: /* PPUSCROLL */
        if (!ppu->w) {
            ppu->fine_x = (uint8_t)(value & 0x07);
            ppu->t = (uint16_t)((ppu->t & ~0x001F) | (value >> 3));
            ppu->w = 1;
        } else {
            ppu->t = (uint16_t)((ppu->t & ~0x73E0) |
                                ((uint16_t)(value & 0x07) << 12) |
                                ((uint16_t)(value & 0xF8) << 2));
            ppu->w = 0;
        }
        break;
    case 6: /* PPUADDR */
        /* Same PCLK latency as $2001: the write reaches the scroll registers a
         * few dots after the CPU cycle that issued it, which is what lets a
         * well-timed $2006 slip between the address latch and the read of a
         * background fetch (AccuracyCoin "Hybrid Addresses"). */
        if (ppu->addr_qn < 4) {
            ppu->addr_q[ppu->addr_qn] = value;
            ppu->addr_q_delay[ppu->addr_qn] = PPU_ADDR_WRITE_DELAY;
            ppu->addr_qn++;
        } else {
            /* Queue full: never happens with sane code, apply immediately. */
            ppu_apply_addr(ppu, value);
        }
        break;
    case 7: /* PPUDATA */
        ppu_vram_write(ppu, (uint16_t)(ppu->v & 0x3FFF), value);
        ppu_increment_v(ppu);
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------- scrolling -- */

static void inc_coarse_x(Ppu *ppu)
{
    if ((ppu->v & 0x001F) == 31) {
        ppu->v &= (uint16_t)~0x001F;
        ppu->v ^= 0x0400;
    } else {
        ppu->v++;
    }
}

static void inc_y(Ppu *ppu)
{
    if ((ppu->v & 0x7000) != 0x7000) {
        ppu->v = (uint16_t)(ppu->v + 0x1000);
    } else {
        uint16_t y;
        ppu->v &= (uint16_t)~0x7000;
        y = (uint16_t)((ppu->v & 0x03E0) >> 5);
        if (y == 29) { y = 0; ppu->v ^= 0x0800; }
        else if (y == 31) { y = 0; }
        else y++;
        ppu->v = (uint16_t)((ppu->v & ~0x03E0) | (y << 5));
    }
}

static void copy_horizontal(Ppu *ppu)
{
    ppu->v = (uint16_t)((ppu->v & ~0x041F) | (ppu->t & 0x041F));
}

static void copy_vertical(Ppu *ppu)
{
    ppu->v = (uint16_t)((ppu->v & ~0x7BE0) | (ppu->t & 0x7BE0));
}

/* --------------------------------------------------------------- fetching -- */

static uint16_t nt_addr(Ppu *ppu) { return (uint16_t)(0x2000 | (ppu->v & 0x0FFF)); }

static uint16_t at_addr(Ppu *ppu)
{
    return (uint16_t)(0x23C0 | (ppu->v & 0x0C00) |
                      ((ppu->v >> 4) & 0x38) | ((ppu->v >> 2) & 0x07));
}

static uint16_t bg_pattern_addr(Ppu *ppu, uint8_t tile, int plane)
{
    uint16_t a = (uint16_t)(((ppu->ctrl & 0x10) ? 0x1000 : 0x0000) |
                      ((uint16_t)tile << 4) |
                      ((uint16_t)plane << 3) |
                      ((ppu->v >> 12) & 0x07));
    return a;
}

/* Attribute quadrant select: {coarse Y bit 1, coarse X bit 1}.
 *
 * The 2-bit palette field inside an attribute byte selects a 16x16-pixel
 * quadrant, i.e. a 2x2 group of tiles: bit 0 of the field comes from coarse-X
 * bit 1 and bit 1 from coarse-Y bit 1.  (coarse-X bit 2 is the low bit of the
 * *byte* index and is already folded into at_addr(); using it here selected the
 * neighbouring attribute block for every tile whose coarse X is 2 or 3 mod 4,
 * which is what drew the red 16-pixel stripes on TMNT Tournament Fighters'
 * skyline.) */
static uint8_t attr_quadrant(Ppu *ppu)
{
    return (uint8_t)((((ppu->v >> 6) & 1) << 1) | ((ppu->v >> 1) & 1));
}

/* Load the pattern word (and its attribute pair) into the 16-bit window.  The
 * staging latch holds plane A; plane B arrives one dot later
 * (04-ppu-rendering.md section 4).  Reversing both bytes and splitting the
 * 16-bit word places the first pixel of the tile in bit 15 exactly when the
 * tile becomes visible, which is what the hardware's paired registers do. */
/* The NES pattern bytes are stored least-significant-pixel-first while the
 * shifter presents the pixel for the current dot in bit 15; reversing a byte
 * makes its MSB the tile's leftmost pixel (04-ppu-rendering.md section 4). */
static uint8_t rev8(uint8_t b)
{
    uint8_t r = 0;
    int i;
    for (i = 0; i < 8; i++)
        r = (uint8_t)((r << 1) | ((b >> i) & 1));
    return r;
}

/* The PPU address bus is 14 bits wide but only six of them come from `v`:
 * the low byte is driven by the octal latch, which was loaded from the address
 * of the fetch phase on the previous dot.  The high bits are sampled when the
 * read actually happens, so a $2006 write landing between the two dots of a
 * fetch produces a "hybrid" address -- high bits from the new `v`, low byte
 * from the latch (AccuracyCoin "Hybrid Addresses", "ALE + Read"). */
static uint16_t bus_addr(Ppu *ppu, uint16_t base)
{
    uint16_t a = (uint16_t)((base & 0xFF00) | ppu->latch);
    return a;
}

/* Load the two pattern planes for the tile that becomes visible eight dots
 * later.
 *
 * Each plane is a 16-bit window whose bit 15 is the pixel being output: the
 * byte just fetched goes into the low half and travels up to bit 15 over the
 * following eight dots.  The high half still holds the previous tile, which
 * is what fine_x > 0 taps.  A byte fetched in one tile period therefore
 * appears exactly eight pixels later, and together with the two prefetched
 * tiles of the previous line that reproduces the hardware's 16-pixel fetch
 * pipeline (04-ppu-rendering.md section 4). */
static void bg_reload(Ppu *ppu, uint8_t pat_hi)
{
    /* The quadrant is sampled when the attribute byte is *fetched* (the name
     * fetch of the same tile period), not when the pattern planes land.  A
     * $2005/$2006 write or the coarse-X increment landing in between must not
     * change which palette the tile uses -- reading the live `v` here made the
     * background palette follow the scroll register instead of the tile
     * position. */
    uint8_t q = ppu->at_quadrant;
    uint8_t at = (uint8_t)((ppu->at_latch >> (q * 2)) & 3);

    ppu->bg_sr_lo = (uint16_t)((ppu->bg_sr_lo & 0xFF00u) | ppu->pat_lo);
    ppu->bg_sr_hi = (uint16_t)((ppu->bg_sr_hi & 0xFF00u) | pat_hi);
    /* The attribute shifters take the very same 16-bit window as the pattern
     * planes: the new quadrant goes into the low half and travels up to
     * bit 15 over the next eight dots, while the high half still holds the
     * previous tile's quadrant.  Writing the quadrant into *both* halves made
     * the attribute run only eight pixels and then flip back to the tile
     * before it, which is what drew the vertical palette stripes on the BTDD
     * logo and the SMB title panel. */
    ppu->bg_at_lo = (uint16_t)((ppu->bg_at_lo & 0xFF00u) | ((at & 1) ? 0x00FFu : 0u));
    ppu->bg_at_hi = (uint16_t)((ppu->bg_at_hi & 0xFF00u) | ((at & 2) ? 0x00FFu : 0u));
}

/* ------------------------------------------------------- sprite evaluation -- */

static int sprite_height(Ppu *ppu) { return (ppu->ctrl & 0x20) ? 16 : 8; }

/* In-range test for the scanline the evaluation is filling: row = line - Y. */
static bool sprite_in_range(Ppu *ppu, uint8_t y, int line)
{
    int h = sprite_height(ppu);
    /* Y <= V < Y + h, i.e. the row index must not wrap: a Y byte of $FF is
     * therefore hidden on the visible lines even though V - Y wraps to a small
     * number (04-ppu-sprites.md section 3).  OAM full of $FF (which is what a
     * cleared page 2 looks like) must not look like 64 in-range sprites. */
    return y <= line && (uint8_t)(line - y) < (unsigned)h;
}

/* One step of the sprite evaluation algorithm of 04-ppu-sprites.md section 3,
 * on the doubled-step cadence the AccuracyCoin "$2004 Stress Test" answer key
 * documents (one byte per step).  The address rules are the ones the
 * misaligned-OAM tests in that same suite pin down:
 *
 *   Y out of range, secondary OAM not full : OAMADDR = (OAMADDR + 4) & $FC
 *   X byte out of range, not full          : OAMADDR = (OAMADDR + 1) & $FC
 *   Y (or X) in range, not full            : copy the byte, OAMADDR += 1
 *   Y in range, secondary OAM full         : sprite overflow, OAMADDR += 5
 *   Y out of range, secondary OAM full     : OAMADDR += 5
 */
static void sprite_eval_step(Ppu *ppu)
{
    /* The comparators work on the low eight bits of the line counter: on the
     * pre-render line that is 5, not 261, so a Y byte of $FF is out of range
     * there just as it is on any visible line. */
    int line = ppu->line & 0xFF;
    int h = sprite_height(ppu);
    uint8_t byte;

    if (ppu->spr_eval_state == 2)
        return;

    if (ppu->spr_eval_found >= 8) {
        /* The overflow walk.  On a miss the sprite index n and the byte index
         * m both advance, so the byte compared is OAM[4n+m] and the OAM address
         * steps by five; the walk terminates when the sprite index wraps past
         * sprite 63, which is why the low part of OAM is never re-read and why
         * eight sprites do not raise the flag (04-ppu-sprites.md section 4). */
        int wn = (ppu->spr_eval_n >> 2) & 63;
        int wm = ppu->spr_eval_n & 3;
        byte = ppu->oam[(wn << 2) | wm];
        if (byte <= line && (uint8_t)(line - byte) < (unsigned)h) {
            ppu->status |= 0x20;
            ppu->spr_eval_state = 2;
            return;
        }
        wn = (wn + 1) & 63;
        wm = (wm + 1) & 3;
        if (wn == 0) {
            ppu->spr_eval_state = 2;
            return;
        }
        ppu->spr_eval_n = (uint8_t)((wn << 2) | wm);
        ppu->spr_eval_m = (uint8_t)wm;
        return;
    }

    byte = ppu->oam[ppu->spr_eval_n];

    /* The in-range test belongs to the byte that is *read as* the Y
     * coordinate: the first byte of the object.  Every other byte of the
     * object (tile, attributes, X) is copied unconditionally, so a sprite
     * whose Y matches the line is never discarded because of its tile number
     * ("Misaligned OAM behavior", 04-ppu-sprites.md section 3). */
    if (ppu->spr_eval_b == 0 &&
        !(byte <= line && (uint8_t)(line - byte) < (unsigned)h)) {
        /* Not in range: skip the whole object, stepping the address by four
         * and re-aligning it to a four-byte boundary. */
        ppu->spr_eval_n = (uint8_t)((ppu->spr_eval_n + 4) & 0xFC);
        if (ppu->spr_eval_n == (ppu->eval_start & 0xFC)) {
            ppu->spr_eval_state = 2;
            ppu->spr_count = ppu->spr_eval_found;
        }
        return;
    }

    if (ppu->spr_eval_b == 0 && ppu->spr_eval_n == ppu->eval_start)
        ppu->sprite0_on_line = true;

    ppu->oam2[ppu->spr_eval_found * 4 + ppu->spr_eval_b] = byte;
    /* Every byte that lands in secondary OAM bumps its address; a bump past
     * the last slot wraps and latches the "increment frozen" flag, which is
     * what makes a later sprite fetch read slot 0 over and over
     * (AccuracyCoin "Frozen OAM2 Increment"). */
    if (!ppu->oam2_inc_frozen) {
        if (ppu->oam2_reg >= 0x1F) {
            ppu->oam2_reg = 0;
            ppu->oam2_inc_frozen = true;
        } else {
            ppu->oam2_reg++;
        }
    }

    if (ppu->spr_eval_b == 3) {
        /* The X byte goes through the very same in-range comparator.  A failure
         * does not discard the object -- all four bytes have already been
         * copied -- it only changes the address step to "+1 and re-align",
         * which makes the next object overlap this one ("misaligned OAM").
         * For an object that ends on a four-byte boundary the re-alignment is
         * a no-op, which is why the quirk is invisible with aligned OAM
         * (AccuracyCoin "Arbitrary Sprite Zero" and "Misaligned OAM
         * behavior"). */
        int x_out_of_range =
            !(byte <= line && (uint8_t)(line - byte) < (unsigned)h);
        ppu->spr_eval_found++;
        ppu->spr_count = ppu->spr_eval_found;
        ppu->spr_eval_b = 0;
        ppu->spr_eval_n = (uint8_t)(ppu->spr_eval_n + 1);
        if (x_out_of_range)
            ppu->spr_eval_n = (uint8_t)(ppu->spr_eval_n & 0xFC);
        if (ppu->spr_eval_n == ppu->eval_start ||
            ppu->spr_eval_n == (ppu->eval_start & 0xFC)) {
            /* The evaluator walked all 64 objects. */
            ppu->spr_eval_state = 2;
            ppu->spr_count = ppu->spr_eval_found;
        }
        return;
    }

    ppu->spr_eval_b++;
    ppu->spr_eval_n = (uint8_t)(ppu->spr_eval_n + 1);
}

/* ------------------------------------------------------------ sprite fetch -- */

static uint16_t spr_pattern_base(Ppu *ppu, uint8_t tile, int row)
{
    uint16_t base;
    if (sprite_height(ppu) == 16) {
        uint8_t t = (uint8_t)(tile & 0xFE);
        base = (uint16_t)(((tile & 0x01) ? 0x1000 : 0x0000) + ((uint16_t)t << 4));
        if (row >= 8) base = (uint16_t)(base + 0x10);
    } else {
        base = (uint16_t)(((ppu->ctrl & 0x08) ? 0x1000 : 0x0000) +
                          ((uint16_t)tile << 4));
    }
    return base;
}

static void sprite_fetch_dot(Ppu *ppu)
{
    int off = ppu->dot - 257;
    int lane = off >> 3;
    int ph = off & 7;
    int h = sprite_height(ppu);
    int row;
    uint8_t sy, tile, attr, sx;

    if (lane < 0 || lane > 7)
        return;
    sy   = ppu->oam2[lane * 4 + 0];
    tile = ppu->oam2[lane * 4 + 1];
    attr = ppu->oam2[lane * 4 + 2];
    sx   = ppu->oam2[lane * 4 + 3];

    /* The row is computed from the 8-bit scanline, so the fetch also works on
     * the pre-render line (04-ppu-sprites.md section 5.1). */
    row = (uint8_t)((ppu->line & 0xFF) - sy);
    if (attr & 0x80) row = h - 1 - row;
    if (row < 0) row = 0;

    /* The sprite slots run their own two-dot read cadence (address latch on
     * the even dots, read on the odd ones) with a *separate* latch so the
     * background's shared-bus state is left alone.  The read cadence drives
     * the shared bus for EVERY lane, empty or not: garbage nametable fetches
     * on dots 1/3, and the OAM2 bytes the fetch takes out of secondary OAM on
     * dots 5/7 ("$2007 Stress Test" second loop samples both). */
    if (ph == 0 || ph == 2) {
        /* The sprite slots run the background's cadence one slot at a time:
         * both address latches are nametable addresses (the sprite fetch has no
         * attribute phase -- "$2007 Stress Test" keys the second read as the
         * nametable byte at the same place the background reads it). */
        ppu->spr_latch = (uint8_t)(nt_addr(ppu) & 0xFF);
        if (ph == 0 && off == 0)
            ppu->spr_latch = ppu->spr_latch0; /* OLD v (pre dot-257 inc/copy) */
    } else if (ph == 1 || ph == 3) {
        uint8_t nt = ppu_vram_read(ppu,
                                   (uint16_t)((nt_addr(ppu) & 0xFF00) |
                                              ppu->spr_latch));
        if (ppu->cpu_read_active)
            ppu->read_buffer = nt;
    } else if (ph == 5 || ph == 7) {
        /* Dots 262/264 are pattern-table reads (04-ppu-rendering.md), so the
         * buffer takes the pattern byte for a lane that has a sprite.  An
         * empty lane leaves the secondary-OAM byte on the bus instead. */
        bool empty = (lane >= ppu->spr_count ||
                      !sprite_in_range(ppu, sy, ppu->line & 0xFF));
        if (ppu->cpu_read_active) {
            if (empty)
                ppu->read_buffer = ppu->oam2[lane * 4 + ((ph == 5) ? 1 : 2)];
            else
                ppu->read_buffer = ppu_vram_read(ppu,
                    (uint16_t)(spr_pattern_base(ppu, tile, row) +
                               (row & 7) + (ph == 7 ? 8 : 0)));
        }
    }

    if (ph == 4 || ph == 6) {
        /* Pattern address register latch (04-ppu-sprites.md section 5.1). */
        ppu->spr_latch = (uint8_t)((spr_pattern_base(ppu, tile, row) + (row & 7)) & 0xFF);
    }

    if (lane >= ppu->spr_count || !sprite_in_range(ppu, sy, ppu->line & 0xFF)) {
        /* Empty lane: the pattern bytes are forced to $00
         * (04-ppu-sprites.md section 5.1).  The PPU nevertheless *performs*
         * the pattern fetch for every one of the eight slots, and that fetch
         * is what drives PPU A12 during dots 257-319.  Skipping it makes A12
         * stop toggling on scanlines with no sprites, which starves an MMC3
         * scanline counter (TMNT Tournament Fighters' IRQ split then never
         * fires), so the read is still issued and its data discarded. */
        if (ph == 4 || ph == 6) {
            uint8_t v = ppu_vram_read(ppu,
                (uint16_t)(spr_pattern_base(ppu, tile, row) +
                           (row & 7) + (ph == 6 ? 8 : 0)));
            /* Every read dot drives the shared bus, so a $2007 state-machine
             * read landing here takes the pattern byte even though the lane
             * itself is empty ("$2007 Stress Test" samples this). */
            if (ppu->cpu_read_active)
                ppu->read_buffer = v;
        } else if (ph == 0) {
            ppu->spr_lane_pat[lane][0] = 0;
            ppu->spr_lane_pat[lane][1] = 0;
            ppu->spr_lane_armed[lane] = 0;
        } else if (ph == 2) {
            ppu->spr_lane_attr[lane] = 0;
        } else if (ph == 3) {
            ppu->spr_lane_x[lane] = 0xFF;
        }
        return;
    }

    if (ph == 2) {
        ppu->spr_lane_attr[lane] = attr;
        ppu->spr_lane_flip[lane] = (uint8_t)((attr >> 6) & 1);
    } else if (ph == 3) {
        ppu->spr_lane_x[lane] = sx;
    } else if (ph == 4) {
        uint8_t v = ppu_vram_read(ppu, (uint16_t)(spr_pattern_base(ppu, tile, row) + (row & 7)));
        /* H flip mirrors the tile: reverse both planes as they are loaded and
         * let the shifter always walk towards bit 7 (04-ppu-sprites.md
         * section 5.2). */
        if (ppu->cpu_read_active) ppu->read_buffer = v;
        if (ppu->spr_lane_flip[lane]) v = rev8(v);
        ppu->spr_lane_pat[lane][0] = v;
    } else if (ph == 6) {
        uint8_t v = ppu_vram_read(ppu, (uint16_t)(spr_pattern_base(ppu, tile, row) + (row & 7) + 8));
        if (ppu->cpu_read_active) ppu->read_buffer = v;
        if (ppu->spr_lane_flip[lane]) v = rev8(v);
        ppu->spr_lane_pat[lane][1] = v;
        ppu->spr_lane_armed[lane] = 1;
        ppu->spr_lane_done[lane] = 0;
        ppu->spr_lane_shifted[lane] = 0;
        /* Lane 0 holds the sprite the evaluator looked at first, which is the
         * one the sprite-0 hit latch is gated on ("Arbitrary Sprite Zero"). */
        ppu->spr_lane_is0[lane] = (uint8_t)((lane == 0 && ppu->sprite0_on_line) ? 1 : 0);
    }

    /* The secondary-OAM address is bumped twice per lane while the lane is
     * loaded.  It wraps at slot 31 and latches the frozen flag, and while the
     * flag is set the bumps stop entirely -- which is what lets a precisely
     * timed rendering toggle keep the fetch reading one slot, and what makes
     * the address at the end of the window depend on how many dots rendering
     * was actually enabled for (AccuracyCoin "Frozen OAM2 Increment" and
     * "Misaligned OAM2 Address" test 3: 18 missing dots -> the address only
     * reaches $18 instead of wrapping, so $2004 reads $06 rather than $5A). */
    if (ph == 2 || ph == 6) {
        if (!ppu->oam2_inc_frozen) {
            if (ppu->oam2_reg >= 0x1F) {
                ppu->oam2_reg = 0;
                ppu->oam2_inc_frozen = true;
            } else {
                ppu->oam2_reg++;
            }
        }
    }
}

/* ------------------------------------------------------------- pixel output -- */

static void render_pixel(Ppu *ppu, int x)
{
    uint8_t bg = 0, bg_pal = 0;
    uint8_t spr = 0, spr_pal = 0, spr_prio = 0;
    uint8_t spr0_bits = 0;
    bool spr0_opaque = false;
    uint8_t lane_emit[8];
    int lane;
    uint8_t index;
    bool bg_opaque, spr_opaque;
    bool bg_enabled = (ppu->mask & 0x08) != 0;
    bool spr_enabled = (ppu->mask & 0x10) != 0;

    if (bg_enabled) {
        uint8_t tap = (uint8_t)(15 - ppu->fine_x);
        bg = (uint8_t)(((ppu->bg_sr_lo >> tap) & 1) |
                       (((ppu->bg_sr_hi >> tap) & 1) << 1));
        bg_pal = (uint8_t)(((ppu->bg_at_lo >> tap) & 1) |
                           (((ppu->bg_at_hi >> tap) & 1) << 1));
    }

    /* Sprite priority: the lowest-numbered lane with an opaque pixel wins, and
     * sprite-vs-sprite priority is resolved before the background comparison
     * (04-ppu-sprites.md section 5.3).  Each lane emits eight pixels, one per
     * dot, starting at its X coordinate; the X down-counter is decremented on
     * every visible dot regardless of the rendering bits, so blanking a few
     * dots does not shift a sprite (AccuracyCoin "Stale Sprite Shift Regs"). */
    for (lane = 0; lane < 8; lane++) {
        uint8_t bits = 0;
        lane_emit[lane] = 0;
        if (!ppu->spr_lane_armed[lane])
            continue;
        if (ppu->spr_lane_cnt[lane] != 0) {
            /* The X down-counter is clocked on every visible dot regardless of
             * the rendering bits, so blanking a few dots does not shift a
             * sprite.  The lane emits once the counter has counted all the way
             * down to zero, i.e. its first pixel lands on dot X
             * (04-ppu-sprites.md section 5.2). */
            ppu->spr_lane_cnt[lane]--;
            continue;
        }
        lane_emit[lane] = 1;
        if (spr_enabled && !ppu->spr_lane_done[lane]) {
            uint8_t lo = ppu->spr_lane_pat[lane][0];
            uint8_t hi = ppu->spr_lane_pat[lane][1];
            bits = (uint8_t)(((lo >> 7) & 1) | (((hi >> 7) & 1) << 1));
            if (lane == 0) {
                spr0_bits = bits;
                spr0_opaque = bits != 0 && ppu->spr_lane_is0[lane];
            }
            if (bits != 0 && spr == 0) {
                spr = bits;
                spr_pal = (uint8_t)(ppu->spr_lane_attr[lane] & 3);
                spr_prio = (uint8_t)((ppu->spr_lane_attr[lane] >> 5) & 1);
            }
        }
    }

    bg_opaque = (bg & 3) != 0;
    spr_opaque = (spr & 3) != 0;


    /* Left-column clipping (04-ppu-rendering.md section 6).  The clipper gates
     * each layer separately before the sprite-0 comparison, so a sprite masked
     * out of the left column cannot strike the background there. */
    if (x < 8) {
        if (!(ppu->mask & 0x02)) bg_opaque = false;
        if (!(ppu->mask & 0x04)) {
            spr_opaque = false;
            spr0_opaque = false;
        }
    }

    /* Sprite 0 hit: both PPUMASK layers enabled, the evaluated sprite 0 emits
     * an opaque pixel (it need not be the winning sprite), the background pixel
     * is opaque, the dot is inside /VIS, and the dot is not x = 255
     * (04-ppu-sprites.md section 7). */
    if (!(ppu->status & 0x40) && x < 255 && bg_enabled && spr_enabled &&
        spr0_opaque && (spr0_bits & 3) != 0) {
        bool bg_for_hit = (bg & 3) != 0;
        if (x < 8 && !(ppu->mask & 0x02)) bg_for_hit = false;
        if (bg_for_hit) {
            ppu->status |= 0x40;
        }
    }

    if (spr_opaque && bg_opaque) {
        if (spr_prio) index = (uint8_t)(bg_pal << 2 | bg);
        else          index = (uint8_t)(0x10 | (spr_pal << 2) | spr);
    } else if (spr_opaque) {
        index = (uint8_t)(0x10 | (spr_pal << 2) | spr);
    } else if (bg_opaque) {
        index = (uint8_t)(bg_pal << 2 | bg);
    } else {
        index = 0x00;
    }

    /* Direct colour (TH/MUX): with blanking active and v inside the palette
     * range the palette index comes straight from v
     * (04-ppu-registers.md section 9). */
    if (!bg_enabled && !spr_enabled && (ppu->v & 0x3F00) == 0x3F00)
        index = (uint8_t)(ppu->v & 0x1F);
    else if (!bg_enabled && !spr_enabled)
        index = 0x00;

    /* Clock the sprite shifters once per visible dot (the pixel just read was
     * bit 7 of both planes). */
    for (lane = 0; lane < 8; lane++) {
        /* Forced blanking stops the sprite shifters (but not the X counters,
         * which keep counting above): a lane that was mid-pixel when rendering
         * stopped resumes from the same pixel when it comes back. */
        if (!ppu->rendering)
            break;
        if (!lane_emit[lane] || ppu->spr_lane_done[lane])
            continue;
        {
            uint8_t lo = ppu->spr_lane_pat[lane][0];
            uint8_t hi = ppu->spr_lane_pat[lane][1];
            ppu->spr_lane_pat[lane][0] = (uint8_t)(lo << 1);
            ppu->spr_lane_pat[lane][1] = (uint8_t)(hi << 1);
        }
        ppu->spr_lane_shifted[lane]++;
        if (ppu->spr_lane_shifted[lane] >= 8)
            ppu->spr_lane_done[lane] = 1;
    }

    {
        uint32_t colour = ppu_palette_rgb[ppu->pal[index] & 0x3F];
        if (ppu->emph_r || ppu->emph_g || ppu->emph_b) {
            uint32_t r = (colour >> 16) & 0xFF, g = (colour >> 8) & 0xFF, b = colour & 0xFF;
            if (ppu->emph_r) r = (r * 3) / 4;
            if (ppu->emph_g) g = (g * 3) / 4;
            if (ppu->emph_b) b = (b * 3) / 4;
            colour = (r << 16) | (g << 8) | b;
        }
        ppu->pixel = index;
        ppu->framebuffer[ppu->line][x] = colour;

    }
}

/* -------------------------------------------------------------- the tick --- */

void ppu_tick(Ppu *ppu)
{
    int h = ppu_lines(ppu);
    int prerender = ppu_prerender_line(ppu);
    int dot = ppu->dot;
    int line = ppu->line;
    bool visible_line = (line < 240);
    bool renderable = ppu->rendering && (visible_line || line == prerender);

    if (ppu->mask_pending_delay > 0 && --ppu->mask_pending_delay == 0)
        ppu_apply_mask(ppu, ppu->mask_pending);
    ppu->cpu_read_active = 0;
    if (ppu->cpu_read_pending > 0 && --ppu->cpu_read_pending == 0) {
        ppu->cpu_read_active = 1;
    }
    if (ppu->inc_pending > 0 && --ppu->inc_pending == 0)
        ppu_increment_v(ppu);
    if (ppu->addr_qn > 0) {
        int q, n = 0;
        for (q = 0; q < ppu->addr_qn; q++)
            if (--ppu->addr_q_delay[q] <= 0)
                ppu_apply_addr(ppu, ppu->addr_q[q]);
            else {
                ppu->addr_q[n] = ppu->addr_q[q];
                ppu->addr_q_delay[n] = ppu->addr_q_delay[q];
                n++;
            }
        ppu->addr_qn = n;
    }

    ppu_bus_tick(ppu);

    /* OAM corruption (04-ppu-sprites.md section 1, NESdev PPU_sprite_evaluation
     * errata): when BLNK becomes active the PPU latches its secondary-OAM
     * address; the first PPU dot on the pre-render line through line 239 on
     * which rendering becomes active again then copies OAM row 0 over the row
     * that address points at.  Enabling rendering during vblank has no effect,
     * which AccuracyCoin "OAM Corruption" sub-test 4 checks. */
    if (!ppu->rendering && ppu->prev_rendering) {
        ppu->corrupt_seed_delay = 3;
        ppu->oam_corrupt_pending = true;
    }
    ppu->prev_rendering = ppu->rendering;
    /* The corruption happens on the first rendered dot of the pre-render line
     * through line 239, not on the dot that re-enabled rendering (which is
     * usually well inside vblank) -- AccuracyCoin "OAM Corruption" tests 3/4
     * check exactly that ordering. */
    if (ppu->oam_corrupt_pending && ppu->rendering &&
        (line == prerender || visible_line)) {
        int i;
        int row = ppu->oam2_seed;
        for (i = 0; i < 8; i++)
            ppu->oam[(row * 8 + i) & 0xFF] = ppu->oam[i];
        ppu->oam2[8] = ppu->oam2[0];
        ppu->oam_corrupt_pending = false;
    }

    /* $2002[7] is set by /VSET and cleared by RESCL
     * (04-ppu-timing.md section 7). */
    if (ppu_vblank_line(ppu) && dot == 1 &&
        !(ppu->vblank_read_suppress && ppu->vblank_set_in_cycle)) {
        ppu->status |= 0x80;
        /* HDL/PPU/fsm.v VBlankInt: the /INT latch follows the vblank flag
         * through the vset_latch1/vset_latch2 chain, two PPU clocks later
         * (the counter is decremented again on this same dot).  A $2002 read
         * in this very CPU cycle resets the latch with priority over the set,
         * so neither the flag nor the NMI appears (AccuracyCoin "VBlank
         * beginning", "NMI Suppression"). */
        ppu->int_delay = 2;
    }
    if (ppu->int_delay > 0) {
        ppu->int_delay--;
        if (ppu->int_delay == 0)
            ppu->int_ff = true;
    }
    if (line == prerender && dot == 1) {
        ppu->status &= (uint8_t)~(0x80 | 0x40 | 0x20);
        ppu->int_ff = false;
        ppu->int_delay = 0;
        ppu->sprite0_on_line = false;
        ppu->rc_active = false;
    }

    /* NTSC odd-frame dot skip: the pre-render line is 340 dots when rendering
     * is enabled and this is an odd frame (04-ppu-timing.md section 8). */
    if (!ppu_region_pal(ppu) && line == prerender && dot == 0)
        ppu->odd_frame_skip = ppu->frame_odd && ppu->rendering;

    /* The background shifter is clocked only while rendering is active, so
     * while BLNK is asserted it keeps its last contents; re-enabling rendering
     * mid-line therefore draws those stale pixels instead of the tiles that
     * were skipped (04-ppu-rendering.md section 7; AccuracyCoin "Rendering
     * Flag Behavior", "Stale BG Shift Registers", "BG Serial In").  No reset is
     * applied on the next line: the hardware simply starts clocking again. */
    if (renderable)
        ppu->bg_frozen = false;

    /* Dot 339 arms the sprite X down-counters from each lane's loaded X value
     * (04-ppu-sprites.md section 5.2).  If rendering is off on that dot they
     * keep running from where they were, which is what makes a stale sprite
     * shifter draw immediately when rendering comes back ("Stale Sprite Shift
     * Regs" checks 5 and 6). */
    if (dot == 339 && (visible_line || line == prerender)) {
        int i;
        /* Dot 339/340 clears every lane's X down-counter and arms it from the X
         * byte.  When rendering is disabled on that dot the load does not
         * happen, so the counters stay at zero and every sprite behaves as if
         * its X were 0 when rendering comes back ("Stale BG Shift Registers"
         * check 4).  The odd-frame dot skip lands *after* this reload and
         * leaves the counters one dot ahead ("Sprites On Scanline 0"). */
        for (i = 0; i < 8; i++)
            ppu->spr_lane_cnt[i] = ppu->rendering ? ppu->spr_lane_x[i] : 0;
        ppu->spr_shift_hpos = ppu->odd_frame_skip;
    }

    if (visible_line && dot < 256) {
        render_pixel(ppu, dot);
    }

    if (renderable) {
        /* The first sprite slot's garbage nametable fetch (dot 257 latch /
         * dot 258 read) uses the OLD value of v from *before* the dot-257
         * coarse-X increment and horizontal t->v copy ("$2007 Stress Test"
         * "COMMON MISTAKES": "should be reading from the OLD value of v before
         * the v register is horizontally reset").  Capture it here, before any
         * of dot 257's v mutations run. */
        if (dot == 257)
            ppu->spr_latch0 = (uint8_t)(nt_addr(ppu) & 0xFF);

        /* ---- background fetch schedule ------------------------------- */

        if (dot >= 1 && dot <= 256) {
            int ph = (dot - 1) & 7;
            if (ph == 0) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                                                        : (uint8_t)(nt_addr(ppu) & 0xFF);
            } else if (ph == 1) {
                ppu->nt_latch = ppu_vram_read(ppu, bus_addr(ppu, nt_addr(ppu)));
                ppu->addr_low = ppu->nt_latch;
                if (ppu->cpu_read_active) ppu->read_buffer = ppu->nt_latch;
            } else if (ph == 2) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                                                        : (uint8_t)(at_addr(ppu) & 0xFF);
            } else if (ph == 3) {
                ppu->at_latch = ppu_vram_read(ppu, bus_addr(ppu, at_addr(ppu)));
                ppu->at_quadrant = attr_quadrant(ppu);
                ppu->addr_low = ppu->at_latch;
                if (ppu->cpu_read_active) ppu->read_buffer = ppu->at_latch;
            } else if (ph == 4) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                            : (uint8_t)(bg_pattern_addr(ppu, ppu->nt_latch, 0) & 0xFF);
            } else if (ph == 5) {
                ppu->pat_lo = ppu_vram_read(ppu, bus_addr(ppu, bg_pattern_addr(ppu, ppu->nt_latch, 0)));
                ppu->addr_low = ppu->pat_lo;
                if (ppu->cpu_read_active) ppu->read_buffer = ppu->pat_lo;
            } else if (ph == 6) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                            : (uint8_t)(bg_pattern_addr(ppu, ppu->nt_latch, 1) & 0xFF);
            } else {
                uint8_t hi = ppu_vram_read(ppu, bus_addr(ppu, bg_pattern_addr(ppu, ppu->nt_latch, 1)));
                ppu->addr_low = hi;
                if (ppu->cpu_read_active) ppu->read_buffer = hi;
                bg_reload(ppu, hi);
            }
        } else if (dot >= 322 && dot <= 337) {
            /* Two-tile prefetch of the next line.  The seven-dot offset from
             * the visible schedule is what puts the first prefetched tile at
             * x = 0 after the fifteen HBlank shift dots and the second at
             * x = 8 (04-ppu-rendering.md section 2). */
            int ph = (dot - 322) & 7;
            if (ph == 0) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                                                        : (uint8_t)(nt_addr(ppu) & 0xFF);
            } else if (ph == 1) {
                ppu->nt_latch = ppu_vram_read(ppu, bus_addr(ppu, nt_addr(ppu)));
                ppu->addr_low = ppu->nt_latch;
                if (ppu->cpu_read_active) ppu->read_buffer = ppu->nt_latch;
            } else if (ph == 2) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                                                        : (uint8_t)(at_addr(ppu) & 0xFF);
            } else if (ph == 3) {
                ppu->at_latch = ppu_vram_read(ppu, bus_addr(ppu, at_addr(ppu)));
                ppu->at_quadrant = attr_quadrant(ppu);
                ppu->addr_low = ppu->at_latch;
                if (ppu->cpu_read_active) ppu->read_buffer = ppu->at_latch;
            } else if (ph == 4) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                            : (uint8_t)(bg_pattern_addr(ppu, ppu->nt_latch, 0) & 0xFF);
            } else if (ph == 5) {
                ppu->pat_lo = ppu_vram_read(ppu, bus_addr(ppu, bg_pattern_addr(ppu, ppu->nt_latch, 0)));
                ppu->addr_low = ppu->pat_lo;
                if (ppu->cpu_read_active) ppu->read_buffer = ppu->pat_lo;
            } else if (ph == 6) {
                ppu->latch = (ppu->cpu_read_active > 0) ? ppu->addr_low
                            : (uint8_t)(bg_pattern_addr(ppu, ppu->nt_latch, 1) & 0xFF);
            } else {
                uint8_t hi = ppu_vram_read(ppu, bus_addr(ppu, bg_pattern_addr(ppu, ppu->nt_latch, 1)));
                ppu->addr_low = hi;
                if (ppu->cpu_read_active) ppu->read_buffer = hi;
                bg_reload(ppu, hi);
            }
        } else if (dot == 338 || dot == 340) {
            /* Two dummy nametable fetches close the line.  Both land on the
             * nametable, not the attribute table (AccuracyCoin "$2007 Stress
             * Test" "COMMON MISTAKES": the dummy phase performs two nametable
             * fetches in a row). */
            ppu->nt_latch = ppu_vram_read(ppu, nt_addr(ppu));
            /* These dummy nametable fetches are ordinary reads on the shared
             * data bus too, so a $2007 state-machine read landing on one of
             * them takes the byte into the buffer exactly like the visible
             * background fetch does ("$2007 Stress Test" keys the last two
             * stable bytes from here). */
            if (ppu->cpu_read_active)
                ppu->read_buffer = ppu->nt_latch;
        }

        /* ---- coarse-X increments ------------------------------------- */
        if (dot >= 8 && dot <= 256 && (dot & 7) == 0) inc_coarse_x(ppu);
        else if (dot == 257) inc_coarse_x(ppu);
        else if (dot == 329 || dot == 337) inc_coarse_x(ppu);

        /* ---- vertical increment at dot 256 --------------------------- */
        if (dot == 256)
            inc_y(ppu);

        /* ---- horizontal t->v copy at dot 257 ------------------------- */
        if (dot == 257)
            copy_horizontal(ppu);

        /* ---- vertical t->v copy, pre-render dots 280-304 ------------- */
        if (line == prerender && dot >= 280 && dot <= 304)
            copy_vertical(ppu);

        /* ---- sprite evaluation / fetch ------------------------------- */
        /* Evaluation runs only on visible lines (04-ppu-sprites.md section 2:
         * "MUST NOT evaluate on the pre-render scanline").  The pre-render
         * line still *fetches* sprites, using whatever secondary OAM line 239
         * left behind -- that stale content is how a sprite can appear on
         * scanline 0 ("Sprites On Scanline 0"). */
        if (visible_line || line == prerender) {
            if (dot == 1 && visible_line) {
                memset(ppu->oam2, 0xFF, sizeof(ppu->oam2));
                ppu->oam2_reg = 0;
                ppu->spr_eval_m = 0;
                ppu->spr_eval_found = 0;
                ppu->spr_eval_state = 0;
                ppu->spr_eval_b = 0;
                ppu->sprite0_on_line = false;
                ppu->spr_count = 0;
            } else if (dot >= 1 && dot <= 64 && visible_line) {
                ppu->oam_readbuf = 0xFF;
            } else if (dot == 65 && visible_line) {
                /* S/EV: the evaluator samples OAMADDR here (dot 66 in the
                 * latched HDL alignment), which is what makes a $2003 write
                 * during the secondary-OAM clear window move the evaluation
                 * start -- the "arbitrary sprite zero" behaviour
                 * (04-ppu-sprites.md sections 2 and 3). */
                ppu->spr_eval_n = ppu->oam_addr;
                ppu->eval_start = ppu->oam_addr;
                sprite_eval_step(ppu);
            } else if (dot > 65 && dot <= 256 && visible_line) {
                /* Each OAM byte takes two PPU clocks to read, so the evaluator
                 * steps every other dot (04-ppu-sprites.md section 2); that
                 * cadence is what AccuracyCoin's "$2004 Stress" table of every
                 * dot of a scanline encodes. */
                if (dot & 1)
                    sprite_eval_step(ppu);
                ppu->oam_readbuf = ppu->oam[ppu->spr_eval_n];
            } else if (dot >= 257 && dot <= 320) {
                /* Sprite fetch window: the eight lanes of secondary OAM are
                 * serialised onto the sprite pipeline, one byte per two dots.
                 * The lanes are dropped at the start of the window, so an
                 * object that is no longer in range cannot keep stale pattern
                 * data (04-ppu-sprites.md section 5). */
                if (dot == 257) {
                    int i;
                    /* The lanes are dropped and the secondary-OAM address
                     * restarts at slot 0 at the beginning of the fetch window
                     * ("Misaligned OAM2 Address": a $2004 read on dot 324 hits
                     * index 0 under normal behaviour). */
                    if (!ppu->oam2_inc_frozen)
                        ppu->oam2_reg = 0;
                    for (i = 0; i < 8; i++) {
                        ppu->spr_lane_armed[i] = 0;
                        ppu->spr_lane_done[i] = 0;
                        ppu->spr_lane_shifted[i] = 0;
                        ppu->spr_lane_flip[i] = 0;
                        ppu->spr_lane_x[i] = 0xFF;
                        ppu->spr_lane_pat[i][0] = 0;
                        ppu->spr_lane_pat[i][1] = 0;
                    }
                }
                sprite_fetch_dot(ppu);
            }
            /* OAMADDR is forced to zero for the whole evaluation and fetch
             * window (04-ppu-sprites.md section 2). */
            if (dot >= 65 && dot <= 320)
                ppu_oam_addr_force_zero(ppu);

            /* Enabling rendering on dots 63, 255 and 339 releases the frozen
             * secondary-OAM address, so the following fetch walks the slots
             * again ("Frozen OAM2 Increment" tests 2-4). */
            if (ppu->rendering && (dot == 63 || dot == 255 || dot == 339))
                ppu->oam2_inc_frozen = false;
        }

        /* ---- secondary-OAM address ----------------------------------- */
        /* The address the evaluator (and the clear/fetch windows) is walking.
         * It is the seed of the OAM-corruption row when BLNK becomes active
         * (AccuracyCoin "OAM Corruption"). */
        if (dot >= 1 && dot <= 64) {
            ppu->oam2_addr = (uint8_t)((dot - 1) >> 1);
        } else if (dot >= 65 && dot <= 256) {
            ppu->oam2_addr = (uint8_t)(ppu->spr_eval_found * 4 + ppu->spr_eval_b);
        } else if (dot >= 257 && dot <= 320) {
            int off = dot - 257;
            int ph = off & 7;
            if (ph <= 2)
                ppu->oam2_addr = (uint8_t)((off >> 3) * 4 + ph + 1);
            else if (ph == 7)
                ppu->oam2_addr = (uint8_t)(((off >> 3) * 4 + 4) & 0x1F);
        }
    }

    /* The corruption seed is the secondary-OAM address two PPU clocks after
     * BLNK became active, so the sample is taken here, after this dot's
     * address update (AccuracyCoin "OAM Corruption" test 2 calibrates it: a
     * $2001 write on dot 7 of scanline 0 must corrupt row 4).  Inside the
     * evaluation window a seed that is not a multiple of four is "ceilinged"
     * to the next object slot, as the test documents. */
    if (ppu->corrupt_seed_delay > 0 && --ppu->corrupt_seed_delay == 0) {
        uint8_t seed = ppu->oam2_addr;
        if (dot >= 65 && dot <= 256)
            seed = (uint8_t)((seed + 3) & ~3u);
        ppu->oam2_seed = seed;
    }

    /* ---- shift registers --------------------------------------------- */
    /* The background shifter rotates once per pixel dot: 256 times during the
     * visible region and 16 more during the two-tile prefetch, i.e. 272 of
     * the line's 341 dots (04-ppu-rendering.md section 4). */
    if (renderable && (dot <= 255 || (dot >= 321 && dot <= 336))) {
        /* The two pattern shifters have their serial inputs tied to different
         * levels: plane A shifts in a 0, plane B shifts in a 1.  Normally the
         * next tile overwrites the low half before the level matters, but a
         * rendering-disable trick can expose it (AccuracyCoin "BG Serial
         * In"). */
        /* The two pattern shifters do not share a serial input: plane A
         * shifts in a 0, plane B shifts in a 1.  The next tile normally
         * overwrites the low half before it matters, but skipping the pattern
         * load (disable rendering around dot%8 == 7) exposes it --
         * AccuracyCoin "BG Serial In". */
        ppu->bg_sr_lo <<= 1;
        ppu->bg_sr_hi = (uint16_t)((ppu->bg_sr_hi << 1) | 1u);
        ppu->bg_at_lo <<= 1;
        ppu->bg_at_hi <<= 1;
    }

    /* The odd-frame pre-render line omits dot 340.  Every dot the counters
     * tick once, so dropping one dot leaves the whole sprite row one dot ahead
     * of the background: the first sprite pixel lands on x = 0 on alternating
     * frames instead of at the object's X.  This is the "Sprites On Scanline 0"
     * behaviour (17.5% of frames on NTSC). */
    if (ppu->spr_shift_hpos && !ppu_region_pal(ppu) &&
        line == prerender && dot == 340) {
        int i;
        for (i = 0; i < 8; i++)
            ppu->spr_lane_cnt[i] = (uint8_t)(ppu->spr_lane_cnt[i] - 1);
        ppu->spr_shift_hpos = false;
    }

    /* ---- advance the counters ---------------------------------------- */
    ppu->dot++;
    if (ppu->dot >= 341) {
        ppu->dot = 0;
        ppu->line++;
        if (ppu->line >= h) {
            ppu->line = 0;
            ppu->frame_odd = !ppu->frame_odd;
            ppu->nes->frame_count++;
        }
    }
    ppu->dot_count++;
}