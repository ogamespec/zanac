/* bank6/BB56.c - bank 6, $BB56-$BE40: the fortress-destruction sequence
 * (sub_1BB56), the slot-table spawner (sub_1BC04) and the palette/score-object
 * helpers sub_1BC4E, sub_1BCE0, sub_1BDC9 and sub_1BE08.
 *
 * Ported from src/bank6.asm lines 10829-11230 (addresses from Build/bank6.lst).
 *
 * Notes for the reviewer (PORTING.md section 5):
 *
 *  - sub_1BC04 contains a backward branch at $BC5E whose target is the *entry
 *    point* of the procedure itself ($BC04).  It is ported as a local label
 *    that calls the procedure and returns (the same convention the other
 *    chunks use for a branch into a procedure entry); in C the label sits at
 *    the end of the function, so the fall-through into it reproduces the
 *    original `BNE sub_1BC04` exactly.  Register X and the Y table offset are
 *    the implicit parameters of that loop, as in the original.
 *
 *  - sub_1BB56 is entered either at $BB56 (`BEQ loc_1BB5B`, which tests the Z
 *    flag left by the caller sub_1B8E5) or straight at $BB5B; the latter is the
 *    ordinary entry, reached by the fall-through below.
 *
 *  - sub_1BC04 is the procedure listed as an un-ported XREF
 *    `sub_1BCE0+7C`; it is called from sub_1BB56 and from sub_1BCE0, and its
 *    own backward branch re-enters it.
 *
 *  - $BB2B, $BB2F, $BC77, $BDB9 and $BDF5 are data tables in the middle of the
 *    code; their bytes stay in the extracted bank image and are read through
 *    the instruction macros (as in the other ported chunks).
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $BB56  sub_1BB56 - the fortress destruction sequence
 *
 *  Original: src/bank6.asm:10829-10922
 *      BEQ loc_1BB5B / JMP sub_1BCE0
 *  loc_1BB5B:
 *      LDA #12 / JSR j_apu_play / LDX #3
 *  loc_1BB62:
 *      LDA $BB2B,X / STA $145,X / STA $148,X / STA $14B,X / STA $14E,X /
 *      DEX / BNE loc_1BB62 / LDA #1 / STA $144 / LDY #$1E / JSR sub_C024 /
 *      JSR sub_C084 / JSR sub_1BC4E / LDA #$F / STA $6F
 *  loc_1BB88:
 *      LDY #4 / JSR sub_C024 / LDA $6F / JSR sub_C08A / LDA $6F / PHA /
 *      EOR #$FF / AND #$1F / STA $6F / JSR sub_C08A / PLA / STA $6F /
 *      DEC $6F / BPL loc_1BB88 / LDA #0 / STA $6F
 *  loc_1BBA9:
 *      LDY #8 / JSR sub_C024 / LDA $6F / INC $6F / CMP #3 / BEQ loc_1BBD3 /
 *      STA $10 / ASL / ADC $10 / ASL / ASL / TAX / LDY #0
 *  loc_1BBC0:
 *      LDA $BB2F,X / STA $146,Y / INX / INY / CPY #$C / BNE loc_1BBC0 /
 *      LDA #1 / STA $144 / BNE loc_1BBA9
 *  loc_1BBD3:
 *      LDA byte_1BB53 / STA $15B / LDA byte_1BB54 / STA $15C /
 *      LDA byte_1BB55 / STA $15D / LDA #3 / STA $144 / LDA #$D1 / STA $C7 /
 *      LDA #2 / STA $C0 / LDA #0 / STA $C1 / STA $C2 / LDA #4 / STA $C3 /
 *      LDA #$C / STA $6F / LDX #$19 / LDY #0
 *  (no RTS: falls through into sub_1BC04 at $BC04)
 *
 *  loc_1BBD3 sets X=$19 (slot) and Y=0 (table offset) for the following
 *  sub_1BC04, exactly like the JSR sub_1BC04 at $BD58 of sub_1BCE0 does.
 * ------------------------------------------------------------------------ */
void sub_1BB56(void)                         /* $BB56 */
{
    BEQ(loc_1BB5B);                          /* BEQ loc_1BB5B */
    sub_1BCE0();                             /* JMP sub_1BCE0 */
    RTS();                                   /* RTS */

loc_1BB5B:                                   /* $BB5B */
    LDAI(0x0C);                              /* LDA #12 */
    j_apu_play();                            /* JSR j_apu_play */
    LDXI(3);                                 /* LDX #3 */

loc_1BB62:                                   /* $BB62 */
    LDA(ABSX(0xBB2B));                       /* LDA $BB2B,X (tab_BB2C - 1) */
    STA(ABSX(0x0145));                       /* STA $145,X */
    STA(ABSX(0x0148));                       /* STA $148,X */
    STA(ABSX(0x014B));                       /* STA $14B,X */
    STA(ABSX(0x014E));                       /* STA $14E,X */
    DEX();                                   /* DEX */
    BNE(loc_1BB62);                          /* BNE loc_1BB62 */
    LDAI(0x01);                              /* LDA #1 */
    STA(cram_update_mode);                   /* STA $144 */
    LDYI(0x1E);                              /* LDY #$1E */
    sub_C024();                              /* JSR sub_C024 */
    sub_C084();                              /* JSR sub_C084 */
    sub_1BC4E();                             /* JSR sub_1BC4E */
    LDAI(0x0F);                              /* LDA #$F */
    STA(byte_6F);                            /* STA $6F */

loc_1BB88:                                   /* $BB88 */
    LDYI(4);                                 /* LDY #4 */
    sub_C024();                              /* JSR sub_C024 */
    LDA(byte_6F);                            /* LDA $6F */
    sub_C08A();                              /* JSR sub_C08A */
    LDA(byte_6F);                            /* LDA $6F */
    PHA();                                   /* PHA */
    EORI(0xFF);                              /* EOR #$FF */
    ANDI(0x1F);                              /* AND #$1F */
    STA(byte_6F);                            /* STA $6F */
    sub_C08A();                              /* JSR sub_C08A */
    PLA();                                   /* PLA */
    STA(byte_6F);                            /* STA $6F */
    DEC(byte_6F);                            /* DEC $6F */
    BPL(loc_1BB88);                          /* BPL loc_1BB88 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_6F);                            /* STA $6F */

loc_1BBA9:                                   /* $BBA9 */
    LDYI(8);                                 /* LDY #8 */
    sub_C024();                              /* JSR sub_C024 */
    LDA(byte_6F);                            /* LDA $6F */
    INC(byte_6F);                            /* INC $6F */
    CMPI(3);                                 /* CMP #3 */
    BEQ(loc_1BBD3);                          /* BEQ loc_1BBD3 */
    STA(byte_10);                            /* STA $10 */
    ASL_A();                                 /* ASL */
    ADC(byte_10);                            /* ADC $10 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    TAX();                                   /* TAX */
    LDYI(0);                                 /* LDY #0 */

loc_1BBC0:                                   /* $BBC0 */
    LDA(ABSX(0xBB2F));                       /* LDA $BB2F,X */
    STA(ABSY(0x0146));                       /* STA $146,Y */
    INX();                                   /* INX */
    INY();                                   /* INY */
    CPYI(0x0C);                              /* CPY #$C */
    BNE(loc_1BBC0);                          /* BNE loc_1BBC0 */
    LDAI(0x01);                              /* LDA #1 */
    STA(cram_update_mode);                   /* STA $144 */
    BNE(loc_1BBA9);                          /* BNE loc_1BBA9 */

loc_1BBD3:                                   /* $BBD3 */
    LDA(byte_1BB53);                         /* LDA byte_1BB53 */
    STA(0x015B);                             /* STA $15B */
    LDA(byte_1BB54);                         /* LDA byte_1BB54 */
    STA(0x015C);                             /* STA $15C */
    LDA(byte_1BB55);                         /* LDA byte_1BB55 */
    STA(0x015D);                             /* STA $15D */
    LDAI(3);                                 /* LDA #3 */
    STA(cram_update_mode);                   /* STA $144 */
    LDAI(0xD1);                              /* LDA #$D1 */
    STA(byte_C7);                            /* STA $C7 */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_C0);                            /* STA $C0 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_C1);                            /* STA $C1 */
    STA(byte_C2);                            /* STA $C2 */
    LDAI(4);                                 /* LDA #4 */
    STA(byte_C3);                            /* STA $C3 */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_6F);                            /* STA $6F */
    LDXI(0x19);                              /* LDX #$19 */
    LDYI(0);                                 /* LDY #0 */
    sub_1BC04();                             /* falls through into $BC04 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $BC04  sub_1BC04 - fill $6F slots from the 7-byte table at $BC77
 *
 *  Original: src/bank6.asm:10928-10967
 *      LDA byte_1BC77,Y / STA $528,X / INY / LDA byte_1BC77,Y / STA $542,X /
 *      INY / LDA ... $55C,X / INY / LDA ... $5F8,X / INY /
 *      LDA ... $612,X / INY / LDA ... $62C,X / INY / LDA ... $646,X / INY /
 *      TYA / PHA / LDA #4 / STA $74A,X / LDA $528,X / CMP #$5A /
 *      BEQ loc_1BC46 / JSR fortress_core_appears
 *  loc_1BC46:
 *      PLA / TAY / DEX / DEC $6F / BNE sub_1BC04 / RTS
 *
 *  X is the slot to fill (a parameter, preserved by the DEX/BNE loop) and Y is
 *  the offset into byte_1BC77 at entry.  The backward branch at $BC5E/BE is
 *  ported as a call to the procedure entry followed by a return (the local
 *  label at the end of the function; X and the PHA balance are unchanged by
 *  that path).
 * ------------------------------------------------------------------------ */
void sub_1BC04(void)                         /* $BC04 */
{
    LDA(ABSY(byte_1BC77));                   /* LDA byte_1BC77,Y */
    STA(ABSX(byte_528));                     /* STA $528,X */
    INY();                                   /* INY */
    LDA(ABSY(byte_1BC77));                   /* LDA byte_1BC77,Y */
    STA(ABSX(byte_542));                     /* STA $542,X */
    INY();                                   /* INY */
    LDA(ABSY(byte_1BC77));                   /* LDA byte_1BC77,Y */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    INY();                                   /* INY */
    LDA(ABSY(byte_1BC77));                   /* LDA byte_1BC77,Y */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    INY();                                   /* INY */
    LDA(ABSY(byte_1BC77));                   /* LDA byte_1BC77,Y */
    STA(ABSX(byte_612));                     /* STA $612,X */
    INY();                                   /* INY */
    LDA(ABSY(byte_1BC77));                   /* LDA byte_1BC77,Y */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    INY();                                   /* INY */
    LDA(ABSY(byte_1BC77));                   /* LDA byte_1BC77,Y */
    STA(ABSX(byte_646));                     /* STA $646,X */
    INY();                                   /* INY */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDAI(4);                                 /* LDA #4 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0x5A);                              /* CMP #$5A */
    BEQ(loc_1BC46);                          /* BEQ loc_1BC46 */
    fortress_core_appears();                 /* JSR fortress_core_appears */

loc_1BC46:                                   /* $BC46 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEX();                                   /* DEX */
    DEC(byte_6F);                            /* DEC $6F */
    BNE(sub_1BC04);                          /* BNE sub_1BC04 ($BC04) */
    goto sub_1BC04;                          /* re-enter the procedure entry */

sub_1BC04:                                   /* $BC04 - entry of this procedure */
    sub_1BC04();                             /* call $BC04, then leave this invocation */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $BC4E  sub_1BC4E - write the 64-byte $104 block as one PPU column
 *
 *  Original: src/bank6.asm:10973-10994
 *      LDA #$43 / JSR sub_C04B / JSR sub_C03C / TAX / LDA #$40 / STA $460,X /
 *      LDA #$23 / STA $461,X / LDA #$C0 / STA $462,X / LDY #0
 *  loc_1BC68:
 *      LDA $104,Y / STA $463,X / INY / INX / CPY #$40 / BNE loc_1BC68 /
 *      JMP sub_C03F
 * ------------------------------------------------------------------------ */
void sub_1BC4E(void)                         /* $BC4E */
{
    LDAI(0x43);                              /* LDA #$43 */
    sub_C04B();                              /* JSR sub_C04B */
    sub_C03C();                              /* JSR sub_C03C */
    TAX();                                   /* TAX */
    LDAI(0x40);                              /* LDA #$40 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDAI(0x23);                              /* LDA #$23 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDAI(0xC0);                              /* LDA #$C0 */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDYI(0);                                 /* LDY #0 */

loc_1BC68:                                   /* $BC68 */
    LDA(ABSY(0x0104));                       /* LDA $104,Y */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INY();                                   /* INY */
    INX();                                   /* INX */
    CPYI(0x40);                              /* CPY #$40 */
    BNE(loc_1BC68);                          /* BNE loc_1BC68 */
    sub_C03F();                              /* JMP sub_C03F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $BCE0  sub_1BCE0 - fortress shape $11: spawn the score objects and the
 *         command block
 *
 *  Original: src/bank6.asm:11016-11128
 *      CMP #$11 / BEQ loc_1BCE7 / JMP sub_1BDC9
 *  loc_1BCE7:
 *      LDA #$D / STA $6F
 *  loc_1BCEB:
 *      JSR sub_C08D / LDY #4
 *  loc_1BCF0:
 *      TYA / PHA / LDY #3
 *  loc_1BCF4:
 *      TYA / PHA / LDA #$F / STA $14 / LDA #$7F / STA $15 / LDA #$80 /
 *      STA $19 / LDA $6F / ASL / ASL / ASL / STA $18 / JSR sub_1B49F / PLA /
 *      TAY / DEY / BNE loc_1BCF4 / LDY #4 / JSR sub_C024 / PLA / TAY / DEY /
 *      BNE loc_1BCF0 / DEC $6F / BNE loc_1BCEB / LDA #$B2 / STA $C7 /
 *      LDA #2 / STA $C0 / LDA #0 / STA $C1 / STA $C2 / STA $C3 /
 *      LDA #$4F / STA $53F / STA $540 / STA $541 / LDA #$30 / STA $55A /
 *      STA $55B / LDA #$50 / STA $559 / STA $575 / LDA #$B0 / STA $574 /
 *      LDA #$80 / STA $573 / LDA #3 / STA $6F / LDX #$16 / LDY #$54 /
 *      JSR sub_1BC04 / LDY #0
 *  loc_1BD61:
 *      LDX byte_1BDB9,Y / INY / LDA byte_1BDB9,Y / INY / STA $4B0,X /
 *      CPY #$10 / BNE loc_1BD61 / LDA #$D / JSR sub_C04B / JSR sub_C03C /
 *      TAX / LDA #2 / STA $460,X / ... (four 4-byte column recipes) ... /
 *      INC byte_24 / INC byte_24 / JMP sub_C03F
 *
 *  A is the parameter of the dispatch at the entry ($C7 & $1F from sub_1B8E5).
 * ------------------------------------------------------------------------ */
void sub_1BCE0(void)                         /* $BCE0 */
{
    CMPI(0x11);                              /* CMP #$11 */
    BEQ(loc_1BCE7);                          /* BEQ loc_1BCE7 */
    sub_1BDC9();                             /* JMP sub_1BDC9 */
    RTS();                                   /* RTS */

loc_1BCE7:                                   /* $BCE7 */
    LDAI(0x0D);                              /* LDA #$D */
    STA(byte_6F);                            /* STA $6F */

loc_1BCEB:                                   /* $BCEB */
    sub_C08D();                              /* JSR sub_C08D */
    LDYI(4);                                 /* LDY #4 */

loc_1BCF0:                                   /* $BCF0 */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDYI(3);                                 /* LDY #3 */

loc_1BCF4:                                   /* $BCF4 */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDAI(0x0F);                              /* LDA #$F */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x7F);                              /* LDA #$7F */
    STA(byte_15);                            /* STA $15 */
    LDAI(0x80);                              /* LDA #$80 */
    STA(byte_19);                            /* STA $19 */
    LDA(byte_6F);                            /* LDA $6F */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    STA(byte_18);                            /* STA $18 */
    sub_1B49F();                             /* JSR sub_1B49F */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEY();                                   /* DEY */
    BNE(loc_1BCF4);                          /* BNE loc_1BCF4 */
    LDYI(4);                                 /* LDY #4 */
    sub_C024();                              /* JSR sub_C024 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEY();                                   /* DEY */
    BNE(loc_1BCF0);                          /* BNE loc_1BCF0 */
    DEC(byte_6F);                            /* DEC $6F */
    BNE(loc_1BCEB);                          /* BNE loc_1BCEB */
    LDAI(0xB2);                              /* LDA #$B2 */
    STA(byte_C7);                            /* STA $C7 */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_C0);                            /* STA $C0 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_C1);                            /* STA $C1 */
    STA(byte_C2);                            /* STA $C2 */
    STA(byte_C3);                            /* STA $C3 */
    LDAI(0x4F);                              /* LDA #$4F */
    STA(0x053F);                             /* STA $53F */
    STA(0x0540);                             /* STA $540 */
    STA(0x0541);                             /* STA $541 */
    LDAI(0x30);                              /* LDA #$30 */
    STA(0x055A);                             /* STA $55A */
    STA(0x055B);                             /* STA $55B */
    LDAI(0x50);                              /* LDA #$50 */
    STA(0x0559);                             /* STA $559 */
    STA(0x0575);                             /* STA $575 */
    LDAI(0xB0);                              /* LDA #$B0 */
    STA(0x0574);                             /* STA $574 */
    LDAI(0x80);                              /* LDA #$80 */
    STA(0x0573);                             /* STA $573 */
    LDAI(3);                                 /* LDA #3 */
    STA(byte_6F);                            /* STA $6F */
    LDXI(0x16);                              /* LDX #$16 */
    LDYI(0x54);                              /* LDY #$54 */
    sub_1BC04();                             /* JSR sub_1BC04 */
    LDYI(0);                                 /* LDY #0 */

loc_1BD61:                                   /* $BD61 */
    LDX(ABSY(byte_1BDB9));                   /* LDX byte_1BDB9,Y */
    INY();                                   /* INY */
    LDA(ABSY(byte_1BDB9));                   /* LDA byte_1BDB9,Y */
    INY();                                   /* INY */
    STA(ABSX(0x04B0));                       /* STA $4B0,X */
    CPYI(0x10);                              /* CPY #$10 */
    BNE(loc_1BD61);                          /* BNE loc_1BD61 */
    LDAI(0x0D);                              /* LDA #$D */
    sub_C04B();                              /* JSR sub_C04B */
    sub_C03C();                              /* JSR sub_C03C */
    TAX();                                   /* TAX */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(0x0465));                       /* STA $465,X */
    STA(ABSX(0x0469));                       /* STA $469,X */
    LDAI(0x23);                              /* LDA #$23 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    STA(ABSX(0x0466));                       /* STA $466,X */
    STA(ABSX(0x046A));                       /* STA $46A,X */
    LDAI(0xCB);                              /* LDA #$CB */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDAI(0xD1);                              /* LDA #$D1 */
    STA(ABSX(0x0467));                       /* STA $467,X */
    LDAI(0xD6);                              /* LDA #$D6 */
    STA(ABSX(0x046B));                       /* STA $46B,X */
    LDAI(0x77);                              /* LDA #$77 */
    STA(ABSX(0x0463));                       /* STA $463,X */
    LDAI(0xDD);                              /* LDA #$DD */
    STA(ABSX(0x0464));                       /* STA $464,X */
    LDAI(0x55);                              /* LDA #$55 */
    STA(ABSX(0x0468));                       /* STA $468,X */
    STA(ABSX(0x046C));                       /* STA $46C,X */
    INC(byte_24);                            /* INC byte_24 */
    INC(byte_24);                            /* INC byte_24 */
    sub_C03F();                              /* JMP sub_C03F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $BDC9  sub_1BDC9 - fortress shape other than $11: the armour-hit sequence
 *
 *  Original: src/bank6.asm:11143-11171
 *      LDA $30 / ORA #$18 / STA $30 / LDA #0 / STA $92 / LDY #$1E /
 *      JSR sub_C024
 *  loc_1BDD8:
 *      LDA #20 / JSR j_apu_play / LDY #$A
 *  loc_1BDDF:
 *      TYA / PHA / LDA #$D / JSR sub_C081 / LDY #1 / JSR sub_C024 / PLA /
 *      TAY / DEY / BNE loc_1BDDF / DEC $32 / BNE loc_1BDD8 / RTS
 * ------------------------------------------------------------------------ */
void sub_1BDC9(void)                         /* $BDC9 */
{
    LDA(game_mode);                          /* LDA $30 */
    ORAI(0x18);                              /* ORA #$18 */
    STA(game_mode);                          /* STA $30 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_92);                            /* STA $92 */
    LDYI(0x1E);                              /* LDY #$1E */
    sub_C024();                              /* JSR sub_C024 */

loc_1BDD8:                                   /* $BDD8 */
    LDAI(20);                                /* LDA #20 (hard hit (armor) sound) */
    j_apu_play();                            /* JSR j_apu_play */
    LDYI(0x0A);                              /* LDY #$A */

loc_1BDDF:                                   /* $BDDF */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDAI(0x0D);                              /* LDA #$D */
    sub_C081();                              /* JSR sub_C081 */
    LDYI(1);                                 /* LDY #1 */
    sub_C024();                              /* JSR sub_C024 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEY();                                   /* DEY */
    BNE(loc_1BDDF);                          /* BNE loc_1BDDF */
    DEC(byte_32);                            /* DEC $32 */
    BNE(loc_1BDD8);                          /* BNE loc_1BDD8 (hard hit (armor) sound) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $BE08  sub_1BE08 - fortress destroyed: restore the game mode and screen
 *
 *  Original: src/bank6.asm:11197-11230
 *      LDA $62 / AND #$F7 / STA $62 / LDA $47 / CLC / ADC #$10 / STA $47 /
 *      JSR sub_18788 / LDA #6 / STA $66 / LDA #$C / STA $C0 / LDA $C7 /
 *      AND #$20 / BEQ loc_1BE2C / JSR j_apu_stop /
 *      JSR j_update_music_after_fortress_destroy
 *  loc_1BE2C:
 *      LDA $C7 / AND #$1F / CMP #$F / BNE locret_1BE40 / LDA #$B / STA $92 /
 *      LDA $30 / AND #$EF / ORA #$20 / STA $30
 *  locret_1BE40:
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1BE08(void)                         /* $BE08 */
{
    LDA(byte_62);                            /* LDA $62 */
    ANDI(0xF7);                              /* AND #$F7 */
    STA(byte_62);                            /* STA $62 */
    LDA(byte_47);                            /* LDA $47 */
    CLC();                                   /* CLC */
    ADCI(0x10);                              /* ADC #$10 */
    STA(byte_47);                            /* STA $47 */
    sub_18788();                             /* JSR sub_18788 */
    LDAI(6);                                 /* LDA #6 */
    STA(byte_66);                            /* STA $66 */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_C0);                            /* STA $C0 */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_1BE2C);                          /* BEQ loc_1BE2C */
    j_apu_stop();                            /* JSR j_apu_stop */
    j_update_music_after_fortress_destroy(); /* JSR j_update_music_after_fortress_destroy */

loc_1BE2C:                                   /* $BE2C */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x1F);                              /* AND #$1F */
    CMPI(0x0F);                              /* CMP #$F */
    BNE(locret_1BE40);                       /* BNE locret_1BE40 */
    LDAI(0x0B);                              /* LDA #$B */
    STA(byte_92);                            /* STA $92 */
    LDA(game_mode);                          /* LDA $30 */
    ANDI(0xEF);                              /* AND #$EF */
    ORAI(0x20);                              /* ORA #$20 */
    STA(game_mode);                          /* STA $30 */

locret_1BE40:                                /* $BE40 */
    RTS();                                   /* RTS */
}
