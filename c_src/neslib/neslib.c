/* neslib.c - the NES engine library used by the Zanac C port.
 *
 * This is the "CPU-less" driver of the Nespeculator core: the emulated PPU,
 * APU, cartridge and controller are kept, the 6502 core is left inert and the
 * game's ported C code supplies the register traffic instead.
 *
 * Two deliberate deviations from cycle-accurate emulation are documented here
 * because the ported code depends on them:
 *
 *  1. Register writes are issued from C rather than from a cycle-timed 6502,
 *     so the PPU's PCLK write-latency queues ($2001/$2006) are flushed before
 *     the next register access is interpreted (ppu_flush_write_queues).  The
 *     6502 wrote those registers four or more cycles apart anyway, so the
 *     *order* the original code produced is preserved.
 *
 *  2. $4016 is strobed immediately instead of at the next ACLK2 edge, because
 *     the ported pad_read writes the register and reads the shift register
 *     with no emulated cycles in between.
 */
#include <stdlib.h>
#include <string.h>

#include "neslib.h"
#include "core/nes.h"
#include "core/cpu.h"
#include "core/ppu.h"
#include "core/apu.h"
#include "core/cart.h"
#include "core/input.h"

static Nes             *g_nes;
static NeslibCallbacks  g_cb;
static NeslibFrameHook  g_frame_hook;

/* Two separate latches, because the hardware vblank flag and "this frame has
 * reached vblank" are not the same thing:
 *
 *   g_vblank_reached  the current frame has passed the vblank rise (line 241,
 *                     dot 1).  The VBlank callback runs once, when it becomes
 *                     true, and it is cleared when the frame is finished.
 *   g_vblank_live     the flag has been set and the game has not read $2002
 *                     since; a $2002 read clears it, exactly like the hardware
 *                     flag.  This is what makes the reset warm-up loop consume
 *                     three distinct frames.
 *
 * Keeping them apart matters because the NMI handler itself reads $2002: if
 * that read cleared "this frame reached vblank" as well, the visible part of
 * the frame would be waited for twice and the loop would never end.
 */
static bool g_vblank_reached;
static bool g_vblank_live;
static NeslibVBlankHook g_vblank_hook;

/* CPU cycles one neslib_pump() call advances (see neslib_pump). */
#define ZANAC_PUMP_CYCLES 32

/* True while the VBlank callback runs.  The ported game does call back into the
 * engine from there (the music engine reaches all_enemies_destroyed -> vsync),
 * and a nested wait must not re-dispatch the callback or reset the flags the
 * outer wait is spinning on: on the real 6502 an NMI cannot be taken again
 * before its handler returns with RTI. */
static bool g_in_vblank;


/* Counters for the debug builds (see neslib_debug_counts). */
static long g_pump_calls;
static long g_vblank_dispatches;

/* Advance one PPU dot; watch for the vblank flag.  When `dispatch` is set the
 * VBlank callback runs at the exact dot the flag appears (line 241, dot 2 on
 * NTSC), which leaves lines 241-261 for its register writes and in particular
 * keeps them before the pre-render t->v scroll copy at (261, 280-304). */
static void dot_tick(Nes *nes, bool dispatch)
{
    uint64_t before = nes->frame_count;

    ppu_tick(nes->ppu);

    if (nes->frame_count != before) {
        /* A whole emulated frame just ended.  The host is told here, in the
         * single place every frame passes through, so that it sees one frame
         * per emulated frame whether the game asked for it or reached it by
         * spinning (neslib_pump). */
        if (g_frame_hook)
            g_frame_hook();
    }
    if (nes->ppu->status & 0x80) {
        g_vblank_live = true;
        if (!g_vblank_reached) {
            /* One emulated frame has just ended: the host gets to pace the run
             * here, so that a host that only sees *some* frames (the game also
             * spins through neslib_pump()) still advances at 60.0988 Hz. */
            g_vblank_reached = true;
            if (g_vblank_hook)
                g_vblank_hook();
            if (dispatch && (nes->ppu->ctrl & 0x80) && g_cb.vblank) {
                g_vblank_dispatches++;
                g_in_vblank = true;
                g_cb.vblank();
                g_in_vblank = false;
            }
        }
    }
}

/* One CPU cycle of the PPU/APU timeline: nes_run_cpu_cycle without the CPU.
 * NTSC/Dendy run three PPU dots per CPU cycle; a frame is 89342 dots, which is
 * not a whole number of CPU cycles, so callers that need a frame boundary stop
 * on the frame counter rather than on a cycle count. */
static void cycle_tick(Nes *nes, bool dispatch)
{
    nes->cpu_cycles++;
    apu_frame_tick(nes->apu);
    input_write_strobe(&nes->input, nes->apu->pad_out ? 1 : 0);
    nes->ppu->vblank_read_suppress = false;
    nes->ppu->vblank_set_in_cycle =
        ppu_vblank_line(nes->ppu) && nes->ppu->dot == 1;

    if (nes->region == NES_REGION_PAL) {
        static const int pal_dots[5] = { 3, 3, 4, 3, 3 };
        int n = pal_dots[nes->cpu_cycles % 5];
        int i;
        for (i = 0; i < n; i++) dot_tick(nes, dispatch);
    } else {
        int i;
        for (i = 0; i < 3; i++) dot_tick(nes, dispatch);
    }
    apu_tick(nes->apu);
}

/* ------------------------------------------------------------- set-up ---- */

void neslib_power_on(const NeslibBoard *board, const NeslibCallbacks *callbacks)
{
    NeslibBoard defaults = { 128u * 1024u, 8192u, 2, true };
    static const NeslibCallbacks none = { NULL, NULL };

    if (!board) board = &defaults;
    g_cb = callbacks ? *callbacks : none;

    g_nes = nes_create(NES_REGION_NTSC);
    if (!g_nes) return;

    /* The cartridge supplies the 8 KB of writable CHR RAM the game uploads its
     * pattern tables into.  No PRG image is needed: the ported code reads the
     * extracted banks directly (see zanac_banks.c), so the mapper is not
     * emulated at all. */
    cart_init_chr_ram(g_nes->cart, g_nes, board->mapper, board->chr_ram_size,
                      board->vertical_mirroring);
    nes_power_on(g_nes);

    /* Deterministic power-on RAM: the game's Reset handler initialises every
     * variable it uses, and a zeroed RAM makes the "hard reset" marker check
     * behave like a cold start. */
    memset(g_nes->ram, 0, sizeof(g_nes->ram));

    /* Power-on / reset clears $2000/$2001/$2005/$2006 for ~29658 CPU cycles.
     * The ported Reset handler runs at once, so the window is closed here
     * (documented deviation; a real 6502 spends those cycles on the reset
     * sequence and the PPU warm-up loop). */
    g_nes->ppu->rc_active = false;

}

void neslib_reset(void)
{
    if (g_cb.reset)
        g_cb.reset();
}

/* ---------------------------------------------------------- registers ---- */

void neslib_write(uint16_t addr, uint8_t value)
{
    if (!g_nes) return;
    if (addr >= 0x2000 && addr < 0x4000)
        ppu_flush_write_queues(g_nes->ppu);

    if (addr == 0x4014) {
        neslib_oam_dma(value);
        return;
    }
    nes_cpu_write(g_nes, addr, value);

    if (addr == 0x4016) {
        /* Immediate strobe: the ported pad_read writes and reads with no
         * emulated CPU cycles in between (see the note at the top). */
        input_write_strobe(&g_nes->input, (uint8_t)(value & 1));
    }
}

uint8_t neslib_read(uint16_t addr)
{
    uint8_t value;
    if (!g_nes) return 0xFF;
    if (addr >= 0x2000 && addr < 0x4000)
        ppu_flush_write_queues(g_nes->ppu);
    value = nes_cpu_read(g_nes, addr);
    /* Reading $2002 clears the vblank flag, so the next wait for vblank has to
     * look for a new rise (this is what makes the reset warm-up loop below
     * consume three distinct frames). */
    if (addr >= 0x2000 && addr < 0x4000 && (addr & 0x07) == 0x02)
        g_vblank_live = false;
    /* The 6502 core updates the open-bus latches after every read; the ported
     * code takes its place, so neslib does the same here. */
    if (addr != 0x4015)
        nes_set_ext_bus(g_nes, value);
    nes_set_open_bus(g_nes, value);
    return value;
}

void neslib_ppu_write(uint16_t reg, uint8_t value) { neslib_write((uint16_t)(0x2000 | (reg & 7)), value); }
uint8_t neslib_ppu_read(uint16_t reg)              { return neslib_read((uint16_t)(0x2000 | (reg & 7))); }
void neslib_apu_write(uint16_t reg, uint8_t value) { neslib_write((uint16_t)(0x4000 | (reg & 0x1F)), value); }
uint8_t neslib_apu_read(uint16_t reg)              { return neslib_read((uint16_t)(0x4000 | (reg & 0x1F))); }

void neslib_oam_dma(uint8_t page)
{
    int i;
    if (!g_nes) return;
    /* A real OAM DMA takes 513/514 CPU cycles; the port performs the transfer
     * at once and only keeps its effect (the game issues it inside vblank with
     * rendering disabled, so no timing is observable). */
    for (i = 0; i < 256; i++)
        ppu_oam_dma_write(g_nes->ppu, g_nes->ram[((uint16_t)page << 8 | i) & 0x7FF]);
    g_nes->oam_dma_pending = false;
    g_nes->oam_dma_active = false;
}

void neslib_joy_strobe(uint8_t value)
{
    if (!g_nes) return;
    input_write_strobe(&g_nes->input, (uint8_t)(value & 1));
}

uint8_t neslib_joy_read(int port)
{
    if (!g_nes) return 0;
    return input_read(&g_nes->input, port ? 1 : 0, g_nes->ext_bus);
}

void neslib_set_button(int port, int button, bool pressed)
{
    if (!g_nes) return;
    input_set_button(&g_nes->input, port, button, pressed);
}

/* -------------------------------------------------------------- frames --- */

/* Advance to the end of the current frame without dispatching the callback
 * again.  Used when the game asks for a frame from inside the VBlank handler:
 * the 6502 cannot take a nested NMI, so the handler only resumes on the next
 * real vblank. */
static void finish_frame_quietly(Nes *nes)
{
    uint64_t frame = nes->frame_count;

    while (nes->frame_count == frame)
        cycle_tick(nes, false);
    g_vblank_reached = false;
    g_vblank_live = false;
}

void neslib_wait_vblank(void)
{
    Nes *nes = g_nes;
    uint64_t frame;

    if (!nes) return;
    if (g_in_vblank) {
        /* Called from inside the VBlank callback (the music engine reaches
         * all_enemies_destroyed -> vsync).  On the 6502 the game's vsync polls
         * $2002 and the NMI's own flag, and both are already set here, so it
         * returns at once; waiting for "the next" frame would stretch every
         * frame the handler touches - which is what made the level-completed
         * loop advance at half speed. */
        return;
    }

    /* Run the visible part of the frame up to the point where $2002 bit 7 is
     * set (line 241, dot 1); the VBlank callback runs there.  If the game is
     * already inside vblank (it read $2002, which cleared g_vblank_live, and
     * has not been advanced since) this waits for the next frame's flag. */
    while (!g_vblank_reached)
        cycle_tick(nes, true);

    /* Vblank and pre-render: finish the frame without dispatching again. */
    frame = nes->frame_count;
    while (nes->frame_count == frame)
        cycle_tick(nes, false);
    g_vblank_reached = false;
    g_vblank_live = false;
    if (g_frame_hook)
        g_frame_hook();
}

void neslib_set_frame_hook(NeslibFrameHook hook)
{
    g_frame_hook = hook;
}

void neslib_set_vblank_hook(NeslibVBlankHook hook)
{
    g_vblank_hook = hook;
}

void neslib_wait_vblank_flag(void)
{
    /* The port of the original's "spin until $2002 bit 7 is set" loops (the
     * reset warm-up waits for three vblanks this way).  The engine is advanced
     * one frame per iteration and the flag is left set, exactly like the
     * hardware the 6502 was polling. */
    if (!g_nes) return;
    while (!g_vblank_live)
        cycle_tick(g_nes, true);
}

void neslib_pump(void)
{
    Nes *nes = g_nes;

    g_pump_calls++;
    if (!nes) return;

    /* The port of "let the NMI handler run while the 6502 spins".
     *
     * The original's spin loops execute thousands of iterations per frame (each
     * one a handful of cycles) and only see the NMI's update once per frame.
     * Advancing a whole frame per *iteration* modelled that wrongly: the boot,
     * the menus and the level intro burned several times as many frames as the
     * ROM, so the level started late - and because the level's scroll speed
     * ramps up by 2/256 per frame, a late start left the whole level scrolling
     * at half speed with the background attributes accumulating differently.
     * Advance the few cycles an iteration of the real loop takes and let the
     * caller's loop spin, exactly as the 6502 did. */
    /* Re-arm the rise detection once the PPU has left vblank: while the game
     * spins, every new frame must dispatch the VBlank callback again, exactly
     * as the 6502 sees a fresh NMI each frame.  Without this the callback runs
     * only once and the loops that wait for the NMI's work (the VRAM upload
     * buffer, the scanline state) never see it. */
    if (g_vblank_reached && !(nes->ppu->status & 0x80)) {
        g_vblank_reached = false;
        g_vblank_live = false;
    }

    {
        int i;
        for (i = 0; i < ZANAC_PUMP_CYCLES; i++)
            cycle_tick(nes, true);
    }
}

void neslib_debug_counts(long *pumps, long *dispatches, long *frames)
{
    if (pumps) *pumps = g_pump_calls;
    if (dispatches) *dispatches = g_vblank_dispatches;
    if (frames) *frames = g_nes ? (long)g_nes->frame_count : 0;
}

void neslib_step_frame(void)
{
    Nes *nes = g_nes;
    uint64_t frame;

    if (!nes) return;
    frame = nes->frame_count;
    while (nes->frame_count == frame)
        cycle_tick(nes, false);
    g_vblank_reached = false;
    g_vblank_live = false;
}

/* -------------------------------------------------------- video/audio ---- */

const uint32_t *neslib_framebuffer(void)
{
    return g_nes ? &g_nes->ppu->framebuffer[0][0] : NULL;
}

void neslib_audio_enable(double sample_rate)
{
    if (g_nes) apu_audio_enable(g_nes->apu, sample_rate);
}

int neslib_audio_available(void)
{
    return g_nes ? apu_audio_available(g_nes->apu) : 0;
}

int neslib_audio_read(float *dst, int max)
{
    return g_nes ? apu_audio_read(g_nes->apu, dst, max) : 0;
}

/* ---------------------------------------------------------------- misc --- */

void *neslib_debug_nes(void)
{
    return g_nes;
}

uint8_t *neslib_ram(void)
{
    return g_nes ? g_nes->ram : NULL;
}

uint8_t neslib_ppu_ctrl(void)
{
    return g_nes ? g_nes->ppu->ctrl : 0;
}

uint8_t neslib_ppu_mask(void)
{
    return g_nes ? g_nes->ppu->mask : 0;
}

bool neslib_nmi_enabled(void)
{
    return g_nes && (g_nes->ppu->ctrl & 0x80) != 0;
}

uint64_t neslib_frame_count(void)
{
    return g_nes ? g_nes->frame_count : 0;
}
