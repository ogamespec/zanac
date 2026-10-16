/* bank7/C95C.c - bank 7, $C938-$CB4B: the object erase/shape helpers, the PRNG,
 * the PPU control wrappers, the OAM buffer clear, the pad reader and the
 * start-game check.
 *
 * Ported from src/bank7.asm lines 1653-2221 (addresses from Build/bank7.lst).
 *
 * Two constructs need review:
 *   - start_game_check ($CB4B) branches out of its own body with
 *     `BNE continue_level_check` ($CC36, ported in bank7/CC36.c); per
 *     PORTING.md section 5 it becomes a call plus return.
 *   - randomize ($CAB2) uses TSX/TXA (the stack pointer) as PRNG entropy.  A C
 *     call does not push a return address, so the port's SP differs from the
 *     original's and the sequence is not bit-exact (see the note there).
 * The chopped-procedure fall-throughs (sub_CA0F -> sub_CA12, ppu_disable_all ->
 * ppu_set_control2, ppu_nmi_disable -> ppu_set_control1, ppu_enable_all ->
 * ppu_nmi_enable, clear_oam_buffer -> mem_page_fill) are ported as a call plus
 * a return, which reproduces the registers and flags of the original.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $C938  enemy_erase_2 - clear the game-object arrays for object X
 *
 *  Original: src/bank7.asm:1653-1667
 *      LDA #0 / STA $528,X / STA $542,X / STA $5C4,X / STA $5DE,X /
 *      STA $5F8,X / STA $62C,X / STA $612,X / STA $646,X / STA $5AA,X /
 *      STA $6E2,X / STA $6FC,X / RTS
 * ------------------------------------------------------------------------ */
void enemy_erase_2(void)                     /* $C938 */
{
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    STA(ABSX(byte_542));                     /* STA $542,X */
    STA(ABSX(byte_5C4));                     /* STA $5C4,X */
    STA(ABSX(byte_5DE));                     /* STA $5DE,X */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    STA(ABSX(byte_612));                     /* STA $612,X */
    STA(ABSX(byte_646));                     /* STA $646,X */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C95C  sub_C95C - split byte_18/byte_19 into the shape address bytes
 *
 *  Original: src/bank7.asm:1673-1700
 *      LDA byte_19 / LSR / LSR / LSR / STA byte_10 / LSR / LSR /
 *      STA byte_11 / LDA #8 / STA byte_19 / LDA byte_18 / STA byte_12 /
 *      AND #$F8 / ASL / ROL byte_19 / ASL / ROL byte_19 / ORA byte_10 /
 *      STA byte_18 / LDA byte_12 / AND #$E0 / LSR / LSR / ORA byte_11 /
 *      TAY / RTS
 * ------------------------------------------------------------------------ */
void sub_C95C(void)                          /* $C95C */
{
    LDA(byte_19);                            /* LDA byte_19 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    STA(byte_10);                            /* STA byte_10 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    STA(byte_11);                            /* STA byte_11 */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_19);                            /* STA byte_19 */
    LDA(byte_18);                            /* LDA byte_18 */
    STA(byte_12);                            /* STA byte_12 */
    ANDI(0xF8);                              /* AND #$F8 */
    ASL_A();                                 /* ASL */
    ROL(byte_19);                            /* ROL byte_19 */
    ASL_A();                                 /* ASL */
    ROL(byte_19);                            /* ROL byte_19 */
    ORA(byte_10);                            /* ORA byte_10 */
    STA(byte_18);                            /* STA byte_18 */
    LDA(byte_12);                            /* LDA byte_12 */
    ANDI(0xE0);                              /* AND #$E0 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ORA(byte_11);                            /* ORA byte_11 */
    TAY();                                   /* TAY */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C985  sub_C985 - queue one shape of $462.. into the OAM buffer
 *
 *  Original: src/bank7.asm:1706-1785
 *      PHA / LDA #8 / JSR sub_C7EB / JSR sub_C7F8 / PLA / STA byte_10 /
 *      TXA / PHA / LDX byte_21 / LDA byte_19 / AND #$F / ORA #$C0 /
 *      STA $460,X / LDA byte_18 / STA $461,X / TYA / ORA #$C0 /
 *      STA $466,X / LDA #0 / STA byte_11 / LDA byte_10 / ASL /
 *      ROL byte_11 / ASL / ROL byte_11 / ADC byte_10 / BCC loc_C9BB /
 *      INC byte_11 / loc_C9BB: ADC byte_BA / STA byte_10 /
 *      BCC loc_C9C3 / INC byte_11 / loc_C9C3: LDA last_bank_num / PHA /
 *      JSR switch_bank_5 / LDY #0 /
 *      loc_C9CB: LDA ($10),Y / STA $462,X / INX / INY / CPY #4 /
 *      BNE loc_C9CB / LDA ($10),Y / STA byte_10 / LDA byte_18 / AND #2 /
 *      LSR / TAY / LDA byte_18 / AND #$40 / BEQ loc_C9E8 / INY / INY /
 *      loc_C9E8: LDA $462,X / TAX / .BYTE $BD,$44,$00 (LDA !$44,X) /
 *      AND byte_D7DB,Y / STA byte_11 / LDA byte_D7DF,Y / LDY byte_10 /
 *      AND $16E,Y / ORA byte_11 / .BYTE $9D,$44,$00 (STA !$44,X) /
 *      LDX byte_21 / STA $467,X / PLA / JSR switch_bank / PLA / TAX /
 *      JMP sub_C807
 * ------------------------------------------------------------------------ */
void sub_C985(void)                          /* $C985 */
{
    PHA();                                   /* PHA */
    LDAI(8);                                 /* LDA #8 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    PLA();                                   /* PLA */
    STA(byte_10);                            /* STA byte_10 */
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */
    LDX(byte_21);                            /* LDX byte_21 */
    LDA(byte_19);                            /* LDA byte_19 */
    ANDI(0x0F);                              /* AND #$F */
    ORAI(0xC0);                              /* ORA #$C0 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(byte_18);                            /* LDA byte_18 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    TYA();                                   /* TYA */
    ORAI(0xC0);                              /* ORA #$C0 */
    STA(ABSX(0x0466));                       /* STA $466,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_11);                            /* STA byte_11 */
    LDA(byte_10);                            /* LDA byte_10 */
    ASL_A();                                 /* ASL */
    ROL(byte_11);                            /* ROL byte_11 */
    ASL_A();                                 /* ASL */
    ROL(byte_11);                            /* ROL byte_11 */
    ADC(byte_10);                            /* ADC byte_10 */
    BCC(loc_C9BB);                           /* BCC loc_C9BB */
    INC(byte_11);                            /* INC byte_11 */

loc_C9BB:                                    /* $C9BB */
    ADC(byte_BA);                            /* ADC byte_BA */
    STA(byte_10);                            /* STA byte_10 */
    BCC(loc_C9C3);                           /* BCC loc_C9C3 */
    INC(byte_11);                            /* INC byte_11 */

loc_C9C3:                                    /* $C9C3 */
    LDA(last_bank_num);                      /* LDA last_bank_num */
    PHA();                                   /* PHA */
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDYI(0);                                 /* LDY #0 */

loc_C9CB:                                    /* $C9CB */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    STA(ABSX(0x0462));                       /* STA $462,X */
    INX();                                   /* INX */
    INY();                                   /* INY */
    CPYI(4);                                 /* CPY #4 */
    BNE(loc_C9CB);                           /* BNE loc_C9CB */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    STA(byte_10);                            /* STA byte_10 */
    LDA(byte_18);                            /* LDA byte_18 */
    ANDI(2);                                 /* AND #2 */
    LSR_A();                                 /* LSR */
    TAY();                                   /* TAY */
    LDA(byte_18);                            /* LDA byte_18 */
    ANDI(0x40);                              /* AND #$40 */
    BEQ(loc_C9E8);                           /* BEQ loc_C9E8 */
    INY();                                   /* INY */
    INY();                                   /* INY */

loc_C9E8:                                    /* $C9E8 */
    LDA(ABSX(0x0462));                       /* LDA $462,X */
    TAX();                                   /* TAX */
    LDA(ABSX(0x0044));                       /* .BYTE $BD,$44,$00 -> LDA !$44,X (force non zero page) */
    AND(ABSY(byte_D7DB));                    /* AND byte_D7DB,Y */
    STA(byte_11);                            /* STA byte_11 */
    LDA(ABSY(byte_D7DF));                    /* LDA byte_D7DF,Y */
    LDY(byte_10);                            /* LDY byte_10 */
    AND(ABSY(0x016E));                       /* AND $16E,Y */
    ORA(byte_11);                            /* ORA byte_11 */
    STA(ABSX(0x0044));                       /* .BYTE $9D,$44,$00 -> STA !$44,X (force non zero page) */
    LDX(byte_21);                            /* LDX byte_21 */
    STA(ABSX(0x0467));                       /* STA $467,X */
    PLA();                                   /* PLA */
    switch_bank();                           /* JSR switch_bank */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    sub_C807();                              /* JMP sub_C807 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CA0F  sub_CA0F - shape entry with the count 8
 *
 *  Original: src/bank7.asm:1791-1794
 *      PHA / LDA #8
 *  (no RTS: execution continues into sub_CA12)
 * ------------------------------------------------------------------------ */
void sub_CA0F(void)                          /* $CA0F */
{
    PHA();                                   /* PHA */
    LDAI(8);                                 /* LDA #8 */
    sub_CA12();                              /* falls through into $CA12 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CA12  sub_CA12 - build a 2x2 shape block of byte_10 bytes
 *
 *  Original: src/bank7.asm:1800-1871
 *      JSR sub_C7EB / JSR sub_C7F8 / PLA / STA byte_10 / TXA / PHA /
 *      LDX byte_21 / LDA byte_19 / AND #$F / ORA #$C0 / STA $460,X /
 *      LDA byte_18 / STA $461,X / TYA / ORA #$C0 / STA $466,X /
 *      LDY byte_10 / TYA / STA $462,X / INY / TYA / STA $464,X / INY /
 *      TYA / STA $463,X / INY / TYA / STA $465,X / LDA byte_18 / AND #2 /
 *      LSR / TAY / LDA byte_18 / AND #$40 / BEQ loc_CA56 / INY / INY /
 *      loc_CA56: LDA byte_25 / CMP #8 / BNE loc_CA7C / LDA $466,X /
 *      TAX / .BYTE $BD,$44,$00 (LDA $44,X) / AND byte_D7DB,Y /
 *      STA byte_11 / LDA byte_D7DF,Y / AND byte_63 / ORA byte_11 /
 *      .BYTE $9D,$44,$00 (STA $44,X) / LDX byte_21 / STA $467,X /
 *      PLA / TAX / JMP sub_C807 /
 *      loc_CA7C: LDA $460,X / ORA #$E0 / STA $460,X / LDA byte_D7DB,Y /
 *      STA $468,X / LDA byte_D7DF,Y / AND byte_63 / STA $467,X /
 *      PLA / TAX / JMP sub_C807
 * ------------------------------------------------------------------------ */
void sub_CA12(void)                          /* $CA12 */
{
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    PLA();                                   /* PLA */
    STA(byte_10);                            /* STA byte_10 */
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */
    LDX(byte_21);                            /* LDX byte_21 */
    LDA(byte_19);                            /* LDA byte_19 */
    ANDI(0x0F);                              /* AND #$F */
    ORAI(0xC0);                              /* ORA #$C0 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(byte_18);                            /* LDA byte_18 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    TYA();                                   /* TYA */
    ORAI(0xC0);                              /* ORA #$C0 */
    STA(ABSX(0x0466));                       /* STA $466,X */
    LDY(byte_10);                            /* LDY byte_10 */
    TYA();                                   /* TYA */
    STA(ABSX(0x0462));                       /* STA $462,X */
    INY();                                   /* INY */
    TYA();                                   /* TYA */
    STA(ABSX(0x0464));                       /* STA $464,X */
    INY();                                   /* INY */
    TYA();                                   /* TYA */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INY();                                   /* INY */
    TYA();                                   /* TYA */
    STA(ABSX(0x0465));                       /* STA $465,X */
    LDA(byte_18);                            /* LDA byte_18 */
    ANDI(2);                                 /* AND #2 */
    LSR_A();                                 /* LSR */
    TAY();                                   /* TAY */
    LDA(byte_18);                            /* LDA byte_18 */
    ANDI(0x40);                              /* AND #$40 */
    BEQ(loc_CA56);                           /* BEQ loc_CA56 */
    INY();                                   /* INY */
    INY();                                   /* INY */

loc_CA56:                                    /* $CA56 */
    LDA(byte_25);                            /* LDA byte_25 */
    CMPI(8);                                 /* CMP #8 */
    BNE(loc_CA7C);                           /* BNE loc_CA7C */
    LDA(ABSX(0x0466));                       /* LDA $466,X */
    TAX();                                   /* TAX */
    LDA(ABSX(0x0044));                       /* .BYTE $BD,$44,$00 -> LDA $44,X (force non zero page) */
    AND(ABSY(byte_D7DB));                    /* AND byte_D7DB,Y */
    STA(byte_11);                            /* STA byte_11 */
    LDA(ABSY(byte_D7DF));                    /* LDA byte_D7DF,Y */
    AND(byte_63);                            /* AND byte_63 */
    ORA(byte_11);                            /* ORA byte_11 */
    STA(ABSX(0x0044));                       /* .BYTE $9D,$44,$00 -> STA $44,X (force non zero page) */
    LDX(byte_21);                            /* LDX byte_21 */
    STA(ABSX(0x0467));                       /* STA $467,X */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    sub_C807();                              /* JMP sub_C807 */
    RTS();

loc_CA7C:                                    /* $CA7C */
    LDA(ABSX(0x0460));                       /* LDA $460,X */
    ORAI(0xE0);                              /* ORA #$E0 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(ABSY(byte_D7DB));                    /* LDA byte_D7DB,Y */
    STA(ABSX(0x0468));                       /* STA $468,X */
    LDA(ABSY(byte_D7DF));                    /* LDA byte_D7DF,Y */
    AND(byte_63);                            /* AND byte_63 */
    STA(ABSX(0x0467));                       /* STA $467,X */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    sub_C807();                              /* JMP sub_C807 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CA97  sub_CA97 - shape entry with the count 9
 *
 *  Original: src/bank7.asm:1877-1881
 *      PHA / LDA #9 / JMP sub_CA12
 * ------------------------------------------------------------------------ */
void sub_CA97(void)                          /* $CA97 */
{
    PHA();                                   /* PHA */
    LDAI(9);                                 /* LDA #9 */
    sub_CA12();                              /* JMP sub_CA12 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CA9D  ppu_cram_load - copy the palette at ($18) to cram_new_palette
 *
 *  Original: src/bank7.asm:1888-1902
 *      LDY #0 / loc_CA9F: LDA ($18),Y / STA cram_new_palette,Y / INY /
 *      CPY #$19 / BNE loc_CA9F / LDA #3 / ORA cram_update_mode /
 *      STA cram_update_mode / RTS
 * ------------------------------------------------------------------------ */
void ppu_cram_load(void)                     /* $CA9D */
{
    LDYI(0);                                 /* LDY #0 */

loc_CA9F:                                    /* $CA9F */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSY(cram_new_palette));             /* STA cram_new_palette,Y */
    INY();                                   /* INY */
    CPYI(0x19);                              /* CPY #$19 */
    BNE(loc_CA9F);                           /* BNE loc_CA9F */
    LDAI(3);                                 /* LDA #3 */
    ORA(cram_update_mode);                   /* ORA cram_update_mode */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CAB2  randomize - PRNG seeded from the last result, the stack and a table
 *
 *  Original: src/bank7.asm:1909-1926
 *      STX byte_10 / LDX last_randomize_result / SEC / ROL /
 *      EOR randomize_tab,X / ADC $10,X / STA last_randomize_result /
 *      TSX / TXA / LDX byte_10 / ADC last_randomize_result /
 *      ADC $470,X / ADC byte_A2 / STA last_randomize_result / RTS
 * ------------------------------------------------------------------------ */
void randomize(void)                         /* $CAB2 */
{
    STX(byte_10);                            /* STX byte_10 */
    LDX(last_randomize_result);              /* LDX last_randomize_result */
    SEC();                                   /* SEC */
    ROL_A();                                 /* ROL */
    EOR(ABSX(randomize_tab));                /* EOR randomize_tab,X */
    ADC(ZPX(byte_10));                       /* ADC $10,X */
    STA(last_randomize_result);              /* STA last_randomize_result */
    /* TSX/TXA contribute the stack pointer as entropy.  The original SP points
     * below the JSR return address the caller pushed, which a C call does not
     * push, so the port's SP (and therefore this PRNG stream) is not identical
     * to the 6502 one.  The instructions are kept literally. */
    TSX();                                   /* TSX */
    TXA();                                   /* TXA */
    LDX(byte_10);                            /* LDX byte_10 */
    ADC(last_randomize_result);              /* ADC last_randomize_result */
    ADC(ABSX(0x0470));                       /* ADC $470,X */
    ADC(byte_A2);                            /* ADC byte_A2 */
    STA(last_randomize_result);              /* STA last_randomize_result */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CACD  ppu_disable_all - disable NMI and BG/sprites
 *
 *  Original: src/bank7.asm:1932-1938
 *      JSR ppu_nmi_disable / LDA PPU_STATUS / LDA last_ppu_ctrl2 /
 *      AND #$E7
 *  (no RTS: execution continues into ppu_set_control2)
 * ------------------------------------------------------------------------ */
void ppu_disable_all(void)                   /* $CACD */
{
    ppu_nmi_disable();                       /* JSR ppu_nmi_disable */
    LDA(PPU_STATUS);                         /* LDA PPU_STATUS */
    LDA(last_ppu_ctrl2);                     /* LDA last_ppu_ctrl2 */
    ANDI(0xE7);                              /* AND #$E7 */
    ppu_set_control2();                      /* falls through into $CAD7 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CAD7  ppu_set_control2 - store A in PPU_CTRL_REG2 and remember it
 *
 *  Original: src/bank7.asm:1945-1950
 *      STA PPU_CTRL_REG2 / STA last_ppu_ctrl2 / RTS
 * ------------------------------------------------------------------------ */
void ppu_set_control2(void)                  /* $CAD7 */
{
    STA(PPU_CTRL_REG2);                      /* STA PPU_CTRL_REG2 */
    STA(last_ppu_ctrl2);                     /* STA last_ppu_ctrl2 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CADD  ppu_enable_picture - enable BG and sprites
 *
 *  Original: src/bank7.asm:1957-1963
 *      LDA last_ppu_ctrl2 / ORA #$18 / STA last_ppu_ctrl2 /
 *      STA PPU_CTRL_REG2 / RTS
 * ------------------------------------------------------------------------ */
void ppu_enable_picture(void)                /* $CADD */
{
    LDA(last_ppu_ctrl2);                     /* LDA last_ppu_ctrl2 */
    ORAI(0x18);                              /* ORA #$18 */
    STA(last_ppu_ctrl2);                     /* STA last_ppu_ctrl2 */
    STA(PPU_CTRL_REG2);                      /* STA PPU_CTRL_REG2 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CAE7  ppu_nmi_disable - clear the NMI bit of PPU_CTRL_REG1
 *
 *  Original: src/bank7.asm:1969-1973
 *      LDA last_ppu_ctrl1 / AND #$7F
 *  (no RTS: execution continues into ppu_set_control1)
 * ------------------------------------------------------------------------ */
void ppu_nmi_disable(void)                   /* $CAE7 */
{
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    ANDI(0x7F);                              /* AND #$7F */
    ppu_set_control1();                      /* falls through into $CAEB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CAEB  ppu_set_control1 - store A in PPU_CTRL_REG1 and remember it
 *
 *  Original: src/bank7.asm:1979-1983
 *      STA PPU_CTRL_REG1 / STA last_ppu_ctrl1 / RTS
 * ------------------------------------------------------------------------ */
void ppu_set_control1(void)                  /* $CAEB */
{
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */
    STA(last_ppu_ctrl1);                     /* STA last_ppu_ctrl1 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CAF1  ppu_enable_all - restore the scroll registers, BG/sprites and NMI
 *
 *  Original: src/bank7.asm:1989-1993
 *      JSR ppu_update_scroll_regs / JSR ppu_enable_picture
 *  (no RTS: execution continues into ppu_nmi_enable)
 * ------------------------------------------------------------------------ */
void ppu_enable_all(void)                    /* $CAF1 */
{
    ppu_update_scroll_regs();                /* JSR ppu_update_scroll_regs */
    ppu_enable_picture();                    /* JSR ppu_enable_picture */
    ppu_nmi_enable();                        /* falls through into $CAF7 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CAF7  ppu_nmi_enable - set the NMI bit of PPU_CTRL_REG1
 *
 *  Original: src/bank7.asm:1999-2006
 *      LDA last_ppu_ctrl1 / ORA #$80 / STA last_ppu_ctrl1 /
 *      STA PPU_CTRL_REG1 / RTS
 * ------------------------------------------------------------------------ */
void ppu_nmi_enable(void)                    /* $CAF7 */
{
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(last_ppu_ctrl1);                     /* STA last_ppu_ctrl1 */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CB01  clear_oam_buffer - fill pages 2..2 with $F8 (the OAM DMA buffer)
 *
 *  Original: src/bank7.asm:2012-2018
 *      LDA #$F8 / LDX #2 / LDY #2 / JMP mem_page_fill
 * ------------------------------------------------------------------------ */
void clear_oam_buffer(void)                  /* $CB01 */
{
    LDAI(0xF8);                              /* LDA #$F8 */
    LDXI(2);                                 /* LDX #2 */
    LDYI(2);                                 /* LDY #2 */
    mem_page_fill();                         /* JMP mem_page_fill */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CB0A  pad_read - shift in both controllers and derive the new presses
 *
 *  Original: src/bank7.asm:2025-2060
 *      LDA #1 / STA JOYPAD_PORT1 / LDA #0 / STA JOYPAD_PORT1 / LDX #8 /
 *      loc_CB16: LDA JOYPAD_PORT1 / LSR / ROL pad_test1 / LSR /
 *      ROL temp_0 / LDA JOYPAD_PORT2 / LSR / ROL pad_test2 / LSR /
 *      ROL temp_1 / DEX / BNE loc_CB16 / LDX #1 /
 *      loc_CB2D: LDY pad_buttons_1,X / LDA pad_test1,X / ORA 0,X /
 *      STA pad_buttons_1,X / TYA / EOR pad_buttons_1,X /
 *      AND pad_buttons_1,X / STA pad_test1,X / DEX / BPL loc_CB2D / RTS
 * ------------------------------------------------------------------------ */
void pad_read(void)                          /* $CB0A */
{
    LDAI(1);                                 /* LDA #1 */
    STA(JOYPAD_PORT1);                       /* STA JOYPAD_PORT1 */
    LDAI(0);                                 /* LDA #0 */
    STA(JOYPAD_PORT1);                       /* STA JOYPAD_PORT1 */
    LDXI(8);                                 /* LDX #8 */

loc_CB16:                                    /* $CB16 */
    LDA(JOYPAD_PORT1);                       /* LDA JOYPAD_PORT1 */
    LSR_A();                                 /* LSR */
    ROL(pad_test1);                          /* ROL pad_test1 */
    LSR_A();                                 /* LSR */
    ROL(temp_0);                             /* ROL temp_0 */
    LDA(JOYPAD_PORT2);                       /* LDA JOYPAD_PORT2 */
    LSR_A();                                 /* LSR */
    ROL(pad_test2);                          /* ROL pad_test2 */
    LSR_A();                                 /* LSR */
    ROL(temp_1);                             /* ROL temp_1 */
    DEX();                                   /* DEX */
    BNE(loc_CB16);                           /* BNE loc_CB16 */
    LDXI(1);                                 /* LDX #1 */

loc_CB2D:                                    /* $CB2D */
    LDY(ZPX(pad_buttons_1));                 /* LDY pad_buttons_1,X */
    LDA(ZPX(pad_test1));                     /* LDA pad_test1,X */
    ORA(ZPX(temp_0));                        /* ORA 0,X */
    STA(ZPX(pad_buttons_1));                 /* STA pad_buttons_1,X */
    TYA();                                   /* TYA */
    EOR(ZPX(pad_buttons_1));                 /* EOR pad_buttons_1,X */
    AND(ZPX(pad_buttons_1));                 /* AND pad_buttons_1,X */
    STA(ZPX(pad_test1));                     /* STA pad_test1,X */
    DEX();                                   /* DEX */
    BPL(loc_CB2D);                           /* BPL loc_CB2D */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CB40  sub_CB40 - clear the array $172...$179
 *
 *  Original: src/bank7.asm:2067-2077
 *      LDA #0 / LDY #8 / loc_CB44: STA $179,Y / DEY / BNE loc_CB44 / RTS
 * ------------------------------------------------------------------------ */
void sub_CB40(void)                          /* $CB40 */
{
    LDAI(0x00);                              /* LDA #0 */
    LDYI(8);                                 /* LDY #8 */

loc_CB44:                                    /* $CB44 */
    STA(ABSY(0x0179));                       /* STA $179,Y */
    DEY();                                   /* DEY */
    BNE(loc_CB44);                           /* BNE loc_CB44 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CB4B  start_game_check - load the menu screens and set up the game/demo
 *
 *  Original: src/bank7.asm:2083-2221
 *      LDA #0 / STA byte_19 / STA byte_18 / LDA #0 / LDY #4 /
 *      JSR sub_C8DF / LDA pad_buttons_1 / ORA pad_buttons_2 / AND #$C0 /
 *      CMP #$C0 / BEQ loc_CB67 / LDA byte_19B / BEQ loc_CB76 /
 *      loc_CB67: LDA #1 / STA byte_19 / LDA #$40 / STA byte_18 /
 *      LDA #1 / LDY #4 / JSR sub_C8DF /
 *      loc_CB76: LDA #$17 / STA byte_19 / LDA #$F0 / STA byte_18 /
 *      LDA #3 / LDY #4 / JSR sub_C8DF / JSR sub_C8BA / LDA #3 /
 *      STA byte_32 / LDA #0 / STA game_mode / STA zanac_main_weapon_level /
 *      STA byte_49 / STA byte_46 / STA byte_47 / STA byte_48 / STA byte_4B /
 *      STA byte_5A / STA byte_4D / STA byte_4A / STA byte_56 / STA byte_54 /
 *      STA byte_176 / STA byte_177 / STA byte_178 / STA byte_39 /
 *      STA byte_55 / STA byte_C0 / STA byte_5D / LDA #$10 / STA byte_77 /
 *      LDA #2 / STA byte_45 / STA byte_179 / LDA #7 / STA byte_62 /
 *      LDA #$F / STA byte_51 / JSR sub_18012 / JSR j_set_default_weapon /
 *      LDA pad_buttons_1 / AND #$10 / BNE continue_level_check / LDA #$80 /
 *      STA game_mode / JSR randomize / AND #7 / CLC / ADC #1 /
 *      STA current_level / LDA #$14 / STA demo_counter_2 / LDA #1 /
 *      STA demo_button_update_counter / LDA #8 / STA byte_51 /
 *      JSR randomize / AND #$1F / STA demo_button_tab_pointer /
 *      INC byte_CD / LDA byte_CD / AND #$F / STA byte_CD / TAY /
 *      LDA unk_CC16,Y / STA byte_18 / LDA unk_CC26,Y / STA byte_19 /
 *      loc_CC0A: BEQ locret_CC15 / LDA byte_18 / JSR sub_1800F /
 *      DEC byte_19 / BNE loc_CC0A / locret_CC15: RTS
 *
 *  The BNE at $CBD4 branches out of this procedure into continue_level_check
 *  ($CC36, ported in bank7/CC36.c); PORTING.md section 5 ports that as a call
 *  plus a return.  loc_CC0A's BEQ tests the flags left by `LDA unk_CC26,Y`,
 *  exactly like the original (the STA in between does not touch them).
 * ------------------------------------------------------------------------ */
void start_game_check(void)                  /* $CB4B */
{
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_19);                            /* STA byte_19 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(0x00);                              /* LDA #0 */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ORA(pad_buttons_2);                      /* ORA pad_buttons_2 */
    ANDI(0xC0);                              /* AND #$C0 */
    CMPI(0xC0);                              /* CMP #$C0 */
    BEQ(loc_CB67);                           /* BEQ loc_CB67 */
    LDA(byte_19B);                           /* LDA byte_19B */
    BEQ(loc_CB76);                           /* BEQ loc_CB76 */

loc_CB67:                                    /* $CB67 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0x40);                              /* LDA #$40 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(1);                                 /* LDA #1 */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */

loc_CB76:                                    /* $CB76 */
    LDAI(0x17);                              /* LDA #$17 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0xF0);                              /* LDA #$F0 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(3);                                 /* LDA #3 */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */
    sub_C8BA();                              /* JSR sub_C8BA */
    LDAI(3);                                 /* LDA #3 */
    STA(byte_32);                            /* STA byte_32 */
    LDAI(0x00);                              /* LDA #0 */
    STA(game_mode);                          /* STA game_mode */
    STA(zanac_main_weapon_level);            /* STA zanac_main_weapon_level */
    STA(byte_49);                            /* STA byte_49 */
    STA(byte_46);                            /* STA byte_46 */
    STA(byte_47);                            /* STA byte_47 */
    STA(byte_48);                            /* STA byte_48 */
    STA(byte_4B);                            /* STA byte_4B */
    STA(byte_5A);                            /* STA byte_5A */
    STA(byte_4D);                            /* STA byte_4D */
    STA(byte_4A);                            /* STA byte_4A */
    STA(byte_56);                            /* STA byte_56 */
    STA(byte_54);                            /* STA byte_54 */
    STA(byte_176);                           /* STA byte_176 */
    STA(byte_177);                           /* STA byte_177 */
    STA(byte_178);                           /* STA byte_178 */
    STA(byte_39);                            /* STA byte_39 */
    STA(byte_55);                            /* STA byte_55 */
    STA(byte_C0);                            /* STA byte_C0 */
    STA(byte_5D);                            /* STA byte_5D */
    LDAI(0x10);                              /* LDA #$10 */
    STA(byte_77);                            /* STA byte_77 */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_45);                            /* STA byte_45 */
    STA(byte_179);                           /* STA byte_179 */
    LDAI(7);                                 /* LDA #7 */
    STA(byte_62);                            /* STA byte_62 */
    LDAI(0x0F);                              /* LDA #$F */
    STA(byte_51);                            /* STA byte_51 */
    sub_18012();                             /* JSR sub_18012 */
    j_set_default_weapon();                  /* JSR j_set_default_weapon */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ANDI(0x10);                              /* AND #$10 (check START pressed) */
    if (!F_Z) {                              /* BNE continue_level_check */
        continue_level_check();              /* branch out of this procedure */
        RTS();
    }
    LDAI(0x80);                              /* LDA #$80 */
    STA(game_mode);                          /* STA game_mode */
    randomize();                             /* JSR randomize */
    ANDI(7);                                 /* AND #7 */
    CLC();                                   /* CLC */
    ADCI(1);                                 /* ADC #1 */
    STA(current_level);                      /* STA current_level */
    LDAI(0x14);                              /* LDA #$14 */
    STA(demo_counter_2);                     /* STA demo_counter_2 */
    LDAI(1);                                 /* LDA #1 */
    STA(demo_button_update_counter);         /* STA demo_button_update_counter */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_51);                            /* STA byte_51 */
    randomize();                             /* JSR randomize */
    ANDI(0x1F);                              /* AND #$1F */
    STA(demo_button_tab_pointer);            /* STA demo_button_tab_pointer */
    INC(byte_CD);                            /* INC byte_CD */
    LDA(byte_CD);                            /* LDA byte_CD */
    ANDI(0x0F);                              /* AND #$F */
    STA(byte_CD);                            /* STA byte_CD */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_CC16));                     /* LDA unk_CC16,Y */
    STA(byte_18);                            /* STA byte_18 */
    LDA(ABSY(unk_CC26));                     /* LDA unk_CC26,Y */
    STA(byte_19);                            /* STA byte_19 */

loc_CC0A:                                    /* $CC0A */
    BEQ(locret_CC15);                        /* BEQ locret_CC15 (flags from LDA unk_CC26,Y) */
    LDA(byte_18);                            /* LDA byte_18 */
    sub_1800F();                             /* JSR sub_1800F */
    DEC(byte_19);                            /* DEC byte_19 */
    BNE(loc_CC0A);                           /* BNE loc_CC0A */

locret_CC15:                                 /* $CC15 */
    RTS();
}
