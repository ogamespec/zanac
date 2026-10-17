/* savestate.h - save states of the Zanac C port.
 *
 * A state file is a self-contained snapshot of everything the port needs to
 * continue a game: the neslib PPU/APU/cartridge state, the 2 KB work RAM, the
 * ported 6502 registers and flags, the UxROM bank latch and the game's resume
 * point.  It is written at a frame boundary by the frame hook (F5 in the SDL
 * frontend) and can be loaded into a fresh process (F7, or --load-state), which
 * is what makes a save useful for a bug report: the run can be reproduced
 * off-line, with --shot / --dump-ppu / --dump-ram at the exact frame.
 *
 * The format is little-endian, versioned and CRC-checked; the documentation of
 * the layout is in savestate.c and in c_src/Readme.md ("Save states").
 */
#ifndef ZANAC_SAVESTATE_H
#define ZANAC_SAVESTATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* F5 / F7 use this path unless --save-state / --load-state or the
 * ZANAC_STATE_FILE environment variable names another one. */
#define ZANAC_QUICKSAVE_PATH "zanac_quicksave.state"

/* The path F5 writes to and F7 reads from.  ZANAC_STATE_FILE overrides the
 * default, which is what the regression tests use to keep their files out of
 * the source directory. */
const char *zanac_state_path(void);

/* Write the current state.  `host_frame` is the frontend's frame number (the
 * one --hashes/--input use); it is stored so a loaded run continues the same
 * timeline.  The call must be made at a frame boundary.  zanac_state_save()
 * is for the states that cannot wait for a level-loop boundary (menu, level
 * setup, a save after a stall); zanac_state_save_checkpoint() marks the file as
 * captured exactly at loc_E880, which is what makes a loaded run continue frame
 * for frame. */
bool zanac_state_save(const char *path, uint64_t host_frame);
bool zanac_state_save_checkpoint(const char *path, uint64_t host_frame);

/* The game's per-frame loops call zanac_state_checkpoint() before their first
 * instruction: the level loop (loc_E880, $E880), the pause loop (loc_D3B9,
 * $D3B9), the title/demo menu loop (loc_E080, $E080) and the GAME
 * START/CONTINUE selector (loc_E190, $E190).  Those points are complete
 * program states - all live state is in RAM/registers - so a snapshot taken
 * there is resumed by entering the same loop, without a C call stack.  The host
 * installs a hook to capture a pending F5 / --save-state at that boundary. */
typedef void (*ZanacStateCheckpointHook)(void);
void zanac_state_set_checkpoint_hook(ZanacStateCheckpointHook hook);
void zanac_state_checkpoint(void);

/* Read `path` into the engine and the ported game.  On success *host_frame gets
 * the frame number stored in the file.  The game's outer loop to enter is left
 * in zanac_resume_kind; call zanac_state_resume() next (it does not return). */
bool zanac_state_load(const char *path, uint64_t *host_frame);

/* True when `path` exists (F7 checks before it unwinds the running game). */
bool zanac_state_exists(const char *path);

/* One-line description of the file (version, frames, mode, level, bank, size),
 * for --state-info.  Returns false if the file is missing or not a state. */
bool zanac_state_info(const char *path, char *out, size_t out_size);

/* Enter the game at the outer loop recorded by zanac_state_load(): the level
 * loop (sub_E81E) or the menu loop (main_menu_loop).  Like the Reset handler it
 * runs the game, so it never returns. */
void zanac_state_resume(void);

#endif /* ZANAC_SAVESTATE_H */
