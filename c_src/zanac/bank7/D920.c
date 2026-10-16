/* bank7/D920.c - bank 7, $D920-$DFBF: the level object-script helpers that
 * sub_D84F dispatches to (sub_D920/sub_D938/sub_D93A/sub_D943/sub_D9E0/
 * sub_DA16/sub_DA28/sub_DA36), the status-bar and level-intro routines
 * (sub_DCC5..sub_DEF3), reset_score and the main menu (show_main_menu).
 *
 * Ported from src/bank7.asm lines 4727-6347 (addresses from Build/bank7.lst).
 *
 * Translation notes for this chunk:
 *   - show_main_menu calls print_string twice with the parameters in the
 *     instruction stream that follows the JSR; the two call sites pass the
 *     block address ($E15A and $E16A) through PRINT_STRING() (PORTING.md
 *     section 4) and keep the parameter bytes as comments.
 *   - sub_D943 and sub_D9E0 end with a conditional branch into the procedure
 *     sub_DA28; those are ported as a call inside an `if (F_...)` (the same
 *     idiom bank6/B44B.c uses for `BCS sub_1B592`).
 *   - the data tables between the procedures ($DBC5, $D7DB/$D7DF, $DF9F and
 *     $E202/$E20F/$E21C) have a label in rom_labels.h; the three tables
 *     sub_DD26 indexes ($DCDC, $DCF4, $DD0C) do not, so those are read at
 *     their literal addresses.  All of them live in the extracted bank image,
 *     which is what MEM8() reads.
 *   - the $8000-window reads ($8056-$805B) follow a JSR switch_bank_5: the
 *     banking itself is transparent in the port (PORTING.md section 3), only
 *     the calls and the callback into sub_C8DF stay where the original has
 *     them.
 */
#include <stdio.h>
#include <stdlib.h>
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $D920  sub_D920 - start a level: remember it in byte_92 and mark $10
 *
 *  Original: src/bank7.asm:4727-4742
 *      LDY byte_16 / LDA game_mode / AND #$20 / BNE loc_D934 /
 *      LDA current_level / STA byte_92 / INC byte_92 / LDA game_mode /
 *      ORA #$10 / STA game_mode / loc_D934: DEY / JMP sub_DA28
 * ------------------------------------------------------------------------ */
void sub_D920(void)                          /* $D920 */
{
    LDY(byte_16);                            /* LDY byte_16 */
    LDA(game_mode);                          /* LDA game_mode (0x80 = demo mode) */
    ANDI(0x20);                              /* AND #$20 */
    BNE(loc_D934);                           /* BNE loc_D934 */
    LDA(current_level);                      /* LDA current_level */
    STA(byte_92);                            /* STA byte_92 */
    INC(byte_92);                            /* INC byte_92 */
    LDA(game_mode);                          /* LDA game_mode */
    ORAI(0x10);                              /* ORA #$10 */
    STA(game_mode);                          /* STA game_mode */

loc_D934:                                    /* $D934 */
    DEY();                                   /* DEY */
    sub_DA28();                              /* JMP sub_DA28 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D938  sub_D938 - script entry: Y = byte_16, then execute the opcode
 *
 *  Original: src/bank7.asm:4748-4750 (no RTS: continues into sub_D93A)
 *      LDY byte_16
 * ------------------------------------------------------------------------ */
void sub_D938(void)                          /* $D938 */
{
    LDY(byte_16);                            /* LDY byte_16 */
    sub_D93A();                              /* falls through into $D93A */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D93A  sub_D93A - read one script byte; zero ends the script
 *
 *  Original: src/bank7.asm:4756-4764 (no RTS: continues into sub_D943)
 *      LDA ($B5),Y / BNE loc_D941 / JMP sub_DA28 / loc_D941: STA byte_14
 * ------------------------------------------------------------------------ */
void sub_D93A(void)                          /* $D93A */
{
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    BNE(loc_D941);                           /* BNE loc_D941 */
    sub_DA28();                              /* JMP sub_DA28 */
    RTS();

loc_D941:                                    /* $D941 */
    STA(byte_14);                            /* STA byte_14 */
    sub_D943();                              /* falls through into $D943 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D943  sub_D943 - decode one object record of the level script
 *
 *  Original: src/bank7.asm:4770-4866.  byte_14 is the object count, ($B5),Y
 *  the script pointer; the record's flag byte byte_15 selects the long or the
 *  short form and the routine loops on itself until the count is exhausted.
 *      INY / LDA ($B5),Y / STA byte_15 / INY / AND #7 / TAX / BIT byte_15 /
 *      BPL loc_D957 / LDA #0 / STA $430,X / BEQ loc_D966 /
 *      loc_D957: ... / loc_D9D6: JSR sub_DCCB / DEC byte_14 /
 *      BEQ sub_DA28 / JMP sub_D943
 * ------------------------------------------------------------------------ */
void sub_D943(void)                          /* $D943 */
{
loc_D943:                                    /* $D943 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_15);                            /* STA byte_15 */
    INY();                                   /* INY */
    ANDI(7);                                 /* AND #7 */
    TAX();                                   /* TAX */
    BIT(byte_15);                            /* BIT byte_15 */
    BPL(loc_D957);                           /* BPL loc_D957 */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x0430));                       /* STA $430,X */
    BEQ(loc_D966);                           /* BEQ loc_D966 */

loc_D957:                                    /* $D957 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    INY();                                   /* INY */
    STA(ABSX(0x0430));                       /* STA $430,X */
    LDA(byte_15);                            /* LDA byte_15 */
    ANDI(0x20);                              /* AND #$20 */
    BNE(loc_D966);                           /* BNE loc_D966 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    INY();                                   /* INY */

loc_D966:                                    /* $D966 */
    STA(ABSX(0x0408));                       /* STA $408,X */
    BIT(byte_15);                            /* BIT byte_15 */
    BVC(loc_D971);                           /* BVC loc_D971 */
    LDAI(0x00);                              /* LDA #0 */
    BEQ(loc_D974);                           /* BEQ loc_D974 */

loc_D971:                                    /* $D971 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    INY();                                   /* INY */

loc_D974:                                    /* $D974 */
    STA(ABSX(0x0438));                       /* STA $438,X */
    LDA(byte_15);                            /* LDA byte_15 */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_D981);                           /* BEQ loc_D981 */
    LDAI(1);                                 /* LDA #1 */
    BNE(loc_D984);                           /* BNE loc_D984 */

loc_D981:                                    /* $D981 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    INY();                                   /* INY */

loc_D984:                                    /* $D984 */
    STA(ABSX(0x0410));                       /* STA $410,X */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(ABSX(0x0400));                       /* STA $400,X */
    INY();                                   /* INY */
    LDA(byte_15);                            /* LDA byte_15 */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_D9AD);                           /* BEQ loc_D9AD */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(ABSX(0x0440));                       /* STA $440,X */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(ABSX(0x0448));                       /* STA $448,X */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(ABSX(0x0418));                       /* STA $418,X */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(ABSX(0x0420));                       /* STA $420,X */
    goto loc_D9D6;                           /* JMP loc_D9D6 */

loc_D9AD:                                    /* $D9AD */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_18);                            /* STA byte_18 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_19);                            /* STA byte_19 */
    STY(byte_16);                            /* STY byte_16 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0418));                       /* STA $418,X */
    INY();                                   /* INY */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0420));                       /* STA $420,X */
    LDA(byte_18);                            /* LDA byte_18 */
    CLC();                                   /* CLC */
    ADCI(2);                                 /* ADC #2 */
    STA(ABSX(0x0440));                       /* STA $440,X */
    LDA(byte_19);                            /* LDA byte_19 */
    ADCI(0x00);                              /* ADC #0 */
    STA(ABSX(0x0448));                       /* STA $448,X */
    LDY(byte_16);                            /* LDY byte_16 */

loc_D9D6:                                    /* $D9D6 */
    sub_DCCB();                              /* JSR sub_DCCB */
    DEC(byte_14);                            /* DEC byte_14 */
    if (F_Z) {                               /* BEQ sub_DA28 (branch to the procedure) */
        sub_DA28();                          /* branch into sub_DA28 */
        RTS();
    }
    goto loc_D943;                           /* JMP sub_D943 */
}

/* ------------------------------------------------------------------------ *
 *  $D9E0  sub_D9E0 - script opcode $Cx: set up a scripted object slot
 *
 *  Original: src/bank7.asm:4872-4905 (no RTS: continues into sub_DA16)
 *      LDY byte_16 / LDA #$3C / STA byte_C4 / LDA ($B5),Y / STA byte_C5 /
 *      BEQ loc_DA0E / ... / JMP sub_D93A /
 *      loc_DA0E: LDA byte_C0 / EOR #$22 / STA byte_C0 / BNE sub_DA28
 * ------------------------------------------------------------------------ */
void sub_D9E0(void)                          /* $D9E0 */
{
    LDY(byte_16);                            /* LDY byte_16 */
    LDAI(0x3C);                              /* LDA #$3C */
    STA(byte_C4);                            /* STA byte_C4 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_C5);                            /* STA byte_C5 */
    BEQ(loc_DA0E);                           /* BEQ loc_DA0E */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_C6);                            /* STA byte_C6 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_C8);                            /* STA byte_C8 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_C7);                            /* STA byte_C7 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_C3);                            /* STA byte_C3 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_C0);                            /* STA byte_C0 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_C1);                            /* STA byte_C1 */
    STA(byte_C2);                            /* STA byte_C2 */
    INY();                                   /* INY */
    sub_D93A();                              /* JMP sub_D93A */
    RTS();

loc_DA0E:                                    /* $DA0E */
    LDA(byte_C0);                            /* LDA byte_C0 */
    EORI(0x22);                              /* EOR #$22 */
    STA(byte_C0);                            /* STA byte_C0 */
    if (!F_Z) {                              /* BNE sub_DA28 (branch to the procedure) */
        sub_DA28();                          /* branch into sub_DA28 */
        RTS();
    }
    sub_DA16();                              /* falls through into $DA16 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DA16  sub_DA16 - script opcode: fill the $16E tile table
 *
 *  Original: src/bank7.asm:4911-4924 (no RTS: continues into sub_DA28)
 *      LDY byte_16 / loc_DA18: LDA ($B5),Y / TAX / CMP #8 / BCS sub_DA28 /
 *      INY / LDA ($B5),Y / STA $16E,X / INY / BNE loc_DA18
 * ------------------------------------------------------------------------ */
void sub_DA16(void)                          /* $DA16 */
{
    LDY(byte_16);                            /* LDY byte_16 */

loc_DA18:                                    /* $DA18 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    TAX();                                   /* TAX */
    CMPI(8);                                 /* CMP #8 */
    if (F_C) {                               /* BCS sub_DA28 (branch to the procedure) */
        sub_DA28();                          /* branch into sub_DA28 */
        RTS();
    }
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(ABSX(0x016E));                       /* STA $16E,X */
    INY();                                   /* INY */
    BNE(loc_DA18);                           /* BNE loc_DA18 */
    sub_DA28();                              /* falls through into $DA28 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DA28  sub_DA28 - script end / shared tail: store B3/B4 and go on
 *
 *  Original: src/bank7.asm:4930-4940
 *      INY / LDA ($B5),Y / STA byte_B3 / INY / LDA ($B5),Y / STA byte_B4 /
 *      INY / JMP sub_D84F
 * ------------------------------------------------------------------------ */
void sub_DA28(void)                          /* $DA28 */
{
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_B3);                            /* STA byte_B3 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_B4);                            /* STA byte_B4 */
    INY();                                   /* INY */
    sub_D84F();                              /* JMP sub_D84F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DA36  sub_DA36 - run one step of all scripted objects and refresh the
 *                    name-table buffers
 *
 *  Original: src/bank7.asm:4946-5196 (entered with JMP from sub_D84F).  It
 *  walks the 8 object slots at $400.., reads the metatile string of the level
 *  data ($189 holds the tile numbers, unk_DBC5 the metatile index), writes the
 *  $470/$104 name-table buffers and finally requests a background update.
 *  Ends with JMP switch_bank_6.
 * ------------------------------------------------------------------------ */
void sub_DA36(void)                          /* $DA36 */
{
    LDA(byte_A8);                            /* LDA byte_A8 */
    ASL_A();                                 /* ASL */
    ADC(byte_A8);                            /* ADC byte_A8 */
    TAY();                                   /* TAY */
    LDA(byte_B8);                            /* LDA byte_B8 */
    STA(byte_10);                            /* STA byte_10 */
    LDA(byte_B9);                            /* LDA byte_B9 */
    STA(byte_11);                            /* STA byte_11 */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    STA(byte_18);                            /* STA byte_18 */
    INY();                                   /* INY */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    STA(byte_19);                            /* STA byte_19 */
    INY();                                   /* INY */
    LDA(byte_B2);                            /* LDA byte_B2 */
    CMP(INDY(byte_10));                      /* CMP ($10),Y */
    BCC(loc_DA58);                           /* BCC loc_DA58 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_B2);                            /* STA byte_B2 */

loc_DA58:                                    /* $DA58 */
    INC(byte_B2);                            /* INC byte_B2 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDXI(0);                                 /* LDX #0 */

loc_DA61:                                    /* $DA61 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0189));                       /* STA $189,X */
    INY();                                   /* INY */
    INX();                                   /* INX */
    CPXI(0x10);                              /* CPX #$10 */
    BNE(loc_DA61);                           /* BNE loc_DA61 */
    LDXI(0);                                 /* LDX #0 */
    STX(byte_15);                            /* STX byte_15 */

loc_DA70:                                    /* $DA70 */
    LDA(ABSX(0x0400));                       /* LDA $400,X */
    CMPI(0x80);                              /* CMP #$80 */
    BNE(loc_DA7A);                           /* BNE loc_DA7A */
    goto loc_DAFB;                           /* JMP loc_DAFB */

loc_DA7A:                                    /* $DA7A */
    LDA(ABSX(0x0430));                       /* LDA $430,X */
    BEQ(loc_DA85);                           /* BEQ loc_DA85 */
    DEC(ABSX(0x0430));                       /* DEC $430,X */
    goto loc_DAFB;                           /* JMP loc_DAFB */

loc_DA85:                                    /* $DA85 */
    LDA(ABSX(0x0450));                       /* LDA $450,X */
    STA(byte_18);                            /* STA byte_18 */
    LDA(ABSX(0x0458));                       /* LDA $458,X */
    STA(byte_19);                            /* STA byte_19 */
    LDYI(0);                                 /* LDY #0 */
    LDA(ABSX(0x0420));                       /* LDA $420,X */
    STA(byte_14);                            /* STA byte_14 */
    LDA(ABSX(0x0400));                       /* LDA $400,X */
    TAX();                                   /* TAX */

loc_DA9A:                                    /* $DA9A */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    BNE(loc_DAA8);                           /* BNE loc_DAA8 */
    INY();                                   /* INY */
    TXA();                                   /* TXA */
    CLC();                                   /* CLC */
    ADC(INDY(byte_18));                      /* ADC ($18),Y */
    TAX();                                   /* TAX */
    INY();                                   /* INY */
    goto loc_DA9A;                           /* JMP loc_DA9A */

loc_DAA8:                                    /* $DAA8 */
    INY();                                   /* INY */
    CMPI(1);                                 /* CMP #1 */
    BEQ(loc_DAC0);                           /* BEQ loc_DAC0 */
    CPXI(0x10);                              /* CPX #$10 */
    BCS(loc_DABB);                           /* BCS loc_DABB */
    STA(ABSX(0x0189));                       /* STA $189,X */
    CMPI(0x1B);                              /* CMP #$1B */
    BCS(loc_DABB);                           /* BCS loc_DABB */
    sub_DD26();                              /* JSR sub_DD26 */

loc_DABB:                                    /* $DABB */
    INX();                                   /* INX */
    DEC(byte_14);                            /* DEC byte_14 */
    BNE(loc_DA9A);                           /* BNE loc_DA9A */

loc_DAC0:                                    /* $DAC0 */
    LDX(byte_15);                            /* LDX byte_15 */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADC(byte_18);                            /* ADC byte_18 */
    STA(ABSX(0x0450));                       /* STA $450,X */
    LDA(byte_19);                            /* LDA byte_19 */
    ADCI(0x00);                              /* ADC #0 */
    STA(ABSX(0x0458));                       /* STA $458,X */
    DEC(ABSX(0x0428));                       /* DEC $428,X */
    BNE(loc_DAFB);                           /* BNE loc_DAFB */
    LDA(ABSX(0x0410));                       /* LDA $410,X */
    BEQ(loc_DAE6);                           /* BEQ loc_DAE6 */
    DEC(ABSX(0x0410));                       /* DEC $410,X */
    BNE(loc_DAE6);                           /* BNE loc_DAE6 */

loc_DADF:                                    /* $DADF */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(0x0400));                       /* STA $400,X */
    BNE(loc_DAFB);                           /* BNE loc_DAFB (always taken) */

loc_DAE6:                                    /* $DAE6 */
    LDA(ABSX(0x0400));                       /* LDA $400,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0438));                       /* ADC $438,X */
    STA(ABSX(0x0400));                       /* STA $400,X */
    CMPI(0x20);                              /* CMP #$20 */
    BCC(loc_DAF8);                           /* BCC loc_DAF8 */
    CMPI(0xF0);                              /* CMP #$F0 */
    BCC(loc_DADF);                           /* BCC loc_DADF */

loc_DAF8:                                    /* $DAF8 */
    sub_DCC5();                              /* JSR sub_DCC5 */

loc_DAFB:                                    /* $DAFB */
    INC(byte_15);                            /* INC byte_15 */
    LDX(byte_15);                            /* LDX byte_15 */
    CPXI(8);                                 /* CPX #8 */
    BEQ(loc_DB06);                           /* BEQ loc_DB06 */
    goto loc_DA70;                           /* JMP loc_DA70 */

loc_DB06:                                    /* $DB06 */
    LDXI(0);                                 /* LDX #0 */
    STX(byte_14);                            /* STX byte_14 */

loc_DB0A:                                    /* $DB0A */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_19);                            /* STA byte_19 */
    LDA(ABSX(0x0189));                       /* LDA $189,X */
    STA(byte_10);                            /* STA byte_10 */
    ASL_A();                                 /* ASL */
    ROL(byte_19);                            /* ROL byte_19 */
    ASL_A();                                 /* ASL */
    ROL(byte_19);                            /* ROL byte_19 */
    ADC(byte_10);                            /* ADC byte_10 */
    BCC(loc_DB1F);                           /* BCC loc_DB1F */
    INC(byte_19);                            /* INC byte_19 */

loc_DB1F:                                    /* $DB1F */
    CLC();                                   /* CLC */
    ADC(byte_BA);                            /* ADC byte_BA */
    STA(byte_18);                            /* STA byte_18 */
    LDA(byte_BB);                            /* LDA byte_BB */
    ADC(byte_19);                            /* ADC byte_19 */
    STA(byte_19);                            /* STA byte_19 */
    LDYI(0);                                 /* LDY #0 */
    TXA();                                   /* TXA */
    ASL_A();                                 /* ASL */
    TAX();                                   /* TAX */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0470));                       /* STA $470,X */
    INY();                                   /* INY */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0471));                       /* STA $471,X */
    INY();                                   /* INY */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0490));                       /* STA $490,X */
    INY();                                   /* INY */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0491));                       /* STA $491,X */
    INY();                                   /* INY */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(byte_10);                            /* STA byte_10 */
    LDA(byte_14);                            /* LDA byte_14 */
    ANDI(1);                                 /* AND #1 */
    TAY();                                   /* TAY */
    LDA(byte_A1);                            /* LDA byte_A1 */
    ANDI(1);                                 /* AND #1 */
    BEQ(loc_DB58);                           /* BEQ loc_DB58 */
    INY();                                   /* INY */
    INY();                                   /* INY */

loc_DB58:                                    /* $DB58 */
    LDA(byte_14);                            /* LDA byte_14 */
    LSR_A();                                 /* LSR */
    CLC();                                   /* CLC */
    ADC(byte_9D);                            /* ADC byte_9D */
    TAX();                                   /* TAX */
    LDA(ABSX(0x0104));                       /* LDA $104,X */
    AND(ABSY(byte_D7DB));                    /* AND byte_D7DB,Y */
    STA(byte_11);                            /* STA byte_11 */
    LDA(ABSY(byte_D7DF));                    /* LDA byte_D7DF,Y */
    LDY(byte_10);                            /* LDY byte_10 */
    AND(ABSY(0x016E));                       /* AND $16E,Y */
    ORA(byte_11);                            /* ORA byte_11 */
    STA(ABSX(0x0104));                       /* STA $104,X */
    if (getenv("ZANAC_ATTR")) fprintf(stderr,"port X=%02X val=%02X A1=%02X 14=%02X 9D=%02X maskY=%02X\n",
        X, A, MEM8(0xA1), MEM8(0x14), MEM8(0x9D), (MEM8(0x14)&1)+2*(MEM8(0xA1)&1));
    INC(byte_14);                            /* INC byte_14 */
    LDX(byte_14);                            /* LDX byte_14 */
    CPXI(0x10);                              /* CPX #$10 */
    BNE(loc_DB0A);                           /* BNE loc_DB0A */
    LDA(byte_A1);                            /* LDA byte_A1 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    STA(byte_18);                            /* STA byte_18 */
    LDYI(0);                                 /* LDY #0 */

loc_DB85:                                    /* $DB85 */
    LDX(ABSY(0x0189));                       /* LDX $189,Y */
    LDA(ABSX(unk_DBC5));                     /* LDA unk_DBC5,X */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    STA(byte_10);                            /* STA byte_10 */
    INY();                                   /* INY */
    LDX(ABSY(0x0189));                       /* LDX $189,Y */
    LDA(ABSX(unk_DBC5));                     /* LDA unk_DBC5,X */
    ORA(byte_10);                            /* ORA byte_10 */
    STY(byte_19);                            /* STY byte_19 */
    LDY(byte_18);                            /* LDY byte_18 */
    STA(ABSY(0x04B0));                       /* STA $4B0,Y */
    LDY(byte_19);                            /* LDY byte_19 */
    INC(byte_18);                            /* INC byte_18 */
    INY();                                   /* INY */
    CPYI(0x10);                              /* CPY #$10 */
    BNE(loc_DB85);                           /* BNE loc_DB85 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_9C);                            /* STA byte_9C */
    DEC(byte_A1);                            /* DEC byte_A1 */
    BPL(loc_DBBC);                           /* BPL loc_DBBC */
    LDAI(0x0E);                              /* LDA #$E */
    STA(byte_A1);                            /* STA byte_A1 */
    LDA(byte_B7);                            /* LDA byte_B7 */
    EORI(1);                                 /* EOR #1 */
    STA(byte_B7);                            /* STA byte_B7 */

loc_DBBC:                                    /* $DBBC */
    LDA(byte_90);                            /* LDA byte_90 */
    ORAI(1);                                 /* ORA #1 */
    STA(byte_90);                            /* STA byte_90 */
    switch_bank_6();                         /* JMP switch_bank_6 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DCC5  sub_DCC5 - reload the reload counter of an object
 *
 *  Original: src/bank7.asm:5459-5462 (no RTS: continues into sub_DCCB)
 *      LDA $408,X / STA $430,X
 * ------------------------------------------------------------------------ */
void sub_DCC5(void)                          /* $DCC5 */
{
    LDA(ABSX(0x0408));                       /* LDA $408,X */
    STA(ABSX(0x0430));                       /* STA $430,X */
    sub_DCCB();                              /* falls through into $DCCB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DCCB  sub_DCCB - restart an object from its saved coordinates
 *
 *  Original: src/bank7.asm:5468-5476
 *      LDA $440,X / STA $450,X / LDA $448,X / STA $458,X /
 *      LDA $418,X / STA $428,X / RTS
 * ------------------------------------------------------------------------ */
void sub_DCCB(void)                          /* $DCCB */
{
    LDA(ABSX(0x0440));                       /* LDA $440,X */
    STA(ABSX(0x0450));                       /* STA $450,X */
    LDA(ABSX(0x0448));                       /* LDA $448,X */
    STA(ABSX(0x0458));                       /* STA $458,X */
    LDA(ABSX(0x0418));                       /* LDA $418,X */
    STA(ABSX(0x0428));                       /* STA $428,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $DD26  sub_DD26 - spawn the object for tile A
 *
 *  Original: src/bank7.asm:5555-5632.  The routine indexes three 24-byte tables
 *  with Y; the instructions address them at $DCDC, $DCF4 and $DD0C.  There is
 *  no label for them in rom_labels.h (the source's .BYTE data is emitted at
 *  $DCDE-$DD25), so the literal addresses of the instructions are kept.
 * ------------------------------------------------------------------------ */
void sub_DD26(void)                          /* $DD26 */
{
    STA(byte_1A);                            /* STA byte_1A */
    STY(byte_17);                            /* STY byte_17 */
    TAY();                                   /* TAY */
    LDA(ABSY(0xDCDC));                       /* LDA $DCDC,Y (unnamed table) */
    PHA();                                   /* PHA */
    STX(byte_16);                            /* STX byte_16 */
    sub_DD9E();                              /* JSR sub_DD9E */
    TYA();                                   /* TYA */
    TAX();                                   /* TAX */
    PLA();                                   /* PLA */
    BCC(loc_DD46);                           /* BCC loc_DD46 */
    CMPI(0x47);                              /* CMP #$47 */
    BEQ(loc_DD41);                           /* BEQ loc_DD41 */
    CMPI(0x52);                              /* CMP #$52 */
    BNE(loc_DD43);                           /* BNE loc_DD43 */

loc_DD41:                                    /* $DD41 */
    INC(byte_AF);                            /* INC byte_AF */

loc_DD43:                                    /* $DD43 */
    goto loc_DD99;                           /* JMP loc_DD99 */

loc_DD46:                                    /* $DD46 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    CMPI(0x47);                              /* CMP #$47 */
    BEQ(loc_DD51);                           /* BEQ loc_DD51 */
    CMPI(0x52);                              /* CMP #$52 */
    BNE(loc_DD5B);                           /* BNE loc_DD5B */

loc_DD51:                                    /* $DD51 */
    LDY(byte_AF);                            /* LDY byte_AF */
    LDA(INDY(byte_B0));                      /* LDA ($B0),Y */
    STA(ABSX(byte_716));                     /* STA $716,X */
    INY();                                   /* INY */
    STY(byte_AF);                            /* STY byte_AF */

loc_DD5B:                                    /* $DD5B */
    LDA(byte_A1);                            /* LDA byte_A1 */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    LDY(byte_1A);                            /* LDY byte_1A */
    LDA(byte_16);                            /* LDA byte_16 */
    ASL_A();                                 /* ASL */
    PHA();                                   /* PHA */
    CLC();                                   /* CLC */
    ADC(byte_9F);                            /* ADC byte_9F */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDA(byte_A0);                            /* LDA byte_A0 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0xE0);                              /* LDA #$E0 */
    CLC();                                   /* CLC */
    ADC(ABSY(0xDCF4));                       /* ADC $DCF4,Y (unnamed table) */
    CLC();                                   /* CLC */
    ADC(byte_A4);                            /* ADC byte_A4 */
    SEC();                                   /* SEC */
    SBC(byte_AE);                            /* SBC byte_AE */
    STA(ABSX(byte_542));                     /* STA $542,X */
    PLA();                                   /* PLA */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    CLC();                                   /* CLC */
    ADC(ABSY(0xDD0C));                       /* ADC $DD0C,Y (unnamed table) */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDA(byte_16);                            /* LDA byte_16 */
    LSR_A();                                 /* LSR */
    CLC();                                   /* CLC */
    ADC(byte_9D);                            /* ADC byte_9D */
    STA(ABSX(byte_646));                     /* STA $646,X */
    LDA(byte_1A);                            /* LDA byte_1A */
    STA(ABSX(0x074A));                       /* STA $74A,X */

loc_DD99:                                    /* $DD99 */
    LDY(byte_17);                            /* LDY byte_17 */
    LDX(byte_16);                            /* LDX byte_16 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $DD9E  sub_DD9E - dissolve the tile at X / Y (returns carry = 1)
 *
 *  Original: src/bank7.asm:5638-5703
 *      LDY #$19 / loc_DDA0: LDA $528,Y / BEQ loc_DDF1 / DEY / CPY #4 /
 *      BNE loc_DDA0 / ... / loc_DDE6: SEC / RTS /
 *      loc_DDE8: ... JSR enemy_erase_2 ... / loc_DDF1: CLC / RTS
 * ------------------------------------------------------------------------ */
void sub_DD9E(void)                          /* $DD9E */
{
    LDYI(0x19);                              /* LDY #$19 */

loc_DDA0:                                    /* $DDA0 */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    BEQ(loc_DDF1);                           /* BEQ loc_DDF1 */
    DEY();                                   /* DEY */
    CPYI(4);                                 /* CPY #4 */
    BNE(loc_DDA0);                           /* BNE loc_DDA0 */

loc_DDAA:                                    /* $DDAA */
    INY();                                   /* INY */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    CMPI(0x14);                              /* CMP #$14 */
    BEQ(loc_DDE8);                           /* BEQ loc_DDE8 */
    CMPI(0x15);                              /* CMP #$15 */
    BEQ(loc_DDE8);                           /* BEQ loc_DDE8 */
    CMPI(0x25);                              /* CMP #$25 */
    BEQ(loc_DDE8);                           /* BEQ loc_DDE8 */
    CMPI(0x26);                              /* CMP #$26 */
    BEQ(loc_DDE8);                           /* BEQ loc_DDE8 */
    CMPI(0x3B);                              /* CMP #$3B */
    BEQ(loc_DDE8);                           /* BEQ loc_DDE8 */
    CMPI(0x29);                              /* CMP #$29 */
    BEQ(loc_DDE8);                           /* BEQ loc_DDE8 */
    CPYI(0x19);                              /* CPY #$19 */
    BNE(loc_DDAA);                           /* BNE loc_DDAA */

loc_DDCC:                                    /* $DDCC */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    CMPI(0x46);                              /* CMP #$46 */
    BCS(loc_DDE1);                           /* BCS loc_DDE1 */
    CMPI(0x38);                              /* CMP #$38 */
    BCS(loc_DDE8);                           /* BCS loc_DDE8 */
    CMPI(0x35);                              /* CMP #$35 */
    BCS(loc_DDE6);                           /* BCS loc_DDE6 */
    CMPI(0x27);                              /* CMP #$27 */
    BNE(loc_DDE8);                           /* BNE loc_DDE8 */

loc_DDE1:                                    /* $DDE1 */
    DEY();                                   /* DEY */
    CPYI(4);                                 /* CPY #4 */
    BNE(loc_DDCC);                           /* BNE loc_DDCC */

loc_DDE6:                                    /* $DDE6 */
    SEC();                                   /* SEC */
    RTS();                                   /* RTS */

loc_DDE8:                                    /* $DDE8 */
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */
    TYA();                                   /* TYA */
    TAX();                                   /* TAX */
    enemy_erase_2();                         /* JSR enemy_erase_2 */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */

loc_DDF1:                                    /* $DDF1 */
    CLC();                                   /* CLC */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $DDF3  sub_DDF3 - flush the pending PPU update and enable everything
 *
 *  Original: src/bank7.asm:5709-5713
 *      JSR sub_DDF9 / JMP ppu_enable_all
 * ------------------------------------------------------------------------ */
void sub_DDF3(void)                          /* $DDF3 */
{
    sub_DDF9();                              /* JSR sub_DDF9 */
    ppu_enable_all();                        /* JMP ppu_enable_all */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DDF9  sub_DDF9 - wait for the pending palette upload to be consumed
 *
 *  Original: src/bank7.asm:5719-5735
 *      LDA cram_update_mode / PHA / LDA #0 / STA cram_update_mode /
 *      JSR ppu_nmi_enable / LDY #1 / JSR all_enemies_destroyed / PLA /
 *      STA cram_update_mode / loc_DE0E: LDA cram_update_mode /
 *      BNE loc_DE0E / RTS
 * ------------------------------------------------------------------------ */
void sub_DDF9(void)                          /* $DDF9 */
{
    LDA(cram_update_mode);                   /* LDA cram_update_mode */
    PHA();                                   /* PHA */
    LDAI(0x00);                              /* LDA #0 */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    ppu_nmi_enable();                        /* JSR ppu_nmi_enable */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    PLA();                                   /* PLA */
    STA(cram_update_mode);                   /* STA cram_update_mode */

loc_DE0E:                                    /* $DE0E */
    LDA(cram_update_mode);                   /* LDA cram_update_mode */
    BNE(loc_DE0E_wait);                      /* BNE loc_DE0E */
    RTS();                                   /* RTS */

loc_DE0E_wait:                               /* the NMI clears cram_update_mode
                                              * (ppu_cram_update), so the spin
                                              * must let it run */
    ZANAC_WAIT_NMI();
    goto loc_DE0E;
}

/* ------------------------------------------------------------------------ *
 *  $DE14  sub_DE14 - load the level's metatile and attribute data from bank 5
 *
 *  Original: src/bank7.asm:5741-5784
 *      JSR switch_bank_5 / LDA $8056 / STA byte_10 / LDA $8057 / STA byte_11 /
 *      LDY #0 / loc_DE23: ... CPY #240 / BNE loc_DE23 / LDA $8058 /
 *      STA byte_10 / LDA $8059 / STA byte_11 / LDY #0 /
 *      loc_DE52: LDA ($10),Y / STA $104,Y / INY / CPY #64 / BNE loc_DE52 /
 *      JMP switch_bank_6
 * ------------------------------------------------------------------------ */
void sub_DE14(void)                          /* $DE14 */
{
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDA(0x8056);                             /* LDA $8056 (bank 5) */
    STA(byte_10);                            /* STA byte_10 */
    LDA(0x8057);                             /* LDA $8057 (bank 5) */
    STA(byte_11);                            /* STA byte_11 */
    LDYI(0);                                 /* LDY #0 */

loc_DE23:                                    /* $DE23 */
    TYA();                                   /* TYA */
    LSR_A();                                 /* LSR */
    STA(byte_13);                            /* STA byte_13 */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    INY();                                   /* INY */
    TAX();                                   /* TAX */
    LDA(ABSX(unk_DBC5));                     /* LDA unk_DBC5,X */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    STA(byte_12);                            /* STA byte_12 */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    INY();                                   /* INY */
    TAX();                                   /* TAX */
    LDA(ABSX(unk_DBC5));                     /* LDA unk_DBC5,X */
    ORA(byte_12);                            /* ORA byte_12 */
    LDX(byte_13);                            /* LDX byte_13 */
    STA(ABSX(0x04B0));                       /* STA $4B0,X */
    CPYI(240);                               /* CPY #240 */
    BNE(loc_DE23);                           /* BNE loc_DE23 */
    LDA(0x8058);                             /* LDA $8058 (bank 5) */
    STA(byte_10);                            /* STA byte_10 */
    LDA(0x8059);                             /* LDA $8059 (bank 5) */
    STA(byte_11);                            /* STA byte_11 */
    LDYI(0);                                 /* LDY #0 */

loc_DE52:                                    /* $DE52 */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    STA(ABSY(0x0104));                       /* STA $104,Y */
    INY();                                   /* INY */
    CPYI(64);                                /* CPY #64 */
    BNE(loc_DE52);                           /* BNE loc_DE52 */
    switch_bank_6();                         /* JMP switch_bank_6 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DE5F  sub_DE5F - the same metatile load for the other buffer
 *
 *  Original: src/bank7.asm:5790-5820 (no RTS: continues into sub_DE91)
 *      JSR switch_bank_5 / LDA $805A / STA byte_10 / LDA $805B / STA byte_11 /
 *      LDY #0 / loc_DE6E: ... CPY #$70 / BNE loc_DE6E
 * ------------------------------------------------------------------------ */
void sub_DE5F(void)                          /* $DE5F */
{
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDA(0x805A);                             /* LDA $805A (bank 5) */
    STA(byte_10);                            /* STA byte_10 */
    LDA(0x805B);                             /* LDA $805B (bank 5) */
    STA(byte_11);                            /* STA byte_11 */
    LDYI(0);                                 /* LDY #0 */

loc_DE6E:                                    /* $DE6E */
    TYA();                                   /* TYA */
    LSR_A();                                 /* LSR */
    STA(byte_13);                            /* STA byte_13 */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    INY();                                   /* INY */
    TAX();                                   /* TAX */
    LDA(ABSX(unk_DBC5));                     /* LDA unk_DBC5,X */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    STA(byte_12);                            /* STA byte_12 */
    LDA(INDY(byte_10));                      /* LDA ($10),Y */
    INY();                                   /* INY */
    TAX();                                   /* TAX */
    LDA(ABSX(unk_DBC5));                     /* LDA unk_DBC5,X */
    ORA(byte_12);                            /* ORA byte_12 */
    LDX(byte_13);                            /* LDX byte_13 */
    STA(ABSX(0x04B0));                       /* STA $4B0,X */
    CPYI(0x70);                              /* CPY #$70 */
    BNE(loc_DE6E);                           /* BNE loc_DE6E */
    sub_DE91();                              /* falls through into $DE91 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DE91  sub_DE91 - build the "score" row of the status bar
 *
 *  Original: src/bank7.asm:5826-5878
 *      JSR switch_bank_5 / LDA #$23 / JSR sub_C7EB / JSR sub_C7F8 / TAX /
 *      STX byte_15 / LDA #$20 / STA $460,X / ... loc_DECC: LDA ($18),Y /
 *      JSR sub_DF48 / ... INC byte_70 / LDA byte_70 / TAY / AND #$F /
 *      BNE loc_DECC / JSR sub_C807 / JMP switch_bank_6
 * ------------------------------------------------------------------------ */
void sub_DE91(void)                          /* $DE91 */
{
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDAI(0x23);                              /* LDA #$23 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAX();                                   /* TAX */
    STX(byte_15);                            /* STX byte_15 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDAI(0x10);                              /* LDA #$10 */
    STA(byte_19);                            /* STA byte_19 */
    LDA(byte_6F);                            /* LDA byte_6F */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    STA(byte_70);                            /* STA byte_70 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ROL(byte_19);                            /* ROL byte_19 */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDA(byte_19);                            /* LDA byte_19 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(0x805A);                             /* LDA $805A (bank 5) */
    STA(byte_18);                            /* STA byte_18 */
    LDA(0x805B);                             /* LDA $805B (bank 5) */
    STA(byte_19);                            /* STA byte_19 */
    LDA(byte_70);                            /* LDA byte_70 */
    ANDI(0xF0);                              /* AND #$F0 */
    STA(byte_70);                            /* STA byte_70 */
    TAY();                                   /* TAY */

loc_DECC:                                    /* $DECC */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    sub_DF48();                              /* JSR sub_DF48 */
    LDA(byte_6F);                            /* LDA byte_6F */
    ANDI(1);                                 /* AND #1 */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(INDY(byte_14));                      /* LDA ($14),Y */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INY();                                   /* INY */
    INX();                                   /* INX */
    LDA(INDY(byte_14));                      /* LDA ($14),Y */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    INC(byte_70);                            /* INC byte_70 */
    LDA(byte_70);                            /* LDA byte_70 */
    TAY();                                   /* TAY */
    ANDI(0x0F);                              /* AND #$F */
    BNE(loc_DECC);                           /* BNE loc_DECC */
    sub_C807();                              /* JSR sub_C807 */
    switch_bank_6();                         /* JMP switch_bank_6 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DEF3  sub_DEF3 - build the "lives / level" column of the status bar
 *
 *  Original: src/bank7.asm:5884-5928
 *      JSR switch_bank_5 / LDA #$21 / JSR sub_C7EB / JSR sub_C7F8 / TAX /
 *      LDA #$9E / STA $460,X / ... loc_DF1E: LDA ($18),Y / JSR sub_DF48 /
 *      ... CMP #$F0 / BCC loc_DF1E / JSR sub_C807 / JMP switch_bank_6
 * ------------------------------------------------------------------------ */
void sub_DEF3(void)                          /* $DEF3 */
{
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDAI(0x21);                              /* LDA #$21 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAX();                                   /* TAX */
    LDAI(0x9E);                              /* LDA #$9E */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(byte_6F);                            /* LDA byte_6F */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDAI(0x20);                              /* LDA #$20 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(byte_6F);                            /* LDA byte_6F */
    LSR_A();                                 /* LSR */
    STA(byte_70);                            /* STA byte_70 */
    TAY();                                   /* TAY */
    LDA(0x8056);                             /* LDA $8056 (bank 5) */
    STA(byte_18);                            /* STA byte_18 */
    LDA(0x8057);                             /* LDA $8057 (bank 5) */
    STA(byte_19);                            /* STA byte_19 */

loc_DF1E:                                    /* $DF1E */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    sub_DF48();                              /* JSR sub_DF48 */
    LDA(byte_6F);                            /* LDA byte_6F */
    ANDI(1);                                 /* AND #1 */
    TAY();                                   /* TAY */
    LDA(INDY(byte_14));                      /* LDA ($14),Y */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    INY();                                   /* INY */
    INY();                                   /* INY */
    LDA(INDY(byte_14));                      /* LDA ($14),Y */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    LDA(byte_70);                            /* LDA byte_70 */
    CLC();                                   /* CLC */
    ADCI(0x10);                              /* ADC #$10 */
    STA(byte_70);                            /* STA byte_70 */
    TAY();                                   /* TAY */
    CMPI(0xF0);                              /* CMP #$F0 */
    BCC(loc_DF1E);                           /* BCC loc_DF1E */
    sub_C807();                              /* JSR sub_C807 */
    switch_bank_6();                         /* JMP switch_bank_6 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $DF48  sub_DF48 - tile number A -> pointer in ($14) to its graphics
 *
 *  Original: src/bank7.asm:5934-5956
 *      STA byte_10 / LDA #0 / STA byte_11 / LDA byte_10 / ASL /
 *      ROL byte_11 / ASL / ROL byte_11 / ADC byte_10 / BCC loc_DF5C /
 *      INC byte_11 / loc_DF5C: CLC / ADC byte_BA / STA byte_14 /
 *      LDA byte_11 / ADC byte_BB / STA byte_15 / RTS
 * ------------------------------------------------------------------------ */
void sub_DF48(void)                          /* $DF48 */
{
    STA(byte_10);                            /* STA byte_10 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_11);                            /* STA byte_11 */
    LDA(byte_10);                            /* LDA byte_10 */
    ASL_A();                                 /* ASL */
    ROL(byte_11);                            /* ROL byte_11 */
    ASL_A();                                 /* ASL */
    ROL(byte_11);                            /* ROL byte_11 */
    ADC(byte_10);                            /* ADC byte_10 */
    BCC(loc_DF5C);                           /* BCC loc_DF5C */
    INC(byte_11);                            /* INC byte_11 */

loc_DF5C:                                    /* $DF5C */
    CLC();                                   /* CLC */
    ADC(byte_BA);                            /* ADC byte_BA */
    STA(byte_14);                            /* STA byte_14 */
    LDA(byte_11);                            /* LDA byte_11 */
    ADC(byte_BB);                            /* ADC byte_BB */
    STA(byte_15);                            /* STA byte_15 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $DF68  reset_score - clear the score on a hard reset
 *
 *  Original: src/bank7.asm:5963-5994.  The "35 53" marker distinguishes a
 *  hard reset (power on) from a soft reset; the 7 bytes copied to $182 come
 *  from unk_DF9F ($DF9F-$DFA5).
 *      LDA hard_reset_marker1 / CMP #$35 / BNE loc_DF7A / ... /
 *      loc_DF7A: LDX #6 / loc_DF7C: LDA unk_DF9F,X / STA $182,X / DEX /
 *      BPL loc_DF7C / ... / RTS
 * ------------------------------------------------------------------------ */
void reset_score(void)                       /* $DF68 */
{
    LDA(hard_reset_marker1);                 /* LDA hard_reset_marker1 */
    CMPI(0x35);                              /* CMP #$35 */
    BNE(loc_DF7A);                           /* BNE loc_DF7A */
    LDA(hard_reset_marker2);                 /* LDA hard_reset_marker2 */
    CMPI(0x53);                              /* CMP #$53 */
    BNE(loc_DF7A);                           /* BNE loc_DF7A */
    INC(reset_counter);                      /* INC reset_counter */
    RTS();                                   /* RTS */

loc_DF7A:                                    /* $DF7A */
    LDXI(6);                                 /* LDX #6 */

loc_DF7C:                                    /* $DF7C */
    LDA(ABSX(unk_DF9F));                     /* LDA unk_DF9F,X */
    STA(ABSX(0x0182));                       /* STA $182,X */
    DEX();                                   /* DEX */
    BPL(loc_DF7C);                           /* BPL loc_DF7C */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_7D);                            /* STA byte_7D */
    LDAI(0x00);                              /* LDA #0 */
    STA(reset_counter);                      /* STA reset_counter */
    STA(byte_199);                           /* STA byte_199 */
    STA(byte_19B);                           /* STA byte_19B */
    LDAI(0x35);                              /* LDA #$35 */
    STA(hard_reset_marker1);                 /* STA hard_reset_marker1 */
    LDAI(0x53);                              /* LDA #$53 */
    STA(hard_reset_marker2);                 /* STA hard_reset_marker2 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $DFBF  show_main_menu - draw the title screen and run the menu loop
 *
 *  Original: src/bank7.asm:6034-6347.  The two JSR print_string calls take
 *  their parameters from the instruction stream right after the JSR (the
 *  address of the first parameter byte, taken from Build/bank7.lst, is passed
 *  to PRINT_STRING; see PORTING.md section 4):
 *      $E157: JSR print_string ; block at $E15A
 *             .byte $A, $22, $2B, "GAME START"   (aGameStart = $E15D)
 *      $E167: JSR print_string ; block at $E16A
 *             .byte 8, $22, $8B, "CONTINUE"      (aContinue = $E16D)
 *  The routine loops on loc_E190 until START is pressed; `JMP loc_E190` at
 *  the end is the loop back edge.
 * ------------------------------------------------------------------------ */
void show_main_menu(void)                    /* $DFBF */
{
    apu_stop();                              /* JSR apu_stop */
    ppu_disable_all();                       /* JSR ppu_disable_all */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_9C);                            /* STA byte_9C */
    STA(byte_20);                            /* STA byte_20 */
    LDAI(0xA6);                              /* LDA #$A6 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(0xDF);                              /* LDA #$DF ($DFA6) */
    STA(byte_19);                            /* STA byte_19 */
    ppu_cram_load();                         /* JSR ppu_cram_load (menu palette) */
    clear_oam_buffer();                      /* JSR clear_oam_buffer */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_19);                            /* STA byte_19 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(0x00);                              /* LDA #0 */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */
    LDAI(0x19);                              /* LDA #$19 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0xF0);                              /* LDA #$F0 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(3);                                 /* LDA #3 */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */
    LDAI(0x12);                              /* LDA #$12 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(2);                                 /* LDA #2 */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */
    sub_C8BA();                              /* JSR sub_C8BA */
    LDAI(0x20);                              /* LDA #$20 */
    LDXI(0x20);                              /* LDX #$20 */
    LDYI(0xAA);                              /* LDY #$AA */
    clear_screen();                          /* JSR clear_screen (clear BG) */
    pad_read();                              /* JSR pad_read */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ORA(pad_buttons_2);                      /* ORA pad_buttons_2 */
    ANDI(0xC0);                              /* AND #$C0 */
    CMPI(0xC0);                              /* CMP #$C0 */
    BNE(loc_E020);                           /* BNE loc_E020 */
    sound_test_menu();                       /* JMP sound_test_menu */
    RTS();

loc_E020:                                    /* $E020 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(0x90);                              /* LDA #$90 */
    STA(byte_16);                            /* STA byte_16 */
    LDAI(0xE2);                              /* LDA #$E2 */
    STA(byte_17);                            /* STA byte_17 */
    sub_E3F4();                              /* JSR sub_E3F4 */
    LDYI(0);                                 /* LDY #0 */
    sub_E399();                              /* JSR sub_E399 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDAI(0x68);                              /* LDA #$68 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDYI(0x7A);                              /* LDY #$7A */
    sub_E266();                              /* JSR sub_E266 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDAI(0x74);                              /* LDA #$74 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDYI(0x82);                              /* LDY #$82 */
    sub_E266();                              /* JSR sub_E266 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0x88);                              /* LDA #$88 */
    STA(byte_18);                            /* STA byte_18 */
    LDA(byte_7D);                            /* LDA byte_7D */
    sub_D347();                              /* JSR sub_D347 */
    sub_C6AB();                              /* JSR sub_C6AB */
    sub_DDF3();                              /* JSR sub_DDF3 */
    sub_18006();                             /* JSR sub_18006 */
    LDAI(3);                                 /* LDA #3 */
    apu_play();                              /* JSR apu_play */
    LDAI(0x0B);                              /* LDA #$B */
    STA(byte_66);                            /* STA byte_66 */
    LDAI(4);                                 /* LDA #4 */
    STA(demo_counter_2);                     /* STA demo_counter_2 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_300);                           /* STA byte_300 */
    STA(byte_6F);                            /* STA byte_6F */

loc_E080:                                    /* $E080 */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    sub_E229();                              /* JSR sub_E229 */
    sub_18009();                             /* JSR sub_18009 */
    sub_18003();                             /* JSR sub_18003 */
    sub_1800C();                             /* JSR sub_1800C */
    pad_read();                              /* JSR pad_read */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ANDI(0x10);                              /* AND #$10 */
    BNE(loc_E0A9);                           /* BNE loc_E0A9 */
    LDA(byte_300);                           /* LDA byte_300 */
    BEQ(loc_E080);                           /* BEQ loc_E080 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_300);                           /* STA byte_300 */
    DEC(demo_counter_2);                     /* DEC demo_counter_2 */
    BNE(loc_E080);                           /* BNE loc_E080 */
    RTS();                                   /* RTS */

loc_E0A9:                                    /* $E0A9 */
    clear_oam_buffer();                      /* JSR clear_oam_buffer */
    LDAI(0x13);                              /* LDA #$13 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAX();                                   /* TAX */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDAI(0x23);                              /* LDA #$23 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDAI(0xE0);                              /* LDA #$E0 */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDYI(0x10);                              /* LDY #$10 */
    LDAI(0xAA);                              /* LDA #$AA */

loc_E0C8:                                    /* $E0C8 */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    DEY();                                   /* DEY */
    BNE(loc_E0C8);                           /* BNE loc_E0C8 */
    sub_C807();                              /* JSR sub_C807 */
    LDAI(0x21);                              /* LDA #$21 */
    STA(byte_6F);                            /* STA byte_6F */
    LDAI(0xA2);                              /* LDA #$A2 */
    STA(byte_70);                            /* STA byte_70 */

loc_E0DA:                                    /* $E0DA */
    LDAI(0x10);                              /* LDA #$10 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAX();                                   /* TAX */
    LDAI(0x8D);                              /* LDA #$8D */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(byte_6F);                            /* LDA byte_6F */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(byte_70);                            /* LDA byte_70 */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDYI(0);                                 /* LDY #0 */

loc_E0F4:                                    /* $E0F4 */
    LDA(byte_70);                            /* LDA byte_70 */
    CMPI(0xBD);                              /* CMP #$BD */
    BCC(loc_E100);                           /* BCC loc_E100 */
    LDA(ABSY(unk_E21C));                     /* LDA unk_E21C,Y */
    goto loc_E10D;                           /* JMP loc_E10D */

loc_E100:                                    /* $E100 */
    CMPI(0xA2);                              /* CMP #$A2 */
    BNE(loc_E10A);                           /* BNE loc_E10A */
    LDA(ABSY(unk_E202));                     /* LDA unk_E202,Y */
    goto loc_E10D;                           /* JMP loc_E10D */

loc_E10A:                                    /* $E10A */
    LDA(ABSY(unk_E20F));                     /* LDA unk_E20F,Y */

loc_E10D:                                    /* $E10D */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    INY();                                   /* INY */
    CPYI(0x0D);                              /* CPY #$D */
    BNE(loc_E0F4);                           /* BNE loc_E0F4 */
    sub_C807();                              /* JSR sub_C807 */
    INC(byte_70);                            /* INC byte_70 */
    LDA(byte_70);                            /* LDA byte_70 */
    CMPI(0xBE);                              /* CMP #$BE */
    BNE(loc_E0DA);                           /* BNE loc_E0DA */
    LDAI(0x21);                              /* LDA #$21 */
    STA(byte_6F);                            /* STA byte_6F */
    LDAI(0xE8);                              /* LDA #$E8 */
    STA(byte_70);                            /* STA byte_70 */

loc_E129:                                    /* $E129 */
    LDAI(0x0B);                              /* LDA #$B */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAX();                                   /* TAX */
    LDAI(0x88);                              /* LDA #$88 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(byte_6F);                            /* LDA byte_6F */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(byte_70);                            /* LDA byte_70 */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDYI(8);                                 /* LDY #8 */
    LDAI(0x20);                              /* LDA #$20 */

loc_E145:                                    /* $E145 */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    DEY();                                   /* DEY */
    BNE(loc_E145);                           /* BNE loc_E145 */
    sub_C807();                              /* JSR sub_C807 */
    INC(byte_70);                            /* INC byte_70 */
    LDA(byte_70);                            /* LDA byte_70 */
    CMPI(0xF7);                              /* CMP #$F7 */
    BNE(loc_E129);                           /* BNE loc_E129 */
    /* $E157: JSR print_string ; inline block at $E15A */
    /*   .byte $A, $22, $2B, "GAME START" (aGameStart = $E15D) */
    PRINT_STRING(0xE15A);
    /* $E167: JSR print_string ; inline block at $E16A */
    /*   .byte 8, $22, $8B, "CONTINUE" (aContinue = $E16D) */
    PRINT_STRING(0xE16A);
    LDAI(0xC6);                              /* LDA #$C6 */
    STA(byte_201);                           /* STA byte_201 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_202);                           /* STA byte_202 */
    LDAI(0x4C);                              /* LDA #$4C */
    STA(byte_203);                           /* STA byte_203 */

loc_E184:                                    /* $E184 */
    LDAI(0x82);                              /* LDA #$82 */
    LDY(byte_199);                           /* LDY byte_199 */
    BEQ(loc_E18D);                           /* BEQ loc_E18D */
    LDAI(0x9A);                              /* LDA #$9A */

loc_E18D:                                    /* $E18D */
    STA(byte_200);                           /* STA byte_200 */

loc_E190:                                    /* $E190 */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    pad_read();                              /* JSR pad_read */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ORA(pad_test2);                          /* ORA pad_test2 */
    TAY();                                   /* TAY */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_E1A2);                           /* BEQ loc_E1A2 */
    RTS();                                   /* RTS */

loc_E1A2:                                    /* $E1A2 */
    TYA();                                   /* TYA */
    ANDI(0x2C);                              /* AND #$2C */
    BEQ(loc_E1C9);                           /* BEQ loc_E1C9 */
    LDAI(0x15);                              /* LDA #$15 */
    apu_play();                              /* JSR apu_play */
    TYA();                                   /* TYA */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_E1BC);                           /* BEQ loc_E1BC */
    LDA(byte_199);                           /* LDA byte_199 */
    EORI(1);                                 /* EOR #1 */

loc_E1B6:                                    /* $E1B6 */
    STA(byte_199);                           /* STA byte_199 */
    goto loc_E184;                           /* JMP loc_E184 */

loc_E1BC:                                    /* $E1BC */
    TYA();                                   /* TYA */
    ANDI(8);                                 /* AND #8 */
    BEQ(loc_E1C5);                           /* BEQ loc_E1C5 */
    LDAI(0x00);                              /* LDA #0 */
    BEQ(loc_E1B6);                           /* BEQ loc_E1B6 */

loc_E1C5:                                    /* $E1C5 */
    LDAI(1);                                 /* LDA #1 */
    BNE(loc_E1B6);                           /* BNE loc_E1B6 */

loc_E1C9:                                    /* $E1C9 */
    LDA(reset_counter);                      /* LDA reset_counter */
    CMPI(0x0D);                              /* CMP #$D */
    BNE(loc_E190);                           /* BNE loc_E190 */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ORA(pad_test2);                          /* ORA pad_test2 */
    ANDI(3);                                 /* AND #3 */
    BEQ(loc_E190);                           /* BEQ loc_E190 */
    ANDI(2);                                 /* AND #2 */
    BNE(loc_E1E6);                           /* BNE loc_E1E6 */
    DEC(byte_7D);                            /* DEC byte_7D */
    BNE(loc_E1F2);                           /* BNE loc_E1F2 */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_7D);                            /* STA byte_7D */
    BNE(loc_E1F2);                           /* BNE loc_E1F2 */

loc_E1E6:                                    /* $E1E6 */
    INC(byte_7D);                            /* INC byte_7D */
    LDA(byte_7D);                            /* LDA byte_7D */
    CMPI(0x0D);                              /* CMP #$D */
    BNE(loc_E1F2);                           /* BNE loc_E1F2 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_7D);                            /* STA byte_7D */

loc_E1F2:                                    /* $E1F2 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0x88);                              /* LDA #$88 */
    STA(byte_18);                            /* STA byte_18 */
    LDA(byte_7D);                            /* LDA byte_7D */
    sub_D347();                              /* JSR sub_D347 */
    goto loc_E190;                           /* JMP loc_E190 */
}
