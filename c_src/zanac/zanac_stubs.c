/* zanac_stubs.c - bookkeeping for the not-yet-ported procedures. */
#include <stdio.h>
#include <string.h>

#include "zanac_stubs.h"

#define ZANAC_STUB_MAX 2048

typedef struct {
    const char *name;
    uint16_t    addr;
    unsigned    hits;
} ZanacStubHit;

static ZanacStubHit s_hits[ZANAC_STUB_MAX];
static int         s_count;

void zanac_stub_hit(const char *name, uint16_t addr)
{
    int i;
    for (i = 0; i < s_count; i++) {
        if (s_hits[i].addr == addr) {
            s_hits[i].hits++;
            return;
        }
    }
    if (s_count < ZANAC_STUB_MAX) {
        s_hits[s_count].name = name;
        s_hits[s_count].addr = addr;
        s_hits[s_count].hits = 1;
        s_count++;
    }
}

int zanac_stub_count(void)
{
    return s_count;
}

void zanac_stub_report(void)
{
    int i;
    fprintf(stderr, "zanac: %d procedure(s) still not ported:\n", s_count);
    for (i = 0; i < s_count; i++)
        fprintf(stderr, "  $%04X %-32s %u call(s)\n",
                s_hits[i].addr, s_hits[i].name, s_hits[i].hits);
}
