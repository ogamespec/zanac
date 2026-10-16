/* bank6/96E8.c - bank 6, $96E8-$9DAF: the enemy handlers of the object types
 * 7-15, 24, 48-52, 63 and 69 and the capital-ship support code.
 *
 * Ported from src/bank6.asm lines 4680-5813 (addresses from Build/bank6.lst).
 *
 * Review notes (PORTING.md, and c_src/analysis/bank6_report.md sections 2.2,
 * 3.1-3.3 and 3.5).  The constructs that needed a convention are:
 *
 *  - fall-through: enemy_type_63 -> sub_196F3, enemy_type_7 -> sub_197B4 and
 *    enemy_type_51_girevu -> enemy_type_49_50_52 end with a call plus RTS();
 *  - shared RTS bytes owned by another procedure: locret_196DB ($96DB, in the
 *    previous chunk) and locret_19C96 ($9C96, inside enemy_type_49_50_52) are
 *    reproduced as local labels of the same name;
 *  - a shared tail inside enemy_type_49_50_52: enemy_type_48_maseru's
 *    "JMP loc_19C85" ($9C23) is reproduced literally (PORTING.md section 5);
 *  - cross-procedure entry points: enemy_type_8/enemy_type_9 reach loc_19744
 *    and loc_19777 inside enemy_type_7, and re-entering enemy_type_7()
 *    repeats the same LDA/BMI and lands on those labels; loc_19744 and
 *    loc_1972F (entered by "JMP loc_1972F" from enemy_type_53 at asm line
 *    8182, chunk bank6_A609) are only reached by fall-through here, so they are
 *    documented as comments instead of unused C labels (the project builds
 *    with -Wall -Wextra, which would flag them);
 *  - "JMP loc_1B48F" ($9CC1): loc_1B48F is the tail of sub_1B482 (chunk
 *    bank6_B44B) and sets A=$1F before falling through into sub_1B491, so the
 *    port sets A and calls sub_1B491;
 *  - the PLA/PLA unwind of sub_19CAB ($9CB0, bank6_report.md section 3.3): the
 *    original drops its own return address so that its RTS resumes the
 *    dispatcher's slot loop at $8807, abandoning the calling handler
 *    enemy_type_48_maseru.  A C callee cannot drop its caller's frame, so the
 *    unwind is reproduced at the only call site, which re-tests $5AA,X after
 *    the call (sub_19CAB leaves that byte unchanged);
 *  - decrease_enemy_health (called from four procedures here) unwinds the same
 *    way; that cannot be expressed from the call site, so those calls are kept
 *    literal and the deviation is reported;
 *  - the bytes at $973A/$973D (inside sub_196F3) and at $97B9 (the last bytes
 *    of sub_197B4's "JMP sub_181F6") are instruction bytes used as data; they
 *    are read from the extracted bank image, which keeps them intact.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $96E8  enemy_type_63 - object type 63 handler
 *
 *  Original: src/bank6.asm:4680-4685 (bank 6)
 *      JSR process_enemy_flags_sprites
 *      JSR sub_181ED
 *      LDA $528,X
 *      BMI locret_196DB
 *  (no RTS: when $528,X is positive execution continues into sub_196F3)
 * ------------------------------------------------------------------------ */
void enemy_type_63(void)                     /* $96E8 */
{
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181ED();                             /* JSR sub_181ED */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(locret_196DB);                       /* BMI locret_196DB */
    sub_196F3();                             /* falls through into $96F3 */
    RTS();                                   /* RTS */

locret_196DB:                                /* $96DB - shared RTS inside the previous procedure */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $96F3  sub_196F3 - warp sequence: raise the main weapon level
 *
 *  Original: src/bank6.asm:4691-4734 (bank 6)
 *
 *  loc_1972F ($972F) is also a jump target of enemy_type_53 (asm line 8182,
 *  chunk bank6_A609): that entry point sets $36/$37/$35 and returns.
 * ------------------------------------------------------------------------ */
void sub_196F3(void)                         /* $96F3 */
{
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(0x17);                              /* LDA #$17 */
    j_apu_play();                            /* JSR j_apu_play */
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA $528 */
    LDA(byte_5AA);                           /* LDA $5AA */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_5AA);                           /* STA $5AA */
    LDAI(0x40);                              /* LDA #$40 */
    STA(byte_764);                           /* STA $764 */
    INC(zanac_main_weapon_level);            /* INC $33 */
    LDA(zanac_main_weapon_level);            /* LDA $33 */
    CMPI(6);                                 /* CMP #6 */
    BCC(loc_1973C);                          /* BCC loc_1973C */
    DEC(zanac_main_weapon_level);            /* DEC $33 */
    INC(byte_5A);                            /* INC $5A */
    LDA(byte_5A);                            /* LDA $5A */
    ANDI(7);                                 /* AND #7 */
    BNE(loc_19725);                          /* BNE loc_19725 */
    sub_187A6();                             /* JSR sub_187A6 */
    sub_187A6();                             /* JSR sub_187A6 */

loc_19725:                                   /* $9725 */
    LDA(byte_5A);                            /* LDA $5A */
    CMPI(0x0D);                              /* CMP #$D */
    BNE(locret_196DB);                       /* BNE locret_196DB */
    LDA(special_weapon_type);                /* LDA $5C */
    BNE(locret_196DB);                       /* BNE locret_196DB */

    /* $972F loc_1972F - second entry point of this procedure, entered by
     * "JMP loc_1972F" from enemy_type_53 (asm line 8182, chunk bank6_A609).
     * Nothing inside sub_196F3 branches here, so it stays a comment; the
     * chunk that owns enemy_type_53 must reproduce these three stores. */
    LDAI(4);                                 /* LDA #4 */
    STA(unk_36);                             /* STA $36 */
    LDAI(0xF5);                              /* LDA #$F5 */
    STA(zanac_main_weapon_speed_y);          /* STA $37 */
    LDAI(3);                                 /* LDA #3 */
    STA(0x0035);                             /* STA $35 */
    RTS();                                   /* RTS */

loc_1973C:                                   /* $973C */
    set_default_weapon();                    /* JMP set_default_weapon */
    RTS();                                   /* RTS */

locret_196DB:                                /* $96DB - shared RTS (reproduced) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $973F  enemy_type_7 - object type 7 handler
 *
 *  Original: src/bank6.asm:4740-4799 (bank 6)
 *
 *  loc_19744 ($9744) and loc_19777 ($9777) are second entry points of this
 *  procedure (enemy_type_8 and enemy_type_9 jump into them); the shared tail
 *  sub_197B4 ($97B4) and sub_197CF ($97CF) belong to the next procedures and
 *  are reached with a local label plus a call.
 * ------------------------------------------------------------------------ */
void enemy_type_7(void)                      /* $973F */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19777);                          /* BMI loc_19777 */

    /* $9744 loc_19744 - also entered by "JMP loc_19744" from enemy_type_8
     * ($97CC) and enemy_type_9 ($9802); both re-enter enemy_type_7(). */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(9);                                 /* LDA #9 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    TAY();                                   /* TAY */
    LDA(ABSY(0x973A));                       /* LDA $973A,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSY(0x973D));                       /* LDA $973D,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */

loc_19777:                                   /* $9777 - also entered by enemy_type_8 */
    LDA(ABSX(byte_612));                     /* LDA $612,X */
    BEQ(loc_19787);                          /* BEQ loc_19787 */
    CMPI(0xFF);                              /* CMP #$FF */
    BNE(sub_197B4);                          /* BNE sub_197B4 */
    LDAI(0x19);                              /* LDA #$19 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    BNE(sub_197B4);                          /* BNE sub_197B4 */

loc_19787:                                   /* $9787 */
    LDAI(0x1A);                              /* LDA #$1A */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSX(byte_5F8));                     /* LDA $5F8,X */
    ORA(ABSX(byte_612));                     /* ORA $612,X */
    BNE(sub_197B4);                          /* BNE sub_197B4 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0x88);                              /* CMP #$88 */
    BEQ(sub_197CF);                          /* BEQ sub_197CF */
    LDAI(7);                                 /* LDA #7 */
    STA(byte_15);                            /* STA $15 */

loc_1979F:                                   /* $979F */
    LDY(byte_15);                            /* LDY $15 */
    LDA(ABSY(0x97B9));                       /* LDA $97B9,Y */
    STA(byte_14);                            /* STA $14 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(sub_197B4);                          /* BCS sub_197B4 */
    LDAI(0x26);                              /* LDA #$26 */
    spawn_projectile();                      /* JSR spawn_projectile */
    DEC(byte_15);                            /* DEC $15 */
    BNE(loc_1979F);                          /* BNE loc_1979F */

sub_197B4:                                   /* $97B4 - entry of the next procedure ($97B4) */
    sub_197B4();                             /* falls through into $97B4 / BNE-BCS target */
    RTS();                                   /* RTS */

sub_197CF:                                   /* $97CF - entry of sub_197CF ($97CF) */
    sub_197CF();                             /* BEQ sub_197CF */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $97B4  sub_197B4 - common tail of the type 7..10 handlers
 *
 *  Original: src/bank6.asm:4805-4809 (bank 6)
 *      JSR process_enemy_flags_sprites
 *      JMP sub_181F6
 * ------------------------------------------------------------------------ */
void sub_197B4(void)                         /* $97B4 */
{
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JMP sub_181F6 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $97C7  enemy_type_8 - object type 8 handler
 *
 *  Original: src/bank6.asm:4829-4833 (bank 6)
 *      LDA $528,X
 *      BMI loc_19777
 *      JMP loc_19744
 *
 *  Both targets live inside enemy_type_7 ($973F).  Re-entering enemy_type_7()
 *  repeats the same "LDA $528,X / BMI" with the same value, so execution
 *  continues at loc_19744 or loc_19777 exactly like the original.
 * ------------------------------------------------------------------------ */
void enemy_type_8(void)                      /* $97C7 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19777);                          /* BMI loc_19777 */
    enemy_type_7();                          /* JMP loc_19744 (inside enemy_type_7) */
    RTS();                                   /* RTS */

loc_19777:                                   /* $9777 - entry point inside enemy_type_7 */
    enemy_type_7();                          /* BMI loc_19777 (re-enter at that label) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $97CF  sub_197CF - spawn the escort pair of object type $28
 *
 *  Original: src/bank6.asm:4839-4855 (bank 6)
 *      JSR check_enemy_overflow / BCS sub_197B4 / LDA #$28 / STA $528,Y /
 *      STY $15 / JSR check_enemy_overflow / BCS sub_197B4 / LDA #5 /
 *      STA $14 / JSR sub_197F3 / LDY $15 / LDA #$83 / STA $14 /
 *      JSR sub_197F3 / JMP sub_197B4
 * ------------------------------------------------------------------------ */
void sub_197CF(void)                         /* $97CF */
{
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(sub_197B4);                          /* BCS sub_197B4 */
    LDAI(0x28);                              /* LDA #$28 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    STY(byte_15);                            /* STY $15 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(sub_197B4);                          /* BCS sub_197B4 */
    LDAI(5);                                 /* LDA #5 */
    STA(byte_14);                            /* STA $14 */
    sub_197F3();                             /* JSR sub_197F3 */
    LDY(byte_15);                            /* LDY $15 */
    LDAI(0x83);                              /* LDA #$83 */
    STA(byte_14);                            /* STA $14 */
    sub_197F3();                             /* JSR sub_197F3 */

sub_197B4:                                   /* $97B4 - entry of sub_197B4 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $97F3  sub_197F3 - release a projectile of type $29 from slot Y
 *
 *  Original: src/bank6.asm:4861-4867 (bank 6)
 *      LDA #0
 *      STA $74A,Y
 *      LDA #$29
 *      JMP spawn_projectile
 * ------------------------------------------------------------------------ */
void sub_197F3(void)                         /* $97F3 */
{
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSY(0x074A));                       /* STA $74A,Y */
    LDAI(0x29);                              /* LDA #$29 */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $97FD  enemy_type_9 - object type 9 handler
 *
 *  Original: src/bank6.asm:4873-4889 (bank 6)
 *      LDA $528,X / BMI loc_19805 / JMP loc_19744 / loc_19805: DEC $6FC,X /
 *      BNE sub_197B4 / LDA #8 / STA $6FC,X / JSR check_enemy_overflow /
 *      BCS sub_197B4 / JSR sub_1981D / JSR spawn_projectile / JMP sub_197B4
 * ------------------------------------------------------------------------ */
void enemy_type_9(void)                      /* $97FD */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19805);                          /* BMI loc_19805 */
    enemy_type_7();                          /* JMP loc_19744 (inside enemy_type_7) */
    RTS();                                   /* RTS */

loc_19805:                                   /* $9805 */
    DEC(ABSX(byte_6FC));                     /* DEC $6FC,X */
    BNE(sub_197B4);                          /* BNE sub_197B4 */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(sub_197B4);                          /* BCS sub_197B4 */
    sub_1981D();                             /* JSR sub_1981D */
    spawn_projectile();                      /* JSR spawn_projectile */

sub_197B4:                                   /* $97B4 - entry of sub_197B4 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $981D  sub_1981D - random direction in $14, projectile type $14 in A
 *
 *  Original: src/bank6.asm:4895-4903 (bank 6)
 *      JSR enemy_prng / AND #3 / SBC #1 / STA $14 / LDA #$14 / RTS
 * ------------------------------------------------------------------------ */
void sub_1981D(void)                         /* $981D */
{
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(3);                                 /* AND #3 */
    SBCI(1);                                 /* SBC #1 */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x14);                              /* LDA #$14 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9829  enemy_type_10_blue_duster - Duster is such a boulder
 *
 *  Original: src/bank6.asm:4910-4938 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_10_blue_duster(void)         /* $9829 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1985D);                          /* BMI loc_1985D */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    LDYI(0);                                 /* LDY #0 */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    CMPI(0x80);                              /* CMP #$80 */
    BCS(loc_19840);                          /* BCS loc_19840 */
    DEY();                                   /* DEY */

loc_19840:                                   /* $9840 */
    TYA();                                   /* TYA */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDAI(0x2C);                              /* LDA #$2C */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x13);                              /* LDA #$13 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_612));                     /* STA $612,X */

loc_1985D:                                   /* $985D */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9860  enemy_type_69 - object type 69 handler
 *
 *  Original: src/bank6.asm:4944-4995 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_69(void)                     /* $9860 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19892);                          /* BMI loc_19892 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_542));                     /* STA $542,X */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_646));                     /* STA $646,X */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    CMPI(0x80);                              /* CMP #$80 */
    BCC(loc_19892);                          /* BCC loc_19892 */
    LDAI(0xFE);                              /* LDA #$FE */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    LDAI(5);                                 /* LDA #5 */
    STA(ABSX(byte_646));                     /* STA $646,X */

loc_19892:                                   /* $9892 */
    LDA(byte_62);                            /* LDA $62 */
    ANDI(8);                                 /* AND #8 */
    BNE(locret_198C5);                       /* BNE locret_198C5 */
    DEC(ABSX(0x074A));                       /* DEC $74A,X */
    BNE(locret_198C5);                       /* BNE locret_198C5 */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_198C5);                       /* BCS locret_198C5 */
    LDA(ABSX(byte_646));                     /* LDA $646,X */
    STA(byte_14);                            /* STA $14 */
    LDA(ABSX(byte_576));                     /* LDA $576,X */
    spawn_projectile();                      /* JSR spawn_projectile */
    DEC(ABSX(byte_590));                     /* DEC $590,X */
    BNE(loc_198BB);                          /* BNE loc_198BB */
    enemy_erase();                           /* JMP enemy_erase */
    RTS();                                   /* RTS */

loc_198BB:                                   /* $98BB */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_62C));                     /* ADC $62C,X */
    STA(ABSX(byte_55C));                     /* STA $55C,X */

locret_198C5:                                /* $98C5 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $98C6  enemy_type_11 - object type 11 handler (spawns a type 69)
 *
 *  Original: src/bank6.asm:5001-5017 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_11(void)                     /* $98C6 */
{
    LDA(byte_49);                            /* LDA $49 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ANDI(0x0E);                              /* AND #$E */
    TAY();                                   /* TAY */
    LDAI(69);                                /* LDA #69 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDA(ABSY(byte_198E7));                   /* LDA byte_198E7,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSY(byte_198E7 + 1));               /* LDA byte_198E7+1,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x28);                              /* LDA #$28 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    enemy_type_69();                         /* JMP enemy_type_69 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9918  sub_19918 - spawn a type 69 object from byte_198F7 (A = index)
 *
 *  Original: src/bank6.asm:5043-5062 (bank 6)
 *      PHA / JSR check_enemy_overflow / PLA / BCS locret_1993B / STX $10 /
 *      TAX / LDA #69 / STA $528,Y / LDA byte_198F7,X / STA $576,Y /
 *      LDA byte_198F7+1,X / STA $590,Y / LDA byte_198F7+2,X / STA $660,Y /
 *      LDX $10 / RTS
 * ------------------------------------------------------------------------ */
void sub_19918(void)                         /* $9918 */
{
    PHA();                                   /* PHA */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    PLA();                                   /* PLA */
    BCS(locret_1993B);                       /* BCS locret_1993B */
    STX(byte_10);                            /* STX $10 */
    TAX();                                   /* TAX */
    LDAI(69);                                /* LDA #69 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDA(ABSX(byte_198F7));                   /* LDA byte_198F7,X */
    STA(ABSY(byte_576));                     /* STA $576,Y */
    LDA(ABSX(byte_198F7 + 1));               /* LDA byte_198F7+1,X */
    STA(ABSY(byte_590));                     /* STA $590,Y */
    LDA(ABSX(byte_198F7 + 2));               /* LDA byte_198F7+2,X */
    STA(ABSY(byte_660));                     /* STA $660,Y */
    LDX(byte_10);                            /* LDX $10 */

locret_1993B:                                /* $993B */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $993C  enemy_type_12_13 - object types 12 and 13 handler
 *
 *  Original: src/bank6.asm:5068-5148 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_12_13(void)                  /* $993C */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1998D);                          /* BMI loc_1998D */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    ANDI(0x7F);                              /* AND #$7F */
    PHA();                                   /* PHA */
    enemy_prng();                            /* JSR enemy_prng */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    LDA(ABSY(0x99C4));                       /* LDA $99C4,Y */
    TAY();                                   /* TAY */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDA(ABSY(0x99D2));                       /* LDA $99D2,Y */
    STA(ABSX(byte_542));                     /* STA $542,X */
    LDA(ABSY(0x99D3));                       /* LDA $99D3,Y */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDA(ABSY(0x99D4));                       /* LDA $99D4,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(4);                                 /* LDA #4 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    LSR_A();                                 /* LSR */
    BCC(loc_1998D);                          /* BCC loc_1998D */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    EORI(0xFF);                              /* EOR #$FF */
    CLC();                                   /* CLC */
    ADCI(1);                                 /* ADC #1 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */

loc_1998D:                                   /* $998D */
    INC(ABSX(byte_764));                     /* INC $764,X */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ANDI(1);                                 /* AND #1 */
    CLC();                                   /* CLC */
    ADCI(0x1B);                              /* ADC #$1B */
    STA(ABSX(byte_576));                     /* STA $576,X */
    DEC(ABSX(byte_6FC));                     /* DEC $6FC,X */
    BNE(loc_199CD);                          /* BNE loc_199CD */
    LDA(ABSX(byte_6E2));                     /* LDA $6E2,X */
    INC(ABSX(byte_6E2));                     /* INC $6E2,X */
    TAY();                                   /* TAY */
    LDA(ABSY(0x99D5));                       /* LDA $99D5,Y */
    BPL(loc_199B2);                          /* BPL loc_199B2 */
    DEC(ABSX(byte_6E2));                     /* DEC $6E2,X */

loc_199B2:                                   /* $99B2 */
    TAY();                                   /* TAY */
    /* $99B3 loc_199B3 has no reference anywhere in the ROM (it is the
     * fall-through of the TAY above), so it stays a comment. */

    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    LSR_A();                                 /* LSR */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    BCC(loc_199C0);                          /* BCC loc_199C0 */
    EORI(0xFF);                              /* EOR #$FF */
    SEC();                                   /* SEC */
    SBCI(7);                                 /* SBC #7 */

loc_199C0:                                   /* $99C0 */
    ANDI(0x0F);                              /* AND #$F */
    set_speed_by_slope();                    /* JSR set_speed_by_slope */
    PLA();                                   /* PLA */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ANDI(0x1C);                              /* AND #$1C */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */

loc_199CD:                                   /* $99CD */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $99F6  enemy_type_14 - object type 14 handler
 *
 *  Original: src/bank6.asm:5193-5315 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_14(void)                     /* $99F6 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19A21);                          /* BMI loc_19A21 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0xC0);                              /* LDA #$C0 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x26);                              /* LDA #$26 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */

loc_19A21:                                   /* $9A21 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BNE(loc_19A8C);                          /* BNE loc_19A8C */
    LDA(byte_542);                           /* LDA $542 */
    SEC();                                   /* SEC */
    SBC(ABSX(byte_542));                     /* SBC $542,X */
    BCC(loc_19A36);                          /* BCC loc_19A36 */
    CMPI(0x60);                              /* CMP #$60 */
    BCC(loc_19A36);                          /* BCC loc_19A36 */
    sub_197B4();                             /* JMP sub_197B4 */

loc_19A36:                                   /* $9A36 */
    INC(ABSX(byte_5AA));                     /* INC $5AA,X */
    LDAI(0x15);                              /* LDA #$15 */
    j_apu_play();                            /* JSR $C030 (j_apu_play) */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x0C);                              /* LDA #$C */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDAI(0x30);                              /* LDA #$30 */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(0x27);                              /* LDA #$27 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19AC9);                          /* BCS loc_19AC9 */
    LDAI(4);                                 /* LDA #4 */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x3B);                              /* LDA #$3B */
    spawn_projectile();                      /* JSR spawn_projectile */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19AC9);                          /* BCS loc_19AC9 */
    INC(byte_14);                            /* INC $14 */
    LDAI(0x3B);                              /* LDA #$3B */
    spawn_projectile();                      /* JSR spawn_projectile */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19AC9);                          /* BCS loc_19AC9 */
    LDAI(3);                                 /* LDA #3 */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x3B);                              /* LDA #$3B */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19A8C:                                   /* $9A8C */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    CMPI(1);                                 /* CMP #1 */
    BNE(loc_19AC9);                          /* BNE loc_19AC9 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19AC9);                          /* BCS loc_19AC9 */
    INC(ABSX(byte_5AA));                     /* INC $5AA,X */
    LDAI(0x8E);                              /* LDA #$8E */
    spawn_projectile();                      /* JSR spawn_projectile */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSY(byte_660));                     /* STA $660,Y */
    STA(ABSY(byte_5AA));                     /* STA $5AA,Y */
    STA(ABSY(0x074A));                       /* STA $74A,Y */
    LDAI(0x0C);                              /* LDA #$C */
    STA(ABSY(byte_716));                     /* STA $716,Y */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSY(0x0730));                       /* STA $730,Y */
    STA(ABSY(byte_6E2));                     /* STA $6E2,Y */
    LDAI(0x27);                              /* LDA #$27 */
    STA(ABSY(byte_576));                     /* STA $576,Y */
    LDAI(0x42);                              /* LDA #$42 */
    STA(ABSY(byte_590));                     /* STA $590,Y */
    LDAI(0x30);                              /* LDA #$30 */
    STA(ABSY(byte_764));                     /* STA $764,Y */
    BNE(loc_19AE6);                          /* BNE loc_19AE6 */

loc_19AC9:                                   /* $9AC9 */
    DEC(ABSX(byte_6E2));                     /* DEC $6E2,X */
    BNE(loc_19AE6);                          /* BNE loc_19AE6 */
    LDAI(6);                                 /* LDA #6 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDA(ABSX(byte_716));                     /* LDA $716,X */
    CMPI(4);                                 /* CMP #4 */
    BEQ(loc_19AE6);                          /* BEQ loc_19AE6 */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0730));                       /* ADC $730,X */
    ANDI(0x0F);                              /* AND #$F */
    STA(ABSX(byte_716));                     /* STA $716,X */
    set_speed_by_slope();                    /* JSR set_speed_by_slope */

loc_19AE6:                                   /* $9AE6 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JSR sub_181F6 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    BNE(locret_19AF6);                       /* BNE locret_19AF6 */
    LDAI(0x0B);                              /* LDA #$B */
    sub_C057();                              /* JMP sub_C057 */
    RTS();                                   /* RTS */

locret_19AF6:                                /* $9AF6 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9AF7  enemy_type_15 - object type 15 handler
 *
 *  Original: src/bank6.asm:5321-5392 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_15(void)                     /* $9AF7 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19B25);                          /* BMI loc_19B25 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDA(byte_55C);                           /* LDA $55C */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0x43);                              /* LDA #$43 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDAI(0x0A);                              /* LDA #$A */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(5);                                 /* LDA #5 */
    STA(ABSX(0x0694));                       /* STA $694,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */

loc_19B25:                                   /* $9B25 */
    DEC(ABSX(0x06AE));                       /* DEC $6AE,X */
    BNE(loc_19B53);                          /* BNE loc_19B53 */
    LDAI(0x0A);                              /* LDA #$A */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */
    calculate_slope_and_speed();             /* JSR calculate_slope_and_speed */
    LDAI(0x40);                              /* LDA #$40 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMP(byte_542);                           /* CMP $542 */
    BCC(loc_19B53);                          /* BCC loc_19B53 */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x82);                              /* LDA #$82 */
    STA(ABSX(byte_590));                     /* STA $590,X */

loc_19B53:                                   /* $9B53 */
    LDA(byte_55C);                           /* LDA $55C */
    SEC();                                   /* SEC */
    SBC(ABSX(byte_55C));                     /* SBC $55C,X */
    CLC();                                   /* CLC */
    ADCI(0x10);                              /* ADC #$10 */
    CMPI(0x1F);                              /* CMP #$1F */
    BCS(loc_19B80);                          /* BCS loc_19B80 */
    DEC(ABSX(0x0694));                       /* DEC $694,X */
    BNE(loc_19B80);                          /* BNE loc_19B80 */
    LDAI(5);                                 /* LDA #5 */
    STA(ABSX(0x0694));                       /* STA $694,X */
    LDYI(4);                                 /* LDY #4 */
    LDA(ABSX(byte_590));                     /* LDA $590,X */
    BPL(loc_19B74);                          /* BPL loc_19B74 */
    LDYI(0x0C);                              /* LDY #$C */

loc_19B74:                                   /* $9B74 */
    STY(byte_14);                            /* STY $14 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19B80);                          /* BCS loc_19B80 */
    LDAI(0x3B);                              /* LDA #$3B */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19B80:                                   /* $9B80 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JSR sub_181F6 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9B8A  enemy_type_48_maseru - capital ship Maseru (cloaks)
 *
 *  Original: src/bank6.asm:5399-5480 (bank 6)
 *
 *  "JMP loc_19C85" ($9C23) is a tail inside enemy_type_49_50_52 ($9C85); it
 *  is reproduced literally below (PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void enemy_type_48_maseru(void)              /* $9B8A */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19BA1);                          /* BMI loc_19BA1 */
    set_capital_ship_parameters();           /* JSR set_capital_ship_parameters */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(0x0694));                       /* STA $694,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSX(unk_67A));                      /* STA $67A,X */

loc_19BA1:                                   /* $9BA1 */
    capital_ships_health_check();            /* JSR capital_ships_health_check */
    sub_19CAB();                             /* JSR sub_19CAB */
    /* sub_19CAB discards its own return address (PLA/PLA at $9CB0) when
     * $5AA,X is non-zero, so its RTS resumes the dispatcher's slot loop and the
     * rest of this handler never runs (bank6_report.md section 3.3).  A C
     * callee cannot drop its caller's frame, so the port repeats sub_19CAB's
     * test here - sub_19CAB leaves $5AA,X unchanged - and returns itself. */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X (the unwind test) */
    BNE(locret_19C26);                       /* abandon: resume the slot loop */
    DEC(ABSX(0x0694));                       /* DEC $694,X */
    BNE(loc_19BE6);                          /* BNE loc_19BE6 */
    INC(ABSX(byte_660));                     /* INC $660,X */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ANDI(3);                                 /* AND #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    TAY();                                   /* TAY */
    CMPI(1);                                 /* CMP #1 */
    BNE(loc_19BCC);                          /* BNE loc_19BCC */
    DEC(ABSX(unk_67A));                      /* DEC $67A,X */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(0x7F);                              /* AND #$7F */
    ADCI(0x10);                              /* ADC #$10 */
    STA(ABSX(byte_542));                     /* STA $542,X */

loc_19BCC:                                   /* $9BCC */
    LDA(ABSY(byte_19C27));                   /* LDA byte_19C27,Y */
    STA(ABSX(0x0694));                       /* STA $694,X */
    CPYI(2);                                 /* CPY #2 */
    BCC(loc_19C04);                          /* BCC loc_19C04 */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    BNE(loc_19BE1);                          /* BNE loc_19BE1 */
    INC(ABSX(unk_67A));                      /* INC $67A,X */
    STA(ABSX(0x0694));                       /* STA $694,X */

loc_19BE1:                                   /* $9BE1 */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(0x074A));                       /* STA $74A,X */

loc_19BE6:                                   /* $9BE6 */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    BEQ(loc_19C04);                          /* BEQ loc_19C04 */
    DEC(ABSX(0x074A));                       /* DEC $74A,X */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    INC(ABSX(0x06AE));                       /* INC $6AE,X */
    ANDI(7);                                 /* AND #7 */
    SBCI(3);                                 /* SBC #3 */
    STA(byte_14);                            /* STA $14 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19C04);                          /* BCS loc_19C04 */
    LDAI(0x14);                              /* LDA #$14 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19C04:                                   /* $9C04 */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    BEQ(locret_19C26);                       /* BEQ locret_19C26 */
    CMPI(2);                                 /* CMP #2 */
    BEQ(loc_19C17);                          /* BEQ loc_19C17 */
    LDA(ABSX(0x0694));                       /* LDA $694,X */
    ANDI(1);                                 /* AND #1 */
    BEQ(locret_19C26);                       /* BEQ locret_19C26 */
    add_enemy_meta_sprite();                 /* JMP add_enemy_meta_sprite */
    RTS();                                   /* RTS */

loc_19C17:                                   /* $9C17 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JSR sub_181F6 */
    sub_19D23();                             /* JSR sub_19D23 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    /* JMP loc_19C85: the tail from $9C85 on lives inside
     * enemy_type_49_50_52 and is reproduced here literally. */
    BNE(locret_19C96);                       /* JMP loc_19C85 -> BNE locret_19C96 */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */
    LDAI(0xB0);                              /* LDA #$B0 */
    STA(ABSX(byte_528));                     /* STA $528,X */

locret_19C26:                                /* $9C26 */
    RTS();                                   /* RTS */

locret_19C96:                                /* $9C96 - shared RTS inside enemy_type_49_50_52 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9C2B  enemy_type_51_girevu - capital ship Girevu (shoots all around)
 *
 *  Original: src/bank6.asm:5492-5498 (bank 6)
 *      LDA $694,X
 *      CMP #120
 *      BNE enemy_type_49_50_52
 *      LDA #$45
 *      STA $576,X
 *  (no RTS: both paths continue into enemy_type_49_50_52)
 * ------------------------------------------------------------------------ */
void enemy_type_51_girevu(void)              /* $9C2B */
{
    LDA(ABSX(0x0694));                       /* LDA $694,X */
    CMPI(120);                               /* CMP #120 */
    BNE(enemy_type_49_50_52);                /* BNE enemy_type_49_50_52 */
    LDAI(0x45);                              /* LDA #$45 */
    STA(ABSX(byte_576));                     /* STA $576,X */

enemy_type_49_50_52:                         /* $9C37 - the next procedure (fall-through) */
    enemy_type_49_50_52();                   /* falls through into $9C37 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9C37  enemy_type_49_50_52 - capital ships Hume, Soreido and Somasu
 *
 *  Original: src/bank6.asm:5505-5556 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_49_50_52(void)               /* $9C37 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19C56);                          /* BMI loc_19C56 */
    set_capital_ship_parameters();           /* JSR set_capital_ship_parameters */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDAI(30);                                /* LDA #30 */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(100);                               /* LDA #100 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */

loc_19C56:                                   /* $9C56 */
    update_chase_counters();                 /* JSR update_chase_counters */
    BCC(loc_19C65);                          /* BCC loc_19C65 */
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(0x0F);                              /* AND #$F */
    ADCI(0x14);                              /* ADC #$14 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */

loc_19C65:                                   /* $9C65 */
    DEC(ABSX(0x0694));                       /* DEC $694,X */
    BNE(loc_19C73);                          /* BNE loc_19C73 */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    sub_19DAF();                             /* JSR sub_19DAF */

loc_19C73:                                   /* $9C73 */
    capital_ships_health_check();            /* JSR capital_ships_health_check */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JSR sub_181F6 */
    sub_19D23();                             /* JSR sub_19D23 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    goto loc_19C85;                          /* JMP loc_19C85 */

loc_19C85:                                   /* $9C85 */
    BNE(locret_19C96);                       /* BNE locret_19C96 */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */
    LDAI(0xB0);                              /* LDA #$B0 */
    STA(ABSX(byte_528));                     /* STA $528,X */

locret_19C96:                                /* $9C96 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9C97  capital_ships_health_check - damage sprite from the health
 *
 *  Original: src/bank6.asm:5563-5580 (bank 6)
 *      LDA $764,X / CMP #12 / BCC loc_19C9F / RTS / loc_19C9F: CMP #4 /
 *      LDA #3 / BCS loc_19CA7 / LDA #1 / loc_19CA7: STA $590,X / RTS
 * ------------------------------------------------------------------------ */
void capital_ships_health_check(void)        /* $9C97 */
{
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    CMPI(12);                                /* CMP #12 */
    BCC(loc_19C9F);                          /* BCC loc_19C9F */
    RTS();                                   /* RTS */

loc_19C9F:                                   /* $9C9F */
    CMPI(4);                                 /* CMP #4 */
    LDAI(3);                                 /* LDA #3 */
    BCS(loc_19CA7);                          /* BCS loc_19CA7 */
    LDAI(1);                                 /* LDA #1 */

loc_19CA7:                                   /* $9CA7 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9CAB  sub_19CAB - capital ship explosion sequence
 *
 *  Original: src/bank6.asm:5586-5618 (bank 6)
 *
 *  The two PLAs at $9CB0 are an unwind: they drop the return address pushed
 *  by the JSR that entered this routine, so every RTS below returns to the
 *  caller's caller - the dispatcher's slot loop at $8807 - abandoning
 *  enemy_type_48_maseru, the only caller (bank6_report.md section 3.3).  The
 *  port performs no stack traffic; the call site reproduces the abandon.
 *
 *  The "DEC $15 / BNE loc_19CD2 / BEQ locret_19C96" tail always reaches the
 *  BEQ with Z set, so the BEQ at $9CE4 is always taken and the bytes of
 *  set_capital_ship_parameters that follow it in the ROM are never reached
 *  from here: the port ends at the shared RTS.
 * ------------------------------------------------------------------------ */
void sub_19CAB(void)                         /* $9CAB */
{
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BEQ(locret_19C96);                       /* BEQ locret_19C96 */

    /* $9CB0 PLA / PLA - drop this routine's own return address (see above) */
    add_enemy_meta_sprite();                 /* JSR add_enemy_meta_sprite */
    DEC(ABSX(0x06AE));                       /* DEC $6AE,X */
    BEQ(loc_19CC4);                          /* BEQ loc_19CC4 */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    ANDI(7);                                 /* AND #7 */
    BNE(locret_19C96);                       /* BNE locret_19C96 */
    /* JMP loc_1B48F: loc_1B48F ($B48F) is the tail of sub_1B482 (chunk
     * bank6_B44B); it loads A with $1F and falls through into sub_1B491, so
     * the port sets A and calls sub_1B491, which runs the same chain. */
    LDAI(0x1F);                              /* JMP loc_1B48F -> LDA #$1F */
    sub_1B491();                             /* falls through into sub_1B491 */
    RTS();                                   /* RTS */

loc_19CC4:                                   /* $9CC4 */
    LDAI(0x5B);                              /* LDA #$5B */
    STA(ABSX(byte_528));                     /* STA $528,X */
    enemy_prng();                            /* JSR enemy_prng */
    STA(byte_14);                            /* STA $14 */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_15);                            /* STA $15 */

loc_19CD2:                                   /* $9CD2 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_19C96);                       /* BCS locret_19C96 */
    LDAI(0x3B);                              /* LDA #$3B */
    spawn_projectile();                      /* JSR spawn_projectile */
    INC(byte_14);                            /* INC $14 */
    INC(byte_14);                            /* INC $14 */
    DEC(byte_15);                            /* DEC $15 */
    BNE(loc_19CD2);                          /* BNE loc_19CD2 */
    BEQ(locret_19C96);                       /* BEQ locret_19C96 */

locret_19C96:                                /* $9C96 - shared RTS inside enemy_type_49_50_52 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9CE6  set_capital_ship_parameters - copy the capital ship's table row
 *
 *  Original: src/bank6.asm:5625-5642 (bank 6)
 *      TAY / ORA #$80 / STA $528,X / LDA $9CDF,Y / STA $576,X /
 *      LDA $9CE4,Y / STA $764,X / LDA $9CE9,Y / STA $67A,X / STA $694,X /
 *      LDA $9CEE,Y / STA $74A,X / LDA #2 / STA $590,X /
 *      JMP spawn_top_random_x_position
 *
 *  The tables are indexed with the raw enemy type (48...52), so the operands
 *  are label-48 ($9D0F-48 = $9CDF, and so on).
 * ------------------------------------------------------------------------ */
void set_capital_ship_parameters(void)       /* $9CE6 */
{
    TAY();                                   /* TAY */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDA(ABSY(0x9CDF));                       /* LDA $9CDF,Y (capital_ships_param1-48) */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSY(0x9CE4));                       /* LDA $9CE4,Y (capital_ships_param_health-48) */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDA(ABSY(0x9CE9));                       /* LDA $9CE9,Y (capital_ships_param3-48) */
    STA(ABSX(unk_67A));                      /* STA $67A,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    LDA(ABSY(0x9CEE));                       /* LDA $9CEE,Y (capital_ships_param4-48) */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDAI(2);                                 /* LDA #2 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    spawn_top_random_x_position();           /* JMP spawn_top_random_x_position */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9D23  sub_19D23 - capital ship: launch or self-destruct
 *
 *  Original: src/bank6.asm:5653-5707 (bank 6)
 * ------------------------------------------------------------------------ */
void sub_19D23(void)                         /* $9D23 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(locret_19D79);                       /* BMI locret_19D79 */
    LDAI(3);                                 /* LDA #3 */
    LDY(0x008A);                             /* LDY $8A */
    BEQ(loc_19D4A);                          /* BEQ loc_19D4A */
    CPYI(4);                                 /* CPY #4 */
    BNE(loc_19D5A);                          /* BNE loc_19D5A */
    LDA(special_weapon_type);                /* LDA $5C */
    CMPI(1);                                 /* CMP #1 */
    BNE(loc_19D40);                          /* BNE loc_19D40 */
    LDA(unk_60);                             /* LDA $60 */
    CMPI(5);                                 /* CMP #5 */
    BEQ(loc_19D48);                          /* BEQ loc_19D48 */
    BNE(loc_19D5A);                          /* BNE loc_19D5A */

loc_19D40:                                   /* $9D40 */
    CMPI(5);                                 /* CMP #5 */
    BNE(loc_19D5A);                          /* BNE loc_19D5A */
    CMP(unk_60);                             /* CMP $60 */
    BNE(loc_19D5A);                          /* BNE loc_19D5A */

loc_19D48:                                   /* $9D48 */
    LDAI(1);                                 /* LDA #1 */

loc_19D4A:                                   /* $9D4A */
    DEC(ABSX(0x0730));                       /* DEC $730,X */
    AND(ABSX(0x0730));                       /* AND $730,X */
    BEQ(locret_19D79);                       /* BEQ locret_19D79 */
    LDA(0x0088);                             /* LDA $88 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    RTS();                                   /* RTS */

loc_19D5A:                                   /* $9D5A */
    LDA(0x0089);                             /* LDA $89 */
    STA(ABSY(0x07E6));                       /* STA $7E6,Y */
    LDAI(0x18);                              /* LDA #$18 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDAI(0x0A);                              /* LDA #$A */
    STA(ABSY(byte_716));                     /* STA $716,Y */
    LDAI(0x42);                              /* LDA #$42 */
    STA(ABSY(0x074A));                       /* STA $74A,Y */
    enemy_prng();                            /* JSR enemy_prng */
    STA(ABSY(byte_77E));                     /* STA $77E,Y */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSY(byte_5AA));                     /* STA $5AA,Y */

locret_19D79:                                /* $9D79 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9D7A  enemy_type_24 - object type 24 handler
 *
 *  Original: src/bank6.asm:5713-5743 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_24(void)                     /* $9D7A */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19D94);                          /* BMI loc_19D94 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BNE(loc_19D94);                          /* BNE loc_19D94 */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDA(ABSX(byte_77E));                     /* LDA $77E,X */
    set_speed_by_slope();                    /* JSR set_speed_by_slope */

loc_19D94:                                   /* $9D94 */
    DEC(ABSX(byte_716));                     /* DEC $716,X */
    BEQ(loc_19D9D);                          /* BEQ loc_19D9D */

loc_19D99:                                   /* $9D99 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    RTS();                                   /* RTS */

loc_19D9D:                                   /* $9D9D */
    CPXI(4);                                 /* CPX #4 */
    BEQ(loc_19D99);                          /* BEQ loc_19D99 */
    LDA(ABSX(0x07E6));                       /* LDA $7E6,X */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181A7();                             /* JMP sub_181A7 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9DAF  sub_19DAF - capital ship: pick and launch a weapon
 *
 *  Original: src/bank6.asm:5749-5813 (bank 6)
 * ------------------------------------------------------------------------ */
void sub_19DAF(void)                         /* $9DAF */
{
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCC(loc_19DB5);                          /* BCC loc_19DB5 */
    RTS();                                   /* RTS */

loc_19DB5:                                   /* $9DB5 */
    STY(byte_19);                            /* STY $19 */
    calculate_slope();                       /* JSR calculate_slope */
    STA(byte_14);                            /* STA $14 */
    LDY(byte_19);                            /* LDY $19 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xB1);                              /* CMP #$B1 */
    BNE(loc_19DCB);                          /* BNE loc_19DCB */
    sub_19E2A();                             /* JSR sub_19E2A */
    sub_197F3();                             /* JMP sub_197F3 */
    RTS();                                   /* RTS */

loc_19DCB:                                   /* $9DCB */
    CMPI(0xB2);                              /* CMP #$B2 */
    BNE(loc_19DEB);                          /* BNE loc_19DEB */
    LDAI(0x4B);                              /* LDA #$4B */
    STA(ABSY(byte_576));                     /* STA $576,Y */
    LDAI(0x18);                              /* LDA #$18 */
    STA(ABSY(byte_764));                     /* STA $764,Y */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSY(byte_660));                     /* STA $660,Y */
    enemy_prng();                            /* JSR enemy_prng */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x24);                              /* LDA #$24 */
    STA(ABSY(byte_5AA));                     /* STA $5AA,Y */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();                                   /* RTS */

loc_19DEB:                                   /* $9DEB */
    CMPI(0xB3);                              /* CMP #$B3 */
    BNE(loc_19E08);                          /* BNE loc_19E08 */
    LDAI(0x46);                              /* LDA #$46 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(0x26);                              /* LDA #$26 */
    STA(byte_18);                            /* STA $18 */
    LDAI(0x10);                              /* LDA #$10 */
    STA(byte_16);                            /* STA $16 */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_17);                            /* STA $17 */
    sub_1B5CF();                             /* JSR sub_1B5CF */
    LDAI(0x16);                              /* LDA #$16 */
    j_apu_play();                            /* JMP j_apu_play */
    RTS();                                   /* RTS */

loc_19E08:                                   /* $9E08 */
    sub_19E2A();                             /* JSR sub_19E2A */
    LDAI(0x29);                              /* LDA #$29 */
    spawn_projectile();                      /* JSR spawn_projectile */
    INC(ABSX(0x06C8));                       /* INC $6C8,X */
    LDA(ABSX(0x06C8));                       /* LDA $6C8,X */
    ANDI(3);                                 /* AND #3 */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_19E26));                    /* LDA unk_19E26,Y */
    LDY(byte_19);                            /* LDY $19 */
    STA(ABSY(0x074A));                       /* STA $74A,Y */
    TXA();                                   /* TXA */
    STA(ABSY(byte_5AA));                     /* STA $5AA,Y */
    RTS();                                   /* RTS */
}
