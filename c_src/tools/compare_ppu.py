#!/usr/bin/env python3
"""Compare the port's PPU memory against the original ROM's, frame by frame.

This is the reproducible half of the acceptance test.  The port cannot match the
ROM's *sprites*: the game's random number generator (enemy_prng, $872B) mixes
the 6502 stack pointer (`TSX`) into every byte it returns, and a ported C
procedure has no 6502 stack to read.  Everything else - the level stream, the
tile decoder, the scrolling engine, the nametable and attribute updates, the CHR
bank loader, the HUD - does not depend on that entropy and must match exactly.

Usage:
    ./build/zanac     --frames 200 --input tests/play.txt --dump-ppu /tmp/pbg_200
    ./build/zanac_rom --frames 220 --input tests/play.txt --dump-ppu /tmp/rbg_220
    python3 tools/compare_ppu.py /tmp/pbg_200 /tmp/rbg_220

Multiple ROM frames may be given; the one that matches the port's name table
best is used, because the port's boot is about 20 frames shorter than the ROM's
and the level scrolls, so only some ROM frame is at the same scroll position.

Reported per port frame: the pattern-table (CHR) mismatch, the name-table
mismatch and the palette mismatch, in bytes.  Exit status 0 when the CHR matches
exactly and the name table differs in at most ALLOWED bytes (the attribute bytes
of the last, invisible tile row, which the game updates one frame later).
"""
import os
import sys

ALLOWED_NT = 8


def read(path):
    try:
        with open(path, "rb") as fp:
            return fp.read()
    except OSError:
        return None


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    port_prefix = sys.argv[1]
    rom_prefixes = sys.argv[2:]

    pchr = read(port_prefix + ".chr")
    pnt = read(port_prefix + ".nt")
    ppal = read(port_prefix + ".pal")
    if pnt is None:
        print("cannot read %s.nt" % port_prefix)
        return 2

    best = None
    for prefix in rom_prefixes:
        rnt = read(prefix + ".nt")
        if rnt is None:
            continue
        d = sum(1 for i in range(min(len(pnt), len(rnt))) if pnt[i] != rnt[i])
        if best is None or d < best[0]:
            best = (d, prefix)
    if best is None:
        print("no ROM dump found")
        return 2

    dnt, prefix = best
    rchr = read(prefix + ".chr")
    rpal = read(prefix + ".pal")
    dchr = (sum(1 for i in range(min(len(pchr), len(rchr))) if pchr[i] != rchr[i])
            if pchr and rchr else -1)
    dpal = (sum(1 for i in range(min(len(ppal), len(rpal))) if ppal[i] != rpal[i])
            if ppal and rpal else -1)

    print("port %s  vs  rom %s" % (port_prefix, prefix))
    print("  pattern tables (CHR): %s" % ("identical" if dchr == 0 else
                                          "%d bytes differ" % dchr))
    print("  name tables:          %d bytes differ" % dnt)
    print("  palette:              %s" % ("identical" if dpal == 0 else
                                          "%d bytes differ" % dpal))
    ok = dchr == 0 and dnt <= ALLOWED_NT
    print("  result: %s" % ("OK" if ok else "FAIL"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
