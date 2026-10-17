/* neslib.h - the NES engine library used by the Zanac C port.
 *
 * neslib is derived from the Nespeculator emulator core (see core/ and
 * ../neslib/README.md): it keeps the RP2C02 PPU, the 2A03 APU, the cartridge
 * and the controller emulation, and replaces the 6502 CPU with the C code of
 * the game.
 *
 * The game never sees a mapper, a CPU or a bus: it asks neslib to write and
 * read NES registers, exactly like the 6502 code it was ported from:
 *
 *      STA PPU_CTRL_REG1        ->  neslib_write(0x2000, A)
 *      LDA PPU_STATUS           ->  neslib_read(0x2002)
 *      STA PPU_SPR_DMA          ->  neslib_write(0x4014, 2)   (OAM DMA)
 *
 * Frame flow: the game calls neslib_wait_vblank() whenever the 6502 code did
 * `JSR vsync`.  neslib advances the PPU and APU to the start of vblank, calls
 * the VBlank callback (the ported NMI handler) if the NMI is enabled in
 * $2000, and then advances to the end of the frame.
 */
#ifndef NESLIB_H
#define NESLIB_H

#include <stdint.h>
#include <stdbool.h>

#include "core/cart.h"   /* Mirroring: the board's nametable arrangement */

/* ------------------------------------------------------------------------ *
 *  Callbacks
 * ------------------------------------------------------------------------ */
typedef struct NeslibCallbacks {
    void (*reset)(void);    /* Reset handler: runs once at power-on */
    void (*vblank)(void);   /* NMI handler: runs at the start of every vblank */
} NeslibCallbacks;

/* Board description of the cartridge the game was written for.  The nametable
 * arrangement is not part of it: it is passed to neslib_power_on() as a
 * Mirroring value (see core/cart.h), because it is board wiring the caller
 * knows from the cartridge header. */
typedef struct NeslibBoard {
    uint32_t prg_size;          /* PRG ROM size in bytes */
    uint32_t chr_ram_size;      /* CHR RAM size in bytes (0: CHR ROM) */
    uint8_t  mapper;            /* iNES mapper number */
} NeslibBoard;

/* Power on the engine (PPU, APU, controller, cartridge) and install the
 * callbacks.  `board` may be NULL for the default board; `mirroring` is the
 * nametable arrangement of the cartridge (MIRROR_HORIZONTAL, MIRROR_VERTICAL,
 * ...).  The Reset callback is *not* run here: the game's own machine model must
 * be initialised first, so the host calls neslib_reset() once everything is in
 * place (that is the console's reset vector). */
void neslib_power_on(const NeslibBoard *board, Mirroring mirroring,
                     const NeslibCallbacks *callbacks);

/* Run the Reset callback - the port of the console's reset vector.  The ported
 * Reset handler never returns: it is the game's outer loop (reset ->
 * main_menu_loop -> sub_E81E -> ...), exactly like the 6502 reset vector. */
void neslib_reset(void);

/* Per-frame hook.  neslib calls it at the end of every frame (after the VBlank
 * callback and the pre-render line), which is where a host presents the
 * picture, drains the audio and decides whether to stop.  To stop a run, the
 * hook must leave through setjmp/longjmp: the game loop is inside
 * neslib_reset() and has no return path of its own. */
typedef void (*NeslibFrameHook)(void);
void neslib_set_frame_hook(NeslibFrameHook hook);

/* Hook called once per *emulated* frame (at the vblank rise), whatever the
 * cause: the game asking for the next frame, or a spin loop inside the ported
 * code (neslib_pump()).  A real-time host uses it to pace the run: the ported
 * game produces one frame of audio per emulated frame, so pacing on the host's
 * frame hook alone lets the emulation run ahead of the sound card and the audio
 * queue grows into a permanent delay.  Headless runs leave it unset. */
typedef void (*NeslibVBlankHook)(void);
void neslib_set_vblank_hook(NeslibVBlankHook hook);

/* ------------------------------------------------------------------------ *
 *  Register access - the only way the game talks to the hardware
 * ------------------------------------------------------------------------ */
/* One 6502 store / load.  $0000-$1FFF is work RAM, $2000-$3FFF the PPU,
 * $4000-$4017 the APU/IO (including OAM DMA at $4014 and the controller
 * strobe at $4016), $4018-$FFFF open bus. */
void    neslib_write(uint16_t addr, uint8_t value);
uint8_t neslib_read(uint16_t addr);

/* Typed helpers (identical semantics, kept for readability at call sites). */
void    neslib_ppu_write(uint16_t reg, uint8_t value);   /* $2000-$2007 */
uint8_t neslib_ppu_read(uint16_t reg);
void    neslib_apu_write(uint16_t reg, uint8_t value);   /* $4000-$4017 */
uint8_t neslib_apu_read(uint16_t reg);

/* $4014: copy 256 bytes from work-RAM page `page` into primary OAM. */
void    neslib_oam_dma(uint8_t page);
/* $4016 bit 0 strobe for the standard controllers. */
void    neslib_joy_strobe(uint8_t value);
/* $4016 (port 0) / $4017 (port 1) shift register read. */
uint8_t neslib_joy_read(int port);

/* Host input: press/release one button of a controller. */
enum {
    NESLIB_BTN_A = 0, NESLIB_BTN_B, NESLIB_BTN_SELECT, NESLIB_BTN_START,
    NESLIB_BTN_UP, NESLIB_BTN_DOWN, NESLIB_BTN_LEFT, NESLIB_BTN_RIGHT
};
void    neslib_set_button(int port, int button, bool pressed);

/* ------------------------------------------------------------------------ *
 *  Frame control
 * ------------------------------------------------------------------------ */
/* Advance to the start of the next vblank, dispatch the VBlank callback when
 * the NMI is enabled, then advance to the end of the frame.  This is the port
 * of the game's `vsync` wait loop. */
void    neslib_wait_vblank(void);

/* Advance only up to the moment $2002 bit 7 becomes set, leaving the flag set
 * (the port of the original's "spin on PPU_STATUS" loops, e.g. the three-vblank
 * warm-up of `reset`). */
void    neslib_wait_vblank_flag(void);

/* Run the engine up to the next vblank and dispatch the VBlank callback once.
 * This is the port's equivalent of "let the NMI handler run while the 6502
 * spins": the ported game code uses it inside the loops that wait for state the
 * NMI updates (cram_update_mode, byte_20, ...) or for a controller press. */
void    neslib_pump(void);

/* Advance one whole frame without dispatching the VBlank callback (used before
 * the game has started and by the host when idle). */
void    neslib_step_frame(void);

/* ------------------------------------------------------------------------ *
 *  Video / audio
 * ------------------------------------------------------------------------ */
#define NESLIB_SCREEN_W 256
#define NESLIB_SCREEN_H 240
/* 240 rows of 256 pixels, 0x00RRGGBB. */
const uint32_t *neslib_framebuffer(void);

void    neslib_audio_enable(double sample_rate);
int     neslib_audio_available(void);           /* samples waiting */
int     neslib_audio_read(float *dst, int max); /* interleaved-free mono */

/* ------------------------------------------------------------------------ *
 *  State the ported game code needs
 * ------------------------------------------------------------------------ */
uint8_t *neslib_ram(void);                      /* the 2 KB work RAM */
uint64_t neslib_frame_count(void);

/* Current contents of the write-only PPU registers, for the HUD and the debug
 * builds (a read of $2000/$2001 returns open bus, not these values). */
/* Counters of the engine's own progress, for the debug builds and the HUD:
 * how many times the ported code asked for a frame (ZANAC_WAIT_NMI / vsync),
 * how many times the VBlank callback actually ran, and the PPU frame counter. */
void    neslib_debug_counts(long *pumps, long *dispatches, long *frames);

/* The engine handle, for the diagnostic dumps (frontend_dump_ppu). */
void   *neslib_debug_nes(void);

/* Called by the save-state loader once the PPU/APU/cartridge/RAM state has been
 * put back.  Drops the engine's per-frame latches (the loaded state is resumed
 * from a game loop, not from inside the wait that was interrupted) and empties
 * the queued audio so the sound card does not play samples from the run that
 * was abandoned. */
void    neslib_state_after_load(void);

uint8_t neslib_ppu_ctrl(void);
uint8_t neslib_ppu_mask(void);
bool    neslib_nmi_enabled(void);

#endif /* NESLIB_H */
