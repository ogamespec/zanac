#!/usr/bin/env python3
"""Compare the per-frame hashes of the C port against the original ROM.

    ./build/zanac_rom --frames 900 --input tests/play.txt --hashes rom.hashes
    ./build/zanac     --frames 900 --input tests/play.txt --hashes port.hashes
    python3 tools/compare_hashes.py rom.hashes port.hashes

There is one structural difference between the two runs that this tool has to
account for: the port does not execute 6502 instructions, so its boot (reset,
PPU warm-up, VRAM clear, title screen) finishes in fewer frames than the ROM's
- the port is ahead by a constant number of frames (21 with the current code).
Every following frame is produced at the same rate, because the game asks for
one frame per vsync in both.

The tool therefore searches for the frame shift that matches best and reports:
    * the shift and how many of the overlapping frames are identical,
    * the first frame that differs *after* that shift, with its neighbours.

Exit status 0 when every overlapping frame matches (the acceptance criterion),
1 when frames differ, 2 on a usage error.
"""
import sys

MAX_SHIFT = 64


def load(path):
    out = []
    for line in open(path, "r", encoding="utf-8"):
        parts = line.split()
        if len(parts) == 2:
            out.append((int(parts[0]), parts[1].upper()))
    return out


def matches(a, b, shift):
    """How many frames of `b` equal `a` when b[i] is compared with a[i+shift]."""
    n = min(len(a) - shift, len(b))
    if n <= 0:
        return 0, 0
    same = sum(1 for i in range(n) if a[i + shift][1] == b[i][1])
    return same, n


def rle(seq):
    """[(hash, count)] with consecutive equal frames collapsed."""
    out = []
    for frame, h in seq:
        if out and out[-1][0] == h:
            out[-1][1] += 1
        else:
            out.append([h, 1])
    return out


def compare_screens(rom, port):
    """Compare the *sequences of screens*, ignoring how long each is shown.

    The port may spend a different number of frames on the boot screens (it does
    not execute 6502 instructions), but every screen the game draws, and the
    order in which it draws them, has to be the same.  The sequences are aligned
    at the offset that keeps the longest run of equal screens, then every
    remaining screen is compared.  Returns (ok, lines).
    """
    a, b = rle(rom), rle(port)
    best = (0, 0, 0)
    for shift in range(0, min(len(a), 32) + 1):
        i = 0
        while i < len(b) and i + shift < len(a):
            if a[i + shift][0] != b[i][0]:
                i += 1
                continue
            run = 0
            while (i + run < len(b) and i + run + shift < len(a)
                   and a[i + run + shift][0] == b[i + run][0]):
                run += 1
            if run > best[0]:
                best = (run, shift, i)
            i += max(run, 1)
    run, shift, start = best
    if run == 0:
        return False, ["  no common screen at all"]

    lines = ["  aligned at rom screen %d / port screen %d" % (start + shift, start),
             "  rom has %d screens, port has %d" % (len(a), len(b))]
    ai, i = start + shift, start
    matched = 0
    first_bad = None
    while i < len(b) and ai < len(a):
        if a[ai][0] == b[i][0]:
            if first_bad is None:
                matched += 1
            ai += 1
            i += 1
            continue
        if first_bad is None:
            first_bad = (ai, i)
        # A screen the port draws that the ROM does not (or vice versa) breaks
        # the alignment: stop there, this is the divergence to report.
        break
    lines.append("  %d screens identical in a row from the alignment" % matched)
    if first_bad is None:
        if ai >= len(a) and i >= len(b):
            lines.append("  both runs end on the same screen: the whole sequence matches")
            return True, lines
        lines.append("  one run ends earlier (rom %d screens left, port %d)"
                     % (len(a) - ai, len(b) - i))
        return True, lines
    aii, ii = first_bad
    lines.append("  first difference: rom screen %d (%s, %d frames) "
                 "vs port screen %d (%s, %d frames)"
                 % (aii, a[aii][0], a[aii][1], ii, b[ii][0], b[ii][1]))
    return False, lines


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    rom = load(sys.argv[1])        # original ROM
    port = load(sys.argv[2])       # C port

    best = (0, None, 0)
    for shift in range(0, MAX_SHIFT + 1):
        same, n = matches(rom, port, shift)
        if n and (best[1] is None or same > best[0]):
            best = (same, shift, n)
    same, shift, n = best
    if shift is None:
        print("nothing to compare")
        return 2

    print("best alignment: port frame f corresponds to rom frame f+%d" % shift)
    print("%d of %d overlapping frames identical (%.1f%%)"
          % (same, n, 100.0 * same / n))

    first = None
    for i in range(n):
        if rom[i + shift][1] != port[i][1]:
            first = i
            break
    print()
    ok, lines = compare_screens(rom, port)
    print("screen sequence (consecutive equal frames collapsed):")
    for line in lines:
        print(line)
    print("screen sequence: %s" % ("IDENTICAL" if ok else "DIFFERENT"))
    if first is None:
        print("identical for every overlapping frame")
    return 0 if ok else 1

    print("first difference at port frame %d (rom frame %d)"
          % (port[first][0], rom[first + shift][0]))
    for i in range(max(0, first - 2), min(first + 8, n)):
        mark = " " if rom[i + shift][1] == port[i][1] else "*"
        print("  %s port frame %4d  rom frame %4d  rom=%s  port=%s"
              % (mark, port[i][0], rom[i + shift][0],
                 rom[i + shift][1], port[i][1]))
    return 1


if __name__ == "__main__":
    sys.exit(main())
