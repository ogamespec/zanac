/* cart.h - cartridge (Game Pak) interface.
 *
 * Reference material:
 *   Specs/05-boards-and-io.md      - cartridge interface, CIRAM A10 /CE, CIC
 *   Specs/04-ppu-memory-map.md     - pattern tables, nametable mirroring
 *
 * SPEC GAP: the specification set lists iNES mapper numbers but explicitly says
 * that mapper register semantics are out of scope ("this repository contains no
 * mapper documentation"), and it does not describe the iNES header at all.
 * See SPEC-GAPS.md items CART-1..CART-4.  The layouts implemented here follow
 * the community-standard register interfaces named by the spec's mapper table.
 */
#ifndef CART_H
#define CART_H

#include <stdint.h>
#include <stdbool.h>

struct Nes;
struct Ppu;

typedef enum {
    MIRROR_HORIZONTAL = 0,
    MIRROR_VERTICAL   = 1,
    MIRROR_FOUR       = 2,
    MIRROR_SINGLE_LO  = 3,
    MIRROR_SINGLE_HI  = 4
} Mirroring;

typedef struct Cart {
    struct Nes *nes;

    /* ROM images */
    uint8_t *prg;         /* PRG ROM, prg_size bytes */
    uint32_t prg_size;
    uint8_t *chr;         /* CHR ROM or CHR RAM, chr_size bytes */
    uint32_t chr_size;
    bool     chr_ram;     /* true when the board supplies writable CHR */

    uint8_t  prg_ram[8192];

    /* Header-derived configuration */
    uint8_t  mapper;
    uint8_t  submapper;
    uint8_t  prg_banks_16k;
    uint8_t  chr_banks_8k;
    bool     battery;
    bool     four_screen;
    bool     nes2;
    Mirroring mirroring;

    /* Mapper state */
    uint32_t prg_bank_lo, prg_bank_hi;
    uint32_t chr_bank[8];
    uint8_t  reg[16];
    uint8_t  shift;        /* MMC1 serial shift register */
    uint8_t  shift_count;
    uint8_t  irq_latch;
    uint8_t  irq_counter;
    uint8_t  irq_reload;
    bool     irq_enable;
    bool     irq_pending;
    /* MMC3 scanline counter: A12 edge tracking (see cart_ppu_a12). */
    bool     a12_high;
    uint8_t  a12_low_dots;
    uint8_t  a12_sprite_edges;
    uint8_t  mmc3_bank_select;
    bool     mmc3_prg_mode;
    bool     mmc3_chr_mode;

    bool     bus_conflict; /* discrete-logic boards: AND with the ROM byte */
} Cart;

enum {
    CART_OK = 0,
    CART_ERR_OPEN,
    CART_ERR_FORMAT,
    CART_ERR_UNSUPPORTED
};

int  cart_load(Cart *cart, struct Nes *nes, const char *path);
void cart_free(Cart *cart);

/* neslib addition: build a board from an in-memory description instead of an
 * iNES file.  No PRG image is installed (the ported game code reads the
 * extracted banks itself), only the CHR-RAM the pattern tables live in and the
 * mirroring the PPU needs. */
void cart_init_chr_ram(Cart *cart, struct Nes *nes, uint8_t mapper,
                       uint32_t chr_ram_size, bool vertical_mirroring);

/* The same board with a real PRG image, for the reference runner that executes
 * the original 6502 ROM (c_src/reference/run_rom.c). */
void cart_init_image(Cart *cart, struct Nes *nes, const uint8_t *prg,
                     uint32_t prg_size, uint8_t mapper, uint32_t chr_ram_size,
                     bool vertical_mirroring);

/* CPU bus $4020-$FFFF */
uint8_t cart_cpu_read(Cart *cart, uint16_t addr);
void    cart_cpu_write(Cart *cart, uint16_t addr, uint8_t value);

/* PPU bus $0000-$1FFF (pattern tables). */
uint8_t cart_chr_read(Cart *cart, uint16_t addr);
void    cart_chr_write(Cart *cart, uint16_t addr, uint8_t value);

/* CIRAM A10 for a nametable address in $2000-$3EFF:
 * returns 0 or 1 selecting the 1 KB VRAM bank. */
int  cart_ciram_a10(Cart *cart, uint16_t addr);
/* CIRAM /CE: false when the cartridge disables the board VRAM. */
bool cart_ciram_enabled(Cart *cart);

/* Called once per PPU dot while rendering (A12 rising edge) so that a mapper
 * can implement a scanline counter. */
void cart_ppu_a12(Cart *cart, uint16_t addr, bool rendering);

/* IRQ line state, true = asserted. */
bool cart_irq(Cart *cart);

extern const char *cart_mapper_name(uint8_t mapper);

#endif /* CART_H */
