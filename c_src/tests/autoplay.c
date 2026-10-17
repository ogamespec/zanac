/* autoplay.c - the long-run harness: play the game without a player.
 *
 * The interesting failures of the port happen deep inside a level, near the
 * fortress ("boss") - far past the point a scripted input file reaches.  This
 * harness gets there quickly and then watches for the failure mode the ported
 * code is prone to: the game spinning in one of its "wait for the NMI" loops.
 *
 *   ./build/zanac_auto                     level 1, normal speed
 *   ./build/zanac_auto 4                   four pixels of scroll per frame
 *   ./build/zanac_auto 4 5                 ... starting at level 5
 *   ./build/zanac_auto 0 3 60000           level 3, invincible, 60000 frames
 *   for i in $(seq 1 12); do ./build/zanac_auto 1 $i 20000 || echo "LEVEL $i STALLED"; done
 *
 * What it does every frame:
 *   - keeps the ship invincible ($5AA bit 7 is the flag enemy_type_60 tests
 *     before running the death code), so the run does not end early;
 *   - optionally forces the scroll speed byte ($AE, the value sub_D824 adds to
 *     the fine scroll accumulator each frame) to cover a level fast;
 *   - optionally forces current_level/byte_7D, so any level can be started.
 *
 * What it watches for:
 *   - a stall: a watchdog (SIGALRM) that is re-armed on every frame the host
 *     sees, so it only fires when the hook has genuinely stopped being called.
 *     It then prints the game state, which is what identified the freeze near
 *     the fortress, and exits with status 3.
 *
 * Build with `make zanac_auto`; it is a debugging tool, not part of the port.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <unistd.h>

#include "zanac.h"
#include "frontend/frontend.h"
#include "frontend/input_script.h"

#define WATCHDOG_SECONDS 20

static NeslibCallbacks callbacks = { reset, nmi_handler };
static jmp_buf stop;
static long frames;
static long speed;              /* 0 = leave the game's own scroll speed */
static int level = -1;          /* -1 = leave the game's own level */
static long max_frames = 60000;

static void dump_state(const char *why)
{
    fprintf(stderr, "%s at host frame %ld (emulated %llu)\n", why, frames,
            (unsigned long long)neslib_frame_count());
    fprintf(stderr, "  mode=%02X level=%02X $528=%02X lives=%02X "
            "A2/A3=%02X%02X B3/B4=%02X%02X $6F=%02X $C0=%02X $C7=%02X\n",
            MEM8(0x30), MEM8(0x7D), MEM8(0x528), MEM8(0x32),
            MEM8(0xA3), MEM8(0xA2), MEM8(0xB4), MEM8(0xB3),
            MEM8(0x6F), MEM8(0xC0), MEM8(0xC7));
}

static void on_alarm(int sig)
{
    (void)sig;
    dump_state("STALL (the frame hook stopped being called)");
    _exit(3);
}

static void hook(void)
{
    /* Force the level only while the game is starting it: poking it forever
     * would also override the level the game advances to on its own. */
    if (level >= 0 && frames < 400) {
        MEMW(0x7D, (uint8_t)level);
        MEMW(0x91, (uint8_t)level);
    }
    MEMW(0x5AA, MEM8(0x5AA) | 0x80);        /* invincible */
    if (speed > 0)
        MEMW(0xAE, (uint8_t)speed);

    if (frames % 2000 == 0) {
        fprintf(stderr, "f=%6ld emu=%6llu ", frames,
                (unsigned long long)neslib_frame_count());
        dump_state("progress");
    }

    frames++;
    alarm(WATCHDOG_SECONDS);                /* re-armed: fires only on a stall */
    if (frames >= max_frames)
        longjmp(stop, 1);
}

int main(int argc, char **argv)
{
    setvbuf(stderr, NULL, _IONBF, 0);
    if (argc > 1) speed = strtol(argv[1], NULL, 0);
    if (argc > 2) level = atoi(argv[2]);
    if (argc > 3) max_frames = strtol(argv[3], NULL, 0);

    signal(SIGALRM, on_alarm);
    alarm(WATCHDOG_SECONDS);

    if (input_script_load("tests/play.txt") != 0)
        return 1;
    frontend_init();
    /* Same board as the port: horizontal mirroring, matching the iNES header of
     * Zanac (flags6 = $20). */
    neslib_power_on(NULL, MIRROR_HORIZONTAL, &callbacks);
    zanac_init();
    neslib_audio_enable(FRONTEND_SAMPLE_RATE);
    neslib_set_frame_hook(hook);

    if (setjmp(stop) == 0)
        neslib_reset();
    fprintf(stderr, "done: %ld frames, no stall\n", frames);
    frontend_shutdown();
    return 0;
}
