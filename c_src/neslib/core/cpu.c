/* cpu.c - cycle-accurate 2A03/2A07 core: instruction sequencer.
 *
 * Reference material:
 *   Specs/02-cpu-core.md         sections 2, 3, 4, 8, 9
 *   Specs/02-cpu-instructions.md sections 3, 4, 5, 6
 *   Specs/02-cpu-dma.md          RDY / DMA halting
 */
#include <string.h>

#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include <stdio.h>

/* ------------------------------------------------------------------ flags -- */

void cpu_set_nz(Cpu *c, uint8_t v)
{
    c->p = (uint8_t)((c->p & (uint8_t)~(FLAG_N | FLAG_Z)) |
                     (v & 0x80) |
                     (v == 0 ? FLAG_Z : 0));
}

bool cpu_flag(Cpu *c, uint8_t f) { return (c->p & f) != 0; }

void cpu_set_flag(Cpu *c, uint8_t f, bool on)
{
    if (on) c->p |= f; else c->p = (uint8_t)(c->p & ~f);
}

void cpu_stall(Cpu *c, int cycles)
{
    if (cycles > 0) c->rdy_cycles += cycles;
}

/* ------------------------------------------------------------------- init -- */

void cpu_init(Cpu *cpu, struct Nes *nes)
{
    memset(cpu, 0, sizeof(*cpu));
    cpu->nes = nes;
    cpu->p = FLAG_U | FLAG_I;
    cpu_power_on(cpu);
}

void cpu_power_on(Cpu *cpu)
{
    /* 02-cpu-core.md section 9.1: A/X/Y = $00, C/Z/D/V/N = 0, I = 1. */
    cpu->a = cpu->x = cpu->y = 0;
    cpu->p = FLAG_U | FLAG_I;
    cpu_reset(cpu);
}

void cpu_reset(Cpu *cpu)
{
    /* Post-/RES: the seven-cycle frame runs with its three stack writes
     * suppressed, so S walks down three times without writing
     * (02-cpu-core.md section 9, phenomenon 10). */
    cpu->p = (uint8_t)((cpu->p & (uint8_t)~(FLAG_B | FLAG_D)) | FLAG_I | FLAG_U);
    cpu->s = (uint8_t)(cpu->s - 3);
    cpu->nmi_pending = false;
    cpu->nmi_latched = false;
    cpu->irq_latched = false;
    cpu->irq_inhibit_next = false;
    cpu->rdy_cycles = 0;
    cpu->stalled_last = false;
    cpu->steps = cpu_reset_frame;
    cpu->cycle = 1;
    cpu->frame_pc = cpu->pc;
    cpu->frame_vector = 0xFFFC;
    cpu->frame_pushed_p = (uint8_t)((cpu->p & ~FLAG_B) | FLAG_U);
    cpu->frame_flags = FRAME_FLAG_RESET;
    cpu->info = NULL;
    cpu->pc = 0;
    cpu->total_cycles = 0;
    cpu->instructions = 0;
}

void cpu_set_nmi(Cpu *cpu, bool asserted)
{
    /* /NMI is falling-edge sensitive (02-cpu-core.md phenomenon 9). */
    if (cpu->nmi_line && !asserted)
        cpu->nmi_pending = true;
    cpu->nmi_line = asserted;
}

void cpu_set_irq(Cpu *cpu, bool asserted) { cpu->irq_line = asserted; }

/* -------------------------------------------------------------- interrupts -- */

static void begin_irq_frame(Cpu *c, uint16_t vector, uint8_t pushed_p)
{
    c->steps = cpu_irq_frame;
    c->cycle = 1;
    c->frame_pc = c->pc;
    c->frame_pushed_p = pushed_p;
    c->frame_vector = vector;
    c->frame_flags = 0;
    c->info = NULL;
}

/* The core's interrupt poll: the /IRQ line (through the I flag) and the
 * latched /NMI edge are sampled at fixed points inside an instruction, and an
 * interrupt seen there is taken when the instruction retires
 * (02-cpu-core.md section 9; AccuracyCoin "CPU Interrupts"). */
static void cpu_poll(Cpu *c)
{
    if (c->nmi_pending)
        c->nmi_latched = true;
    else if (c->irq_line && !cpu_flag(c, FLAG_I))
        c->irq_latched = true;
}

static bool cpu_is_final_cycle(Cpu *c)
{
    uint8_t action = c->steps[c->cycle];
    if (c->steps[c->cycle + 1] == CA_NONE)
        return true;
    switch (action) {
    case CA_INDEXED_READ: {
        /* Indexed reads end on the un-carried read unless it carries, in
         * which case the corrected read on the next cycle is the last one. */
        uint16_t sum = (uint16_t)((c->base & 0xFF) + cpu_index_value(c));
        return sum <= 0xFF;
    }
    case CA_JUMP:
    case CA_IND_READ_HI:
    case CA_FETCH_TGT_HI:
    case CA_RTS_FINISH:
    case CA_FRAME_VEC_HI:
    case CA_FRAME_FINISH:
        return true;
    default:
        return false;
    }
}

/* Is the upcoming cycle one where the core polls for interrupts?
 *  - a normal instruction polls at the start of its final cycle;
 *  - a branch polls before cycle 2, and again before cycle 4 only when it is
 *    taken across a page boundary;
 *  - BRK polls before cycles 2, 3, 4 and 5 and not before 6 and 7;
 *  - a hardware interrupt/reset frame does not poll. */
static bool cpu_poll_point(Cpu *c)
{
    uint8_t action = c->steps[c->cycle];

    if (c->frame_flags & FRAME_FLAG_BRK)
        return action == CA_DISCARD_FETCH || action == CA_PUSH_FRAME_PCH ||
               action == CA_PUSH_FRAME_PCL || action == CA_PUSH_FRAME_P;
    if (c->frame_flags & FRAME_FLAG_RESET)
        return false;
    if (c->info == NULL)
        return false;   /* hardware interrupt frame */
    if (c->info->mode == AM_REL) {
        if (action == CA_FETCH_LO)
            return true;
        if (action == CA_BRANCH_EXTRA && c->pages)
            return true;
        return false;
    }
    return cpu_is_final_cycle(c);
}

static void instruction_end(Cpu *c)
{
    c->steps = NULL;
    c->info = NULL;
    c->cycle = 1;
    c->instructions++;
    if (c->nmi_latched) {
        c->nmi_latched = false;
        c->irq_latched = false;
        c->nmi_pending = false;
        begin_irq_frame(c, 0xFFFA, (uint8_t)((c->p & ~FLAG_B) | FLAG_U));
    } else if (c->irq_latched) {
        c->irq_latched = false;
        begin_irq_frame(c, 0xFFFE, (uint8_t)((c->p & ~FLAG_B) | FLAG_U));
    }
}

/* ------------------------------------------------------------------- tick -- */

static uint8_t fetch_read(Cpu *c, uint16_t addr);

static uint8_t bus_read(Cpu *c, uint16_t addr)
{
    c->bus_addr = addr;
    c->bus_write = false;
    c->data = nes_cpu_read(c->nes, addr);
    if (c->nes->buslog && c->nes->cpu_cycles >= c->nes->buslog_from &&
        (c->nes->buslog_to == 0 || c->nes->cpu_cycles <= c->nes->buslog_to) &&
        (addr >= 0x8000 || (addr >= 0x2000 && addr < 0x4020))) {
        fprintf(c->nes->buslog, "%llu [%d,%d] R %04X %02X pc=%04X%s\n",
                (unsigned long long)c->nes->cpu_cycles,
                c->nes->ppu ? c->nes->ppu->line : -1,
                c->nes->ppu ? c->nes->ppu->dot : -1,
                addr, c->data, c->pc,
                c->fetching ? " F" : "");
    }
    /* Every read that drives the bus updates the open-bus latch, including
     * opcode and operand fetches: the ROM/cartridge drives the fetched byte,
     * and that byte is what a later read of an unmapped address returns
     * (AccuracyCoin "Open Bus" tests 1-3).  A $4015 read is the exception:
     * the APU status is placed on the internal bus only, so the external bus
     * (which unmapped reads return) keeps its previous value - AccuracyCoin
     * "Internal Data Bus" test 3. */
    if (addr != 0x4015)
        nes_set_ext_bus(c->nes, c->data);
    nes_set_open_bus(c->nes, c->data);
    return c->data;
}

static uint8_t fetch_read(Cpu *c, uint16_t addr)
{
    bool saved = c->fetching;
    uint8_t v;
    c->fetching = true;
    v = bus_read(c, addr);
    c->fetching = saved;
    return v;
}

static void bus_write(Cpu *c, uint16_t addr, uint8_t v)
{
    c->bus_addr = addr;
    c->bus_write = true;
    c->bus_data = v;
    nes_cpu_write(c->nes, addr, v);
    if (c->nes->buslog && c->nes->cpu_cycles >= c->nes->buslog_from &&
        (c->nes->buslog_to == 0 || c->nes->cpu_cycles <= c->nes->buslog_to)) {
        fprintf(c->nes->buslog, "%llu [%d,%d] W %04X %02X pc=%04X\n",
                (unsigned long long)c->nes->cpu_cycles,
                c->nes->ppu ? c->nes->ppu->line : -1,
                c->nes->ppu ? c->nes->ppu->dot : -1,
                addr, v, c->pc);
    }
}

static void instruction_end(Cpu *c);

/* Peek at the upcoming cycle (see cpu.h).  Only the actions the opcode tables
 * actually use are listed; anything else reports "no bus access", which the DMA
 * engines treat as a cycle that cannot accept a halt. */
bool cpu_peek_bus(Cpu *c, uint16_t *addr, bool *is_write)
{
    *addr = 0;
    *is_write = false;
    if (!c->steps) {
        *addr = c->pc;              /* opcode fetch */
        return true;
    }
    switch (c->steps[c->cycle]) {
    case CA_OPCODE:
    case CA_FETCH_LO:
    case CA_FETCH_HI:
    case CA_FETCH_TGT_LO:
    case CA_FETCH_TGT_HI:
    case CA_FETCH_LO_IDX:
    case CA_FETCH_LO_PTR:
    case CA_DISCARD_FETCH:
    case CA_BRANCH_TAKEN:
    case CA_BRANCH_EXTRA:
    case CA_RTS_FINISH:
        *addr = c->pc;
        return true;

    case CA_READ:
    case CA_READ_ALU:
    case CA_RMW_READ:
    case CA_INDEXED_READ_FIX:
    case CA_RMW_READ_IDX:
        *addr = c->ea;
        return true;

    case CA_INDEXED_READ:
    case CA_INDEXED_DUMMY: {
        uint16_t sum = (uint16_t)((c->base & 0xFF) + cpu_index_value(c));
        *addr = (uint16_t)((c->base & 0xFF00) | (sum & 0xFF));
        return true;
    }

    case CA_ZP_IDX_DUMMY:
        *addr = c->operand;
        return true;

    case CA_IZX_DUMMY:
        *addr = c->base;
        return true;

    case CA_ZP_PTR_LO:
        *addr = c->operand;
        return true;
    case CA_ZP_PTR_HI:
        *addr = (uint16_t)(uint8_t)(c->operand + 1);
        return true;

    case CA_IND_READ_LO:
        *addr = c->ea;
        return true;
    case CA_IND_READ_HI:
        *addr = (uint16_t)((c->ea & 0xFF00) | ((c->ea + 1) & 0xFF));
        return true;

    case CA_FRAME_VEC_LO:
        *addr = c->frame_vector;
        return true;
    case CA_FRAME_VEC_HI:
        *addr = (uint16_t)(c->frame_vector + 1);
        return true;

    /* Stack accesses: the exact address is irrelevant (it is always internal
     * RAM), so the pointer is reported as-is. */
    case CA_RTS_DUMMY:
    case CA_RTI_DUMMY:
    case CA_PULL_LO:
    case CA_RTS_PULL_LO:
    case CA_RTS_PULL_HI:
    case CA_PULL_A:
    case CA_PULL_P_PLP:
    case CA_PULL_HI:
        *addr = (uint16_t)(0x0100 | c->s);
        return true;
    case CA_PULL_P_RTI:
        *addr = (uint16_t)(0x0100 | (uint8_t)(c->s + 1));
        return true;

    case CA_STORE:
    case CA_RMW_WRITE_OLD:
    case CA_RMW_WRITE_NEW:
    case CA_PUSH_A:
    case CA_PUSH_P:
    case CA_PUSH_JSRH:
    case CA_PUSH_JSRL:
        *addr = c->ea;
        *is_write = true;
        return true;
    case CA_STORE_IDX:
        *addr = (uint16_t)(c->base + cpu_index_value(c));
        *is_write = true;
        return true;
    case CA_PUSH_FRAME_PCH:
    case CA_PUSH_FRAME_PCL:
    case CA_PUSH_FRAME_P: {
        bool reset = (c->frame_flags & FRAME_FLAG_RESET) != 0;
        *addr = (uint16_t)(0x0100 | c->s);
        *is_write = !reset;
        return true;
    }

    default:
        return false;
    }
}

void cpu_tick(Cpu *c)
{
    if (c->rdy_cycles > 0) {
        /* RDY only halts read cycles (02-cpu-core.md phenomenon 4): a stalled
         * cycle issues no bus access and does not advance the instruction. */
        c->rdy_cycles--;
        c->stalled_last = true;
        c->total_cycles++;
        return;
    }
    c->stalled_last = false;

    if (!c->steps) {
        /* Opcode fetch cycle */
        uint8_t op;
        op = fetch_read(c, c->pc);
        c->opcode = op;
        c->pc++;
        c->info = &cpu_op_table[op];
        c->steps = c->info->steps;
        c->cycle = 2;
        c->pages = 0;
        c->base = 0;
        c->ea = 0;
        c->ptr = 0;
        c->operand = 0;
        c->rmw_value = 0;
        c->rmw_new = 0;
        c->store_scratch = 0;
        c->dmc_dma_seen = false;
        c->jsr_addr = 0;
        if (!c->steps) {
            /* No table (should not happen): treat as a two-cycle NOP. */
            instruction_end(c);
            c->total_cycles++;
            return;
        }
        if (c->info->op == OP_JSR) {
            c->jsr_addr = (uint16_t)(c->pc + 1); /* return address = opcode + 2 */
        }
        if (c->info->op == OP_BRK) {
            /* BRK is a software interrupt: it pushes the status with B set and
             * its own vector, and it increments PC on both of its first two
             * cycles (02-cpu-instructions.md phenomenon 6).  Without this the
             * frame would default to all-clear and behave as a hardware /IRQ. */
            c->frame_flags = FRAME_FLAG_BRK;
            c->frame_pc = c->pc;
            c->frame_pushed_p = (uint8_t)((c->p & ~FLAG_B) | FLAG_B | FLAG_U);
            c->frame_vector = 0xFFFE;
        }
        c->total_cycles++;
        return;
    }

    uint8_t action = c->steps[c->cycle];
    if (action == CA_NONE) {
        instruction_end(c);
        c->total_cycles++;
        return;
    }

    if (cpu_poll_point(c))
        cpu_poll(c);

    switch (action) {

    case CA_OPCODE:
        /* Only reached at cycle 1 of the reset frame (opcode fetches are
         * handled above).  Fetch and discard, PC stays at the reset vector. */
        bus_read(c, c->pc);
        break;

    case CA_FETCH_LO:
        c->operand = fetch_read(c, c->pc);
        c->pc++;
        /* Immediate mode: the operand *is* the data.  For absolute modes the
         * low byte of the address is latched instead. */
        if (c->info->mode == AM_IMM) {
            c->data = c->operand;
            c->ea = 0;
            /* Immediate instructions are two cycles: the operation completes
             * in the operand cycle (02-cpu-instructions.md section 4).  Only
             * commit when this really is the instruction's last cycle
             * (PHA/PLA/BRK-style frames have more). */
            if (c->steps[c->cycle + 1] == CA_NONE)
                cpu_commit(c);
        } else if (c->info->mode == AM_REL) {
            c->data = c->operand;
            /* A branch that is not taken ends after the displacement. */
            if (!cpu_branch_taken(c)) {
                instruction_end(c);
                c->total_cycles++;
                return;
            }
        } else {
            c->base = (uint16_t)((c->base & 0xFF00) | c->operand);
            c->ea = c->base;
        }
        break;

    case CA_FETCH_LO_IDX: {
        /* zero page,X / zero page,Y.  The dummy read of the un-indexed address
         * happens in the next cycle (CA_ZP_IDX_DUMMY), so only the operand is
         * latched here. */
        uint8_t zp = bus_read(c, c->pc);
        c->pc++;
        c->operand = zp;
        c->base = zp;
        c->ea = zp;
        break;
    }

    case CA_FETCH_LO_PTR: {
        /* (d,X): the operand is the zero-page pointer base; the pointer itself
         * is at (zp + X) & $FF.  The dummy read of the un-indexed address comes
         * next (CA_IZX_DUMMY), which then moves the pointer address into
         * c->operand for the two pointer fetches. */
        uint8_t zp = bus_read(c, c->pc);
        c->pc++;
        c->ptr = (uint8_t)(zp + c->x);
        c->operand = zp;
        c->base = zp;
        c->ea = zp;
        break;
    }

    case CA_FETCH_HI:
        c->operand = fetch_read(c, c->pc);
        c->pc++;
        c->base = (uint16_t)((c->base & 0x00FF) | ((uint16_t)c->operand << 8));
        c->ea = c->base;
        /* JMP absolute is three cycles: the jump completes in the high-byte
         * cycle.  JMP indirect has two more cycles of its own. */
        if (c->info->op == OP_JMP && c->info->mode == AM_ABS) {
            c->pc = c->ea;
            instruction_end(c);
            c->total_cycles++;
            return;
        }
        break;

    case CA_FETCH_TGT_LO:
        /* The value pushed by JSR is the address of the JSR's last byte
         * (opcode address + 2), which the opcode-fetch cycle stored in
         * jsr_addr (02-cpu-instructions.md section 4, "Stack, returns"). */
        c->jsr_lo_scratch = bus_read(c, c->pc);
        c->pc++;
        break;

    case CA_FETCH_TGT_HI:
        c->operand = bus_read(c, c->pc);
        c->pc++;
        c->pc = (uint16_t)(((uint16_t)c->operand << 8) | c->jsr_lo_scratch);
        instruction_end(c);
        c->total_cycles++;
        return;

    case CA_READ:
        bus_read(c, c->ea);
        break;

    case CA_READ_ALU:
        c->data = bus_read(c, c->ea);
        if (c->info->op == OP_JMP) {
            /* JMP (a) is resolved inside CA_IND_READ_HI instead. */
            c->pc = c->ea;
            instruction_end(c);
            c->total_cycles++;
            return;
        }
        cpu_commit(c);
        if (c->info->op == OP_JSR) {
            instruction_end(c);
            c->total_cycles++;
            return;
        }
        break;

    case CA_INDEXED_LO: {
        /* Dummy read at the un-carried address (02-cpu-instructions.md
         * section 3, "the index is added to the low byte first"). */
        uint8_t idx = cpu_index_value(c);
        uint16_t sum = (uint16_t)((c->base & 0xFF) + idx);
        c->pages = (sum > 0xFF) ? 1 : 0;
        c->ea = (uint16_t)((c->base & 0xFF00) | (sum & 0xFF));
        bus_read(c, c->ea);
        break;
    }

    case CA_INDEXED_FIX: {
        c->ea = (uint16_t)(c->base + cpu_index_value(c));
        if (c->pages && c->info->page_cross) {
            /* One more read at the corrected address (the "oops" cycle). */
            bus_read(c, c->ea);
        }
        break;
    }

    case CA_ZP_IDX_DUMMY:
        /* zp,X / zp,Y: the un-indexed zero-page address is read on this cycle
         * (a real bus access); the effective address then wraps inside the
         * zero page. */
        bus_read(c, (uint16_t)(c->operand & 0xFF));
        c->ea = (uint16_t)((uint8_t)(c->operand + cpu_index_value(c)));
        c->base = c->ea;
        break;

    case CA_IZX_DUMMY:
        /* (d,X): the un-indexed pointer base is read here, then the wrapped
         * (zp + X) address is used for the two pointer fetches. */
        bus_read(c, (uint16_t)(c->base & 0xFF));
        c->operand = c->ptr;
        break;

    case CA_INDEXED_READ: {
        /* Indexed read: the index is added to the low byte first and that
         * address is read.  Without a carry that read *is* the data; with a
         * carry the value is discarded and the corrected address is read on
         * the next cycle. */
        uint8_t idx = cpu_index_value(c);
        uint16_t sum = (uint16_t)((c->base & 0xFF) + idx);
        c->pages = (sum > 0xFF) ? 1 : 0;
        c->ea = (uint16_t)((c->base & 0xFF00) | (sum & 0xFF));
        c->data = bus_read(c, c->ea);
        if (!c->pages) {
            cpu_commit(c);
            instruction_end(c);
            c->total_cycles++;
            return;
        }
        c->ea = (uint16_t)(c->base + idx);
        break;
    }

    case CA_INDEXED_READ_FIX:
        c->data = bus_read(c, c->ea);
        cpu_commit(c);
        break;

    case CA_INDEXED_DUMMY: {
        /* Indexed RMW/store: the un-carried address is read (and discarded),
         * then the corrected address is used by the following action. */
        uint8_t idx = cpu_index_value(c);
        uint16_t sum = (uint16_t)((c->base & 0xFF) + idx);
        c->pages = (sum > 0xFF) ? 1 : 0;
        c->ea = (uint16_t)((c->base & 0xFF00) | (sum & 0xFF));
        bus_read(c, c->ea);
        c->ea = (uint16_t)(c->base + idx);
        break;
    }

    case CA_RMW_READ_IDX:
        c->rmw_value = bus_read(c, c->ea);
        break;

    case CA_RTS_DUMMY:
    case CA_RTI_DUMMY:
        /* The dummy stack read that precedes the pulled bytes.  S is not
         * advanced: the following pull action increments it before reading
         * (02-cpu-instructions.md section 4, "Stack, returns"). */
        bus_read(c, (uint16_t)(0x0100 | c->s));
        break;

    case CA_STORE:
        bus_write(c, c->ea, cpu_store_value(c));
        break;

    case CA_STORE_IDX:
        c->ea = (uint16_t)(c->base + cpu_index_value(c));
        if (cpu_prepare_sh_store(c))
            bus_write(c, c->ea, c->store_scratch);
        else
            bus_write(c, c->ea, cpu_store_value(c));
        break;

    case CA_RMW_READ:
        c->rmw_value = bus_read(c, c->ea);
        break;

    case CA_RMW_WRITE_OLD:
        bus_write(c, c->ea, c->rmw_value);
        break;

    case CA_RMW_WRITE_NEW:
        cpu_rmw_apply(c);
        bus_write(c, c->ea, c->rmw_new);
        break;

    case CA_PUSH_A:
        bus_write(c, (uint16_t)(0x0100 | c->s), c->a);
        c->s--;
        break;

    case CA_PUSH_P:
        bus_write(c, (uint16_t)(0x0100 | c->s),
                  (uint8_t)((c->p & ~FLAG_B) | FLAG_B | FLAG_U));
        c->s--;
        break;

    case CA_PUSH_JSRH:
        bus_write(c, (uint16_t)(0x0100 | c->s), (uint8_t)(c->jsr_addr >> 8));
        c->s--;
        break;

    case CA_PUSH_JSRL:
        bus_write(c, (uint16_t)(0x0100 | c->s), (uint8_t)(c->jsr_addr & 0xFF));
        c->s--;
        break;

    case CA_PUSH_FRAME_PCH:
        if (c->frame_flags & FRAME_FLAG_RESET)
            bus_read(c, (uint16_t)(0x0100 | c->s));
        else
            bus_write(c, (uint16_t)(0x0100 | c->s), (uint8_t)(c->frame_pc >> 8));
        c->s--;
        break;

    case CA_PUSH_FRAME_PCL:
        if (c->frame_flags & FRAME_FLAG_RESET)
            bus_read(c, (uint16_t)(0x0100 | c->s));
        else
            bus_write(c, (uint16_t)(0x0100 | c->s), (uint8_t)(c->frame_pc & 0xFF));
        c->s--;
        break;

    case CA_PUSH_FRAME_P:
        if (c->frame_flags & FRAME_FLAG_RESET)
            bus_read(c, (uint16_t)(0x0100 | c->s));
        else
            bus_write(c, (uint16_t)(0x0100 | c->s), c->frame_pushed_p);
        c->s--;
        /* The vector is fixed during cycle 5, so an /NMI asserted during the
         * first four cycles of a BRK hijacks it: the NMI vector is used while
         * P is still pushed with B = 1 (02-cpu-instructions.md phenomenon 10).
         * The same rule applies to an /IRQ frame, which the NMI pre-empts. */
        if ((c->nmi_pending || c->nmi_latched) &&
            c->frame_vector == 0xFFFE) {
            c->nmi_pending = false;
            c->nmi_latched = false;
            c->frame_vector = 0xFFFA;
        }
        cpu_set_flag(c, FLAG_I, true);
        break;

    case CA_FRAME_VEC_LO:
        c->frame_lo = bus_read(c, c->frame_vector);
        break;

    case CA_FRAME_VEC_HI:
        c->operand = bus_read(c, (uint16_t)(c->frame_vector + 1));
        c->pc = (uint16_t)(((uint16_t)c->operand << 8) | c->frame_lo);
        c->frame_flags = 0;
        instruction_end(c);
        c->total_cycles++;
        return;

    case CA_FRAME_FINISH:
        instruction_end(c);
        c->total_cycles++;
        return;

    case CA_RTS_PULL_LO:
        c->s++;
        c->frame_lo = bus_read(c, (uint16_t)(0x0100 | c->s));
        break;

    case CA_RTS_PULL_HI:
        c->s++;
        c->operand = bus_read(c, (uint16_t)(0x0100 | c->s));
        c->pc = (uint16_t)(((uint16_t)c->operand << 8) | c->frame_lo);
        break;

    case CA_PULL_A:
        c->a = bus_read(c, (uint16_t)(0x0100 | c->s));
        cpu_set_nz(c, c->a);
        break;

    case CA_PULL_P_PLP:
        /* CA_PULL_LO already advanced S for this byte. */
        c->data = bus_read(c, (uint16_t)(0x0100 | c->s));
        c->p = (uint8_t)((c->data & ~(FLAG_B | FLAG_U)) | FLAG_U);
        c->irq_inhibit_next = true;
        break;

    case CA_PULL_P_RTI:
        /* RTI pulls P first; this is its own byte, so advance S here. */
        c->s++;
        c->data = bus_read(c, (uint16_t)(0x0100 | c->s));
        c->p = (uint8_t)((c->data & ~(FLAG_B | FLAG_U)) | FLAG_U);
        /* RTI restores I before the poll, so its effect is immediate. */
        c->irq_inhibit_next = false;
        break;

    case CA_PULL_LO:
        /* Used by PLA/PLP, which advance S once for their single byte. */
        c->s++;
        c->frame_lo = bus_read(c, (uint16_t)(0x0100 | c->s));
        break;

    case CA_PULL_HI:
        c->s++;
        c->operand = bus_read(c, (uint16_t)(0x0100 | c->s));
        c->pc = (uint16_t)(((uint16_t)c->operand << 8) | c->frame_lo);
        break;

    case CA_RTS_FINISH:        /* RTS: read at PC (the extra cycle), then PC++ */
        bus_read(c, c->pc);
        c->pc++;
        instruction_end(c);
        c->total_cycles++;
        return;

    case CA_RTI_FINISH:
        instruction_end(c);
        c->total_cycles++;
        return;

    case CA_DISCARD_FETCH:
        c->data = fetch_read(c, c->pc);
        /* BRK increments PC on both of its first two cycles; an interrupt does
         * not (02-cpu-instructions.md phenomenon 6).  The value pushed by the
         * frame is the PC *after* both increments, i.e. the instruction after
         * BRK's padding byte, so it is latched here rather than at the opcode
         * fetch. */
        if (c->frame_flags & FRAME_FLAG_BRK) {
            c->pc++;
            c->frame_pc = c->pc;
        }
        /* Implied and accumulator operations are two cycles: the operation
         * completes in the discard cycle. */
        if (c->info && (c->info->mode == AM_IMP || c->info->mode == AM_ACC) &&
            c->steps[c->cycle + 1] == CA_NONE)
            cpu_commit(c);
        break;

    case CA_IMPLIED:
        cpu_commit(c);
        break;

    case CA_JUMP:
        c->pc = c->ea;
        instruction_end(c);
        c->total_cycles++;
        return;

    case CA_IND_READ_LO:
        c->ptr = bus_read(c, c->ea);
        break;

    case CA_IND_READ_HI: {
        /* JMP ($xxFF) fetches the high byte from the same page: neither the
         * Wiki nor the HDL documents it (02-cpu-instructions.md Open
         * questions); the wrap is the community-documented behaviour. */
        uint16_t hi = (uint16_t)((c->ea & 0xFF00) | ((c->ea + 1) & 0xFF));
        c->operand = bus_read(c, hi);
        c->pc = (uint16_t)(((uint16_t)c->operand << 8) | c->ptr);
        instruction_end(c);
        c->total_cycles++;
        return;
    }

    case CA_BRANCH_TAKEN:
        /* Fetch-and-discard at the un-corrected PC while the displacement is
         * added (02-cpu-instructions.md section 4, "Branch").  A taken branch
         * on the same page ends here (3 cycles); across a page one more read
         * follows (4 cycles). */
        bus_read(c, c->pc);
        c->pc++;
        {
            int8_t disp = (int8_t)c->operand;
            uint16_t un_fixed = (uint16_t)(c->pc - 1); /* PC before the discard */
            uint16_t target = (uint16_t)(un_fixed + disp);
            c->pages = ((un_fixed ^ target) & 0xFF00) ? 1 : 0;
            c->base = un_fixed;   /* kept for the extra cycle's dummy address */
            c->pc = target;
        }
        if (!c->pages) {
            instruction_end(c);
            c->total_cycles++;
            return;
        }
        break;

    case CA_BRANCH_FIX:
        /* Superseded by CA_BRANCH_TAKEN (kept so old save states decode). */
        break;

    case CA_BRANCH_EXTRA:
        /* The page-crossing branch's fourth cycle reads the address built from
         * the *unfixed* high byte and the corrected low byte - the "temporary
         * location of the PC before correcting the high byte" the hardware
         * presents (AccuracyCoin "Branch Dummy Reads" test 5 reads PPUSTATUS
         * through it). */
        if (c->pages)
            bus_read(c, (uint16_t)((c->base & 0xFF00) | (c->pc & 0x00FF)));
        instruction_end(c);
        c->total_cycles++;
        return;

    case CA_ZP_PTR_LO:
        /* (d),Y: pointer low from the zero page */
        c->ptr = bus_read(c, (uint16_t)c->operand);
        break;

    case CA_ZP_PTR_HI: {
        /* Read the pointer's high byte.  Using c->ptr (not c->operand) makes the
         * read wrap to $0000 when the pointer sits at $xxFF, as the 6502 does
         * (02-cpu-core.md section 6), and it is what (d,X) needs: there c->ptr
         * was set to the pointer address by CA_FETCH_LO_PTR, whereas for (d),Y
         * CA_ZP_PTR_LO set it. */
        uint8_t hi = (uint8_t)(c->operand + 1);
        uint8_t lo = c->ptr;
        uint8_t hi_val = bus_read(c, (uint16_t)hi);
        c->base = (uint16_t)(((uint16_t)hi_val << 8) | lo);
        c->ea = c->base;
        break;
    }

    case CA_NONE:
    default:
        instruction_end(c);
        c->total_cycles++;
        return;
    }

    c->cycle++;
    c->total_cycles++;
    /* CA_NONE is a terminator, not a cycle: the instruction ends on the cycle
     * of its last real bus access. */
    if (c->steps[c->cycle] == CA_NONE)
        instruction_end(c);
}
