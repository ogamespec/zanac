#!/usr/bin/env python3
"""compare_ram.py - diff two 2 KB RAM dumps byte by byte, by variable name.

    tools/compare_ram.py PORT_DUMP ROM_DUMP [--all]

Both frontends write a raw 2 KB dump with `--dump-ram FILE` (the port and
`zanac_rom` use the same layout), so the interesting comparison is:

    build/zanac     --frames 317 --hashes /tmp/p.hashes --dump-ram /tmp/p.ram
    build/zanac_rom --frames 280 --hashes /tmp/r.hashes --dump-ram /tmp/r.ram
    tools/compare_ram.py /tmp/p.ram /tmp/r.ram

*Pick frames whose screens are identical* (compare_hashes.py prints the
alignment): the port reaches a given game state a few frames away from the ROM,
and dumping the same frame number on both sides compares two different states.

The names come from zanac/vars.h.  By default the report hides the scratch
areas (unk_*, and the byte_2xx..byte_9xx blocks) and the variables the frame
hook itself touches, because those legitimately differ depending on where in the
frame the dump was taken; --all shows everything.
"""
import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Variables whose value depends on where in the frame the dump was taken rather
# than on the game state.  They are reported separately instead of hiding real
# differences among them.
TRANSIENT = {"vblank_wait_flag", "ppu_status_in_vblank", "cram_update_mode",
             "byte_9C", "byte_20", "byte_90", "byte_9B"}


def load_names():
    names = {}
    path = ROOT / "zanac" / "vars.h"
    for line in path.read_text().splitlines():
        m = re.match(r"#define\s+(\w+)\s+0x([0-9A-Fa-f]{4})", line)
        if m:
            names.setdefault(int(m.group(2), 16), []).append(m.group(1))
    return names


def scratch(name, addr):
    """Zero-page scratch and the larger scratch blocks.

    The ROM leaves the operands of its multiply/divide helpers in $06-$08 and
    the port leaves zeroes there; those bytes say nothing about the game state.
    """
    return (name.startswith("unk_")
            or addr < 0x10
            or re.match(r"byte_[2-9][0-9A-Fa-f]{2}$", name or ""))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port", help="RAM dump from the port")
    ap.add_argument("rom", help="RAM dump from the ROM runner")
    ap.add_argument("--all", action="store_true",
                    help="also list the scratch areas and transients")
    args = ap.parse_args()

    port = Path(args.port).read_bytes()
    rom = Path(args.rom).read_bytes()
    if len(port) != len(rom):
        print("sizes differ: %d vs %d" % (len(port), len(rom)), file=sys.stderr)
        return 2

    names = load_names()
    differing = [a for a in range(len(port)) if port[a] != rom[a]]
    named = [(names[a][0], a) for a in differing if a in names]

    real = [(n, a) for n, a in named if n not in TRANSIENT and not scratch(n, a)]
    trans = [(n, a) for n, a in named if n in TRANSIENT]

    print("%d of %d bytes differ (%d named)" % (len(differing), len(port), len(named)))
    if real:
        print("  state:")
        for n, a in real:
            print("    %-24s $%03X  rom=%02X port=%02X" % (n, a, rom[a], port[a]))
    if trans:
        print("  transient (depends on the dump point):")
        for n, a in trans:
            print("    %-24s $%03X  rom=%02X port=%02X" % (n, a, rom[a], port[a]))
    if args.all:
        rest = [(a, names[a][0] if a in names else "") for a in differing]
        print("  all differing bytes:")
        for a, n in rest:
            print("    $%03X %-20s rom=%02X port=%02X" % (a, n, rom[a], port[a]))

    if real:
        print("RESULT: state differs")
        return 1
    print("RESULT: state identical")
    return 0


if __name__ == "__main__":
    sys.exit(main())
