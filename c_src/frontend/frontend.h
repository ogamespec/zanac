/* frontend.h - host side of the Zanac C port: video output and headless dumps.
 *
 * The game itself never calls into this header; main.c uses it for the
 * interactive window (frontend_run) and for the offline dumps used by the
 * regression tests (frontend_write_ppm / frontend_write_wav).
 */
#ifndef ZANAC_FRONTEND_H
#define ZANAC_FRONTEND_H

#include <stdint.h>

/* The game's outer loop never returns (it is the ported 6502 reset path), so
 * the host drives nothing: it installs the frame hook of neslib and lets the
 * engine call it once per frame.  These are the two hook implementations.
 *
 * frontend_init() is called before the engine starts, frontend_frame() once
 * per frame; it returns what the host should do next (the caller leaves the
 * game loop with a longjmp when it is not FRONTEND_CONTINUE).
 * frontend_shutdown() runs at exit.
 *
 * The SDL frontend reports a save/load key through the return value instead of
 * calling the save-state code itself: the state belongs to the game
 * (zanac/savestate.c), which the frontend does not link against directly.  The
 * frame returned by frontend_frame() is a frame boundary, which is exactly the
 * point a state may be captured or replaced. */
enum {
    FRONTEND_CONTINUE    = 0,
    FRONTEND_QUIT        = 1,
    FRONTEND_SAVE_STATE  = 2,   /* F5: main writes the state, then continues */
    FRONTEND_LOAD_STATE  = 3    /* F7: main loads the state and resumes      */
};
int  frontend_init(void);
int  frontend_frame(void);
void frontend_shutdown(void);

/* Called after a state was loaded (the SDL frontend drops the queued audio and
 * re-synchronises the frame clock; the headless one has nothing to do). */
void frontend_state_loaded(void);

/* Write a framebuffer (256x240, 0x00RRGGBB) as a binary PPM (P6). */
int frontend_write_ppm(const char *path, const uint32_t *framebuffer,
                       int width, int height);

/* Write everything the APU has produced since the last call as a 16-bit mono
 * WAV file at `sample_rate`. */
int frontend_write_wav(const char *path);

/* FNV-1a hash of a framebuffer, used by the regression tests to compare the C
 * port against the original ROM frame by frame. */
uint32_t frontend_frame_hash(const uint32_t *framebuffer);

/* Write the PPU memory (pattern tables, name-table RAM, palette, OAM) next to
 * `prefix` as prefix.chr / .nt / .pal / .oam - the diagnostic used to compare
 * the port against the original ROM. */
int frontend_dump_ppu(const char *prefix);
/* The same, for a caller that owns the engine (c_src/reference/run_rom.c). */
struct Nes;
int frontend_dump_nes(struct Nes *nes, const char *prefix);

/* Write the 2 KB of work RAM to `path` (the variables of the port and of the
 * original ROM can then be compared byte for byte). */
int frontend_dump_ram(const uint8_t *ram, const char *path);

/* Sample rate the frontends use. */
#define FRONTEND_SAMPLE_RATE 48000.0

#endif /* ZANAC_FRONTEND_H */
