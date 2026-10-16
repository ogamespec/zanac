/* bank6/AD25.c - bank 6, $AD25-$B44A: the enemy type handlers 54, 55, 62,
 * 70-79, 81, 82, 88, 89, the formation / meta-sprite helpers ($B09A-$B267) and
 * the fortress-core code ($B337-$B44A).
 *
 * Ported from src/bank6.asm lines 8194-9517 (addresses from Build/bank6.lst).
 *
 * Control flow that needs a comment (see PORTING.md section 5):
 *   - several procedures have no RTS and fall through into the next one; the
 *     port reproduces that as a tail call followed by RTS();
 *   - sub_1B09A branches forward into the entry point of sub_1B0AA ($B0AA);
 *   - enemy_type_82_weapon_distro jumps to $AEC6 / $AED8, which are inner
 *     labels of enemy_type_70_71_81_88_89, and enemy_type_79 jumps to $B3A5,
 *     an inner label of sub_1B39E.  C cannot goto into another function, so
 *     those shared tails are reproduced inside the jumping procedure under
 *     local labels of the same name (the PORTING.md rule for a shared tail);
 *   - enemy_type_73_74_75_76_77_78 and enemy_type_79 branch to the entry point
 *     fortress_core_appears, and sub_1B39E branches to sub_1B467; the port
 *     reproduces those with a local label that calls the procedure;
 *   - sub_1B2ED has a second entry point at $B2EF (sub_1946F jumps there from
 *     another chunk); it is not a local goto target, so it is documented where
 *     it occurs instead of being declared as a C label (an unused label would
 *     be a warning);
 *   - the whole range contains no inline parameter block, no switch_case table
 *     and no computed RTS / JMP.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $AD25  enemy_type_54
 *
 *  Original: src/bank6.asm:8194-8204 (bank 6)
 *      JSR sub_1B87F
 *      BCC loc_1AD2F
 *      LDA #$A
 *      STA $764,X
 *  loc_1AD2F:
 *      JSR sub_18204
 *      JSR decrease_enemy_health
 *      RTS
 * ------------------------------------------------------------------------ */
void enemy_type_54(void)                     /* $AD25 */
{
    sub_1B87F();                             /* JSR sub_1B87F */
    BCC(loc_1AD2F);                          /* BCC loc_1AD2F */
    LDAI(0x0A);                              /* LDA #$A */
    STA(ABSX(byte_764));                     /* STA $764,X */

loc_1AD2F:                                   /* $AD2F */
    sub_18204();                             /* JSR sub_18204 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $AD36  enemy_type_55_fairy - Rio (Fairy)
 *
 *  Original: src/bank6.asm:8211-8335 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_55_fairy(void)               /* $AD36 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1AD68);                          /* BMI loc_1AD68 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */
    STA(ABSX(byte_660));                     /* STA $660,X */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0xFE);                              /* LDA #$FE */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(0x0A);                              /* LDA #$A */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    STA(ABSX(0x06C8));                       /* STA $6C8,X */

loc_1AD68:                                   /* $AD68 */
    INC(ABSX(0x06AE));                       /* INC $6AE,X */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    ANDI(0x02);                              /* AND #2 */
    LSR_A();                                 /* LSR */
    CLC();                                   /* CLC */
    ADCI(0x4E);                              /* ADC #$4E */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSX(byte_590));                     /* LDA $590,X */
    BNE(loc_1AD9D);                          /* BNE loc_1AD9D */
    sub_181ED();                             /* JSR sub_181ED */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BPL(loc_1AD88);                          /* BPL loc_1AD88 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    RTS();                                   /* RTS */

loc_1AD88:                                   /* $AD88 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    INC(ABSX(byte_590));                     /* INC $590,X */
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA $528 */
    sub_1B2ED();                             /* JSR sub_1B2ED */
    LDAI(0x03);                              /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */

loc_1AD9D:                                   /* $AD9D */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    BEQ(loc_1ADA7);                          /* BEQ loc_1ADA7 */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */

loc_1ADA7:                                   /* $ADA7 */
    update_chase_counters();                 /* JSR update_chase_counters */
    BCC(loc_1ADB7);                          /* BCC loc_1ADB7 */
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(0x0F);                              /* AND #$F */
    ADC(ABSX(byte_6E2));                     /* ADC $6E2,X */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */

loc_1ADB7:                                   /* $ADB7 */
    LDA(ABSX(0x06C8));                       /* LDA $6C8,X */
    TAY();                                   /* TAY */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    CMPI(0xB0);                              /* CMP #$B0 */
    BCC(loc_1ADC6);                          /* BCC loc_1ADC6 */
    CMPI(0xB5);                              /* CMP #$B5 */
    BCC(loc_1ADD4);                          /* BCC loc_1ADD4 */

loc_1ADC6:                                   /* $ADC6 */
    INY();                                   /* INY */
    CPYI(0x1A);                              /* CPY #$1A */
    BCC(loc_1ADCD);                          /* BCC loc_1ADCD */
    LDYI(0x05);                              /* LDY #5 */

loc_1ADCD:                                   /* $ADCD */
    TYA();                                   /* TYA */
    STA(ABSX(0x06C8));                       /* STA $6C8,X */
    goto loc_1AE07;                          /* JMP loc_1AE07 */

loc_1ADD4:                                   /* $ADD4 */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    ANDI(0x07);                              /* AND #7 */
    BNE(loc_1AE07);                          /* BNE loc_1AE07 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMP(ABSY(byte_542));                     /* CMP $542,Y */
    PHP();                                   /* PHP */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMP(0x0542);                             /* CMP $542 */
    LDAI(0x00);                              /* LDA #0 */
    ROL_A();                                 /* ROL */
    STA(byte_10);                            /* STA $10 */
    LDAI(0x00);                              /* LDA #0 */
    PLP();                                   /* PLP */
    ROL_A();                                 /* ROL */
    EOR(byte_10);                            /* EOR $10 */
    BEQ(loc_1AE07);                          /* BEQ loc_1AE07 */
    LDA(ABSY(byte_764));                     /* LDA $764,Y */
    SEC();                                   /* SEC */
    SBCI(0x01);                              /* SBC #1 */
    BNE(loc_1ADFF);                          /* BNE loc_1ADFF */
    LDAI(0x01);                              /* LDA #1 */

loc_1ADFF:                                   /* $ADFF */
    STA(ABSY(byte_764));                     /* STA $764,Y */
    LDAI(0x14);                              /* LDA #$14 */
    j_apu_play();                            /* JSR j_apu_play */

loc_1AE07:                                   /* $AE07 */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x22);                              /* AND #$22 */
    CMPI(0x02);                              /* CMP #2 */
    BNE(loc_1AE16);                          /* BNE loc_1AE16 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_C2);                            /* STA $C2 */
    DEC(ABSX(byte_764));                     /* DEC $764,X */

loc_1AE16:                                   /* $AE16 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $AE1A  enemy_type_62
 *
 *  Original: src/bank6.asm:8341-8415 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_62(void)                     /* $AE1A */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1AE3D);                          /* BMI loc_1AE3D */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x06);                              /* LDA #6 */
    STA(ABSX(byte_764));                     /* STA $764,X */

loc_1AE3D:                                   /* $AE3D */
    INC(ABSX(0x06AE));                       /* INC $6AE,X */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ANDI(0x01);                              /* AND #1 */
    CLC();                                   /* CLC */
    ADCI(0x24);                              /* ADC #$24 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JSR sub_181F6 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(locret_1AE98);                       /* BMI locret_1AE98 */
    LDA(0x008A);                             /* LDA $8A */
    BEQ(loc_1AE74);                          /* BEQ loc_1AE74 */
    DEC(ABSX(byte_764));                     /* DEC $764,X */
    BNE(loc_1AE6D);                          /* BNE loc_1AE6D */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_590));                     /* STA $590,X */

loc_1AE6D:                                   /* $AE6D */
    LDAI(0xBE);                              /* LDA #$BE */
    STA(ABSX(byte_528));                     /* STA $528,X */
    BNE(locret_1AE98);                       /* BNE locret_1AE98 */

loc_1AE74:                                   /* $AE74 */
    LDA(ABSX(byte_590));                     /* LDA $590,X */
    BEQ(loc_1AE88);                          /* BEQ loc_1AE88 */
    LDY(byte_51);                            /* LDY $51 */
    CPYI(0x06);                              /* CPY #6 */
    DEY();                                   /* DEY */
    BNE(loc_1AE81);                          /* BNE loc_1AE81 */
    INY();                                   /* INY */

loc_1AE81:                                   /* $AE81 */
    BCC(loc_1AE85);                          /* BCC loc_1AE85 */
    LDYI(0x05);                              /* LDY #5 */

loc_1AE85:                                   /* $AE85 */
    TYA();                                   /* TYA */
    STA(byte_51);                            /* STA $51 */

loc_1AE88:                                   /* $AE88 */
    LDAI(0x08);                              /* LDA #8 */
    j_apu_play();                            /* JSR $C030 */
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA $528 */
    INC(byte_32);                            /* INC $32 */
    BNE(locret_1AE98);                       /* BNE locret_1AE98 */
    DEC(byte_32);                            /* DEC $32 */

locret_1AE98:                                /* $AE98 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $AE99  enemy_type_82_weapon_distro
 *
 *  Original: src/bank6.asm:8422-8441 (bank 6)
 *      JSR sub_1B87F
 *      BCS loc_1AEC6
 *      ... setup ...
 *      JMP loc_1AED8
 *
 *  Both exits of this procedure jump into enemy_type_70_71_81_88_89, at $AEC6
 *  and $AED8.  The shared tail is reproduced below under local labels of the
 *  same name; see the file header.
 * ------------------------------------------------------------------------ */
void enemy_type_82_weapon_distro(void)       /* $AE99 */
{
    sub_1B87F();                             /* JSR sub_1B87F */
    BCS(loc_1AEC6);                          /* BCS loc_1AEC6 ($AEC6) */
    LDA(ABSX(byte_716));                     /* LDA $716,X */
    ASL_A();                                 /* ASL */
    STA(byte_85);                            /* STA $85 */
    LDAI(0xF7);                              /* LDA #$F7 */
    STA(byte_84);                            /* STA $84 */
    LDAI(0xEC);                              /* LDA #$EC */
    STA(byte_87);                            /* STA $87 */
    LDAI(0x01);                              /* LDA #1 */
    STA(byte_86);                            /* STA $86 */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    add_next_sprite_from_zeropage();         /* JSR add_next_sprite_from_zeropage */
    LDA(byte_C0);                            /* LDA $C0 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_C0);                            /* STA $C0 */
    goto loc_1AED8;                          /* JMP loc_1AED8 ($AED8) */

    /* -------------------------------------------------------------------- *
     *  Shared tail of enemy_type_70_71_81_88_89 ($AEC6..$AF99), reproduced
     *  here because the original jumps into the middle of that procedure
     *  (PORTING.md section 5, "shared tail").  The two entry points are
     *  loc_1AEC6 ($AEC6) and loc_1AED8 ($AED8).
     * -------------------------------------------------------------------- */

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
    LDAI(0x08);                              /* LDA #8 */
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
    RTS();

loc_1AF28:                                   /* $AF28 */
    CMPI(0x52);                              /* CMP #$52 */
    BNE(loc_1AF49);                          /* BNE loc_1AF49 */
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(0x01);                              /* LDA #1 */
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
    RTS();

loc_1AF49:                                   /* $AF49 */
    CMPI(0x58);                              /* CMP #$58 */
    BCS(loc_1AF83);                          /* BCS loc_1AF83 */
    CMPI(0x54);                              /* CMP #$54 */
    BNE(loc_1AF5B);                          /* BNE loc_1AF5B */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x11);                              /* LDY #$11 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();

loc_1AF5B:                                   /* $AF5B */
    CMPI(0x57);                              /* CMP #$57 */
    BNE(loc_1AF6B);                          /* BNE loc_1AF6B */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    ANDI(0x02);                              /* AND #2 */
    ASL_A();                                 /* ASL */
    ADCI(0x10);                              /* ADC #$10 */
    TAY();                                   /* TAY */
    sub_1B1E2();                             /* JMP sub_1B1E2 */
    RTS();

loc_1AF6B:                                   /* $AF6B */
    CMPI(0x55);                              /* CMP #$55 */
    BNE(loc_1AF79);                          /* BNE loc_1AF79 */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x21);                              /* LDY #$21 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();

loc_1AF79:                                   /* $AF79 */
    LDAI(0x40);                              /* LDA #$40 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x12);                              /* LDY #$12 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();

loc_1AF83:                                   /* $AF83 */
    BEQ(loc_1AF95);                          /* BEQ loc_1AF95 */
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(0x07);                              /* AND #7 */
    goto loc_1AF37;                          /* JMP loc_1AF37 */

loc_1AF95:                                   /* $AF95 */
    LDYI(0x0C);                              /* LDY #$C */
    sub_1B1E8();                             /* JMP sub_1B1E8 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $AEC1  enemy_type_70_71_81_88_89
 *
 *  Original: src/bank6.asm:8447-8575 (bank 6)
 *      JSR sub_1B87F
 *      BCC loc_1AED8
 *  loc_1AEC6:
 *      ...
 *
 *  $AEC6 and $AED8 are also entered from enemy_type_82_weapon_distro (see
 *  above); the tail of this procedure contains the meta-sprite set-up and the
 *  weapon handling by the object type in $88.
 * ------------------------------------------------------------------------ */
void enemy_type_70_71_81_88_89(void)         /* $AEC1 */
{
    sub_1B87F();                             /* JSR sub_1B87F */
    BCC(loc_1AED8);                          /* BCC loc_1AED8 */

    /* loc_1AEC6 ($AEC6) is the fall-through of the BCC above; it is also a
     * second entry point (enemy_type_84_85_86_87 jumps to it from another
     * chunk), so it is documented here instead of being declared as a C label
     * that nothing in this procedure branches to. */
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
    LDAI(0x08);                              /* LDA #8 */
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
    RTS();

loc_1AF28:                                   /* $AF28 */
    CMPI(0x52);                              /* CMP #$52 */
    BNE(loc_1AF49);                          /* BNE loc_1AF49 */
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(0x01);                              /* LDA #1 */
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
    RTS();

loc_1AF49:                                   /* $AF49 */
    CMPI(0x58);                              /* CMP #$58 */
    BCS(loc_1AF83);                          /* BCS loc_1AF83 */
    CMPI(0x54);                              /* CMP #$54 */
    BNE(loc_1AF5B);                          /* BNE loc_1AF5B */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x11);                              /* LDY #$11 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();

loc_1AF5B:                                   /* $AF5B */
    CMPI(0x57);                              /* CMP #$57 */
    BNE(loc_1AF6B);                          /* BNE loc_1AF6B */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    ANDI(0x02);                              /* AND #2 */
    ASL_A();                                 /* ASL */
    ADCI(0x10);                              /* ADC #$10 */
    TAY();                                   /* TAY */
    sub_1B1E2();                             /* JMP sub_1B1E2 */
    RTS();

loc_1AF6B:                                   /* $AF6B */
    CMPI(0x55);                              /* CMP #$55 */
    BNE(loc_1AF79);                          /* BNE loc_1AF79 */
    LDAI(0x00);                              /* LDA #0 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x21);                              /* LDY #$21 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();

loc_1AF79:                                   /* $AF79 */
    LDAI(0x40);                              /* LDA #$40 */
    sub_1B169();                             /* JSR sub_1B169 */
    LDYI(0x12);                              /* LDY #$12 */
    sub_1B09A();                             /* JMP sub_1B09A */
    RTS();

loc_1AF83:                                   /* $AF83 */
    BEQ(loc_1AF95);                          /* BEQ loc_1AF95 */
    sub_C02A();                              /* JSR sub_C02A */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(0x07);                              /* AND #7 */
    goto loc_1AF37;                          /* JMP loc_1AF37 */

loc_1AF95:                                   /* $AF95 */
    LDYI(0x0C);                              /* LDY #$C */
    sub_1B1E8();                             /* JMP sub_1B1E8 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B09A  sub_1B09A
 *
 *  Original: src/bank6.asm:8838-8850 (bank 6)
 *      LDA $174
 *      STA $76
 *      STY $10
 *      TYA
 *      LSR
 *      LSR
 *      LSR
 *      LSR
 *      BNE sub_1B0AA
 *      LDA #$10
 *  (no RTS: execution continues into sub_1B0AA)
 * ------------------------------------------------------------------------ */
void sub_1B09A(void)                         /* $B09A */
{
    LDA(byte_174);                           /* LDA $174 */
    STA(0x0076);                             /* STA $76 */
    STY(byte_10);                            /* STY $10 */
    TYA();                                   /* TYA */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    BNE(loc_1B0AA);                          /* BNE sub_1B0AA (its entry point) */
    LDAI(0x10);                              /* LDA #$10 */

loc_1B0AA:                                   /* $B0AA - entry point of sub_1B0AA */
    sub_1B0AA();                             /* falls through into $B0AA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B0AA  sub_1B0AA - draw a row of meta-sprite cells along a sinusoid
 *
 *  Original: src/bank6.asm:8856-8988 (bank 6)
 * ------------------------------------------------------------------------ */
void sub_1B0AA(void)                         /* $B0AA */
{
    STA(byte_70);                            /* STA $70 */
    LDA(byte_10);                            /* LDA $10 */
    ANDI(0x0F);                              /* AND #$F */
    BNE(loc_1B0B4);                          /* BNE loc_1B0B4 */
    LDAI(0x10);                              /* LDA #$10 */

loc_1B0B4:                                   /* $B0B4 */
    STA(byte_6F);                            /* STA $6F */
    STA(byte_71);                            /* STA $71 */
    LDA(byte_18);                            /* LDA $18 */
    ANDI(0x1F);                              /* AND #$1F */
    LSR_A();                                 /* LSR */
    STA(byte_72);                            /* STA $72 */

loc_1B0BF:                                   /* $B0BF */
    LDA(byte_72);                            /* LDA $72 */
    STA(byte_74);                            /* STA $74 */
    LDA(byte_14);                            /* LDA $14 */
    STA(byte_73);                            /* STA $73 */
    LDA(byte_18);                            /* LDA $18 */
    STA(byte_75);                            /* STA $75 */

loc_1B0CB:                                   /* $B0CB */
    LDA(byte_15);                            /* LDA $15 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    STA(byte_10);                            /* STA $10 */
    LDA(byte_74);                            /* LDA $74 */
    LSR_A();                                 /* LSR */
    CLC();                                   /* CLC */
    ADC(byte_10);                            /* ADC $10 */
    TAY();                                   /* TAY */
    LDA(byte_74);                            /* LDA $74 */
    LSR_A();                                 /* LSR */
    LDA(ABSY(0x04B0));                       /* LDA $4B0,Y */
    BCS(loc_1B0E5);                          /* BCS loc_1B0E5 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */

loc_1B0E5:                                   /* $B0E5 */
    ANDI(0x0F);                              /* AND #$F */
    BEQ(loc_1B10C);                          /* BEQ loc_1B10C */
    CMPI(0x0F);                              /* CMP #$F */
    BNE(loc_1B0F5);                          /* BNE loc_1B0F5 */
    enemy_prng();                            /* JSR enemy_prng */
    ANDI(0x03);                              /* AND #3 */
    CLC();                                   /* CLC */
    ADCI(0x0F);                              /* ADC #$F */

loc_1B0F5:                                   /* $B0F5 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ADCI(0x3E);                              /* ADC #$3E */
    STA(byte_1A);                            /* STA $1A */
    LDAI(0xB0);                              /* LDA #$B0 */
    ADCI(0x00);                              /* ADC #0 */
    STA(byte_1B);                            /* STA $1B */
    LDY(0x0076);                             /* LDY $76 */
    sub_1B20E();                             /* JSR sub_1B20E */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_1B10C);                          /* BEQ loc_1B10C */

loc_1B10C:                                   /* $B10C */
    INC(byte_74);                            /* INC $74 */
    INC(byte_18);                            /* INC $18 */
    INC(byte_18);                            /* INC $18 */
    LDA(byte_18);                            /* LDA $18 */
    ANDI(0x02);                              /* AND #2 */
    BNE(loc_1B11A);                          /* BNE loc_1B11A */
    INC(byte_14);                            /* INC $14 */

loc_1B11A:                                   /* $B11A */
    DEC(byte_71);                            /* DEC $71 */
    BNE(loc_1B0CB);                          /* BNE loc_1B0CB */
    LDA(byte_6F);                            /* LDA $6F */
    STA(byte_71);                            /* STA $71 */
    LDA(byte_73);                            /* LDA $73 */
    STA(byte_14);                            /* STA $14 */
    LDA(byte_15);                            /* LDA $15 */
    INC(byte_15);                            /* INC $15 */
    CMPI(0x0E);                              /* CMP #$E */
    BEQ(loc_1B146);                          /* BEQ loc_1B146 */
    LSR_A();                                 /* LSR */
    BCC(loc_1B138);                          /* BCC loc_1B138 */
    LDA(byte_14);                            /* LDA $14 */
    CLC();                                   /* CLC */
    ADCI(0x08);                              /* ADC #8 */
    STA(byte_14);                            /* STA $14 */

loc_1B138:                                   /* $B138 */
    LDA(byte_75);                            /* LDA $75 */
    CLC();                                   /* CLC */
    ADCI(0x40);                              /* ADC #$40 */
    STA(byte_18);                            /* STA $18 */
    BCC(loc_1B143);                          /* BCC loc_1B143 */
    INC(byte_19);                            /* INC $19 */

loc_1B143:                                   /* $B143 */
    goto loc_1B161;                          /* JMP loc_1B161 */

loc_1B146:                                   /* $B146 */
    LDA(byte_14);                            /* LDA $14 */
    CLC();                                   /* CLC */
    ADCI(0x08);                              /* ADC #8 */
    ANDI(0x3F);                              /* AND #$3F */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_15);                            /* STA $15 */
    LDA(byte_75);                            /* LDA $75 */
    ANDI(0x3F);                              /* AND #$3F */
    STA(byte_18);                            /* STA $18 */
    LDA(byte_19);                            /* LDA $19 */
    ANDI(0x28);                              /* AND #$28 */
    EORI(0x08);                              /* EOR #8 */
    STA(byte_19);                            /* STA $19 */

loc_1B161:                                   /* $B161 */
    DEC(byte_70);                            /* DEC $70 */
    BEQ(locret_1B168);                       /* BEQ locret_1B168 */
    goto loc_1B0BF;                          /* JMP loc_1B0BF */

locret_1B168:                                /* $B168 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B169  sub_1B169 - convert an object's position back to polar-ish fields
 *
 *  Original: src/bank6.asm:8994-9058 (bank 6)
 * ------------------------------------------------------------------------ */
void sub_1B169(void)                         /* $B169 */
{
    STA(byte_10);                            /* STA $10 */
    LDA(ABSX(byte_5F8));                     /* LDA $5F8,X */
    STA(byte_18);                            /* STA $18 */
    LDA(ABSX(byte_612));                     /* LDA $612,X */
    STA(byte_19);                            /* STA $19 */
    LDA(ABSX(byte_62C));                     /* LDA $62C,X */
    STA(byte_15);                            /* STA $15 */
    LDA(ABSX(byte_646));                     /* LDA $646,X */
    STA(byte_14);                            /* STA $14 */
    BIT(byte_10);                            /* BIT $10 */
    BVC(loc_1B18F);                          /* BVC loc_1B18F */
    DEC(byte_18);                            /* DEC $18 */
    DEC(byte_18);                            /* DEC $18 */
    LDA(byte_18);                            /* LDA $18 */
    ANDI(0x02);                              /* AND #2 */
    BEQ(loc_1B18F);                          /* BEQ loc_1B18F */
    DEC(byte_14);                            /* DEC $14 */

loc_1B18F:                                   /* $B18F */
    BIT(byte_10);                            /* BIT $10 */
    BPL(locret_1B1AE);                       /* BPL locret_1B1AE */
    LDA(byte_15);                            /* LDA $15 */
    BEQ(loc_1B1AF);                          /* BEQ loc_1B1AF */
    DEC(byte_15);                            /* DEC $15 */
    LSR_A();                                 /* LSR */
    BCS(loc_1B1A3);                          /* BCS loc_1B1A3 */
    LDA(byte_14);                            /* LDA $14 */
    SEC();                                   /* SEC */
    SBCI(0x08);                              /* SBC #8 */
    STA(byte_14);                            /* STA $14 */

loc_1B1A3:                                   /* $B1A3 */
    LDA(byte_18);                            /* LDA $18 */
    SEC();                                   /* SEC */
    SBCI(0x40);                              /* SBC #$40 */
    STA(byte_18);                            /* STA $18 */
    BCS(locret_1B1AE);                       /* BCS locret_1B1AE */
    DEC(byte_19);                            /* DEC $19 */

locret_1B1AE:                                /* $B1AE */
    RTS();                                   /* RTS */

loc_1B1AF:                                   /* $B1AF */
    LDAI(0x0E);                              /* LDA #$E */
    STA(byte_15);                            /* STA $15 */
    LDA(byte_18);                            /* LDA $18 */
    ANDI(0x3F);                              /* AND #$3F */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_18);                            /* STA $18 */
    LDA(byte_19);                            /* LDA $19 */
    EORI(0x08);                              /* EOR #8 */
    ORAI(0x03);                              /* ORA #3 */
    STA(byte_19);                            /* STA $19 */
    LDA(byte_14);                            /* LDA $14 */
    SEC();                                   /* SEC */
    SBCI(0x08);                              /* SBC #8 */
    ANDI(0x3F);                              /* AND #$3F */
    STA(byte_14);                            /* STA $14 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B1CD  sub_1B1CD
 *
 *  Original: src/bank6.asm:9064-9067 (bank 6)
 *      LDA $16F
 *  (no RTS: execution continues into sub_1B1D0)
 * ------------------------------------------------------------------------ */
void sub_1B1CD(void)                         /* $B1CD */
{
    LDA(0x016F);                             /* LDA $16F */
    sub_1B1D0();                             /* falls through into $B1D0 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B1D0  sub_1B1D0 - point $1A/$1B at $AFC2 + Y
 *
 *  Original: src/bank6.asm:9073-9085 (bank 6)
 *      PHA
 *      TYA
 *      CLC
 *      ADC #$C2
 *      STA byte_1A
 *      LDA #$AF
 *      ADC #0
 *      STA byte_1B
 *      PLA
 *      TAY
 *      JMP sub_1B1FA
 * ------------------------------------------------------------------------ */
void sub_1B1D0(void)                         /* $B1D0 */
{
    PHA();                                   /* PHA */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADCI(0xC2);                              /* ADC #$C2 */
    STA(byte_1A);                            /* STA byte_1A */
    LDAI(0xAF);                              /* LDA #$AF */
    ADCI(0x00);                              /* ADC #0 */
    STA(byte_1B);                            /* STA byte_1B */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    sub_1B1FA();                             /* JMP sub_1B1FA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B1E2  sub_1B1E2
 *
 *  Original: src/bank6.asm:9091-9094 (bank 6)
 *      LDA byte_171
 *      JMP sub_1B1EB
 * ------------------------------------------------------------------------ */
void sub_1B1E2(void)                         /* $B1E2 */
{
    LDA(byte_171);                           /* LDA byte_171 */
    sub_1B1EB();                             /* JMP sub_1B1EB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B1E8  sub_1B1E8
 *
 *  Original: src/bank6.asm:9100-9103 (bank 6)
 *      LDA byte_174
 *  (no RTS: execution continues into sub_1B1EB)
 * ------------------------------------------------------------------------ */
void sub_1B1E8(void)                         /* $B1E8 */
{
    LDA(byte_174);                           /* LDA byte_174 */
    sub_1B1EB();                             /* falls through into $B1EB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B1EB  sub_1B1EB - point $1A/$1B at $B03E + Y
 *
 *  Original: src/bank6.asm:9109-9121 (bank 6)
 *      PHA
 *      TYA
 *      CLC
 *      ADC #$3E
 *      STA byte_1A
 *      LDA #$B0
 *      ADC #0
 *      STA byte_1B
 *      PLA
 *      TAY
 *  (no RTS: execution continues into sub_1B1FA)
 * ------------------------------------------------------------------------ */
void sub_1B1EB(void)                         /* $B1EB */
{
    PHA();                                   /* PHA */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADCI(0x3E);                              /* ADC #$3E */
    STA(byte_1A);                            /* STA byte_1A */
    LDAI(0xB0);                              /* LDA #$B0 */
    ADCI(0x00);                              /* ADC #0 */
    STA(byte_1B);                            /* STA byte_1B */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    sub_1B1FA();                             /* falls through into $B1FA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B1FA  sub_1B1FA - copy the object's position into $18/$19/$15/$14
 *
 *  Original: src/bank6.asm:9127-9136 (bank 6)
 *      LDA $5F8,X
 *      STA byte_18
 *      LDA $612,X
 *      STA byte_19
 *      LDA $62C,X
 *      STA byte_15
 *      LDA $646,X
 *      STA byte_14
 *  (no RTS: execution continues into sub_1B20E)
 * ------------------------------------------------------------------------ */
void sub_1B1FA(void)                         /* $B1FA */
{
    LDA(ABSX(byte_5F8));                     /* LDA $5F8,X */
    STA(byte_18);                            /* STA byte_18 */
    LDA(ABSX(byte_612));                     /* LDA $612,X */
    STA(byte_19);                            /* STA byte_19 */
    LDA(ABSX(byte_62C));                     /* LDA $62C,X */
    STA(byte_15);                            /* STA byte_15 */
    LDA(ABSX(byte_646));                     /* LDA $646,X */
    STA(byte_14);                            /* STA byte_14 */
    sub_1B20E();                             /* falls through into $B20E */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B20E  sub_1B20E - put the 4-cell meta-sprite addressed by $1A/$1B on the
 *                     sprite page, at the index returned by sub_C04B/sub_C03C
 *
 *  Original: src/bank6.asm:9142-9188 (bank 6)
 * ------------------------------------------------------------------------ */
void sub_1B20E(void)                         /* $B20E */
{
    STX(byte_65);                            /* STX $65 */
    STY(byte_17);                            /* STY $17 */
    LDAI(0x09);                              /* LDA #9 */
    sub_C04B();                              /* JSR sub_C04B */
    sub_C03C();                              /* JSR sub_C03C */
    TAX();                                   /* TAX */
    LDA(byte_19);                            /* LDA $19 */
    ANDI(0x0F);                              /* AND #$F */
    ORAI(0xE0);                              /* ORA #$E0 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(byte_18);                            /* LDA $18 */

    /* loc_1B226 ($B226) is only the fall-through of the LDA above; nothing
     * branches to it inside this procedure. */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(byte_14);                            /* LDA $14 */
    ORAI(0xC0);                              /* ORA #$C0 */
    STA(ABSX(0x0466));                       /* STA $466,X */
    LDA(byte_15);                            /* LDA $15 */
    ANDI(0x01);                              /* AND #1 */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(byte_18);                            /* LDA $18 */
    ANDI(0x02);                              /* AND #2 */
    BEQ(loc_1B23D);                          /* BEQ loc_1B23D */
    INY();                                   /* INY */

loc_1B23D:                                   /* $B23D */
    LDA(ABSY(unk_1B260));                    /* LDA unk_1B260,Y */
    STA(ABSX(0x0468));                       /* STA $468,X */
    LDA(byte_17);                            /* LDA $17 */
    AND(ABSY(unk_1B264));                    /* AND unk_1B264,Y */
    STA(ABSX(0x0467));                       /* STA $467,X */
    LDYI(0x00);                              /* LDY #0 */

loc_1B24D:                                   /* $B24D */
    LDA(INDY(byte_1A));                      /* LDA ($1A),Y */
    STA(ABSX(0x0462));                       /* STA $462,X */
    INX();                                   /* INX */
    INY();                                   /* INY */
    CPYI(0x04);                              /* CPY #4 */
    BNE(loc_1B24D);                          /* BNE loc_1B24D */
    sub_C03F();                              /* JSR sub_C03F */
    LDX(byte_65);                            /* LDX $65 */
    LDY(byte_17);                            /* LDY $17 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B268  enemy_type_72
 *
 *  Original: src/bank6.asm:9203-9272 (bank 6)
 * ------------------------------------------------------------------------ */
void enemy_type_72(void)                     /* $B268 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1B289);                          /* BMI loc_1B289 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0xF8);                              /* LDA #$F8 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */

loc_1B289:                                   /* $B289 */
    DEC(ABSX(byte_6E2));                     /* DEC $6E2,X */
    BNE(loc_1B2A2);                          /* BNE loc_1B2A2 */
    LDA(ABSX(byte_6FC));                     /* LDA $6FC,X */
    BEQ(loc_1B2A2);                          /* BEQ loc_1B2A2 */
    DEC(ABSX(byte_6FC));                     /* DEC $6FC,X */
    BNE(loc_1B2A2);                          /* BNE loc_1B2A2 */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    CMPI(0x46);                              /* CMP #$46 */
    BNE(loc_1B2A2);                          /* BNE loc_1B2A2 */
    DEC(ABSX(byte_6FC));                     /* DEC $6FC,X */

loc_1B2A2:                                   /* $B2A2 */
    LDA(ABSX(byte_6E2));                     /* LDA $6E2,X */
    ANDI(0x03);                              /* AND #3 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    CLC();                                   /* CLC */
    ADCI(0x3C);                              /* ADC #$3C */
    STA(ABSX(byte_576));                     /* STA $576,X */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    LDA(ABSX(byte_590));                     /* LDA $590,X */
    ANDI(0x01);                              /* AND #1 */
    CLC();                                   /* CLC */
    ADCI(0x36);                              /* ADC #$36 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSX(byte_6FC));                     /* LDA $6FC,X */
    BEQ(loc_1B2C6);                          /* BEQ loc_1B2C6 */
    add_enemy_meta_sprite();                 /* JSR add_enemy_meta_sprite */

loc_1B2C6:                                   /* $B2C6 */
    sub_181ED();                             /* JSR sub_181ED */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BPL(loc_1B2CF);                          /* BPL loc_1B2CF */
    RTS();                                   /* RTS */

loc_1B2CF:                                   /* $B2CF */
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA $528 */
    LDA(ABSX(byte_6FC));                     /* LDA $6FC,X */
    BEQ(loc_1B2E1);                          /* BEQ loc_1B2E1 */
    sub_1B2ED();                             /* JSR sub_1B2ED */
    LDAI(0x13);                              /* LDA #$13 */
    j_apu_play();                            /* JMP j_apu_play */
    RTS();

loc_1B2E1:                                   /* $B2E1 */
    LDA(ABSX(byte_716));                     /* LDA $716,X */
    STA(byte_92);                            /* STA $92 */
    LDA(game_mode);                          /* LDA $30 */
    ORAI(0x20);                              /* ORA #$20 */
    STA(game_mode);                          /* STA $30 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B2ED  sub_1B2ED - flash the palette and retype the whole enemy list
 *
 *  Original: src/bank6.asm:9278-9322 (bank 6)
 *
 *  $B2EF is a second entry point: sub_1946F:loc_19480 (another chunk) jumps
 *  there, skipping the LDA #$23.  It is not a local goto target, so it is kept
 *  as a comment rather than an unused C label.
 * ------------------------------------------------------------------------ */
void sub_1B2ED(void)                         /* $B2ED */
{
    LDAI(0x23);                              /* LDA #$23 */

    /* loc_1B2EF: ($B2EF) second entry point, entered with A already set */
    PHA();                                   /* PHA */
    LDA(cram_new_palette);                   /* LDA $145 */
    PHA();                                   /* PHA */
    LDAI(0x30);                              /* LDA #$30 */
    sub_1B32E();                             /* JSR sub_1B32E */
    LDYI(0x05);                              /* LDY #5 */
    j_all_enemies_destroyed();               /* JSR j_all_enemies_destroyed */
    PLA();                                   /* PLA */
    STA(byte_10);                            /* STA $10 */
    LDYI(0x05);                              /* LDY #5 */

loc_1B303:                                   /* $B303 */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    BEQ(loc_1B322);                          /* BEQ loc_1B322 */
    CMPI(0x46);                              /* CMP #$46 */
    BCS(loc_1B322);                          /* BCS loc_1B322 */
    CMPI(0x28);                              /* CMP #$28 */
    BEQ(loc_1B322);                          /* BEQ loc_1B322 */
    CMPI(0x30);                              /* CMP #$30 */
    BCC(loc_1B31A);                          /* BCC loc_1B31A */
    CMPI(0x38);                              /* CMP #$38 */
    BCC(loc_1B322);                          /* BCC loc_1B322 */

loc_1B31A:                                   /* $B31A */
    STA(ABSY(0x07E6));                       /* STA $7E6,Y */
    PLA();                                   /* PLA */
    PHA();                                   /* PHA */
    STA(ABSY(byte_528));                     /* STA $528,Y */

loc_1B322:                                   /* $B322 */
    INY();                                   /* INY */
    CPYI(0x1A);                              /* CPY #$1A */
    BNE(loc_1B303);                          /* BNE loc_1B303 */
    LDA(byte_10);                            /* LDA $10 */
    sub_1B32E();                             /* JSR sub_1B32E */
    PLA();                                   /* PLA */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B32E  sub_1B32E - set the CRAM update mode and the new palette byte
 *
 *  Original: src/bank6.asm:9328-9334 (bank 6)
 *      STA cram_new_palette
 *      LDA #3
 *      STA cram_update_mode
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1B32E(void)                         /* $B32E */
{
    STA(cram_new_palette);                   /* STA cram_new_palette */
    LDAI(0x03);                              /* LDA #3 */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $B337  enemy_type_79
 *
 *  Original: src/bank6.asm:9340-9345 (bank 6)
 *      LDA $528,X
 *      BPL fortress_core_appears
 *      JSR add_enemy_meta_sprite
 *      JMP loc_1B3A5
 *
 *  loc_1B3A5 ($B3A5) is an inner label of sub_1B39E; the tail of that
 *  procedure is reproduced below under local labels of the same name.
 * ------------------------------------------------------------------------ */
void enemy_type_79(void)                     /* $B337 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BPL(loc_1B355);                          /* BPL fortress_core_appears ($B355) */
    add_enemy_meta_sprite();                 /* JSR add_enemy_meta_sprite */
    goto loc_1B3A5;                          /* JMP loc_1B3A5 ($B3A5) */

    /* -------------------------------------------------------------------- *
     *  Shared tail of sub_1B39E ($B3A5..$B44A), reproduced here because the
     *  original jumps into the middle of that procedure (PORTING.md section 5,
     *  "shared tail").
     * -------------------------------------------------------------------- */

loc_1B3A5:                                   /* $B3A5 */
    LDA(byte_C0);                            /* LDA $C0 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_C0);                            /* STA $C0 */
    ANDI(0x06);                              /* AND #6 */
    BNE(loc_1B3B0);                          /* BNE loc_1B3B0 */
    RTS();                                   /* RTS */

loc_1B3B0:                                   /* $B3B0 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    ANDI(0x02);                              /* AND #2 */
    BEQ(loc_1B3BA);                          /* BEQ loc_1B3BA */
    sub_1B482();                             /* JMP sub_1B482 */
    RTS();

loc_1B3BA:                                   /* $B3BA */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xCF);                              /* CMP #$CF */
    BNE(loc_1B3C4);                          /* BNE loc_1B3C4 */
    goto loc_1B41B;                          /* JMP loc_1B41B */

loc_1B3C4:                                   /* $B3C4 */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x08);                              /* AND #8 */
    BEQ(loc_1B3E3);                          /* BEQ loc_1B3E3 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    ANDI(0xFE);                              /* AND #$FE */
    CMP(ABSX(byte_5AA));                     /* CMP $5AA,X */
    BNE(loc_1B3DC);                          /* BNE loc_1B3DC */
    ORAI(0x01);                              /* ORA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    ASL(ABSX(byte_6E2));                     /* ASL $6E2,X */

loc_1B3DC:                                   /* $B3DC */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    CMPI(0x03);                              /* CMP #3 */
    BEQ(loc_1B41B);                          /* BEQ loc_1B41B */

loc_1B3E3:                                   /* $B3E3 */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0694));                       /* ADC $694,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    BCC(loc_1B411);                          /* BCC loc_1B411 */
    INC(ABSX(0x06AE));                       /* INC $6AE,X */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    CMPI(0x06);                              /* CMP #6 */
    BNE(loc_1B3FE);                          /* BNE loc_1B3FE */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */

loc_1B3FE:                                   /* $B3FE */
    TAY();                                   /* TAY */
    LDA(ABSY(0xB65B));                       /* LDA $B65B,Y */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDA(ABSY(0xB661));                       /* LDA $B661,Y */
    STA(ABSX(0x0730));                       /* STA $730,X */
    sub_1B667();                             /* JSR sub_1B667 */
    sub_1B4D5();                             /* JSR sub_1B4D5 */

loc_1B411:                                   /* $B411 */
    LDA(ABSX(0x0730));                       /* LDA $730,X */
    BNE(loc_1B417);                          /* BNE loc_1B417 */
    RTS();                                   /* RTS */

loc_1B417:                                   /* $B417 */
    CMPI(0x03);                              /* CMP #3 */
    BNE(loc_1B42A);                          /* BNE loc_1B42A */

loc_1B41B:                                   /* $B41B */
    LDA(ABSX(byte_6E2));                     /* LDA $6E2,X */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_6FC));                     /* ADC $6FC,X */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    BCC(loc_1B42A);                          /* BCC loc_1B42A */
    sub_1B533();                             /* JSR sub_1B533 */

loc_1B42A:                                   /* $B42A */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xCF);                              /* CMP #$CF */
    BNE(loc_1B434);                          /* BNE loc_1B434 */
    sub_1B44B();                             /* JSR sub_1B44B */

loc_1B434:                                   /* $B434 */
    sub_18204();                             /* JSR sub_18204 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    BEQ(loc_1B467);                          /* BEQ sub_1B467 (its entry point) */
    CMPI(0xCF);                              /* CMP #$CF */
    BEQ(loc_1B441);                          /* BEQ loc_1B441 */
    RTS();                                   /* RTS */

loc_1B441:                                   /* $B441 */
    LDAI(0x11);                              /* LDA #$11 */
    j_apu_play();                            /* JSR j_apu_play */
    LDAI(0x0F);                              /* LDA #$F */
    sub_1B491();                             /* JSR sub_1B491 */
    sub_1B44B();                             /* falls through into $B44B */
    RTS();

loc_1B467:                                   /* $B467 - entry point of sub_1B467 */
    sub_1B467();                             /* BEQ sub_1B467 */
    RTS();

loc_1B355:                                   /* $B355 - entry point of fortress_core_appears */
    fortress_core_appears();                 /* BPL fortress_core_appears */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B342  enemy_type_73_74_75_76_77_78
 *
 *  Original: src/bank6.asm:9351-9363 (bank 6)
 *      JSR sub_1B8A3
 *      LDA $528,X
 *      BPL loc_1B352
 *      LDA $542,X
 *      CMP #$F0
 *      JMP sub_1B39E
 *  loc_1B352:
 *      BCS fortress_core_appears
 *      RTS
 * ------------------------------------------------------------------------ */
void enemy_type_73_74_75_76_77_78(void)      /* $B342 */
{
    sub_1B8A3();                             /* JSR sub_1B8A3 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BPL(loc_1B352);                          /* BPL loc_1B352 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMPI(0xF0);                              /* CMP #$F0 */
    sub_1B39E();                             /* JMP sub_1B39E */
    RTS();

loc_1B352:                                   /* $B352 */
    BCS(loc_1B355);                          /* BCS fortress_core_appears ($B355) */
    RTS();                                   /* RTS */

loc_1B355:                                   /* $B355 - entry point of fortress_core_appears */
    fortress_core_appears();                 /* BCS fortress_core_appears */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B355  fortress_core_appears
 *
 *  Original: src/bank6.asm:9370-9402 (bank 6); no RTS, execution continues
 *  into sub_1B39E ($B39E) with the carry cleared.
 * ------------------------------------------------------------------------ */
void fortress_core_appears(void)             /* $B355 */
{
    LDAI(0x03);                              /* LDA #3 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    STA(ABSX(byte_5C4));                     /* STA $5C4,X */
    STA(ABSX(byte_716));                     /* STA $716,X */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    SEC();                                   /* SEC */
    SBCI(0xC9);                              /* SBC #$C9 */
    STA(byte_10);                            /* STA $10 */
    ASL_A();                                 /* ASL */
    ADC(byte_10);                            /* ADC $10 */
    TAY();                                   /* TAY */
    LDA(ABSY(byte_1B646));                   /* LDA byte_1B646,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSY(byte_1B646 + 1));               /* LDA byte_1B646+1,Y */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDA(ABSY(byte_1B646 + 2));               /* LDA byte_1B646+2,Y */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    INC(byte_C1);                            /* INC $C1 */
    INC(byte_C2);                            /* INC $C2 */
    LDA(byte_C1);                            /* LDA $C1 */
    STA(ABSX(0x06C8));                       /* STA $6C8,X */
    sub_1B667();                             /* JSR sub_1B667 */
    CLC();                                   /* CLC */
    sub_1B39E();                             /* falls through into $B39E */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $B39E  sub_1B39E - the fortress-core state machine
 *
 *  Original: src/bank6.asm:9408-9517 (bank 6)
 *
 *  $B3A5 is also entered from enemy_type_79 (see above), and the procedure
 *  ends by falling through into sub_1B44B ($B44B).
 * ------------------------------------------------------------------------ */
void sub_1B39E(void)                         /* $B39E */
{
    BCC(loc_1B3A5);                          /* BCC loc_1B3A5 */
    DEC(byte_C2);                            /* DEC $C2 */
    enemy_erase();                           /* JMP enemy_erase */
    RTS();

loc_1B3A5:                                   /* $B3A5 */
    LDA(byte_C0);                            /* LDA $C0 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_C0);                            /* STA $C0 */
    ANDI(0x06);                              /* AND #6 */
    BNE(loc_1B3B0);                          /* BNE loc_1B3B0 */
    RTS();                                   /* RTS */

loc_1B3B0:                                   /* $B3B0 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    ANDI(0x02);                              /* AND #2 */
    BEQ(loc_1B3BA);                          /* BEQ loc_1B3BA */
    sub_1B482();                             /* JMP sub_1B482 */
    RTS();

loc_1B3BA:                                   /* $B3BA */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xCF);                              /* CMP #$CF */
    BNE(loc_1B3C4);                          /* BNE loc_1B3C4 */
    goto loc_1B41B;                          /* JMP loc_1B41B */

loc_1B3C4:                                   /* $B3C4 */
    LDA(byte_C0);                            /* LDA $C0 */
    ANDI(0x08);                              /* AND #8 */
    BEQ(loc_1B3E3);                          /* BEQ loc_1B3E3 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    ANDI(0xFE);                              /* AND #$FE */
    CMP(ABSX(byte_5AA));                     /* CMP $5AA,X */
    BNE(loc_1B3DC);                          /* BNE loc_1B3DC */
    ORAI(0x01);                              /* ORA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    ASL(ABSX(byte_6E2));                     /* ASL $6E2,X */

loc_1B3DC:                                   /* $B3DC */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    CMPI(0x03);                              /* CMP #3 */
    BEQ(loc_1B41B);                          /* BEQ loc_1B41B */

loc_1B3E3:                                   /* $B3E3 */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0694));                       /* ADC $694,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    BCC(loc_1B411);                          /* BCC loc_1B411 */
    INC(ABSX(0x06AE));                       /* INC $6AE,X */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    CMPI(0x06);                              /* CMP #6 */
    BNE(loc_1B3FE);                          /* BNE loc_1B3FE */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */

loc_1B3FE:                                   /* $B3FE */
    TAY();                                   /* TAY */
    LDA(ABSY(0xB65B));                       /* LDA $B65B,Y */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDA(ABSY(0xB661));                       /* LDA $B661,Y */
    STA(ABSX(0x0730));                       /* STA $730,X */
    sub_1B667();                             /* JSR sub_1B667 */
    sub_1B4D5();                             /* JSR sub_1B4D5 */

loc_1B411:                                   /* $B411 */
    LDA(ABSX(0x0730));                       /* LDA $730,X */
    BNE(loc_1B417);                          /* BNE loc_1B417 */
    RTS();                                   /* RTS */

loc_1B417:                                   /* $B417 */
    CMPI(0x03);                              /* CMP #3 */
    BNE(loc_1B42A);                          /* BNE loc_1B42A */

loc_1B41B:                                   /* $B41B */
    LDA(ABSX(byte_6E2));                     /* LDA $6E2,X */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_6FC));                     /* ADC $6FC,X */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    BCC(loc_1B42A);                          /* BCC loc_1B42A */
    sub_1B533();                             /* JSR sub_1B533 */

loc_1B42A:                                   /* $B42A */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xCF);                              /* CMP #$CF */
    BNE(loc_1B434);                          /* BNE loc_1B434 */
    sub_1B44B();                             /* JSR sub_1B44B */

loc_1B434:                                   /* $B434 */
    sub_18204();                             /* JSR sub_18204 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    BEQ(loc_1B467);                          /* BEQ sub_1B467 (its entry point) */
    CMPI(0xCF);                              /* CMP #$CF */
    BEQ(loc_1B441);                          /* BEQ loc_1B441 */
    RTS();                                   /* RTS */

loc_1B441:                                   /* $B441 */
    LDAI(0x11);                              /* LDA #$11 */
    j_apu_play();                            /* JSR j_apu_play */
    LDAI(0x0F);                              /* LDA #$F */
    sub_1B491();                             /* JSR sub_1B491 */
    sub_1B44B();                             /* falls through into $B44B */
    RTS();

loc_1B467:                                   /* $B467 - entry point of sub_1B467 */
    sub_1B467();                             /* BEQ sub_1B467 */
    RTS();
}
