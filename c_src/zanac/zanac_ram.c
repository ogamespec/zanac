/* zanac_ram.c - the 6502 programmer's model of the port (see zanac_ram.h). */
#include <string.h>

#include "zanac_ram.h"
#include "rom/zanac_banks.h"

uint8_t  A, X, Y, SP;
uint8_t  F_C, F_Z, F_N, F_V, F_I, F_D;
uint8_t *RAM;

/* The bank currently visible at $8000-$BFFF.  On power-on the UxROM latch is
 * undefined; the port starts on bank 7 so that a read before the first
 * switch_bank returns the same bytes the fixed window would. */
static uint8_t s_bank = ZANAC_FIXED_BANK;

void zanac_init(void)
{
    RAM = neslib_ram();

    /* The work RAM is deliberately *not* cleared.
     *
     * RAM contents at power-on are undefined on real hardware, and this game
     * reads some of it before writing it: the background attribute pass
     * ($DB58) builds its value from $16E, which the level script only fills
     * later, so the first passes run against whatever the RAM happened to
     * hold.  The engine fills the 2 KB with a fixed, deterministic pattern
     * (nes_power_on), which is what the reference ROM runner sees and what the
     * game was written against - zeroing it here made the bottom attribute row
     * of the background come out as 0F instead of the values the ROM has
     * ("holes" at the bottom of the map). */
    A = X = Y = 0;
    SP = 0xFF;                    /* reset does LDX #$FF / TXS itself */
    F_C = F_Z = F_N = F_V = F_I = F_D = 0;
    s_bank = ZANAC_FIXED_BANK;
}

/* ------------------------------------------------------------- memory ---- */

uint8_t zanac_bank(void)
{
    return s_bank;
}

/* Load path only: put the latch back without recording a mapper-write in the
 * diagnostic history (a load is not a bank switch the game executed). */
void zanac_bank_restore(uint8_t bank)
{
    s_bank = (uint8_t)(bank & 0x0F);
}

/* Diagnostic ring of the most recent bank selections (see zanac_call_at_missing
 * in the generated zanac_dispatch.c): the port's banking is transparent, so a
 * wrong selection shows up as a bogus computed call a lot later. */
uint8_t zanac_bank_history[16];
int     zanac_bank_history_pos;

void zanac_switch_bank(uint8_t bank)
{
    s_bank = (uint8_t)(bank & 0x0F);
    zanac_bank_history[zanac_bank_history_pos++ & 15] = s_bank;
}

uint8_t zanac_bank_read(uint8_t bank, uint16_t addr)
{
    const uint8_t *image = zanac_banks[bank & 0x07];
    return image[(uint16_t)(addr - ZANAC_BANK_WINDOW) & (ZANAC_BANK_SIZE - 1)];
}

uint8_t zanac_mem_read(uint16_t addr)
{
    if (addr < 0x2000)
        return RAM[addr & 0x07FF];          /* 2 KB work RAM, four mirrors */
    if (addr < 0x8000)
        return neslib_read(addr);           /* PPU/APU/IO and open bus */
    if (addr < 0xC000)
        return zanac_bank_read(s_bank, addr);  /* switched window */
    return zanac_bank_read(ZANAC_FIXED_BANK, addr); /* bank 7 is fixed at $C000 */
}

void zanac_mem_write(uint16_t addr, uint8_t value)
{
    if (addr < 0x2000) {
        RAM[addr & 0x07FF] = value;
        return;
    }
    if (addr < 0x8000) {
        neslib_write(addr, value);          /* PPU/APU/IO registers */
        return;
    }
    /* $8000-$FFFF is the UxROM bank register.  No mapper emulation: the write
     * only selects which extracted bank the $8000-$BFFF window reads from. */
    zanac_switch_bank(value);
}

/* ------------------------------------------------------------ ALU ------- */

void set_nz(uint8_t value)
{
    F_Z = (uint8_t)(value == 0);
    F_N = (uint8_t)((value >> 7) & 1);
}

void adc(uint8_t value)
{
    unsigned sum;
    uint8_t  result;

    if (F_D) {
        /* Decimal mode.  Zanac never executes SED, so this path is provided
         * only so that the model is complete; N/V follow the binary result as
         * on the NMOS 6502. */
        unsigned lo = (unsigned)(A & 0x0F) + (value & 0x0F) + F_C;
        unsigned hi = (unsigned)(A >> 4) + (value >> 4);
        if (lo > 9) { lo += 6; hi++; }
        if (hi > 9) hi += 6;
        sum = (unsigned)A + value + F_C;
        result = (uint8_t)((hi << 4) | (lo & 0x0F));
        F_C = (uint8_t)(hi > 15);
        F_V = (uint8_t)((~(A ^ value) & (A ^ (uint8_t)sum) & 0x80) != 0);
        A = result;
        set_nz(A);
        F_Z = (uint8_t)(((uint8_t)sum) == 0);
        return;
    }

    sum = (unsigned)A + value + F_C;
    result = (uint8_t)(sum & 0xFF);
    F_C = (uint8_t)(sum > 0xFF);
    F_V = (uint8_t)((~(A ^ value) & (A ^ result) & 0x80) != 0);
    A = result;
    set_nz(A);
}

void sbc(uint8_t value)
{
    int diff;
    uint8_t result;

    if (F_D) {
        int borrow = 1 - (int)F_C;
        int lo = (int)(A & 0x0F) - (int)(value & 0x0F) - borrow;
        int hi = (int)(A >> 4) - (int)(value >> 4);
        if (lo < 0) { lo += 10; hi--; }
        if (hi < 0) hi += 10;
        diff = (int)A - (int)value - borrow;
        result = (uint8_t)((hi << 4) | (lo & 0x0F));
        F_C = (uint8_t)(diff >= 0);
        F_V = (uint8_t)(((A ^ value) & (A ^ (uint8_t)diff) & 0x80) != 0);
        A = result;
        set_nz(A);
        F_Z = (uint8_t)(((uint8_t)diff) == 0);
        return;
    }

    diff = (int)A - (int)value - (1 - (int)F_C);
    result = (uint8_t)(diff & 0xFF);
    F_C = (uint8_t)(diff >= 0);
    F_V = (uint8_t)(((A ^ value) & (A ^ result) & 0x80) != 0);
    A = result;
    set_nz(A);
}

void and_a(uint8_t value) { A = (uint8_t)(A & value); set_nz(A); }
void ora_a(uint8_t value) { A = (uint8_t)(A | value); set_nz(A); }
void eor_a(uint8_t value) { A = (uint8_t)(A ^ value); set_nz(A); }

void cmp(uint8_t reg, uint8_t value)
{
    int diff = (int)reg - (int)value;
    F_C = (uint8_t)(diff >= 0);
    set_nz((uint8_t)(diff & 0xFF));
}

void bit_m(uint8_t value)
{
    F_Z = (uint8_t)((A & value) == 0);
    F_N = (uint8_t)((value >> 7) & 1);
    F_V = (uint8_t)((value >> 6) & 1);
}

void shl_m(uint16_t addr, int kind)
{
    uint8_t v = MEM8(addr);
    uint8_t r;
    switch (kind) {
    case 0: /* ASL */
        F_C = (uint8_t)((v >> 7) & 1);
        r = (uint8_t)(v << 1);
        break;
    case 1: /* LSR */
        F_C = (uint8_t)(v & 1);
        r = (uint8_t)(v >> 1);
        break;
    case 2: /* ROL */
        r = (uint8_t)((v << 1) | F_C);
        F_C = (uint8_t)((v >> 7) & 1);
        break;
    default: /* ROR */
        r = (uint8_t)((v >> 1) | (F_C << 7));
        F_C = (uint8_t)(v & 1);
        break;
    }
    MEMW(addr, r);
    set_nz(r);
}

/* --------------------------------------------------- flags and stack ----- */

uint8_t pack_status(void)
{
    return (uint8_t)(0x20 | (F_C ? 0x01 : 0) | (F_Z ? 0x02 : 0) |
                     (F_I ? 0x04 : 0) | (F_D ? 0x08 : 0) | (F_V ? 0x40 : 0) |
                     (F_N ? 0x80 : 0));
}

void unpack_status(uint8_t value)
{
    F_C = (uint8_t)(value & 0x01);
    F_Z = (uint8_t)((value >> 1) & 1);
    F_I = (uint8_t)((value >> 2) & 1);
    F_D = (uint8_t)((value >> 3) & 1);
    F_V = (uint8_t)((value >> 6) & 1);
    F_N = (uint8_t)((value >> 7) & 1);
}

uint8_t peek_stack(uint8_t offset)
{
    return RAM[0x0100 + (uint8_t)(SP + offset)];
}

/* ------------------------------------------------- computed control flow -- */

#include "zanac.h"

/* Set by the PRINT_STRING / INLINE_CALL / SWITCH_CASE macros at a call site;
 * they replace the return address the original popped off the 6502 stack. */
uint16_t zanac_inline_block;
uint16_t zanac_inline_table;

/* Stack unwinding (see zanac_ram.h): the enemy dispatcher arms the jump and
 * the PLA/RTS routines jump back to it. */
jmp_buf zanac_unwind_env;
int     zanac_unwind_armed;
int     zanac_unwind_pending;

/* Save-state resume point (see zanac_ram.h): the per-frame loop the ported
 * code is in at a checkpoint.  E229.c, D2FD.c and D920.c set it on entry to the
 * level, pause and both menu loops; the loader restores it and then calls
 * zanac_state_resume(). */
uint8_t zanac_resume_kind = ZANAC_RESUME_MENU;
uint8_t zanac_resume_skip_setup;
uint8_t zanac_resume_pause;
uint8_t zanac_resume_ea0b_after_pause;
uint8_t zanac_resume_level_after_ea0b;
uint8_t zanac_resume_menu;
uint8_t zanac_resume_menu_select;

/* RTS to a computed address: pop the two bytes the original pushed and call
 * the ported procedure that lives there (RTS adds one to the popped address). */
void zanac_rts_dispatch(void)
{
    uint8_t lo = RAM[0x0100 + (uint8_t)(SP + 1)];
    uint8_t hi = RAM[0x0100 + (uint8_t)(SP + 2)];
    SP = (uint8_t)(SP + 2);
    zanac_call_at((uint16_t)(((uint16_t)hi << 8) | lo) + 1);
}
