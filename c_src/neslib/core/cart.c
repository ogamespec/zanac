/* cart.c - cartridge loading and mapper implementations.
 *
 * SPEC GAP CART-1..4 (see SPEC-GAPS.md): the specification set names iNES mapper
 * numbers and their capabilities but states explicitly that "this repository
 * contains no mapper documentation" and that "individual mapper register
 * semantics are out of scope and left to NESdev"; it also never describes the
 * iNES header.  The header layout and the register interfaces below therefore
 * follow the community-standard definitions of the mapper numbers the spec
 * names (0 NROM, 1 MMC1, 2 UxROM, 3 CNROM, 4 MMC3, 7 AxROM), which is the only
 * way to load a ROM at all.  Everything else - the board-level contract (CIRAM
 * A10 / CIRAM /CE, CHR-ROM vs CHR-RAM, bus conflicts) - follows the spec.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nes.h"
#include "cart.h"

const char *cart_mapper_name(uint8_t mapper)
{
    switch (mapper) {
        case 0:  return "NROM";
        case 1:  return "MMC1";
        case 2:  return "UxROM";
        case 3:  return "CNROM";
        case 4:  return "MMC3";
        case 5:  return "MMC5 (unsupported)";
        case 7:  return "AxROM";
        case 9:  return "MMC2 (unsupported)";
        case 10: return "MMC4 (unsupported)";
        case 24: return "VRC6 (unsupported)";
        case 26: return "VRC6 (unsupported)";
        case 69: return "FME-7 (unsupported)";
        case 85: return "VRC7 (unsupported)";
        default: return "unsupported";
    }
}

/* ---------------------------------------------------------------- loading -- */

static uint8_t *read_file(const char *path, uint32_t *size)
{
    FILE *f = fopen(path, "rb");
    uint8_t *buf;
    long len;
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    len = ftell(f);
    if (len <= 0) { fclose(f); return NULL; }
    rewind(f);
    buf = (uint8_t *)malloc((size_t)len);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)len, f) != (size_t)len) {
        free(buf);
        fclose(f);
        return NULL;
    }
    fclose(f);
    *size = (uint32_t)len;
    return buf;
}

int cart_load(Cart *cart, struct Nes *nes, const char *path)
{
    uint8_t *file;
    uint32_t size;
    uint8_t flags6, flags7, flags8, flags9;
    uint32_t offset;
    uint32_t prg_bytes, chr_bytes;

    memset(cart, 0, sizeof(*cart));
    cart->nes = nes;

    file = read_file(path, &size);
    if (!file) return CART_ERR_OPEN;
    if (size < 16 || memcmp(file, "NES\x1a", 4) != 0) {
        free(file);
        return CART_ERR_FORMAT;
    }

    flags6 = file[6];
    flags7 = file[7];
    flags8 = file[8];
    flags9 = file[9];
    cart->mapper = (uint8_t)((flags6 >> 4) | (flags7 & 0xF0));
    cart->nes2 = (flags7 & 0x0C) == 0x08;
    if (cart->nes2) {
        cart->mapper = (uint8_t)(cart->mapper | ((flags8 & 0x0F) << 8));
        cart->submapper = (uint8_t)((flags8 >> 4) & 0x0F);
    }

    cart->prg_banks_16k = file[4];
    cart->chr_banks_8k = file[5];
    if (cart->nes2) {
        /* NES 2.0 byte 9 carries the upper bits of both size fields:
         * PRG = (MSB << 8) | byte 4 (16 KB units), CHR = (MSB << 8) | byte 5
         * (8 KB units).  A zero nibble means the field is just byte 4/5, so a
         * zero CHR-ROM size means CHR-RAM.  The previous `| 0x10` turned the
         * all-zero CHR fields of a CHR-RAM image into 16 CHR-ROM banks
         * (128 KB), which made every NES 2.0 CHR-RAM ROM - including all of
         * the nes-audio-tests builds - fail to load. */
        uint32_t prg_banks = (uint32_t)file[4] | ((uint32_t)(flags9 & 0x0F) << 8);
        uint32_t chr_banks = (uint32_t)file[5] | ((uint32_t)(flags9 >> 4) << 8);
        if (prg_banks > 0xFFu || chr_banks > 0xFFu) {
            /* Larger than the loader's bank counters can describe. */
            free(file);
            return CART_ERR_FORMAT;
        }
        cart->prg_banks_16k = (uint8_t)prg_banks;
        cart->chr_banks_8k = (uint8_t)chr_banks;
    }

    cart->battery = (flags6 & 0x02) != 0;
    cart->four_screen = (flags6 & 0x08) != 0;

    if (flags6 & 0x04)          /* 512-byte trainer */
        offset = 16 + 512;
    else
        offset = 16;

    prg_bytes = (uint32_t)cart->prg_banks_16k * 16384u;
    chr_bytes = (uint32_t)cart->chr_banks_8k * 8192u;

    if (offset + prg_bytes > size) {
        free(file);
        return CART_ERR_FORMAT;
    }
    cart->prg = (uint8_t *)malloc(prg_bytes ? prg_bytes : 1);
    if (!cart->prg) { free(file); return CART_ERR_OPEN; }
    memcpy(cart->prg, file + offset, prg_bytes);
    cart->prg_size = prg_bytes;

    cart->chr_ram = (chr_bytes == 0);
    if (cart->chr_ram) {
        /* A CHR-RAM board supplies the pattern tables; 8 KB is the standard
         * size when the header declares none. */
        chr_bytes = 8192;
        cart->chr = (uint8_t *)calloc(1, chr_bytes);
    } else {
        if (offset + prg_bytes + chr_bytes > size) {
            free(cart->prg);
            free(file);
            return CART_ERR_FORMAT;
        }
        cart->chr = (uint8_t *)malloc(chr_bytes);
        if (!cart->chr) { free(cart->prg); free(file); return CART_ERR_OPEN; }
        memcpy(cart->chr, file + offset + prg_bytes, chr_bytes);
    }
    cart->chr_size = chr_bytes;

    free(file);

    if (cart->four_screen)
        cart->mirroring = MIRROR_FOUR;
    else
        cart->mirroring = (flags6 & 0x01) ? MIRROR_VERTICAL : MIRROR_HORIZONTAL;

    /* Mapper default register state. */
    switch (cart->mapper) {
        case 0:
            cart->prg_bank_lo = 0;
            cart->prg_bank_hi = (cart->prg_size > 16384) ? 1 : 0;
            break;
        case 1:
            cart->shift = 0x10;
            cart->shift_count = 0;
            cart->reg[0] = 0x0C; /* PRG mode 3 */
            cart->reg[1] = 0;
            cart->reg[2] = 0;
            cart->reg[3] = 0;
            {
                /* $8000 selects the low 16 KB, $C000 the last bank. */
                uint32_t banks = cart->prg_size / 16384;
                cart->prg_bank_lo = 0;
                cart->prg_bank_hi = banks ? banks - 1 : 0;
            }
            break;
        case 2:
            cart->prg_bank_lo = 0;
            cart->prg_bank_hi = (cart->prg_size / 16384) ? (cart->prg_size / 16384) - 1 : 0;
            break;
        case 3:
        case 7:
            cart->prg_bank_lo = 0;
            cart->prg_bank_hi = (cart->prg_size > 32768) ? 1 : 0;
            break;
        case 4:
            cart->prg_bank_lo = 0;
            cart->prg_bank_hi = (cart->prg_size / 8192) ? (cart->prg_size / 8192) - 1 : 0;
            {
                int i;
                for (i = 0; i < 8; i++) cart->chr_bank[i] = (uint32_t)i;
            }
            break;
        default:
            return CART_ERR_UNSUPPORTED;
    }

    /* Discrete-logic boards can suffer bus conflicts (05-boards-and-io.md
     * section 9): the written value is ANDed with the ROM byte. */
    cart->bus_conflict = (cart->mapper == 0 || cart->mapper == 2 || cart->mapper == 3 ||
                          cart->mapper == 7);
    return CART_OK;
}

void cart_free(Cart *cart)
{
    free(cart->prg);
    free(cart->chr);
    cart->prg = NULL;
    cart->chr = NULL;
}

/* neslib addition (see cart.h): a board built from an in-memory description.
 * The ported game code reads the extracted PRG banks directly, so no PRG image
 * is installed; the PPU still needs the pattern-table memory and the board
 * mirroring. */
void cart_init_chr_ram(Cart *cart, struct Nes *nes, uint8_t mapper,
                       uint32_t chr_ram_size, Mirroring mirroring)
{
    memset(cart, 0, sizeof(*cart));
    cart->nes = nes;
    cart->mapper = mapper;
    cart->mirroring = mirroring;
    /* No PRG image is installed, so bus conflicts cannot be resolved against
     * the ROM byte; the port does not write the mapper register anyway (it
     * switches banks itself, see zanac_ram.c). */
    cart->bus_conflict = false;
    cart->prg = (uint8_t *)calloc(1, 1);      /* nothing ever reads it */
    cart->prg_size = 0;
    cart->prg_banks_16k = 0;
    cart->prg_bank_lo = 0;
    cart->prg_bank_hi = 0;
    cart->chr_ram = true;
    cart->chr_size = chr_ram_size ? chr_ram_size : 8192u;
    cart->chr = (uint8_t *)calloc(1, cart->chr_size);
}

/* neslib addition: the same board, but with a real PRG image installed.  Used
 * by the reference runner (c_src/reference/run_rom.c), which executes the
 * original 6502 ROM in the engine so that the C port can be compared against
 * it frame by frame. */
void cart_init_image(Cart *cart, struct Nes *nes, const uint8_t *prg,
                     uint32_t prg_size, uint8_t mapper, uint32_t chr_ram_size,
                     Mirroring mirroring)
{
    cart_init_chr_ram(cart, nes, mapper, chr_ram_size, mirroring);
    free(cart->prg);
    cart->prg = (uint8_t *)malloc(prg_size ? prg_size : 1);
    memcpy(cart->prg, prg, prg_size);
    cart->prg_size = prg_size;
    cart->prg_banks_16k = (uint8_t)(prg_size / 16384);
    cart->prg_bank_lo = 0;
    cart->prg_bank_hi = cart->prg_banks_16k ? cart->prg_banks_16k - 1 : 0;
    /* The board really does have bus conflicts; the mapper register is written
     * through the identity table at $C0F8 (see switch_bank), so keep them. */
    cart->bus_conflict = true;
}

/* ------------------------------------------------------------- bank helpers -- */

static uint8_t prg_read(Cart *cart, uint16_t addr)
{
    uint32_t banks16 = cart->prg_size / 16384;
    uint32_t off = 0;
    if (banks16 == 0) return 0;

    switch (cart->mapper) {
    case 0:
        off = (uint32_t)(addr - 0x8000);
        if (cart->prg_size == 16384) off &= 0x3FFF;
        break;
    case 1: {
        /* MMC1 PRG windowing, selected by control-register bits 2-3:
         *   0, 1 = 32 KB at $8000-$FFFF, bank number bit 0 not wired
         *   2    = fixed first 16 KB at $8000, switchable 16 KB at $C000
         *   3    = switchable 16 KB at $8000, fixed last 16 KB at $C000
         * The 32 KB case used to ignore the bank register completely and always
         * expose the first 32 KB. */
        uint32_t mode = (cart->reg[0] >> 2) & 3;
        if (mode <= 1) {
            off = (cart->prg_bank_lo & ~1u) * 16384u + (uint32_t)(addr - 0x8000);
        } else if (addr < 0xC000) {
            off = (mode == 2) ? 0
                              : cart->prg_bank_lo * 16384u + (uint32_t)(addr - 0x8000);
        } else {
            off = (mode == 2) ? cart->prg_bank_lo * 16384u + (uint32_t)(addr - 0xC000)
                              : cart->prg_bank_hi * 16384u + (uint32_t)(addr - 0xC000);
        }
        break;
    }
    case 2:
        if (addr < 0xC000)
            off = cart->prg_bank_lo * 16384u + (uint32_t)(addr - 0x8000);
        else
            off = cart->prg_bank_hi * 16384u + (uint32_t)(addr - 0xC000);
        break;
    case 3:
        off = (uint32_t)(addr - 0x8000);
        break;
    case 4: {
        uint32_t n8 = cart->prg_size / 8192;
        uint32_t bank;
        if (addr < 0xA000) {
            bank = cart->mmc3_prg_mode ? (n8 - 2) : cart->reg[6];
            off = (bank % n8) * 8192u + (uint32_t)(addr - 0x8000);
        } else if (addr < 0xC000) {
            bank = cart->reg[7];
            off = (bank % n8) * 8192u + (uint32_t)(addr - 0xA000);
        } else if (addr < 0xE000) {
            bank = cart->mmc3_prg_mode ? cart->reg[6] : (n8 - 2);
            off = (bank % n8) * 8192u + (uint32_t)(addr - 0xC000);
        } else {
            bank = n8 - 1;
            off = (bank % n8) * 8192u + (uint32_t)(addr - 0xE000);
        }
        break;
    }
    case 7:
        off = cart->prg_bank_lo * 32768u + (uint32_t)(addr - 0x8000);
        break;
    default:
        off = (uint32_t)(addr - 0x8000);
        break;
    }
    if (off >= cart->prg_size) off %= cart->prg_size;
    return cart->prg[off];
}

static uint32_t chr_bank_offset(Cart *cart, uint16_t addr)
{
    uint32_t n1k = cart->chr_size / 1024;
    uint32_t bank;
    if (n1k == 0) return 0;
    switch (cart->mapper) {
    case 0:
        return (uint32_t)addr;
    case 1:
        if (cart->reg[0] & 0x10) {
            bank = ((addr < 0x1000) ? cart->reg[1] : cart->reg[2]) * 4u + ((addr >> 10) & 3);
        } else {
            bank = (cart->reg[1] & 0x1E) * 4u + ((addr >> 10) & 3);
        }
        return (bank % n1k) * 1024u + (addr & 0x3FF);
    case 2:
        return (uint32_t)addr; /* CHR-RAM, no banking */
    case 3:
        return ((cart->chr_bank[0] % n1k) * 1024u * 0 + cart->chr_bank[0] * 8192u) + (addr & 0x1FFF);
    case 4: {
        /* The 8 KB PPU pattern space is split into eight 1 KB slots.  R0 and
         * R1 are *2 KB* registers covering PPU $0000-$07FF and $0800-$0FFF,
         * while R2-R5 are 1 KB registers at $1000-$13FF, $1400-$17FF,
         * $1800-$1BFF and $1C00-$1FFF.  When the CHR A12 inversion bit is set
         * the two 2 KB regions move to $1000-$1FFF and R2-R5 start at $0000
         * (MMC3 "Bank select", NESdev; previously this table was shifted by one
         * register, which fetched the wrong CHR banks). */
        static const uint8_t slot_map[2][8] = {
            { 0, 0, 1, 1, 2, 3, 4, 5 },   /* A12 = 0 */
            { 2, 3, 4, 5, 0, 0, 1, 1 }    /* A12 = 1 (inverted) */
        };
        /* Which 1 KB half of a 2 KB register a slot selects; unused for the
         * 1 KB registers, whose low bit is part of the bank number. */
        static const uint8_t slot_half[2][8] = {
            { 0, 1, 0, 1, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 1, 0, 1 }
        };
        int mode = cart->mmc3_chr_mode ? 1 : 0;
        uint32_t slot = (addr >> 10) & 7;
        uint32_t reg  = slot_map[mode][slot];
        bank = cart->chr_bank[reg];
        if (reg < 2)
            bank = (bank & 0xFEu) + slot_half[mode][slot];
        return (bank % n1k) * 1024u + (addr & 0x3FF);
    }
    case 7:
        return (uint32_t)addr;
    default:
        return (uint32_t)addr;
    }
}

uint8_t cart_chr_read(Cart *cart, uint16_t addr)
{
    uint32_t off;
    if (!cart->chr || cart->chr_size == 0) return 0;
    addr = (uint16_t)(addr & 0x1FFF);
    off = chr_bank_offset(cart, addr);
    if (off >= cart->chr_size) off %= cart->chr_size;
    return cart->chr[off];
}

void cart_chr_write(Cart *cart, uint16_t addr, uint8_t value)
{
    uint32_t off;
    if (!cart->chr_ram || !cart->chr) return; /* CHR-ROM ignores /WR */
    addr = (uint16_t)(addr & 0x1FFF);
    off = chr_bank_offset(cart, addr);
    if (off >= cart->chr_size) off %= cart->chr_size;
    cart->chr[off] = value;
}

/* ------------------------------------------------------------------ CPU -- */

static uint8_t bus_conflict_resolve(Cart *cart, uint16_t addr, uint8_t value)
{
    if (cart->bus_conflict && addr >= 0x8000)
        return (uint8_t)(value & prg_read(cart, addr));
    return value;
}

uint8_t cart_cpu_read(Cart *cart, uint16_t addr)
{
    if (addr >= 0x8000)
        return prg_read(cart, addr);
    if (addr >= 0x6000)
        return cart->prg_ram[addr & 0x1FFF];
    return 0;
}

/* MMC1 serial port (05-boards-and-io.md names MMC1 "serial PRG/CHR banking"). */
static void mmc1_write(Cart *cart, uint16_t addr, uint8_t value)
{
    if (value & 0x80) {
        cart->shift = 0x10;
        cart->shift_count = 0;
        cart->reg[0] |= 0x0C;
        return;
    }
    {
        cart->shift = (uint8_t)((cart->shift >> 1) | ((value & 1) << 4));
        cart->shift_count++;
        if (cart->shift_count == 5) {
            uint8_t data = (uint8_t)(cart->shift & 0x1F);
            int reg = (addr >> 13) & 3;
            cart->reg[reg] = data;
            cart->shift = 0x10;
            cart->shift_count = 0;
            if (reg == 0) {
                switch (data & 3) {
                    case 0: cart->mirroring = MIRROR_SINGLE_LO; break;
                    case 1: cart->mirroring = MIRROR_SINGLE_HI; break;
                    case 2: cart->mirroring = MIRROR_VERTICAL; break;
                    case 3: cart->mirroring = MIRROR_HORIZONTAL; break;
                }
            } else if (reg == 3) {
                /* PRG bank register: bits 0-3 hold the 16 KB bank number for
                 * the switchable window (bit 4 is only wired on boards with
                 * more than 256 KB of PRG).  Bit 0 goes to the ROM's A14 and is
                 * therefore NOT a "32 KB board" marker: clearing it here made
                 * every odd 16 KB bank unreachable, so a game that switches to
                 * bank 3 (Darkwing Duck does) silently read bank 2 and crashed.
                 * The one place bit 0 really is unused is the 32 KB modes,
                 * which prg_read handles by masking the bank itself. */
                uint32_t banks = cart->prg_size / 16384;
                cart->prg_bank_lo = banks ? (uint32_t)(data & 0x1F) % banks : 0;
            }
        }
    }
}

/* MMC3 (spec: "8 KB PRG / 1 KB CHR banking, scanline IRQ, 4-screen option"). */
static void mmc3_write(Cart *cart, uint16_t addr, uint8_t value)
{
    bool even = (addr & 1) == 0;
    if (addr < 0xA000) {
        if (even) {
            cart->mmc3_bank_select = (uint8_t)(value & 7);
            cart->mmc3_prg_mode = (value & 0x40) != 0;
            cart->mmc3_chr_mode = (value & 0x80) != 0;
        } else {
            switch (cart->mmc3_bank_select) {
            case 0: cart->chr_bank[0] = value; break;
            case 1: cart->chr_bank[1] = value; break;
            case 2: cart->chr_bank[2] = value; break;
            case 3: cart->chr_bank[3] = value; break;
            case 4: cart->chr_bank[4] = value; break;
            case 5: cart->chr_bank[5] = value; break;
            case 6: cart->reg[6] = value; break;
            case 7: cart->reg[7] = value; break;
            default: break;
            }
            /* The bank registers are stored raw (R0..R7) and only reordered /
             * masked at read time by the CHR mode bit and the 2 KB alignment,
             * so do NOT wrap or mask them here (SPEC GAP CART-2: the
             * specification does not describe these registers at all). */
        }
    } else if (addr < 0xC000) {
        if (even) {
            if (cart->four_screen) {
                /* 4-screen: CIRAM A10 is ignored. */
            } else if (value & 1) {
                cart->mirroring = MIRROR_HORIZONTAL;
            } else {
                cart->mirroring = MIRROR_VERTICAL;
            }
        }
    } else if (addr < 0xE000) {
        if (even) cart->irq_latch = value;
        else cart->irq_counter = 0;
    } else {
        if (even) {
            /* $E000: disable IRQ and acknowledge it. */
            cart->irq_enable = false;
            cart->irq_pending = false;
        } else {
            /* $E001: enable IRQ. */
            cart->irq_enable = true;
        }
    }
}

void cart_cpu_write(Cart *cart, uint16_t addr, uint8_t value)
{
    if (addr >= 0x6000 && addr < 0x8000) {
        cart->prg_ram[addr & 0x1FFF] = value;
        return;
    }
    if (addr < 0x8000)
        return;

    value = bus_conflict_resolve(cart, addr, value);

    switch (cart->mapper) {
    case 0:
        break; /* fixed */
    case 1:
        mmc1_write(cart, addr, value);
        break;
    case 2:
        cart->prg_bank_lo = value & 0x0F;
        break;
    case 3:
        cart->chr_bank[0] = value & 0x03;
        break;
    case 4:
        mmc3_write(cart, addr, value);
        break;
    case 7:
        cart->prg_bank_lo = value & 0x0F;
        cart->mirroring = (value & 0x10) ? MIRROR_SINGLE_HI : MIRROR_SINGLE_LO;
        break;
    default:
        break;
    }
}

/* -------------------------------------------------------------- mirroring -- */

int cart_ciram_a10(Cart *cart, uint16_t addr)
{
    uint16_t a = (uint16_t)((addr >> 10) & 3);
    switch (cart->mirroring) {
    case MIRROR_VERTICAL:   return (a & 1);       /* $2000=$2800, $2400=$2C00 */
    case MIRROR_HORIZONTAL: return (a >> 1) & 1;  /* $2000=$2400, $2800=$2C00 */
    case MIRROR_SINGLE_LO:  return 0;
    case MIRROR_SINGLE_HI:  return 1;
    case MIRROR_FOUR:       return (a & 1);
    default:                return 0;
    }
}

bool cart_ciram_enabled(Cart *cart) { return !cart->four_screen; }

/* MMC3 scanline counter.
 *
 * The specification describes the MMC3 only as "8 KB PRG / 1 KB CHR banking,
 * scanline IRQ, 4-screen option" and states that per-mapper register semantics
 * are out of scope, so the counter behaviour used here is the standard one and
 * is recorded in SPEC-GAPS.md.
 *
 * The counter is clocked by a rising edge on the PPU address line A12.  It is
 * not enough to test A12's level: the pattern fetches assert A12 repeatedly
 * within a scanline, and clocking on each of those would count far too fast.
 * A12 must be seen low for a while before it goes high again, which is what
 * separates consecutive scanlines (and the sprite fetch from the next
 * scanline's first background fetch).  A12 goes low for dots 0-7 of every
 * scanline, so the filter is expressed in PPU dots.
 */
#define MMC3_A12_LOW_DOTS 8

void cart_ppu_a12(Cart *cart, uint16_t addr, bool rendering)
{

    bool high = (addr & 0x1000) != 0;

    if (cart->mapper != 4) {
        cart->a12_high = high;
        return;
    }

    /* Track the level even while rendering is off, so that the first fetch
     * after rendering is enabled does not look like a stale rising edge. */
    if (!rendering) {
        if (!high) {
            cart->a12_high = false;
            cart->a12_low_dots = 0;
        }
        return;
    }

    if (!high) {
        cart->a12_high = false;
        if (cart->a12_low_dots < MMC3_A12_LOW_DOTS)
            cart->a12_low_dots++;
        return;
    }

    /* A12 is high: clock the counter only if it was low long enough to count
     * as the start of a new scanline. */
    if (!cart->a12_high && cart->a12_low_dots >= MMC3_A12_LOW_DOTS) {
        cart->a12_sprite_edges = 0;
        if (cart->irq_counter == 0) {
            cart->irq_counter = cart->irq_latch;
            if (cart->irq_enable && cart->irq_latch == 0)
                cart->irq_pending = true;
        } else {
            cart->irq_counter--;
            if (cart->irq_counter == 0 && cart->irq_enable)
                cart->irq_pending = true;
        }
    } else if (!cart->a12_high && cart->irq_enable) {
        /* A12 went high without a full low period (the sprite fetch inside the
         * same scanline).  The standard behaviour is to still clock after the
         * third such occurrence rather than never clocking. */
        if (++cart->a12_sprite_edges >= 3) {
            cart->a12_sprite_edges = 0;
            if (cart->irq_counter == 0) {
                cart->irq_counter = cart->irq_latch;
                if (cart->irq_latch == 0)
                    cart->irq_pending = true;
            } else {
                cart->irq_counter--;
                if (cart->irq_counter == 0)
                    cart->irq_pending = true;
            }
        }
    }

    cart->a12_high = true;
    cart->a12_low_dots = 0;
}

bool cart_irq(Cart *cart) { return cart->irq_pending; }
