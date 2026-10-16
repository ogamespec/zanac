/* run_rom.c - the reference runner: execute the ORIGINAL Zanac ROM.
 *
 * The C port is verified against this program.  Both use the very same engine
 * (c_src/neslib, derived from the Nespeculator core) and the very same frame
 * timing, but this one lets the emulated 6502 run the original machine code
 * while the port runs its translated C.  Feeding both the same number of
 * frames and the same controller input makes their framebuffers comparable:
 *
 *      ./build/zanac_rom --frames 300 --hash          (original ROM)
 *      ./build/zanac      --frames 300 --hash         (C port)
 *
 * The PRG image is the one extracted from Build/bank*.bin and compiled into
 * zanac_banks.c, so no external ROM file is needed.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "neslib/neslib.h"
#include "neslib/core/nes.h"
#include "neslib/core/cpu.h"
#include "neslib/core/ppu.h"
#include "neslib/core/apu.h"
#include "neslib/core/cart.h"
#include "neslib/core/input.h"
#include "frontend/frontend.h"
#include "frontend/input_script.h"
#include "zanac/rom/zanac_banks.h"

/* Build the 128 KB PRG image: banks 0-3 are empty, 4-7 come from the port's
 * extracted bank arrays. */
static void build_prg(uint8_t *dst)
{
    static const uint8_t *const order[8] = {
        zanac_bank_empty, zanac_bank_empty, zanac_bank_empty, zanac_bank_empty,
        zanac_bank4, zanac_bank5, zanac_bank6, zanac_bank7
    };
    int i;
    for (i = 0; i < 8; i++)
        memcpy(dst + i * ZANAC_BANK_SIZE, order[i], ZANAC_BANK_SIZE);
}

int main(int argc, char **argv)
{
    static uint8_t prg[8 * ZANAC_BANK_SIZE];
    Nes *nes;
    long frames = 300;
    long input_offset = 0;
    long i;
    const char *shot = NULL;
    const char *hashes = NULL;
    const char *input_script = NULL;
    const char *dump = NULL;
    const char *ramdump = NULL;
    int verbose = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--frames") && i + 1 < argc) frames = strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--shot") && i + 1 < argc) shot = argv[++i];
        else if (!strcmp(argv[i], "--hashes") && i + 1 < argc) hashes = argv[++i];
        else if (!strcmp(argv[i], "--input") && i + 1 < argc) input_script = argv[++i];
        else if (!strcmp(argv[i], "--input-offset") && i + 1 < argc)
            input_offset = strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--dump-ppu") && i + 1 < argc) dump = argv[++i];
        else if (!strcmp(argv[i], "--dump-ram") && i + 1 < argc) ramdump = argv[++i];
        else if (!strcmp(argv[i], "--verbose")) verbose = 1;
        else { fprintf(stderr, "usage: %s [--frames N] [--shot FILE] [--hashes FILE]\n", argv[0]); return 1; }
    }

    input_script_set_offset(input_offset);
    if (input_script && input_script_load(input_script) != 0)
        return 1;

    build_prg(prg);
    nes = nes_create(NES_REGION_NTSC);
    cart_init_image(nes->cart, nes, prg, sizeof(prg), 2, 8192, true);
    nes_power_on(nes);

    {
        FILE *hf = hashes ? fopen(hashes, "w") : NULL;
        for (i = 0; i < frames; i++) {
            if (input_script) {
                uint8_t mask = input_script_buttons(i);
                int b;
                for (b = 0; b < 8; b++)
                    input_set_button(&nes->input, 0, b, (mask & (1u << b)) != 0);
            }
            nes_run_frame(nes);
            if (hf) {
                fprintf(hf, "%ld %08X\n", i,
                        frontend_frame_hash(&nes->ppu->framebuffer[0][0]));
            }
            if (verbose && (i % 60) == 0)
                fprintf(stderr, "rom frame %ld\n", i);
        }
        if (hf) fclose(hf);
    }

    if (shot)
        frontend_write_ppm(shot, &nes->ppu->framebuffer[0][0], 256, 240);
    if (dump)
        frontend_dump_nes(nes, dump);
    if (ramdump)
        frontend_dump_ram(nes->ram, ramdump);

    fprintf(stderr, "rom: %ld frame(s), final frame hash %08X\n", frames,
            frontend_frame_hash(&nes->ppu->framebuffer[0][0]));
    nes_destroy(nes);
    return 0;
}
