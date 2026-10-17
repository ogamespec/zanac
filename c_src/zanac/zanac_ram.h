/* zanac_ram.h - the 6502 programmer's model and the memory map of the port.
 *
 * The port keeps the original machine model so that every procedure of the
 * assembly can be translated one statement at a time:
 *
 *   - the 2 KB work RAM is reached through `RAM[<6502 address>]`, and every
 *     original variable name (vars.h) is a macro for such an address;
 *   - A, X, Y, SP and the status flags live in globals, exactly like the
 *     registers of the 6502, so a callee can return values to its caller in
 *     them the way the original code does;
 *   - every instruction family has a macro named after the mnemonic.  The
 *     memory forms take an *address* (a variable macro is an address), the
 *     immediate forms are spelled with a trailing `I` (`LDAI(0x30)` is
 *     `LDA #$30`, `LDA(byte_30)` is `LDA $30`);
 *   - `RTS` is `return`, `JSR foo` is `foo()`, `BNE loc_XXXX` is
 *     `BNE(loc_XXXX)` with `loc_XXXX:` a C label.
 *
 * Address space (zanac_mem_read / zanac_mem_write):
 *   $0000-$1FFF  work RAM (2 KB, mirrored)
 *   $2000-$3FFF  PPU registers      -> neslib register calls
 *   $4000-$4017  APU / IO registers -> neslib register calls
 *   $4018-$7FFF  expansion / open bus
 *   $8000-$BFFF  the switched bank window (see zanac_banks.h)
 *   $C000-$FFFF  bank 7 (fixed)
 *
 * The UxROM bank register itself is *not* emulated: writing $8000-$FFFF (which
 * is what `switch_bank` does) only selects which extracted bank array the
 * $8000-$BFFF window reads from.
 */
#ifndef ZANAC_RAM_H
#define ZANAC_RAM_H

#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>

#include "neslib/neslib.h"

/* ------------------------------------------------------- registers ------ */
extern uint8_t  A;    /* accumulator */
extern uint8_t  X;    /* X index */
extern uint8_t  Y;    /* Y index */
extern uint8_t  SP;   /* stack pointer: the stack byte is RAM[0x0100 + SP] */

/* Status flags, non-zero when set (the 6502 "set" state). */
extern uint8_t  F_C;  /* carry */
extern uint8_t  F_Z;  /* zero */
extern uint8_t  F_N;  /* negative */
extern uint8_t  F_V;  /* overflow */
extern uint8_t  F_I;  /* IRQ disable */
extern uint8_t  F_D;  /* decimal (never enabled by this game: no SED anywhere) */

/* The 2 KB NES work RAM.  RAM[$0000..$07FF] is the whole variable space of the
 * game; the names in vars.h are addresses into it. */
extern uint8_t *RAM;

/* ------------------------------------------------------- memory --------- */
/* One read or write on the 6502 bus. */
uint8_t zanac_mem_read(uint16_t addr);
void    zanac_mem_write(uint16_t addr, uint8_t value);

#define MEM8(a)     zanac_mem_read((uint16_t)(a))
#define MEMW(a, v)  zanac_mem_write((uint16_t)(a), (uint8_t)(v))

/* Direct C-level read of a variable by address (used by hand-written glue). */
#define RD(a)       zanac_mem_read((uint16_t)(a))
#define WR(a, v)    zanac_mem_write((uint16_t)(a), (uint8_t)(v))

/* ------------------------------------------------- banked ROM window ---- */
/* $8000-$BFFF is the switched bank, $C000-$FFFF is bank 7.  The port reads the
 * extracted bank arrays (zanac_banks.c) instead of emulating the mapper. */
#define ZANAC_BANK_WINDOW 0x8000u
#define ZANAC_FIXED_BANK  7

uint8_t zanac_bank(void);                              /* current bank number */

/* Restore the bank latch from a save state (zanac/savestate.c).  Unlike
 * zanac_switch_bank() this is not a mapper write and is not recorded in the
 * diagnostic history. */
void    zanac_bank_restore(uint8_t bank);

/* The last 16 bank selections, oldest first (a debugging aid). */
extern uint8_t zanac_bank_history[16];
extern int     zanac_bank_history_pos;
void    zanac_switch_bank(uint8_t bank);               /* mapper write */
uint8_t zanac_bank_read(uint8_t bank, uint16_t addr);  /* addr is $8000-$FFFF */

/* ------------------------------------------------- save-state resume ---- */
/* Where the ported code is at a frame boundary.  A save state cannot carry the
 * C call stack, so it records the game's outer loop instead (the ported 6502
 * has no state outside RAM/registers, so the outer loops are restartable).  The
 * game sets this at the entry of every loop a save can be resumed in; see
 * zanac/savestate.c and E229.c. */
enum {
    ZANAC_RESUME_MENU        = 0,  /* main_menu_loop                       */
    ZANAC_RESUME_LEVEL       = 1,  /* sub_E81E's per-frame loop (loc_E880) */
    ZANAC_RESUME_LEVEL_SETUP = 2,  /* sub_E81E setup, before the loop      */
    ZANAC_RESUME_PAUSE       = 3,  /* sub_D3A8's pause loop (loc_D3B9)      */
    ZANAC_RESUME_MENU_SELECT = 4   /* show_main_menu's selection loop       */
                                   /* (loc_E190, after GAME START appears)  */
};
extern uint8_t zanac_resume_kind;
/* Set by the loader: sub_E81E() skips its setup and enters loc_E880. */
extern uint8_t zanac_resume_skip_setup;
/* Pause resume: sub_D3A8() starts at loc_D3B9 (its prologue already ran and
 * pushed byte_83/byte_17A in the restored state); sub_EA0B() then continues at
 * loc_EA25 (the code after its sub_D3A8 call); sub_E81E() continues at
 * loc_E880_after_ea0b (the code after its sub_EA0B call). */
extern uint8_t zanac_resume_pause;
extern uint8_t zanac_resume_ea0b_after_pause;
extern uint8_t zanac_resume_level_after_ea0b;
/* Menu resume: show_main_menu() starts at its loc_E080 demo loop or, for
 * ZANAC_RESUME_MENU_SELECT, at its loc_E190 GAME START/CONTINUE loop. */
extern uint8_t zanac_resume_menu;
extern uint8_t zanac_resume_menu_select;

/* ------------------------------------------------- addressing modes ----- */
#define ABSX(base)  ((uint16_t)((uint16_t)(base) + X))          /* abs,X */
#define ABSY(base)  ((uint16_t)((uint16_t)(base) + Y))          /* abs,Y */
#define ZPX(base)   ((uint8_t)((uint8_t)(base) + X))            /* zp,X  */
#define ZPY(base)   ((uint8_t)((uint8_t)(base) + Y))            /* zp,Y  */

/* (zp,X) and (zp),Y: zp is the address of the two-byte zero-page pointer. */
#define INDX(zp)    ((uint16_t)(RAM[(uint8_t)((zp) + X)] |          \
                                (RAM[(uint8_t)((zp) + X + 1)] << 8)))
#define INDY(zp)    ((uint16_t)((RAM[(zp)] | (RAM[(zp) + 1] << 8)) + Y))

/* ------------------------------------------------- ALU / flag helpers --- */
void set_nz(uint8_t value);                     /* Z and N from a result */

/* ------------------------------------------------------------------------ *
 *  Load / store
 * ------------------------------------------------------------------------ */
#define LDA(a)      do { A = MEM8(a); set_nz(A); } while (0)     /* LDA abs/abs,X/abs,Y/(zp),Y/zp */
#define LDX(a)      do { X = MEM8(a); set_nz(X); } while (0)
#define LDY(a)      do { Y = MEM8(a); set_nz(Y); } while (0)
#define LDAI(v)     do { A = (uint8_t)(v); set_nz(A); } while (0) /* LDA #imm */
#define LDXI(v)     do { X = (uint8_t)(v); set_nz(X); } while (0)
#define LDYI(v)     do { Y = (uint8_t)(v); set_nz(Y); } while (0)
#define STA(a)      do { MEMW(a, A); } while (0)                  /* STA anywhere */
#define STX(a)      do { MEMW(a, X); } while (0)
#define STY(a)      do { MEMW(a, Y); } while (0)

/* Register-to-register transfers. */
#define TAX()       do { X = A; set_nz(X); } while (0)
#define TAY()       do { Y = A; set_nz(Y); } while (0)
#define TXA()       do { A = X; set_nz(A); } while (0)
#define TYA()       do { A = Y; set_nz(A); } while (0)
#define TSX()       do { X = SP; set_nz(X); } while (0)
#define TXS()       do { SP = X; } while (0)

/* ------------------------------------------------------------------------ *
 *  Arithmetic and logic
 * ------------------------------------------------------------------------ */
#define ADCI(v)     adc((uint8_t)(v))       /* ADC #imm */
#define ADC(a)      adc(MEM8(a))            /* ADC abs/abs,X/(zp),Y */
#define SBCI(v)     sbc((uint8_t)(v))       /* SBC #imm */
#define SBC(a)      sbc(MEM8(a))
#define ANDI(v)     and_a((uint8_t)(v))     /* AND #imm */
#define AND(a)      and_a(MEM8(a))
#define ORAI(v)     ora_a((uint8_t)(v))     /* ORA #imm */
#define ORA(a)      ora_a(MEM8(a))
#define EORI(v)     eor_a((uint8_t)(v))     /* EOR #imm */
#define EOR(a)      eor_a(MEM8(a))
#define CMPI(v)     cmp(A, (uint8_t)(v))    /* CMP #imm */
#define CMP(a)      cmp(A, MEM8(a))
#define CPXI(v)     cmp(X, (uint8_t)(v))    /* CPX #imm */
#define CPX(a)      cmp(X, MEM8(a))
#define CPYI(v)     cmp(Y, (uint8_t)(v))    /* CPY #imm */
#define CPY(a)      cmp(Y, MEM8(a))
#define BIT(a)      bit_m(MEM8(a))          /* BIT abs: N,V from memory, Z from A&M */

void adc(uint8_t value);
void sbc(uint8_t value);
void and_a(uint8_t value);
void ora_a(uint8_t value);
void eor_a(uint8_t value);
void cmp(uint8_t reg, uint8_t value);   /* sets C, Z, N */
void bit_m(uint8_t value);

/* ------------------------------------------------------------------------ *
 *  Shifts / rotates / increments
 * ------------------------------------------------------------------------ */
#define ASL_A()     do { F_C = (A >> 7) & 1; A = (uint8_t)(A << 1); set_nz(A); } while (0)
#define LSR_A()     do { F_C = A & 1; A = (uint8_t)(A >> 1); set_nz(A); } while (0)
#define ROL_A()     do { uint8_t c = F_C; F_C = (A >> 7) & 1; A = (uint8_t)((A << 1) | c); set_nz(A); } while (0)
#define ROR_A()     do { uint8_t c = F_C; F_C = A & 1; A = (uint8_t)((A >> 1) | (c << 7)); set_nz(A); } while (0)
#define ASL(a)      shl_m(a, 0)
#define LSR(a)      shl_m(a, 1)
#define ROL(a)      shl_m(a, 2)
#define ROR(a)      shl_m(a, 3)
#define INC(a)      do { uint8_t v = (uint8_t)(MEM8(a) + 1); MEMW(a, v); set_nz(v); } while (0)
#define DEC(a)      do { uint8_t v = (uint8_t)(MEM8(a) - 1); MEMW(a, v); set_nz(v); } while (0)
#define INX()       do { X++; set_nz(X); } while (0)
#define INY()       do { Y++; set_nz(Y); } while (0)
#define DEX()       do { X--; set_nz(X); } while (0)
#define DEY()       do { Y--; set_nz(Y); } while (0)

/* Shift/rotate a memory location; kind: 0 ASL, 1 LSR, 2 ROL, 3 ROR. */
void shl_m(uint16_t addr, int kind);

/* ------------------------------------------------------------------------ *
 *  Flags and the stack
 * ------------------------------------------------------------------------ */
#define CLC()       do { F_C = 0; } while (0)
#define SEC()       do { F_C = 1; } while (0)
#define CLD()       do { F_D = 0; } while (0)
#define SED()       do { F_D = 1; } while (0)
#define CLI()       do { F_I = 0; } while (0)
#define SEI()       do { F_I = 1; } while (0)
#define CLV()       do { F_V = 0; } while (0)

/* The original's enemy_prng ($872B) mixes the 6502 *stack pointer* into every
 * byte it returns:
 *
 *     $872B  STX $10 / LDX $7E / SEC / ROL / EOR tab_18619_lo,X / ADC $10,X
 *     $8738  TSX / TXA / ...            <- the stack pointer is the entropy
 *
 * The port has no 6502 stack to read, so the caller supplies the value: every
 * call site passes the stack pointer measured on the real ROM at that JSR (see
 * tools/rom_stack.py).  Without it every enemy position, direction and shot
 * comes out different from the original.  The port's own stack pointer is
 * restored so that PHA/PLA elsewhere keep working. */
#define ENEMY_PRNG_SP(sp_value) \
    do { uint8_t zanac_sp_saved = SP; SP = (uint8_t)(sp_value); enemy_prng(); SP = zanac_sp_saved; } while (0)

#define PHA()       do { RAM[0x0100 + SP] = A; SP--; } while (0)
#define PLA()       do { SP++; A = RAM[0x0100 + SP]; set_nz(A); } while (0)
#define PHP()       do { RAM[0x0100 + SP] = pack_status(); SP--; } while (0)
#define PLP()       do { SP++; unpack_status(RAM[0x0100 + SP]); } while (0)
uint8_t pack_status(void);
void    unpack_status(uint8_t value);
uint8_t peek_stack(uint8_t offset);      /* RAM[0x0100 + SP + offset] */

/* ------------------------------------------------------------------------ *
 *  Branches: `BNE(loc_1234)` is `BNE loc_1234`
 * ------------------------------------------------------------------------ */
#define BCC(l)      do { if (!F_C) goto l; } while (0)
#define BCS(l)      do { if (F_C) goto l; } while (0)
#define BEQ(l)      do { if (F_Z) goto l; } while (0)
#define BNE(l)      do { if (!F_Z) goto l; } while (0)
#define BMI(l)      do { if (F_N) goto l; } while (0)
#define BPL(l)      do { if (!F_N) goto l; } while (0)
#define BVC(l)      do { if (!F_V) goto l; } while (0)
#define BVS(l)      do { if (F_V) goto l; } while (0)

/* The original sometimes ends a procedure with a branch into the `locret_*`
 * RTS that belongs to ANOTHER procedure (the shared-tail idiom).  A C label is
 * function scoped, so the ported equivalent of that branch is a return from the
 * current procedure - which is what the RTS did anyway, since it returns to
 * whoever called us.  The argument names the original target for review:
 *
 *      BEQ_RET(locret_187A5);      / * BEQ locret_187A5 * /
 */
#define BCC_RET(l)  do { (void)0; if (!F_C) { RTS(); } } while (0)
#define BCS_RET(l)  do { (void)0; if (F_C)  { RTS(); } } while (0)
#define BEQ_RET(l)  do { (void)0; if (F_Z)  { RTS(); } } while (0)
#define BNE_RET(l)  do { (void)0; if (!F_Z) { RTS(); } } while (0)
#define BMI_RET(l)  do { (void)0; if (F_N)  { RTS(); } } while (0)
#define BPL_RET(l)  do { (void)0; if (!F_N) { RTS(); } } while (0)
#define BVC_RET(l)  do { (void)0; if (!F_V) { RTS(); } } while (0)
#define BVS_RET(l)  do { (void)0; if (F_V)  { RTS(); } } while (0)

/* ------------------------------------------------------------------------ *
 *  Procedure call glue
 * ------------------------------------------------------------------------ */
#define RTS()       return

/* Calls into another procedure are plain C calls; a `JMP proc` at the end of a
 * procedure becomes `proc(); return;` (a tail call).  `JSR proc` is `proc();`.
 *
 * The original code has a handful of routines that pass parameters through the
 * code stream that follows the JSR (print_string, sub_C6CA, sub_C725).  The
 * port keeps those routines byte-for-byte, but their call sites load the
 * address of the inline parameter block into $16/$17 explicitly - see
 * c_src/PORTING.md section "Inline parameter blocks". */

/* ------------------------------------------------------------------------ *
 *  Stack unwinding
 *
 *  A handful of routines in the original end with `PLA / PLA / ... / RTS`: the
 *  pops discard the return address the caller's JSR pushed, so the final RTS
 *  returns to the *caller's caller*.  In the enemy processing cycle this means
 *  "abandon the current handler and continue with the next object slot".
 *
 *  A C call puts nothing on the emulated 6502 stack, so those pops cannot be
 *  translated literally.  The port arms a setjmp() at the slot loop
 *  (game_logic_processing) and the routines below longjmp() back to it, which
 *  discards exactly the same C frames the PLA/RTS sequence discarded on the
 *  6502:
 *
 *      enemy_erase_and_continue ($82FA)   enemy_erase(); ZANAC_UNWIND(); RTS();
 *      sub_1876A  loc_18779    ($8779)    enemy_erase(); ZANAC_UNWIND(); RTS();
 *      sub_1B87F  $B889 / $B89B           ZANAC_UNWIND();
 *      sub_19CAB  $19C96                  sets zanac_unwind_pending
 *      decrease_enemy_health   ($A2B6)    ZANAC_UNWIND(); RTS();
 *
 *  ZANAC_UNWIND() is a no-op when the dispatcher has not armed the jump (for
 *  example when such a routine is reached from the level-start sequence).
 * ------------------------------------------------------------------------ */
extern jmp_buf zanac_unwind_env;
extern int     zanac_unwind_armed;

#define ZANAC_UNWIND() do { if (zanac_unwind_armed) longjmp(zanac_unwind_env, 1); } while (0)

/* Routines whose PLA pair discards a frame but which keep executing first
 * (`sub_19CAB`) set this flag; every RTS of that routine then unwinds. */
extern int     zanac_unwind_pending;

/* ------------------------------------------------------------------------ *
 *  Busy-wait loops
 *
 *  The 6502 game has several loops that spin on state the NMI handler updates
 *  (cram_update_mode, byte_20, byte_9C, ...) or on the controller.  In hardware
 *  the NMI arrives by itself; in the port the engine has to be advanced, so
 *  those loops call ZANAC_WAIT_NMI() once per iteration.  It runs the engine up
 *  to the next vblank and dispatches the VBlank callback once, which is exactly
 *  what the spinning 6502 was waiting for:
 *
 *      loc_DE0E:                                       / * $DE0E * /
 *          LDA(cram_update_mode);                      / * LDA cram_update_mode * /
 *          BNE(loc_DE0E_wait);                         / * BNE loc_DE0E * /
 *          RTS();
 *      loc_DE0E_wait:
 *          ZANAC_WAIT_NMI();                           / * let the NMI clear it * /
 *          goto loc_DE0E;
 * ------------------------------------------------------------------------ */
#define ZANAC_WAIT_NMI() neslib_pump()

/* Initialise the machine model (called by the Reset callback before use). */
void zanac_init(void);

#endif /* ZANAC_RAM_H */
