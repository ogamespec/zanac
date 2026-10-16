# Makefile - build the Zanac C port (neslib engine + ported game code).
#
#   make            build build/zanac  (headless frontend)
#   make run        build and run 600 frames
#   make shot       write build/frame.ppm
#   make clean
#
# Every generated file is refreshed first: vars.h/labels.h/rom_labels.h and the
# bank images (gen_zanac.py), the thunks, the stubs and the address dispatcher
# (gen_support.py).

CC      ?= gcc
PYTHON  ?= python3
BUILD   := build
BIN     := $(BUILD)/zanac
ROMBIN  := $(BUILD)/zanac_rom

CFLAGS  ?= -std=c11 -O2 -g -Wall -Wextra \
           -Wno-unused-parameter -Wno-unused-const-variable \
           -Wno-unused-function -Wno-comment -Wno-infinite-recursion
CPPFLAGS += -I. -Izanac -Ineslib -Ifrontend $(SDL3_CFLAGS)

NESLIB_SRC := $(wildcard neslib/*.c) $(wildcard neslib/core/*.c)
ZANAC_SRC  := $(wildcard zanac/*.c) $(wildcard zanac/bank6/*.c) $(wildcard zanac/bank7/*.c) \
              $(wildcard zanac/rom/*.c)
# FRONTEND=headless (default, no window) or FRONTEND=sdl (needs SDL3:
# make FRONTEND=sdl SDL3_CFLAGS=... SDL3_LIBS=...).
FRONTEND   ?= headless
FRONT_SRC  := frontend/image.c frontend/input_script.c frontend/$(FRONTEND).c

SRC := $(NESLIB_SRC) $(ZANAC_SRC) $(FRONT_SRC)
OBJ := $(patsubst %.c,$(BUILD)/obj/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

.PHONY: all run shot clean generated verify compare rom

all: $(BIN) $(ROMBIN)

generated:
	@$(PYTHON) tools/gen_zanac.py
	@$(PYTHON) tools/gen_support.py

$(BUILD)/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

# The generated sources are produced once per build; the object files depend on
# the headers through the .d files.
$(OBJ): | generated

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(SDL3_LIBS) -lm

# Reference runner: executes the original 6502 ROM in the same engine.
REF_OBJ := $(BUILD)/obj/reference/run_rom.o $(BUILD)/obj/frontend/image.o \
           $(BUILD)/obj/frontend/input_script.o \
           $(BUILD)/obj/zanac/rom/zanac_banks.o \
           $(patsubst %.c,$(BUILD)/obj/%.o,$(NESLIB_SRC))
$(ROMBIN): $(REF_OBJ)
	$(CC) $(CFLAGS) -o $@ $(REF_OBJ) -lm

rom: $(ROMBIN)

run: $(BIN)
	$(BIN) --frames 600

shot: $(BIN)
	$(BIN) --frames 900 --shot $(BUILD)/frame.ppm
	@echo "wrote $(BUILD)/frame.ppm"

# Regression tests.
#
#   make verify   run the C port with the scripted input of tests/ and compare
#                 it against the reference hashes of the original ROM that are
#                 stored in tests/ (tests/rom_play_900.hashes)
#   make compare  re-run BOTH the original ROM and the port, then compare them
#                 (slower, but it regenerates the reference)
#
# The hashes are FNV-1a of the whole 256x240 framebuffer, one line per frame.
FRAMES ?= 900
SCRIPT ?= tests/play.txt
REFHASH ?= tests/rom_play_900.hashes

verify: $(BIN)
	$(BIN) --frames $(FRAMES) --input $(SCRIPT) --hashes $(BUILD)/port.hashes
	$(PYTHON) tools/compare_hashes.py $(REFHASH) $(BUILD)/port.hashes

# The reproducible acceptance test: at matched frames the port's PPU memory must
# equal the ROM's (CHR byte for byte, name tables within the invisible attribute
# bytes).  See Readme.md - the sprites cannot match, the background must.
BGPORT ?= 140 200 260 320 400
BGRAM  ?= 120 130 140 150 160 180 200 220 240 260 280 300 320 340 360 380 400 420

check-bg: $(BIN) $(ROMBIN)
	@for f in $(BGPORT); do $(BIN) --frames $$f --input $(SCRIPT) --dump-ppu $(BUILD)/pbg_$$f >/dev/null; done
	@for f in $(BGRAM); do $(ROMBIN) --frames $$f --input $(SCRIPT) --dump-ppu $(BUILD)/rbg_$$f >/dev/null; done
	@rc=0; for f in $(BGPORT); do \
	    $(PYTHON) tools/compare_ppu.py $(BUILD)/pbg_$$f $(shell for g in $(BGRAM); do echo $(BUILD)/rbg_$$g; done) || rc=1; \
	done; exit $$rc

compare: $(BIN) $(ROMBIN)
	$(ROMBIN) --frames $(FRAMES) --input $(SCRIPT) --hashes $(BUILD)/rom.hashes
	$(BIN) --frames $(FRAMES) --input $(SCRIPT) --hashes $(BUILD)/port.hashes
	$(PYTHON) tools/compare_hashes.py $(BUILD)/rom.hashes $(BUILD)/port.hashes

clean:
	rm -rf $(BUILD)

-include $(DEP)
