# Zanac C port

A procedure-by-procedure translation of the Zanac (USA) NES game from 6502
assembly to portable C, running on **neslib** — the NES engine built from the
Nespeculator emulator core (`c_src/neslib`, see its README).

Everything in this directory is the port. The original sources stay where they
were (`../src/bank4..bank7.asm`) and are the single source of truth: every
ported procedure keeps its original name, its original statement order, its
original RAM addresses, and carries a comment with its 6502 address so the two
can be compared line by line.

```
c_src/
  PORTING.md            the translation contract (read this first)
  Makefile              build (gcc/clang, Linux/WSL)
  CMakeLists.txt        build (MSVC/CMake, Windows, with the SDL3 frontend)
  neslib/               the NES engine: PPU, APU, cartridge, controller
    neslib.h/.c         the register-level API the game uses
    core/               Nespeculator's emulator core (PPU/APU/cart/input/CPU)
  zanac/                the ported game
    zanac_ram.h/.c      the 6502 programmer's model (registers, flags, RAM)
    zanac.h             master header (macros for the tricky call conventions)
    nesregs.h           the NES register addresses (mirrors src/nesregs.asm)
    vars.h              every RAM variable with its 6502 address  (generated)
    labels.h            one prototype per procedure + its address (generated)
    rom_labels.h        every data label of the ROM bank        (generated)
    labels_addr.h       every label of the ROM with its address (generated)
    thunks.c            the original's 3-byte JMP trampolines    (generated)
    stubs.c             placeholders for the not-yet-ported procs (generated)
    zanac_dispatch.c    address -> procedure dispatch table      (generated)
    rom/zanac_banks.c   the assembled PRG banks as C arrays       (generated)
    bank6/*.c           bank 6 ($8000-$BFFF): enemy & game logic
    bank7/*.c           bank 7 ($C000-$FFFF): engine, PPU/APU, menus
    main.c              entry point, installs Reset/NMI as engine callbacks
  frontend/             host side: SDL3 window, headless runner, PPM/WAV dumps
  reference/run_rom.c   the ORIGINAL ROM running in the same engine (verifier)
  tools/                generators and the frame comparison script
  analysis/             structural reports of the original banks, the plan
```

## The shape of the port

The port keeps the machine model instead of "rewriting" the game, because that
is what makes a 1:1 translation possible and reviewable:

* `RAM[<address>]` is the 2 KB work RAM; every name in `vars.h` expands to its
  6502 **address** (`LDA(byte_90)` is `LDA $90`, `STA(ABSX(byte_542))` is
  `STA $542,X`).
* `A`, `X`, `Y`, `SP` and the status flags are globals, so a callee returns its
  result to the caller exactly like the 6502 did.
* Every instruction family has a macro named after the mnemonic:
  `LDAI(0x30)` is `LDA #$30`, `ADCI(1)` is `ADC #1`, `BNE(loc_CB44)` is
  `BNE loc_CB44`, `JSR foo` is `foo()`, `RTS` is `return`.
* NES registers are reached only through neslib: `STA(PPU_CTRL_REG1)` ends up
  in `neslib_write(0x2000, A)`, `LDA(PPU_STATUS)` in `neslib_read(0x2002)`.
* The UxROM bank register is not emulated: writing `$8000-$FFFF` selects which
  extracted bank array the `$8000-$BFFF` window reads from, and `MEM8()` does
  the banking transparently.
* The two handlers the console calls are ordinary ported procedures installed
  as engine callbacks: `reset` (`$E7BF`) and `nmi_handler` (`$CE8D`).

`PORTING.md` documents the conventions, including the three places where the
original's tricks need an explicit equivalent: the inline parameter blocks of
`print_string` (parameters stored in the instruction stream), the computed
`RTS` jumps (`switch_case`, the enemy dispatcher) and the `PLA` stack unwinds.

## Building and running

```
cd c_src
make                      # build/zanac (headless) and build/zanac_rom
make run                  # 600 frames, headless
./build/zanac --shot out.ppm --frames 900
make FRONTEND=sdl SDL3_CFLAGS=-I<SDL3>/include SDL3_LIBS=-lSDL3
```

On Windows use CMake (it finds the SDL3 copy vendored in the Nespeculator
checkout and produces a windowed build):

```
cmake -S c_src -B c_src/build-cmake
cmake --build c_src/build-cmake --config Release
```

Screenshots of a run (and of the original ROM in the same engine) are in
`shots/`: `07_port_vs_rom_start.png` is the port's frame 140 next to the ROM's
frame 160, `04_port_vs_rom_level1.png` three later pairs.

## Audio pacing

The ported game does not only ask for frames: the loops that wait for state the
NMI updates (`neslib_pump()`, the port of "let the NMI run while the 6502
spins") also advance whole *emulated* frames. Measured on the boot screens, the
host saw 100 frames while 215 emulated frames went by. Since the APU produces one
frame of samples per emulated frame, pushing all of them into the SDL stream
made the sound card fall seconds behind - the lag was permanent, because
playback runs at 1x while the emulation was going at 2x.

`neslib_set_vblank_hook()` therefore calls the host once per **emulated** frame
(the vblank rise, whatever caused it), and the SDL frontend paces there, so the
emulation cannot run ahead of real time. On top of that the audio cushion is
kept bounded: above 150 ms of queued samples the excess is dropped down to
60 ms. Measured with `ZANAC_AUDIO_LOG=1`:

```
t=  8.27s emulated frames=347 queued= 118.8 ms
t= 17.25s emulated frames=887 queued= 124.1 ms     # 60.1 emulated frames/s, queue stable
```

## Verification

`zanac_rom` runs the **original 6502 ROM** in the very same engine (same PPU,
same APU, same frame timing, PRG image taken from the extracted banks), so the
port can be compared against the original frame by frame and, when something
looks wrong, byte by byte in the PPU and in RAM:

```
make compare FRAMES=900 SCRIPT=tests/play.txt   # port vs ROM, per-frame hashes
make verify                                     # against tests/rom_play_900.hashes
make check-bg                                   # PPU memory (CHR/nametables) at matched frames
make auto                                       # long run with a stall watchdog (tests/autoplay.c)
./build/zanac     --frames 317 --input tests/play.txt --dump-ppu /tmp/p --dump-ram /tmp/p.ram
./build/zanac_rom --frames 280 --input tests/play.txt --dump-ppu /tmp/r --dump-ram /tmp/r.ram
python3 tools/compare_ram.py /tmp/p.ram /tmp/r.ram   # RAM, byte by byte, by variable name
cmp /tmp/p.chr /tmp/r.chr      # pattern tables
cmp /tmp/p.nt  /tmp/r.nt       # name tables + attributes
cmp /tmp/p.pal /tmp/r.pal      # palette
```

**Compare frames whose screens are identical.** The port reaches a given game
state a few frames away from the ROM (its boot is shorter), so dumping the same
frame number on both sides compares two different states and every byte looks
wrong. `compare_hashes.py` prints the alignment; `compare_ram.py` then reports
the variables that really differ and separates the ones that only depend on
where in the frame the dump was taken.

**Feed both runs the same input at the same point in the game.** The script is
keyed by frame number, so the ~21-frame head start of the port means a press
lands on a different point of the level in the two runs, and the player's
movement then differs for the rest of the comparison.  `--input-offset N`
shifts a script for one of the runs:

```
./build/zanac     --frames 700 --input tests/play.txt                 --hashes /tmp/p.hashes
./build/zanac_rom --frames 700 --input tests/play.txt --input-offset 21 --hashes /tmp/r.hashes
```

The cleanest comparison of all presses START once and then leaves the pad alone
(`--level N` on both sides forces the level, so any level can be checked this
way):

```
printf '100 START\n122 -\n' > /tmp/go1.txt
./build/zanac     --frames 1800 --level 1 --input /tmp/go1.txt --hashes /tmp/p.hashes
./build/zanac_rom --frames 1818 --level 1 --input /tmp/go1.txt --hashes /tmp/r.hashes
python3 tools/compare_hashes.py /tmp/r.hashes /tmp/p.hashes
```

| level | frames identical | identical from |
|---|---|---|
| 1 | 1775/1800 = 98.6% | frame 30 to 1799 (1770 frames) |
| 3, 5, 7, 9, 11 | 875/900 = 97.2% | frame 30 to the end |

In other words: after the boot, the port's framebuffer is **identical to the
original ROM frame for frame**, with the same input, on every level tested - and
at the aligned frames the whole rendered state matches too (name table 0
differing bytes, OAM 0 differing bytes at frames 250, 400, 600, 900 and 1200 of
level 1, i.e. enemies and their shots included).

The frames that differ are 0-29, the boot: the port calls the game's Reset
handler directly instead of executing the 6502 reset sequence and its PPU
warm-up loop, so its first screens come up a few frames earlier.  That is the
deliberate deviation documented under "Power-on / reset" in neslib_power_on(),
and it is the only one left.

Two harness details matter for reproducing this.  Feed the buttons *before* the
frame runs (the port's frame hook now uses `s_frames + 1`; using `s_frames` made
it react to every press one frame late, which shifted the whole level), and give
the scripted movement runs a phase-aligned script (`--input-offset`) - a script
keyed by absolute frame numbers hits a different point of the level in the two
runs, because the boot lengths differ slightly.

With the input matched this way (or with a script that only starts the game and
then stands still, `/tmp/still.txt` above) the port and the ROM agree on **98
consecutive screens** and 54% of all frames, and the differences that remain are
isolated single frames rather than a drift.

`tools/compare_hashes.py` reports both the frame-level match and the *screen
sequence* (how each run's framebuffers collapse into a list of distinct
screens). The screen sequence is the meaningful test: the port does not execute
6502 instructions, so its boot is about 20 frames shorter than the ROM's, and
after that both produce one frame per vsync.

Measured with the current code (`tests/play.txt`: a START press at frame 120,
another at 200, then movement):

| what | result |
|---|---|
| boot screens | same screens, in the same order, each shown for the same number of frames except the shortened boot |
| screen sequence | **74 consecutive screens** frame-identical with the standard script, **98** with the input matched (`--input-offset`); 60.5% of all frames |
| port frame 317 vs ROM frame 280 (identical screens) | RAM state **identical**; the only named difference is `vblank_wait_flag`, which depends on the dump point |
| that frame's PPU memory | pattern tables (CHR) **byte-identical**, name tables within **7 bytes** |
| first divergence | screen 36: same state, but the fine scroll is one pixel out of phase |
| stalls | none: `make auto` runs 12000-60000 frames on **all twelve levels**, at normal and 4x scroll speed, plus 4000 frames on the game-over path |
| enemy and shot sprites | **byte-identical OAM** at the aligned frames up to ~frame 330 (e.g. port 320 == ROM 281/285 exactly), then a few bytes drift (2 bytes at port 350, 10 at port 500) |
| first real divergence | port frame 323: two sprites' Y differs by 3 pixels, everything else matches |

The game's random number generator mixes the 6502 **stack pointer** into every
byte it returns (`enemy_prng`, $872B: `TSX / TXA / ADC last_randomize_result`),
and a ported C procedure has no 6502 stack to read.  The port therefore passes
the value in: every call site uses `ENEMY_PRNG_SP(sp)` with the stack pointer
measured on the real ROM at that JSR by `make stack-scan`
(`tests/stack_scan.c`) - each spawner is called from a fixed depth, so one
measurement per site is enough.  With that in place the sprite memory matches the
ROM byte for byte over long stretches of level 1.

What is left are two small differences, both of them sprite positions: from
around port frame 350 the Y of one or two sprites is a few pixels out (8 OAM
bytes at frame 350, 12 at 450), and the enemy states then drift apart over the
rest of the level.  The background is exact: with the attribute row fixed the
name tables match byte for byte, the pattern tables are byte-identical, and the
RAM comparison reports identical state at the aligned frames.

That last fix is worth recording because it was not a porting mistake in the
game code at all.  The engine used to clear the 2 KB of work RAM at power-on
("the Reset handler initialises every variable it uses"), but the game reads some
of it *before* writing it: the background attribute pass at $DB58 builds its
value from $16E, which the level script only fills later, so the first passes run
against whatever the RAM happened to hold.  On hardware - and in the reference
ROM runner, which never had that memset - $16E holds the deterministic power-on
pattern ($39 $DE $D7 $49 ...), and the accumulated attribute row comes out the way
the original draws it.  With the RAM left alone the port's attribute buffer
matches the ROM's exactly (0 differing bytes where it used to be 7) and the
"holes at the bottom" of the map are gone.

Two further differences are deliberate and documented in the code: `apu_fx_5`
(a sound script that unwinds the 6502 stack with four `PLA`s; the port returns
normally instead) and `sub_EFA6` (whose `RTS` would land one byte past the
pointer it is given - a path the ROM never takes).

