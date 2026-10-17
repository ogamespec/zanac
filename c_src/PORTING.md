# Zanac C port — translation rules

This document is the contract for every file under `c_src/zanac/`. It describes
how one procedure of the original 6502 assembly becomes one C function, keeping
the original statement order, the original RAM addresses and the original
control flow, so that a reviewer can compare the two line by line.

Source of truth: `src/bank6.asm` and `src/bank7.asm` (plus the level/graphics
data in `Build/bank4.bin`..`bank7.bin`). Addresses come from
`Build/bank6.lst` / `Build/bank7.lst` (DASM listings). `c_src/analysis/
procedures.json` holds, for every procedure, its bank, its 6502 address and the
line range in the `.asm` file.

---

## 1. File and procedure layout

Ported code lives in `c_src/zanac/bank6/<chunk>.c` and
`c_src/zanac/bank7/<chunk>.c`, one chunk per assigned range of procedures.

Every file starts with a header naming the bank and the address range, then:

```c
/* zanac_ram.h / vars.h / labels.h / nesregs.h via zanac.h */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $CB40  sub_CB40 - clear array $172...$179
 *
 *  Original: src/bank7.asm:2069-2077 (bank 7)
 *      LDA #0
 *      LDY #8
 *  loc_CB44:
 *      STA $179,Y
 *      DEY
 *      BNE loc_CB44
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_CB40(void)                          /* $CB40 */
{
    LDAI(0x00);                              /* LDA #0      */
    LDYI(8);                                 /* LDY #8      */

loc_CB44:                                    /* $CB44       */
    STA(ABSY(0x0179));                       /* STA $179,Y  */
    DEY();                                   /* DEY         */
    BNE(loc_CB44);                           /* BNE loc_CB44 */
    RTS();                                   /* RTS         */
}
```

Rules:

* One C function per **procedure** (a top-level label listed in
  `c_src/analysis/procedures.json`, `thunk` entries excepted — those are
  generated).
* The function name is the original label, unchanged.
* The comment block gives the 6502 address of the procedure and the exact
  `.asm` line range. Keep the original statement order.
* **Every** statement carries a trailing comment with the original instruction
  and, when it touches memory, the absolute 6502 address
  (`/* STA $179,Y */`, `/* LDA $91 */`).
* Labels inside a procedure (`loc_XXXX`, `locret_XXXX`) become C labels, with a
  comment giving their 6502 address. Inner labels are never functions.
* The body ends with `RTS();` (`return`), or with the tail call that the
  original `JMP` performed.
* Never invent behaviour. If something is unclear, port it literally and leave a
  `/* TODO: unclear - literal translation */` note. Do not "improve" the code.

---

## 2. Registers, flags and memory

`zanac_ram.h` provides the whole model. A variable name from `vars.h` is **an
address**, not a value:

```c
#define byte_10   RAM[0x0010]   -- NO: it is  #define byte_10 0x0010
```

so it is used exactly like the assembly operand:

| assembly | C |
|---|---|
| `LDA #$30` | `LDAI(0x30)` |
| `LDA byte_10` | `LDA(byte_10)` |
| `LDA $10,X` | `LDA(ZPX(0x10))` |
| `LDA $376,X` | `LDA(ABSX(0x0376))` |
| `LDA $376,Y` | `LDA(ABSY(0x0376))` |
| `LDA ($16),Y` | `LDA(INDY(0x0016))` |
| `LDA ($16,X)` | `LDA(INDX(0x0016))` |
| `STA byte_10` | `STA(byte_10)` |
| `STA $2007` | `STA(PPU_DATA)` |
| `STA $460,X` | `STA(ABSX(0x0460))` |
| `STA (temp_0,X)` | `STA(INDX(temp_0))` |
| `LDX #$FF` / `LDY #0` | `LDXI(0xFF)` / `LDYI(0)` |
| `STX $10` / `STY $11` | `STX(0x0010)` / `STY(0x0011)` |
| `ADC #$20` / `ADC byte_5` | `ADCI(0x20)` / `ADC(byte_5)` |
| `SBC`, `AND`, `ORA`, `EOR`, `CMP`, `CPX`, `CPY` | same pattern with `I` for immediates |
| `BIT byte_90` | `BIT(byte_90)` |
| `INC byte_7` / `DEC byte_7` | `INC(byte_7)` / `DEC(byte_7)` |
| `INC $376,X` | `INC(ABSX(0x0376))` |
| `ASL` / `LSR` / `ROL` / `ROR` (accumulator) | `ASL_A()` / `LSR_A()` / `ROL_A()` / `ROR_A()` |
| `ASL byte_7` (memory) | `ASL(byte_7)` / `LSR(byte_7)` / `ROL(byte_7)` / `ROR(byte_7)` |
| `INX` `INY` `DEX` `DEY` | `INX()` `INY()` `DEX()` `DEY()` |
| `TAX` `TAY` `TXA` `TYA` `TSX` `TXS` | `TAX()` `TAY()` `TXA()` `TYA()` `TSX()` `TXS()` |
| `PHA` `PLA` `PHP` `PLP` | `PHA()` `PLA()` `PHP()` `PLP()` |
| `CLC` `SEC` `CLD` `SED` `CLI` `SEI` `CLV` | `CLC()` `SEC()` `CLD()` `SED()` `CLI()` `SEI()` `CLV()` |
| `BNE lbl` `BEQ lbl` `BCC lbl` `BCS lbl` `BMI lbl` `BPL lbl` `BVC lbl` `BVS lbl` | `BNE(lbl)` … `BVS(lbl)` |
| `JSR foo` | `foo();` |
| `JMP foo` (another procedure, tail call) | `foo(); RTS();` |
| `JMP loc_XXXX` (same procedure) | `goto loc_XXXX;` |
| `RTS` | `RTS();` |

Branches to a label in **another** procedure only happen through the chopped-
procedure idiom (see §5); do not turn them into `goto`.

`MEM8(addr)` is a direct read of any address, `MEMW(addr, v)` a direct write;
`RD(addr)` / `WR(addr, v)` are the same thing for hand-written glue code. The
ported procedures normally use the instruction macros instead.

`A`, `X`, `Y`, `SP` and the flags `F_C`, `F_Z`, `F_N`, `F_V`, `F_I`, `F_D` are
globals: a callee leaves its result in `A` for the caller exactly like the 6502.
Never shadow them with local variables of the same name.

---

## 3. Banked reads

The `$8000-$BFFF` window reads the *currently selected bank*, `$C000-$FFFF`
always reads bank 7. `MEM8()` does this automatically from the extracted bank
arrays (`rom/zanac_banks.c`) — the mapper is not emulated. `switch_bank` and
`switch_bank_5/6` are ported like any other procedure; they end up calling
`zanac_switch_bank()`, whose only effect is changing the index used by
`zanac_bank_read()`. **Nothing else in the port needs to know about banks.**

---

## 4. Inline parameter blocks (`print_string` family)

`print_string`, `sub_C6CA`, `sub_C6EA` and `sub_C725` are reached by a `JSR`
followed by a data block; the routine pops the JSR return address and reads the
block there.

* The callee is ported literally, **except** that the two `PLA` instructions
  that pop the return address become

  ```c
  byte_16 = ZANAC_LO(zanac_inline_block);   /* PLA -> low byte of the block */
  byte_17 = ZANAC_HI(zanac_inline_block);   /* PLA -> high byte of the block */
  ```

* `JMP stack_call_return` (which pushed `$16/$17` back and executed `RTS`) is
  ported as a plain `RTS();` — the C caller simply continues with its next
  statement, which is where the original resumed.
* The **call site** passes the block address, taken from the `.lst` (it is the
  address of the first parameter byte, i.e. the JSR address + 3):

  ```c
  /* $C7DC: JSR print_string ; inline block at $C7E3 */
  /*   .byte 9, <vram low>, <vram high>, "ZANAC", 0 */
  PRINT_STRING(0xC7E3);
  ```

  and the inline bytes themselves are left as a comment (they are still present
  in the extracted bank image, which is what the callee reads).

## 4b. `switch_case` (computed jump through an inline table)

`JSR switch_case` / `JSR j_switch_case` is followed by a table of 16-bit
addresses (`entry - 1`). The ported `switch_case` body keeps its logic, but the
two `PLA`s take the table address from `zanac_inline_table`, and the final
`PHA/PHA/RTS` becomes `zanac_rts_dispatch()` (which pops the address from the
emulated stack and calls the ported procedure there through `zanac_call_at()`).

The call site passes the table address (the byte after the JSR, from the
`.lst`):

```c
/* $D50A: JSR switch_case ; table of 5 words at $D50D */
SWITCH_CASE(0xD50D);
```

**The `-1` is what makes this work, and it is easy to get wrong.** The 6502 JSR
pushes the address of its own *last* byte, so the two `PLA`s in `switch_case`
recover `table - 1`, not `table`. The following `INY / INY` then lands on the
entry's high byte and the `DEY` on its low byte, which is how the original reads
a little-endian word from a base that is one byte too low. `switch_case`
therefore does `uint16_t base = zanac_inline_table - 1;` before storing
`temp_0/temp_1`. Without it every `switch_case` in the port dispatches to
`(lo of the *next* entry) << 8 | hi of this entry) + 1` - a bogus address in the
middle of an instruction (this was the cause of the "no ported procedure at
$A0D9" class of failures, and of the level command interpreter never running).

## 4c. Other computed control flow

Any other construct that computes a target address (an `RTS` used as a jump, a
`JMP` to an address read from a table, self-modifying stores into a jump slot)
must be ported using `zanac_call_at(addr)` and documented in a comment. Report
such a construct in the file header so it can be reviewed.

---

## 5. Chopped procedures, shared tails and fall-through

The original frequently ends a procedure without `RTS`/`JMP`, letting execution
continue into the next procedure, and several entry points share one `RTS`.

* **Fall-through into the next procedure**: port the tail as a call plus a
  return, which reproduces the state (A, X, Y, flags) exactly:

  ```c
  void ppu_disable_all(void)          /* $CACB */
  {
      ppu_nmi_disable();              /* JSR ppu_nmi_disable */
      LDA(PPU_STATUS);                /* LDA PPU_STATUS */
      LDA(last_ppu_ctrl2);            /* LDA last_ppu_ctrl2 */
      ANDI(0xE7);                     /* AND #$E7 */
      ppu_set_control2();             /* falls through into $CAE2 */
      RTS();
  }
  ```

* **Shared tail** (several entry points `JMP` to one `RTS` at the end of
  another procedure): call the procedure that owns the tail, or — when the tail
  is a label inside a procedure — reproduce the tail faithfully in both places
  with a comment pointing at the shared label.

* **A label that is entered from outside** (an `XREF` that is a `↓J`, i.e. a
  jump, not a call) is a second entry point. Implement it as its own function if
  the surrounding code allows it; otherwise add a comment.

---

## 6. Writes that are not plain RAM

* Writes to `$2000-$4017` are NES registers: `STA(PPU_CTRL_REG1)` etc. →
  `MEMW(P, A)` → `neslib_write()`. Never bypass this with a direct write.
* Writes to `$8000-$FFFF` are bank switches (see §3).
* `STA ($xx),Y` and `STA ($xx,X)` are indirect stores: use
  `MEMW(INDY(0x00xx), A)` / `MEMW(INDX(0x00xx), A)`.

---

## 7. Hand-written glue (not part of the ported assembly)

`c_src/zanac/zanac_ram.c`, `savestate.c/.h`, `thunks.c`, `stubs.c`,
`zanac_dispatch.c` and `main.c` are hand-written or generated support code. They are the only places
allowed to use plain C idioms; they must still document the 6502 address of
anything they replace.

The Reset and VBlank handlers are ordinary ported procedures (`reset` at
`$E7BF`, `nmi_handler` at `$EBxx`) and are installed as the engine callbacks by
`main.c`:

```c
NeslibCallbacks cb = { reset, nmi_handler };
```

---

## 8. Save-state glue

Save states need a few hand-written statements inside otherwise literal ported
procedures. They are deliberate and must be kept when re-porting a chunk:

* `zanac_state_checkpoint()` at the top of the level loop (`sub_E81E`,
  `loc_E880`), the pause loop (`sub_D3A8`, `loc_D3B9`) and both menu loops
  (`show_main_menu`, `loc_E080` and `loc_E190`). It is a no-op unless the host has a pending
  save; the host may `longjmp` out of it (that is how `--save-state` stops after
  writing a canonical file).
* The `zanac_resume_*` guards at `reset`, `main_menu_loop`, `sub_E81E`,
  `sub_EA0B`, `sub_D3A8` and `show_main_menu`. They skip a prologue that has
  already run in the restored state and `goto` the matching loop label. In a
  normal run every flag is 0, so the instructions execute exactly as written in
  the assembly.
* `zanac_resume_kind = ...` before each loop, which records where a snapshot
  belongs (`menu`, `menu-select`, `level`, `level-setup`, `pause`).

The file format, the complete field lists and the resume chains are documented
in `zanac/savestate.c`. Changing a field list requires a format-version bump.

---

## 9. Build and check

```
python3 c_src/tools/gen_zanac.py      # regenerate vars.h/labels.h/bank images
python3 c_src/tools/gen_support.py    # regenerate thunks.c/stubs.c/dispatch
make -C c_src                         # build (see c_src/Makefile)
```

`gen_support.py` prints how many procedures are still stubbed; a procedure is
considered ported as soon as a `void <name>(void)` definition exists anywhere
under `c_src/zanac`. Compile with warnings on:

```
make -C c_src            # -Wall -Wextra
```

The port must always compile; if a procedure is not finished, leave the stub
in place rather than writing a partial definition that does not build.
