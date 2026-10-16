/* input_script.h - scripted controller input for the regression runs. */
#ifndef ZANAC_INPUT_SCRIPT_H
#define ZANAC_INPUT_SCRIPT_H

#include <stdint.h>

/* Parse the script; returns 0 on success. */
int input_script_load(const char *path);

/* Buttons held on `frame`, in the NES joypad bit order
 * (bit 0 A, 1 B, 2 Select, 3 Start, 4 Up, 5 Down, 6 Left, 7 Right). */
uint8_t input_script_buttons(long frame);

/* Shift the script's frame numbers by `frames`.
 *
 * The port's boot is shorter than the ROM's (it does not execute the 6502 reset
 * code), so by the time either reaches a given point in a level their frame
 * counters differ by a constant - around 21 frames for level 1.  A comparison
 * that wants to feed both runs the *same input at the same point in the game*
 * gives the ROM `--input-offset 21` and the port nothing. */
void input_script_set_offset(long frames);

#endif /* ZANAC_INPUT_SCRIPT_H */
