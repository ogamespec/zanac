/* bank6/B44B.c - bank 6, $B44B-$BB2B: the fortress sub-type handlers, the
 * projectile spawner, the enemy_type_80/83/84-87/90 handlers and the
 * level/final-fortress state machine sub_1B8E5.
 *
 * Ported from src/bank6.asm lines 9523-10780 (addresses from Build/bank6.lst).
 *
 * Notes for the reviewer (PORTING.md sections 4b, 4c and 5):
 *
 *  - sub_1B533 ends with `JSR j_switch_case` followed by a 7-word table at
 *    $B53C (bytes 49 B5 75 B5 91 B5 AF B5 99 B5 BD B5 10 B6 = the entries
 *    $B549, $B575, $B591, $B5AF, $B599, $B5BD, $B610, i.e. target-1).  It is
 *    ported with SWITCH_CASE(0xB53C); the RTS() that follows returns to the
 *    caller of sub_1B533 exactly like the original computed RTS did.
 *
 *  - sub_1B87F unwinds the 6502 stack (`PLA / PLA / RTS` at $B889 and
 *    `PLA / PLA / JSR sub_18788 / JMP enemy_erase` at $B89B) in order to
 *    abandon the calling handler and resume the enemy slot loop at loc_18807.
 *    A C call pushes no return address on the emulated stack, so "return from
 *    the caller as well" cannot be expressed; the port keeps the PLAs and the
 *    rest of the code literally and returns to the caller.  This is a
 *    documented deviation (see the comments inside sub_1B87F).
 *
 *  - enemy_type_80 jumps/JSRs to $A93F and $A96C (inside
 *    enemy_type_35_small_explosion, chunk bank6_A609) and to $B895 (inside
 *    sub_1B87F, this file); enemy_type_84_85_86_87 jumps to $AEC6 and $AED8
 *    (inside enemy_type_70_71_81_88_89, chunk bank6_AD25).  A C label of one
 *    function cannot be reached from another translation unit, so - as
 *    PORTING.md section 5 prescribes for a tail that is a label inside another
 *    procedure - the shared tails are reproduced verbatim here with a comment
 *    naming the original label and its 6502 address.  Each such region starts
 *    with a "reproduced from" comment.
 *
 *  - sub_1B54A does `BCS sub_1B592`, i.e. branches straight into another
 *    procedure's entry point; that is ported as a call plus return, which
 *    reproduces the original tail flow.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $B44B  sub_1B44B - pick the sprite id from the enemy health
 *
 *  Original: src/bank6.asm:9523-9541
 *      LDY #$50 / LDA $764,X / CMP #$1E / BCS loc_1B462 / LDY #$51 /
 *      CMP #$14 / BCS loc_1B462 / LDY #$52 / CMP #$A / BCS loc_1B462 /
 *      LDY #$53 / loc_1B462: TYA / STA $576,X / RTS
 * ------------------------------------------------------------------------ */
void sub_1B44B(void)                         /* $B44B */
{
    LDYI(0x50);                              /* LDY #$50 */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    CMPI(0x1E);                              /* CMP #$1E */
    BCS(loc_1B462);                          /* BCS loc_1B462 */
    LDYI(0x51);                              /* LDY #$51 */
    CMPI(0x14);                              /* CMP #$14 */
    BCS(loc_1B462);                          /* BCS loc_1B462 */
    LDYI(0x52);                              /* LDY #$52 */
    CMPI(0x0A);                              /* CMP #$A */
    BCS(loc_1B462);                          /* BCS loc_1B462 */
    LDYI(0x53);                              /* LDY #$53 */

loc_1B462:                                   /* $B462 */
    TYA();                                   /* TYA */
    STA(ABSX(byte_576));                     /* STA $576,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B467  sub_1B467 - destroyed fortress sub-type: turn into a score object
 *
 *  Original: src/bank6.asm:9547-9564
 *      LDA $88 / CMP #$4F / BNE loc_1B478 / LDA #$CF / STA $528,X /
 *      LDA #2 / STA $5AA,X / RTS
 *  loc_1B478:
 *      LDA #$50 / STA $528,X / DEC $C2 / JMP sub_1B507
 * ------------------------------------------------------------------------ */
void sub_1B467(void)                         /* $B467 */
{
    LDA(0x0088);                             /* LDA $88 */
    CMPI(0x4F);                              /* CMP #$4F */
    BNE(loc_1B478);                          /* BNE loc_1B478 */
    LDAI(0xCF);                              /* LDA #$CF */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    RTS();                                   /* RTS */

loc_1B478:                                   /* $B478 */
    LDAI(0x50);                              /* LDA #$50 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    DEC(byte_C2);                            /* DEC $C2 */
    sub_1B507();                             /* JMP sub_1B507 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B482  sub_1B482 - fortress sub-type: count the health down, blink
 *
 *  Original: src/bank6.asm:9570-9582
 *      DEC $764,X
 *      BEQ loc_1B478
 *      LDA $764,X
 *      AND #3
 *      BEQ loc_1B48F
 *      RTS
 *  loc_1B48F:
 *      LDA #$1F
 *  (no RTS: falls through into sub_1B491)
 *
 *  loc_1B478 is a label inside sub_1B467; that tail is reproduced here
 *  (PORTING.md section 5) so that the branch keeps its original effect.
 * ------------------------------------------------------------------------ */
void sub_1B482(void)                         /* $B482 */
{
    DEC(ABSX(byte_764));                     /* DEC $764,X */
    BEQ(loc_1B478);                          /* BEQ loc_1B478 */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    ANDI(0x03);                              /* AND #3 */
    BEQ(loc_1B48F);                          /* BEQ loc_1B48F */
    RTS();                                   /* RTS */

loc_1B478:                                   /* $B478 - reproduced from sub_1B467 */
    LDAI(0x50);                              /* LDA #$50 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    DEC(byte_C2);                            /* DEC $C2 */
    sub_1B507();                             /* JMP sub_1B507 */
    RTS();                                   /* RTS */

loc_1B48F:                                   /* $B48F */
    LDAI(0x1F);                              /* LDA #$1F */
    sub_1B491();                             /* falls through into $B491 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B491  sub_1B491 - stash the parent's position and the score argument (A)
 *
 *  Original: src/bank6.asm:9588-9595
 *      STA $14 / STA $15 / LDA $55C,X / STA $19 / LDA $542,X / STA $18
 *  (no RTS: falls through into sub_1B49F)
 * ------------------------------------------------------------------------ */
void sub_1B491(void)                         /* $B491 */
{
    STA(byte_14);                            /* STA $14 */
    STA(byte_15);                            /* STA $15 */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    STA(byte_19);                            /* STA $19 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    STA(byte_18);                            /* STA $18 */
    sub_1B49F();                             /* falls through into $B49F */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B49F  sub_1B49F - spawn a score object at a random offset around the parent
 *
 *  Original: src/bank6.asm:9601-9631
 *      JSR check_enemy_overflow / BCS locret_1B4D4 / LDA #$23 /
 *      STA $528,Y / JSR enemy_prng / AND $14 / PHA / LDA $14 / LSR /
 *      STA $10 / PLA / SBC $10 / ADC $18 / STA $542,Y / JSR enemy_prng /
 *      AND $15 / PHA / LDA $15 / LSR / STA $10 / PLA / SBC $10 / ADC $19 /
 *      STA $55C,Y / LDA #0 / STA $7E6,Y
 *  locret_1B4D4:
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1B49F(void)                         /* $B49F */
{
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_1B4D4);                       /* BCS locret_1B4D4 */
    LDAI(0x23);                              /* LDA #$23 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    enemy_prng();                            /* JSR enemy_prng */
    AND(byte_14);                            /* AND $14 */
    PHA();                                   /* PHA */
    LDA(byte_14);                            /* LDA $14 */
    LSR_A();                                 /* LSR */
    STA(byte_10);                            /* STA $10 */
    PLA();                                   /* PLA */
    SBC(byte_10);                            /* SBC $10 */
    ADC(byte_18);                            /* ADC $18 */
    STA(ABSY(byte_542));                     /* STA $542,Y */
    enemy_prng();                            /* JSR enemy_prng */
    AND(byte_15);                            /* AND $15 */
    PHA();                                   /* PHA */
    LDA(byte_15);                            /* LDA $15 */
    LSR_A();                                 /* LSR */
    STA(byte_10);                            /* STA $10 */
    PLA();                                   /* PLA */
    SBC(byte_10);                            /* SBC $10 */
    ADC(byte_19);                            /* ADC $19 */
    STA(ABSY(byte_55C));                     /* STA $55C,Y */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSY(0x07E6));                       /* STA $7E6,Y */

locret_1B4D4:                                /* $B4D4 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B4D5  sub_1B4D5 - fortress sub-type: draw the destroyed core
 *
 *  Original: src/bank6.asm:9637-9673
 *      LDA $528,X / CMP #$CF / BNE loc_1B4DD
 *  locret_1B4DC:
 *      RTS
 *  loc_1B4DD:
 *      CMP #$CB / BNE loc_1B4F9 / LDA $74A,X / CMP #$12 / LDA $528,X /
 *      BCC loc_1B4F9 / LDA $730,X / ASL / ASL / ADC #$60 / TAY /
 *      LDA byte_171 / JMP sub_1B1D0
 *  loc_1B4F9:
 *      SEC / SBC #$C9 / ASL / ASL / ADC $730,X / ASL / ASL / TAY /
 *      JMP sub_1B1CD
 *
 *  locret_1B4DC is also the target of `JMP loc_1B4DC` in sub_1B507; that
 *  procedure reproduces the RTS as its own local label.
 * ------------------------------------------------------------------------ */
void sub_1B4D5(void)                         /* $B4D5 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xCF);                              /* CMP #$CF */
    BNE(loc_1B4DD);                          /* BNE loc_1B4DD */
    /* $B4DC locret_1B4DC: reached here by fall-through only (the branch to it
     * comes from sub_1B507, reproduced there); no C label is needed. */
    RTS();                                   /* RTS */

loc_1B4DD:                                   /* $B4DD */
    CMPI(0xCB);                              /* CMP #$CB */
    BNE(loc_1B4F9);                          /* BNE loc_1B4F9 */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    CMPI(0x12);                              /* CMP #$12 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BCC(loc_1B4F9);                          /* BCC loc_1B4F9 */
    LDA(ABSX(0x0730));                       /* LDA $730,X */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ADCI(0x60);                              /* ADC #$60 */
    TAY();                                   /* TAY */
    LDA(byte_171);                           /* LDA byte_171 */
    sub_1B1D0();                             /* JMP sub_1B1D0 */
    RTS();                                   /* RTS */

loc_1B4F9:                                   /* $B4F9 */
    SEC();                                   /* SEC */
    SBCI(0xC9);                              /* SBC #$C9 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ADC(ABSX(0x0730));                       /* ADC $730,X */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    sub_1B1CD();                             /* JMP sub_1B1CD */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B507  sub_1B507 - fortress sub-type: choose the score sprite / sound
 *
 *  Original: src/bank6.asm:9679-9705
 *      LDA $7E6,X / CMP #$4F / BEQ locret_1B4DC / CMP #$4B / BEQ loc_1B51C /
 *      BCS loc_1B52B / LDY #4 / LDA byte_171 / JMP sub_1B1EB
 *  loc_1B51C:
 *      LDY #$58 / LDA $74A,X / CMP #4 / BEQ loc_1B52B / LDA byte_171 /
 *      JMP sub_1B1EB
 *  loc_1B52B:
 *      LDY #8 / LDA byte_171 / JMP sub_1B1EB
 * ------------------------------------------------------------------------ */
void sub_1B507(void)                         /* $B507 */
{
    LDA(ABSX(0x07E6));                       /* LDA $7E6,X */
    CMPI(0x4F);                              /* CMP #$4F */
    BEQ(locret_1B4DC);                       /* BEQ locret_1B4DC (RTS at $B4DC) */
    CMPI(0x4B);                              /* CMP #$4B */
    BEQ(loc_1B51C);                          /* BEQ loc_1B51C */
    BCS(loc_1B52B);                          /* BCS loc_1B52B */
    LDYI(4);                                 /* LDY #4 */
    LDA(byte_171);                           /* LDA byte_171 */
    sub_1B1EB();                             /* JMP sub_1B1EB */
    RTS();                                   /* RTS */

loc_1B51C:                                   /* $B51C */
    LDYI(0x58);                              /* LDY #$58 */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    CMPI(4);                                 /* CMP #4 */
    BEQ(loc_1B52B);                          /* BEQ loc_1B52B */
    LDA(byte_171);                           /* LDA byte_171 */
    sub_1B1EB();                             /* JMP sub_1B1EB */
    RTS();                                   /* RTS */

loc_1B52B:                                   /* $B52B */
    LDYI(8);                                 /* LDY #8 */
    LDA(byte_171);                           /* LDA byte_171 */
    sub_1B1EB();                             /* JMP sub_1B1EB */
    RTS();                                   /* RTS */

locret_1B4DC:                                /* $B4DC - reproduced from sub_1B4D5 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B533  sub_1B533 - dispatch on the fortress sub-type
 *
 *  Original: src/bank6.asm:9711-9716
 *      LDA $528,X
 *      SEC
 *      SBC #$C9
 *      JSR j_switch_case
 *  (the 7-word table at $B53C follows in the instruction stream)
 *
 *  Table at $B53C (bytes 49 B5 75 B5 91 B5 AF B5 99 B5 BD B5 10 B6):
 *      $B549 -> sub_1B54A   $B575 -> sub_1B576   $B591 -> sub_1B592
 *      $B5AF -> sub_1B5B0   $B599 -> sub_1B59A   $B5BD -> sub_1B5BE
 *      $B610 -> sub_1B611
 *  switch_case returns to the selected entry with a computed RTS; the RTS()
 *  below is the return to sub_1B533's caller once that entry has finished.
 * ------------------------------------------------------------------------ */
void sub_1B533(void)                         /* $B533 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    SEC();                                   /* SEC */
    SBCI(0xC9);                              /* SBC #$C9 */
    SWITCH_CASE(0xB53C);                     /* JSR j_switch_case (table at $B53C) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B54A  sub_1B54A - fortress sub-type 0
 *
 *  Original: src/bank6.asm:9730-9762
 *      LDA $660,X
 *  loc_1B54D:
 *      CLC / ADC #3 / STA $660,X / AND #$F / CMP #$F / BCS loc_1B56E /
 *      CMP #$E / BCS sub_1B592 / CMP #9 / BCS loc_1B54D / STA $14 /
 *      JSR check_enemy_overflow / BCC loc_1B569
 *  locret_1B568:
 *      RTS
 *  loc_1B569:
 *      LDA #$15 / JMP spawn_projectile
 *  loc_1B56E:
 *      JSR check_enemy_overflow / BCS locret_1B568 / JMP loc_1B62B
 *
 *  `BCS sub_1B592` branches into another procedure; it is ported as a call
 *  plus return.  loc_1B62B lives inside sub_1B611 and is reproduced below.
 * ------------------------------------------------------------------------ */
void sub_1B54A(void)                         /* $B54A */
{
    LDA(ABSX(byte_660));                     /* LDA $660,X */

loc_1B54D:                                   /* $B54D */
    CLC();                                   /* CLC */
    ADCI(3);                                 /* ADC #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    ANDI(0x0F);                              /* AND #$F */
    CMPI(0x0F);                              /* CMP #$F */
    BCS(loc_1B56E);                          /* BCS loc_1B56E */
    CMPI(0x0E);                              /* CMP #$E */
    if (F_C) {                               /* BCS sub_1B592 (branch to the procedure) */
        sub_1B592();                         /* tail jump to $B592 */
        RTS();                               /* RTS */
    }
    CMPI(0x09);                              /* CMP #9 */
    BCS(loc_1B54D);                          /* BCS loc_1B54D */
    STA(byte_14);                            /* STA $14 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCC(loc_1B569);                          /* BCC loc_1B569 */

locret_1B568:                                /* $B568 */
    RTS();                                   /* RTS */

loc_1B569:                                   /* $B569 */
    LDAI(0x15);                              /* LDA #$15 */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();                                   /* RTS */

loc_1B56E:                                   /* $B56E */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_1B568);                       /* BCS locret_1B568 */
    goto loc_1B62B;                          /* JMP loc_1B62B */

loc_1B62B:                                   /* $B62B - reproduced from sub_1B611 */
    LDAI(0x2A);                              /* LDA #$2A */
    /* BNE spawn_projectile: always taken (A = $2A != 0) */
    spawn_projectile();                      /* BNE spawn_projectile */
    RTS();                                   /* RTS */
    LDAI(0x2B);                              /* LDA #$2B (unreachable) */
}

/* ------------------------------------------------------------------------ *
 *  $B576  sub_1B576 - fortress sub-type 1
 *
 *  Original: src/bank6.asm:9768-9783
 *      JSR check_enemy_overflow / BCS locret_1B568 / TYA / PHA /
 *      JSR calculate_slope / STA $14 / PLA / TAY / LDA #6 / STA $74A,Y /
 *      TXA / STA $5AA,Y / LDA #$29 / JMP spawn_projectile
 * ------------------------------------------------------------------------ */
void sub_1B576(void)                         /* $B576 */
{
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_1B568);                       /* BCS locret_1B568 (RTS at $B568) */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    calculate_slope();                       /* JSR calculate_slope */
    STA(byte_14);                            /* STA $14 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    LDAI(6);                                 /* LDA #6 */
    STA(ABSY(0x074A));                       /* STA $74A,Y */
    TXA();                                   /* TXA */
    STA(ABSY(byte_5AA));                     /* STA $5AA,Y */
    LDAI(0x29);                              /* LDA #$29 */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();                                   /* RTS */

locret_1B568:                                /* $B568 - reproduced from sub_1B54A */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B592  sub_1B592 - fortress sub-type 2
 *
 *  Original: src/bank6.asm:9789-9794
 *      JSR check_enemy_overflow / BCS locret_1B568 / JMP loc_1B62B
 * ------------------------------------------------------------------------ */
void sub_1B592(void)                         /* $B592 */
{
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_1B568);                       /* BCS locret_1B568 (RTS at $B568) */
    goto loc_1B62B;                          /* JMP loc_1B62B */

locret_1B568:                                /* $B568 - reproduced from sub_1B54A */
    RTS();                                   /* RTS */

loc_1B62B:                                   /* $B62B - reproduced from sub_1B611 */
    LDAI(0x2A);                              /* LDA #$2A */
    /* BNE spawn_projectile: always taken (A = $2A != 0) */
    spawn_projectile();                      /* BNE spawn_projectile */
    RTS();                                   /* RTS */
    LDAI(0x2B);                              /* LDA #$2B (unreachable) */
}

/* ------------------------------------------------------------------------ *
 *  $B59A  sub_1B59A - fortress sub-type 4: fan of 4 shots
 *
 *  Original: src/bank6.asm:9800-9811
 *      LDA $660,X
 *      INC $660,X
 *      STA $14
 *      LDA #4
 *      STA $16
 *      LDA #3
 *      STA $17
 *      LDA #$15
 *      STA $18
 *      BNE sub_1B5CF
 * ------------------------------------------------------------------------ */
void sub_1B59A(void)                         /* $B59A */
{
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    INC(ABSX(byte_660));                     /* INC $660,X */
    STA(byte_14);                            /* STA $14 */
    LDAI(4);                                 /* LDA #4 */
    STA(byte_16);                            /* STA $16 */
    LDAI(3);                                 /* LDA #3 */
    STA(byte_17);                            /* STA $17 */
    LDAI(0x15);                              /* LDA #$15 */
    STA(byte_18);                            /* STA $18 */
    /* BNE sub_1B5CF: always taken (A = $15 != 0) */
    sub_1B5CF();                             /* BNE sub_1B5CF */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B5B0  sub_1B5B0 - fortress sub-type 3: 3-shot spread
 *
 *  Original: src/bank6.asm:9817-9825
 *      LDA #3
 *      STA $16
 *      LDA #0
 *      STA $17
 *      LDA #$3B
 *      STA $18
 *      BNE loc_1B5CA
 *
 *  loc_1B5CA is a label inside sub_1B5BE; its tail is reproduced below.
 * ------------------------------------------------------------------------ */
void sub_1B5B0(void)                         /* $B5B0 */
{
    LDAI(3);                                 /* LDA #3 */
    STA(byte_16);                            /* STA $16 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_17);                            /* STA $17 */
    LDAI(0x3B);                              /* LDA #$3B */
    STA(byte_18);                            /* STA $18 */
    /* BNE loc_1B5CA: always taken (A = $3B != 0) */
    goto loc_1B5CA;                          /* BNE loc_1B5CA */

loc_1B5CA:                                   /* $B5CA - reproduced from sub_1B5BE */
    calculate_slope();                       /* JSR calculate_slope */
    STA(byte_14);                            /* STA $14 */
    sub_1B5CF();                             /* falls through into $B5CF */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B5BE  sub_1B5BE - fortress sub-type 5: 5-shot spread
 *
 *  Original: src/bank6.asm:9831-9842
 *      LDA #5
 *      STA $16
 *      LDA #7
 *      STA $17
 *      LDA #$2B
 *      STA $18
 *  loc_1B5CA:
 *      JSR calculate_slope
 *      STA $14
 *  (no RTS: falls through into sub_1B5CF)
 *
 *  loc_1B5CA has no branch inside this procedure (only the external, always
 *  taken branch from sub_1B5B0, reproduced there); it is therefore documented
 *  here instead of being declared as an unused C label.
 * ------------------------------------------------------------------------ */
void sub_1B5BE(void)                         /* $B5BE */
{
    LDAI(5);                                 /* LDA #5 */
    STA(byte_16);                            /* STA $16 */
    LDAI(7);                                 /* LDA #7 */
    STA(byte_17);                            /* STA $17 */
    LDAI(0x2B);                              /* LDA #$2B */
    STA(byte_18);                            /* STA $18 */
    /* $B5CA loc_1B5CA (fall-through) */
    calculate_slope();                       /* JSR calculate_slope */
    STA(byte_14);                            /* STA $14 */
    sub_1B5CF();                             /* falls through into $B5CF */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B5CF  sub_1B5CF - fortress sub-type 0/3/4/5/6: fire a spread of shots
 *
 *  Original: src/bank6.asm:9848-9877
 *      LDA #0 / STA $15
 *  loc_1B5D3:
 *      LDA $15 / CLC / ADC $17 / TAY / LDA $14 / CLC / ADC unk_1B5F5,Y /
 *      STA $14 / JSR check_enemy_overflow / BCC loc_1B5E7
 *  locret_1B5E6:
 *      RTS
 *  loc_1B5E7:
 *      LDA $18 / JSR spawn_projectile / INC $15 / DEC $16 /
 *      BEQ locret_1B5E6 / JMP loc_1B5D3
 * ------------------------------------------------------------------------ */
void sub_1B5CF(void)                         /* $B5CF */
{
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_15);                            /* STA $15 */

loc_1B5D3:                                   /* $B5D3 */
    LDA(byte_15);                            /* LDA $15 */
    CLC();                                   /* CLC */
    ADC(byte_17);                            /* ADC $17 */
    TAY();                                   /* TAY */
    LDA(byte_14);                            /* LDA $14 */
    CLC();                                   /* CLC */
    ADC(ABSY(unk_1B5F5));                    /* ADC unk_1B5F5,Y */
    STA(byte_14);                            /* STA $14 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCC(loc_1B5E7);                          /* BCC loc_1B5E7 */

locret_1B5E6:                                /* $B5E6 */
    RTS();                                   /* RTS */

loc_1B5E7:                                   /* $B5E7 */
    LDA(byte_18);                            /* LDA $18 */
    spawn_projectile();                      /* JSR spawn_projectile */
    INC(byte_15);                            /* INC $15 */
    DEC(byte_16);                            /* DEC $16 */
    BEQ(locret_1B5E6);                       /* BEQ locret_1B5E6 */
    goto loc_1B5D3;                          /* JMP loc_1B5D3 */
}

/* ------------------------------------------------------------------------ *
 *  $B611  sub_1B611 - fortress sub-type 6: alternating shots
 *
 *  Original: src/bank6.asm:9912-9933
 *      INC $660,X / LDA $660,X / AND #3 / BNE loc_1B61E / JMP sub_1B592
 *  loc_1B61E:
 *      LDA #4 / STA $14 / JSR check_enemy_overflow / BCS locret_1B5E6 /
 *      LDA #$2D / BNE spawn_projectile
 *  loc_1B62B:
 *      LDA #$2A / BNE spawn_projectile / LDA #$2B
 *
 *  loc_1B62B is a second entry point (reached only by `JMP loc_1B62B` from
 *  sub_1B54A/sub_1B592, reproduced there), so no C label is declared here; the
 *  statements are kept in place.  `LDA #$2B` ($B62F) is unreachable because
 *  the BNE above always branches.
 * ------------------------------------------------------------------------ */
void sub_1B611(void)                         /* $B611 */
{
    INC(ABSX(byte_660));                     /* INC $660,X */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ANDI(0x03);                              /* AND #3 */
    BNE(loc_1B61E);                          /* BNE loc_1B61E */
    sub_1B592();                             /* JMP sub_1B592 */
    RTS();                                   /* RTS */

loc_1B61E:                                   /* $B61E */
    LDAI(4);                                 /* LDA #4 */
    STA(byte_14);                            /* STA $14 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_1B5E6);                       /* BCS locret_1B5E6 (RTS at $B5E6) */
    LDAI(0x2D);                              /* LDA #$2D */
    /* BNE spawn_projectile: always taken (A = $2D != 0) */
    spawn_projectile();                      /* BNE spawn_projectile */
    RTS();                                   /* RTS */

    /* $B62B loc_1B62B (second entry point, see the header comment) */
    LDAI(0x2A);                              /* LDA #$2A */
    /* BNE spawn_projectile: always taken (A = $2A != 0) */
    spawn_projectile();                      /* BNE spawn_projectile */
    RTS();                                   /* RTS */
    LDAI(0x2B);                              /* LDA #$2B (unreachable) */

locret_1B5E6:                                /* $B5E6 - reproduced from sub_1B5CF */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B631  spawn_projectile - release a bullet from the parent object (A =
 *         projectile type, Y = slot)
 *
 *  Original: src/bank6.asm:9940-9950
 *      STA $528,Y
 *      LDA $14
 *      STA $77E,Y
 *      LDA $542,X
 *      STA $542,Y
 *      LDA $55C,X
 *      STA $55C,Y
 *      RTS
 * ------------------------------------------------------------------------ */
void spawn_projectile(void)                  /* $B631 */
{
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDA(byte_14);                            /* LDA $14 */
    STA(ABSY(byte_77E));                     /* STA $77E,Y */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    STA(ABSY(byte_542));                     /* STA $542,Y */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    STA(ABSY(byte_55C));                     /* STA $55C,Y */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B667  sub_1B667 - pick the chase parameters from unk_1B8AD
 *
 *  Original: src/bank6.asm:9968-9998
 *      LDA $6C8,X / AND #7 / STA $10 / ASL / ADC $10 / ASL / ADC $716,X /
 *      ADC $5C4,X / TAY / LDA unk_1B8AD,Y / STA $67A,X / LDA $716,X /
 *      CMP #2 / BNE loc_1B695 / LDA $5C4,X / CLC / ADC #3 / CMP #6 /
 *      BNE loc_1B692 / LDA #0 /
 *  loc_1B692:
 *      STA $5C4 /
 *  loc_1B695:
 *      LDA #0 / STA $694,X / RTS
 * ------------------------------------------------------------------------ */
void sub_1B667(void)                         /* $B667 */
{
    LDA(ABSX(0x06C8));                       /* LDA $6C8,X */
    ANDI(0x07);                              /* AND #7 */
    STA(byte_10);                            /* STA $10 */
    ASL_A();                                 /* ASL */
    ADC(byte_10);                            /* ADC $10 */
    ASL_A();                                 /* ASL */
    ADC(ABSX(byte_716));                     /* ADC $716,X */
    ADC(ABSX(byte_5C4));                     /* ADC $5C4,X */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_1B8AD));                    /* LDA unk_1B8AD,Y */
    STA(ABSX(unk_67A));                      /* STA $67A,X */
    LDA(ABSX(byte_716));                     /* LDA $716,X */
    CMPI(2);                                 /* CMP #2 */
    BNE(loc_1B695);                          /* BNE loc_1B695 */
    LDA(ABSX(byte_5C4));                     /* LDA $5C4,X */
    CLC();                                   /* CLC */
    ADCI(3);                                 /* ADC #3 */
    CMPI(6);                                 /* CMP #6 */
    BNE(loc_1B692);                          /* BNE loc_1B692 */
    LDAI(0x00);                              /* LDA #0 */

loc_1B692:                                   /* $B692 */
    STA(0x05C4);                             /* STA $5C4 */

loc_1B695:                                   /* $B695 */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x0694));                       /* STA $694,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B69B  enemy_type_80 - enemy type 80 (small explosion)
 *
 *  Original: src/bank6.asm:10004-10018
 *      LDA $528,X
 *      BMI loc_1B6B0
 *      ORA #$80
 *      STA $528,X
 *      JSR sub_18796
 *      LDA #18
 *      JSR j_apu_play
 *      JMP loc_1A93F
 *  loc_1B6B0:
 *      JSR loc_1B895
 *      JMP loc_1A96C
 *
 *  loc_1A93F/loc_1A96C are labels inside enemy_type_35_small_explosion
 *  (chunk bank6_A609) and loc_1B895 is inside sub_1B87F (this file); their
 *  tails are reproduced below, as PORTING.md section 5 prescribes.
 * ------------------------------------------------------------------------ */
void enemy_type_80(void)                     /* $B69B */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1B6B0);                          /* BMI loc_1B6B0 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    sub_18796();                             /* JSR sub_18796 */
    LDAI(18);                                /* LDA #18 */
    j_apu_play();                            /* JSR j_apu_play */
    goto loc_1A93F;                          /* JMP loc_1A93F */

loc_1B6B0:                                   /* $B6B0 */
    /* JSR loc_1B895 ($B895, inside sub_1B87F): the called tail is inlined
     * here; its RTS at $B89A returns to the JMP below, while the $B89B path
     * erases the object and does not come back. */
    sub_1B8A3();                             /* JSR sub_1B8A3 */
    BCS(loc_1B89B);                          /* BCS loc_1B89B */
    goto loc_1A96C;                          /* JMP loc_1A96C (after the RTS at $B89A) */

loc_1B89B:                                   /* $B89B - reproduced from sub_1B87F */
    /* PLA / PLA: unwind, see the file header */
    PLA();                                   /* PLA */
    PLA();                                   /* PLA */
    sub_18788();                             /* JSR sub_18788 */
    enemy_erase();                           /* JMP enemy_erase */
    RTS();                                   /* RTS */

    /* ---- reproduced from enemy_type_35_small_explosion ($A8EA, chunk
     *      bank6_A609), src/bank6.asm:7581-7616 ---- */

loc_1A93F:                                   /* $A93F */
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x40);                              /* LDA #$40 */
    LDYI(7);                                 /* LDY #7 */
    sub_1A2CB();                             /* JSR sub_1A2CB */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDA(ABSX(0x07E6));                       /* LDA $7E6,X */
    BEQ(loc_1A96C);                          /* BEQ loc_1A96C */
    CMPI(0x25);                              /* CMP #$25 */
    BEQ(loc_1A96C);                          /* BEQ loc_1A96C */
    CMPI(0x26);                              /* CMP #$26 */
    BEQ(loc_1A96C);                          /* BEQ loc_1A96C */
    DEC(byte_45);                            /* DEC $45 */
    BNE(loc_1A96C);                          /* BNE loc_1A96C */
    LDAI(0x10);                              /* LDA #$10 */
    STA(byte_45);                            /* STA $45 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_46);                            /* STA $46 */

loc_1A96C:                                   /* $A96C */
    sub_183BC();                             /* JSR sub_183BC */
    BCS(loc_1A97B);                          /* BCS loc_1A97B */
    LDA(ABSY(unk_1A97E));                    /* LDA unk_1A97E,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    RTS();                                   /* RTS */

loc_1A97B:                                   /* $A97B */
    enemy_erase();                           /* JMP enemy_erase */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B6B6  enemy_type_83 - enemy type 83
 *
 *  Original: src/bank6.asm:10024-10086 (73 lines, see the .asm for the full
 *  listing; the statements below follow it in order).
 * ------------------------------------------------------------------------ */
void enemy_type_83(void)                     /* $B6B6 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1B6D9);                          /* BMI loc_1B6D9 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0xE0);                              /* LDA #$E0 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x35);                              /* LDA #$35 */
    STA(ABSX(byte_576));                     /* STA $576,X */

loc_1B6D9:                                   /* $B6D9 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    LDAI(0xFC);                              /* LDA #$FC */
    STA(byte_87);                            /* STA $87 */
    LDYI(3);                                 /* LDY #3 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BEQ(loc_1B6E9);                          /* BEQ loc_1B6E9 */
    LDYI(2);                                 /* LDY #2 */

loc_1B6E9:                                   /* $B6E9 */
    STY(byte_86);                            /* STY $86 */
    LDAI(0xF6);                              /* LDA #$F6 */
    STA(byte_84);                            /* STA $84 */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    ASL_A();                                 /* ASL */
    STA(byte_85);                            /* STA $85 */
    add_next_sprite_from_zeropage();         /* JSR add_next_sprite_from_zeropage */
    sub_181ED();                             /* JSR sub_181ED */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BPL(loc_1B701);                          /* BPL loc_1B701 */
    RTS();                                   /* RTS */

loc_1B701:                                   /* $B701 */
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA $528 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_764);                           /* STA $764 */
    LDAI(0x80);                              /* LDA #$80 */
    STA(byte_5AA);                           /* STA $5AA */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BNE(loc_1B720);                          /* BNE loc_1B720 */
    LDA(byte_5A);                            /* LDA $5A */
    SEC();                                   /* SEC */
    SBCI(5);                                 /* SBC #5 */
    BCS(loc_1B71E);                          /* BCS loc_1B71E */
    LDAI(0x00);                              /* LDA #0 */

loc_1B71E:                                   /* $B71E */
    STA(byte_5A);                            /* STA $5A */

loc_1B720:                                   /* $B720 */
    sub_1879F();                             /* JSR sub_1879F */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    sub_19493();                             /* JSR sub_19493 */
    enemy_erase();                           /* JMP enemy_erase */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B72C  enemy_type_84_85_86_87 - enemy types 84-87
 *
 *  Original: src/bank6.asm:10092-10175 (84 lines, see the .asm for the full
 *  listing).
 *
 *  loc_1AEC6 and loc_1AED8 are labels inside enemy_type_70_71_81_88_89
 *  (chunk bank6_AD25); the whole shared continuation from $AEC6 to the end of
 *  that procedure is reproduced below (PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void enemy_type_84_85_86_87(void)            /* $B72C */
{
    sub_1B87F();                             /* JSR sub_1B87F */
    BCC(loc_1B73E);                          /* BCC loc_1B73E */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDAI(0x18);                              /* LDA #$18 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    goto loc_1AEC6;                          /* JMP loc_1AEC6 */

loc_1B73E:                                   /* $B73E */
    DEC(ABSX(byte_6E2));                     /* DEC $6E2,X */
    BNE(loc_1B775);                          /* BNE loc_1B775 */
    LDA(ABSX(byte_6FC));                     /* LDA $6FC,X */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_1B775);                          /* BCS loc_1B775 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xD4);                              /* CMP #$D4 */
    BEQ(loc_1B79F);                          /* BEQ loc_1B79F */
    CMPI(0xD5);                              /* CMP #$D5 */
    BEQ(loc_1B778);                          /* BEQ loc_1B778 */
    CMPI(0xD7);                              /* CMP #$D7 */
    BEQ(loc_1B7A3);                          /* BEQ loc_1B7A3 */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    INC(ABSX(byte_660));                     /* INC $660,X */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ANDI(0x03);                              /* AND #3 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_14);                            /* STA $14 */

loc_1B770:                                   /* $B770 */
    LDAI(0x15);                              /* LDA #$15 */

loc_1B772:                                   /* $B772 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_1B775:                                   /* $B775 */
    goto loc_1AED8;                          /* JMP loc_1AED8 */

loc_1B778:                                   /* $B778 */
    LDA(byte_55C);                           /* LDA $55C */
    CMP(ABSX(byte_55C));                     /* CMP $55C,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_14);                            /* STA $14 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_15);                            /* STA $15 */
    BCS(loc_1B790);                          /* BCS loc_1B790 */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_14);                            /* STA $14 */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(byte_15);                            /* STA $15 */

loc_1B790:                                   /* $B790 */
    LDA(byte_AA);                            /* LDA $AA */
    ORA(byte_AB);                            /* ORA $AB */
    BEQ(loc_1B770);                          /* BEQ loc_1B770 */
    LDA(byte_14);                            /* LDA $14 */
    CLC();                                   /* CLC */
    ADC(byte_15);                            /* ADC $15 */
    STA(byte_14);                            /* STA $14 */
    BNE(loc_1B770);                          /* BNE loc_1B770 */

loc_1B79F:                                   /* $B79F */
    LDAI(0x25);                              /* LDA #$25 */
    BNE(loc_1B772);                          /* BNE loc_1B772 */

loc_1B7A3:                                   /* $B7A3 */
    STY(byte_10);                            /* STY $10 */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    TAY();                                   /* TAY */
    LDA(ABSY(0xB7A7));                       /* LDA $B7A7,Y */
    STA(byte_14);                            /* STA $14 */
    LDY(byte_10);                            /* LDY $10 */
    LDAI(0x15);                              /* LDA #$15 */
    BNE(loc_1B772);                          /* BNE loc_1B772 */

    /* ---- reproduced from enemy_type_70_71_81_88_89 ($AEC1, chunk
     *      bank6_AD25), src/bank6.asm:8451-8575 ---- */

loc_1AEC6:                                   /* $AEC6 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ANDI(0x7F);                              /* AND #$7F */
    TAY();                                   /* TAY */
    LDA(ABSY(0xAF54));                       /* LDA $AF54,Y */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDA(ABSY(0xAF68));                       /* LDA $AF68,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */

loc_1AED8:                                   /* $AED8 */
    sub_18204();                             /* JSR sub_18204 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    BEQ(loc_1AEE1);                          /* BEQ loc_1AEE1 */
    RTS();                                   /* RTS */

loc_1AEE1:                                   /* $AEE1 */
    LDA(0x0088);                             /* LDA $88 */
    CMPI(0x51);                              /* CMP #$51 */
    BCS(loc_1AF21);                          /* BCS loc_1AF21 */
    STA(ABSX(unk_67A));                      /* STA $67A,X */
    sub_187A6();                             /* JSR sub_187A6 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCC(loc_1AEF3);                          /* BCC loc_1AEF3 */
    RTS();                                   /* RTS */

loc_1AEF3:                                   /* $AEF3 */
    LDAI(0xD1);                              /* LDA #$D1 */
    spawn_projectile();                      /* JSR spawn_projectile */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSY(byte_576));                     /* STA $576,Y */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSY(byte_764));                     /* STA $764,Y */
    LDA(ABSX(byte_5F8));                     /* LDA $5F8,X */
    STA(ABSY(byte_5F8));                     /* STA $5F8,Y */
    LDA(ABSX(byte_612));                     /* LDA $612,X */
    STA(ABSY(byte_612));                     /* STA $612,Y */
    LDA(ABSX(byte_62C));                     /* LDA $62C,X */
    STA(ABSY(byte_62C));                     /* STA $62C,Y */
    LDA(ABSX(byte_646));                     /* LDA $646,X */
    STA(ABSY(byte_646));                     /* STA $646,Y */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    STA(ABSY(0x074A));                       /* STA $74A,Y */
    RTS();                                   /* RTS */

loc_1AF21:                                   /* $AF21 */
    BNE(loc_1AF28);                          /* BNE loc_1AF28 */
    LDYI(0x4C);                              /* LDY #$4C */
    sub_1B1E8();                             /* JMP sub_1B1E8 */
    RTS();                                   /* RTS */

loc_1AF28:                                   /* $AF28 */
    CMPI(0x52);                              /* CMP #$52 */
    BNE(loc_1AF49);                          /* BNE loc_1AF49 */
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LDA(ABSX(byte_716));                     /* LDA $716,X */

loc_1AF37:                                   /* $AF37 */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(0x12);                              /* LDA #$12 */
    j_apu_play();                            /* JSR j_apu_play */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x11);                              /* LDY #$11 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();                                   /* RTS */

loc_1AF49:                                   /* $AF49 */
    CMPI(0x58);                              /* CMP #$58 */
    BCS(loc_1AF83);                          /* BCS loc_1AF83 */
    CMPI(0x54);                              /* CMP #$54 */
    BNE(loc_1AF5B);                          /* BNE loc_1AF5B */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x11);                              /* LDY #$11 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();                                   /* RTS */

loc_1AF5B:                                   /* $AF5B */
    CMPI(0x57);                              /* CMP #$57 */
    BNE(loc_1AF6B);                          /* BNE loc_1AF6B */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    ANDI(0x02);                              /* AND #2 */
    ASL_A();                                 /* ASL */
    ADCI(0x10);                              /* ADC #$10 */
    TAY();                                   /* TAY */
    sub_1B1E2();                             /* JMP sub_1B1E2 */
    RTS();                                   /* RTS */

loc_1AF6B:                                   /* $AF6B */
    CMPI(0x55);                              /* CMP #$55 */
    BNE(loc_1AF79);                          /* BNE loc_1AF79 */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x21);                              /* LDY #$21 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();                                   /* RTS */

loc_1AF79:                                   /* $AF79 */
    LDAI(0x40);                              /* LDA #$40 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x12);                              /* LDY #$12 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();                                   /* RTS */

loc_1AF83:                                   /* $AF83 */
    BEQ(loc_1AF95);                          /* BEQ loc_1AF95 */
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(0x07);                              /* AND #7 */
    goto loc_1AF37;                          /* JMP loc_1AF37 */

loc_1AF95:                                   /* $AF95 */
    LDYI(0x0C);                              /* LDY #$C */
    sub_1B1E8();                             /* JMP sub_1B1E8 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B7B8  enemy_type_90 - enemy type 90, the fortress core
 *
 *  Original: src/bank6.asm:10186-10282 (97 lines, see the .asm for the full
 *  listing).
 * ------------------------------------------------------------------------ */
void enemy_type_90(void)                     /* $B7B8 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1B7D4);                          /* BMI loc_1B7D4 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(0x0A);                              /* LDA #$A */
    STA(ABSX(byte_764));                     /* STA $764,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */

loc_1B7D4:                                   /* $B7D4 */
    LDA(ABSX(0x052B));                       /* LDA $52B,X */
    CMPI(0xCF);                              /* CMP #$CF */
    BEQ(loc_1B7DE);                          /* BEQ loc_1B7DE */
    goto loc_1B860;                          /* JMP loc_1B860 */

loc_1B7DE:                                   /* $B7DE */
    LDA(ABSX(0x0767));                       /* LDA $767,X */
    LDYI(0);                                 /* LDY #0 */
    CMPI(0x28);                              /* CMP #$28 */
    BCS(loc_1B7FB);                          /* BCS loc_1B7FB */
    LDYI(0x18);                              /* LDY #$18 */
    CMPI(0x1E);                              /* CMP #$1E */
    BCS(loc_1B7FB);                          /* BCS loc_1B7FB */
    LDYI(0x30);                              /* LDY #$30 */
    CMPI(0x14);                              /* CMP #$14 */
    BCS(loc_1B7FB);                          /* BCS loc_1B7FB */
    LDYI(0x60);                              /* LDY #$60 */
    CMPI(0x0A);                              /* CMP #$A */
    BCS(loc_1B7FB);                          /* BCS loc_1B7FB */
    LDYI(0xF0);                              /* LDY #$F0 */

loc_1B7FB:                                   /* $B7FB */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0694));                       /* ADC $694,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    BCC(loc_1B860);                          /* BCC loc_1B860 */
    sub_1B611();                             /* JSR sub_1B611 */
    INC(ABSX(0x0767));                       /* INC $767,X */
    INC(ABSX(0x06AE));                       /* INC $6AE,X */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    ANDI(0x03);                              /* AND #3 */
    TAY();                                   /* TAY */
    LDA(ABSX(byte_646));                     /* LDA $646,X */
    ORAI(0xC0);                              /* ORA #$C0 */
    STA(byte_14);                            /* STA $14 */
    LDAI(5);                                 /* LDA #5 */
    sub_C039();                              /* JSR sub_C039 */
    BCS(loc_1B860);                          /* BCS loc_1B860 */
    sub_C03C();                              /* JSR sub_C03C */
    STX(byte_10);                            /* STX $10 */
    TAX();                                   /* TAX */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDAI(0x23);                              /* LDA #$23 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(byte_14);                            /* LDA $14 */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDA(byte_10);                            /* LDA $10 */
    CMPI(0x14);                              /* CMP #$14 */
    BEQ(loc_1B84F);                          /* BEQ loc_1B84F */
    LDA(ABSY(unk_1B873));                    /* LDA unk_1B873,Y */
    STA(ABSX(0x0463));                       /* STA $463,X */
    DEC(ABSX(0x0460));                       /* DEC $460,X */
    DEC(byte_25);                            /* DEC $25 */
    LDX(byte_10);                            /* LDX $10 */
    sub_C03F();                              /* JSR sub_C03F */
    BNE(loc_1B860);                          /* BNE loc_1B860 */

loc_1B84F:                                   /* $B84F */
    LDA(ABSY(unk_1B877));                    /* LDA unk_1B877,Y */
    STA(ABSX(0x0463));                       /* STA $463,X */
    LDA(ABSY(unk_1B87B));                    /* LDA unk_1B87B,Y */
    STA(ABSX(0x0464));                       /* STA $464,X */
    LDX(byte_10);                            /* LDX $10 */
    sub_C03F();                              /* JSR sub_C03F */

loc_1B860:                                   /* $B860 */
    sub_18204();                             /* JSR sub_18204 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    BEQ(loc_1B869);                          /* BEQ loc_1B869 */
    RTS();                                   /* RTS */

loc_1B869:                                   /* $B869 */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x22);                              /* LDY #$22 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B87F  sub_1B87F - move an object's Y position and spawn/erase it
 *
 *  Original: src/bank6.asm:10301-10331
 *      LDA $528,X
 *      BMI loc_1B895
 *      JSR sub_1B8A3
 *      BCS loc_1B88C
 *      PLA
 *      PLA
 *      RTS
 *  loc_1B88C:
 *      LDA $528,X
 *      ORA #$80
 *      STA $528,X
 *      RTS
 *  loc_1B895:
 *      JSR sub_1B8A3
 *      BCS loc_1B89B
 *      RTS
 *  loc_1B89B:
 *      PLA
 *      PLA
 *      JSR sub_18788
 *      JMP enemy_erase
 *
 *  Both PLA/PLA/RTS sequences discard this JSR's return address so that the
 *  final RTS resumes the enemy slot loop at loc_18807, abandoning the calling
 *  handler.  In C a call pushes nothing on the emulated 6502 stack, so the
 *  double return cannot be expressed; the port keeps the PLAs and returns to
 *  the caller instead of the slot loop.  Documented deviation.
 * ------------------------------------------------------------------------ */
void sub_1B87F(void)                         /* $B87F */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1B895);                          /* BMI loc_1B895 */
    sub_1B8A3();                             /* JSR sub_1B8A3 */
    BCS(loc_1B88C);                          /* BCS loc_1B88C */
    /* $B889 PLA / PLA / RTS: the pops discard this routine's JSR frame, so the
     * RTS resumes the dispatcher's slot loop ($8807) - the port's longjmp
     * (see zanac_ram.h). */
    ZANAC_UNWIND();                          /* PLA / PLA / RTS */
    RTS();                                   /* RTS */

loc_1B88C:                                   /* $B88C */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    RTS();                                   /* RTS */

loc_1B895:                                   /* $B895 */
    sub_1B8A3();                             /* JSR sub_1B8A3 */
    BCS(loc_1B89B);                          /* BCS loc_1B89B */
    RTS();                                   /* RTS */

loc_1B89B:                                   /* $B89B */
    /* $B89B PLA / PLA: the pops discard this routine's JSR frame, so the RTS of
     * enemy_erase resumes the dispatcher's slot loop ($8807) - the port's
     * longjmp (see zanac_ram.h). */
    sub_18788();                             /* JSR sub_18788 */
    enemy_erase();                           /* JMP enemy_erase */
    ZANAC_UNWIND();                          /* the RTS of enemy_erase */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B8A3  sub_1B8A3 - add byte_AE to the object's Y position
 *
 *  Original: src/bank6.asm:10337-10344
 *      LDA $AE
 *      CLC
 *      ADC $542,X
 *      STA $542,X
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1B8A3(void)                         /* $B8A3 */
{
    LDA(byte_AE);                            /* LDA $AE */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_542));                     /* ADC $542,X */
    STA(ABSX(byte_542));                     /* STA $542,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B8E5  sub_1B8E5 - the level / final-fortress state machine
 *
 *  Original: src/bank6.asm:10407-10780 (374 lines, see the .asm for the full
 *  listing; the statements below follow it in order).
 *
 *  The PHA at $BA27 / PLA at $BAA1 is a genuine save/restore of A across the
 *  fortress-destruction block and is ported literally.
 * ------------------------------------------------------------------------ */
void sub_1B8E5(void)                         /* $B8E5 */
{
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0xA0);                              /* AND #$A0 */
    BEQ(loc_1B90F);                          /* BEQ loc_1B90F */
    INC(0x00CC);                             /* INC $CC */
    LDA(0x00CC);                             /* LDA $CC */
    ANDI(0x07);                              /* AND #7 */
    BNE(loc_1B90F);                          /* BNE loc_1B90F */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x7F);                              /* AND #$7F */
    STA(byte_C0);                            /* STA $C0 */
    LDA(0x00CC);                             /* LDA $CC */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ANDI(0x07);                              /* AND #7 */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_1B8DD));                    /* LDA unk_1B8DD,Y */
    STA(0x014B);                             /* STA $14B */
    LDAI(1);                                 /* LDA #1 */
    ORA(cram_update_mode);                   /* ORA $144 */
    STA(cram_update_mode);                   /* STA $144 */

loc_1B90F:                                   /* $B90F */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x20);                              /* AND #$20 */
    BNE(locret_1B924);                       /* BNE locret_1B924 */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x02);                              /* AND #2 */
    BEQ(loc_1B91E);                          /* BEQ loc_1B91E */
    goto loc_1B9AC;                          /* JMP loc_1B9AC */

loc_1B91E:                                   /* $B91E */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x01);                              /* AND #1 */
    BNE(loc_1B925);                          /* BNE loc_1B925 */

locret_1B924:                                /* $B924 */
    RTS();                                   /* RTS */

loc_1B925:                                   /* $B925 */
    LDA(byte_90);                            /* LDA $90 */
    ANDI(0x01);                              /* AND #1 */
    BEQ(locret_1B924);                       /* BEQ locret_1B924 */
    DEC(byte_C6);                            /* DEC $C6 */
    BEQ(loc_1B95F);                          /* BEQ loc_1B95F */
    LDA(byte_C6);                            /* LDA $C6 */
    CMPI(8);                                 /* CMP #8 */
    BCS(locret_1B924);                       /* BCS locret_1B924 */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(ABSY(0xB950));                       /* LDA $B950,Y */
    CMP(byte_AB);                            /* CMP $AB */
    BEQ(loc_1B940);                          /* BEQ loc_1B940 */
    BCS(locret_1B924);                       /* BCS locret_1B924 */

loc_1B940:                                   /* $B940 */
    LDA(ABSY(0xB94F));                       /* LDA $B94F,Y */
    BCC(loc_1B949);                          /* BCC loc_1B949 */
    CMP(byte_AA);                            /* CMP $AA */
    BCS(locret_1B924);                       /* BCS locret_1B924 */

loc_1B949:                                   /* $B949 */
    STA(byte_AA);                            /* STA $AA */
    LDA(ABSY(0xB950));                       /* LDA $B950,Y */
    STA(byte_AB);                            /* STA $AB */
    RTS();                                   /* RTS */

loc_1B95F:                                   /* $B95F */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_AA);                            /* STA $AA */
    STA(byte_AB);                            /* STA $AB */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_1B96E);                          /* BEQ loc_1B96E */
    sub_C072();                              /* JSR sub_C072 */

loc_1B96E:                                   /* $B96E */
    LDAI(0x60);                              /* LDA #$60 */
    STA(0x00CA);                             /* STA $CA */
    LDAI(0x3C);                              /* LDA #$3C */
    STA(byte_C4);                            /* STA $C4 */
    LDA(byte_C5);                            /* LDA $C5 */
    STA(0x00C9);                             /* STA $C9 */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x1F);                              /* AND #$1F */
    CMPI(0x10);                              /* CMP #$10 */
    BCC(loc_1B988);                          /* BCC loc_1B988 */
    LDYI(0);                                 /* LDY #0 */
    STY(byte_AC);                            /* STY $AC */
    STY(byte_AD);                            /* STY $AD */

loc_1B988:                                   /* $B988 */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_C0);                            /* STA $C0 */
    LDA(byte_C8);                            /* LDA $C8 */
    ANDI(0x02);                              /* AND #2 */
    BNE(loc_1B99C);                          /* BNE loc_1B99C */
    LDA(byte_62);                            /* LDA $62 */
    ORAI(0x08);                              /* ORA #8 */
    STA(byte_62);                            /* STA $62 */
    LDA(byte_C5);                            /* LDA $C5 */
    STA(0x00C9);                             /* STA $C9 */

loc_1B99C:                                   /* $B99C */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x1F);                              /* AND #$1F */
    CMPI(0x10);                              /* CMP #$10 */
    BCS(loc_1B9AC);                          /* BCS loc_1B9AC */
    LDA(game_mode);                          /* LDA $30 */
    BMI(loc_1B9AC);                          /* BMI loc_1B9AC */
    LDYI(8);                                 /* LDY #8 */
    STY(byte_66);                            /* STY $66 */

loc_1B9AC:                                   /* $B9AC */
    LDA(0x00CA);                             /* LDA $CA */
    BEQ(loc_1B9BF);                          /* BEQ loc_1B9BF */
    DEC(0x00CA);                             /* DEC $CA */
    BNE(loc_1B9BF);                          /* BNE loc_1B9BF */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_1B9BF);                          /* BEQ loc_1B9BF */
    LDAI(26);                                /* LDA #26 (final fortress battle music) */
    j_apu_play();                            /* JSR j_apu_play */

loc_1B9BF:                                   /* $B9BF */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x1F);                              /* AND #$1F */
    CMPI(0x10);                              /* CMP #$10 */
    BCS(loc_1B9EB);                          /* BCS loc_1B9EB */
    DEC(byte_C4);                            /* DEC $C4 */
    BNE(loc_1B9EB);                          /* BNE loc_1B9EB */
    LDAI(0x3C);                              /* LDA #$3C */
    STA(byte_C4);                            /* STA $C4 */
    DEC(byte_C5);                            /* DEC $C5 */
    BNE(loc_1B9D6);                          /* BNE loc_1B9D6 */
    sub_1BE08();                             /* JMP sub_1BE08 */
    RTS();                                   /* RTS */

loc_1B9D6:                                   /* $B9D6 */
    LDA(0x00C9);                             /* LDA $C9 */
    SEC();                                   /* SEC */
    SBCI(3);                                 /* SBC #3 */
    STA(0x00C9);                             /* STA $C9 */
    BCC(loc_1B9EB);                          /* BCC loc_1B9EB */
    LDA(byte_C8);                            /* LDA $C8 */
    ANDI(0x01);                              /* AND #1 */
    BEQ(loc_1B9EB);                          /* BEQ loc_1B9EB */
    LDA(byte_62);                            /* LDA $62 */
    ANDI(0xF7);                              /* AND #$F7 */
    STA(byte_62);                            /* STA $62 */

loc_1B9EB:                                   /* $B9EB */
    LDA(byte_C2);                            /* LDA $C2 */
    BEQ(loc_1B9FD);                          /* BEQ loc_1B9FD */
    CMP(byte_C3);                            /* CMP $C3 */
    BEQ(loc_1B9F6);                          /* BEQ loc_1B9F6 */
    BCC(loc_1B9F6);                          /* BCC loc_1B9F6 */
    RTS();                                   /* RTS */

loc_1B9F6:                                   /* $B9F6 */
    LDA(byte_C0);                            /* LDA $C0 */
    ORAI(0x08);                              /* ORA #8 */
    STA(byte_C0);                            /* STA $C0 */
    RTS();                                   /* RTS */

loc_1B9FD:                                   /* $B9FD */
    LDA(byte_47);                            /* LDA $47 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    STA(byte_10);                            /* STA $10 */
    LDA(byte_47);                            /* LDA $47 */
    SEC();                                   /* SEC */
    SBC(byte_10);                            /* SBC $10 */
    STA(byte_47);                            /* STA $47 */
    LDA(byte_4B);                            /* LDA $4B */
    SBCI(8);                                 /* SBC #8 */
    BCS(loc_1BA12);                          /* BCS loc_1BA12 */
    LDAI(0x00);                              /* LDA #0 */

loc_1BA12:                                   /* $BA12 */
    STA(byte_4B);                            /* STA $4B */
    sub_18796();                             /* JSR sub_18796 */
    LDA(byte_62);                            /* LDA $62 */
    ANDI(0xF7);                              /* AND #$F7 */
    STA(byte_62);                            /* STA $62 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_C0);                            /* STA $C0 */
    sub_1B2ED();                             /* JSR sub_1B2ED */
    LDA(cram_new_palette);                   /* LDA $145 */
    PHA();                                   /* PHA */
    LDAI(0x30);                              /* LDA #$30 */
    sub_1B32E();                             /* JSR sub_1B32E */
    LDA(byte_C7);                            /* LDA $C7 */
    ANDI(0x80);                              /* AND #$80 */
    BNE(loc_1BAA1);                          /* BNE loc_1BAA1 */
    LDYI(5);                                 /* LDY #5 */

loc_1BA35:                                   /* $BA35 */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    CMPI(0x53);                              /* CMP #$53 */
    BEQ(loc_1BA58);                          /* BEQ loc_1BA58 */
    CMPI(0x49);                              /* CMP #$49 */
    BCC(loc_1BA58);                          /* BCC loc_1BA58 */
    CMPI(0x4F);                              /* CMP #$4F */
    BCC(loc_1BA4E);                          /* BCC loc_1BA4E */
    CMPI(0x52);                              /* CMP #$52 */
    BCC(loc_1BA58);                          /* BCC loc_1BA58 */
    CMPI(0x5A);                              /* CMP #$5A */
    BCS(loc_1BA58);                          /* BCS loc_1BA58 */

loc_1BA4E:                                   /* $BA4E */
    LDAI(0x50);                              /* LDA #$50 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSY(0x07E6));                       /* STA $7E6,Y */

loc_1BA58:                                   /* $BA58 */
    INY();                                   /* INY */
    CPYI(0x1A);                              /* CPY #$1A */
    BNE(loc_1BA35);                          /* BNE loc_1BA35 */

loc_1BA5D:                                   /* $BA5D */
    LDA(cram_update_mode);                   /* LDA $144 */
    ORA(byte_9C);                            /* ORA $9C */
    BNE(loc_1BA5D_wait);                     /* BNE loc_1BA5D */
    goto loc_1BA60;

loc_1BA5D_wait:                              /* both are cleared by the NMI */
    ZANAC_WAIT_NMI();
    goto loc_1BA5D;

loc_1BA60:                                   /* $BA60 */
    LDYI(0x0A);                              /* LDY #$A */
    j_all_enemies_destroyed();               /* JSR j_all_enemies_destroyed */
    LDA(byte_C0);                            /* LDA $C0 */
    ORAI(0x10);                              /* ORA #$10 */
    STA(byte_C0);                            /* STA $C0 */
    j_ppu_disable_all();                     /* JSR j_ppu_disable_all */
    LDA(byte_A1);                            /* LDA $A1 */
    CMPI(0x0E);                              /* CMP #$E */
    BNE(loc_1BA7A);                          /* BNE loc_1BA7A */
    LDAI(0xFF);                              /* LDA #$FF */

loc_1BA7A:                                   /* $BA7A */
    STA(byte_15);                            /* STA byte_15 */
    INC(byte_15);                            /* INC byte_15 */
    LDA(byte_9D);                            /* LDA byte_9D */
    STA(byte_14);                            /* STA byte_14 */
    LDA(byte_9F);                            /* LDA byte_9F */
    STA(byte_18);                            /* STA byte_18 */
    LDA(byte_A0);                            /* LDA byte_A0 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_10);                            /* STA byte_10 */
    LDA(0x0175);                             /* LDA $175 */
    STA(0x0076);                             /* STA $76 */
    LDAI(0x0F);                              /* LDA #$F */
    sub_1B0AA();                             /* JSR sub_1B0AA */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0xEF);                              /* AND #$EF */
    STA(byte_C0);                            /* STA $C0 */
    j_ppu_enable_all();                      /* JSR j_ppu_enable_all */

loc_1BAA1:                                   /* $BAA1 */
    PLA();                                   /* PLA */
    sub_1B32E();                             /* JSR sub_1B32E */

loc_1BAA5:                                   /* $BAA5 */
    LDA(cram_update_mode);                   /* LDA cram_update_mode */
    BNE(loc_1BAA5_wait);                     /* BNE loc_1BAA5 */
    goto loc_1BAA8;

loc_1BAA5_wait:                              /* cleared by the NMI */
    ZANAC_WAIT_NMI();
    goto loc_1BAA5;

loc_1BAA8:                                   /* $BAA8 */
    LDYI(0x1B);                              /* LDY #$1B */
    LDA(byte_C7);                            /* LDA byte_C7 */
    ANDI(0xDF);                              /* AND #$DF */
    CMP(byte_C7);                            /* CMP byte_C7 */
    BNE(loc_1BAB6);                          /* BNE loc_1BAB6 */
    LDYI(0x1C);                              /* LDY #$1C */

loc_1BAB6:                                   /* $BAB6 */
    ANDI(0x1F);                              /* AND #$1F */
    CMPI(0x11);                              /* CMP #$11 */
    BEQ(loc_1BACF);                          /* BEQ loc_1BACF */
    PHA();                                   /* PHA */
    j_apu_stop();                            /* JSR j_apu_stop */
    PLA();                                   /* PLA */
    CMPI(0x10);                              /* CMP #$10 */
    BEQ(loc_1BACF);                          /* BEQ loc_1BACF */
    TYA();                                   /* TYA */
    j_apu_play();                            /* JSR j_apu_play */
    sub_C090();                              /* JSR sub_C090 */
    j_apu_stop();                            /* JSR j_apu_stop */

loc_1BACF:                                   /* $BACF */
    LDA(byte_C7);                            /* LDA byte_C7 */
    ANDI(0xBF);                              /* AND #$BF */
    CMP(byte_C7);                            /* CMP byte_C7 */
    BNE(loc_1BADB);                          /* BNE loc_1BADB */
    LDYI(0x0A);                              /* LDY #$A */
    STY(byte_66);                            /* STY byte_66 */

loc_1BADB:                                   /* $BADB */
    ANDI(0x1F);                              /* AND #$1F */
    TAY();                                   /* TAY */
    CMPI(0x10);                              /* CMP #$10 */
    BCC(loc_1BAE8);                          /* BCC loc_1BAE8 */
    LDA(game_mode);                          /* LDA game_mode */
    ORAI(0x04);                              /* ORA #4 */
    STA(game_mode);                          /* STA game_mode */

loc_1BAE8:                                   /* $BAE8 */
    LDA(ABSY(unk_1BDF5));                    /* LDA unk_1BDF5,Y */
    STA(byte_CB);                            /* STA byte_CB */
    sub_C057();                              /* JSR sub_C057 */
    LDYI(0x64);                              /* LDY #$64 */
    LDA(byte_C7);                            /* LDA byte_C7 */
    ANDI(0x1F);                              /* AND #$1F */
    ANDI(0x1E);                              /* AND #$1E */
    CMPI(0x10);                              /* CMP #$10 */
    BEQ(loc_1BAFF);                          /* BEQ loc_1BAFF */
    sub_C024();                              /* JSR sub_C024 */

loc_1BAFF:                                   /* $BAFF */
    sub_C090();                              /* JSR sub_C090 */
    LDYI(0x28);                              /* LDY #$28 */
    sub_C024();                              /* JSR sub_C024 */
    LDYI(6);                                 /* LDY #6 */
    LDA(game_mode);                          /* LDA game_mode */
    BPL(loc_1BB0F);                          /* BPL loc_1BB0F */
    LDYI(9);                                 /* LDY #9 */

loc_1BB0F:                                   /* $BB0F */
    STY(byte_66);                            /* STY byte_66 */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0x02);                              /* AND #2 */
    BEQ(loc_1BB18);                          /* BEQ loc_1BB18 */
    RTS();                                   /* RTS */

loc_1BB18:                                   /* $BB18 */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0xFB);                              /* AND #$FB */
    STA(game_mode);                          /* STA game_mode */
    LDA(byte_C7);                            /* LDA byte_C7 */
    ANDI(0x1F);                              /* AND #$1F */
    CMPI(0x10);                              /* CMP #$10 */
    BCS(loc_1BB29);                          /* BCS loc_1BB29 */
    j_update_music_after_fortress_destroy(); /* JMP j_update_music_after_fortress_destroy */
    RTS();                                   /* RTS */

loc_1BB29:                                   /* $BB29 */
    sub_1BB56();                             /* JMP sub_1BB56 */
    RTS();                                   /* RTS */
}
