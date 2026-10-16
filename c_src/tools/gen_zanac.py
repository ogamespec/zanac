#!/usr/bin/env python3
"""Generate the machine-generated Zanac port headers, tables and ROM data.

Inputs (relative to the repository root):
  src/vars.asm            RAM variable equates with their 6502 addresses
  src/bank4..bank7.asm    the original sources (label names and procedure spans)
  Build/bank4..bank7.lst  DASM listings (source line -> 6502 address)
  Build/bank4..bank7.bin  the assembled banks (the game's data)

Outputs:
  c_src/zanac/vars.h          every RAM variable as RAM[<address>] + comment
  c_src/zanac/labels.h        one prototype per original *procedure* + address
  c_src/zanac/labels_addr.h   name -> 6502 address (every label, incl. inner)
  c_src/zanac/rom/zanac_banks.{h,c}   the assembled banks as C arrays
  c_src/analysis/procedures.json      procedure map (bank, name, addr, lines)
  c_src/analysis/labels.json          every label with kind and address

The DASM listing column layout is:
    <source-line>  <address>  <bytes>  <source text>
and the source-line numbers of bankN.asm entries match bankN.asm exactly, so a
label on src/bankN.asm:<line> has the address found on bankN.lst:<line>.

Classification
  code procedure   a code label that is not `loc_*`/`locret_*` and is not inside
                   another procedure (those markers come from the original
                   `; End of function <name>` comments)
  inner label      `loc_*` / `locret_*`, or a code label inside a procedure body
  data label       the first thing after the label is a .BYTE/.WORD/.RES
  thunk            3-byte `JMP <target>` body
"""
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import asm_check

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SRC = os.path.join(ROOT, "src")
BUILD = os.path.join(ROOT, "Build")
OUT = os.path.join(ROOT, "c_src", "zanac")
ANALYSIS = os.path.join(ROOT, "c_src", "analysis")

MNEMONIC = re.compile(
    r"^\s*(?:[A-Za-z_][A-Za-z0-9_]*:\s*)*"
    r"(ADC|AND|ASL|BCC|BCS|BEQ|BIT|BMI|BNE|BPL|BRK|BVC|BVS|CLC|CLD|CLI|CLV|CMP|CPX|CPY|"
    r"DEC|DEX|DEY|EOR|INC|INX|INY|JMP|JSR|LDA|LDX|LDY|LSR|NOP|ORA|PHA|PHP|PLA|PLP|ROL|ROR|"
    r"RTI|RTS|SBC|SEC|SED|SEI|STA|STX|STY|TAX|TAY|TSX|TXA|TXS|TYA|DCP|ISB|LAX|SAX|SLO|RLA|SRE|RRA)\b",
    re.IGNORECASE)
DIRECTIVE = re.compile(r"^\s*(?:[A-Za-z_][A-Za-z0-9_]*:\s*)*(\.\w+)", re.IGNORECASE)


def read_text(path):
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        return fh.read()


# --------------------------------------------------------------------- vars --
def parse_vars():
    out, seen = [], set()
    for raw in read_text(os.path.join(SRC, "vars.asm")).splitlines():
        m = re.match(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s+equ\s+\$([0-9A-Fa-f]+)\s*(;.*)?$", raw)
        if not m:
            continue
        name, addr, comment = m.group(1), int(m.group(2), 16), (m.group(3) or "")
        if (name, addr) in seen:
            continue
        seen.add((name, addr))
        out.append({"name": name, "addr": addr, "comment": comment.lstrip(";").strip()})
    return out


# -------------------------------------------------------------------- labels --
def parse_lst(path):
    addr_by_line = {}
    for raw in read_text(path).splitlines():
        m = re.match(r"^\s*(\d+)\s+([0-9A-Fa-f]{4})\s", raw.rstrip("\r\n") + " ")
        if not m:
            continue
        lineno = int(m.group(1))
        if lineno:
            addr_by_line[lineno] = int(m.group(2), 16)
    return addr_by_line


def scan_asm(asm_path, addr_by_line):
    """Every label in the file with its kind, address and enclosing procedure."""
    lines = read_text(asm_path).splitlines()
    labels = []

    def address_for(lineno):
        if lineno in addr_by_line:
            return addr_by_line[lineno]
        for probe in range(lineno + 1, min(lineno + 400, len(lines) + 1)):
            if probe in addr_by_line:
                return addr_by_line[probe]
        return None

    def kind_for(idx):
        """Look forward from a label for the first instruction or directive."""
        for probe in range(idx, min(idx + 60, len(lines))):
            text = lines[probe]
            # strip an optional leading label
            body = re.sub(r"^\s*[A-Za-z_][A-Za-z0-9_]*:", "", text)
            body = body.split(";")[0]
            if not body.strip():
                continue
            if DIRECTIVE.match(text):
                return "data"
            if MNEMONIC.match(text):
                return "code"
            return "data"
        return "data"

    for idx, raw in enumerate(lines):
        m = re.match(r"^([A-Za-z_][A-Za-z0-9_]*):", raw)
        if not m:
            continue
        name = m.group(1)
        lineno = idx + 1
        addr = address_for(lineno)
        if addr is None:
            sys.stderr.write("warning: no address for %s:%d (%s)\n" % (asm_path, lineno, name))
            continue
        labels.append({
            "name": name, "addr": addr, "line": lineno,
            "kind": kind_for(idx),
            "inner": name.startswith("loc_") or name.startswith("locret_"),
        })

    # Assign each label to the procedure that encloses it, using the original
    # "; End of function <name>" markers.
    procedures = []
    current = None
    by_line = {lab["line"]: lab for lab in labels}
    for idx, raw in enumerate(lines):
        lineno = idx + 1
        lab = by_line.get(lineno)
        if lab and lab["kind"] == "code" and not lab["inner"]:
            current = {"name": lab["name"], "addr": lab["addr"], "start": lineno,
                       "end": None, "inner": []}
            procedures.append(current)
        elif lab and current is not None and lab["addr"] >= current["addr"]:
            current["inner"].append(lab["name"])
        m = re.match(r"^\s*;\s*End of function\s+([A-Za-z_][A-Za-z0-9_]*)", raw)
        if m and current is not None and current["name"] == m.group(1):
            current["end"] = lineno
            current = None

    # A code label that follows the end of the previous procedure body without a
    # new "; End of function" marker (chopped procedures) closes the old one.
    return labels, procedures


def strip_asm(path):
    return read_text(path).splitlines()


def procedure_thunk(lines, proc):
    """True when the procedure body is a single 3-byte JMP."""
    body = []
    for idx in range(proc["start"], min(proc["start"] + 5, len(lines))):
        text = lines[idx].split(";")[0].strip()
        if not text or re.match(r"^[A-Za-z_][A-Za-z0-9_]*:$", text):
            continue
        body.append(text)
        if len(body) >= 2:
            break
    return bool(body) and re.match(r"^JMP\s+\S+$", body[0], re.IGNORECASE) is not None


# --------------------------------------------------------------- generation ---
def gen_vars(variables):
    b = []
    b.append("/* vars.h - every Zanac RAM variable, at its original 6502 address.")
    b.append(" *")
    b.append(" * GENERATED by c_src/tools/gen_zanac.py from src/vars.asm - do not edit.")
    b.append(" *")
    b.append(" * Each name expands to the 6502 ADDRESS of the variable, so a ported")
    b.append(" * statement looks exactly like the assembly it came from:")
    b.append(" *")
    b.append(" *      LDA(byte_90)        is  LDA $90")
    b.append(" *      STA(PPU_CTRL_REG1)  is  STA $2000")
    b.append(" *      LDA(ABSX(byte_542)) is  LDA $542,X")
    b.append(" *")
    b.append(" * The whole variable space is the 2 KB of NES work RAM at $0000-$07FF")
    b.append(" * (zero page, the $0100 stack page, the $0200 OAM buffer, the $0300 APU")
    b.append(" * engine state, the $0400 background engine and the $0500-$07FF object")
    b.append(" * arrays).  Hand-written glue code reads a variable with MEM8(name) and")
    b.append(" * writes it with MEMW(name, value).")
    b.append(" */")
    b.append("#ifndef ZANAC_VARS_H")
    b.append("#define ZANAC_VARS_H")
    b.append("")
    page = None
    for var in variables:
        if page is not None and (var["addr"] >> 8) != page:
            b.append("")
            b.append("/* ---- $%03X page ---- */" % (var["addr"] & 0xFF00))
        page = var["addr"] >> 8
        line = "#define %-28s 0x%04X" % (var["name"], var["addr"])
        if var["comment"]:
            line = "%-52s/* %s */" % (line, var["comment"])
        b.append(line)
    b.append("")
    b.append("#endif /* ZANAC_VARS_H */")
    return "\n".join(b) + "\n"


def gen_rom_labels(banks):
    """Plain names for the data labels of the original banks, so the ported
    code can write `LDA(randomize_tab)` exactly like the assembly."""
    b = []
    b.append("/* rom_labels.h - the 6502 address of every DATA label of the ROM banks.")
    b.append(" *")
    b.append(" * GENERATED by c_src/tools/gen_zanac.py - do not edit.")
    b.append(" * These names expand to an address, exactly like the variable names of")
    b.append(" * vars.h, so `LDA(randomize_tab)` is the original `LDA randomize_tab`.")
    b.append(" * The bytes themselves are read from the extracted bank arrays")
    b.append(" * (rom/zanac_banks.c) through MEM8()/zANAC_* pointers.")
    b.append(" */")
    b.append("#ifndef ZANAC_ROM_LABELS_H")
    b.append("#define ZANAC_ROM_LABELS_H")
    b.append("")
    seen = {}
    for bank, _procs, labels in banks:
        b.append("/* ---- bank %d ---- */" % bank)
        for lab in labels:
            if lab["kind"] != "data":
                continue
            if lab["name"] in seen and seen[lab["name"]] != lab["addr"]:
                print("warning: data label %s appears at $%04X and $%04X"
                      % (lab["name"], seen[lab["name"]], lab["addr"]))
                continue
            seen[lab["name"]] = lab["addr"]
            b.append("#define %-28s 0x%04X" % (lab["name"], lab["addr"]))
        b.append("")
    b.append("#endif /* ZANAC_ROM_LABELS_H */")
    return "\n".join(b) + "\n"


def gen_labels(banks):
    b = []
    b.append("/* labels.h - one C prototype per original 6502 procedure.")
    b.append(" *")
    b.append(" * GENERATED by c_src/tools/gen_zanac.py from src/bankN.asm and")
    b.append(" * Build/bankN.lst.  The address in the trailing comment is the original")
    b.append(" * entry point, so a definition can be cross-checked against the assembly")
    b.append(" * line by line.  Inner labels (loc_*) are branch targets inside the body")
    b.append(" * of a procedure and are implemented with C labels, not as functions.")
    b.append(" */")
    b.append("#ifndef ZANAC_LABELS_H")
    b.append("#define ZANAC_LABELS_H")
    b.append("")
    b.append('#include "zanac_ram.h"')
    b.append("")
    for bank, procs, _labels in banks:
        lo, hi = (0x8000, 0xBFFF) if bank == 6 else (0xC000, 0xFFFF)
        b.append("/* ================= bank %d ($%04X-$%04X): %d procedures ================= */"
                 % (bank, lo, hi, len(procs)))
        for proc in procs:
            b.append("void %-40s(void);   /* $%04X */" % (proc["name"], proc["addr"]))
        b.append("")
    b.append("#endif /* ZANAC_LABELS_H */")
    return "\n".join(b) + "\n"


def gen_labels_addr(banks):
    b = []
    b.append("/* labels_addr.h - the 6502 address of every original label.")
    b.append(" *")
    b.append(" * GENERATED by c_src/tools/gen_zanac.py - used for the address comments in")
    b.append(" * the ported code and by the porting tools.")
    b.append(" */")
    b.append("#ifndef ZANAC_LABELS_ADDR_H")
    b.append("#define ZANAC_LABELS_ADDR_H")
    b.append("")
    for bank, _procs, labels in banks:
        b.append("/* ---- bank %d ---- */" % bank)
        for lab in labels:
            b.append("#define ADDR_%-36s 0x%04X" % (lab["name"], lab["addr"]))
        b.append("")
    b.append("#endif /* ZANAC_LABELS_ADDR_H */")
    return "\n".join(b) + "\n"


def gen_rom(bank_bins):
    h = []
    h.append("/* zanac_banks.h - the assembled PRG banks of Zanac (USA) as C arrays.")
    h.append(" *")
    h.append(" * GENERATED by c_src/tools/gen_zanac.py from Build/bankN.bin.")
    h.append(" * Banks 0-3 are empty, bank 4 holds graphics data, bank 5 level data and")
    h.append(" * banks 6/7 code plus the data tables the ported code reads through the")
    h.append(" * bank window.  The C port reads these arrays instead of emulating the")
    h.append(" * UxROM mapper: switching a bank is just changing the index used by")
    h.append(" * zanac_bank_read().")
    h.append(" */")
    h.append("#ifndef ZANAC_BANKS_H")
    h.append("#define ZANAC_BANKS_H")
    h.append("")
    h.append("#include <stdint.h>")
    h.append("")
    h.append("#define ZANAC_BANK_SIZE  16384u")
    h.append("#define ZANAC_BANK_COUNT 8")
    h.append("")
    h.append("extern const uint8_t zanac_bank_empty[ZANAC_BANK_SIZE];")
    for n in (4, 5, 6, 7):
        h.append("extern const uint8_t zanac_bank%d[ZANAC_BANK_SIZE];" % n)
    h.append("")
    h.append("/* All banks, indexed by the value written to the bank register. */")
    h.append("extern const uint8_t *const zanac_banks[ZANAC_BANK_COUNT];")
    h.append("")
    h.append("#endif /* ZANAC_BANKS_H */")

    c = []
    c.append("/* zanac_banks.c - the assembled PRG banks of Zanac (USA).")
    c.append(" *")
    c.append(" * GENERATED by c_src/tools/gen_zanac.py from Build/bankN.bin - do not edit.")
    c.append(" */")
    c.append('#include "zanac_banks.h"')
    c.append("")
    c.append("const uint8_t zanac_bank_empty[ZANAC_BANK_SIZE] = { 0 };")
    c.append("")
    for n in (4, 5, 6, 7):
        data = bank_bins[n]
        c.append("/* bank %d: %d bytes assembled */" % (n, len(data)))
        c.append("const uint8_t zanac_bank%d[ZANAC_BANK_SIZE] = {" % n)
        for off in range(0, len(data), 16):
            c.append("    " + " ".join("0x%02X," % x for x in data[off:off + 16]))
        c.append("};")
        c.append("")
    c.append("const uint8_t *const zanac_banks[ZANAC_BANK_COUNT] = {")
    c.append("    zanac_bank_empty, zanac_bank_empty, zanac_bank_empty, zanac_bank_empty,")
    c.append("    zanac_bank4, zanac_bank5, zanac_bank6, zanac_bank7")
    c.append("};")
    return "\n".join(h) + "\n", "\n".join(c) + "\n"


def main():
    variables = parse_vars()
    banks = []
    all_records = []
    bank_bins = {}
    for n in (4, 5, 6, 7):
        with open(os.path.join(BUILD, "bank%d.bin" % n), "rb") as fh:
            bank_bins[n] = fh.read()

    # Every bank is scanned for its labels; only banks 6 and 7 contain code.
    data_banks = []
    for bank, asm in ((4, "bank4.asm"), (5, "bank5.asm"), (6, "bank6.asm"), (7, "bank7.asm")):
        lst = parse_lst(os.path.join(BUILD, "bank%d.lst" % bank))
        labels, procs = scan_asm(os.path.join(SRC, asm), lst)
        lines = strip_asm(os.path.join(SRC, asm))
        for proc in procs:
            proc["thunk"] = procedure_thunk(lines, proc)
            proc["end_line"] = proc["end"] or proc["start"]
        for idx, proc in enumerate(procs):
            if proc["end"] is None:
                proc["end_line"] = procs[idx + 1]["start"] - 1 if idx + 1 < len(procs) else len(lines)
            else:
                proc["end_line"] = proc["end"]
        if bank in (4, 5):
            # Banks 4 and 5 are pure data and their DASM listings predate the
            # current sources, so their label addresses are taken from a fresh
            # assembly of the sources instead (asm_check.py verifies the result
            # is byte-identical to Build/bankN.bin).
            exact, _image = asm_check.assemble_data_bank(bank)
            fixed = []
            for lab in labels:
                if lab["name"] in exact:
                    lab["addr"] = exact[lab["name"]]
                    fixed.append(lab)
                else:
                    sys.stderr.write("warning: bank %d label %s is not in the "
                                     "reassembled image\n" % (bank, lab["name"]))
            labels = fixed
        data_banks.append((bank, procs, labels))
        if bank in (6, 7):
            banks.append((bank, procs, labels))
            for lab in labels:
                all_records.append({"bank": bank, "name": lab["name"], "addr": lab["addr"],
                                    "kind": lab["kind"], "inner": lab["inner"]})
        print("bank %d: %d procs, %d labels, %d thunks"
              % (bank, len(procs), len(labels), sum(1 for p in procs if p["thunk"])))

    os.makedirs(os.path.join(OUT, "rom"), exist_ok=True)
    os.makedirs(ANALYSIS, exist_ok=True)
    write = lambda p, s: open(p, "w", encoding="utf-8", newline="\n").write(s)
    write(os.path.join(OUT, "vars.h"), gen_vars(variables))
    write(os.path.join(OUT, "labels.h"), gen_labels(banks))
    write(os.path.join(OUT, "labels_addr.h"), gen_labels_addr(banks))
    write(os.path.join(OUT, "rom_labels.h"), gen_rom_labels(data_banks))
    rh, rc = gen_rom(bank_bins)
    write(os.path.join(OUT, "rom", "zanac_banks.h"), rh)
    write(os.path.join(OUT, "rom", "zanac_banks.c"), rc)

    procs_json = []
    for bank, procs, _labels in banks:
        for p in procs:
            procs_json.append({"bank": bank, "name": p["name"], "addr": p["addr"],
                               "start": p["start"], "end": p["end_line"],
                               "inner": p["inner"], "thunk": p["thunk"]})
    write(os.path.join(ANALYSIS, "procedures.json"),
          json.dumps(procs_json, indent=1) + "\n")
    write(os.path.join(ANALYSIS, "labels.json"),
          json.dumps(all_records, indent=1) + "\n")
    print("vars: %d" % len(variables))


if __name__ == "__main__":
    main()
