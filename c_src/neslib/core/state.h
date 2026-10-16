/* state.h - save states.
 *
 * The specification set does not define a save-state format (SPEC GAP
 * STATE-1): it describes the hardware state, not how to persist it.  This
 * module therefore defines its own container (a magic, a version and a payload)
 * and serialises every piece of emulated state: CPU registers, PPU registers
 * and memory, APU channel state, mapper registers and the work RAM.
 */
#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include <stdbool.h>

struct Nes;

#define STATE_MAGIC   0x4E455350u /* "NESP" */
/* 3: the APU gained the queued $4017 resets, the separate frame-IRQ line and
 * clear-pending flag and the pad strobe latch, and the CPU gained the external
 * data bus, so version-2 files are rejected instead of misread. */
#define STATE_VERSION 4u

/* Write a state file.  Returns true on success. */
bool state_save(struct Nes *nes, const char *path);
/* Load a state file.  Returns true on success. */
bool state_load(struct Nes *nes, const char *path);

/* Quick single-slot helpers used by the HUD: slot 0..9. */
bool state_save_slot(struct Nes *nes, int slot);
bool state_load_slot(struct Nes *nes, int slot);
bool state_slot_exists(int slot);
const char *state_slot_path(int slot);

#endif /* STATE_H */
