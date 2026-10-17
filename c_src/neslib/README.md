# neslib — the NES engine of the Zanac C port

`neslib` is the base NES engine the ported game runs on. It is **derived from
the Nespeculator emulator** (`C:\Work\Nespeculator`): the RP2C02 PPU, the 2A03
APU, the cartridge and the controller are kept as they were, the 6502 core is
left inert, and the game's translated C code supplies the register traffic
instead of the CPU.

```
neslib/
  neslib.h/.c     the API the ported game uses
  core/           Nespeculator's core, taken over as-is where possible
    nes.c/.h      system orchestration (clocks, reset, power-on)
    ppu.c/.h      RP2C02: registers, VRAM/OAM, per-dot rendering
    apu.c/.h      RP2A03 audio: channels, frame counter, DMC, audio capture
    cart.c/.h     cartridge: mappers, CHR memory, mirroring
    input.c/.h    standard controllers ($4016/$4017)
    bus.c/.h      CPU address space
    cpu.c/.h      the 6502 core (kept so the original ROM can also be run —
    cpu_ops.c     c_src/reference/run_rom.c does exactly that)
    state.c/.h    save states of the emulator core (kept with the core; the
                  port's own save states are in ../zanac/savestate.c, because
                  they also contain the ported 6502 registers and the game's
                  resume point)
```

## The API

The game never sees a CPU, a bus or a mapper. It asks neslib to access a NES
register exactly like the 6502 code it was ported from:

```c
neslib_write(0x2000, value);        /* STA $2000  (PPU_CTRL_REG1)  */
value = neslib_read(0x2002);        /* LDA $2002  (PPU_STATUS)     */
neslib_write(0x4014, 2);            /* STA $4014  (OAM DMA page 2) */
neslib_oam_dma(2);                  /* the same, as an explicit call */
neslib_joy_strobe(1);               /* STA $4016                   */
```

Everything else is the frame flow and the host side:

```c
typedef struct NeslibCallbacks {
    void (*reset)(void);            /* Reset handler: runs at power-on  */
    void (*vblank)(void);           /* NMI handler: runs at vblank      */
} NeslibCallbacks;

neslib_power_on(&board, MIRROR_HORIZONTAL, &callbacks);
neslib_wait_vblank();               /* the port of the game's `JSR vsync` */
const uint32_t *fb = neslib_framebuffer();   /* 256x240, 0x00RRGGBB */
neslib_audio_enable(48000.0);
neslib_audio_read(samples, n);
neslib_set_button(0, NESLIB_BTN_A, pressed);

neslib_set_frame_hook(host_frame);   /* once per frame the host asked for */
neslib_set_vblank_hook(host_pace);   /* once per *emulated* frame          */
```

The two hooks are not the same thing. The frame hook runs at the end of the
frames the host asked for (that is where a host presents the picture and drains
the audio). The vblank hook runs at every emulated vblank rise, including the
ones the ported game reaches by spinning itself (`neslib_pump()`, the port of
"let the NMI run while the 6502 spins"): the boot screens alone advance 215
emulated frames while the host sees 100. A real-time host must pace there, since
the APU produces one frame of samples per emulated frame - pacing only on the
frame hook lets the emulation run at twice real time and the sound card falls
seconds behind, permanently.

`neslib_wait_vblank()` advances the PPU and APU to the point where `$2002` bit 7
is set (line 241, dot 2), calls the VBlank callback when `$2000` bit 7 enables
the NMI, and then advances to the end of the frame. That is what makes the
ported `vsync` loop and the ported `nmi_handler` behave like the original: the
NMI's VRAM writes land inside vblank, before the pre-render `t -> v` copy.

## Changes made to the Nespeculator sources

The core is taken over unchanged except for four additions, each marked with a
`neslib` comment in the source:

| change | why |
|---|---|
| `ppu_flush_write_queues()` (ppu.c/ppu.h) | the `$2001`/`$2006` PCLK write-latency queues must be drained, because the ported code issues register writes back-to-back from C instead of four or more cycles apart |
| `cart_init_chr_ram()` (cart.c/cart.h) | build the board from an in-memory description: UxROM, no PRG image (the port reads the extracted banks itself), 8 KB CHR RAM, and the nametable `Mirroring` the caller passes in |
| `cart_init_image()` (cart.c/cart.h) | the same board *with* a PRG image, used by the reference runner |
| `apu_reset()` renamed to `neslib_apu_reset()` (apu.c/apu.h/nes.c) | the port has its own procedure called `apu_reset` (bank 7, `$EF71`), and C has one global namespace |

Two documented deviations from cycle-accurate emulation live in `neslib.c`:
register writes flush the PPU latency queues (above), and the `$4016`
controller strobe takes effect immediately instead of at the next ACLK2 edge,
because the ported `pad_read` writes the register and reads the shift register
with no emulated CPU cycles in between.

## Running the original ROM with it

`core/` still contains the 6502 core, `nes_run_frame()` still emulates the whole
machine, and `cart_init_image()` installs a real PRG image. That is how
`c_src/reference/run_rom.c` runs the original Zanac ROM in the very same engine
the port uses, which is what makes a frame-by-frame comparison between the port
and the original possible (`make compare` in `c_src`).
