/* headless.c - the no-window frontend of the Zanac C port.
 *
 * Used for the regression tests: it runs the ported game with the host doing
 * nothing but counting frames, so a run can be compared against the original
 * ROM running in the emulator (same framebuffer, same frame count).
 */
#include <stdio.h>
#include <stdlib.h>

#include "frontend.h"
#include "neslib/neslib.h"

static unsigned long s_frames;
static unsigned long s_max;

int frontend_init(void)
{
    const char *limit = getenv("ZANAC_HEADLESS_FRAMES");

    s_frames = 0;
    s_max = limit ? strtoul(limit, NULL, 0) : 0;
    return 0;
}

int frontend_frame(void)
{
    s_frames++;
    if (!s_max && (s_frames % 600) == 0)
        fprintf(stderr, "zanac: %lu frames\n", s_frames);
    return s_max && s_frames >= s_max;
}

void frontend_shutdown(void)
{
}
