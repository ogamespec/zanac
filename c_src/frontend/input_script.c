/* input_script.c - scripted controller input for the regression runs.
 *
 * Both the C port and the reference ROM runner read the same script, so their
 * framebuffers can be compared while the game is actually being played:
 *
 *      # frame  buttons            (buttons: A B SELECT START UP DOWN LEFT RIGHT)
 *      60       START
 *      70       -
 *      120      RIGHT+A
 *
 * A line sets the controller state from that frame until the next line, so
 * "held" buttons are written once.  The parser also accepts a hex mask as the
 * second field (0x08 = START).
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_script.h"

typedef struct {
    long    frame;
    uint8_t buttons;
} ScriptStep;

static ScriptStep *g_steps;
static int         g_count;
static long        g_offset;

/* Case-insensitive comparison of two ASCII strings (strcasecmp is POSIX and
 * does not exist on MSVC, where it is _stricmp). */
static int name_is(const char *a, const char *b)
{
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static int bit_for(const char *name)
{
    if (name_is(name, "A")) return 0;
    if (name_is(name, "B")) return 1;
    if (name_is(name, "SELECT")) return 2;
    if (name_is(name, "START")) return 3;
    if (name_is(name, "UP")) return 4;
    if (name_is(name, "DOWN")) return 5;
    if (name_is(name, "LEFT")) return 6;
    if (name_is(name, "RIGHT")) return 7;
    return -1;
}

int input_script_load(const char *path)
{
    FILE *fp;
    char line[256];

    fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "zanac: cannot read the input script %s\n", path);
        return -1;
    }
    g_steps = (ScriptStep *)calloc(256, sizeof(*g_steps));
    g_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        char *hash = strchr(line, '#');
        char *frame_text;
        char *buttons_text;
        long frame;
        uint8_t mask = 0;

        if (hash) *hash = '\0';
        frame_text = strtok(line, " \t\r\n");
        if (!frame_text) continue;
        buttons_text = strtok(NULL, " \t\r\n");
        frame = strtol(frame_text, NULL, 0);
        if (buttons_text) {
            if (!strncmp(buttons_text, "0x", 2) || !strncmp(buttons_text, "0X", 2)) {
                mask = (uint8_t)strtoul(buttons_text, NULL, 16);
            } else if (strcmp(buttons_text, "-")) {
                char *token = strtok(buttons_text, "+|,");
                while (token) {
                    int bit = bit_for(token);
                    if (bit >= 0) mask |= (uint8_t)(1u << bit);
                    else fprintf(stderr, "zanac: unknown button '%s'\n", token);
                    token = strtok(NULL, "+|,");
                }
            }
        }
        if (g_count < 256) {
            g_steps[g_count].frame = frame;
            g_steps[g_count].buttons = mask;
            g_count++;
        }
    }
    fclose(fp);
    return 0;
}

void input_script_set_offset(long frames)
{
    g_offset = frames;
}

uint8_t input_script_buttons(long frame)
{
    uint8_t mask = 0;
    int i;

    frame -= g_offset;
    for (i = 0; i < g_count; i++) {
        if (g_steps[i].frame <= frame)
            mask = g_steps[i].buttons;
        else
            break;
    }
    return mask;
}
