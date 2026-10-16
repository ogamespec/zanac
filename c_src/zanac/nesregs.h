/* nesregs.h - the NES register addresses, mirroring src/nesregs.asm.
 *
 * Every name and value is identical to the original file so that a ported
 * statement such as `STA PPU_CTRL_REG1` becomes `STA(PPU_CTRL_REG1)`.
 */
#ifndef ZANAC_NESREGS_H
#define ZANAC_NESREGS_H

/* PPU */
#define PPU_CTRL_REG1   0x2000
#define PPU_CTRL_REG2   0x2001
#define PPU_STATUS      0x2002
#define PPU_SPR_ADDR    0x2003
#define PPU_SPR_DATA    0x2004
#define PPU_SCROLL_REG  0x2005
#define PPU_ADDRESS     0x2006
#define PPU_DATA        0x2007

/* APU */
#define APU_REG_BASE       0x4000
#define APU_SQUARE1_REG    0x4000
#define APU_SQUARE2_REG    0x4004
#define APU_TRIANGLE_REG   0x4008
#define APU_NOISE_REG      0x400C
#define APU_DELTA_REG      0x4010
#define APU_MASTERCTRL_REG 0x4015

/* IO */
#define PPU_SPR_DMA  0x4014
#define JOYPAD_PORT1 0x4016
#define JOYPAD_PORT2 0x4017

#endif /* ZANAC_NESREGS_H */
