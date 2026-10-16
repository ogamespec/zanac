/* input.c - standard controller shift registers. */
#include <string.h>

#include "nes.h"
#include "input.h"

void input_init(Input *in)
{
    memset(in, 0, sizeof(*in));
    in->shift[0] = 0;
    in->shift[1] = 0;
}

void input_write_strobe(Input *in, uint8_t value)
{
    in->strobe = (uint8_t)(value & 1);
    if (in->strobe) {
        /* Parallel load while OUT0 is high. */
        in->shift[0] = in->buttons[0];
        in->shift[1] = in->buttons[1];
        in->read_count[0] = 0;
        in->read_count[1] = 0;
    } else if (in->read_count[0] == 0 && in->read_count[1] == 0) {
        /* The 1 -> 0 transition latches the button state into the registers. */
        in->shift[0] = in->buttons[0];
        in->shift[1] = in->buttons[1];
    }
}

uint8_t input_read(Input *in, int port, uint8_t open_bus)
{
    uint8_t bit;
    uint8_t result;

    if (in->strobe) {
        /* While OUT0 is high the register keeps reloading: reading in a tight
         * loop without the 1 -> 0 latch write returns button A eight times
         * (05-boards-and-io.md section 6). */
        bit = (uint8_t)(in->buttons[port] & 1);
    } else {
        bit = (uint8_t)(in->shift[port] & 1);
        in->shift[port] = (uint8_t)((in->shift[port] >> 1) | 0x80);
        in->read_count[port]++;
    }
    /* D5-D7 are never driven and hold the previous bus byte
     * (05-boards-and-io.md section 6.3); D1-D4 read 0 on the NES-001/Famicom. */
    result = (uint8_t)(open_bus & 0xE0);
    result = (uint8_t)(result | (bit & 1));
    return result;
}

void input_set_button(Input *in, int port, int button, bool pressed)
{
    if (port < 0 || port > 1 || button < 0 || button >= BTN_COUNT)
        return;
    if (pressed) in->buttons[port] |= (uint8_t)(1u << button);
    else         in->buttons[port] &= (uint8_t)~(1u << button);
}
