/* main.c - the entry point of the Zanac C port.
 *
 * The ported game code runs on neslib (c_src/neslib), the NES engine derived
 * from the Nespeculator core.  The two handlers the console calls are the
 * ported procedures themselves, installed as neslib callbacks:
 *
 *      Reset  ($E7BF)  reset()        -> NeslibCallbacks::reset
 *      NMI    ($CE8D)  nmi_handler()  -> NeslibCallbacks::vblank
 *
 * Like the 6502 reset vector, the ported Reset handler never returns: it runs
 * the game's outer loop (reset -> main_menu_loop -> sub_E81E -> ...), and the
 * per-frame work happens inside it.  The host therefore observes the game
 * through neslib's frame hook: frontend_frame() is called once per frame and
 * returns non-zero to stop, and the host leaves the game loop with
 * setjmp/longjmp.
 *
 * Modes:
 *      zanac                 interactive (SDL window if built in, else idle)
 *      zanac --frames N      run N frames, then report / dump
 *      zanac --shot FILE     write the last frame as a PPM image
 *      zanac --audio FILE    write the APU output as a WAV file
 *      zanac --hashes FILE   one line per frame: "frame FNV1a(framebuffer)"
 *      zanac --input FILE    scripted pad input (see frontend/input_script.h)
 */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zanac.h"
#include "frontend/frontend.h"
#include "frontend/input_script.h"

static NeslibCallbacks callbacks = { reset, nmi_handler };

static jmp_buf  s_stop;
static long     s_frames;
static long     s_max_frames = -1;
static long     s_level = -1;       /* forced starting level, or -1 */
static FILE    *s_hash_file;
static int      s_stopped_by_host;

/* The command-line paths.  They are file-scope because the run ends with a
 * longjmp() out of reset() and locals of main() would be indeterminate. */
static const char *s_script, *s_hashes, *s_shot, *s_dump, *s_ramdump, *s_wav;

/* Called by neslib once per completed frame, i.e. at the boundary between two
 * frames.  The buttons set here are the ones the *next* frame sees, so the
 * script index used is s_frames + 1 - the reference runner feeds
 * input_script_buttons(i) immediately before running frame i, and using
 * s_frames here made the port react to every input one frame late. */
static void frame_hook(void)
{
    if (s_script) {
        uint8_t mask = input_script_buttons(s_frames + 1);
        int b;
        for (b = 0; b < 8; b++)
            neslib_set_button(0, b, (mask & (1u << b)) != 0);
    }
    /* --level N: force the starting level for the first frames, the same way
     * the reference runner does, so the two can be compared on one level. */
    if (s_level >= 0 && s_frames + 1 < 400) {
        MEMW(0x7D, (uint8_t)s_level);
        MEMW(0x91, (uint8_t)s_level);
    }
    if (s_hash_file)
        fprintf(s_hash_file, "%ld %08X\n", s_frames,
                frontend_frame_hash(neslib_framebuffer()));
    s_frames++;
    if (frontend_frame())
        longjmp(s_stop, 1);
    if (s_max_frames >= 0 && s_frames >= s_max_frames)
        longjmp(s_stop, 1);
}

int main(int argc, char **argv)
{





    long i;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--frames") && i + 1 < argc)
            s_max_frames = strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--shot") && i + 1 < argc)
            s_shot = argv[++i];
        else if (!strcmp(argv[i], "--audio") && i + 1 < argc)
            s_wav = argv[++i];
        else if (!strcmp(argv[i], "--hashes") && i + 1 < argc)
            s_hashes = argv[++i];
        else if (!strcmp(argv[i], "--dump-ppu") && i + 1 < argc)
            s_dump = argv[++i];
        else if (!strcmp(argv[i], "--dump-ram") && i + 1 < argc)
            s_ramdump = argv[++i];
        else if (!strcmp(argv[i], "--level") && i + 1 < argc)
            s_level = strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--input-offset") && i + 1 < argc)
            input_script_set_offset(strtol(argv[++i], NULL, 0));
        else if (!strcmp(argv[i], "--input") && i + 1 < argc)
            s_script = argv[++i];
        else {
            fprintf(stderr, "usage: %s [--frames N] [--shot FILE] [--audio FILE]"
                            " [--hashes FILE] [--input FILE]\n", argv[0]);
            return 1;
        }
    }

    if (s_script && input_script_load(s_script) != 0)
        return 1;
    if (s_hashes) {
        s_hash_file = fopen(s_hashes, "w");
        if (!s_hash_file) {
            fprintf(stderr, "zanac: cannot write %s\n", s_hashes);
            return 1;
        }
    }
    if (frontend_init() != 0)
        return 1;

    neslib_power_on(NULL, &callbacks);
    zanac_init();                     /* the port's RAM pointer and registers */
    neslib_audio_enable(FRONTEND_SAMPLE_RATE);
    neslib_set_frame_hook(frame_hook);

    if (setjmp(s_stop) == 0) {
        neslib_reset();               /* runs the game; never returns by itself */
    } else {
        s_stopped_by_host = 1;
    }

    if (s_hash_file)
        fclose(s_hash_file);
    if (s_shot)
        frontend_write_ppm(s_shot, neslib_framebuffer(), NESLIB_SCREEN_W, NESLIB_SCREEN_H);
    if (s_dump)
        frontend_dump_ppu(s_dump);
    if (s_ramdump)
        frontend_dump_ram(neslib_ram(), s_ramdump);
    if (s_wav)
        frontend_write_wav(s_wav);
    frontend_shutdown();

    fprintf(stderr, "zanac: %ld frame(s)%s, %d stubbed procedure(s) reached\n",
            s_frames, s_stopped_by_host ? "" : " (the game stopped on its own)",
            zanac_stub_count());
    if (zanac_stub_count())
        zanac_stub_report();
    return 0;
}
