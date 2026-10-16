/* cpu_ops.c - 6502 addressing/cycle tables and per-opcode semantics.
 *
 * Step tables are transcriptions of the "Execution frames" in
 * Specs/02-cpu-instructions.md section 4:
 *   read       : opcode, lo, hi, [indexed dummy read], data
 *   read (zp)  : opcode, zp, data
 *   RMW        : opcode, lo, hi, read, write-old, write-new
 *   store      : opcode, lo, hi, dummy read, write
 *   branch     : opcode, disp, dummy fetch, [extra read on a page cross]
 * Cycle costs in the table below are section 2 of the same file; the seven
 * "extra cycle" rules are section 3.
 */
#include <string.h>

#include "nes.h"
#include "cpu.h"
#include <stdio.h>

/* ----------------------------------------------------------- step tables -- */

/* Immediate: opcode, operand (the operation completes in the operand cycle,
 * where the byte is put on the bus).  2 cycles. */
static const uint8_t T_IMM[]        = { 0, CA_OPCODE, CA_FETCH_LO, CA_NONE };
/* Absent operation: still fetches the operand byte (they are real reads). */
static const uint8_t T_IMM_NOP[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_NONE };
/* Zero page */
static const uint8_t T_ZP[]         = { 0, CA_OPCODE, CA_FETCH_LO, CA_READ_ALU, CA_NONE };
static const uint8_t T_ZP_NOP[]     = { 0, CA_OPCODE, CA_FETCH_LO, CA_READ, CA_NONE };
static const uint8_t T_ZP_RMW[]     = { 0, CA_OPCODE, CA_FETCH_LO, CA_RMW_READ,
                                        CA_RMW_WRITE_OLD, CA_RMW_WRITE_NEW, CA_NONE };
static const uint8_t T_ZP_STORE[]   = { 0, CA_OPCODE, CA_FETCH_LO, CA_STORE, CA_NONE };
/* Zero page,X / zero page,Y.  The frame spends a real cycle reading the
 * un-indexed zero-page address before the (wrapped) indexed access:
 * opcode, zp, dummy read zp, read/write (zp+index). */
static const uint8_t T_ZPX[]        = { 0, CA_OPCODE, CA_FETCH_LO_IDX, CA_ZP_IDX_DUMMY,
                                        CA_READ_ALU, CA_NONE };
static const uint8_t T_ZPX_NOP[]    = { 0, CA_OPCODE, CA_FETCH_LO_IDX, CA_ZP_IDX_DUMMY,
                                        CA_READ, CA_NONE };
static const uint8_t T_ZPX_RMW[]    = { 0, CA_OPCODE, CA_FETCH_LO_IDX, CA_ZP_IDX_DUMMY,
                                        CA_RMW_READ, CA_RMW_WRITE_OLD, CA_RMW_WRITE_NEW,
                                        CA_NONE };
static const uint8_t T_ZPX_STORE[]  = { 0, CA_OPCODE, CA_FETCH_LO_IDX, CA_ZP_IDX_DUMMY,
                                        CA_STORE, CA_NONE };
/* Absolute */
static const uint8_t T_ABS[]        = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_READ_ALU, CA_NONE };
static const uint8_t T_ABS_NOP[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_READ, CA_NONE };
static const uint8_t T_ABS_RMW[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_RMW_READ,
                                        CA_RMW_WRITE_OLD, CA_RMW_WRITE_NEW, CA_NONE };
static const uint8_t T_ABS_STORE[]  = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_STORE, CA_NONE };
/* Absolute,X / absolute,Y (read): opcode, lo, hi, read at base+index without
 * the carry (also the data read when there is no carry), then the corrected
 * read on a page cross.  4 cycles, 5 on a cross. */
static const uint8_t T_ABX[]        = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_INDEXED_READ,
                                        CA_INDEXED_READ_FIX, CA_NONE };
static const uint8_t T_ABX_NOP[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_INDEXED_READ,
                                        CA_INDEXED_READ_FIX, CA_NONE };
/* Indexed RMW: the un-carried read is a real cycle, the original value is read
 * at the corrected address, then the two writes.  7 cycles, always. */
static const uint8_t T_ABX_RMW[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_INDEXED_DUMMY,
                                        CA_RMW_READ_IDX, CA_RMW_WRITE_OLD,
                                        CA_RMW_WRITE_NEW, CA_NONE };
/* Indexed store: the dummy read is always performed, never the extra cycle */
static const uint8_t T_ABX_STORE[]  = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_INDEXED_DUMMY,
                                        CA_STORE_IDX, CA_NONE };
/* (d,X): opcode, zp, dummy read zp, pointer low at (zp+X)&$FF, pointer high,
 * then the data access.  6 cycles. */
static const uint8_t T_IZX[]        = { 0, CA_OPCODE, CA_FETCH_LO_PTR, CA_IZX_DUMMY,
                                        CA_ZP_PTR_LO, CA_ZP_PTR_HI, CA_READ_ALU, CA_NONE };
static const uint8_t T_IZX_NOP[]    = { 0, CA_OPCODE, CA_FETCH_LO_PTR, CA_IZX_DUMMY,
                                        CA_ZP_PTR_LO, CA_ZP_PTR_HI, CA_READ, CA_NONE };
static const uint8_t T_IZX_STORE[]  = { 0, CA_OPCODE, CA_FETCH_LO_PTR, CA_IZX_DUMMY,
                                        CA_ZP_PTR_LO, CA_ZP_PTR_HI, CA_STORE, CA_NONE };
static const uint8_t T_IZX_RMW[]    = { 0, CA_OPCODE, CA_FETCH_LO_PTR, CA_IZX_DUMMY,
                                        CA_ZP_PTR_LO, CA_ZP_PTR_HI, CA_RMW_READ,
                                        CA_RMW_WRITE_OLD, CA_RMW_WRITE_NEW, CA_NONE };
/* (d),Y: opcode, zp, pointer low/high, then the indexed read frame.  5/6. */
static const uint8_t T_IZY[]        = { 0, CA_OPCODE, CA_FETCH_LO, CA_ZP_PTR_LO, CA_ZP_PTR_HI,
                                        CA_INDEXED_READ, CA_INDEXED_READ_FIX, CA_NONE };
static const uint8_t T_IZY_NOP[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_ZP_PTR_LO, CA_ZP_PTR_HI,
                                        CA_INDEXED_READ, CA_INDEXED_READ_FIX, CA_NONE };
static const uint8_t T_IZY_STORE[]  = { 0, CA_OPCODE, CA_FETCH_LO, CA_ZP_PTR_LO, CA_ZP_PTR_HI,
                                        CA_INDEXED_DUMMY, CA_STORE_IDX, CA_NONE };
static const uint8_t T_IZY_RMW[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_ZP_PTR_LO, CA_ZP_PTR_HI,
                                        CA_INDEXED_DUMMY, CA_RMW_READ_IDX,
                                        CA_RMW_WRITE_OLD, CA_RMW_WRITE_NEW, CA_NONE };
/* Implied / accumulator.  Cycle 2 is the dummy read of the byte after the
 * opcode (MOS6502_Opcodes/Readme.md: "the byte read in an instruction's last
 * cycle is the opcode of the next instruction"). */
static const uint8_t T_IMP[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_NONE };
static const uint8_t T_IMP_NOOP[]   = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_NONE };
/* Control transfers */
static const uint8_t T_JMP_ABS[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI, CA_NONE };
static const uint8_t T_JMP_IND[]    = { 0, CA_OPCODE, CA_FETCH_LO, CA_FETCH_HI,
                                        CA_IND_READ_LO, CA_IND_READ_HI, CA_NONE };
static const uint8_t T_JSR[]        = { 0, CA_OPCODE, CA_FETCH_TGT_LO, CA_DISCARD_FETCH,
                                        CA_PUSH_JSRH, CA_PUSH_JSRL, CA_FETCH_TGT_HI, CA_NONE };
static const uint8_t T_RTS[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_RTS_DUMMY,
                                        CA_RTS_PULL_LO, CA_RTS_PULL_HI, CA_RTS_FINISH, CA_NONE };
static const uint8_t T_RTI[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_RTI_DUMMY,
                                        CA_PULL_P_RTI, CA_PULL_LO, CA_PULL_HI, CA_NONE };
static const uint8_t T_BRK[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_PUSH_FRAME_PCH,
                                        CA_PUSH_FRAME_PCL, CA_PUSH_FRAME_P, CA_FRAME_VEC_LO,
                                        CA_FRAME_VEC_HI, CA_FRAME_FINISH, CA_NONE };
static const uint8_t T_PHA[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_PUSH_A, CA_NONE };
static const uint8_t T_PHP[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_PUSH_P, CA_NONE };
static const uint8_t T_PLA[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_PULL_LO, CA_PULL_A, CA_NONE };
static const uint8_t T_PLP[]        = { 0, CA_OPCODE, CA_DISCARD_FETCH, CA_PULL_LO, CA_PULL_P_PLP, CA_NONE };
/* Branch */
/* Branch: not taken ends after the displacement (2 cycles); taken adds the
 * fetch-and-discard cycle (3) and the page-cross read (4). */
static const uint8_t T_REL[]        = { 0, CA_OPCODE, CA_FETCH_LO, CA_BRANCH_TAKEN,
                                        CA_BRANCH_EXTRA, CA_NONE };

/* Interrupt frame (BRK / IRQ / NMI / RES).  02-cpu-instructions.md section 4:
 * cycles 1..2 fetch-and-discard (BRK increments PC on both, an interrupt does
 * not), 3..5 push PCH/PCL/P, 6..7 read the vector. */
const uint8_t cpu_irq_frame[9] = {
    0, CA_OPCODE, CA_DISCARD_FETCH, CA_PUSH_FRAME_PCH, CA_PUSH_FRAME_PCL,
    CA_PUSH_FRAME_P, CA_FRAME_VEC_LO, CA_FRAME_VEC_HI, CA_FRAME_FINISH
};
const uint8_t cpu_reset_frame[9] = {
    0, CA_OPCODE, CA_DISCARD_FETCH, CA_PUSH_FRAME_PCH, CA_PUSH_FRAME_PCL,
    CA_PUSH_FRAME_P, CA_FRAME_VEC_LO, CA_FRAME_VEC_HI, CA_FRAME_FINISH
};

/* ---------------------------------------------------------------- table --- */

#define E(name, mode, op, cyc, idx, pg, rmw, uno, tbl) \
    [0x00] = { 0 } /* placeholder to keep the macro list readable */

#define OP(name, mode, op, cyc, idx, pg, rmw, uno, tbl) \
    { name, AM_##mode, op, cyc, idx, pg, rmw, uno, tbl }

const OpInfo cpu_op_table[256] = {
    /* ---- row 0x00 ---- */
    OP("BRK", IMP, OP_BRK, 7, 0, 0, 0, 0, T_BRK),
    OP("ORA", IZX, OP_ORA, 6, 0, 0, 0, 0, T_IZX),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SLO", IZX, OP_SLO, 8, 0, 0, 1, 1, T_IZX_RMW),
    OP("NOP", ZP,  OP_NONE, 3, 0, 0, 0, 1, T_ZP_NOP),
    OP("ORA", ZP,  OP_ORA, 3, 0, 0, 0, 0, T_ZP),
    OP("ASL", ZP,  OP_ASL, 5, 0, 0, 1, 0, T_ZP_RMW),
    OP("SLO", ZP,  OP_SLO, 5, 0, 0, 1, 1, T_ZP_RMW),
    OP("PHP", IMP, OP_PHP, 3, 0, 0, 0, 0, T_PHP),
    OP("ORA", IMM, OP_ORA, 2, 0, 0, 0, 0, T_IMM),
    OP("ASL", ACC, OP_ASL_A, 2, 0, 0, 0, 0, T_IMP),
    OP("ANC", IMM, OP_ANC, 2, 0, 0, 0, 1, T_IMM),
    OP("NOP", ABS, OP_NONE, 4, 0, 0, 0, 1, T_ABS_NOP),
    OP("ORA", ABS, OP_ORA, 4, 0, 0, 0, 0, T_ABS),
    OP("ASL", ABS, OP_ASL, 6, 0, 0, 1, 0, T_ABS_RMW),
    OP("SLO", ABS, OP_SLO, 6, 0, 0, 1, 1, T_ABS_RMW),
    /* ---- row 0x10 ---- */
    OP("BPL", REL, OP_BPL, 2, 0, 0, 0, 0, T_REL),
    OP("ORA", IZY, OP_ORA, 5, 2, 1, 0, 0, T_IZY),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SLO", IZY, OP_SLO, 8, 2, 0, 1, 1, T_IZY_RMW),
    OP("NOP", ZPX, OP_NONE, 4, 1, 0, 0, 1, T_ZPX_NOP),
    OP("ORA", ZPX, OP_ORA, 4, 1, 0, 0, 0, T_ZPX),
    OP("ASL", ZPX, OP_ASL, 6, 1, 0, 1, 0, T_ZPX_RMW),
    OP("SLO", ZPX, OP_SLO, 6, 1, 0, 1, 1, T_ZPX_RMW),
    OP("CLC", IMP, OP_CLC, 2, 0, 0, 0, 0, T_IMP),
    OP("ORA", ABY, OP_ORA, 4, 2, 1, 0, 0, T_ABX),
    OP("NOP", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SLO", ABY, OP_SLO, 7, 2, 0, 1, 1, T_ABX_RMW),
    OP("NOP", ABX, OP_NONE, 4, 1, 1, 0, 1, T_ABX_NOP),
    OP("ORA", ABX, OP_ORA, 4, 1, 1, 0, 0, T_ABX),
    OP("ASL", ABX, OP_ASL, 7, 1, 0, 1, 0, T_ABX_RMW),
    OP("SLO", ABX, OP_SLO, 7, 1, 0, 1, 1, T_ABX_RMW),
    /* ---- row 0x20 ---- */
    OP("JSR", ABS, OP_JSR, 6, 0, 0, 0, 0, T_JSR),
    OP("AND", IZX, OP_AND, 6, 0, 0, 0, 0, T_IZX),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("RLA", IZX, OP_RLA, 8, 0, 0, 1, 1, T_IZX_RMW),
    OP("BIT", ZP,  OP_BIT, 3, 0, 0, 0, 0, T_ZP),
    OP("AND", ZP,  OP_AND, 3, 0, 0, 0, 0, T_ZP),
    OP("ROL", ZP,  OP_ROL, 5, 0, 0, 1, 0, T_ZP_RMW),
    OP("RLA", ZP,  OP_RLA, 5, 0, 0, 1, 1, T_ZP_RMW),
    OP("PLP", IMP, OP_PLP, 4, 0, 0, 0, 0, T_PLP),
    OP("AND", IMM, OP_AND, 2, 0, 0, 0, 0, T_IMM),
    OP("ROL", ACC, OP_ROL_A, 2, 0, 0, 0, 0, T_IMP),
    OP("ANC", IMM, OP_ANC, 2, 0, 0, 0, 1, T_IMM),
    OP("BIT", ABS, OP_BIT, 4, 0, 0, 0, 0, T_ABS),
    OP("AND", ABS, OP_AND, 4, 0, 0, 0, 0, T_ABS),
    OP("ROL", ABS, OP_ROL, 6, 0, 0, 1, 0, T_ABS_RMW),
    OP("RLA", ABS, OP_RLA, 6, 0, 0, 1, 1, T_ABS_RMW),
    /* ---- row 0x30 ---- */
    OP("BMI", REL, OP_BMI, 2, 0, 0, 0, 0, T_REL),
    OP("AND", IZY, OP_AND, 5, 2, 1, 0, 0, T_IZY),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("RLA", IZY, OP_RLA, 8, 2, 0, 1, 1, T_IZY_RMW),
    OP("NOP", ZPX, OP_NONE, 4, 1, 0, 0, 1, T_ZPX_NOP),
    OP("AND", ZPX, OP_AND, 4, 1, 0, 0, 0, T_ZPX),
    OP("ROL", ZPX, OP_ROL, 6, 1, 0, 1, 0, T_ZPX_RMW),
    OP("RLA", ZPX, OP_RLA, 6, 1, 0, 1, 1, T_ZPX_RMW),
    OP("SEC", IMP, OP_SEC, 2, 0, 0, 0, 0, T_IMP),
    OP("AND", ABY, OP_AND, 4, 2, 1, 0, 0, T_ABX),
    OP("NOP", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("RLA", ABY, OP_RLA, 7, 2, 0, 1, 1, T_ABX_RMW),
    OP("NOP", ABX, OP_NONE, 4, 1, 1, 0, 1, T_ABX_NOP),
    OP("AND", ABX, OP_AND, 4, 1, 1, 0, 0, T_ABX),
    OP("ROL", ABX, OP_ROL, 7, 1, 0, 1, 0, T_ABX_RMW),
    OP("RLA", ABX, OP_RLA, 7, 1, 0, 1, 1, T_ABX_RMW),
    /* ---- row 0x40 ---- */
    OP("RTI", IMP, OP_RTI, 6, 0, 0, 0, 0, T_RTI),
    OP("EOR", IZX, OP_EOR, 6, 0, 0, 0, 0, T_IZX),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SRE", IZX, OP_SRE, 8, 0, 0, 1, 1, T_IZX_RMW),
    OP("NOP", ZP,  OP_NONE, 3, 0, 0, 0, 1, T_ZP_NOP),
    OP("EOR", ZP,  OP_EOR, 3, 0, 0, 0, 0, T_ZP),
    OP("LSR", ZP,  OP_LSR, 5, 0, 0, 1, 0, T_ZP_RMW),
    OP("SRE", ZP,  OP_SRE, 5, 0, 0, 1, 1, T_ZP_RMW),
    OP("PHA", IMP, OP_PHA, 3, 0, 0, 0, 0, T_PHA),
    OP("EOR", IMM, OP_EOR, 2, 0, 0, 0, 0, T_IMM),
    OP("LSR", ACC, OP_LSR_A, 2, 0, 0, 0, 0, T_IMP),
    OP("ALR", IMM, OP_ALR, 2, 0, 0, 0, 1, T_IMM),
    OP("JMP", ABS, OP_JMP, 3, 0, 0, 0, 0, T_JMP_ABS),
    OP("EOR", ABS, OP_EOR, 4, 0, 0, 0, 0, T_ABS),
    OP("LSR", ABS, OP_LSR, 6, 0, 0, 1, 0, T_ABS_RMW),
    OP("SRE", ABS, OP_SRE, 6, 0, 0, 1, 1, T_ABS_RMW),
    /* ---- row 0x50 ---- */
    OP("BVC", REL, OP_BVC, 2, 0, 0, 0, 0, T_REL),
    OP("EOR", IZY, OP_EOR, 5, 2, 1, 0, 0, T_IZY),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SRE", IZY, OP_SRE, 8, 2, 0, 1, 1, T_IZY_RMW),
    OP("NOP", ZPX, OP_NONE, 4, 1, 0, 0, 1, T_ZPX_NOP),
    OP("EOR", ZPX, OP_EOR, 4, 1, 0, 0, 0, T_ZPX),
    OP("LSR", ZPX, OP_LSR, 6, 1, 0, 1, 0, T_ZPX_RMW),
    OP("SRE", ZPX, OP_SRE, 6, 1, 0, 1, 1, T_ZPX_RMW),
    OP("CLI", IMP, OP_CLI, 2, 0, 0, 0, 0, T_IMP),
    OP("EOR", ABY, OP_EOR, 4, 2, 1, 0, 0, T_ABX),
    OP("NOP", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SRE", ABY, OP_SRE, 7, 2, 0, 1, 1, T_ABX_RMW),
    OP("NOP", ABX, OP_NONE, 4, 1, 1, 0, 1, T_ABX_NOP),
    OP("EOR", ABX, OP_EOR, 4, 1, 1, 0, 0, T_ABX),
    OP("LSR", ABX, OP_LSR, 7, 1, 0, 1, 0, T_ABX_RMW),
    OP("SRE", ABX, OP_SRE, 7, 1, 0, 1, 1, T_ABX_RMW),
    /* ---- row 0x60 ---- */
    OP("RTS", IMP, OP_RTS, 6, 0, 0, 0, 0, T_RTS),
    OP("ADC", IZX, OP_ADC, 6, 0, 0, 0, 0, T_IZX),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("RRA", IZX, OP_RRA, 8, 0, 0, 1, 1, T_IZX_RMW),
    OP("NOP", ZP,  OP_NONE, 3, 0, 0, 0, 1, T_ZP_NOP),
    OP("ADC", ZP,  OP_ADC, 3, 0, 0, 0, 0, T_ZP),
    OP("ROR", ZP,  OP_ROR, 5, 0, 0, 1, 0, T_ZP_RMW),
    OP("RRA", ZP,  OP_RRA, 5, 0, 0, 1, 1, T_ZP_RMW),
    OP("PLA", IMP, OP_PLA, 4, 0, 0, 0, 0, T_PLA),
    OP("ADC", IMM, OP_ADC, 2, 0, 0, 0, 0, T_IMM),
    OP("ROR", ACC, OP_ROR_A, 2, 0, 0, 0, 0, T_IMP),
    OP("ARR", IMM, OP_ARR, 2, 0, 0, 0, 1, T_IMM),
    OP("JMP", IND, OP_JMP, 5, 0, 0, 0, 0, T_JMP_IND),
    OP("ADC", ABS, OP_ADC, 4, 0, 0, 0, 0, T_ABS),
    OP("ROR", ABS, OP_ROR, 6, 0, 0, 1, 0, T_ABS_RMW),
    OP("RRA", ABS, OP_RRA, 6, 0, 0, 1, 1, T_ABS_RMW),
    /* ---- row 0x70 ---- */
    OP("BVS", REL, OP_BVS, 2, 0, 0, 0, 0, T_REL),
    OP("ADC", IZY, OP_ADC, 5, 2, 1, 0, 0, T_IZY),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("RRA", IZY, OP_RRA, 8, 2, 0, 1, 1, T_IZY_RMW),
    OP("NOP", ZPX, OP_NONE, 4, 1, 0, 0, 1, T_ZPX_NOP),
    OP("ADC", ZPX, OP_ADC, 4, 1, 0, 0, 0, T_ZPX),
    OP("ROR", ZPX, OP_ROR, 6, 1, 0, 1, 0, T_ZPX_RMW),
    OP("RRA", ZPX, OP_RRA, 6, 1, 0, 1, 1, T_ZPX_RMW),
    OP("SEI", IMP, OP_SEI, 2, 0, 0, 0, 0, T_IMP),
    OP("ADC", ABY, OP_ADC, 4, 2, 1, 0, 0, T_ABX),
    OP("NOP", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("RRA", ABY, OP_RRA, 7, 2, 0, 1, 1, T_ABX_RMW),
    OP("NOP", ABX, OP_NONE, 4, 1, 1, 0, 1, T_ABX_NOP),
    OP("ADC", ABX, OP_ADC, 4, 1, 1, 0, 0, T_ABX),
    OP("ROR", ABX, OP_ROR, 7, 1, 0, 1, 0, T_ABX_RMW),
    OP("RRA", ABX, OP_RRA, 7, 1, 0, 1, 1, T_ABX_RMW),
    /* ---- row 0x80 ---- */
    OP("NOP", IMM, OP_NONE, 2, 0, 0, 0, 1, T_IMM_NOP),
    OP("STA", IZX, OP_STA, 6, 0, 0, 0, 0, T_IZX_STORE),
    OP("NOP", IMM, OP_NONE, 2, 0, 0, 0, 1, T_IMM_NOP),
    OP("SAX", IZX, OP_SAX, 6, 0, 0, 0, 1, T_IZX_STORE),
    OP("STY", ZP,  OP_STY, 3, 0, 0, 0, 0, T_ZP_STORE),
    OP("STA", ZP,  OP_STA, 3, 0, 0, 0, 0, T_ZP_STORE),
    OP("STX", ZP,  OP_STX, 3, 0, 0, 0, 0, T_ZP_STORE),
    OP("SAX", ZP,  OP_SAX, 3, 0, 0, 0, 1, T_ZP_STORE),
    OP("DEY", IMP, OP_DEY, 2, 0, 0, 0, 0, T_IMP),
    OP("NOP", IMM, OP_NONE, 2, 0, 0, 0, 1, T_IMM_NOP),
    OP("TXA", IMP, OP_TXA, 2, 0, 0, 0, 0, T_IMP),
    OP("XAA", IMM, OP_XAA, 2, 0, 0, 0, 1, T_IMM),
    OP("STY", ABS, OP_STY, 4, 0, 0, 0, 0, T_ABS_STORE),
    OP("STA", ABS, OP_STA, 4, 0, 0, 0, 0, T_ABS_STORE),
    OP("STX", ABS, OP_STX, 4, 0, 0, 0, 0, T_ABS_STORE),
    OP("SAX", ABS, OP_SAX, 4, 0, 0, 0, 1, T_ABS_STORE),
    /* ---- row 0x90 ---- */
    OP("BCC", REL, OP_BCC, 2, 0, 0, 0, 0, T_REL),
    OP("STA", IZY, OP_STA, 6, 2, 0, 0, 0, T_IZY_STORE),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SHA", IZY, OP_SHA, 6, 2, 0, 0, 1, T_IZY_STORE),
    OP("STY", ZPX, OP_STY, 4, 1, 0, 0, 0, T_ZPX_STORE),
    OP("STA", ZPX, OP_STA, 4, 1, 0, 0, 0, T_ZPX_STORE),
    OP("STX", ZPY, OP_STX, 4, 2, 0, 0, 0, T_ZPX_STORE),
    OP("SAX", ZPY, OP_SAX, 4, 2, 0, 0, 1, T_ZPX_STORE),
    OP("TYA", IMP, OP_TYA, 2, 0, 0, 0, 0, T_IMP),
    OP("STA", ABY, OP_STA, 5, 2, 0, 0, 0, T_ABX_STORE),
    OP("TXS", IMP, OP_TXS, 2, 0, 0, 0, 0, T_IMP),
    OP("SHS", ABY, OP_SHS, 5, 2, 0, 0, 1, T_ABX_STORE),
    OP("SHY", ABX, OP_SHY, 5, 1, 0, 0, 1, T_ABX_STORE),
    OP("STA", ABX, OP_STA, 5, 1, 0, 0, 0, T_ABX_STORE),
    OP("SHX", ABY, OP_SHX, 5, 2, 0, 0, 1, T_ABX_STORE),
    OP("SHA", ABY, OP_SHA, 5, 2, 0, 0, 1, T_ABX_STORE),
    /* ---- row 0xA0 ---- */
    OP("LDY", IMM, OP_LDY, 2, 0, 0, 0, 0, T_IMM),
    OP("LDA", IZX, OP_LDA, 6, 0, 0, 0, 0, T_IZX),
    OP("LDX", IMM, OP_LDX, 2, 0, 0, 0, 0, T_IMM),
    OP("LAX", IZX, OP_LAX, 6, 0, 0, 0, 1, T_IZX),
    OP("LDY", ZP,  OP_LDY, 3, 0, 0, 0, 0, T_ZP),
    OP("LDA", ZP,  OP_LDA, 3, 0, 0, 0, 0, T_ZP),
    OP("LDX", ZP,  OP_LDX, 3, 0, 0, 0, 0, T_ZP),
    OP("LAX", ZP,  OP_LAX, 3, 0, 0, 0, 1, T_ZP),
    OP("TAY", IMP, OP_TAY, 2, 0, 0, 0, 0, T_IMP),
    OP("LDA", IMM, OP_LDA, 2, 0, 0, 0, 0, T_IMM),
    OP("TAX", IMP, OP_TAX, 2, 0, 0, 0, 0, T_IMP),
    OP("LAX", IMM, OP_LAX, 2, 0, 0, 0, 1, T_IMM),
    OP("LDY", ABS, OP_LDY, 4, 0, 0, 0, 0, T_ABS),
    OP("LDA", ABS, OP_LDA, 4, 0, 0, 0, 0, T_ABS),
    OP("LDX", ABS, OP_LDX, 4, 0, 0, 0, 0, T_ABS),
    OP("LAX", ABS, OP_LAX, 4, 0, 0, 0, 1, T_ABS),
    /* ---- row 0xB0 ---- */
    OP("BCS", REL, OP_BCS, 2, 0, 0, 0, 0, T_REL),
    OP("LDA", IZY, OP_LDA, 5, 2, 1, 0, 0, T_IZY),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("LAX", IZY, OP_LAX, 5, 2, 1, 0, 1, T_IZY),
    OP("LDY", ZPX, OP_LDY, 4, 1, 0, 0, 0, T_ZPX),
    OP("LDA", ZPX, OP_LDA, 4, 1, 0, 0, 0, T_ZPX),
    OP("LDX", ZPY, OP_LDX, 4, 2, 0, 0, 0, T_ZPX),
    OP("LAX", ZPY, OP_LAX, 4, 2, 0, 0, 1, T_ZPX),
    OP("CLV", IMP, OP_CLV, 2, 0, 0, 0, 0, T_IMP),
    OP("LDA", ABY, OP_LDA, 4, 2, 1, 0, 0, T_ABX),
    OP("TSX", IMP, OP_TSX, 2, 0, 0, 0, 0, T_IMP),
    OP("LAS", ABY, OP_LAS, 4, 2, 1, 0, 1, T_ABX),
    OP("LDY", ABX, OP_LDY, 4, 1, 1, 0, 0, T_ABX),
    OP("LDA", ABX, OP_LDA, 4, 1, 1, 0, 0, T_ABX),
    OP("LDX", ABY, OP_LDX, 4, 2, 1, 0, 0, T_ABX),
    OP("LAX", ABY, OP_LAX, 4, 2, 1, 0, 1, T_ABX),
    /* ---- row 0xC0 ---- */
    OP("CPY", IMM, OP_CPY, 2, 0, 0, 0, 0, T_IMM),
    OP("CMP", IZX, OP_CMP, 6, 0, 0, 0, 0, T_IZX),
    OP("NOP", IMM, OP_NONE, 2, 0, 0, 0, 1, T_IMM_NOP),
    OP("DCP", IZX, OP_DCP, 8, 0, 0, 1, 1, T_IZX_RMW),
    OP("CPY", ZP,  OP_CPY, 3, 0, 0, 0, 0, T_ZP),
    OP("CMP", ZP,  OP_CMP, 3, 0, 0, 0, 0, T_ZP),
    OP("DEC", ZP,  OP_DEC, 5, 0, 0, 1, 0, T_ZP_RMW),
    OP("DCP", ZP,  OP_DCP, 5, 0, 0, 1, 1, T_ZP_RMW),
    OP("INY", IMP, OP_INY, 2, 0, 0, 0, 0, T_IMP),
    OP("CMP", IMM, OP_CMP, 2, 0, 0, 0, 0, T_IMM),
    OP("DEX", IMP, OP_DEX, 2, 0, 0, 0, 0, T_IMP),
    OP("AXS", IMM, OP_AXS, 2, 0, 0, 0, 1, T_IMM),
    OP("CPY", ABS, OP_CPY, 4, 0, 0, 0, 0, T_ABS),
    OP("CMP", ABS, OP_CMP, 4, 0, 0, 0, 0, T_ABS),
    OP("DEC", ABS, OP_DEC, 6, 0, 0, 1, 0, T_ABS_RMW),
    OP("DCP", ABS, OP_DCP, 6, 0, 0, 1, 1, T_ABS_RMW),
    /* ---- row 0xD0 ---- */
    OP("BNE", REL, OP_BNE, 2, 0, 0, 0, 0, T_REL),
    OP("CMP", IZY, OP_CMP, 5, 2, 1, 0, 0, T_IZY),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("DCP", IZY, OP_DCP, 8, 2, 0, 1, 1, T_IZY_RMW),
    OP("NOP", ZPX, OP_NONE, 4, 1, 0, 0, 1, T_ZPX_NOP),
    OP("CMP", ZPX, OP_CMP, 4, 1, 0, 0, 0, T_ZPX),
    OP("DEC", ZPX, OP_DEC, 6, 1, 0, 1, 0, T_ZPX_RMW),
    OP("DCP", ZPX, OP_DCP, 6, 1, 0, 1, 1, T_ZPX_RMW),
    OP("CLD", IMP, OP_CLD, 2, 0, 0, 0, 0, T_IMP),
    OP("CMP", ABY, OP_CMP, 4, 2, 1, 0, 0, T_ABX),
    OP("NOP", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("DCP", ABY, OP_DCP, 7, 2, 0, 1, 1, T_ABX_RMW),
    OP("NOP", ABX, OP_NONE, 4, 1, 1, 0, 1, T_ABX_NOP),
    OP("CMP", ABX, OP_CMP, 4, 1, 1, 0, 0, T_ABX),
    OP("DEC", ABX, OP_DEC, 7, 1, 0, 1, 0, T_ABX_RMW),
    OP("DCP", ABX, OP_DCP, 7, 1, 0, 1, 1, T_ABX_RMW),
    /* ---- row 0xE0 ---- */
    OP("CPX", IMM, OP_CPX, 2, 0, 0, 0, 0, T_IMM),
    OP("SBC", IZX, OP_SBC, 6, 0, 0, 0, 0, T_IZX),
    OP("NOP", IMM, OP_NONE, 2, 0, 0, 0, 1, T_IMM_NOP),
    OP("ISB", IZX, OP_ISB, 8, 0, 0, 1, 1, T_IZX_RMW),
    OP("CPX", ZP,  OP_CPX, 3, 0, 0, 0, 0, T_ZP),
    OP("SBC", ZP,  OP_SBC, 3, 0, 0, 0, 0, T_ZP),
    OP("INC", ZP,  OP_INC, 5, 0, 0, 1, 0, T_ZP_RMW),
    OP("ISB", ZP,  OP_ISB, 5, 0, 0, 1, 1, T_ZP_RMW),
    OP("INX", IMP, OP_INX, 2, 0, 0, 0, 0, T_IMP),
    OP("SBC", IMM, OP_SBC, 2, 0, 0, 0, 0, T_IMM),
    OP("NOP", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("SBC", IMM, OP_SBC_EB, 2, 0, 0, 0, 1, T_IMM),
    OP("CPX", ABS, OP_CPX, 4, 0, 0, 0, 0, T_ABS),
    OP("SBC", ABS, OP_SBC, 4, 0, 0, 0, 0, T_ABS),
    OP("INC", ABS, OP_INC, 6, 0, 0, 1, 0, T_ABS_RMW),
    OP("ISB", ABS, OP_ISB, 6, 0, 0, 1, 1, T_ABS_RMW),
    /* ---- row 0xF0 ---- */
    OP("BEQ", REL, OP_BEQ, 2, 0, 0, 0, 0, T_REL),
    OP("SBC", IZY, OP_SBC, 5, 2, 1, 0, 0, T_IZY),
    OP("KIL", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("ISB", IZY, OP_ISB, 8, 2, 0, 1, 1, T_IZY_RMW),
    OP("NOP", ZPX, OP_NONE, 4, 1, 0, 0, 1, T_ZPX_NOP),
    OP("SBC", ZPX, OP_SBC, 4, 1, 0, 0, 0, T_ZPX),
    OP("INC", ZPX, OP_INC, 6, 1, 0, 1, 0, T_ZPX_RMW),
    OP("ISB", ZPX, OP_ISB, 6, 1, 0, 1, 1, T_ZPX_RMW),
    OP("SED", IMP, OP_SED, 2, 0, 0, 0, 0, T_IMP),
    OP("SBC", ABY, OP_SBC, 4, 2, 1, 0, 0, T_ABX),
    OP("NOP", IMP, OP_NONE, 2, 0, 0, 0, 1, T_IMP_NOOP),
    OP("ISB", ABY, OP_ISB, 7, 2, 0, 1, 1, T_ABX_RMW),
    OP("NOP", ABX, OP_NONE, 4, 1, 1, 0, 1, T_ABX_NOP),
    OP("SBC", ABX, OP_SBC, 4, 1, 1, 0, 0, T_ABX),
    OP("INC", ABX, OP_INC, 7, 1, 0, 1, 0, T_ABX_RMW),
    OP("ISB", ABX, OP_ISB, 7, 1, 0, 1, 1, T_ABX_RMW)
};

/* -------------------------------------------------------------- helpers --- */

uint8_t cpu_index_value(Cpu *c)
{
    switch (c->info->add_index) {
        case 1: return c->x;
        case 2: return c->y;
        default: return 0;
    }
}

uint8_t cpu_store_value(Cpu *c)
{
    switch (c->info->op) {
        case OP_STA: return c->a;
        case OP_STX: return c->x;
        case OP_STY: return c->y;
        case OP_SAX: return (uint8_t)(c->a & c->x);
        default:     return c->data;
    }
}

/* Unstable "store high byte" family: SHA ($93/$9F), SHS ($9B), SHY ($9C) and
 * SHX ($9E).  These mirror the 2A03's SH* stores: the value written is the
 * opcode's register ANDed with the target's high byte + 1, and when the index
 * carries out of the low byte the address high byte itself is ANDed with the
 * register (the "high byte corruption").  SHS also drops the AND into S.
 * The behaviour is out of scope of 02-cpu-instructions.md section 6; it is the
 * Mesen2/AccuracyCoin "behaviour 1" variant (see SPEC-GAPS.md).  Returns false
 * for every other opcode so CA_STORE_IDX can fall through to the normal store. */
bool cpu_prepare_sh_store(Cpu *c)
{
    uint8_t vreg;

    switch (c->info->op) {
        case OP_SHA: vreg = (uint8_t)(c->a & c->x); break;
        case OP_SHS: vreg = (uint8_t)(c->a & c->x); c->s = vreg; break;
        case OP_SHX: vreg = c->x; break;
        case OP_SHY: vreg = c->y; break;
        default:     return false;
    }

    if (c->pages) {
        uint16_t hi = (uint16_t)((c->ea >> 8) & vreg);
        c->ea = (uint16_t)((c->ea & 0x00FF) | (hi << 8));
    }
    /* A DMC sample fetch that halted the CPU around the dummy-read cycle pulls
     * the data bus low, so the AND with (H+1) never happens (AccuracyCoin
     * "SH*" sub-test 7; Mesen2 SyaSxaAxa).  The fetch has to land within a few
     * cycles of the write; the window covers the halt and the store cycle. */
    {
        bool dma_near = c->dmc_dma_seen ||
            (c->nes->dmc_last_cycle != 0 &&
             c->nes->cpu_cycles - c->nes->dmc_last_cycle <= 12);
        c->store_scratch = dma_near
            ? vreg
            : (uint8_t)(vreg & (uint8_t)((c->base >> 8) + 1));
    }
    return true;
}

bool cpu_branch_taken(Cpu *c)
{
    switch (c->info->op) {
        case OP_BCC: return !cpu_flag(c, FLAG_C);
        case OP_BCS: return  cpu_flag(c, FLAG_C);
        case OP_BEQ: return  cpu_flag(c, FLAG_Z);
        case OP_BNE: return !cpu_flag(c, FLAG_Z);
        case OP_BMI: return  cpu_flag(c, FLAG_N);
        case OP_BPL: return !cpu_flag(c, FLAG_N);
        case OP_BVC: return !cpu_flag(c, FLAG_V);
        case OP_BVS: return  cpu_flag(c, FLAG_V);
        default:     return false;
    }
}

/* ------------------------------------------------------ operation commit -- */

void cpu_commit(Cpu *c)
{
    uint8_t v = c->data;

    switch (c->info->op) {
    case OP_LDA: c->a = v; cpu_set_nz(c, v); break;
    case OP_LDX: c->x = v; cpu_set_nz(c, v); break;
    case OP_LDY: c->y = v; cpu_set_nz(c, v); break;
    /* LAE/LAS: the read byte is ANDed with S and dropped into A, X and S. */
    case OP_LAS: {
        uint8_t r = (uint8_t)(c->s & v);
        c->a = r; c->x = r; c->s = r;
        cpu_set_nz(c, r);
        break;
    }

    case OP_TAX: c->x = c->a; cpu_set_nz(c, c->x); break;
    case OP_TAY: c->y = c->a; cpu_set_nz(c, c->y); break;
    case OP_TXA: c->a = c->x; cpu_set_nz(c, c->a); break;
    case OP_TYA: c->a = c->y; cpu_set_nz(c, c->a); break;
    case OP_TSX: c->x = c->s; cpu_set_nz(c, c->x); break;
    case OP_TXS: c->s = c->x; break; /* never reaches DB, so no flag change */

    case OP_AND: c->a &= v; cpu_set_nz(c, c->a); break;
    case OP_ORA: c->a |= v; cpu_set_nz(c, c->a); break;
    case OP_EOR: c->a ^= v; cpu_set_nz(c, c->a); break;
    case OP_BIT:
        cpu_set_flag(c, FLAG_Z, (c->a & v) == 0);
        c->p = (uint8_t)((c->p & (uint8_t)~(FLAG_N | FLAG_V)) | (v & (FLAG_N | FLAG_V)));
        break;

    case OP_ADC: {
        unsigned r = (unsigned)c->a + v + (cpu_flag(c, FLAG_C) ? 1u : 0u);
        cpu_set_flag(c, FLAG_C, r > 0xFF);
        cpu_set_flag(c, FLAG_V, ((c->a ^ (uint8_t)r) & (v ^ (uint8_t)r) & 0x80) != 0);
        c->a = (uint8_t)r;
        cpu_set_nz(c, c->a);
        break;
    }
    case OP_SBC: case OP_SBC_EB: {
        unsigned r = (unsigned)c->a + (v ^ 0xFF) + (cpu_flag(c, FLAG_C) ? 1u : 0u);
        cpu_set_flag(c, FLAG_C, r > 0xFF);
        cpu_set_flag(c, FLAG_V, ((c->a ^ (uint8_t)r) & ((v ^ 0xFF) ^ (uint8_t)r) & 0x80) != 0);
        c->a = (uint8_t)r;
        cpu_set_nz(c, c->a);
        break;
    }
    case OP_CMP: case OP_CPX: case OP_CPY: {
        uint8_t reg = (c->info->op == OP_CMP) ? c->a
                    : (c->info->op == OP_CPX ? c->x : c->y);
        cpu_set_flag(c, FLAG_C, reg >= v);
        cpu_set_nz(c, (uint8_t)(reg - v));
        break;
    }

    case OP_INX: c->x++; cpu_set_nz(c, c->x); break;
    case OP_INY: c->y++; cpu_set_nz(c, c->y); break;
    case OP_DEX: c->x--; cpu_set_nz(c, c->x); break;
    case OP_DEY: c->y--; cpu_set_nz(c, c->y); break;

    /* Accumulator shifts (02-cpu-instructions.md section 5). */
    case OP_ASL_A:
        cpu_set_flag(c, FLAG_C, (c->a & 0x80) != 0);
        c->a = (uint8_t)(c->a << 1);
        cpu_set_nz(c, c->a);
        break;
    case OP_LSR_A:
        cpu_set_flag(c, FLAG_C, (c->a & 1) != 0);
        c->a = (uint8_t)(c->a >> 1);
        cpu_set_nz(c, c->a);
        break;
    case OP_ROL_A: {
        uint8_t carry = cpu_flag(c, FLAG_C) ? 1 : 0;
        cpu_set_flag(c, FLAG_C, (c->a & 0x80) != 0);
        c->a = (uint8_t)((c->a << 1) | carry);
        cpu_set_nz(c, c->a);
        break;
    }
    case OP_ROR_A: {
        uint8_t carry = cpu_flag(c, FLAG_C) ? 0x80 : 0;
        cpu_set_flag(c, FLAG_C, (c->a & 1) != 0);
        c->a = (uint8_t)((c->a >> 1) | carry);
        cpu_set_nz(c, c->a);
        break;
    }

    case OP_CLC: cpu_set_flag(c, FLAG_C, false); break;
    case OP_SEC: cpu_set_flag(c, FLAG_C, true);  break;
    case OP_CLI: cpu_set_flag(c, FLAG_I, false); c->irq_inhibit_next = true; break;
    case OP_SEI: cpu_set_flag(c, FLAG_I, true);  c->irq_inhibit_next = true; break;
    case OP_CLV: cpu_set_flag(c, FLAG_V, false); break;
    case OP_CLD: cpu_set_flag(c, FLAG_D, false); break;
    case OP_SED:
        /* The D latch itself works; only the BCD correction path is cut
         * (02-cpu-core.md section 10). */
        cpu_set_flag(c, FLAG_D, true);
        break;

    case OP_JSR: c->pc = (uint16_t)(((uint16_t)c->data << 8) | c->jsr_lo_scratch); break;
    case OP_JMP: break; /* handled by CA_JUMP / CA_IND_READ_HI */

    /* ---- undocumented ---- */
    case OP_LAX: c->a = c->x = v; cpu_set_nz(c, v); break;
    case OP_ANC: c->a &= v; cpu_set_nz(c, c->a); cpu_set_flag(c, FLAG_C, (c->a & 0x80) != 0); break;
    case OP_ALR:
        c->a &= v;
        cpu_set_flag(c, FLAG_C, (c->a & 1) != 0);
        c->a >>= 1;
        cpu_set_nz(c, c->a);
        break;
    case OP_ARR: {
        uint8_t carry = cpu_flag(c, FLAG_C) ? 0x80 : 0;
        c->a &= v;
        c->a = (uint8_t)((c->a >> 1) | carry);
        cpu_set_nz(c, c->a);
        cpu_set_flag(c, FLAG_C, (c->a & 0x40) != 0);
        cpu_set_flag(c, FLAG_V, (((c->a >> 6) ^ (c->a >> 5)) & 1) != 0);
        break;
    }
    case OP_XAA:
        /* Unstable on silicon; no specification, modelled as (A | mag) & X & imm. */
        c->a = (uint8_t)((c->a | 0xEE) & c->x & v);
        cpu_set_nz(c, c->a);
        break;
    case OP_AXS: {
        uint16_t t = (uint16_t)((c->a & c->x) - v);
        cpu_set_flag(c, FLAG_C, t < 0x100);
        c->x = (uint8_t)t;
        cpu_set_nz(c, c->x);
        break;
    }

    case OP_PLA: c->a = v; cpu_set_nz(c, c->a); break;
    case OP_PLP: break; /* done by CA_PULL_P_PLP */
    case OP_PHA: case OP_PHP: case OP_BRK: case OP_NOP:
    case OP_STA: case OP_STX: case OP_STY: case OP_SAX:
    case OP_RTS: case OP_RTI:
    case OP_BCC: case OP_BCS: case OP_BEQ: case OP_BMI:
    case OP_BNE: case OP_BPL: case OP_BVC: case OP_BVS:
    case OP_ASL: case OP_LSR: case OP_ROL: case OP_ROR:
    case OP_INC: case OP_DEC:
    case OP_DCP: case OP_ISB: case OP_SLO: case OP_RLA: case OP_SRE: case OP_RRA:
    case OP_NONE:
    default:
        break;
    }
}

/* RMW: apply the operation to the value read in the RMW read cycle, then the
 * two writes store the original and the modified value
 * (02-cpu-instructions.md section 4, phenomenon 1). */
void cpu_rmw_apply(Cpu *c)
{
    switch (c->info->op) {
    case OP_INC: c->rmw_new = (uint8_t)(c->rmw_value + 1); break;
    case OP_DEC: c->rmw_new = (uint8_t)(c->rmw_value - 1); break;
    case OP_ASL:
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 0x80) != 0);
        c->rmw_new = (uint8_t)(c->rmw_value << 1);
        break;
    case OP_LSR:
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 1) != 0);
        c->rmw_new = (uint8_t)(c->rmw_value >> 1);
        break;
    case OP_ROL: {
        uint8_t carry = cpu_flag(c, FLAG_C) ? 1 : 0;
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 0x80) != 0);
        c->rmw_new = (uint8_t)((c->rmw_value << 1) | carry);
        break;
    }
    case OP_ROR: {
        uint8_t carry = cpu_flag(c, FLAG_C) ? 0x80 : 0;
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 1) != 0);
        c->rmw_new = (uint8_t)((c->rmw_value >> 1) | carry);
        break;
    }
    case OP_DCP:
        /* Decrement, then CMP against the *decremented* value.  The spec
         * (02-cpu-instructions.md section 6) says T1 compares A with the original
         * value, but silicon and AccuracyCoin compare against M-1; the spec row
         * is wrong (see SPEC-GAPS.md). */
        c->rmw_new = (uint8_t)(c->rmw_value - 1);
        cpu_set_flag(c, FLAG_C, c->a >= c->rmw_new);
        cpu_set_nz(c, (uint8_t)(c->a - c->rmw_new));
        return;
    case OP_ISB: {
        c->rmw_new = (uint8_t)(c->rmw_value + 1);
        unsigned r = (unsigned)c->a + (c->rmw_new ^ 0xFF) + (cpu_flag(c, FLAG_C) ? 1u : 0u);
        cpu_set_flag(c, FLAG_C, r > 0xFF);
        cpu_set_flag(c, FLAG_V, ((c->a ^ (uint8_t)r) & ((c->rmw_new ^ 0xFF) ^ (uint8_t)r) & 0x80) != 0);
        c->a = (uint8_t)r;
        cpu_set_nz(c, c->a);
        return;
    }
    case OP_SLO:
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 0x80) != 0);
        c->rmw_new = (uint8_t)(c->rmw_value << 1);
        c->a |= c->rmw_new;
        cpu_set_nz(c, c->a);
        return;
    case OP_RLA: {
        uint8_t carry = cpu_flag(c, FLAG_C) ? 1 : 0;
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 0x80) != 0);
        c->rmw_new = (uint8_t)((c->rmw_value << 1) | carry);
        c->a &= c->rmw_new;
        cpu_set_nz(c, c->a);
        return;
    }
    case OP_SRE:
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 1) != 0);
        c->rmw_new = (uint8_t)(c->rmw_value >> 1);
        c->a ^= c->rmw_new;
        cpu_set_nz(c, c->a);
        return;
    case OP_RRA: {
        uint8_t carry = cpu_flag(c, FLAG_C) ? 0x80 : 0;
        cpu_set_flag(c, FLAG_C, (c->rmw_value & 1) != 0);
        c->rmw_new = (uint8_t)((c->rmw_value >> 1) | carry);
        unsigned r = (unsigned)c->a + c->rmw_new + (cpu_flag(c, FLAG_C) ? 1u : 0u);
        cpu_set_flag(c, FLAG_C, r > 0xFF);
        cpu_set_flag(c, FLAG_V, ((c->a ^ (uint8_t)r) & (c->rmw_new ^ (uint8_t)r) & 0x80) != 0);
        c->a = (uint8_t)r;
        cpu_set_nz(c, c->a);
        return;
    }
    default:
        c->rmw_new = c->rmw_value;
        break;
    }
    cpu_set_nz(c, c->rmw_new);
}
