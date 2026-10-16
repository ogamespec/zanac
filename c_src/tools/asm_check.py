#!/usr/bin/env python3
"""Cross-checks between the original sources, the DASM listings and the
assembled banks, plus an exact label map for the data-only banks.

1. `--check` verifies that the byte columns of Build/bankN.lst agree with
   Build/bankN.bin at the addresses the listing reports.  The listing and the
   binary come from the same assembly run, so agreement means the labels the
   generator extracted carry the addresses of the shipped ROM.

2. The data-only banks (4 and 5) are assembled here from their .asm sources
   (they contain only `org`, `.byte` and `.word`) and the result is compared
   byte for byte with Build/bankN.bin.  That is what proves the stale listings
   of those two banks did not misplace their labels, and it produces an exact
   label map in c_src/analysis/data_labels.json.
"""
import argparse
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SRC = os.path.join(ROOT, "src")
BUILD = os.path.join(ROOT, "Build")
ANALYSIS = os.path.join(ROOT, "c_src", "analysis")


def read_lines(path):
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        return [l.rstrip("\r\n") for l in fh]


def strip_comment(text):
    out, in_char = [], False
    for i, ch in enumerate(text):
        if ch == "'":
            in_char = not in_char
        if ch == ";" and not in_char:
            break
        out.append(ch)
    return "".join(out)


# ------------------------------------------------------------ listing check --
def parse_lst(path, main):
    entries = []
    current = None
    for raw in read_lines(path):
        m = re.match(r"^-+\s*FILE\s+(\S+)", raw)
        if m:
            current = m.group(1).replace("\\", "/").rsplit("/", 1)[-1]
            continue
        if current != main:
            continue
        parts = raw.split("\t")
        head = re.match(r"^\s*(\d+)\s+([0-9A-Fa-f]{4})\s*$", parts[0])
        if not head:
            continue
        lineno = int(head.group(1))
        if lineno == 0:
            continue
        addr = int(head.group(2), 16)
        bytes_field = parts[2] if len(parts) > 2 else ""
        # The byte column is 16 bytes max; a '*' introduces a label definition.
        hexpart = bytes_field.split("*")[0]
        data = []
        for token in hexpart.split():
            if re.fullmatch(r"[0-9A-Fa-f]{2}", token):
                data.append(int(token, 16))
        entries.append((lineno, addr, data))
    return entries


def check_listing(bank):
    entries = parse_lst(os.path.join(BUILD, "bank%d.lst" % bank), "bank%d.asm" % bank)
    with open(os.path.join(BUILD, "bank%d.bin" % bank), "rb") as fh:
        image = fh.read()
    base = 0x8000 if bank < 7 else 0xC000
    bad = 0
    checked = 0
    for lineno, addr, data in entries:
        off = addr - base
        if off < 0 or off + len(data) > len(image):
            continue
        for i, byte in enumerate(data):
            checked += 1
            if image[off + i] != byte:
                bad += 1
                if bad < 8:
                    print("  bank%d lst line %d: $%04X expected %02X, image has %02X"
                          % (bank, lineno, addr + i, byte, image[off + i]))
    return checked, bad


# -------------------------------------------------- data-only bank assembly --
SYMBOL = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)\s*:\s*(.*)$")
EQU = re.compile(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s+equ\s+\$([0-9A-Fa-f]+)\s*$", re.IGNORECASE)


def eval_operand(token, symbols):
    token = token.strip()
    if token.startswith("$"):
        return int(token[1:], 16)
    if token.startswith("%"):
        return int(token[1:], 2)
    if re.fullmatch(r"\d+", token):
        return int(token)
    if len(token) == 3 and token[0] == "'" and token[2] == "'":
        return ord(token[1])
    if token in symbols:
        return symbols[token]
    raise ValueError("unknown operand %r" % token)


def split_operands(text):
    return [t for t in (x.strip() for x in text.split(",")) if t]


def assemble_data_bank(bank):
    """Two-pass assembly of a data-only bank; returns (labels, bytes)."""
    path = os.path.join(SRC, "bank%d.asm" % bank)
    symbols = {}
    labels = {}

    def pass_one():
        pc = None
        for raw in read_lines(path):
            text = strip_comment(raw)
            m = EQU.match(text)
            if m:
                symbols[m.group(1)] = int(m.group(2), 16)
                continue
            m = SYMBOL.match(text.strip())
            body = text
            if m:
                name, body = m.group(1), m.group(2)
                if pc is None:
                    raise ValueError("label %s before org" % name)
                labels[name] = pc
                symbols[name] = pc
            body = body.strip()
            if not body:
                continue
            m = re.match(r"org\s+\$([0-9A-Fa-f]+)", body, re.IGNORECASE)
            if m:
                pc = int(m.group(1), 16)
                continue
            m = re.match(r"\.(byte|word|res|ds)\s*(.*)$", body, re.IGNORECASE)
            if not m:
                if body.startswith("processor") or body.startswith("include"):
                    continue
                raise ValueError("unsupported statement %r" % body)
            directive, args = m.group(1).lower(), m.group(2)
            if directive in ("res", "ds"):
                pc += int(args.strip() or 0)
                continue
            size = 1 if directive == "byte" else 2
            pc += size * len(split_operands(args))

    def pass_two(pc0=0x8000):
        pc = None
        out = bytearray()
        base = None
        for raw in read_lines(path):
            text = strip_comment(raw)
            if EQU.match(text):
                continue
            m = SYMBOL.match(text.strip())
            body = text
            if m:
                body = m.group(2)
            body = body.strip()
            if not body:
                continue
            m = re.match(r"org\s+\$([0-9A-Fa-f]+)\s*,?\s*(\d*)", body, re.IGNORECASE)
            if m:
                pc = int(m.group(1), 16)
                fill = int(m.group(2)) if m.group(2) else 0
                if base is None:
                    base = pc
                    out = bytearray()
                else:
                    # pad up to pc
                    while base + len(out) < pc:
                        out.append(fill & 0xFF)
                continue
            m = re.match(r"\.(byte|word|res|ds)\s*(.*)$", body, re.IGNORECASE)
            if not m:
                continue
            directive, args = m.group(1).lower(), m.group(2)
            if directive in ("res", "ds"):
                out.extend(b"\x00" * int(args.strip() or 0))
                continue
            for token in split_operands(args):
                value = eval_operand(token, symbols)
                if directive == "byte":
                    out.append(value & 0xFF)
                else:
                    out.append(value & 0xFF)          # DASM .WORD is little-endian
                    out.append((value >> 8) & 0xFF)
        return base, bytes(out)

    pass_one()
    _base, image = pass_two()
    return labels, image


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="verify listings against the banks")
    ap.add_argument("--write", action="store_true", help="write data_labels.json")
    args = ap.parse_args()

    if args.check:
        for bank in (4, 5, 6, 7):
            checked, bad = check_listing(bank)
            print("bank%d listing: %d bytes checked, %d mismatches" % (bank, checked, bad))

    for bank in (4, 5):
        labels, data = assemble_data_bank(bank)
        with open(os.path.join(BUILD, "bank%d.bin" % bank), "rb") as fh:
            image = fh.read()
        same = data == image
        print("bank%d reassembled: %d bytes, %s" % (bank, len(data),
              "identical to Build/bank%d.bin" % bank if same else "DIFFERENT"))
        if not same:
            for i, (a, b) in enumerate(zip(data, image)):
                if a != b:
                    print("  first difference at offset $%04X: %02X vs %02X" % (i, a, b))
                    break
        if args.write:
            with open(os.path.join(ANALYSIS, "data_labels_bank%d.json" % bank),
                      "w", encoding="utf-8", newline="\n") as fh:
                json.dump({k: v for k, v in sorted(labels.items(), key=lambda kv: kv[1])},
                          fh, indent=1)
    return 0


if __name__ == "__main__":
    sys.exit(main())
