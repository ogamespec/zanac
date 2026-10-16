#!/usr/bin/env python3
"""Mechanical clean-up of the ported chunk files.

1. "shared RTS" branches
   The original frequently ends a procedure with a branch into the `locret_*`
   RTS that belongs to *another* procedure (the shared-tail idiom).  In C a
   label is function scoped, so such a branch cannot be a `goto`; it is an
   early return of the current procedure:

       BEQ(locret_187A5);              ->   if (!F_Z) goto locret_187A5_cont;
                                            RTS();
                                        locret_187A5_cont:;

   The tool only rewrites targets that are known `locret_*` labels (they are
   always an RTS).  Branches to any other label of another procedure are
   reported for manual review.

2. Unused local labels: a `locXX:` label that no statement in the same function
   references is left in place but marked, so `-Wunused-label` stays quiet.

Run it with --fix to write the changes, without it to only report.
"""
import argparse
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ZANAC = os.path.join(ROOT, "c_src", "zanac")

BRANCHES = ["BEQ", "BNE", "BCC", "BCS", "BMI", "BPL", "BVC", "BVS"]
INVERSE = {"BEQ": "BNE", "BNE": "BEQ", "BCC": "BCS", "BCS": "BCC",
           "BMI": "BPL", "BPL": "BMI", "BVC": "BVS", "BVS": "BVC"}
FLAG_OF = {"BEQ": "F_Z", "BNE": "F_Z", "BCC": "F_C", "BCS": "F_C",
           "BMI": "F_N", "BPL": "F_N", "BVC": "F_V", "BVS": "F_V"}

FUNC = re.compile(r"^void\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*void\s*\)[^\n{]*$", re.MULTILINE)
LABEL = re.compile(r"^(loc(?:ret)?_[0-9A-Fa-f]+)\s*:", re.MULTILINE)
BRANCH_CALL = re.compile(r"^\s*(%s)\((loc(?:ret)?_[0-9A-Fa-f]+)\);" % "|".join(BRANCHES),
                         re.MULTILINE)


def functions(text):
    """[(name, body_start, body_end)] for every `void name(void)` definition."""
    out = []
    for m in FUNC.finditer(text):
        brace = text.find("{", m.end())
        if brace < 0:
            continue
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


def known_labels():
    path = os.path.join(ZANAC, "labels_addr.h")
    names = set()
    with open(path, "r", encoding="utf-8") as fh:
        for line in fh:
            m = re.match(r"#define ADDR_([A-Za-z_][A-Za-z0-9_]*)", line)
            if m:
                names.add(m.group(1))
    return names


CONT_PATTERN = re.compile(
    r"^(?P<indent>[ \t]*)/\*[^\n]*\*/\n"
    r"[ \t]*(?P<inv>%s)\((?P<target>locret_[0-9A-Fa-f]+)_cont\);[^\n]*\n"
    r"[ \t]*RTS\(\);[^\n]*\n"
    r"[ \t]*locret_[0-9A-Fa-f]+_cont:;[^\n]*\n" % "|".join(BRANCHES),
    re.MULTILINE)


def simplify_cont_labels(text):
    """Rewrite the `Bxx(target_cont); RTS(); target_cont:;` idiom (which needs a
    synthetic, per-function-unique label) into the Bxx_RET(target) macro."""
    def repl(match):
        indent = match.group("indent")
        target = match.group("target")
        op = INVERSE[match.group("inv")]          # the inverse of the inverse
        return ("%s/* %s %s - the shared RTS of another procedure */\n"
                "%s%s_RET(%s);\n" % (indent, op, target, indent, op, target))
    return CONT_PATTERN.sub(repl, text)


def process(path, labels, fix):
    with open(path, "r", encoding="utf-8") as fh:
        text = fh.read()
    original = text
    changes = []
    for name, start, end in functions(text):
        body = text[start:end]
        defined = set(LABEL.findall(body))
        for m in list(BRANCH_CALL.finditer(text[start:end])):
            op, target = m.group(1), m.group(2)
            if target in defined:
                continue
            if target not in labels:
                continue
            line_start = start + m.start()
            line_end = start + m.end()
            indent = re.match(r"\s*", text[line_start:]).group(0)
            if not target.startswith("locret_"):
                changes.append(("manual", name, op, target))
                continue
            cont = target + "_cont"
            replacement = ("%s/* %s %s - the shared RTS of another procedure */\n"
                           "%s%s(%s);\n"
                           "%sRTS();\n"
                           "%s%s:;" % (indent, op, target, indent,
                                       INVERSE[op], cont, indent, indent, cont))
            text = text[:line_start] + replacement + text[line_end:]
            changes.append(("fixed", name, op, target))
            break   # offsets changed; reprocess this file
    simplified = simplify_cont_labels(text)
    if simplified != text:
        changes.append(("simplified", "", "", ""))
        text = simplified
    if fix and text != original:
        with open(path, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(text)
    return changes


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fix", action="store_true")
    args = ap.parse_args()
    labels = known_labels()
    files = []
    for dirpath, _dirs, names in os.walk(ZANAC):
        for n in sorted(names):
            if n.endswith(".c") and n not in ("stubs.c", "thunks.c", "zanac_dispatch.c",
                                              "zanac_ram.c", "zanac_stubs.c"):
                files.append(os.path.join(dirpath, n))
    total_fixed = total_manual = 0
    for path in files:
        while True:
            changes = process(path, labels, args.fix)
            if not args.fix:
                break
            if not any(c[0] == "fixed" for c in changes):
                break
        for kind, name, op, target in changes:
            rel = os.path.relpath(path, ROOT)
            if kind == "fixed":
                total_fixed += 1
                print("fixed  %s: %s in %s -> early return" % (rel, op + "(" + target + ")", name))
            elif kind == "simplified":
                total_fixed += 1
                print("fixed  %s: shared-RTS branches rewritten to Bxx_RET" % rel)
            else:
                total_manual += 1
                print("REVIEW %s: %s in %s branches into another procedure"
                      % (rel, op + "(" + target + ")", name))
    print("shared-RTS branches fixed: %d, needing review: %d" % (total_fixed, total_manual))
    return 0


if __name__ == "__main__":
    sys.exit(main())
