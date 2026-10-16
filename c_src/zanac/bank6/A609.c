/* bank6/A609.c - bank 6, enemy behaviour handlers, entry points $A609-$ACDE.
 *
 * Ported from src/bank6.asm lines 7053-8188 (addresses from Build/bank6.lst,
 * bytes from Build/bank6.bin).  The last procedure's body reaches $AD25.
 *
 * Deviations from a one-to-one translation, all forced by C control flow:
 *
 *  - The original branches/JMPs from one procedure into the middle of another
 *    one and shares code blocks with it: the initialisation block of
 *    enemy_type_56 at loc_1A613/loc_1A618/loc_1A638/loc_1A642 (entered from
 *    enemy_type_57/58/59), the block of enemy_type_57 at loc_1A659/loc_1A66C/
 *    loc_1A66F/loc_1A698 (entered from enemy_type_58/59), the shared tail at
 *    loc_1A740 of enemy_type_36_giza (entered from enemy_type_45), the JSR to
 *    loc_1AA96 inside sub_1AA91 (from enemy_type_45), the shared tail at
 *    loc_1A97B of enemy_type_35_small_explosion (entered from
 *    enemy_type_91_big_explosion) and the shared tail at loc_1972F of
 *    sub_196F3 ($96F3, from enemy_type_53).  A C goto cannot cross functions
 *    and those labels are not procedure entries, so each block is reproduced
 *    at the branch site with a comment naming its original address
 *    (PORTING.md section 5).
 *
 *  - For the same reason a loc_* label that is only entered from another
 *    procedure is documented with a comment but is not declared as a C label
 *    (an unused C label is a compiler warning).
 *
 *  - `BMI sub_1AA86` (a branch to a procedure entry) is a goto to a local C
 *    label of the same original name, followed by a call and RTS, which
 *    reproduces the transfer exactly.
 *
 * Data tables read by this chunk (bytes quoted from Build/bank6.bin):
 *   unk_1A74A  $A74A: 1D 1E
 *   unk_1A97E  $A97E: 06 07 08 08 07 06 05
 *   byte_1AA26 $AA26: 00 00 01 01 01 01 01 02 03 03 03 03 03 03 04 04
 *   byte_1AA36 $AA36: 0C F4 04 F5 10 06 00 F2 08 01 F3 F0 F6 04 10 05
 *   byte_1AA46 $AA46: 00 0C F0 F2 10 F8 06 12 F4 F0 0C FE 10 03 F8 08
 *   byte_1AA56 $AA56: 06 07 08 08 08 07 08 08 08 07 06 05
 *   byte_1AB90 $AB90: 02 04 03 47 01 00 01 04 05 1B 02 06 01 83 01 08 00 06
 *   unk_1AC44  $AC44: 07 09
 *   unk_1AC46  $AC46: 00 02
 *   unk_1AC83  $AC83: 09 0A 0B 0C 0D 0C 0B 0A 09 0A 0B 0C 0D 0C 0B 0A
 *               $AC93: 00 01 02 03 00 41 42 43 00 C1 C2 C3 00 81 82 83
 *                      ($AC93 = unk_1AC83+$10, the second half of the table)
 *   $8E92      ($8E91 unk_18E91 + 1): 10 0A 08 06 05 04 04 03 ...
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $A609  enemy_type_56
 *
 *  Original: src/bank6.asm:7053-7088
 *      LDA $528,X / BMI loc_1A638 / JSR spawn_top_random_x_position / LDY #4
 *  loc_1A613:
 *      LDA #$E / STA $576,X
 *  loc_1A618:
 *      LDA #5 / STA $74A,X / TYA / JSR set_speed_by_slope / LDA #3 /
 *      STA $660,X / LDA #2 / STA $590,X / LDA #$20 / STA $716,X /
 *      LDA $528,X / ORA #$80 / STA $528,X
 *  loc_1A638:
 *      LDA $612,X / BPL loc_1A642 / LDA #$82 / STA $590,X
 *  loc_1A642:
 *      JMP sub_197B4
 *
 *  loc_1A613 ($A613) is entered by enemy_type_59 and loc_1A618 ($A618) by
 *  enemy_type_57/58; those ports reproduce this block.
 * ------------------------------------------------------------------------ */
void enemy_type_56(void)                          /* $A609 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A638);                               /* BMI loc_1A638 */
    spawn_top_random_x_position();                /* JSR spawn_top_random_x_position */
    LDYI(4);                                      /* LDY #4 */

    /* $A613 loc_1A613: entry used by enemy_type_59 (fall-through here). */
    LDAI(0x0E);                                   /* LDA #$E */
    STA(ABSX(byte_576));                          /* STA $576,X */

    /* $A618 loc_1A618: entry used by enemy_type_57/58 (fall-through here). */
    LDAI(5);                                      /* LDA #5 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    TYA();                                        /* TYA */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(0x20);                                   /* LDA #$20 */
    STA(ABSX(byte_716));                          /* STA $716,X */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */

loc_1A638:                                        /* $A638 */
    LDA(ABSX(byte_612));                          /* LDA $612,X */
    BPL(loc_1A642);                               /* BPL loc_1A642 */
    LDAI(0x82);                                   /* LDA #$82 */
    STA(ABSX(byte_590));                          /* STA $590,X */

loc_1A642:                                        /* $A642 */
    sub_197B4();                                  /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A645  enemy_type_57
 *
 *  Original: src/bank6.asm:7094-7145
 *      LDA $528,X / BMI loc_1A659 / JSR sub_1876A /
 *      JSR spawn_top_random_x_position / LDA #$F / STA $576,X / LDY #4 /
 *      BNE loc_1A618
 *  loc_1A659:
 *      JSR sub_1877E / LDA $528,X / CMP #$B9 / BEQ loc_1A66C /
 *      LDA $730,X / TAY / LDA #2 / STA $542,Y
 *  loc_1A66C:
 *      DEC $716,X
 *  loc_1A66F:
 *      BNE loc_1A638 / LDA #$15 / JSR j_apu_play / JSR calculate_slope /
 *      STA $14 / DEC $14 / LDA $764,X / TAY / LDA #$3B /
 *      JSR spawn_projectile / INC $14 / LDA $528,X / CMP #$B9 /
 *      BEQ loc_1A698 / LDA $730,X / TAY / LDA #$3B / JSR spawn_projectile
 *  loc_1A698:
 *      LDA #$3B / STA $528,X / INC $14 / LDA $14 / STA $77E,X / RTS
 *
 *  loc_1A659 ($A659) is entered by enemy_type_58 and loc_1A66F ($A66F) by
 *  enemy_type_59; those ports reproduce this block.  The BNE at loc_1A66F and
 *  the BNE at $A657 go into enemy_type_56, which is reproduced at the end of
 *  this function.
 * ------------------------------------------------------------------------ */
void enemy_type_57(void)                          /* $A645 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A659);                               /* BMI loc_1A659 */
    sub_1876A();                                  /* JSR sub_1876A */
    spawn_top_random_x_position();                /* JSR spawn_top_random_x_position */
    LDAI(0x0F);                                   /* LDA #$F */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDYI(4);                                      /* LDY #4 */
    BNE(loc_1A618);                               /* BNE loc_1A618 (inside enemy_type_56) */

loc_1A659:                                        /* $A659 */
    sub_1877E();                                  /* JSR sub_1877E */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    CMPI(0xB9);                                   /* CMP #$B9 */
    BEQ(loc_1A66C);                               /* BEQ loc_1A66C */
    LDA(ABSX(0x0730));                            /* LDA $730,X */
    TAY();                                        /* TAY */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSY(byte_542));                          /* STA $542,Y */

loc_1A66C:                                        /* $A66C */
    DEC(ABSX(byte_716));                          /* DEC $716,X */

    /* $A66F loc_1A66F: entry used by enemy_type_59 (fall-through here). */
    BNE(loc_1A638);                               /* BNE loc_1A638 (inside enemy_type_56) */
    LDAI(0x15);                                   /* LDA #$15 */
    j_apu_play();                                 /* JSR j_apu_play */
    calculate_slope();                            /* JSR calculate_slope */
    STA(byte_14);                                 /* STA $14 */
    DEC(byte_14);                                 /* DEC $14 */
    LDA(ABSX(byte_764));                          /* LDA $764,X */
    TAY();                                        /* TAY */
    LDAI(0x3B);                                   /* LDA #$3B */
    spawn_projectile();                           /* JSR spawn_projectile */
    INC(byte_14);                                 /* INC $14 */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    CMPI(0xB9);                                   /* CMP #$B9 */
    BEQ(loc_1A698);                               /* BEQ loc_1A698 */
    LDA(ABSX(0x0730));                            /* LDA $730,X */
    TAY();                                        /* TAY */
    LDAI(0x3B);                                   /* LDA #$3B */
    spawn_projectile();                           /* JSR spawn_projectile */

loc_1A698:                                        /* $A698 */
    LDAI(0x3B);                                   /* LDA #$3B */
    STA(ABSX(byte_528));                          /* STA $528,X */
    INC(byte_14);                                 /* INC $14 */
    LDA(byte_14);                                 /* LDA $14 */
    STA(ABSX(byte_77E));                          /* STA $77E,X */
    RTS();                                        /* RTS */

    /* $A618 loc_1A618 (inside enemy_type_56): the tail of that procedure,
     * reproduced here because a C goto cannot cross functions. */
loc_1A618:
    LDAI(5);                                      /* LDA #5 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    TYA();                                        /* TYA */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(0x20);                                   /* LDA #$20 */
    STA(ABSX(byte_716));                          /* STA $716,X */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */

loc_1A638:                                        /* $A638 */
    LDA(ABSX(byte_612));                          /* LDA $612,X */
    BPL(loc_1A642);                               /* BPL loc_1A642 */
    LDAI(0x82);                                   /* LDA #$82 */
    STA(ABSX(byte_590));                          /* STA $590,X */

loc_1A642:                                        /* $A642 */
    sub_197B4();                                  /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A6A5  enemy_type_58
 *
 *  Original: src/bank6.asm:7151-7163
 *      LDA $528,X
 *      BMI loc_1A659
 *      JSR sub_1876A
 *      TYA
 *      STA $730,X
 *      JSR sub_1876A
 *      JSR spawn_top_random_x_position
 *      LDA #$10
 *      STA $576,X
 *      LDY #4
 *      JMP loc_1A618
 *
 *  loc_1A659 belongs to enemy_type_57 and loc_1A618 to enemy_type_56; both
 *  blocks are reproduced here because a C goto cannot cross functions.
 * ------------------------------------------------------------------------ */
void enemy_type_58(void)                          /* $A6A5 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A659);                               /* BMI loc_1A659 (inside enemy_type_57) */
    sub_1876A();                                  /* JSR sub_1876A */
    TYA();                                        /* TYA */
    STA(ABSX(0x0730));                            /* STA $730,X */
    sub_1876A();                                  /* JSR sub_1876A */
    spawn_top_random_x_position();                /* JSR spawn_top_random_x_position */
    LDAI(0x10);                                   /* LDA #$10 */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDYI(4);                                      /* LDY #4 */
    goto loc_1A618;                               /* JMP loc_1A618 (inside enemy_type_56) */

loc_1A618:                                        /* $A618 */
    LDAI(5);                                      /* LDA #5 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    TYA();                                        /* TYA */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(0x20);                                   /* LDA #$20 */
    STA(ABSX(byte_716));                          /* STA $716,X */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */

loc_1A638:                                        /* $A638 */
    LDA(ABSX(byte_612));                          /* LDA $612,X */
    BPL(loc_1A642);                               /* BPL loc_1A642 */
    LDAI(0x82);                                   /* LDA #$82 */
    STA(ABSX(byte_590));                          /* STA $590,X */

loc_1A642:                                        /* $A642 */
    sub_197B4();                                  /* JMP sub_197B4 */
    RTS();

    /* $A659 loc_1A659 (inside enemy_type_57): reproduced here. */
loc_1A659:
    sub_1877E();                                  /* JSR sub_1877E */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    CMPI(0xB9);                                   /* CMP #$B9 */
    BEQ(loc_1A66C);                               /* BEQ loc_1A66C */
    LDA(ABSX(0x0730));                            /* LDA $730,X */
    TAY();                                        /* TAY */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSY(byte_542));                          /* STA $542,Y */

loc_1A66C:                                        /* $A66C */
    DEC(ABSX(byte_716));                          /* DEC $716,X */

    /* $A66F loc_1A66F (fall-through here, as in enemy_type_57). */
    BNE(loc_1A638);                               /* BNE loc_1A638 */
    LDAI(0x15);                                   /* LDA #$15 */
    j_apu_play();                                 /* JSR j_apu_play */
    calculate_slope();                            /* JSR calculate_slope */
    STA(byte_14);                                 /* STA $14 */
    DEC(byte_14);                                 /* DEC $14 */
    LDA(ABSX(byte_764));                          /* LDA $764,X */
    TAY();                                        /* TAY */
    LDAI(0x3B);                                   /* LDA #$3B */
    spawn_projectile();                           /* JSR spawn_projectile */
    INC(byte_14);                                 /* INC $14 */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    CMPI(0xB9);                                   /* CMP #$B9 */
    BEQ(loc_1A698);                               /* BEQ loc_1A698 */
    LDA(ABSX(0x0730));                            /* LDA $730,X */
    TAY();                                        /* TAY */
    LDAI(0x3B);                                   /* LDA #$3B */
    spawn_projectile();                           /* JSR spawn_projectile */

loc_1A698:                                        /* $A698 */
    LDAI(0x3B);                                   /* LDA #$3B */
    STA(ABSX(byte_528));                          /* STA $528,X */
    INC(byte_14);                                 /* INC $14 */
    LDA(byte_14);                                 /* LDA $14 */
    STA(ABSX(byte_77E));                          /* STA $77E,X */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A6C1  enemy_type_59
 *
 *  Original: src/bank6.asm:7169-7176
 *      LDA $528,X
 *      BMI loc_1A66F
 *      LDA $77E,X
 *      AND #$F
 *      TAY
 *      JMP loc_1A613
 *
 *  loc_1A66F belongs to enemy_type_57 and loc_1A613 to enemy_type_56; both
 *  blocks are reproduced here because a C goto cannot cross functions.
 * ------------------------------------------------------------------------ */
void enemy_type_59(void)                          /* $A6C1 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A66F);                               /* BMI loc_1A66F (inside enemy_type_57) */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    ANDI(0x0F);                                   /* AND #$F */
    TAY();                                        /* TAY */
    goto loc_1A613;                               /* JMP loc_1A613 (inside enemy_type_56) */

loc_1A613:                                        /* $A613 */
    LDAI(0x0E);                                   /* LDA #$E */
    STA(ABSX(byte_576));                          /* STA $576,X */

    /* $A618 loc_1A618 (fall-through here, as in enemy_type_56). */
    LDAI(5);                                      /* LDA #5 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    TYA();                                        /* TYA */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(0x20);                                   /* LDA #$20 */
    STA(ABSX(byte_716));                          /* STA $716,X */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */

loc_1A638:                                        /* $A638 */
    LDA(ABSX(byte_612));                          /* LDA $612,X */
    BPL(loc_1A642);                               /* BPL loc_1A642 */
    LDAI(0x82);                                   /* LDA #$82 */
    STA(ABSX(byte_590));                          /* STA $590,X */

loc_1A642:                                        /* $A642 */
    sub_197B4();                                  /* JMP sub_197B4 */
    RTS();

    /* $A66F loc_1A66F (inside enemy_type_57): reproduced here. */
loc_1A66F:
    BNE(loc_1A638);                               /* BNE loc_1A638 */
    LDAI(0x15);                                   /* LDA #$15 */
    j_apu_play();                                 /* JSR j_apu_play */
    calculate_slope();                            /* JSR calculate_slope */
    STA(byte_14);                                 /* STA $14 */
    DEC(byte_14);                                 /* DEC $14 */
    LDA(ABSX(byte_764));                          /* LDA $764,X */
    TAY();                                        /* TAY */
    LDAI(0x3B);                                   /* LDA #$3B */
    spawn_projectile();                           /* JSR spawn_projectile */
    INC(byte_14);                                 /* INC $14 */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    CMPI(0xB9);                                   /* CMP #$B9 */
    BEQ(loc_1A698);                               /* BEQ loc_1A698 */
    LDA(ABSX(0x0730));                            /* LDA $730,X */
    TAY();                                        /* TAY */
    LDAI(0x3B);                                   /* LDA #$3B */
    spawn_projectile();                           /* JSR spawn_projectile */

loc_1A698:                                        /* $A698 */
    LDAI(0x3B);                                   /* LDA #$3B */
    STA(ABSX(byte_528));                          /* STA $528,X */
    INC(byte_14);                                 /* INC $14 */
    LDA(byte_14);                                 /* LDA $14 */
    STA(ABSX(byte_77E));                          /* STA $77E,X */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A6CF  enemy_type_64_random - random enemy generator
 *
 *  Original: src/bank6.asm:7183-7206
 *      JSR enemy_prng / AND #3 / STA $10 / LDA $49 / LSR / LSR / CLC /
 *      ADC $10 / CMP #$3C / BCC loc_1A6E3 / LDA #$3B
 *  loc_1A6E3:
 *      TAY / JSR j_get_random_enemy / CMP #$40 / BNE loc_1A6ED / LDA #$2C
 *  loc_1A6ED:
 *      STA $528,X / RTS
 * ------------------------------------------------------------------------ */
void enemy_type_64_random(void)                   /* $A6CF */
{
    enemy_prng();                                 /* JSR enemy_prng */
    ANDI(0x03);                                   /* AND #3 */
    STA(byte_10);                                 /* STA $10 */
    LDA(byte_49);                                 /* LDA $49 */
    LSR_A();                                      /* LSR */
    LSR_A();                                      /* LSR */
    CLC();                                        /* CLC */
    ADC(byte_10);                                 /* ADC $10 */
    CMPI(0x3C);                                   /* CMP #$3C */
    BCC(loc_1A6E3);                               /* BCC loc_1A6E3 */
    LDAI(0x3B);                                   /* LDA #$3B */

loc_1A6E3:                                        /* $A6E3 */
    TAY();                                        /* TAY */
    j_get_random_enemy();                         /* JSR j_get_random_enemy */
    CMPI(0x40);                                   /* CMP #$40 */
    BNE(loc_1A6ED);                               /* BNE loc_1A6ED */
    LDAI(0x2C);                                   /* LDA #$2C */

loc_1A6ED:                                        /* $A6ED */
    STA(ABSX(byte_528));                          /* STA $528,X */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A6F1  enemy_type_36_giza - hard floater, 16 shots to destroy
 *
 *  Original: src/bank6.asm:7213-7260
 *      LDA $528,X / BMI loc_1A72A / ORA #$80 / STA $528,X / LDA $5AA,X /
 *      BEQ loc_1A70E / LDA #1 / STA $74A,X / LDA $77E,X /
 *      JSR set_speed_by_slope / JMP loc_1A720
 *  loc_1A70E:
 *      JSR spawn_top_random_x_position / LDA #$10 / STA $764,X / LDA #1 /
 *      STA $660,X / LDA #$80 / STA $5F8,X
 *  loc_1A720:
 *      LDA #2 / STA $590,X / LDA #$10 / STA $764,X
 *  loc_1A72A:
 *      LDA $5AA,X / BNE loc_1A740 / INC $6AE,X / LDA $6AE,X / LSR / LSR /
 *      AND #1 / TAY / LDA unk_1A74A,Y / STA $576,X
 *  loc_1A740:
 *      JSR process_enemy_flags_sprites / JSR sub_181F6 /
 *      JSR decrease_enemy_health / RTS
 *
 *  loc_1A740 ($A740) is the shared tail used by enemy_type_45 as well.
 * ------------------------------------------------------------------------ */
void enemy_type_36_giza(void)                     /* $A6F1 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A72A);                               /* BMI loc_1A72A */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDA(ABSX(byte_5AA));                          /* LDA $5AA,X */
    BEQ(loc_1A70E);                               /* BEQ loc_1A70E */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    goto loc_1A720;                               /* JMP loc_1A720 */

loc_1A70E:                                        /* $A70E */
    spawn_top_random_x_position();                /* JSR spawn_top_random_x_position */
    LDAI(0x10);                                   /* LDA #$10 */
    STA(ABSX(byte_764));                          /* STA $764,X */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(0x80);                                   /* LDA #$80 */
    STA(ABSX(byte_5F8));                          /* STA $5F8,X */

loc_1A720:                                        /* $A720 */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(0x10);                                   /* LDA #$10 */
    STA(ABSX(byte_764));                          /* STA $764,X */

loc_1A72A:                                        /* $A72A */
    LDA(ABSX(byte_5AA));                          /* LDA $5AA,X */
    BNE(loc_1A740);                               /* BNE loc_1A740 */
    INC(ABSX(0x06AE));                            /* INC $6AE,X */
    LDA(ABSX(0x06AE));                            /* LDA $6AE,X */
    LSR_A();                                      /* LSR */
    LSR_A();                                      /* LSR */
    ANDI(1);                                      /* AND #1 */
    TAY();                                        /* TAY */
    LDA(ABSY(unk_1A74A));                         /* LDA unk_1A74A,Y */
    STA(ABSX(byte_576));                          /* STA $576,X */

loc_1A740:                                        /* $A740 - also entered from enemy_type_45 */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    sub_181F6();                                  /* JSR sub_181F6 */
    decrease_enemy_health();                      /* JSR decrease_enemy_health */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A74C  enemy_type_44_yellow_duster
 *
 *  Original: src/bank6.asm:7270-7290
 *      LDA $528,X / BMI loc_1A776 / ORA #$80 / STA $528,X /
 *      JSR spawn_top_random_x_position / JSR enemy_prng / AND #3 /
 *      STA $74A,X / INC $74A,X / JSR calculate_slope_and_speed / LDA #3 /
 *      STA $660,X / LDA #$2C / STA $576,X / LDA #3 / STA $590,X
 *  loc_1A776:
 *      JMP sub_197B4
 * ------------------------------------------------------------------------ */
void enemy_type_44_yellow_duster(void)            /* $A74C */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A776);                               /* BMI loc_1A776 */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    spawn_top_random_x_position();                /* JSR spawn_top_random_x_position */
    enemy_prng();                                 /* JSR enemy_prng */
    ANDI(0x03);                                   /* AND #3 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    INC(ABSX(0x074A));                            /* INC $74A,X */
    calculate_slope_and_speed();                  /* JSR calculate_slope_and_speed */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(0x2C);                                   /* LDA #$2C */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_590));                          /* STA $590,X */

loc_1A776:                                        /* $A776 */
    sub_197B4();                                  /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A779  enemy_type_61_sart
 *
 *  Original: src/bank6.asm:7296-7398
 *      LDA $528,X / BMI loc_1A7C2 / ORA #$80 / STA $528,X / LDA $55C /
 *      CMP #$80 / LDY #$30 / BCS loc_1A78E / LDY #$D0
 *  loc_1A78E:
 *      TYA / STA $55C,X / LDA #1 / STA $612,X / LDA #$80 / STA $5F8,X /
 *      LDA #2 / STA $590,X / LDA #1 / STA $660,X / LDA $5B / INC $5B /
 *      AND #7 / STA $764,X / LDA #$20 / STA $74A,X / LDA $5A / LDY #$2B /
 *      CMP #5 / BCC loc_1A7BE / LDY #$2D
 *  loc_1A7BE:
 *      TYA / STA $576,X
 *  loc_1A7C2:
 *      LDA $542,X / CMP #$81 / BNE loc_1A7DB / LDA #0 / STA $660,X /
 *      DEC $74A,X / BNE loc_1A7DB / INC $660,X / LDA #$FC / STA $612,X
 *  loc_1A7DB:
 *      JSR process_enemy_flags_sprites / LDA $576,X / CMP #$2D /
 *      BNE loc_1A7FA / LDA $764,X / ASL / STA $85 / LDA #$F1 / STA $84 /
 *      LDA #3 / STA $86 / LDA #$FC / STA $87 /
 *      JSR add_next_sprite_from_zeropage
 *  loc_1A7FA:
 *      JSR sub_181F6 / LDA $528,X / BPL loc_1A803 / RTS
 *  loc_1A803:
 *      JSR sub_18796 / JSR sub_18796 / LDA $55 / AND #$3F / STA $10 /
 *      LDA $17E / AND #$3F / CMP $10 / BNE loc_1A821 / JSR sub_C02A /
 *      LDA #$3E / STA $528,X / RTS
 *  loc_1A821:
 *      LDA $576,X / CMP #$2D / BNE locret_1A835 / JSR sub_C02A /
 *      LDA #$53 / STA $528,X / LDA #0 / STA $5AA,X
 *  locret_1A835:
 *      RTS
 * ------------------------------------------------------------------------ */
void enemy_type_61_sart(void)                     /* $A779 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A7C2);                               /* BMI loc_1A7C2 */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDA(byte_55C);                                /* LDA $55C */
    CMPI(0x80);                                   /* CMP #$80 */
    LDYI(0x30);                                   /* LDY #$30 */
    BCS(loc_1A78E);                               /* BCS loc_1A78E */
    LDYI(0xD0);                                   /* LDY #$D0 */

loc_1A78E:                                        /* $A78E */
    TYA();                                        /* TYA */
    STA(ABSX(byte_55C));                          /* STA $55C,X */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_612));                          /* STA $612,X */
    LDAI(0x80);                                   /* LDA #$80 */
    STA(ABSX(byte_5F8));                          /* STA $5F8,X */
    LDAI(2);                                      /* LDA #2 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDA(unk_5B);                                  /* LDA $5B */
    INC(unk_5B);                                  /* INC $5B */
    ANDI(7);                                      /* AND #7 */
    STA(ABSX(byte_764));                          /* STA $764,X */
    LDAI(0x20);                                   /* LDA #$20 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    LDA(byte_5A);                                 /* LDA $5A */
    LDYI(0x2B);                                   /* LDY #$2B */
    CMPI(5);                                      /* CMP #5 */
    BCC(loc_1A7BE);                               /* BCC loc_1A7BE */
    LDYI(0x2D);                                   /* LDY #$2D */

loc_1A7BE:                                        /* $A7BE */
    TYA();                                        /* TYA */
    STA(ABSX(byte_576));                          /* STA $576,X */

loc_1A7C2:                                        /* $A7C2 */
    LDA(ABSX(byte_542));                          /* LDA $542,X */
    CMPI(0x81);                                   /* CMP #$81 */
    BNE(loc_1A7DB);                               /* BNE loc_1A7DB */
    LDAI(0);                                      /* LDA #0 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    DEC(ABSX(0x074A));                            /* DEC $74A,X */
    BNE(loc_1A7DB);                               /* BNE loc_1A7DB */
    INC(ABSX(byte_660));                          /* INC $660,X */
    LDAI(0xFC);                                   /* LDA #$FC */
    STA(ABSX(byte_612));                          /* STA $612,X */

loc_1A7DB:                                        /* $A7DB */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    LDA(ABSX(byte_576));                          /* LDA $576,X */
    CMPI(0x2D);                                   /* CMP #$2D */
    BNE(loc_1A7FA);                               /* BNE loc_1A7FA */
    LDA(ABSX(byte_764));                          /* LDA $764,X */
    ASL_A();                                      /* ASL */
    STA(byte_85);                                 /* STA $85 */
    LDAI(0xF1);                                   /* LDA #$F1 */
    STA(byte_84);                                 /* STA $84 */
    LDAI(3);                                      /* LDA #3 */
    STA(byte_86);                                 /* STA $86 */
    LDAI(0xFC);                                   /* LDA #$FC */
    STA(byte_87);                                 /* STA $87 */
    add_next_sprite_from_zeropage();              /* JSR add_next_sprite_from_zeropage */

loc_1A7FA:                                        /* $A7FA */
    sub_181F6();                                  /* JSR sub_181F6 */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BPL(loc_1A803);                               /* BPL loc_1A803 */
    RTS();                                        /* RTS */

loc_1A803:                                        /* $A803 */
    sub_18796();                                  /* JSR sub_18796 */
    sub_18796();                                  /* JSR sub_18796 */
    LDA(byte_55);                                 /* LDA $55 */
    ANDI(0x3F);                                   /* AND #$3F */
    STA(byte_10);                                 /* STA $10 */
    LDA(0x017E);                                  /* LDA $17E */
    ANDI(0x3F);                                   /* AND #$3F */
    CMP(byte_10);                                 /* CMP $10 */
    BNE(loc_1A821);                               /* BNE loc_1A821 */
    sub_C02A();                                   /* JSR sub_C02A */
    LDAI(0x3E);                                   /* LDA #$3E */
    STA(ABSX(byte_528));                          /* STA $528,X */
    RTS();                                        /* RTS */

loc_1A821:                                        /* $A821 */
    LDA(ABSX(byte_576));                          /* LDA $576,X */
    CMPI(0x2D);                                   /* CMP #$2D */
    BNE(locret_1A835);                            /* BNE locret_1A835 */
    sub_C02A();                                   /* JSR sub_C02A */
    LDAI(0x53);                                   /* LDA #$53 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDAI(0);                                      /* LDA #0 */
    STA(ABSX(byte_5AA));                          /* STA $5AA,X */

locret_1A835:                                     /* $A835 */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A836  enemy_type_67_valkyrie
 *
 *  Original: src/bank6.asm:7405-7490
 *      LDA $528,X / BMI loc_1A86F / ORA #$80 / STA $528,X /
 *      JSR spawn_top_random_x_position / LDA $55C,X / PHA /
 *      JSR spawn_top_random_x_position / PLA / STA $542,X / LDA #$1D /
 *      STA $576,X / LDA #5 / STA $764,X / LDA #3 / STA $74A,X /
 *      STA $660,X / LDA #$78 / STA $6E2,X / LDA #$32 / STA $716,X /
 *      LDA #$1E / STA $6FC,X
 *  loc_1A86F:
 *      LDA $5AA,X / BMI loc_1A87C / LDA $6E2,X / AND #3 / STA $590,X
 *  loc_1A87C:
 *      LDA $6E2,X / LSR / LSR / LSR / AND #1 / CLC / ADC #$1D /
 *      STA $576,X / JSR update_chase_counters / BCC loc_1A899 / LDA #1 /
 *      STA $590,X / LDA #$80 / STA $5AA,X
 *  loc_1A899:
 *      LDA $5AA,X / BMI loc_1A8A1 / JMP add_enemy_meta_sprite
 *  loc_1A8A1:
 *      JSR sub_181F6 / LDA $528,X / BMI loc_1A8CD / LDA #$14 /
 *      JSR j_apu_play / LDA $8A / CMP #4 / BNE loc_1A8C2 / LDA $5C /
 *      CMP #2 / BNE loc_1A8C2 / LDA #$C3 / STA $528,X /
 *      JMP add_enemy_meta_sprite
 *  loc_1A8C2:
 *      DEC $764,X / BNE loc_1A8C8 / RTS
 *  loc_1A8C8:
 *      LDA #$C3 / STA $528,X
 *  loc_1A8CD:
 *      JSR process_enemy_flags_sprites / RTS
 * ------------------------------------------------------------------------ */
void enemy_type_67_valkyrie(void)                 /* $A836 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A86F);                               /* BMI loc_1A86F */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    spawn_top_random_x_position();                /* JSR spawn_top_random_x_position */
    LDA(ABSX(byte_55C));                          /* LDA $55C,X */
    PHA();                                        /* PHA */
    spawn_top_random_x_position();                /* JSR spawn_top_random_x_position */
    PLA();                                        /* PLA */
    STA(ABSX(byte_542));                          /* STA $542,X */
    LDAI(0x1D);                                   /* LDA #$1D */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDAI(5);                                      /* LDA #5 */
    STA(ABSX(byte_764));                          /* STA $764,X */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(0x78);                                   /* LDA #$78 */
    STA(ABSX(byte_6E2));                          /* STA $6E2,X */
    LDAI(0x32);                                   /* LDA #$32 */
    STA(ABSX(byte_716));                          /* STA $716,X */
    LDAI(0x1E);                                   /* LDA #$1E */
    STA(ABSX(byte_6FC));                          /* STA $6FC,X */

loc_1A86F:                                        /* $A86F */
    LDA(ABSX(byte_5AA));                          /* LDA $5AA,X */
    BMI(loc_1A87C);                               /* BMI loc_1A87C */
    LDA(ABSX(byte_6E2));                          /* LDA $6E2,X */
    ANDI(3);                                      /* AND #3 */
    STA(ABSX(byte_590));                          /* STA $590,X */

loc_1A87C:                                        /* $A87C */
    LDA(ABSX(byte_6E2));                          /* LDA $6E2,X */
    LSR_A();                                      /* LSR */
    LSR_A();                                      /* LSR */
    LSR_A();                                      /* LSR */
    ANDI(1);                                      /* AND #1 */
    CLC();                                        /* CLC */
    ADCI(0x1D);                                   /* ADC #$1D */
    STA(ABSX(byte_576));                          /* STA $576,X */
    update_chase_counters();                      /* JSR update_chase_counters */
    BCC(loc_1A899);                               /* BCC loc_1A899 */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(0x80);                                   /* LDA #$80 */
    STA(ABSX(byte_5AA));                          /* STA $5AA,X */

loc_1A899:                                        /* $A899 */
    LDA(ABSX(byte_5AA));                          /* LDA $5AA,X */
    BMI(loc_1A8A1);                               /* BMI loc_1A8A1 */
    add_enemy_meta_sprite();                      /* JMP add_enemy_meta_sprite */
    RTS();

loc_1A8A1:                                        /* $A8A1 */
    sub_181F6();                                  /* JSR sub_181F6 */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A8CD);                               /* BMI loc_1A8CD */
    LDAI(0x14);                                   /* LDA #$14 */
    j_apu_play();                                 /* JSR j_apu_play */
    LDA(0x008A);                                  /* LDA $8A */
    CMPI(4);                                      /* CMP #4 */
    BNE(loc_1A8C2);                               /* BNE loc_1A8C2 */
    LDA(special_weapon_type);                     /* LDA $5C */
    CMPI(2);                                      /* CMP #2 */
    BNE(loc_1A8C2);                               /* BNE loc_1A8C2 */
    LDAI(0xC3);                                   /* LDA #$C3 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    add_enemy_meta_sprite();                      /* JMP add_enemy_meta_sprite */
    RTS();

loc_1A8C2:                                        /* $A8C2 */
    DEC(ABSX(byte_764));                          /* DEC $764,X */
    BNE(loc_1A8C8);                               /* BNE loc_1A8C8 */
    RTS();                                        /* RTS */

loc_1A8C8:                                        /* $A8C8 */
    LDAI(0xC3);                                   /* LDA #$C3 */
    STA(ABSX(byte_528));                          /* STA $528,X */

loc_1A8CD:                                        /* $A8CD */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A8D1  update_chase_counters
 *
 *  Update the Zanac pursuit counters; sets carry when a coordinate update was
 *  performed.
 *
 *  Original: src/bank6.asm:7497-7513
 *      CLC / LDA $6FC,X / BEQ locret_1A8E9 / DEC $6E2,X /
 *      BNE locret_1A8E9 / LDA $716,X / STA $6E2,X /
 *      JSR calculate_slope_and_speed / DEC $6FC,X / SEC
 *  locret_1A8E9:
 *      RTS
 * ------------------------------------------------------------------------ */
void update_chase_counters(void)                  /* $A8D1 */
{
    CLC();                                        /* CLC */
    LDA(ABSX(byte_6FC));                          /* LDA $6FC,X */
    BEQ(locret_1A8E9);                            /* BEQ locret_1A8E9 */
    DEC(ABSX(byte_6E2));                          /* DEC $6E2,X */
    BNE(locret_1A8E9);                            /* BNE locret_1A8E9 */
    LDA(ABSX(byte_716));                          /* LDA $716,X */
    STA(ABSX(byte_6E2));                          /* STA $6E2,X */
    calculate_slope_and_speed();                  /* JSR calculate_slope_and_speed */
    DEC(ABSX(byte_6FC));                          /* DEC $6FC,X */
    SEC();                                        /* SEC */

locret_1A8E9:                                     /* $A8E9 */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A8EA  enemy_type_35_small_explosion - pulsating red-white small explosion
 *
 *  Original: src/bank6.asm:7520-7616
 *      LDA $528,X / BMI loc_1A96C / ORA #$80 / STA $528,X / LDA $48 / CLC /
 *      ADC #$10 / STA $48 / BCC loc_1A900 / JSR sub_18788
 *  loc_1A900:
 *      LDA $4C / CMP #$11 / BCC loc_1A90A / LDA #1 / BNE loc_1A90E
 *  loc_1A90A:
 *      TAY
 *  loc_1A90B:
 *      LDA $8E92,Y
 *  loc_1A90E:
 *      CLC / ADC $4A / STA $4A / BCC loc_1A918 / JSR sub_1879F
 *  loc_1A918:
 *      LDA $56 / CMP #8 / BCS loc_1A928 / ASL / ASL / EOR #$FF / CLC /
 *      ADC #$25 / JMP loc_1A92A
 *  loc_1A928:
 *      LDA #1
 *  loc_1A92A:
 *      CLC / ADC $4A / STA $4A / BCC loc_1A934 / JSR sub_1879F
 *  loc_1A934:
 *      LDA #0 / STA $4C / STA $56 / LDA #17 / JSR j_apu_play
 *  loc_1A93F:
 *      JSR sub_C02A / LDA #0 / STA $660,X / LDA #$40 / LDY #7 /
 *      JSR sub_1A2CB / LDA #1 / STA $590,X / LDA $7E6,X / BEQ loc_1A96C /
 *      CMP #$25 / BEQ loc_1A96C / CMP #$26 / BEQ loc_1A96C / DEC $45 /
 *      BNE loc_1A96C / LDA #$10 / STA $45 / LDA #1 / STA $46
 *  loc_1A96C:
 *      JSR sub_183BC / BCS loc_1A97B / LDA unk_1A97E,Y / STA $576,X /
 *      JSR process_enemy_flags_sprites / RTS
 *  loc_1A97B:
 *      JMP enemy_erase
 *
 *  loc_1A93F ($A93F) is also entered by enemy_type_80 (outside this chunk) and
 *  loc_1A97B ($A97B) by enemy_type_91_big_explosion, which reproduces the
 *  tail.  $8E92 is unk_18E91+1.
 * ------------------------------------------------------------------------ */
void enemy_type_35_small_explosion(void)          /* $A8EA */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A96C);                               /* BMI loc_1A96C */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDA(byte_48);                                 /* LDA $48 */
    CLC();                                        /* CLC */
    ADCI(0x10);                                   /* ADC #$10 */
    STA(byte_48);                                 /* STA $48 */
    BCC(loc_1A900);                               /* BCC loc_1A900 */
    sub_18788();                                  /* JSR sub_18788 */

loc_1A900:                                        /* $A900 */
    LDA(byte_4C);                                 /* LDA $4C */
    CMPI(0x11);                                   /* CMP #$11 */
    BCC(loc_1A90A);                               /* BCC loc_1A90A */
    LDAI(1);                                      /* LDA #1 */
    BNE(loc_1A90E);                               /* BNE loc_1A90E */

loc_1A90A:                                        /* $A90A */
    TAY();                                        /* TAY */

    /* $A90B loc_1A90B is fallen into from loc_1A90A. */
    LDA(ABSY(unk_18E91 + 1));                     /* LDA $8E92,Y */

loc_1A90E:                                        /* $A90E */
    CLC();                                        /* CLC */
    ADC(byte_4A);                                 /* ADC $4A */
    STA(byte_4A);                                 /* STA $4A */
    BCC(loc_1A918);                               /* BCC loc_1A918 */
    sub_1879F();                                  /* JSR sub_1879F */

loc_1A918:                                        /* $A918 */
    LDA(byte_56);                                 /* LDA $56 */
    CMPI(8);                                      /* CMP #8 */
    BCS(loc_1A928);                               /* BCS loc_1A928 */
    ASL_A();                                      /* ASL */
    ASL_A();                                      /* ASL */
    EORI(0xFF);                                   /* EOR #$FF */
    CLC();                                        /* CLC */
    ADCI(0x25);                                   /* ADC #$25 */
    goto loc_1A92A;                               /* JMP loc_1A92A */

loc_1A928:                                        /* $A928 */
    LDAI(1);                                      /* LDA #1 */

loc_1A92A:                                        /* $A92A */
    CLC();                                        /* CLC */
    ADC(byte_4A);                                 /* ADC $4A */
    STA(byte_4A);                                 /* STA $4A */
    BCC(loc_1A934);                               /* BCC loc_1A934 */
    sub_1879F();                                  /* JSR sub_1879F */

loc_1A934:                                        /* $A934 */
    LDAI(0);                                      /* LDA #0 */
    STA(byte_4C);                                 /* STA $4C */
    STA(byte_56);                                 /* STA $56 */
    LDAI(0x11);                                   /* LDA #17 - short explosion sound */
    j_apu_play();                                 /* JSR j_apu_play */

    /* $A93F loc_1A93F is fallen into here; enemy_type_80 jumps here too. */
    sub_C02A();                                   /* JSR sub_C02A */
    LDAI(0);                                      /* LDA #0 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(0x40);                                   /* LDA #$40 */
    LDYI(7);                                      /* LDY #7 */
    sub_1A2CB();                                  /* JSR sub_1A2CB */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDA(ABSX(0x07E6));                            /* LDA $7E6,X */
    BEQ(loc_1A96C);                               /* BEQ loc_1A96C */
    CMPI(0x25);                                   /* CMP #$25 */
    BEQ(loc_1A96C);                               /* BEQ loc_1A96C */
    CMPI(0x26);                                   /* CMP #$26 */
    BEQ(loc_1A96C);                               /* BEQ loc_1A96C */
    DEC(byte_45);                                 /* DEC $45 */
    BNE(loc_1A96C);                               /* BNE loc_1A96C */
    LDAI(0x10);                                   /* LDA #$10 */
    STA(byte_45);                                 /* STA $45 */
    LDAI(1);                                      /* LDA #1 */
    STA(byte_46);                                 /* STA $46 */

loc_1A96C:                                        /* $A96C */
    sub_183BC();                                  /* JSR sub_183BC */
    BCS(loc_1A97B);                               /* BCS loc_1A97B */
    LDA(ABSY(unk_1A97E));                         /* LDA unk_1A97E,Y */
    STA(ABSX(byte_576));                          /* STA $576,X */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    RTS();                                        /* RTS */

loc_1A97B:                                        /* $A97B - also entered from enemy_type_91_big_explosion */
    enemy_erase();                                /* JMP enemy_erase */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A985  enemy_type_91_big_explosion - big explosion with multiple blasts
 *
 *  Original: src/bank6.asm:7631-7725
 *      LDA $528,X / BMI loc_1A9A0 / ORA #$80 / STA $528,X / LDA #18 /
 *      JSR j_apu_play / LSR $4B / JSR sub_C02A / LDA #$60 / LDY #$10 /
 *      JSR sub_1A2CB
 *  loc_1A9A0:
 *      JSR sub_183BC / BCS loc_1A97B / LDA $55C,X / STA $17 /
 *      LDA $542,X / STA $16 / LDA #0 / STA $15
 *  loc_1A9B3:
 *      LDY $15 / LDA $6AE,X / SEC / SBC byte_1AA26,Y / BCC loc_1AA07 /
 *      CMP #12 / BCS loc_1AA07 / TAY / LDA byte_1AA56,Y / STA $576,X /
 *      LDY $15 / CLC / LDA byte_1AA36,Y / BMI loc_1A9D7 / ADC $17 /
 *      BCS loc_1AA07 / BCC loc_1A9DB
 *  loc_1A9D7:
 *      ADC $17 / BCC loc_1AA07
 *  loc_1A9DB:
 *      STA $55C,X / CLC / LDA byte_1AA46,Y / BMI loc_1A9EA / ADC $16 /
 *      BCS loc_1AA07 / BCC loc_1A9EE
 *  loc_1A9EA:
 *      ADC $16 / BCC loc_1AA07
 *  loc_1A9EE:
 *      STA $542,X / LDA #1 / STA $590,X / TYA / AND #7 / BNE loc_1AA04 /
 *      TYA / LSR / LSR / LSR / ADC #1 / STA $590,X
 *  loc_1AA04:
 *      JSR add_enemy_meta_sprite
 *  loc_1AA07:
 *      INC $15 / LDA $15 / CMP #16 / BNE loc_1A9B3 / LDA $17 /
 *      STA $55C,X / LDA $16 / STA $542,X / LDA $6AE,X / AND #7 /
 *      BNE locret_1AA25 / LDA #18 / JSR j_apu_play
 *  locret_1AA25:
 *      RTS
 *
 *  The BCS at $A9A3 goes to loc_1A97B, the shared tail of
 *  enemy_type_35_small_explosion, which is reproduced below.
 * ------------------------------------------------------------------------ */
void enemy_type_91_big_explosion(void)            /* $A985 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1A9A0);                               /* BMI loc_1A9A0 */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDAI(0x12);                                   /* LDA #18 - long explosion sound */
    j_apu_play();                                 /* JSR j_apu_play */
    LSR(byte_4B);                                 /* LSR $4B */
    sub_C02A();                                   /* JSR sub_C02A */
    LDAI(0x60);                                   /* LDA #$60 */
    LDYI(0x10);                                   /* LDY #$10 */
    sub_1A2CB();                                  /* JSR sub_1A2CB */

loc_1A9A0:                                        /* $A9A0 */
    sub_183BC();                                  /* JSR sub_183BC */
    BCS(loc_1A97B);                               /* BCS loc_1A97B (inside enemy_type_35) */
    LDA(ABSX(byte_55C));                          /* LDA $55C,X */
    STA(byte_17);                                 /* STA $17 */
    LDA(ABSX(byte_542));                          /* LDA $542,X */
    STA(byte_16);                                 /* STA $16 */
    LDAI(0);                                      /* LDA #0 */
    STA(byte_15);                                 /* STA $15 */

loc_1A9B3:                                        /* $A9B3 */
    LDY(byte_15);                                 /* LDY $15 */
    LDA(ABSX(0x06AE));                            /* LDA $6AE,X */
    SEC();                                        /* SEC */
    SBC(ABSY(byte_1AA26));                        /* SBC byte_1AA26,Y */
    BCC(loc_1AA07);                               /* BCC loc_1AA07 */
    CMPI(12);                                     /* CMP #12 */
    BCS(loc_1AA07);                               /* BCS loc_1AA07 */
    TAY();                                        /* TAY */
    LDA(ABSY(byte_1AA56));                        /* LDA byte_1AA56,Y */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDY(byte_15);                                 /* LDY $15 */
    CLC();                                        /* CLC */
    LDA(ABSY(byte_1AA36));                        /* LDA byte_1AA36,Y */
    BMI(loc_1A9D7);                               /* BMI loc_1A9D7 */
    ADC(byte_17);                                 /* ADC $17 */
    BCS(loc_1AA07);                               /* BCS loc_1AA07 */
    BCC(loc_1A9DB);                               /* BCC loc_1A9DB */

loc_1A9D7:                                        /* $A9D7 */
    ADC(byte_17);                                 /* ADC $17 */
    BCC(loc_1AA07);                               /* BCC loc_1AA07 */

loc_1A9DB:                                        /* $A9DB */
    STA(ABSX(byte_55C));                          /* STA $55C,X - X Position */
    CLC();                                        /* CLC */
    LDA(ABSY(byte_1AA46));                        /* LDA byte_1AA46,Y */
    BMI(loc_1A9EA);                               /* BMI loc_1A9EA */
    ADC(byte_16);                                 /* ADC $16 */
    BCS(loc_1AA07);                               /* BCS loc_1AA07 */
    BCC(loc_1A9EE);                               /* BCC loc_1A9EE */

loc_1A9EA:                                        /* $A9EA */
    ADC(byte_16);                                 /* ADC $16 */
    BCC(loc_1AA07);                               /* BCC loc_1AA07 */

loc_1A9EE:                                        /* $A9EE */
    STA(ABSX(byte_542));                          /* STA $542,X - Y Position */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    TYA();                                        /* TYA */
    ANDI(7);                                      /* AND #7 */
    BNE(loc_1AA04);                               /* BNE loc_1AA04 */
    TYA();                                        /* TYA */
    LSR_A();                                      /* LSR */
    LSR_A();                                      /* LSR */
    LSR_A();                                      /* LSR */
    ADCI(1);                                      /* ADC #1 */
    STA(ABSX(byte_590));                          /* STA $590,X */

loc_1AA04:                                        /* $AA04 */
    add_enemy_meta_sprite();                      /* JSR add_enemy_meta_sprite */

loc_1AA07:                                        /* $AA07 */
    INC(byte_15);                                 /* INC $15 */
    LDA(byte_15);                                 /* LDA $15 */
    CMPI(16);                                     /* CMP #16 */
    BNE(loc_1A9B3);                               /* BNE loc_1A9B3 */
    LDA(byte_17);                                 /* LDA $17 */
    STA(ABSX(byte_55C));                          /* STA $55C,X */
    LDA(byte_16);                                 /* LDA $16 */
    STA(ABSX(byte_542));                          /* STA $542,X */
    LDA(ABSX(0x06AE));                            /* LDA $6AE,X */
    ANDI(7);                                      /* AND #7 */
    BNE(locret_1AA25);                            /* BNE locret_1AA25 */
    LDAI(0x12);                                   /* LDA #18 - long explosion sound */
    j_apu_play();                                 /* JSR j_apu_play */

locret_1AA25:                                     /* $AA25 */
    RTS();                                        /* RTS */

    /* $A97B loc_1A97B (inside enemy_type_35_small_explosion): reproduced
     * here because a C goto cannot cross functions. */
loc_1A97B:
    enemy_erase();                                /* JMP enemy_erase */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AA62  enemy_type_37
 *
 *  Original: src/bank6.asm:7736-7739
 *      LDA $528,X
 *      BMI sub_1AA86
 *  (no RTS: execution continues into sub_1AA67)
 * ------------------------------------------------------------------------ */
void enemy_type_37(void)                          /* $AA62 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(sub_1AA86);                               /* BMI sub_1AA86 */
    sub_1AA67();                                  /* falls through into $AA67 */
    RTS();

sub_1AA86:                                        /* $AA86 - entry of the sub_1AA86 procedure */
    sub_1AA86();                                  /* JMP sub_1AA86 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AA67  sub_1AA67
 *
 *  Original: src/bank6.asm:7745-7749
 *      LDA #3
 *      STA $74A,X
 *      JSR calculate_slope_and_speed
 *  (no RTS: execution continues into sub_1AA6F)
 * ------------------------------------------------------------------------ */
void sub_1AA67(void)                              /* $AA67 */
{
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    calculate_slope_and_speed();                  /* JSR calculate_slope_and_speed */
    sub_1AA6F();                                  /* falls through into $AA6F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AA6F  sub_1AA6F
 *
 *  Original: src/bank6.asm:7755-7765
 *      LDA #3
 *      STA $660,X
 *      LDA #$47
 *      STA $576,X
 *      LDA #1
 *      STA $590,X
 *      LDA $528,X
 *      ORA #$80
 *      STA $528,X
 *  (no RTS: execution continues into sub_1AA86)
 * ------------------------------------------------------------------------ */
void sub_1AA6F(void)                              /* $AA6F */
{
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(0x47);                                   /* LDA #$47 */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    sub_1AA86();                                  /* falls through into $AA86 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AA86  sub_1AA86
 *
 *  Original: src/bank6.asm:7771-7775
 *      JSR process_enemy_flags_sprites
 *      JMP sub_181E4
 *
 *  Also entered by the `BMI sub_1AA86` of enemy_type_37 and enemy_type_38_lead.
 * ------------------------------------------------------------------------ */
void sub_1AA86(void)                              /* $AA86 */
{
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    sub_181E4();                                  /* JMP sub_181E4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AA8C  enemy_type_38_lead - lead/chip, the simplest projectile
 *
 *  Original: src/bank6.asm:7782-7785
 *      LDA $528,X
 *      BMI sub_1AA86
 *  (no RTS: execution continues into sub_1AA91)
 * ------------------------------------------------------------------------ */
void enemy_type_38_lead(void)                     /* $AA8C */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(sub_1AA86);                               /* BMI sub_1AA86 */
    sub_1AA91();                                  /* falls through into $AA91 */
    RTS();

sub_1AA86:                                        /* $AA86 - entry of the sub_1AA86 procedure */
    sub_1AA86();                                  /* JMP sub_1AA86 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AA91  sub_1AA91
 *
 *  Original: src/bank6.asm:7791-7799
 *      LDA #3
 *      STA $74A,X
 *  loc_1AA96:
 *      LDA $77E,X
 *      JSR set_speed_by_slope
 *      JMP sub_1AA6F
 *
 *  loc_1AA96 ($AA96) is called by a JSR in enemy_type_45, which reproduces
 *  this tail.
 * ------------------------------------------------------------------------ */
void sub_1AA91(void)                              /* $AA91 */
{
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(0x074A));                            /* STA $74A,X */

    /* $AA96 loc_1AA96: entered with a JSR from enemy_type_45
     * (fall-through here). */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    sub_1AA6F();                                  /* JMP sub_1AA6F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AA9F  enemy_type_39
 *
 *  Original: src/bank6.asm:7805-7813
 *      DEC $542,X
 *      BEQ loc_1AAA5
 *      RTS
 *  loc_1AAA5:
 *      JMP enemy_erase
 * ------------------------------------------------------------------------ */
void enemy_type_39(void)                          /* $AA9F */
{
    DEC(ABSX(byte_542));                          /* DEC $542,X - Y Position */
    BEQ(loc_1AAA5);                               /* BEQ loc_1AAA5 */
    RTS();                                        /* RTS */

loc_1AAA5:                                        /* $AAA5 */
    enemy_erase();                                /* JMP enemy_erase */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AAA8  enemy_type_41
 *
 *  Original: src/bank6.asm:7819-7929
 *      LDA $528,X / BMI loc_1AB18 / ORA #$80 / STA $528,X / LDA #3 /
 *      STA $660,X / LDA $74A,X / PHA / TAY / LDA byte_1AB90,Y /
 *      STA $74A,X / CLC / LDA $77E,X / STA $67A,X /
 *      JSR set_speed_by_slope / LDA $5F8,X / STA $716,X / LDA $612,X /
 *      STA $730,X / LDA $62C,X / STA $6AE,X / LDA $646,X / STA $6C8,X /
 *      LDA #8 / STA $694,X / LDA $67A,X / BPL loc_1AAF3 / LDA #0 /
 *      STA $694,X
 *  loc_1AAF3:
 *      PLA / TAY / LDA byte_1AB90+1,Y / STA $74A,X /
 *      LDA byte_1AB90+2,Y / STA $6FC,X / LDA byte_1AB90+3,Y /
 *      STA $576,X / LDA byte_1AB90+4,Y / STA $590,X /
 *      LDA byte_1AB90+5,Y / STA $764,X / LDA #1 / STA $6E2,X
 *  loc_1AB18:
 *      DEC $6E2,X / BNE loc_1AB5D / LDA $6FC,X / STA $6E2,X /
 *      LDA $67A,X / BPL loc_1AB2E / DEC $694,X / JMP loc_1AB31
 *  loc_1AB2E:
 *      INC $694,X
 *  loc_1AB31:
 *      LDA $694,X / JSR set_speed_by_slope / LDA $5F8,X / CLC /
 *      ADC $716,X / STA $5F8,X / LDA $612,X / ADC $730,X / STA $612,X /
 *      LDA $62C,X / CLC / ADC $6AE,X / STA $62C,X / LDA $646,X /
 *      ADC $6C8,X / STA $646,X
 *  loc_1AB5D:
 *      JSR process_enemy_flags_sprites / LDA $576,X / CMP #$47 /
 *      BNE loc_1AB6A / JMP sub_181E4
 *  loc_1AB6A:
 *      LDA $5AA,X / TAY / LDA $528,Y / AND #$7F / CMP #$23 /
 *      BEQ loc_1AB7B / CMP #$50 / BNE loc_1AB89
 *  loc_1AB7B:
 *      LDA #$23 / STA $528,X / LDA $528,X / AND #$7F / STA $7E6,X / RTS
 *  loc_1AB89:
 *      JSR sub_181F6 / JSR decrease_enemy_health / RTS
 * ------------------------------------------------------------------------ */
void enemy_type_41(void)                          /* $AAA8 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1AB18);                               /* BMI loc_1AB18 */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDA(ABSX(0x074A));                            /* LDA $74A,X */
    PHA();                                        /* PHA */
    TAY();                                        /* TAY */
    LDA(ABSY(byte_1AB90));                        /* LDA byte_1AB90,Y */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    CLC();                                        /* CLC */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    STA(ABSX(unk_67A));                           /* STA $67A,X */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    LDA(ABSX(byte_5F8));                          /* LDA $5F8,X */
    STA(ABSX(byte_716));                          /* STA $716,X */
    LDA(ABSX(byte_612));                          /* LDA $612,X */
    STA(ABSX(0x0730));                            /* STA $730,X */
    LDA(ABSX(byte_62C));                          /* LDA $62C,X */
    STA(ABSX(0x06AE));                            /* STA $6AE,X */
    LDA(ABSX(byte_646));                          /* LDA $646,X */
    STA(ABSX(0x06C8));                            /* STA $6C8,X */
    LDAI(8);                                      /* LDA #8 */
    STA(ABSX(0x0694));                            /* STA $694,X */
    LDA(ABSX(unk_67A));                           /* LDA $67A,X */
    BPL(loc_1AAF3);                               /* BPL loc_1AAF3 */
    LDAI(0);                                      /* LDA #0 */
    STA(ABSX(0x0694));                            /* STA $694,X */

loc_1AAF3:                                        /* $AAF3 */
    PLA();                                        /* PLA */
    TAY();                                        /* TAY */
    LDA(ABSY(byte_1AB90 + 1));                    /* LDA byte_1AB90+1,Y */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    LDA(ABSY(byte_1AB90 + 2));                    /* LDA byte_1AB90+2,Y */
    STA(ABSX(byte_6FC));                          /* STA $6FC,X */
    LDA(ABSY(byte_1AB90 + 3));                    /* LDA byte_1AB90+3,Y */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDA(ABSY(byte_1AB90 + 4));                    /* LDA byte_1AB90+4,Y */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDA(ABSY(byte_1AB90 + 5));                    /* LDA byte_1AB90+5,Y */
    STA(ABSX(byte_764));                          /* STA $764,X */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_6E2));                          /* STA $6E2,X */

loc_1AB18:                                        /* $AB18 */
    DEC(ABSX(byte_6E2));                          /* DEC $6E2,X */
    BNE(loc_1AB5D);                               /* BNE loc_1AB5D */
    LDA(ABSX(byte_6FC));                          /* LDA $6FC,X */
    STA(ABSX(byte_6E2));                          /* STA $6E2,X */
    LDA(ABSX(unk_67A));                           /* LDA $67A,X */
    BPL(loc_1AB2E);                               /* BPL loc_1AB2E */
    DEC(ABSX(0x0694));                            /* DEC $694,X */
    goto loc_1AB31;                               /* JMP loc_1AB31 */

loc_1AB2E:                                        /* $AB2E */
    INC(ABSX(0x0694));                            /* INC $694,X */

loc_1AB31:                                        /* $AB31 */
    LDA(ABSX(0x0694));                            /* LDA $694,X */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    LDA(ABSX(byte_5F8));                          /* LDA $5F8,X */
    CLC();                                        /* CLC */
    ADC(ABSX(byte_716));                          /* ADC $716,X */
    STA(ABSX(byte_5F8));                          /* STA $5F8,X */
    LDA(ABSX(byte_612));                          /* LDA $612,X */
    ADC(ABSX(0x0730));                            /* ADC $730,X */
    STA(ABSX(byte_612));                          /* STA $612,X */
    LDA(ABSX(byte_62C));                          /* LDA $62C,X */
    CLC();                                        /* CLC */
    ADC(ABSX(0x06AE));                            /* ADC $6AE,X */
    STA(ABSX(byte_62C));                          /* STA $62C,X */
    LDA(ABSX(byte_646));                          /* LDA $646,X */
    ADC(ABSX(0x06C8));                            /* ADC $6C8,X */
    STA(ABSX(byte_646));                          /* STA $646,X */

loc_1AB5D:                                        /* $AB5D */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    LDA(ABSX(byte_576));                          /* LDA $576,X */
    CMPI(0x47);                                   /* CMP #$47 */
    BNE(loc_1AB6A);                               /* BNE loc_1AB6A */
    sub_181E4();                                  /* JMP sub_181E4 */
    RTS();

loc_1AB6A:                                        /* $AB6A */
    LDA(ABSX(byte_5AA));                          /* LDA $5AA,X */
    TAY();                                        /* TAY */
    LDA(ABSY(byte_528));                          /* LDA $528,Y */
    ANDI(0x7F);                                   /* AND #$7F */
    CMPI(0x23);                                   /* CMP #$23 */
    BEQ(loc_1AB7B);                               /* BEQ loc_1AB7B */
    CMPI(0x50);                                   /* CMP #$50 */
    BNE(loc_1AB89);                               /* BNE loc_1AB89 */

loc_1AB7B:                                        /* $AB7B */
    LDAI(0x23);                                   /* LDA #$23 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    ANDI(0x7F);                                   /* AND #$7F */
    STA(ABSX(0x07E6));                            /* STA $7E6,X */
    RTS();                                        /* RTS */

loc_1AB89:                                        /* $AB89 */
    sub_181F6();                                  /* JSR sub_181F6 */
    decrease_enemy_health();                      /* JSR decrease_enemy_health */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ABA2  enemy_type_42
 *
 *  Original: src/bank6.asm:7939-7943
 *      JSR sub_1AA67
 *      LDA #$A5
 *      JMP sub_1ABAF
 * ------------------------------------------------------------------------ */
void enemy_type_42(void)                          /* $ABA2 */
{
    sub_1AA67();                                  /* JSR sub_1AA67 */
    LDAI(0xA5);                                   /* LDA #$A5 */
    sub_1ABAF();                                  /* JMP sub_1ABAF */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ABAA  enemy_type_43
 *
 *  Original: src/bank6.asm:7949-7952
 *      JSR sub_1AA91
 *      LDA #$A6
 *  (no RTS: execution continues into sub_1ABAF)
 * ------------------------------------------------------------------------ */
void enemy_type_43(void)                          /* $ABAA */
{
    sub_1AA91();                                  /* JSR sub_1AA91 */
    LDAI(0xA6);                                   /* LDA #$A6 */
    sub_1ABAF();                                  /* falls through into $ABAF */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ABAF  sub_1ABAF
 *
 *  Original: src/bank6.asm:7958-7984
 *      STA $528,X / JSR enemy_prng / AND #$7F / ADC $62C,X / STA $62C,X /
 *      LDA $646,X / ADC #0 / STA $646,X / LDA $62C,X / ADC #$C0 /
 *      STA $62C,X / LDA $646,X / ADC #$FF / STA $646,X / ORA $62C,X /
 *      BNE locret_1ABE7 / LDA $5F8,X / ORA $612,X / BNE locret_1ABE7 /
 *      LDA #$40 / STA $5F8,X
 *  locret_1ABE7:
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1ABAF(void)                              /* $ABAF */
{
    STA(ABSX(byte_528));                          /* STA $528,X */
    enemy_prng();                                 /* JSR enemy_prng */
    ANDI(0x7F);                                   /* AND #$7F */
    ADC(ABSX(byte_62C));                          /* ADC $62C,X */
    STA(ABSX(byte_62C));                          /* STA $62C,X */
    LDA(ABSX(byte_646));                          /* LDA $646,X */
    ADCI(0x00);                                   /* ADC #0 */
    STA(ABSX(byte_646));                          /* STA $646,X */
    LDA(ABSX(byte_62C));                          /* LDA $62C,X */
    ADCI(0xC0);                                   /* ADC #$C0 */
    STA(ABSX(byte_62C));                          /* STA $62C,X */
    LDA(ABSX(byte_646));                          /* LDA $646,X */
    ADCI(0xFF);                                   /* ADC #$FF */
    STA(ABSX(byte_646));                          /* STA $646,X */
    ORA(ABSX(byte_62C));                          /* ORA $62C,X */
    BNE(locret_1ABE7);                            /* BNE locret_1ABE7 */
    LDA(ABSX(byte_5F8));                          /* LDA $5F8,X */
    ORA(ABSX(byte_612));                          /* ORA $612,X */
    BNE(locret_1ABE7);                            /* BNE locret_1ABE7 */
    LDAI(0x40);                                   /* LDA #$40 */
    STA(ABSX(byte_5F8));                          /* STA $5F8,X */

locret_1ABE7:                                     /* $ABE7 */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ABE8  enemy_type_45
 *
 *  Original: src/bank6.asm:7990-8040
 *      LDA $528,X / BMI loc_1AC11 / JSR enemy_prng / AND #1 / CLC /
 *      ADC #2 / STA $74A,X / LDA $77E,X / STA $6FC,X / JSR loc_1AA96 /
 *      LDA $528,X / BMI loc_1AC07 / RTS
 *  loc_1AC07:
 *      LDA #3 / STA $764,X
 *  loc_1AC0C:
 *      LDA #$28 / STA $6E2,X
 *  loc_1AC11:
 *      DEC $6E2,X / BNE loc_1AC2F / JSR enemy_prng / BPL loc_1AC0C /
 *      AND #8 / CLC / ADC $6FC,X / SEC / SBC #4 / AND #$F /
 *      STA $6FC,X / JSR set_speed_by_slope / JMP loc_1AC0C
 *  loc_1AC2F:
 *      LDA $6E2,X / AND #1 / TAY / LDA unk_1AC44,Y / STA $576,X /
 *      LDA unk_1AC46,Y / STA $590,X / JMP loc_1A740
 *
 *  The JSR loc_1AA96 target (in sub_1AA91) and the JMP loc_1A740 target
 *  (the shared tail of enemy_type_36_giza) are reproduced here, because a C
 *  goto cannot cross functions.
 * ------------------------------------------------------------------------ */
void enemy_type_45(void)                          /* $ABE8 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1AC11);                               /* BMI loc_1AC11 */
    enemy_prng();                                 /* JSR enemy_prng */
    ANDI(0x01);                                   /* AND #1 */
    CLC();                                        /* CLC */
    ADCI(2);                                      /* ADC #2 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    STA(ABSX(byte_6FC));                          /* STA $6FC,X */

    /* $AA96 loc_1AA96 (inside sub_1AA91): the JSR target's tail, reproduced
     * here so that control returns to $AC01 below. */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    sub_1AA6F();                                  /* JMP sub_1AA6F */

    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1AC07);                               /* BMI loc_1AC07 */
    RTS();                                        /* RTS */

loc_1AC07:                                        /* $AC07 */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_764));                          /* STA $764,X */

loc_1AC0C:                                        /* $AC0C */
    LDAI(0x28);                                   /* LDA #$28 */
    STA(ABSX(byte_6E2));                          /* STA $6E2,X */

loc_1AC11:                                        /* $AC11 */
    DEC(ABSX(byte_6E2));                          /* DEC $6E2,X */
    BNE(loc_1AC2F);                               /* BNE loc_1AC2F */
    enemy_prng();                                 /* JSR enemy_prng */
    BPL(loc_1AC0C);                               /* BPL loc_1AC0C */
    ANDI(0x08);                                   /* AND #8 */
    CLC();                                        /* CLC */
    ADC(ABSX(byte_6FC));                          /* ADC $6FC,X */
    SEC();                                        /* SEC */
    SBCI(4);                                      /* SBC #4 */
    ANDI(0x0F);                                   /* AND #$F */
    STA(ABSX(byte_6FC));                          /* STA $6FC,X */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    goto loc_1AC0C;                               /* JMP loc_1AC0C */

loc_1AC2F:                                        /* $AC2F */
    LDA(ABSX(byte_6E2));                          /* LDA $6E2,X */
    ANDI(0x01);                                   /* AND #1 */
    TAY();                                        /* TAY */
    LDA(ABSY(unk_1AC44));                         /* LDA unk_1AC44,Y */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDA(ABSY(unk_1AC46));                         /* LDA unk_1AC46,Y */
    STA(ABSX(byte_590));                          /* STA $590,X */

    /* $A740 loc_1A740 (the shared tail inside enemy_type_36_giza),
     * reproduced here because a C goto cannot cross functions. */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    sub_181F6();                                  /* JSR sub_181F6 */
    decrease_enemy_health();                      /* JSR decrease_enemy_health */
    RTS();                                        /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $AC48  enemy_type_21
 *
 *  Original: src/bank6.asm:8051-8076
 *      LDA $528,X / BMI loc_1AC6E / ORA #$80 / STA $528,X / LDA #4 /
 *      STA $74A,X / LDA $77E,X / JSR set_speed_by_slope / LDA #3 /
 *      STA $660,X / LDA #$60 / LDY #$10 / JSR sub_1A2CB / LDA #$16 /
 *      JSR j_apu_play
 *  loc_1AC6E:
 *      JSR sub_183BC / LDA unk_1AC83,Y / STA $576,X / LDA $AC93,Y /
 *      STA $590,X / JSR process_enemy_flags_sprites / JMP sub_181F6
 *
 *  $AC93 is the second half (offset $10) of the unk_1AC83 table.
 * ------------------------------------------------------------------------ */
void enemy_type_21(void)                          /* $AC48 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1AC6E);                               /* BMI loc_1AC6E */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    LDAI(4);                                      /* LDA #4 */
    STA(ABSX(0x074A));                            /* STA $74A,X */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    set_speed_by_slope();                         /* JSR set_speed_by_slope */
    LDAI(3);                                      /* LDA #3 */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(0x60);                                   /* LDA #$60 */
    LDYI(0x10);                                   /* LDY #$10 */
    sub_1A2CB();                                  /* JSR sub_1A2CB */
    LDAI(0x16);                                   /* LDA #$16 */
    j_apu_play();                                 /* JSR j_apu_play */

loc_1AC6E:                                        /* $AC6E */
    sub_183BC();                                  /* JSR sub_183BC */
    LDA(ABSY(unk_1AC83));                         /* LDA unk_1AC83,Y */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDA(ABSY(unk_1AC83 + 0x10));                  /* LDA $AC93,Y */
    STA(ABSX(byte_590));                          /* STA $590,X */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    sub_181F6();                                  /* JMP sub_181F6 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ACA3  enemy_type_20
 *
 *  Original: src/bank6.asm:8115-8140
 *      LDA $528,X / BMI loc_1ACD8 / ORA #$80 / STA $528,X / STA $5F8,X /
 *      LDA #$47 / STA $576,X / LDA #1 / STA $590,X / LDA #$B /
 *      STA $660,X / LDA #$FF / STA $6E2,X / STA $612,X / LDA #$C /
 *      STA $716,X / LDA $77E,X / STA $646,X / JSR enemy_prng /
 *      STA $62C,X
 *  loc_1ACD8:
 *      JSR process_enemy_flags_sprites / JMP sub_181E4
 * ------------------------------------------------------------------------ */
void enemy_type_20(void)                          /* $ACA3 */
{
    LDA(ABSX(byte_528));                          /* LDA $528,X */
    BMI(loc_1ACD8);                               /* BMI loc_1ACD8 */
    ORAI(0x80);                                   /* ORA #$80 */
    STA(ABSX(byte_528));                          /* STA $528,X */
    STA(ABSX(byte_5F8));                          /* STA $5F8,X */
    LDAI(0x47);                                   /* LDA #$47 */
    STA(ABSX(byte_576));                          /* STA $576,X */
    LDAI(1);                                      /* LDA #1 */
    STA(ABSX(byte_590));                          /* STA $590,X */
    LDAI(0x0B);                                   /* LDA #$B */
    STA(ABSX(byte_660));                          /* STA $660,X */
    LDAI(0xFF);                                   /* LDA #$FF */
    STA(ABSX(byte_6E2));                          /* STA $6E2,X */
    STA(ABSX(byte_612));                          /* STA $612,X */
    LDAI(0x0C);                                   /* LDA #$C */
    STA(ABSX(byte_716));                          /* STA $716,X */
    LDA(ABSX(byte_77E));                          /* LDA $77E,X */
    STA(ABSX(byte_646));                          /* STA $646,X */
    enemy_prng();                                 /* JSR enemy_prng */
    STA(ABSX(byte_62C));                          /* STA $62C,X */

loc_1ACD8:                                        /* $ACD8 */
    process_enemy_flags_sprites();                /* JSR process_enemy_flags_sprites */
    sub_181E4();                                  /* JMP sub_181E4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ACDE  enemy_type_53
 *
 *  Original: src/bank6.asm:8146-8188
 *      JSR sub_1B87F / BCC loc_1ACE8 / LDA #$1E / STA $764,X
 *  loc_1ACE8:
 *      LDA $5AA,X / BEQ loc_1AD05 / LDA $6E2,X / CMP #9 / BCS loc_1AD05 /
 *      INC $6E2,X / TAY / AND #3 / BNE loc_1AD05 / TYA / AND #$C /
 *      ADC #$70 / TAY / JSR sub_1B1CD
 *  loc_1AD05:
 *      JSR sub_18204 / JSR decrease_enemy_health / BNE loc_1AD13 /
 *      JSR sub_C02A / JMP sub_C02A
 *  loc_1AD13:
 *      LDA $5AA,X / INC $5AA,X / CMP #0 / BNE loc_1AD20 / JMP loc_1972F
 *  loc_1AD20:
 *      LDA $5C / JMP sub_19493
 *
 *  loc_1972F ($972F) is the shared tail inside sub_196F3, reproduced here
 *  because a C goto cannot cross functions.
 * ------------------------------------------------------------------------ */
void enemy_type_53(void)                          /* $ACDE */
{
    sub_1B87F();                                  /* JSR sub_1B87F */
    BCC(loc_1ACE8);                               /* BCC loc_1ACE8 */
    LDAI(0x1E);                                   /* LDA #$1E */
    STA(ABSX(byte_764));                          /* STA $764,X */

loc_1ACE8:                                        /* $ACE8 */
    LDA(ABSX(byte_5AA));                          /* LDA $5AA,X */
    BEQ(loc_1AD05);                               /* BEQ loc_1AD05 */
    LDA(ABSX(byte_6E2));                          /* LDA $6E2,X */
    CMPI(9);                                      /* CMP #9 */
    BCS(loc_1AD05);                               /* BCS loc_1AD05 */
    INC(ABSX(byte_6E2));                          /* INC $6E2,X */
    TAY();                                        /* TAY */
    ANDI(3);                                      /* AND #3 */
    BNE(loc_1AD05);                               /* BNE loc_1AD05 */
    TYA();                                        /* TYA */
    ANDI(0x0C);                                   /* AND #$C */
    ADCI(0x70);                                   /* ADC #$70 */
    TAY();                                        /* TAY */
    sub_1B1CD();                                  /* JSR sub_1B1CD */

loc_1AD05:                                        /* $AD05 */
    sub_18204();                                  /* JSR sub_18204 */
    decrease_enemy_health();                      /* JSR decrease_enemy_health */
    BNE(loc_1AD13);                               /* BNE loc_1AD13 */
    sub_C02A();                                   /* JSR sub_C02A */
    sub_C02A();                                   /* JMP sub_C02A */
    RTS();

loc_1AD13:                                        /* $AD13 */
    LDA(ABSX(byte_5AA));                          /* LDA $5AA,X */
    INC(ABSX(byte_5AA));                          /* INC $5AA,X */
    CMPI(0x00);                                   /* CMP #0 */
    BNE(loc_1AD20);                               /* BNE loc_1AD20 */

    /* $972F loc_1972F (the shared tail inside sub_196F3), reproduced here. */
    LDAI(4);                                      /* LDA #4 */
    STA(unk_36);                                  /* STA $36 */
    LDAI(0xF5);                                   /* LDA #$F5 */
    STA(zanac_main_weapon_speed_y);               /* STA $37 */
    LDAI(3);                                      /* LDA #3 */
    STA(0x0035);                                  /* STA $35 */
    RTS();                                        /* RTS */

loc_1AD20:                                        /* $AD20 */
    LDA(special_weapon_type);                     /* LDA $5C */
    sub_19493();                                  /* JMP sub_19493 */
    RTS();
}
