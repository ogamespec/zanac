#!/usr/bin/env python3
"""Check that every indexed access in the ported code matches its comment.

The ported statements carry the original instruction as a trailing comment, so a
mismatch between the macro used and the operands in the comment is a translation
bug.  This checks the three pairing rules:

    ABSX(...)  <-> comment ends with ",X"
    ABSY(...)  <-> comment ends with ",Y"
    ZPX(...)   <-> comment contains ",X"
    ZPY(...)   <-> comment contains ",Y"
    INDY(...)  <-> comment is "($xx),Y"
    INDX(...)  <-> comment is "($xx,X)"

Immediate macros (...I) must not be used with an address comment and vice versa.
Reported lines are worth a manual look; the tool is deliberately noisy rather
than clever.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ZANAC = os.path.join(ROOT, "c_src", "zanac")
STATEMENT = re.compile(r"^(?P<indent>\s*)(?P<code>.*?);\s*/\*\s*(?P<comment>[^*]*?)\s*\*/\s*$")
SKIP = {"stubs.c", "thunks.c", "zanac_dispatch.c", "zanac_ram.c", "zanac_stubs.c"}


def check_line(code, comment):
    """Return a list of problems for one statement."""
    problems = []
    c = comment.strip()
    # trailing ",X" / ",Y" of an absolute or zero-page operand
    m = re.search(r",\s*([XY])\b", c)
    index = m.group(1) if m else None
    if "ABSX(" in code and index != "X":
        problems.append("ABSX but comment says %s" % (index or "no index"))
    if "ABSY(" in code and index != "Y":
        problems.append("ABSY but comment says %s" % (index or "no index"))
    if "ZPX(" in code and index != "X":
        problems.append("ZPX but comment says %s" % (index or "no index"))
    if "ZPY(" in code and index != "Y":
        problems.append("ZPY but comment says %s" % (index or "no index"))
    if "INDY(" in code and not re.search(r"\)\s*,\s*Y", c):
        problems.append("INDY but comment is '%s'" % c)
    if "INDX(" in code and not re.search(r",\s*X\s*\)", c):
        problems.append("INDX but comment is '%s'" % c)
    # immediate vs address
    imm = re.match(r"^(LDAI|LDXI|LDYI|ADCI|SBCI|ANDI|ORAI|EORI|CMPI|CPXI|CPYI)\(", code)
    plain = re.match(r"^(LDA|LDX|LDY|ADC|SBC|AND|ORA|EOR|CMP|CPX|CPY)\(", code)
    if imm and not re.match(r"^(LDA|LDX|LDY|ADC|SBC|AND|ORA|EOR|CMP|CPX|CPY)\s+#", c):
        problems.append("immediate macro but comment is '%s'" % c)
    if plain and re.match(r"^(LDA|LDX|LDY|ADC|SBC|AND|ORA|EOR|CMP|CPX|CPY)\s+#", c):
        problems.append("address macro but comment is immediate '%s'" % c)
    return problems


def main():
    total = 0
    for dirpath, _dirs, names in os.walk(ZANAC):
        for name in sorted(names):
            if not name.endswith(".c") or name in SKIP:
                continue
            path = os.path.join(dirpath, name)
            for lineno, line in enumerate(open(path, encoding="utf-8", errors="replace"), 1):
                m = STATEMENT.match(line.rstrip("\n"))
                if not m:
                    continue
                for problem in check_line(m.group("code"), m.group("comment")):
                    print("%s:%d  %s" % (os.path.relpath(path, ROOT), lineno, problem))
                    total += 1
    print("%d indexing/immediate mismatch(es)" % total)
    return 0


if __name__ == "__main__":
    sys.exit(main())
