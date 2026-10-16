/* stack_scan.c - measure the 6502 stack pointer at every JSR enemy_prng.
 *
 * Why this exists: the original's enemy_prng ($872B) mixes the 6502 *stack
 * pointer* into every byte it returns:
 *
 *     $872B  STX $10 / LDX $7E / SEC / ROL / EOR tab_18619_lo,X / ADC $10,X
 *     $8738  TSX / TXA / ...            <- the stack pointer is the entropy
 *
 * so the enemies' positions, directions and shots depend on how deep the caller
 * is.  A ported C procedure has no 6502 stack to read, so the port passes the
 * value explicitly (ENEMY_PRNG_SP in zanac/zanac_ram.h) and this tool measures
 * it: run the *original ROM* in the engine, single-step it, and record the stack
 * pointer at the cycle on which each JSR enemy_prng has fetched its opcode (so
 * nothing has been pushed yet).
 *
 *   make stack-scan            # 6000 frames, invincible, from tests/play.txt
 *   ./build/stack_scan 40000   # longer run reaches more of the later levels
 *
 * The output is one line per call site:
 *
 *     <return address> <stack pointer at the JSR> <hits>
 *
 * and the value the port needs is that pointer minus 2, because the JSR pushes
 * the two bytes of its return address before enemy_prng's TSX.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "neslib/core/nes.h"
#include "neslib/core/cpu.h"
#include "neslib/core/cart.h"
#include "frontend/input_script.h"
#include "zanac/rom/zanac_banks.h"

#define ENEMY_PRNG 0x872B

static void build_prg(uint8_t *dst)
{
    static const uint8_t *const order[8] = { zanac_bank_empty, zanac_bank_empty,
        zanac_bank_empty, zanac_bank_empty, zanac_bank4, zanac_bank5, zanac_bank6,
        zanac_bank7 };
    for (int i = 0; i < 8; i++)
        memcpy(dst + i * ZANAC_BANK_SIZE, order[i], ZANAC_BANK_SIZE);
}

static struct { uint16_t ret; uint8_t sp; long hits; } tab[512];
static int ntab;

static void record(uint16_t ret, uint8_t sp)
{
    for (int i = 0; i < ntab; i++)
        if (tab[i].ret == ret && tab[i].sp == sp) { tab[i].hits++; return; }
    if (ntab < 512) {
        tab[ntab].ret = ret;
        tab[ntab].sp = sp;
        tab[ntab].hits = 1;
        ntab++;
    }
}

int main(int argc, char **argv)
{
    static uint8_t prg[8 * ZANAC_BANK_SIZE];
    long frames = argc > 1 ? strtol(argv[1], NULL, 0) : 6000;
    Nes *nes;

    setvbuf(stdout, NULL, _IONBF, 0);
    if (input_script_load("tests/play.txt") != 0)
        return 1;

    build_prg(prg);
    nes = nes_create(NES_REGION_NTSC);
    cart_init_image(nes->cart, nes, prg, sizeof(prg), 2, 8192, true);
    nes_power_on(nes);

    for (long i = 0; i < frames; i++) {
        uint8_t m = input_script_buttons(i);
        for (int b = 0; b < 8; b++)
            input_set_button(&nes->input, 0, b, (m & (1u << b)) != 0);
        nes->ram[0x5AA] |= 0x80;        /* invincible: reach the later spawners */

        uint64_t start = nes->frame_count;
        while (nes->frame_count == start) {
            Cpu *c = nes->cpu;
            /* Cycle 2 of a JSR: the opcode is fetched (c->pc is opcode + 1 and
             * c->jsr_addr is the address it will push) and nothing is pushed
             * yet, so c->s is the value the port has to reproduce. */
            if (c->opcode == 0x20 && c->cycle == 2) {
                uint16_t tgt = (uint16_t)(nes_cpu_read(nes, c->pc) |
                               (nes_cpu_read(nes, (uint16_t)(c->pc + 1)) << 8));
                if (tgt == ENEMY_PRNG)
                    record(c->jsr_addr, c->s);
            }
            nes_run_cpu_cycle(nes);
        }
    }

    for (int i = 0; i < ntab; i++)
        printf("%04X %02X %ld\n", tab[i].ret, tab[i].sp, tab[i].hits);
    return 0;
}
