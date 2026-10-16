/* cpu.h - 2A03/2A07 (MOS 6502 core) emulation.
 *
 * Reference material:
 *   Specs/02-cpu-core.md         - registers, P flags, open bus, reset, interrupts
 *   Specs/02-cpu-instructions.md - opcodes, cycles, per-cycle bus frames
 *   Specs/02-cpu-dma.md          - RDY halting
 *
 * The core is a cycle-by-cycle state machine: each CPU cycle performs exactly
 * one bus access, mirroring the PHI1 (address + R/W) / PHI2 (data) split of
 * 02-cpu-core.md section 2.  Dummy reads and the RMW double write are issued as
 * real bus accesses because 02-cpu-instructions.md sections 3 and 4 require
 * them to be observable by mappers and by PPU/APU registers.
 */
#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

struct Nes;

/* Status register bits (02-cpu-core.md section 4). */
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20 /* no stored flag; always pushed/read as 1 */
#define FLAG_V 0x40
#define FLAG_N 0x80

/* Per-cycle bus actions.  A step table starts at index 1 and is terminated by
 * CA_NONE.  cpu_ops.c builds them. */
typedef enum {
    CA_NONE = 0,
    CA_OPCODE,          /* 1: fetch opcode at PC, PC++ */
    CA_FETCH_LO,        /* fetch operand low at PC, PC++ */
    CA_FETCH_LO_IDX,    /* fetch operand low; addr = ((lo + index) & $FF) */
    CA_FETCH_LO_PTR,    /* fetch operand low; ptr = ((lo + X) & $FF) */
    CA_FETCH_HI,        /* fetch operand high at PC, PC++ */
    CA_FETCH_TGT_LO,    /* JSR: fetch target low, PC++ */
    CA_FETCH_TGT_HI,    /* JSR: fetch target high, PC = target */
    CA_READ,            /* read the address; result discarded */
    CA_READ_ALU,        /* read the address, then commit the operation */
    CA_INDEXED_LO,      /* dummy read at (base & $FF00) | (lo + index) */
    CA_INDEXED_FIX,     /* complete the indexed address; +1 cycle on a cross */
    CA_STORE,           /* write the store register to the effective address */
    CA_STORE_IDX,       /* complete the indexed address, then write */
    CA_RMW_READ,        /* RMW: read the original value */
    CA_RMW_WRITE_OLD,   /* RMW: write the unmodified value back */
    CA_RMW_WRITE_NEW,   /* RMW: apply the operation, write the result */
    CA_PUSH_A,          /* PHA: write A at $0100+S, S-- */
    CA_PUSH_P,          /* PHP: write P (B=1) at $0100+S, S-- */
    CA_PUSH_JSRH,       /* JSR: push PCH */
    CA_PUSH_JSRL,       /* JSR: push PCL */
    CA_PUSH_FRAME_PCH,  /* interrupt frame: push PCH (suppressed during /RES) */
    CA_PUSH_FRAME_PCL,  /* interrupt frame: push PCL */
    CA_PUSH_FRAME_P,    /* interrupt frame: push P, set I */
    CA_FRAME_VEC_LO,    /* interrupt frame: read vector low */
    CA_FRAME_VEC_HI,    /* interrupt frame: read vector high, jump */
    CA_PULL_A,          /* PLA */
    CA_PULL_P_PLP,      /* PLP (a pending /IRQ is delayed one instruction) */
    CA_PULL_P_RTI,      /* RTI: pull P, I applies immediately */
    CA_PULL_LO,         /* PLA/PLP/RTI: S++, pull */
    CA_RTS_PULL_LO,     /* RTS: S++, pull PCL (low byte of the return address) */
    CA_RTS_PULL_HI,     /* RTS: S++, pull PCH */
    CA_PULL_HI,         /* RTS/RTI: pull PCH */
    CA_RTS_FINISH,      /* RTS: PC = pulled + 1 after the pull of PCL */
    CA_RTI_FINISH,      /* RTI: PC = pulled */
    CA_DISCARD_FETCH,   /* dummy read at PC, PC++ */
    CA_IMPLIED,         /* implied/accumulator operation */
    CA_JUMP,            /* PC = effective address (absolute JMP) */
    CA_IND_READ_LO,     /* JMP (a): read pointer low */
    CA_IND_READ_HI,     /* JMP (a): read pointer high at the wrapped address */
    CA_END,             /* end the instruction (no bus access) */
    CA_BRANCH_TAKEN,    /* relative branch: dummy fetch, then take if taken */
    CA_BRANCH_FIX,      /* add the signed displacement; may cross a page */
    CA_BRANCH_EXTRA,    /* taken branch across a page: one more read */
    CA_ZP_PTR_LO,       /* (d),Y: read the zero-page pointer low */
    CA_ZP_PTR_HI,       /* (d),Y: read the zero-page pointer high (wraps) */
    CA_IZX_READ_LO,     /* (d,X): read pointer low at the wrapped zp address */
    CA_FRAME_FINISH,    /* end of the interrupt/reset frame */
    /* Exact-frame actions (02-cpu-instructions.md section 4).  The indexed
     * modes spend a real bus cycle on the un-indexed/un-carried address; the
     * read form ends on that cycle when no carry occurs and takes a second
     * read only on a page cross. */
    CA_ZP_IDX_DUMMY,    /* zp,X / zp,Y: dummy read at zp, then ea = (zp+idx)&$FF */
    CA_IZX_DUMMY,       /* (d,X): dummy read at the un-indexed zp operand */
    CA_INDEXED_READ,    /* indexed read: read at un-carried ea; commit+end when
                         * no carry, otherwise set the corrected ea and continue */
    CA_INDEXED_READ_FIX,/* indexed read page cross: read at the corrected ea */
    CA_INDEXED_DUMMY,   /* indexed RMW/store: read at un-carried ea, set corrected */
    CA_RMW_READ_IDX,    /* indexed RMW: read the original value at the corrected ea */
    CA_RTS_DUMMY,       /* RTS: the dummy stack read before PCL is pulled */
    CA_RTI_DUMMY        /* RTI: the dummy stack read before P is pulled */
} CycleAction;

/* Addressing modes (02-cpu-core.md section 6). */
typedef enum {
    AM_IMP = 0, AM_ACC, AM_IMM, AM_ZP, AM_ZPX, AM_ZPY,
    AM_ABS, AM_ABX, AM_ABY, AM_IND, AM_IZX, AM_IZY, AM_REL
} AddrMode;

typedef enum {
    OP_NONE = 0, OP_ADC, OP_AND, OP_ASL, OP_ASL_A, OP_BCC, OP_BCS, OP_BEQ,
    OP_BIT, OP_BMI, OP_BNE, OP_BPL, OP_BRK, OP_BVC, OP_BVS, OP_CLC, OP_CLD,
    OP_CLI, OP_CLV, OP_CMP, OP_CPX, OP_CPY, OP_DEC, OP_DEX, OP_DEY, OP_EOR,
    OP_INC, OP_INX, OP_INY, OP_JMP, OP_JSR, OP_LDA, OP_LDX, OP_LDY, OP_LSR,
    OP_LSR_A, OP_NOP, OP_ORA, OP_PHA, OP_PHP, OP_PLA, OP_PLP, OP_ROL, OP_ROL_A,
    OP_ROR, OP_ROR_A, OP_RTI, OP_RTS, OP_SBC, OP_SEC, OP_SED, OP_SEI, OP_STA,
    OP_STX, OP_STY, OP_TAX, OP_TAY, OP_TSX, OP_TXA, OP_TXS, OP_TYA,
    /* Undocumented (stable set).  02-cpu-instructions.md section 6 records that
     * the full set is out of scope of the specification and that an unofficial
     * opcode "is decoded by whatever X lines its bit pattern matches and runs
     * through the same T-cycle machinery". */
    OP_LAX, OP_SAX, OP_DCP, OP_ISB, OP_SLO, OP_RLA, OP_SRE, OP_RRA,
    OP_ANC, OP_ALR, OP_ARR, OP_XAA, OP_AXS, OP_SBC_EB,
    /* Unstable stores and the LAE/LAS load: the "high byte corruption"
     * instructions that AccuracyCoin's SH* suite pins down (out of scope of
     * 02-cpu-instructions.md section 6; see SPEC-GAPS.md). */
    OP_SHA, OP_SHX, OP_SHY, OP_SHS, OP_LAS
} CpuOp;

typedef struct Cpu Cpu;

typedef struct {
    const char    *name;
    uint8_t        mode;       /* AddrMode */
    uint8_t        op;         /* CpuOp */
    uint8_t        cycles;     /* base cycle count including the opcode fetch */
    uint8_t        add_index;  /* 0 none, 1 X, 2 Y */
    uint8_t        page_cross; /* 1 when an indexed read may take the +1 cycle */
    uint8_t        rmw;        /* 1 for read-modify-write */
    uint8_t        unofficial;
    const uint8_t *steps;      /* NUL-terminated per-cycle action table; index 0 unused */
} OpInfo;

/* frame_flags bits */
#define FRAME_FLAG_BRK    0x01 /* BRK opcode: PC increments during cycles 1..2 */
#define FRAME_FLAG_RESET  0x02 /* /RES frame: the three pushes are suppressed */

struct Cpu {
    struct Nes *nes;

    /* Programmer-visible registers (02-cpu-core.md section 3). */
    uint8_t  a, x, y, s, p;
    uint16_t pc;

    /* Instruction state */
    uint8_t        opcode;
    const OpInfo  *info;
    const uint8_t *steps;
    uint8_t        cycle;
    uint8_t        pages;
    uint16_t       base;
    uint16_t       ea;
    uint8_t        ptr;
    uint8_t        operand;
    uint8_t        data;
    uint8_t        rmw_value;
    uint8_t        rmw_new;
    uint8_t        store_scratch; /* SH*: value computed for the unstable store */
    uint8_t        jsr_lo_scratch;
    uint16_t       jsr_addr;   /* PC of the JSR opcode (the value pushed is jsr_addr+2) */
    uint8_t        frame_lo;
    uint16_t       frame_pc;
    uint8_t        frame_pushed_p;
    uint16_t       frame_vector;
    uint8_t        frame_flags;

    /* Interrupts */
    bool    nmi_line;
    bool    nmi_pending;    /* a /NMI falling edge has been latched */
    bool    nmi_latched;    /* an interrupt poll saw the pending /NMI */
    bool    irq_line;
    bool    irq_latched;    /* an interrupt poll saw /IRQ with I clear */
    bool    irq_inhibit_next;

    /* RDY / DMA stall (02-cpu-dma.md) */
    int     rdy_cycles;
    bool    stalled_last;
    bool    fetching;     /* suppress the open-bus latch during a fetch */
    bool    dmc_dma_seen; /* a DMC sample fetch halted this instruction */

    /* Bus interface of the cycle being executed */
    uint16_t bus_addr;
    bool     bus_write;
    uint8_t  bus_data;

    uint64_t total_cycles;
    uint64_t dbg_seq;
    uint64_t instructions;
};

void cpu_init(Cpu *cpu, struct Nes *nes);
void cpu_power_on(Cpu *cpu);
void cpu_reset(Cpu *cpu);
void cpu_tick(Cpu *c);
/* Peek at the cycle the sequencer is about to execute without changing any
 * state: returns true when that cycle performs a bus access and reports its
 * address and whether it is a write.  The DMA engines use it to decide whether
 * a halt can be accepted (read cycles only) and which address to repeat
 * (02-cpu-dma.md section 2). */
bool cpu_peek_bus(Cpu *c, uint16_t *addr, bool *is_write);
void cpu_stall(Cpu *cpu, int cycles);
void cpu_set_nmi(Cpu *cpu, bool asserted);
void cpu_set_irq(Cpu *cpu, bool asserted);

/* Shared helpers (cpu_ops.c). */
void    cpu_set_nz(Cpu *c, uint8_t v);
bool    cpu_flag(Cpu *c, uint8_t f);
void    cpu_set_flag(Cpu *c, uint8_t f, bool on);
uint8_t cpu_index_value(Cpu *c);
uint8_t cpu_store_value(Cpu *c);
bool    cpu_prepare_sh_store(Cpu *c);
bool    cpu_branch_taken(Cpu *c);
void    cpu_commit(Cpu *c);
void    cpu_rmw_apply(Cpu *c);

extern const OpInfo  cpu_op_table[256];
extern const uint8_t cpu_irq_frame[9];
extern const uint8_t cpu_reset_frame[9];

#endif /* CPU_H */
