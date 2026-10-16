/* input_script.h - scripted controller input for the regression runs. */
#ifndef ZANAC_INPUT_SCRIPT_H
#define ZANAC_INPUT_SCRIPT_H

#include <stdint.h>

/* Parse the script; returns 0 on success. */
int input_script_load(const char *path);

/* Buttons held on `frame`, in the NES joypad bit order
 * (bit 0 A, 1 B, 2 Select, 3 Start, 4 Up, 5 Down, 6 Left, 7 Right). */
uint8_t input_script_buttons(long frame);

#endif /* ZANAC_INPUT_SCRIPT_H */
