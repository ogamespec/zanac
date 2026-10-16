#!/usr/bin/env python3
"""Find the busy-wait loops of the ported code.

The 6502 game spins on variables that the NMI handler updates (cram_update_mode,
byte_20, byte_2B, ...).  In the port the engine only advances when the game asks
for it, so every such loop must contain an engine call - otherwise it spins
forever.  This tool lists every backward branch whose body contains no engine
call, so the loops can be patched (see PORTED_BUSY_WAIT in zanac_ram.h).
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ZANAC = os.path.join(ROOT, "c_src", "zanac")
ENGINE = ("vsync()", "neslib_wait_vblank", "neslib_step_frame", "all_enemies_destroyed()",
          "neslib_pump", "sub_EA0B()", "ZANAC_WAIT_NMI", "neslib_wait_vblank_flag")
FUNC = re.compile(r"^void\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*void\s*\)[^\n{]*$", re.MULTILINE)
LABEL = re.compile(r"^(loc(?:ret)?_[0-9A-Fa-f]+)\s*:", re.MULTILINE)
BRANCH = re.compile(r"\b(BEQ|BNE|BCC|BCS|BMI|BPL|BVC|BVS|BEQ_RET|BNE_RET|BCC_RET|BCS_RET|"
                    r"BMI_RET|BPL_RET|BVC_RET|BVS_RET)\((loc(?:ret)?_[0-9A-Fa-f]+)\)")
GOTO = re.compile(r"\bgoto\s+(loc(?:ret)?_[0-9A-Fa-f]+)\s*;")


def functions(text):
    out = []
    for m in FUNC.finditer(text):
        brace = text.find("{", m.end())
        depth = 0
        i = brace
        while i < len(text):
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        out.append((m.group(1), brace, i))
    return out


def line_of(text, pos):
    return text.count("\n", 0, pos) + 1


def main():
    report = []
    for dirpath, _dirs, names in os.walk(ZANAC):
        for name in sorted(names):
            if not name.endswith(".c") or name in ("stubs.c", "thunks.c", "zanac_dispatch.c",
                                                    "zanac_ram.c", "zanac_stubs.c"):
                continue
            path = os.path.join(dirpath, name)
            text = open(path, encoding="utf-8", errors="replace").read()
            for fname, start, end in functions(text):
                body = text[start:end]
                labels = {m.group(1): m.start() for m in LABEL.finditer(body)}
                for m in list(BRANCH.finditer(body)) + list(GOTO.finditer(body)):
                    target = m.group(2) if m.lastindex and m.lastindex >= 2 else m.group(1)
                    if target not in labels:
                        continue
                    if labels[target] > m.start():
                        continue            # forward branch, not a loop
                    loop_body = body[labels[target]:m.start()]
                    if any(call in loop_body for call in ENGINE):
                        continue
                    report.append((os.path.relpath(path, ROOT), fname,
                                   line_of(text, start + m.start()), target))
    # Keep one entry per (file, function, label)
    seen = set()
    for path, fname, line, target in report:
        key = (path, fname, target)
        if key in seen:
            continue
        seen.add(key)
        print("%s:%d  %s  spins on %s" % (path, line, fname, target))
    print("%d busy-wait loop(s) without an engine call" % len(seen))
    return 0


if __name__ == "__main__":
    sys.exit(main())
