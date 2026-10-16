#!/usr/bin/env python3
"""Split the port into work chunks.

  c_src/analysis/plan.json  machine-readable chunk list
  c_src/analysis/plan.md    the same, for the humans

A chunk is a run of consecutive procedures of one bank whose combined assembly
length is close to --lines.  Each chunk maps to one file
c_src/zanac/bank<bank>/<first-address>.c.
"""
import argparse
import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ANALYSIS = os.path.join(ROOT, "c_src", "analysis")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lines", type=int, default=800, help="target assembly lines per chunk")
    args = ap.parse_args()

    procs = [p for p in json.load(open(os.path.join(ANALYSIS, "procedures.json")))
             if not p["thunk"]]
    procs.sort(key=lambda p: (p["bank"], p["addr"]))

    chunks = []
    for bank in (6, 7):
        bank_procs = [p for p in procs if p["bank"] == bank]
        current = []
        for proc in bank_procs:
            current.append(proc)
            length = sum(p["end"] - p["start"] + 1 for p in current)
            if length >= args.lines:
                chunks.append((bank, current))
                current = []
        if current:
            chunks.append((bank, current))

    out = []
    for index, (bank, members) in enumerate(chunks):
        first = members[0]
        last = members[-1]
        lines = sum(p["end"] - p["start"] + 1 for p in members)
        entry = {
            "id": "bank%d_%04X" % (bank, first["addr"]),
            "bank": bank,
            "file": "c_src/zanac/bank%d/%04X.c" % (bank, first["addr"]),
            "addr_first": first["addr"],
            "addr_last": last["addr"],
            "line_first": first["start"],
            "line_last": last["end"],
            "asm_lines": lines,
            "procs": [{"name": p["name"], "addr": p["addr"],
                       "start": p["start"], "end": p["end"], "inner": p["inner"]}
                      for p in members],
        }
        out.append(entry)

    with open(os.path.join(ANALYSIS, "plan.json"), "w", encoding="utf-8", newline="\n") as fh:
        json.dump(out, fh, indent=1)

    md = ["# Porting plan", "",
          "| chunk | bank | file | procedures | asm lines | $first-$last |",
          "|---|---|---|---|---|---|"]
    for entry in out:
        md.append("| %s | %d | %s | %d | %d | $%04X-$%04X |"
                  % (entry["id"], entry["bank"], entry["file"], len(entry["procs"]),
                     entry["asm_lines"], entry["addr_first"], entry["addr_last"]))
    total_procs = sum(len(e["procs"]) for e in out)
    total_lines = sum(e["asm_lines"] for e in out)
    md.append("")
    md.append("Total: %d chunks, %d procedures, %d assembly lines."
              % (len(out), total_procs, total_lines))
    with open(os.path.join(ANALYSIS, "plan.md"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("\n".join(md) + "\n")

    print("%d chunks, %d procedures, %d asm lines" % (len(out), total_procs, total_lines))
    for entry in out:
        print("  %-14s %-40s %3d procs %5d lines  $%04X-$%04X"
              % (entry["id"], entry["file"], len(entry["procs"]), entry["asm_lines"],
                 entry["addr_first"], entry["addr_last"]))


if __name__ == "__main__":
    main()
