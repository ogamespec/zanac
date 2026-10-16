/* zanac.h - the master header of the Zanac C port.
 *
 * Every ported translation unit includes this file: it pulls in the 6502
 * programmer's model (zanac_ram.h), the RAM variables with their original
 * addresses (vars.h), the prototypes of all original procedures (labels.h) and
 * the NES register addresses (nesregs.h).
 *
 * The two helpers below cover the original's two "computed control flow"
 * idioms.  See c_src/PORTING.md for the full translation rules.
 */
#ifndef ZANAC_H
#define ZANAC_H

#include "neslib/neslib.h"
#include "zanac_ram.h"
#include "nesregs.h"
#include "vars.h"
#include "labels.h"
#include "rom_labels.h"
#include "zanac_stubs.h"

/* ------------------------------------------------------------------------ *
 *  Inline parameter blocks
 *
 *  print_string, sub_C6CA and sub_C725 were called with their parameters in
 *  the instruction stream that followed the JSR (the callee popped the return
 *  address and read the block there).  The port keeps the callee byte for byte
 *  but the call site supplies the block address explicitly, because a C call
 *  has no return address in the 6502 stack page:
 *
 *      PRINT_STRING(0xC7E3);     / * JSR print_string, block at $C7E3 * /
 *
 *  zanac_inline_block is what the callee reads instead of the popped return
 *  address (the PLAs of the original are replaced by a single assignment).
 * ------------------------------------------------------------------------ */
extern uint16_t zanac_inline_block;

#define ZANAC_LO(a) ((uint8_t)((uint16_t)(a) & 0xFF))
#define ZANAC_HI(a) ((uint8_t)(((uint16_t)(a) >> 8) & 0xFF))

/* JSR print_string ; .byte count, .word vram, text... */
#define PRINT_STRING(block)                                     \
    do { zanac_inline_block = (uint16_t)(block); print_string(); } while (0)

/* JSR sub_C6CA (count/attribute block) -- same convention. */
#define INLINE_CALL(proc, block)                                    \
    do { zanac_inline_block = (uint16_t)(block); proc(); } while (0)

/* ------------------------------------------------------------------------ *
 *  switch_case
 *
 *  `JSR j_switch_case` is followed by a table of 16-bit addresses; the routine
 *  indexes it with A and returns to the selected entry.  In C the call site
 *  passes the table address and switch_case() dispatches through
 *  zanac_call_at(), which maps an original 6502 address to its ported C
 *  procedure (generated in zanac_dispatch.c):
 *
 *      SWITCH_CASE(0xD50D);      / * JSR j_switch_case, table at $D50D * /
 * ------------------------------------------------------------------------ */
extern uint16_t zanac_inline_table;

#define SWITCH_CASE(table)                                          \
    do { zanac_inline_table = (uint16_t)(table); j_switch_case(); } while (0)

/* Call the ported procedure whose original entry point is `addr`.  Generated
 * from the label map; reports an error when `addr` is not a procedure entry. */
void zanac_call_at(uint16_t addr);

/* RTS to the address currently on top of the emulated 6502 stack.  This is the
 * port of the computed `RTS`-jumps the original uses (switch_case and the
 * PLA/RTS unwind helpers). */
void zanac_rts_dispatch(void);

/* Called by zanac_call_at() for an unknown address (prints and returns). */
void zanac_call_at_missing(uint16_t addr);

#endif /* ZANAC_H */
