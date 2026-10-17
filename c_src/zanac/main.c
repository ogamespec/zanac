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
 * reports whether to continue, quit, save or load; the host leaves the game
 * loop with setjmp/longjmp.
 *
 * Modes:
 *      zanac                 interactive (SDL window if built in, else idle)
 *      zanac --frames N      run N frames, then report / dump
 *      zanac --shot FILE     write the last frame as a PPM image
 *      zanac --audio FILE    write the APU output as a WAV file
 *      zanac --hashes FILE   one line per frame: "frame FNV1a(framebuffer)"
 *      zanac --input FILE    scripted pad input (see frontend/input_script.h)
 *      zanac --save-state FILE  write a save state when the run ends
 *      zanac --load-state FILE  start from a save state instead of the reset
 *                               vector (runs --frames N frames after loading)
 *      zanac --state-info FILE  print what a save state contains, then exit
 *      zanac --save-at N        the F5 action at frame N, headless
 *      zanac --load-at N        the F7 action at frame N, headless
 *
 * In the SDL frontend F5 writes the quick state and F7 loads it; see
 * zanac/savestate.c for the format and the resume model.
 */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zanac.h"
#include "savestate.h"
#include "frontend/frontend.h"
#include "frontend/input_script.h"

static NeslibCallbacks callbacks = { reset, nmi_handler };

/* Why the longjmp() out of the game loop happened. */
enum { STOP_NONE = 0, STOP_QUIT = 1, STOP_LOAD = 2, STOP_SELF = 3 };

static jmp_buf  s_stop;
static long     s_frames;
static long     s_frame_limit = -1; /* --frames N (frames of this run) */
static long     s_max_frames = -1;  /* absolute s_frames limit */
static long     s_level = -1;       /* forced starting level, or -1 */
static FILE    *s_hash_file;
static int      s_stopped_by_host;
static int      s_stop_reason;

/* The command-line paths.  They are file-scope because the run ends with a
 * longjmp() out of reset() and locals of main() would be indeterminate. */
static const char *s_script, *s_hashes, *s_shot, *s_dump, *s_ramdump, *s_wav;
static const char *s_save_state, *s_load_state, *s_state_info;
static long        s_save_at = -1, s_load_at = -1;   /* scripted F5 / F7 */

/* A save requested from the frame hook is written at the next
 * zanac_state_checkpoint() (level loc_E880, pause loc_D3B9 or menu loc_E080),
 * which is the point a load can resume from exactly.  A game transition (level
 * complete -> menu, game over -> menu, a long spin) may keep it waiting for a
 * while, so there is also a deadline that falls back to an immediate snapshot;
 * it is long enough for those transitions and only matters for loops with no
 * checkpoint at all (the sound-test screen). */
static int         s_save_pending;
static const char *s_pending_save_path;
static long        s_save_deadline;
static int         s_stop_pending;
static long        s_stop_deadline;
static const char *s_last_saved_path;
static int         s_stop_saved_at_checkpoint;
#define STATE_CHECKPOINT_TIMEOUT 600   /* frames (about ten seconds) */

static int state_saved_to(const char *path)
{
    return s_last_saved_path && strcmp(s_last_saved_path, path) == 0;
}

static void save_now(const char *path)
{
    if (zanac_state_save(path, (uint64_t)s_frames)) {
        fprintf(stderr, "zanac: saved state to %s (frame %ld)\n", path, s_frames);
        s_last_saved_path = path;
    } else {
        fprintf(stderr, "zanac: could not write save state %s\n", path);
    }
}

/* Called by sub_E81E at loc_E880, i.e. at a point where all live state is in
 * RAM/registers.  Writing a save here is what makes a later load continue the
 * game frame for frame (see zanac/savestate.h). */
static void checkpoint_hook(void)
{
    if (s_save_pending) {
        if (zanac_state_save_checkpoint(s_pending_save_path, (uint64_t)s_frames)) {
            fprintf(stderr, "zanac: saved state to %s (frame %ld, checkpoint)\n",
                    s_pending_save_path, s_frames);
            s_last_saved_path = s_pending_save_path;
        } else {
            fprintf(stderr, "zanac: could not write save state %s\n",
                    s_pending_save_path);
        }
        s_save_pending = 0;
    }

    if (s_stop_pending) {
        if (s_save_state && !state_saved_to(s_save_state)) {
            if (zanac_state_save_checkpoint(s_save_state, (uint64_t)s_frames)) {
                fprintf(stderr, "zanac: saved state to %s (frame %ld, checkpoint)\n",
                        s_save_state, s_frames);
                s_last_saved_path = s_save_state;
            } else {
                fprintf(stderr, "zanac: could not write save state %s\n",
                        s_save_state);
            }
        }
        if (s_save_state)
            s_stop_saved_at_checkpoint = 1;
        s_stop_reason = STOP_QUIT;
        longjmp(s_stop, 1);
    }
}

/* The F5 action: request a canonical save at the next loop boundary. */
static void request_state_save(void)
{
    const char *path = s_save_state ? s_save_state : zanac_state_path();
    if (zanac_resume_kind == ZANAC_RESUME_LEVEL ||
        zanac_resume_kind == ZANAC_RESUME_LEVEL_SETUP ||
        zanac_resume_kind == ZANAC_RESUME_PAUSE ||
        zanac_resume_kind == ZANAC_RESUME_MENU) {
        s_save_pending = 1;
        s_pending_save_path = path;
        s_save_deadline = s_frames + STATE_CHECKPOINT_TIMEOUT;
        fprintf(stderr, "zanac: state will be captured at the next frame boundary\n");
    } else {
        save_now(path);
    }
}

/* The F7 action: validate, then leave the game loop so main can load. */
static void request_state_load(void)
{
    const char *path = s_load_state ? s_load_state : zanac_state_path();
    char info[512];

    if (zanac_state_exists(path) &&
        zanac_state_info(path, info, sizeof(info))) {
        s_stop_reason = STOP_LOAD;
        longjmp(s_stop, 1);
    }
    fprintf(stderr, "zanac: no usable save state at %s\n", path);
}

/* Called by neslib once per completed frame, i.e. at the boundary between two
 * frames.  The buttons set here are the ones the *next* frame sees, so the
 * script index used is s_frames + 1 - the reference runner feeds
 * input_script_buttons(i) immediately before running frame i, and using
 * s_frames here made the port react to every input one frame late. */
static void frame_hook(void)
{
    /* A pending action did not reach loc_E880 in time (pause screen, a spin
     * loop, a menu): fall back to an immediate snapshot. */
    if (s_save_pending && s_frames >= s_save_deadline) {
        save_now(s_pending_save_path);
        s_save_pending = 0;
    }
    if (s_stop_pending && s_frames >= s_stop_deadline) {
        if (s_save_state && !state_saved_to(s_save_state))
            save_now(s_save_state);
        if (s_save_state)
            s_stop_saved_at_checkpoint = 1;   /* do not write it again */
        s_stop_reason = STOP_QUIT;
        longjmp(s_stop, 1);
    }

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

    /* The frontend reports F5/F7 from its event loop.  This is a frame
     * boundary, so it is a valid point for both operations. */
    /* --save-at / --load-at are the scripted equivalents of F5/F7, so the
     * same code path can run headless in the regression tests. */
    if (s_load_at >= 0 && s_frames == s_load_at) {
        s_load_at = -1;
        request_state_load();       /* does not return when the file is good */
    }
    if (s_save_at >= 0 && s_frames == s_save_at) {
        s_save_at = -1;
        request_state_save();
    }

    switch (frontend_frame()) {
    case FRONTEND_SAVE_STATE:
        request_state_save();
        break;
    case FRONTEND_LOAD_STATE:
        request_state_load();       /* does not return when the file is good */
        break;
    case FRONTEND_QUIT:
        s_stop_reason = STOP_QUIT;
        longjmp(s_stop, 1);
    default:
        break;
    }

    if (s_max_frames >= 0 && s_frames >= s_max_frames) {
        if (s_save_state &&
            (zanac_resume_kind == ZANAC_RESUME_LEVEL ||
             zanac_resume_kind == ZANAC_RESUME_LEVEL_SETUP ||
             zanac_resume_kind == ZANAC_RESUME_PAUSE ||
             zanac_resume_kind == ZANAC_RESUME_MENU ||
             zanac_resume_kind == ZANAC_RESUME_MENU_SELECT)) {
            /* Let the game reach loc_E880 once more, so --save-state produces
             * a file that reloads exactly.  The deadline in the checks above
             * covers a game that is paused or stuck in a spin loop. */
            if (!s_stop_pending) {
                s_stop_pending = 1;
                s_stop_deadline = s_frames + STATE_CHECKPOINT_TIMEOUT;
            }
        } else {
            s_stop_reason = STOP_QUIT;
            longjmp(s_stop, 1);
        }
    }
}

int main(int argc, char **argv)
{





    long i;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--frames") && i + 1 < argc)
            s_frame_limit = strtol(argv[++i], NULL, 0);
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
        else if (!strcmp(argv[i], "--save-state") && i + 1 < argc)
            s_save_state = argv[++i];
        else if (!strcmp(argv[i], "--load-state") && i + 1 < argc)
            s_load_state = argv[++i];
        else if (!strcmp(argv[i], "--state-info") && i + 1 < argc)
            s_state_info = argv[++i];
        else if (!strcmp(argv[i], "--save-at") && i + 1 < argc)
            s_save_at = strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--load-at") && i + 1 < argc)
            s_load_at = strtol(argv[++i], NULL, 0);
        else {
            fprintf(stderr, "usage: %s [--frames N] [--shot FILE] [--audio FILE]\n"
                            "          [--hashes FILE] [--input FILE]"
                            " [--input-offset N] [--level N]\n"
                            "          [--dump-ppu PREFIX] [--dump-ram FILE]\n"
                            "          [--save-state FILE] [--load-state FILE]"
                            " [--state-info FILE]\n"
                            "          [--save-at N] [--load-at N]\n", argv[0]);
            return 1;
        }
    }

    /* --state-info is a pure file read; no engine or window is needed. */
    if (s_state_info) {
        char info[512];
        if (!zanac_state_info(s_state_info, info, sizeof(info))) {
            fprintf(stderr, "zanac: %s is not a readable save state\n", s_state_info);
            return 1;
        }
        printf("%s\n", info);
        return 0;
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

    /* Zanac (USA) is UxROM with horizontal mirroring: its iNES header carries
     * flags6 = $20 (bit 0 = 0).  The game is a vertical scroller, so the two
     * nametables must be stacked ($2000 and $2800 distinct, CIRAM A10 = PPU
     * A11) and the scroll's page toggle (PPU_CTRL bit 1) really switches banks;
     * with vertical mirroring the bottom rows of the screen would repeat the
     * top of the first nametable. */
    neslib_power_on(NULL, MIRROR_HORIZONTAL, &callbacks);
    zanac_init();                     /* the port's RAM pointer and registers */
    neslib_audio_enable(FRONTEND_SAMPLE_RATE);
    neslib_set_frame_hook(frame_hook);
    zanac_state_set_checkpoint_hook(checkpoint_hook);

    s_max_frames = s_frame_limit;
    {
        volatile int resume = 0;

        if (s_load_state) {
            uint64_t loaded = 0;
            if (!zanac_state_load(s_load_state, &loaded)) {
                fprintf(stderr, "zanac: cannot load save state %s\n", s_load_state);
                if (s_hash_file) fclose(s_hash_file);
                frontend_shutdown();
                return 1;
            }
            s_frames = (long)loaded;
            if (s_frame_limit >= 0)
                s_max_frames = (long)loaded + s_frame_limit;
            s_save_pending = 0;
            s_stop_pending = 0;
            s_stop_saved_at_checkpoint = 0;
            frontend_state_loaded();
            fprintf(stderr, "zanac: loaded %s (host frame %llu)\n",
                    s_load_state, (unsigned long long)loaded);
            resume = 1;
        }

        for (;;) {
            s_stop_reason = STOP_NONE;
            if (setjmp(s_stop) == 0) {
                if (resume) {
                    resume = 0;
                    zanac_state_resume(); /* runs the game; never returns */
                } else {
                    neslib_reset();       /* runs the game; never returns */
                }
                /* Only reached if the ported outer loop returns (it should
                 * not); report it like the old code did. */
                s_stop_reason = STOP_SELF;
            }
            if (s_stop_reason != STOP_LOAD)
                break;

            /* F7: replace the running game with the state file.  The load
             * happens here, outside the abandoned C call stack; the resume
             * point recorded in the file then restarts the matching outer
             * loop. */
            {
                const char *path = s_load_state ? s_load_state : zanac_state_path();
                uint64_t loaded = 0;
                if (!zanac_state_load(path, &loaded)) {
                    fprintf(stderr, "zanac: cannot load save state %s; stopping\n", path);
                    s_stop_reason = STOP_QUIT;
                    break;
                }
                s_frames = (long)loaded;
                if (s_frame_limit >= 0)
                    s_max_frames = (long)loaded + s_frame_limit;
                s_save_pending = 0;
                s_stop_pending = 0;
                s_stop_saved_at_checkpoint = 0;
                frontend_state_loaded();
                fprintf(stderr, "zanac: loaded %s (host frame %llu)\n",
                        path, (unsigned long long)loaded);
                resume = 1;
            }
        }
    }
    s_stopped_by_host = (s_stop_reason == STOP_QUIT);

    /* --save-state writes on any exit (frames reached, F7 failure, Esc).  The
     * run stopped at a frame boundary, so the snapshot is coherent. */
    if (s_save_state && !s_stop_saved_at_checkpoint)
        save_now(s_save_state);

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
