/* input.h - standard controllers ($4016/$4017).
 *
 * Reference: Specs/05-boards-and-io.md section 6.
 * SPEC GAP BRD-1: the specification records that the controller protocol has no
 * rank-1 source; the protocol implemented here is the one the specification
 * transcribes (write $4016 bit 0 = 1 then 0, then eight reads; the report order
 * is A, B, Select, Start, Up, Down, Left, Right and 1 means pressed).
 * SPEC GAP BRD-2: the spec notes the shift order for Select/Start is contested
 * (`Docs/NES/NESController/timing.jpg` labels them the other way round).
 */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

/* Button bit order as reported by the shift register. */
enum {
    BTN_A      = 0,
    BTN_B      = 1,
    BTN_SELECT = 2,
    BTN_START  = 3,
    BTN_UP     = 4,
    BTN_DOWN   = 5,
    BTN_LEFT   = 6,
    BTN_RIGHT  = 7,
    BTN_COUNT  = 8
};

typedef struct Input {
    uint8_t  buttons[2];   /* current button state, bit per BTN_* */
    uint8_t  shift[2];     /* 8-bit shift register */
    uint8_t  strobe;       /* $4016 bit 0 */
    uint8_t  read_count[2];
} Input;

void    input_init(Input *in);
void    input_write_strobe(Input *in, uint8_t value);
uint8_t input_read(Input *in, int port, uint8_t open_bus);
void    input_set_button(Input *in, int port, int button, bool pressed);

#endif /* INPUT_H */
