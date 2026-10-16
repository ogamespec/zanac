/* bank6/9039.c - bank 6, $9039-$96DB: the Zanac standard shot, the eight
 * special-weapon handlers, the enemy types 2/3/19/68/4-5-6 and their helpers.
 *
 * Ported from src/bank6.asm lines 3399-4661 (addresses from Build/bank6.lst).
 *
 * Constructs that need a convention (see c_src/PORTING.md):
 *
 *  - Computed control flow (section 4b): enemy_type_3, special_weapon_process
 *    and enemy_type_19 pick their continuation with
 *    `JSR j_switch_case` followed by an inline .WORD table.  The table address
 *    is passed by SWITCH_CASE(); the bytes are quoted at each call site.
 *
 *  - Chopped procedures (section 5): sub_1913A, sub_191E6, sub_1925C,
 *    sub_192F1, sub_1935C and sub_1948B fall through into the next procedure;
 *    sub_1913A branches into loc_19164, the shared tail inside sub_1914D.
 *
 *  - Conditional branch to another procedure: the original branches straight to
 *    the entry point of a following procedure (e.g. `BMI special_weapon_process`,
 *    `BNE special_weapon_type_0`).  A C goto cannot jump into another function,
 *    so the branch target becomes a local label carried by the procedure call,
 *    which reproduces the flow (the branch keeps the original stack depth, and
 *    the callee returns to this procedure's caller).
 *
 *  - $9477 `JSR loc_1B2EF` enters sub_1B2ED two bytes in (skipping its
 *    `LDA #$23`).  C cannot enter a function in the middle, so sub_1946F calls
 *    sub_1B2ED(), which reloads A with #$23 instead of keeping the #$3E loaded
 *    at the call site.  This is the only statement of this file that is not
 *    translated faithfully; it is marked with a TODO at the call site.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $9039  enemy_type_2_zanac_standard_shot - a shot fired from a Zanac
 *         standard weapon
 *
 *  Original: src/bank6.asm:3399-3424
 *      LDA $528,X / BMI loc_19068 / ORA #$80 / STA $528,X / LDA $36 /
 *      STA $576,X / LDA zanac_main_weapon_level / LSR / CLC / ADC #13 /
 *      JSR j_apu_play / LDA #0 / STA $590,X / LDA #0 / STA $5AA,X /
 *      STA $5F8,X / LDA #1 / STA $660,X / LDA zanac_main_weapon_speed_y /
 *      STA $612,X
 *  loc_19068:
 *      JSR process_enemy_flags_sprites / JMP sub_181A7
 * ------------------------------------------------------------------------ */
void enemy_type_2_zanac_standard_shot(void)   /* $9039 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19068);                          /* BMI loc_19068 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDA(unk_36);                             /* LDA $36 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(zanac_main_weapon_level);            /* LDA zanac_main_weapon_level (0...5) */
    LSR_A();                                 /* LSR */
    CLC();                                   /* CLC */
    ADCI(13);                                /* ADC #13 (13 single, 14 double, 15 triple) */
    j_apu_play();                            /* JSR j_apu_play */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDA(zanac_main_weapon_speed_y);          /* LDA zanac_main_weapon_speed_y (Y speed) */
    STA(ABSX(byte_612));                     /* STA $612,X */

loc_19068:                                    /* $9068 */
    process_enemy_flags_sprites();            /* JSR process_enemy_flags_sprites */
    sub_181A7();                              /* JMP sub_181A7 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $906E  sub_1906E - count down $768 and $5E
 *
 *  Original: src/bank6.asm:3430-3442
 *      DEC $768
 *      BNE locret_1907E
 *      LDA #$3C
 *      STA $768
 *      DEC $5E
 *      LDA $5E
 *      CMP #$FF
 *  locret_1907E:
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1906E(void)                          /* $906E */
{
    DEC(0x0768);                             /* DEC $768 */
    BNE(locret_1907E);                        /* BNE locret_1907E */
    LDAI(0x3C);                              /* LDA #$3C */
    STA(0x0768);                             /* STA $768 */
    DEC(byte_5E);                            /* DEC $5E */
    LDA(byte_5E);                            /* LDA $5E */
    CMPI(0xFF);                              /* CMP #$FF (result of the caller's BNE test) */

locret_1907E:                                 /* $907E */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $907F  enemy_type_3 - one of the special-weapon projectiles
 *
 *  Original: src/bank6.asm:3448-3529
 *      LDA $52C / BMI special_weapon_process / ORA #$80 / STA $52C /
 *      LDA $60 / CMP #5 / BNE loc_19099 / LDA special_weapon_type / CMP #5 /
 *      BNE loc_19099 / LDA #$18 / BNE loc_1909F
 *  loc_19099:
 *      LDA #6 / CMP special_weapon_type / BEQ loc_190A2
 *  loc_1909F:
 *      JSR j_apu_play
 *  loc_190A2:
 *      LDA special_weapon_type / ASL / ASL / ASL / TAY / LDA byte_190F2,Y /
 *      STA $664 / LDA unk_190F3,Y / STA $594 / TYA / CLC / ADC $61 / TAY /
 *      LDA unk_190F4,Y / STA $57A / LDA unk_190F7,Y / STA $616 / STA $74E /
 *      LDA special_weapon_type / JSR j_switch_case
 *
 *  The table after the JSR (8 words at $90CD) is passed by SWITCH_CASE.
 * ------------------------------------------------------------------------ */
void enemy_type_3(void)                       /* $907F */
{
    LDA(byte_52C);                            /* LDA $52C */
    BMI(special_weapon_process);              /* BMI special_weapon_process */
    ORAI(0x80);                               /* ORA #$80 */
    STA(byte_52C);                            /* STA $52C */
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BNE(loc_19099);                           /* BNE loc_19099 */
    LDA(special_weapon_type);                 /* LDA special_weapon_type */
    CMPI(5);                                  /* CMP #5 */
    BNE(loc_19099);                           /* BNE loc_19099 */
    LDAI(0x18);                               /* LDA #$18 */
    BNE(loc_1909F);                           /* BNE loc_1909F */

loc_19099:                                    /* $9099 */
    LDAI(6);                                  /* LDA #6 */
    CMP(special_weapon_type);                 /* CMP special_weapon_type */
    BEQ(loc_190A2);                           /* BEQ loc_190A2 */

loc_1909F:                                    /* $909F */
    j_apu_play();                             /* JSR j_apu_play */

loc_190A2:                                    /* $90A2 */
    LDA(special_weapon_type);                 /* LDA special_weapon_type */
    ASL_A();                                  /* ASL */
    ASL_A();                                  /* ASL */
    ASL_A();                                  /* ASL */
    TAY();                                    /* TAY */
    LDA(ABSY(byte_190F2));                    /* LDA byte_190F2,Y */
    STA(0x0664);                              /* STA $664 */
    LDA(ABSY(unk_190F3));                     /* LDA unk_190F3,Y */
    STA(0x0594);                              /* STA $594 */
    TYA();                                    /* TYA */
    CLC();                                    /* CLC */
    ADC(0x0061);                              /* ADC $61 */
    TAY();                                    /* TAY */
    LDA(ABSY(unk_190F4));                     /* LDA unk_190F4,Y */
    STA(0x057A);                              /* STA $57A */
    LDA(ABSY(unk_190F7));                     /* LDA unk_190F7,Y */
    STA(0x0616);                              /* STA $616 */
    STA(0x074E);                              /* STA $74E */
    LDA(special_weapon_type);                 /* LDA special_weapon_type */
    /* $90CA: JSR j_switch_case ; table of 8 words at $90CD:
     *   39 91  A0 91  E5 91  5B 92  F0 92  5B 93  0F 94  4C 91 */
    SWITCH_CASE(0x90CD);                      /* JSR j_switch_case */
    RTS();                                   /* RTS */

special_weapon_process:                       /* $90DD - BMI target */
    special_weapon_process();                 /* BMI special_weapon_process */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $90DD  special_weapon_process - works every frame and serves special
 *         weapon projectiles
 *
 *  Original: src/bank6.asm:3536-3547
 *      LDA special_weapon_type
 *      JSR j_switch_case
 *
 *  The table after the JSR (8 words at $90E2) is passed by SWITCH_CASE.
 * ------------------------------------------------------------------------ */
void special_weapon_process(void)             /* $90DD */
{
    LDA(special_weapon_type);                 /* LDA special_weapon_type */
    /* $90DF: JSR j_switch_case ; table of 8 words at $90E2:
     *   9A 91  71 91  EA 91  77 92  2E 93  71 93  2E 94  69 91 */
    SWITCH_CASE(0x90E2);                      /* JSR j_switch_case */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $913A  sub_1913A - type 0 (All Range Weapon) setup
 *
 *  Original: src/bank6.asm:3634-3644
 *      LDA $782
 *      TAY
 *      LDA $8E7D,Y
 *      LDY $60
 *      CPY #4
 *      BCC loc_19164
 *      LDY #3
 *      STY $5F
 *      BNE loc_19164
 *
 *  loc_19164 ($9164) is the shared tail inside sub_1914D: this procedure has no
 *  RTS of its own, both exits branch into it.  It is reproduced here with a
 *  comment pointing at the shared label (PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void sub_1913A(void)                          /* $913A */
{
    LDA(byte_782);                            /* LDA $782 */
    TAY();                                    /* TAY */
    LDA(ABSY(unk_18E7D));                     /* LDA $8E7D,Y */
    LDY(unk_60);                              /* LDY $60 */
    CPYI(4);                                  /* CPY #4 */
    BCC(loc_19164);                           /* BCC loc_19164 */
    LDYI(3);                                  /* LDY #3 */
    STY(0x005F);                              /* STY $5F */
    BNE(loc_19164);                           /* BNE loc_19164 */

loc_19164:                                    /* $9164 - the shared tail inside sub_1914D */
    set_speed_by_slope();                     /* JSR set_speed_by_slope */
    special_weapon_process();                 /* JMP special_weapon_process */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $914D  sub_1914D - type 7 (High Speed) setup
 *
 *  Original: src/bank6.asm:3650-3668
 *      LDA $60 / CMP #3 / BCC loc_1915D / LDA #$40 / STA $57A / LDA #$C3 /
 *      STA $74E
 *  loc_1915D:
 *      LDA $782 / TAY / LDA unk_191DD,Y
 *  loc_19164:
 *      JSR set_speed_by_slope / JMP special_weapon_process
 *
 *  loc_19164 is the shared tail also entered by sub_1913A; here it is only
 *  reached by fall-through, so it stays a comment instead of a C label.
 * ------------------------------------------------------------------------ */
void sub_1914D(void)                          /* $914D */
{
    LDA(unk_60);                              /* LDA $60 */
    CMPI(3);                                  /* CMP #3 */
    BCC(loc_1915D);                           /* BCC loc_1915D */
    LDAI(0x40);                               /* LDA #$40 */
    STA(0x057A);                              /* STA $57A */
    LDAI(0xC3);                               /* LDA #$C3 */
    STA(0x074E);                              /* STA $74E */

loc_1915D:                                    /* $915D */
    LDA(byte_782);                            /* LDA $782 */
    TAY();                                    /* TAY */
    LDA(ABSY(unk_191DD));                     /* LDA unk_191DD,Y */

    /* loc_19164 ($9164): shared tail, entered by sub_1913A */
    set_speed_by_slope();                     /* JSR set_speed_by_slope */
    special_weapon_process();                 /* JMP special_weapon_process */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $916A  special_weapon_type_7 - high speed
 *
 *  Original: src/bank6.asm:3674-3678
 *      JSR sub_1906E
 *      BNE special_weapon_type_0
 *      JMP sub_1948B
 * ------------------------------------------------------------------------ */
void special_weapon_type_7(void)              /* $916A */
{
    sub_1906E();                              /* JSR sub_1906E */
    BNE(special_weapon_type_0);               /* BNE special_weapon_type_0 */
    sub_1948B();                              /* JMP sub_1948B */
    RTS();                                   /* RTS */

special_weapon_type_0:                        /* $919B - BNE target */
    special_weapon_type_0();                  /* BNE special_weapon_type_0 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9172  special_weapon_type_1 - straight crusher
 *
 *  Original: src/bank6.asm:3684-3706
 *      LDA $60 / CMP #5 / BNE loc_1918F / LDA $6B2 /
 *      JSR set_speed_by_slope / LDA $5FC / CLC / ADC #$80 / STA $5FC /
 *      BCS loc_1918C / DEC $616
 *  loc_1918C:
 *      DEC $6B2
 *  loc_1918F:
 *      LDA $57A / CMP #$40 / BEQ special_weapon_type_0 / EOR #1 / STA $57A
 *  (no RTS: falls through into special_weapon_type_0)
 * ------------------------------------------------------------------------ */
void special_weapon_type_1(void)              /* $9172 */
{
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BNE(loc_1918F);                           /* BNE loc_1918F */
    LDA(0x06B2);                              /* LDA $6B2 */
    set_speed_by_slope();                     /* JSR set_speed_by_slope */
    LDA(0x05FC);                              /* LDA $5FC */
    CLC();                                    /* CLC */
    ADCI(0x80);                               /* ADC #$80 */
    STA(0x05FC);                              /* STA $5FC */
    BCS(loc_1918C);                           /* BCS loc_1918C */
    DEC(0x0616);                              /* DEC $616 */

loc_1918C:                                    /* $918C */
    DEC(0x06B2);                              /* DEC $6B2 */

loc_1918F:                                    /* $918F */
    LDA(0x057A);                              /* LDA $57A */
    CMPI(0x40);                               /* CMP #$40 */
    BEQ(special_weapon_type_0);               /* BEQ special_weapon_type_0 */
    EORI(1);                                  /* EOR #1 */
    STA(0x057A);                              /* STA $57A */

special_weapon_type_0:                        /* $919B - BEQ target and fall-through */
    special_weapon_type_0();                  /* falls through into special_weapon_type_0 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $919B  special_weapon_type_0 - all range weapon
 *
 *  Original: src/bank6.asm:3712-3716
 *      JSR process_enemy_flags_sprites
 *      JMP sub_181A7
 * ------------------------------------------------------------------------ */
void special_weapon_type_0(void)              /* $919B */
{
    process_enemy_flags_sprites();            /* JSR process_enemy_flags_sprites */
    sub_181A7();                              /* JMP sub_181A7 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $91A1  sub_191A1 - all range weapon, homing projectile
 *
 *  Original: src/bank6.asm:3722-3757
 *      DEC $5E / LDA $60 / CMP #5 / BNE special_weapon_type_0 / LDA $560 /
 *      SEC / SBC #8 / CMP #$D0 / BCC loc_191B7 / LDA #$D0 /
 *      BNE loc_191BD
 *  loc_191B7:
 *      CMP #$20 / BCS loc_191BD / LDA #$20
 *  loc_191BD:
 *      STA $560 / LDA $546 / CMP #$68 / BCS loc_191CC / LDA #$68 /
 *      STA $546
 *  loc_191CC:
 *      LDA #3 / STA $664 / LDA #0 / STA $6B2 / LDA #$C1 / STA $74E /
 *      BNE special_weapon_type_0
 * ------------------------------------------------------------------------ */
void sub_191A1(void)                          /* $91A1 */
{
    DEC(byte_5E);                             /* DEC $5E */
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BNE(special_weapon_type_0);               /* BNE special_weapon_type_0 */
    LDA(byte_560);                            /* LDA $560 */
    SEC();                                    /* SEC */
    SBCI(8);                                  /* SBC #8 */
    CMPI(0xD0);                               /* CMP #$D0 */
    BCC(loc_191B7);                           /* BCC loc_191B7 */
    LDAI(0xD0);                               /* LDA #$D0 */
    BNE(loc_191BD);                           /* BNE loc_191BD */

loc_191B7:                                    /* $91B7 */
    CMPI(0x20);                               /* CMP #$20 */
    BCS(loc_191BD);                           /* BCS loc_191BD */
    LDAI(0x20);                               /* LDA #$20 */

loc_191BD:                                    /* $91BD */
    STA(byte_560);                            /* STA $560 */
    LDA(byte_546);                            /* LDA $546 */
    CMPI(0x68);                               /* CMP #$68 */
    BCS(loc_191CC);                           /* BCS loc_191CC */
    LDAI(0x68);                               /* LDA #$68 */
    STA(byte_546);                            /* STA $546 */

loc_191CC:                                    /* $91CC */
    LDAI(3);                                  /* LDA #3 */
    STA(0x0664);                              /* STA $664 */
    LDAI(0x00);                               /* LDA #0 */
    STA(0x06B2);                              /* STA $6B2 */
    LDAI(0xC1);                               /* LDA #$C1 */
    STA(0x074E);                              /* STA $74E */
    BNE(special_weapon_type_0);               /* BNE special_weapon_type_0 */

special_weapon_type_0:                        /* $919B - BNE target */
    special_weapon_type_0();                  /* BNE special_weapon_type_0 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $91E6  sub_191E6 - reset the field shutter timer
 *
 *  Original: src/bank6.asm:3773-3776
 *      LDA #$3C
 *      STA $768
 *  (no RTS: falls through into special_weapon_type_2)
 * ------------------------------------------------------------------------ */
void sub_191E6(void)                          /* $91E6 */
{
    LDAI(0x3C);                               /* LDA #$3C */
    STA(0x0768);                              /* STA $768 */
    special_weapon_type_2();                  /* falls through into $91EB */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $91EB  special_weapon_type_2 - field shutter
 *
 *  Original: src/bank6.asm:3782-3835
 *      LDA $60 / CMP #5 / BNE loc_1921E / LDA #$80 / STA $5AA /
 *      JSR sub_1906E / BNE loc_191FE / JMP sub_1948B
 *  loc_191FE:
 *      LDA $768 / ORA #$80 / STA $764 / LDA #0 / STA $5F / LDA $542 /
 *      STA $546 / LDA $55C / STA $560 / LDA #$40 / STA $576,X /
 *      JMP add_enemy_meta_sprite
 *  loc_1921E:
 *      LDA $542 / SEC / SBC #8 / STA $546 / LDA $55C / STA $560 /
 *      DEC $768 / LDA $5E / LSR / LSR / LSR / LSR / CMP #8 / BCC loc_1923C /
 *      LDA #7
 *  loc_1923C:
 *      STA $10 / LDA $768 / AND #1 / ADC $10 / TAY / LDA unk_19252,Y /
 *      STA $57A / JSR add_enemy_meta_sprite / JMP sub_181A7
 * ------------------------------------------------------------------------ */
void special_weapon_type_2(void)              /* $91EB */
{
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BNE(loc_1921E);                           /* BNE loc_1921E */
    LDAI(0x80);                               /* LDA #$80 */
    STA(byte_5AA);                            /* STA $5AA */
    sub_1906E();                              /* JSR sub_1906E */
    BNE(loc_191FE);                           /* BNE loc_191FE */
    sub_1948B();                              /* JMP sub_1948B */
    RTS();                                   /* RTS */

loc_191FE:                                    /* $91FE */
    LDA(0x0768);                              /* LDA $768 */
    ORAI(0x80);                               /* ORA #$80 */
    STA(byte_764);                            /* STA $764 */
    LDAI(0x00);                               /* LDA #0 */
    STA(0x005F);                              /* STA $5F */
    LDA(byte_542);                            /* LDA $542 */
    STA(byte_546);                            /* STA $546 */
    LDA(byte_55C);                            /* LDA $55C */
    STA(byte_560);                            /* STA $560 */
    LDAI(0x40);                               /* LDA #$40 */
    STA(ABSX(byte_576));                      /* STA $576,X */
    add_enemy_meta_sprite();                  /* JMP add_enemy_meta_sprite */
    RTS();                                   /* RTS */

loc_1921E:                                    /* $921E */
    LDA(byte_542);                            /* LDA $542 */
    SEC();                                    /* SEC */
    SBCI(8);                                  /* SBC #8 */
    STA(byte_546);                            /* STA $546 */
    LDA(byte_55C);                            /* LDA $55C */
    STA(byte_560);                            /* STA $560 */
    DEC(0x0768);                              /* DEC $768 */
    LDA(byte_5E);                             /* LDA $5E */
    LSR_A();                                  /* LSR */
    LSR_A();                                  /* LSR */
    LSR_A();                                  /* LSR */
    LSR_A();                                  /* LSR */
    CMPI(8);                                  /* CMP #8 */
    BCC(loc_1923C);                           /* BCC loc_1923C */
    LDAI(7);                                  /* LDA #7 */

loc_1923C:                                    /* $923C */
    STA(byte_10);                             /* STA $10 */
    LDA(0x0768);                              /* LDA $768 */
    ANDI(1);                                  /* AND #1 */
    ADC(byte_10);                             /* ADC $10 */
    TAY();                                    /* TAY */
    LDA(ABSY(unk_19252));                     /* LDA unk_19252,Y */
    STA(0x057A);                              /* STA $57A */
    add_enemy_meta_sprite();                  /* JSR add_enemy_meta_sprite */
    sub_181A7();                              /* JMP sub_181A7 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $925C  sub_1925C - type 3 (circular) setup
 *
 *  Original: src/bank6.asm:3852-3864
 *      LDA #0 / STA $6B2 / STA $67E / LDA #$F6 / STA $6CC / LDA #$C0 /
 *      STA $698 / LDA #1 / STA $6E6 / LDA #$FF / STA $700
 *  (no RTS: falls through into special_weapon_type_3)
 * ------------------------------------------------------------------------ */
void sub_1925C(void)                          /* $925C */
{
    LDAI(0x00);                               /* LDA #0 */
    STA(0x06B2);                              /* STA $6B2 */
    STA(0x067E);                              /* STA $67E */
    LDAI(0xF6);                               /* LDA #$F6 */
    STA(0x06CC);                              /* STA $6CC */
    LDAI(0xC0);                               /* LDA #$C0 */
    STA(0x0698);                              /* STA $698 */
    LDAI(1);                                  /* LDA #1 */
    STA(0x06E6);                              /* STA $6E6 */
    LDAI(0xFF);                               /* LDA #$FF */
    STA(0x0700);                              /* STA $700 */
    special_weapon_type_3();                  /* falls through into $9278 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9278  special_weapon_type_3 - circular
 *
 *  Original: src/bank6.asm:3870-3935
 *      JSR sub_1906E / BNE loc_19280 / JMP sub_1948B
 *  loc_19280:
 *      DEC $6E6 / BNE loc_19295 / LDA #1 / STA $6E6 / INC $700 / LDA $700 /
 *      AND #$F / JSR set_speed_by_slope
 *  loc_19295:
 *      LDA $5FC / CLC / ADC $67E / STA $67E / LDA $616 / ADC $698 /
 *      STA $698 / LDA $542 / CMP #$48 / BCS loc_192B3 / LDA #$48 /
 *      BNE loc_192B9
 *  loc_192B3:
 *      CMP #$C8 / BCC loc_192B9 / LDA #$C7
 *  loc_192B9:
 *      CLC / ADC $698 / STA $546 / LDA $630 / CLC / ADC $6B2 / STA $6B2 /
 *      LDA $64A / ADC $6CC / STA $6CC / LDA $55C / CMP #$40 /
 *      BCS loc_192DE / LDA #$40 / BNE loc_192E4
 *  loc_192DE:
 *      CMP #$C8 / BCC loc_192E4 / LDA #$C7
 *  loc_192E4:
 *      CLC / ADC $6CC / STA $560 / JSR add_enemy_meta_sprite / JMP sub_181A7
 * ------------------------------------------------------------------------ */
void special_weapon_type_3(void)              /* $9278 */
{
    sub_1906E();                              /* JSR sub_1906E */
    BNE(loc_19280);                           /* BNE loc_19280 */
    sub_1948B();                              /* JMP sub_1948B */
    RTS();                                   /* RTS */

loc_19280:                                    /* $9280 */
    DEC(0x06E6);                              /* DEC $6E6 */
    BNE(loc_19295);                           /* BNE loc_19295 */
    LDAI(1);                                  /* LDA #1 */
    STA(0x06E6);                              /* STA $6E6 */
    INC(0x0700);                              /* INC $700 */
    LDA(0x0700);                              /* LDA $700 */
    ANDI(0x0F);                               /* AND #$F */
    set_speed_by_slope();                     /* JSR set_speed_by_slope */

loc_19295:                                    /* $9295 */
    LDA(0x05FC);                              /* LDA $5FC */
    CLC();                                    /* CLC */
    ADC(0x067E);                              /* ADC $67E */
    STA(0x067E);                              /* STA $67E */
    LDA(0x0616);                              /* LDA $616 */
    ADC(0x0698);                              /* ADC $698 */
    STA(0x0698);                              /* STA $698 */
    LDA(byte_542);                            /* LDA $542 */
    CMPI(0x48);                               /* CMP #$48 */
    BCS(loc_192B3);                           /* BCS loc_192B3 */
    LDAI(0x48);                               /* LDA #$48 */
    BNE(loc_192B9);                           /* BNE loc_192B9 */

loc_192B3:                                    /* $92B3 */
    CMPI(0xC8);                               /* CMP #$C8 */
    BCC(loc_192B9);                           /* BCC loc_192B9 */
    LDAI(0xC7);                               /* LDA #$C7 */

loc_192B9:                                    /* $92B9 */
    CLC();                                    /* CLC */
    ADC(0x0698);                              /* ADC $698 */
    STA(byte_546);                            /* STA $546 */
    LDA(0x0630);                              /* LDA $630 */
    CLC();                                    /* CLC */
    ADC(0x06B2);                              /* ADC $6B2 */
    STA(0x06B2);                              /* STA $6B2 */
    LDA(0x064A);                              /* LDA $64A */
    ADC(0x06CC);                              /* ADC $6CC */
    STA(0x06CC);                              /* STA $6CC */
    LDA(byte_55C);                            /* LDA $55C */
    CMPI(0x40);                               /* CMP #$40 */
    BCS(loc_192DE);                           /* BCS loc_192DE */
    LDAI(0x40);                               /* LDA #$40 */
    BNE(loc_192E4);                           /* BNE loc_192E4 */

loc_192DE:                                    /* $92DE */
    CMPI(0xC8);                               /* CMP #$C8 */
    BCC(loc_192E4);                           /* BCC loc_192E4 */
    LDAI(0xC7);                               /* LDA #$C7 */

loc_192E4:                                    /* $92E4 */
    CLC();                                    /* CLC */
    ADC(0x06CC);                              /* ADC $6CC */
    STA(byte_560);                            /* STA $560 */
    add_enemy_meta_sprite();                  /* JSR add_enemy_meta_sprite */
    sub_181A7();                              /* JMP sub_181A7 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $92F1  sub_192F1 - type 4 (vibrator) setup
 *
 *  Original: src/bank6.asm:3941-3976
 *      LDA #4 / STA $71A / LDA #$FF / STA $6E6 / LDA #$F4 / STA $64A /
 *      LDA $55C / CMP #$48 / BCS loc_1930B / LDA #$48 / BNE loc_19311
 *  loc_1930B:
 *      CMP #$CA / BCC loc_19311 / LDA #$C9
 *  loc_19311:
 *      STA $700 / CLC / ADC #$18 / STA $560 / LDA $546 / CMP #$50 /
 *      BCS loc_19326 / LDA #$50 / STA $546
 *  loc_19326:
 *      LDA #$46 / STA $74E / LDA #$3C / STA $5E
 *  (no RTS: falls through into special_weapon_type_4)
 * ------------------------------------------------------------------------ */
void sub_192F1(void)                          /* $92F1 */
{
    LDAI(4);                                  /* LDA #4 */
    STA(0x071A);                              /* STA $71A */
    LDAI(0xFF);                               /* LDA #$FF */
    STA(0x06E6);                              /* STA $6E6 */
    LDAI(0xF4);                               /* LDA #$F4 */
    STA(0x064A);                              /* STA $64A */
    LDA(byte_55C);                            /* LDA $55C */
    CMPI(0x48);                               /* CMP #$48 */
    BCS(loc_1930B);                           /* BCS loc_1930B */
    LDAI(0x48);                               /* LDA #$48 */
    BNE(loc_19311);                           /* BNE loc_19311 */

loc_1930B:                                    /* $930B */
    CMPI(0xCA);                               /* CMP #$CA */
    BCC(loc_19311);                           /* BCC loc_19311 */
    LDAI(0xC9);                               /* LDA #$C9 */

loc_19311:                                    /* $9311 */
    STA(0x0700);                              /* STA $700 */
    CLC();                                    /* CLC */
    ADCI(0x18);                               /* ADC #$18 */
    STA(byte_560);                            /* STA $560 */
    LDA(byte_546);                            /* LDA $546 */
    CMPI(0x50);                               /* CMP #$50 */
    BCS(loc_19326);                           /* BCS loc_19326 */
    LDAI(0x50);                               /* LDA #$50 */
    STA(byte_546);                            /* STA $546 */

loc_19326:                                    /* $9326 */
    LDAI(0x46);                               /* LDA #$46 */
    STA(0x074E);                              /* STA $74E */
    LDAI(0x3C);                               /* LDA #$3C */
    STA(byte_5E);                             /* STA $5E */
    special_weapon_type_4();                  /* falls through into $932F */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $932F  special_weapon_type_4 - vibrator
 *
 *  Original: src/bank6.asm:3982-4008
 *      DEC $74E / BNE loc_19339 / LDA #2 / STA $664
 *  loc_19339:
 *      LDA $560 / CMP $700 / BCS loc_1934D / LDA $64A / CLC / ADC #4 /
 *      STA $64A / JMP loc_19356
 *  loc_1934D:
 *      LDA $64A / SEC / SBC #4 / STA $64A
 *  loc_19356:
 *      JSR process_enemy_flags_sprites / JMP sub_181A7
 * ------------------------------------------------------------------------ */
void special_weapon_type_4(void)              /* $932F */
{
    DEC(0x074E);                              /* DEC $74E */
    BNE(loc_19339);                           /* BNE loc_19339 */
    LDAI(2);                                  /* LDA #2 */
    STA(0x0664);                              /* STA $664 */

loc_19339:                                    /* $9339 */
    LDA(byte_560);                            /* LDA $560 */
    CMP(0x0700);                              /* CMP $700 */
    BCS(loc_1934D);                           /* BCS loc_1934D */
    LDA(0x064A);                              /* LDA $64A */
    CLC();                                    /* CLC */
    ADCI(4);                                  /* ADC #4 */
    STA(0x064A);                              /* STA $64A */
    goto loc_19356;                           /* JMP loc_19356 */

loc_1934D:                                    /* $934D */
    LDA(0x064A);                              /* LDA $64A */
    SEC();                                    /* SEC */
    SBCI(4);                                  /* SBC #4 */
    STA(0x064A);                              /* STA $64A */

loc_19356:                                    /* $9356 */
    process_enemy_flags_sprites();            /* JSR process_enemy_flags_sprites */
    sub_181A7();                              /* JMP sub_181A7 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $935C  sub_1935C - type 5 (rewinder / laser) setup
 *
 *  Original: src/bank6.asm:4014-4024
 *      LDA #4 / STA $71A / LDA #$FF / STA $6E6 / LDA $546 / SEC / SBC #$10 /
 *      STA $7B6 / STA $7D0
 *  (no RTS: falls through into special_weapon_type_5)
 * ------------------------------------------------------------------------ */
void sub_1935C(void)                          /* $935C */
{
    LDAI(4);                                  /* LDA #4 */
    STA(0x071A);                              /* STA $71A */
    LDAI(0xFF);                               /* LDA #$FF */
    STA(0x06E6);                              /* STA $6E6 */
    LDA(byte_546);                            /* LDA $546 */
    SEC();                                    /* SEC */
    SBCI(0x10);                               /* SBC #$10 */
    STA(0x07B6);                              /* STA $7B6 */
    STA(0x07D0);                              /* STA $7D0 */
    special_weapon_type_5();                  /* falls through into $9372 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9372  special_weapon_type_5 - rewinder / laser
 *
 *  Original: src/bank6.asm:4030-4122
 *      LDA $60 / CMP #5 / BCS loc_193A6 / LDA $546 / CMP #$10 /
 *      BCS loc_19384 / LDA #$10 / STA $546
 *  loc_19384:
 *      LDA $55C / STA $560 / LDA $542 / CLC / ADC #8 / CMP $546 /
 *      BCS loc_19398 / JMP enemy_erase
 *  loc_19398:
 *      LDA $57A / EOR #1 / STA $57A / JSR process_enemy_flags_sprites /
 *      JMP sub_181A7
 *  loc_193A6:
 *      LDA $55C / SEC / SBC #8 / STA $782 / CLC / ADC #$10 / STA $79C /
 *      LDA $7B6 / CMP #$10 / BCC loc_193C9 / LDA $7D0 / SEC / SBC $7B6 /
 *      BCC loc_1940D / CMP $77 / BCC loc_193D4
 *  loc_193C9:
 *      LDA $7D0 / SEC / SBC #$C / BCC loc_1940D / STA $7D0
 *  loc_193D4:
 *      LDA $7B6 / SEC / SBC #$C / BCS loc_193DE / ADC #$10
 *  loc_193DE:
 *      STA $7B6 / STA $84 / LDA $55C / SEC / SBC #4 / STA $87 /
 *      LDA #$7E / STA $85 / LDA #0 / STA $86 / LDA #$84 / STA $18 /
 *      LDA #0 / STA $19
 *  loc_193FB:
 *      LDY #0 / JSR add_sprite / LDA $84 / CLC / ADC #$10 / STA $84 /
 *      CMP $7D0 / BCC loc_193FB / RTS
 *  loc_1940D:
 *      JMP enemy_erase
 * ------------------------------------------------------------------------ */
void special_weapon_type_5(void)              /* $9372 */
{
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BCS(loc_193A6);                           /* BCS loc_193A6 */
    LDA(byte_546);                            /* LDA $546 */
    CMPI(0x10);                               /* CMP #$10 */
    BCS(loc_19384);                           /* BCS loc_19384 */
    LDAI(0x10);                               /* LDA #$10 */
    STA(byte_546);                            /* STA $546 */

loc_19384:                                    /* $9384 */
    LDA(byte_55C);                            /* LDA $55C */
    STA(byte_560);                            /* STA $560 */
    LDA(byte_542);                            /* LDA $542 */
    CLC();                                    /* CLC */
    ADCI(8);                                  /* ADC #8 */
    CMP(byte_546);                            /* CMP $546 */
    BCS(loc_19398);                           /* BCS loc_19398 */
    enemy_erase();                            /* JMP enemy_erase */
    RTS();                                   /* RTS */

loc_19398:                                    /* $9398 */
    LDA(0x057A);                              /* LDA $57A */
    EORI(1);                                  /* EOR #1 */
    STA(0x057A);                              /* STA $57A */
    process_enemy_flags_sprites();            /* JSR process_enemy_flags_sprites */
    sub_181A7();                              /* JMP sub_181A7 */
    RTS();                                   /* RTS */

loc_193A6:                                    /* $93A6 */
    LDA(byte_55C);                            /* LDA $55C */
    SEC();                                    /* SEC */
    SBCI(8);                                  /* SBC #8 */
    STA(byte_782);                            /* STA $782 */
    CLC();                                    /* CLC */
    ADCI(0x10);                               /* ADC #$10 */
    STA(0x079C);                              /* STA $79C */
    LDA(0x07B6);                              /* LDA $7B6 */
    CMPI(0x10);                               /* CMP #$10 */
    BCC(loc_193C9);                           /* BCC loc_193C9 */
    LDA(0x07D0);                              /* LDA $7D0 */
    SEC();                                    /* SEC */
    SBC(0x07B6);                              /* SBC $7B6 */
    BCC(loc_1940D);                           /* BCC loc_1940D */
    CMP(byte_77);                             /* CMP $77 */
    BCC(loc_193D4);                           /* BCC loc_193D4 */

loc_193C9:                                    /* $93C9 */
    LDA(0x07D0);                              /* LDA $7D0 */
    SEC();                                    /* SEC */
    SBCI(0x0C);                               /* SBC #$C */
    BCC(loc_1940D);                           /* BCC loc_1940D */
    STA(0x07D0);                              /* STA $7D0 */

loc_193D4:                                    /* $93D4 */
    LDA(0x07B6);                              /* LDA $7B6 */
    SEC();                                    /* SEC */
    SBCI(0x0C);                               /* SBC #$C */
    BCS(loc_193DE);                           /* BCS loc_193DE */
    ADCI(0x10);                               /* ADC #$10 */

loc_193DE:                                    /* $93DE */
    STA(0x07B6);                              /* STA $7B6 */
    STA(byte_84);                             /* STA $84 */
    LDA(byte_55C);                            /* LDA $55C */
    SEC();                                    /* SEC */
    SBCI(4);                                  /* SBC #4 */
    STA(byte_87);                             /* STA $87 */
    LDAI(0x7E);                               /* LDA #$7E */
    STA(byte_85);                             /* STA $85 */
    LDAI(0x00);                               /* LDA #0 */
    STA(byte_86);                             /* STA $86 */
    LDAI(0x84);                               /* LDA #$84 */
    STA(byte_18);                             /* STA $18 */
    LDAI(0x00);                               /* LDA #0 */
    STA(byte_19);                             /* STA $19 */

loc_193FB:                                    /* $93FB */
    LDYI(0);                                  /* LDY #0 */
    add_sprite();                             /* JSR add_sprite */
    LDA(byte_84);                             /* LDA $84 */
    CLC();                                    /* CLC */
    ADCI(0x10);                               /* ADC #$10 */
    STA(byte_84);                             /* STA $84 */
    CMP(0x07D0);                              /* CMP $7D0 */
    BCC(loc_193FB);                           /* BCC loc_193FB */
    RTS();                                   /* RTS */

loc_1940D:                                    /* $940D */
    enemy_erase();                            /* JMP enemy_erase */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9410  sub_19410 - plasma flash projectile
 *
 *  Original: src/bank6.asm:4128-4146
 *      LDA $F5 / ORA $F6 / AND #$80 / BNE loc_1941B / JMP enemy_erase
 *  loc_1941B:
 *      LDA #6 / JSR j_apu_play / DEC $5E / LDA $60 / CMP #5 /
 *      BEQ special_weapon_type_6 / CMP #2 / BCC special_weapon_type_6 /
 *      JMP sub_1946F
 * ------------------------------------------------------------------------ */
void sub_19410(void)                          /* $9410 */
{
    LDA(pad_test1);                           /* LDA $F5 */
    ORA(pad_test2);                           /* ORA $F6 */
    ANDI(0x80);                               /* AND #$80 */
    BNE(loc_1941B);                           /* BNE loc_1941B */
    enemy_erase();                            /* JMP enemy_erase */
    RTS();                                   /* RTS */

loc_1941B:                                    /* $941B */
    LDAI(6);                                  /* LDA #6 */
    j_apu_play();                             /* JSR j_apu_play */
    DEC(byte_5E);                             /* DEC $5E */
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BEQ(special_weapon_type_6);               /* BEQ special_weapon_type_6 */
    CMPI(2);                                  /* CMP #2 */
    BCC(special_weapon_type_6);               /* BCC special_weapon_type_6 */
    sub_1946F();                              /* JMP sub_1946F */
    RTS();                                   /* RTS */

special_weapon_type_6:                        /* $942F - BEQ/BCC target */
    special_weapon_type_6();                  /* BEQ/BCC special_weapon_type_6 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $942F  special_weapon_type_6 - plasma flash
 *
 *  Original: src/bank6.asm:4152-4156
 *      JSR process_enemy_flags_sprites
 *      JMP sub_181A7
 * ------------------------------------------------------------------------ */
void special_weapon_type_6(void)              /* $942F */
{
    process_enemy_flags_sprites();            /* JSR process_enemy_flags_sprites */
    sub_181A7();                              /* JMP sub_181A7 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9435  enemy_type_19 - enemy type $13 (19): the special weapon carrier
 *
 *  Original: src/bank6.asm:4162-4167
 *      LDA #$83
 *      STA $52C
 *      LDA $5C
 *      JSR j_switch_case
 *
 *  The table after the JSR (8 words at $943F) is passed by SWITCH_CASE; it
 *  holds enemy_common, special_weapon_type_1, sub_19464, special_weapon_type_3,
 *  sub_1944F, special_weapon_type_5, sub_1946F and special_weapon_type_7.
 * ------------------------------------------------------------------------ */
void enemy_type_19(void)                      /* $9435 */
{
    LDAI(0x83);                               /* LDA #$83 */
    STA(byte_52C);                            /* STA $52C */
    LDA(special_weapon_type);                 /* LDA $5C */
    /* $943C: JSR j_switch_case ; table of 8 words at $943F:
     *   FD 82  71 91  63 94  77 92  4E 94  71 93  6E 94  69 91 */
    SWITCH_CASE(0x943F);                      /* JSR j_switch_case */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $944F  sub_1944F - special weapon countdown (vibrator)
 *
 *  Original: src/bank6.asm:4182-4197
 *      DEC $5E
 *      BEQ loc_19461
 *      LDA $5E
 *      CMP #$A
 *      BNE loc_1945E
 *      LDA #$3C
 *      STA $57A
 *  loc_1945E:
 *      JMP special_weapon_type_4
 *  loc_19461:
 *      JMP enemy_erase
 * ------------------------------------------------------------------------ */
void sub_1944F(void)                          /* $944F */
{
    DEC(byte_5E);                             /* DEC $5E */
    BEQ(loc_19461);                           /* BEQ loc_19461 */
    LDA(byte_5E);                             /* LDA $5E */
    CMPI(0x0A);                               /* CMP #$A */
    BNE(loc_1945E);                           /* BNE loc_1945E */
    LDAI(0x3C);                               /* LDA #$3C */
    STA(0x057A);                              /* STA $57A */

loc_1945E:                                    /* $945E */
    special_weapon_type_4();                  /* JMP special_weapon_type_4 */
    RTS();                                   /* RTS */

loc_19461:                                    /* $9461 */
    enemy_erase();                            /* JMP enemy_erase */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9464  sub_19464 - special weapon countdown (field shutter)
 *
 *  Original: src/bank6.asm:4203-4209
 *      DEC $5E
 *      LDA $5E
 *      CMP #$FF
 *      BEQ sub_1948B
 *      JMP special_weapon_type_2
 * ------------------------------------------------------------------------ */
void sub_19464(void)                          /* $9464 */
{
    DEC(byte_5E);                             /* DEC $5E */
    LDA(byte_5E);                             /* LDA $5E */
    CMPI(0xFF);                               /* CMP #$FF */
    BEQ(sub_1948B);                           /* BEQ sub_1948B */
    special_weapon_type_2();                  /* JMP special_weapon_type_2 */
    RTS();                                   /* RTS */

sub_1948B:                                    /* $948B - BEQ target */
    sub_1948B();                              /* BEQ sub_1948B */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $946F  sub_1946F - special weapon expiry (rewinder / plasma)
 *
 *  Original: src/bank6.asm:4215-4232
 *      LDA $60 / CMP #5 / BCC loc_19480 / LDA #$3E / JSR loc_1B2EF /
 *      JSR sub_1948B / JMP loc_19483
 *  loc_19480:
 *      JSR sub_1B2ED
 *  loc_19483:
 *      LDA #$13 / JSR j_apu_play / JMP enemy_erase
 *
 *  $9477 JSR loc_1B2EF enters sub_1B2ED at +2, skipping its `LDA #$23`, so the
 *  value pushed by loc_1B2EF's first PHA is the `#$3E` loaded just before.  C
 *  cannot enter a function in the middle: the port calls sub_1B2ED(), which
 *  reloads A with #$23 instead of keeping #$3E.  This is the only statement of
 *  this file that is not translated faithfully.
 * ------------------------------------------------------------------------ */
void sub_1946F(void)                          /* $946F */
{
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BCC(loc_19480);                           /* BCC loc_19480 */
    LDAI(0x3E);                               /* LDA #$3E */
    /* TODO: unclear - the original enters sub_1B2ED at loc_1B2EF, which C
     * cannot do; this call reloads A with #$23 where the original kept #$3E. */
    sub_1B2ED();                              /* JSR loc_1B2EF (enters sub_1B2ED at +2) */
    sub_1948B();                              /* JSR sub_1948B */
    goto loc_19483;                           /* JMP loc_19483 */

loc_19480:                                    /* $9480 */
    sub_1B2ED();                              /* JSR sub_1B2ED */

loc_19483:                                    /* $9483 */
    LDAI(0x13);                               /* LDA #$13 */
    j_apu_play();                             /* JSR j_apu_play */
    enemy_erase();                            /* JMP enemy_erase */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $948B  sub_1948B - despawn the special weapon carrier
 *
 *  Original: src/bank6.asm:4238-4244
 *      LDA #$FF
 *      STA $5C
 *      LDA #0
 *      STA $60
 *  (no RTS: falls through into sub_19493)
 * ------------------------------------------------------------------------ */
void sub_1948B(void)                          /* $948B */
{
    LDAI(0xFF);                               /* LDA #$FF */
    STA(special_weapon_type);                 /* STA $5C */
    LDAI(0x00);                               /* LDA #0 */
    STA(unk_60);                              /* STA $60 */
    sub_19493();                              /* falls through into $9493 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9493  sub_19493 - start/advance a special weapon (A = weapon type)
 *
 *  Original: src/bank6.asm:4250-4404
 *      TAY / LDA unk_19586,Y / STA $5F / LDA #$14 / STA $5E / LDA $5C /
 *      STY $5C / CMP $5C / BEQ loc_194BD / LDA $49 / SEC / SBC #8 /
 *      BCS loc_194AE / LDA #0
 *  loc_194AE:
 *      STA $49 / LDA #$28 / STA $52C / LDA #0 / STA $60 / STA $61 /
 *      BEQ loc_19515
 *  loc_194BD:
 *      LDA $52C / BEQ loc_194D2 / TXA / PHA / LDX #4 / JSR enemy_erase /
 *      PLA / TAX / CPY #3 / BNE loc_194D2 / STY $52C
 *  loc_194D2:
 *      INC $60 / LDA $60 / CMP #3 / BCC loc_194DC / LDA #2
 *  loc_194DC:
 *      STA $61 / LDA $60 / CMP #5 / BCC loc_19515 / LDA unk_19578,Y /
 *      AND $5D / BNE loc_19511 / LDA $5D / ORA unk_19578,Y / STA $5D /
 *      LDA #5 / STA $60 / LDA $5C / CMP #5 / BNE loc_19507 / LDA $77 /
 *      CLC / ADC #$20 / BCS loc_19515 / STA $77 / BNE loc_19515
 *  loc_19507:
 *      CMP #6 / BNE loc_19515 / LDA #1 / STA $5E / BNE loc_19515
 *  loc_19511:
 *      LDA #4 / STA $60
 *  loc_19515:
 *      CPY #2 / BEQ loc_19558 / CPY #1 / BEQ loc_19553 / LDA #$3C /
 *      STA $768 / CPY #7 / BEQ loc_1954E / CPY #3 / BNE locret_19557 /
 *      LDA #$FA / STA $5E / LDA $60 / LSR / STA $61 / BEQ locret_19557 /
 *      LDA $91 / LSR / LSR / BEQ locret_19557 / LDY #$18 / CMP 2 /
 *      BCC loc_1954B / JSR enemy_prng / LDY #$1B / LSR / BCS loc_1954B /
 *      LDY #$1E
 *  loc_1954B:
 *      JMP loc_19574
 *  loc_1954E:
 *      LDA #$C8 / STA $5E / RTS
 *  loc_19553:
 *      LDA #$C8 / STA $5E
 *  locret_19557:
 *      RTS
 *  loc_19558:
 *      LDY $60 / LDA unk_19580,Y / STA $5E / LDA #3 / STA $52C / LDA $60 /
 *      ASL / CLC / ADC $60 / TAY / LDA $91 / CMP #5 / BCC loc_19574 /
 *      INY / INY / INY
 *  loc_19574:
 *      TYA / JMP sub_19918
 *
 *  `CMP 2` ($953D c5 02) is a zero-page compare against $02 (byte_2).
 * ------------------------------------------------------------------------ */
void sub_19493(void)                          /* $9493 */
{
    TAY();                                    /* TAY */
    LDA(ABSY(unk_19586));                     /* LDA unk_19586,Y */
    STA(0x005F);                              /* STA $5F */
    LDAI(0x14);                               /* LDA #$14 */
    STA(byte_5E);                             /* STA $5E */
    LDA(special_weapon_type);                 /* LDA $5C */
    STY(special_weapon_type);                 /* STY $5C */
    CMP(special_weapon_type);                 /* CMP $5C */
    BEQ(loc_194BD);                           /* BEQ loc_194BD */
    LDA(byte_49);                             /* LDA $49 */
    SEC();                                    /* SEC */
    SBCI(8);                                  /* SBC #8 */
    BCS(loc_194AE);                           /* BCS loc_194AE */
    LDAI(0x00);                               /* LDA #0 */

loc_194AE:                                    /* $94AE */
    STA(byte_49);                             /* STA $49 */
    LDAI(0x28);                               /* LDA #$28 */
    STA(byte_52C);                            /* STA $52C */
    LDAI(0x00);                               /* LDA #0 */
    STA(unk_60);                              /* STA $60 */
    STA(0x0061);                              /* STA $61 */
    BEQ(loc_19515);                           /* BEQ loc_19515 */

loc_194BD:                                    /* $94BD */
    LDA(byte_52C);                            /* LDA $52C */
    BEQ(loc_194D2);                           /* BEQ loc_194D2 */
    TXA();                                    /* TXA */
    PHA();                                    /* PHA */
    LDXI(4);                                  /* LDX #4 */
    enemy_erase();                            /* JSR enemy_erase */
    PLA();                                    /* PLA */
    TAX();                                    /* TAX */
    CPYI(3);                                  /* CPY #3 */
    BNE(loc_194D2);                           /* BNE loc_194D2 */
    STY(byte_52C);                            /* STY $52C */

loc_194D2:                                    /* $94D2 */
    INC(unk_60);                              /* INC $60 */
    LDA(unk_60);                              /* LDA $60 */
    CMPI(3);                                  /* CMP #3 */
    BCC(loc_194DC);                           /* BCC loc_194DC */
    LDAI(2);                                  /* LDA #2 */

loc_194DC:                                    /* $94DC */
    STA(0x0061);                              /* STA $61 */
    LDA(unk_60);                              /* LDA $60 */
    CMPI(5);                                  /* CMP #5 */
    BCC(loc_19515);                           /* BCC loc_19515 */
    LDA(ABSY(unk_19578));                     /* LDA unk_19578,Y */
    AND(byte_5D);                             /* AND $5D */
    BNE(loc_19511);                           /* BNE loc_19511 */
    LDA(byte_5D);                             /* LDA $5D */
    ORA(ABSY(unk_19578));                     /* ORA unk_19578,Y */
    STA(byte_5D);                             /* STA $5D */
    LDAI(5);                                  /* LDA #5 */
    STA(unk_60);                              /* STA $60 */
    LDA(special_weapon_type);                 /* LDA $5C */
    CMPI(5);                                  /* CMP #5 */
    BNE(loc_19507);                           /* BNE loc_19507 */
    LDA(byte_77);                             /* LDA $77 */
    CLC();                                    /* CLC */
    ADCI(0x20);                               /* ADC #$20 */
    BCS(loc_19515);                           /* BCS loc_19515 */
    STA(byte_77);                             /* STA $77 */
    BNE(loc_19515);                           /* BNE loc_19515 */

loc_19507:                                    /* $9507 */
    CMPI(6);                                  /* CMP #6 */
    BNE(loc_19515);                           /* BNE loc_19515 */
    LDAI(1);                                  /* LDA #1 */
    STA(byte_5E);                             /* STA $5E */
    BNE(loc_19515);                           /* BNE loc_19515 */

loc_19511:                                    /* $9511 */
    LDAI(4);                                  /* LDA #4 */
    STA(unk_60);                              /* STA $60 */

loc_19515:                                    /* $9515 */
    CPYI(2);                                  /* CPY #2 */
    BEQ(loc_19558);                           /* BEQ loc_19558 */
    CPYI(1);                                  /* CPY #1 */
    BEQ(loc_19553);                           /* BEQ loc_19553 */
    LDAI(0x3C);                               /* LDA #$3C */
    STA(0x0768);                              /* STA $768 */
    CPYI(7);                                  /* CPY #7 */
    BEQ(loc_1954E);                           /* BEQ loc_1954E */
    CPYI(3);                                  /* CPY #3 */
    BNE(locret_19557);                        /* BNE locret_19557 */
    LDAI(0xFA);                               /* LDA #$FA */
    STA(byte_5E);                             /* STA $5E */
    LDA(unk_60);                              /* LDA $60 */
    LSR_A();                                  /* LSR */
    STA(0x0061);                              /* STA $61 */
    BEQ(locret_19557);                        /* BEQ locret_19557 */
    LDA(current_level);                       /* LDA $91 */
    LSR_A();                                  /* LSR */
    LSR_A();                                  /* LSR */
    BEQ(locret_19557);                        /* BEQ locret_19557 */
    LDYI(0x18);                               /* LDY #$18 */
    CMP(byte_2);                              /* CMP 2 ($02) */
    BCC(loc_1954B);                           /* BCC loc_1954B */
    ENEMY_PRNG_SP(0xF5);                             /* JSR enemy_prng */
    LDYI(0x1B);                               /* LDY #$1B */
    LSR_A();                                  /* LSR */
    BCS(loc_1954B);                           /* BCS loc_1954B */
    LDYI(0x1E);                               /* LDY #$1E */

loc_1954B:                                    /* $954B */
    goto loc_19574;                           /* JMP loc_19574 */

loc_1954E:                                    /* $954E */
    LDAI(0xC8);                               /* LDA #$C8 */
    STA(byte_5E);                             /* STA $5E */
    RTS();                                   /* RTS */

loc_19553:                                    /* $9553 */
    LDAI(0xC8);                               /* LDA #$C8 */
    STA(byte_5E);                             /* STA $5E */

locret_19557:                                 /* $9557 */
    RTS();                                   /* RTS */

loc_19558:                                    /* $9558 */
    LDY(unk_60);                              /* LDY $60 */
    LDA(ABSY(unk_19580));                     /* LDA unk_19580,Y */
    STA(byte_5E);                             /* STA $5E */
    LDAI(3);                                  /* LDA #3 */
    STA(byte_52C);                            /* STA $52C */
    LDA(unk_60);                              /* LDA $60 */
    ASL_A();                                  /* ASL */
    CLC();                                    /* CLC */
    ADC(unk_60);                              /* ADC $60 */
    TAY();                                    /* TAY */
    LDA(current_level);                       /* LDA $91 */
    CMPI(5);                                  /* CMP #5 */
    BCC(loc_19574);                           /* BCC loc_19574 */
    INY();                                    /* INY */
    INY();                                    /* INY */
    INY();                                    /* INY */

loc_19574:                                    /* $9574 */
    TYA();                                    /* TYA */
    sub_19918();                              /* JMP sub_19918 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $958E  enemy_type_68 - spawn a row of drone enemies
 *
 *  Original: src/bank6.asm:4433-4473
 *      JSR enemy_prng / AND #$7F / ADC #$20 / STA $14 / LDA $17E /
 *      JSR sub_19615 / STA $15 / LDA $17D / JSR sub_19615 / STA $16 /
 *      LDA #3 / STA $17
 *  loc_195AB:
 *      LDY $15 / LDA unk_195D9,Y / INC $15 / STA $528,X / LDA $14 /
 *      STA $55C,X / CLC / ADC #$20 / STA $14 / LDY $16 / LDA unk_195F7,Y /
 *      STA $542,X / INC $16 / DEC $17 / BEQ loc_195D6 /
 *      JSR check_enemy_overflow / BCS loc_195D6 / TYA / TAX / BCC loc_195AB
 *  loc_195D6:
 *      LDX $40 / RTS
 * ------------------------------------------------------------------------ */
void enemy_type_68(void)                      /* $958E */
{
    ENEMY_PRNG_SP(0xF5);                             /* JSR enemy_prng */
    ANDI(0x7F);                               /* AND #$7F */
    ADCI(0x20);                               /* ADC #$20 */
    STA(byte_14);                             /* STA $14 */
    LDA(0x017E);                              /* LDA $17E */
    sub_19615();                              /* JSR sub_19615 */
    STA(byte_15);                             /* STA $15 */
    LDA(0x017D);                              /* LDA $17D */
    sub_19615();                              /* JSR sub_19615 */
    STA(byte_16);                             /* STA $16 */
    LDAI(3);                                  /* LDA #3 */
    STA(byte_17);                             /* STA $17 */

loc_195AB:                                    /* $95AB */
    LDY(byte_15);                             /* LDY $15 */
    LDA(ABSY(unk_195D9));                     /* LDA unk_195D9,Y */
    INC(byte_15);                             /* INC $15 */
    STA(ABSX(byte_528));                      /* STA $528,X */
    LDA(byte_14);                             /* LDA $14 */
    STA(ABSX(byte_55C));                      /* STA $55C,X */
    CLC();                                    /* CLC */
    ADCI(0x20);                               /* ADC #$20 */
    STA(byte_14);                             /* STA $14 */
    LDY(byte_16);                             /* LDY $16 */
    LDA(ABSY(unk_195F7));                     /* LDA unk_195F7,Y */
    STA(ABSX(byte_542));                      /* STA $542,X */
    INC(byte_16);                             /* INC $16 */
    DEC(byte_17);                             /* DEC $17 */
    BEQ(loc_195D6);                           /* BEQ loc_195D6 */
    check_enemy_overflow();                   /* JSR check_enemy_overflow */
    BCS(loc_195D6);                           /* BCS loc_195D6 */
    TYA();                                    /* TYA */
    TAX();                                    /* TAX */
    BCC(loc_195AB);                           /* BCC loc_195AB */

loc_195D6:                                    /* $95D6 */
    LDX(byte_40);                             /* LDX $40 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9615  sub_19615 - A = A * 3 + bit 7 of A
 *
 *  Original: src/bank6.asm:4540-4546
 *      STA $10
 *      ASL
 *      ADC $10
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_19615(void)                          /* $9615 */
{
    STA(byte_10);                             /* STA $10 */
    ASL_A();                                  /* ASL */
    ADC(byte_10);                             /* ADC $10 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $961B  enemy_type_4_5_6_boxes - boxes (4-bullets, 5-empty, 6-power chip)
 *
 *  Original: src/bank6.asm:4553-4661
 *      LDA $528,X / BMI loc_19644 / DEC $542,X / BEQ loc_19626 / RTS
 *  loc_19626:
 *      ORA #$80 / STA $528,X / LDA #$2F / STA $576,X / LDA #2 / STA $590,X /
 *      LDA #5 / STA $764,X / LDA #$C0 / STA $5F8,X / LDA #1 / STA $660,X
 *  loc_19644:
 *      LDA $764,X / CMP #3 / BCS loc_1966C / INC $6AE,X / LDA $6AE,X /
 *      AND #1 / ASL / STA $10 / LDA $528,X / AND #$7F / ASL / ASL /
 *      ADC $10 / TAY / LDA $96CC,Y / STA $576,X / LDA $96CD,Y / STA $590,X
 *  loc_1966C:
 *      JSR process_enemy_flags_sprites / LDA #$2F / STA $576,X /
 *      JSR sub_181F6 / JSR decrease_enemy_health / LDA $8A / BNE loc_196A2 /
 *      LDA $88 / CMP #6 / BNE loc_196A2 / LDA #$3F / STA $7E6,X /
 *      LDA $764,X / CMP #3 / BCC loc_1969C / LDY #4
 *  loc_19692:
 *      TYA / PHA / JSR sub_196F3 / PLA / TAY / DEY / BNE loc_19692
 *  loc_1969C:
 *      JSR sub_196F3 / JMP enemy_erase
 *  loc_196A2:
 *      LDA $764,X / BNE locret_196DB / LDA $88 / CMP #5 / BEQ locret_196DB /
 *      CMP #4 / BEQ loc_196C3 / LDA #$BF / STA $528,X / LDA #1 /
 *      STA $590,X / LDA #$2E / STA $576,X / JMP enemy_type_63
 *  loc_196C3:
 *      LDA #38 / STA $528,X / LDA #3 / STA $77E,X /
 *      JSR check_enemy_overflow / BCS locret_196DB / LDA #5 / STA $14 /
 *      LDA #38 / JMP spawn_projectile
 *  locret_196DB:
 *      RTS
 *
 *  $96CC/$96CD are `tab_96DC - $10` (the table is labelled only at its +$10).
 * ------------------------------------------------------------------------ */
void enemy_type_4_5_6_boxes(void)             /* $961B */
{
    LDA(ABSX(byte_528));                      /* LDA $528,X */
    BMI(loc_19644);                           /* BMI loc_19644 */
    DEC(ABSX(byte_542));                      /* DEC $542,X */
    BEQ(loc_19626);                           /* BEQ loc_19626 */
    RTS();                                   /* RTS */

loc_19626:                                    /* $9626 */
    ORAI(0x80);                               /* ORA #$80 */
    STA(ABSX(byte_528));                      /* STA $528,X */
    LDAI(0x2F);                               /* LDA #$2F */
    STA(ABSX(byte_576));                      /* STA $576,X */
    LDAI(2);                                  /* LDA #2 */
    STA(ABSX(byte_590));                      /* STA $590,X */
    LDAI(5);                                  /* LDA #5 */
    STA(ABSX(byte_764));                      /* STA $764,X */
    LDAI(0xC0);                               /* LDA #$C0 */
    STA(ABSX(byte_5F8));                      /* STA $5F8,X */
    LDAI(1);                                  /* LDA #1 */
    STA(ABSX(byte_660));                      /* STA $660,X */

loc_19644:                                    /* $9644 */
    LDA(ABSX(byte_764));                      /* LDA $764,X */
    CMPI(3);                                  /* CMP #3 */
    BCS(loc_1966C);                           /* BCS loc_1966C */
    INC(ABSX(0x06AE));                        /* INC $6AE,X */
    LDA(ABSX(0x06AE));                        /* LDA $6AE,X */
    ANDI(1);                                  /* AND #1 */
    ASL_A();                                  /* ASL */
    STA(byte_10);                             /* STA $10 */
    LDA(ABSX(byte_528));                      /* LDA $528,X */
    ANDI(0x7F);                               /* AND #$7F */
    ASL_A();                                  /* ASL */
    ASL_A();                                  /* ASL */
    ADC(byte_10);                             /* ADC $10 */
    TAY();                                    /* TAY */
    LDA(ABSY(0x96CC));                        /* LDA $96CC,Y (tab_96DC-$10) */
    STA(ABSX(byte_576));                      /* STA $576,X */
    LDA(ABSY(0x96CD));                        /* LDA $96CD,Y (tab_96DC-$10+1) */
    STA(ABSX(byte_590));                      /* STA $590,X */

loc_1966C:                                    /* $966C */
    process_enemy_flags_sprites();            /* JSR process_enemy_flags_sprites */
    LDAI(0x2F);                               /* LDA #$2F */
    STA(ABSX(byte_576));                      /* STA $576,X */
    sub_181F6();                              /* JSR sub_181F6 */
    decrease_enemy_health();                  /* JSR decrease_enemy_health */
    LDA(0x008A);                              /* LDA $8A */
    BNE(loc_196A2);                           /* BNE loc_196A2 */
    LDA(0x0088);                              /* LDA $88 */
    CMPI(6);                                  /* CMP #6 */
    BNE(loc_196A2);                           /* BNE loc_196A2 */
    LDAI(0x3F);                               /* LDA #$3F */
    STA(ABSX(0x07E6));                        /* STA $7E6,X */
    LDA(ABSX(byte_764));                      /* LDA $764,X */
    CMPI(3);                                  /* CMP #3 */
    BCC(loc_1969C);                           /* BCC loc_1969C */
    LDYI(4);                                  /* LDY #4 */

loc_19692:                                    /* $9692 */
    TYA();                                    /* TYA */
    PHA();                                    /* PHA */
    sub_196F3();                              /* JSR sub_196F3 */
    PLA();                                    /* PLA */
    TAY();                                    /* TAY */
    DEY();                                    /* DEY */
    BNE(loc_19692);                           /* BNE loc_19692 */

loc_1969C:                                    /* $969C */
    sub_196F3();                              /* JSR sub_196F3 */
    enemy_erase();                            /* JMP enemy_erase */
    RTS();                                   /* RTS */

loc_196A2:                                    /* $96A2 */
    LDA(ABSX(byte_764));                      /* LDA $764,X */
    BNE(locret_196DB);                        /* BNE locret_196DB */
    LDA(0x0088);                              /* LDA $88 */
    CMPI(5);                                  /* CMP #5 */
    BEQ(locret_196DB);                        /* BEQ locret_196DB */
    CMPI(4);                                  /* CMP #4 */
    BEQ(loc_196C3);                           /* BEQ loc_196C3 (Lead) */
    LDAI(0xBF);                               /* LDA #$BF */
    STA(ABSX(byte_528));                      /* STA $528,X */
    LDAI(1);                                  /* LDA #1 */
    STA(ABSX(byte_590));                      /* STA $590,X */
    LDAI(0x2E);                               /* LDA #$2E */
    STA(ABSX(byte_576));                      /* STA $576,X */
    enemy_type_63();                          /* JMP enemy_type_63 */
    RTS();                                   /* RTS */

loc_196C3:                                    /* $96C3 (Lead) */
    LDAI(38);                                 /* LDA #38 */
    STA(ABSX(byte_528));                      /* STA $528,X */
    LDAI(3);                                  /* LDA #3 */
    STA(ABSX(byte_77E));                      /* STA $77E,X */
    check_enemy_overflow();                   /* JSR check_enemy_overflow */
    BCS(locret_196DB);                        /* BCS locret_196DB */
    LDAI(5);                                  /* LDA #5 */
    STA(byte_14);                             /* STA $14 */
    LDAI(38);                                 /* LDA #38 */
    spawn_projectile();                       /* JMP spawn_projectile */
    RTS();                                   /* RTS */

locret_196DB:                                 /* $96DB */
    RTS();                                   /* RTS */
}
