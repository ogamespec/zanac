/* bank6/9E2A.c - bank 6, $9E2A-$A509: the enemy handlers of types 16-34 and
 * 46/47 with the helpers they share.
 *
 * Ported from src/bank6.asm lines 5824-7038 (addresses from Build/bank6.lst).
 *
 * Notes for the reviewer - the constructs of this chunk that need a convention:
 *
 *  - Shared code blocks.  Several handlers branch or jump into the middle of
 *    another handler.  C has no cross-function goto and inner labels may not
 *    become functions, so every such block is reproduced at the branch source
 *    with a comment naming its owner (PORTING.md section 5):
 *      loc_19F05 ($9F05)  owned by sub_19EB4,   entered by enemy_type_16/17
 *      loc_19F94 ($9F94)  owned by enemy_type_18, entered by sub_19EB4
 *      loc_19FF9 ($9FF9)  owned by enemy_type_22, entered by enemy_type_23
 *      loc_1A01B ($A01B)  owned by enemy_type_22, entered by enemy_type_23
 *      loc_1A16E ($A16E)  owned by enemy_type_27, entered by enemy_type_28
 *      loc_1A3C5 ($A3C5)  owned by enemy_type_30_32, entered by
 *                         enemy_type_31_33
 *
 *  - Branches to another procedure's entry ($A0BE and $A16E: `BMI sub_1A110`).
 *    They become a local label of the same name that calls the procedure, so
 *    the state (A/X/Y/flags) and the return flow match the original.
 *
 *  - decrease_enemy_health's PLA/PLA + RTS "double return" ($A2B6-$A2B8): the
 *    pops discard this routine's own JSR return address so that the RTS
 *    resumes the enemy slot loop at $8807 and abandons the calling handler
 *    (bank6_report.md section 3.3).  A C call keeps that address on the CPU
 *    stack, so - exactly like the return-address PLAs of print_string
 *    (PORTING.md section 4) - the pops are modelled, not executed.  See the
 *    function for the details.
 *
 *  - Chopped procedures (no RTS/JMP at the end) end with a call plus RTS():
 *    enemy_type_16 -> sub_19EB4, enemy_type_26 -> sub_1A0F9,
 *    sub_1A0F9 -> sub_1A110, enemy_type_30_32 -> enemy_type_31_33.
 *    A trailing `JMP <procedure>` is a call plus RTS() as well.
 *
 * There are no inline parameter blocks (print_string family) and no
 * switch_case tables in this chunk.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $9E2A  sub_19E2A - force the object's flags bit 7 when its chase counter
 *                     is odd
 *
 *  Original: src/bank6.asm:5824-5835 (bank 6)
 *      LDA $6FC,X
 *      AND #1
 *      BEQ locret_19E37
 *      LDA $14
 *      ORA #$80
 *      STA $14
 *  locret_19E37:
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_19E2A(void)                         /* $9E2A */
{
    LDA(ABSX(byte_6FC));                     /* LDA $6FC,X */
    ANDI(0x01);                              /* AND #1 */
    BEQ(locret_19E37);                       /* BEQ locret_19E37 */
    LDA(byte_14);                            /* LDA $14 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_14);                            /* STA $14 */

locret_19E37:                                /* $9E37 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9E38  enemy_type_25_carla - "T-Cell (Carla)"
 *
 *  Original: src/bank6.asm:5842-5892 (bank 6)
 *      LDA $528,X / BMI loc_19E64 / ORA #$80 / STA $528,X /
 *      JSR spawn_top_random_x_position / LDA #1 / STA $590,X / STA $6E2,X /
 *      STA $74A,X / LDA #$4C / STA $576,X / LDA #3 / STA $660,X /
 *      LDA #$14 / STA $716,X / LDA #$FF / STA $6FC,X
 *  loc_19E64:
 *      JSR update_chase_counters / JSR process_enemy_flags_sprites /
 *      INC $730,X / LDA $730,X / AND #$10 / BEQ loc_19E76 / LDA #1
 *  loc_19E76:
 *      CLC / ADC #$4C / STA $576,X / JSR sub_181F6 / LDA $528,X /
 *      BMI locret_19E98 / LDY $8A / BNE loc_19E8E / LDA #$81 / STA $528 /
 *      RTS
 *  loc_19E8E:
 *      LDA #$28 / STA $528,Y / LDA #$99 / STA $528,X
 *  locret_19E98:
 *      RTS
 * ------------------------------------------------------------------------ */
void enemy_type_25_carla(void)               /* $9E38 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19E64);                          /* BMI loc_19E64 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDAI(0x4C);                              /* LDA #$4C */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(0x03);                              /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x14);                              /* LDA #$14 */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */

loc_19E64:                                   /* $9E64 */
    update_chase_counters();                 /* JSR update_chase_counters */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    INC(ABSX(0x0730));                       /* INC $730,X */
    LDA(ABSX(0x0730));                       /* LDA $730,X */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_19E76);                          /* BEQ loc_19E76 */
    LDAI(0x01);                              /* LDA #1 */

loc_19E76:                                   /* $9E76 */
    CLC();                                   /* CLC */
    ADCI(0x4C);                              /* ADC #$4C */
    STA(ABSX(byte_576));                     /* STA $576,X */
    sub_181F6();                             /* JSR sub_181F6 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(locret_19E98);                       /* BMI locret_19E98 */
    LDY(0x008A);                             /* LDY $8A */
    BNE(loc_19E8E);                          /* BNE loc_19E8E */
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA $528 */
    RTS();                                   /* RTS */

loc_19E8E:                                   /* $9E8E */
    LDAI(0x28);                              /* LDA #$28 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDAI(0x99);                              /* LDA #$99 */
    STA(ABSX(byte_528));                     /* STA $528,X */

locret_19E98:                                /* $9E98 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $9E99  enemy_type_16 - set up the object and continue in sub_19EB4
 *
 *  Original: src/bank6.asm:5898-5910 (bank 6)
 *      LDA $528,X / BMI loc_19F05 / LDA #1 / STA $660,X / LDA #$80 /
 *      STA $5F8,X / LDA #1 / STA $612,X / LDA #$C0 / STA $67A,X / LDY #0
 *  (no RTS: execution continues into sub_19EB4)
 *
 *  loc_19F05 lives inside sub_19EB4; the BMI at $9E9C enters it, so the block
 *  is reproduced here (see the file header and PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void enemy_type_16(void)                     /* $9E99 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19F05);                          /* BMI loc_19F05 */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0xC0);                              /* LDA #$C0 */
    STA(ABSX(unk_67A));                      /* STA $67A,X */
    LDYI(0);                                 /* LDY #0 */
    sub_19EB4();                             /* falls through into $9EB4 */
    RTS();

loc_19F05:                                   /* $9F05 - shared, owned by sub_19EB4 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CLC();                                   /* CLC */
    ADCI(0x18);                              /* ADC #$18 */
    AND(ABSX(unk_67A));                      /* AND $67A,X */
    SEC();                                   /* SEC */
    SBCI(0x18);                              /* SBC #$18 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    BEQ(loc_19F41);                          /* BEQ loc_19F41 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CLC();                                   /* CLC */
    ADCI(0x10);                              /* ADC #$10 */
    AND(ABSX(unk_67A));                      /* AND $67A,X */
    SEC();                                   /* SEC */
    SBCI(0x10);                              /* SBC #$10 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    BEQ(loc_19F2A);                          /* BEQ loc_19F2A */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_19F2A:                                   /* $9F2A */
    LDAI(0x16);                              /* LDA #$16 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19F3E);                          /* BCS loc_19F3E */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x26);                              /* LDA #$26 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19F3E:                                   /* $9F3E */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_19F41:                                   /* $9F41 */
    LDAI(0x15);                              /* LDA #$15 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $9EB4  sub_19EB4 - set up the object from the sub-type tables and run the
 *                     common handler tail
 *
 *  Original: src/bank6.asm:5916-5998 (bank 6)
 *      STY $14 / JSR enemy_prng / AND #1 / BEQ loc_19EBF / INC $14
 *  loc_19EBF:
 *      LDA #$15 / STA $576,X / LDY $14 / LDA unk_19FBC,Y / STA $55C,X /
 *      LDA $9FBE,Y / STA $590,X / LDA $528,X / ORA #$80 / STA $528,X /
 *      CMP #$92 / BEQ loc_19EF0 / LSR / LDA unk_19FC0,Y / STA $764,X /
 *      BCC loc_19F05 / LDA unk_19FBC,Y / STA $6FC,X / JMP loc_19F05
 *  loc_19EF0:
 *      LDA #3 / STA $646,X / LDA unk_19FC0,Y / STA $6FC,X / BEQ loc_19F02 /
 *      LDA #$FD / STA $646,X
 *  loc_19F02:
 *      JMP loc_19F94
 *  loc_19F05:
 *      LDA $542,X / CLC / ADC #$18 / AND $67A,X / SEC / SBC #$18 /
 *      CMP $542,X / BEQ loc_19F41 / LDA $542,X / CLC / ADC #$10 / AND $67A,X /
 *      SEC / SBC #$10 / CMP $542,X / BEQ loc_19F2A / JMP sub_197B4
 *  loc_19F2A:
 *      LDA #$16 / STA $576,X / JSR check_enemy_overflow / BCS loc_19F3E /
 *      LDA $764,X / STA $14 / LDA #$26 / JSR spawn_projectile
 *  loc_19F3E:
 *      JMP sub_197B4
 *  loc_19F41:
 *      LDA #$15 / STA $576,X / JMP sub_197B4
 *
 *  loc_19F94 belongs to enemy_type_18 and is reached from loc_19F02, so that
 *  block is reproduced here (see the file header and PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void sub_19EB4(void)                         /* $9EB4 */
{
    STY(byte_14);                            /* STY $14 */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    ANDI(0x01);                              /* AND #1 */
    BEQ(loc_19EBF);                          /* BEQ loc_19EBF */
    INC(byte_14);                            /* INC $14 */

loc_19EBF:                                   /* $9EBF */
    LDAI(0x15);                              /* LDA #$15 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDY(byte_14);                            /* LDY $14 */
    LDA(ABSY(unk_19FBC));                    /* LDA unk_19FBC,Y */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDA(ABSY(0x9FBE));                       /* LDA $9FBE,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    CMPI(0x92);                              /* CMP #$92 */
    BEQ(loc_19EF0);                          /* BEQ loc_19EF0 */
    LSR_A();                                 /* LSR */
    LDA(ABSY(unk_19FC0));                    /* LDA unk_19FC0,Y */
    STA(ABSX(byte_764));                     /* STA $764,X */
    BCC(loc_19F05);                          /* BCC loc_19F05 */
    LDA(ABSY(unk_19FBC));                    /* LDA unk_19FBC,Y */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    goto loc_19F05;                          /* JMP loc_19F05 */

loc_19EF0:                                   /* $9EF0 */
    LDAI(0x03);                              /* LDA #3 */
    STA(ABSX(byte_646));                     /* STA $646,X */
    LDA(ABSY(unk_19FC0));                    /* LDA unk_19FC0,Y */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    BEQ(loc_19F02);                          /* BEQ loc_19F02 */
    LDAI(0xFD);                              /* LDA #$FD */
    STA(ABSX(byte_646));                     /* STA $646,X */

loc_19F02:                                   /* $9F02 */
    goto loc_19F94;                          /* JMP loc_19F94 */

loc_19F05:                                   /* $9F05 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CLC();                                   /* CLC */
    ADCI(0x18);                              /* ADC #$18 */
    AND(ABSX(unk_67A));                      /* AND $67A,X */
    SEC();                                   /* SEC */
    SBCI(0x18);                              /* SBC #$18 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    BEQ(loc_19F41);                          /* BEQ loc_19F41 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CLC();                                   /* CLC */
    ADCI(0x10);                              /* ADC #$10 */
    AND(ABSX(unk_67A));                      /* AND $67A,X */
    SEC();                                   /* SEC */
    SBCI(0x10);                              /* SBC #$10 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    BEQ(loc_19F2A);                          /* BEQ loc_19F2A */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_19F2A:                                   /* $9F2A */
    LDAI(0x16);                              /* LDA #$16 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19F3E);                          /* BCS loc_19F3E */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x26);                              /* LDA #$26 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19F3E:                                   /* $9F3E */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_19F41:                                   /* $9F41 */
    LDAI(0x15);                              /* LDA #$15 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_19F94:                                   /* $9F94 - shared, owned by enemy_type_18 */
    DEC(ABSX(byte_764));                     /* DEC $764,X */
    BNE(loc_19FAD);                          /* BNE loc_19FAD */
    LDAI(0x30);                              /* LDA #$30 */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(0x15);                              /* LDA #$15 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19FAD);                          /* BCS loc_19FAD */
    LDAI(0x25);                              /* LDA #$25 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19FAD:                                   /* $9FAD */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    CMPI(0x08);                              /* CMP #8 */
    BNE(loc_19FB9);                          /* BNE loc_19FB9 */
    LDAI(0x16);                              /* LDA #$16 */
    STA(ABSX(byte_576));                     /* STA $576,X */

loc_19FB9:                                   /* $9FB9 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $9F49  enemy_type_17 - set up the object and continue in sub_19EB4
 *
 *  Original: src/bank6.asm:6004-6019 (bank 6)
 *      LDA $528,X / BMI loc_19F05 / LDA #$13 / STA $660,X / LDA #2 /
 *      STA $612,X / LDA #$FA / STA $646,X / LDA #$FF / STA $730,X /
 *      LDA #$E0 / STA $67A,X / LDY #6 / JMP sub_19EB4
 *
 *  loc_19F05 lives inside sub_19EB4, so the block is reproduced here (see the
 *  file header and PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void enemy_type_17(void)                     /* $9F49 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19F05);                          /* BMI loc_19F05 */
    LDAI(0x13);                              /* LDA #$13 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0xFA);                              /* LDA #$FA */
    STA(ABSX(byte_646));                     /* STA $646,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDAI(0xE0);                              /* LDA #$E0 */
    STA(ABSX(unk_67A));                      /* STA $67A,X */
    LDYI(6);                                 /* LDY #6 */
    sub_19EB4();                             /* JMP sub_19EB4 */
    RTS();

loc_19F05:                                   /* $9F05 - shared, owned by sub_19EB4 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CLC();                                   /* CLC */
    ADCI(0x18);                              /* ADC #$18 */
    AND(ABSX(unk_67A));                      /* AND $67A,X */
    SEC();                                   /* SEC */
    SBCI(0x18);                              /* SBC #$18 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    BEQ(loc_19F41);                          /* BEQ loc_19F41 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CLC();                                   /* CLC */
    ADCI(0x10);                              /* ADC #$10 */
    AND(ABSX(unk_67A));                      /* AND $67A,X */
    SEC();                                   /* SEC */
    SBCI(0x10);                              /* SBC #$10 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    BEQ(loc_19F2A);                          /* BEQ loc_19F2A */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_19F2A:                                   /* $9F2A */
    LDAI(0x16);                              /* LDA #$16 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19F3E);                          /* BCS loc_19F3E */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x26);                              /* LDA #$26 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19F3E:                                   /* $9F3E */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_19F41:                                   /* $9F41 */
    LDAI(0x15);                              /* LDA #$15 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $9F6C  enemy_type_18 - set up the object, then the loc_19F94 tail
 *
 *  Original: src/bank6.asm:6025-6067 (bank 6)
 *      LDA $528,X / BMI loc_19F94 / LDA #2 / STA $612,X / LDA #$1B /
 *      STA $660,X / LDA #$30 / STA $764,X / LDA #$E / STA $730,X /
 *      LDA #$44 / STA $6E2,X / LDA #8 / STA $716,X / LDY #$C / JMP sub_19EB4
 *  loc_19F94:
 *      DEC $764,X / BNE loc_19FAD / LDA #$30 / STA $764,X / LDA #$15 /
 *      STA $576,X / JSR check_enemy_overflow / BCS loc_19FAD / LDA #$25 /
 *      JSR spawn_projectile
 *  loc_19FAD:
 *      LDA $764,X / CMP #8 / BNE loc_19FB9 / LDA #$16 / STA $576,X
 *  loc_19FB9:
 *      JMP sub_197B4
 * ------------------------------------------------------------------------ */
void enemy_type_18(void)                     /* $9F6C */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_19F94);                          /* BMI loc_19F94 */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x1B);                              /* LDA #$1B */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x30);                              /* LDA #$30 */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(0x0E);                              /* LDA #$E */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDAI(0x44);                              /* LDA #$44 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDAI(0x08);                              /* LDA #8 */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDYI(0x0C);                              /* LDY #$C */
    sub_19EB4();                             /* JMP sub_19EB4 */
    RTS();

loc_19F94:                                   /* $9F94 */
    DEC(ABSX(byte_764));                     /* DEC $764,X */
    BNE(loc_19FAD);                          /* BNE loc_19FAD */
    LDAI(0x30);                              /* LDA #$30 */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(0x15);                              /* LDA #$15 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_19FAD);                          /* BCS loc_19FAD */
    LDAI(0x25);                              /* LDA #$25 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_19FAD:                                   /* $9FAD */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    CMPI(0x08);                              /* CMP #8 */
    BNE(loc_19FB9);                          /* BNE loc_19FB9 */
    LDAI(0x16);                              /* LDA #$16 */
    STA(ABSX(byte_576));                     /* STA $576,X */

loc_19FB9:                                   /* $9FB9 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $9FD6  enemy_type_22 - sub-type setup, then the loc_1A01B state machine
 *
 *  Original: src/bank6.asm:6100-6168 (bank 6)
 *      LDA $528,X / BMI loc_1A01B / ORA #$80 / STA $528,X / JSR enemy_prng /
 *      LDY #0 / LSR / BCC loc_19FEA / LDY #2
 *  loc_19FEA:
 *      LDA #9 / STA $660,X / LDA #2 / STA $590,X / LDA #$38 / STA $74A,X
 *  loc_19FF9:
 *      LDA $9FCE,Y / STA $55C,X / LDA $9FCF,Y / STA $646,X / LDA #5 /
 *      STA $612,X / LDA #$14 / STA $716,X / LDA #$1F / STA $576,X /
 *      LDA #$20 / LDY #5 / JSR sub_1A2CB
 *  loc_1A01B:
 *      LDA $5AA,X / BEQ loc_1A026 / CMP #1 / BNE loc_1A051 / BEQ loc_1A033
 *  loc_1A026:
 *      DEC $74A,X / BNE loc_1A051 / JSR sub_1A05E / LDA #1 / STA $5AA,X
 *  loc_1A033:
 *      JSR sub_183BC / BCS loc_1A04C / LDA $A054,Y / STA $576,X /
 *      LDA $590,X / AND #3 / ORA $A059,Y / STA $590,X / JMP loc_1A051
 *  loc_1A04C:
 *      LDA #2 / STA $5AA,X
 *  loc_1A051:
 *      JMP sub_197B4
 * ------------------------------------------------------------------------ */
void enemy_type_22(void)                     /* $9FD6 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1A01B);                          /* BMI loc_1A01B */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    LDYI(0);                                 /* LDY #0 */
    LSR_A();                                 /* LSR */
    BCC(loc_19FEA);                          /* BCC loc_19FEA */
    LDYI(2);                                 /* LDY #2 */

loc_19FEA:                                   /* $9FEA */
    LDAI(0x09);                              /* LDA #9 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x38);                              /* LDA #$38 */
    STA(ABSX(0x074A));                       /* STA $74A,X */

    /* $9FF9 (loc_19FF9) is only a fall-through point here - enemy_type_23
     * jumps to it and carries its own copy (PORTING.md section 5), exactly
     * like locret_C7EA in bank7/C6AB.c. */
    LDA(ABSY(0x9FCE));                       /* LDA $9FCE,Y */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDA(ABSY(0x9FCF));                       /* LDA $9FCF,Y */
    STA(ABSX(byte_646));                     /* STA $646,X */
    LDAI(0x05);                              /* LDA #5 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x14);                              /* LDA #$14 */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(0x1F);                              /* LDA #$1F */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(0x20);                              /* LDA #$20 */
    LDYI(5);                                 /* LDY #5 */
    sub_1A2CB();                             /* JSR sub_1A2CB */

loc_1A01B:                                   /* $A01B */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BEQ(loc_1A026);                          /* BEQ loc_1A026 */
    CMPI(0x01);                              /* CMP #1 */
    BNE(loc_1A051);                          /* BNE loc_1A051 */
    BEQ(loc_1A033);                          /* BEQ loc_1A033 */

loc_1A026:                                   /* $A026 */
    DEC(ABSX(0x074A));                       /* DEC $74A,X */
    BNE(loc_1A051);                          /* BNE loc_1A051 */
    sub_1A05E();                             /* JSR sub_1A05E */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */

loc_1A033:                                   /* $A033 */
    sub_183BC();                             /* JSR sub_183BC */
    BCS(loc_1A04C);                          /* BCS loc_1A04C */
    LDA(ABSY(0xA054));                       /* LDA $A054,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSX(byte_590));                     /* LDA $590,X */
    ANDI(0x03);                              /* AND #3 */
    ORA(ABSY(0xA059));                       /* ORA $A059,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    goto loc_1A051;                          /* JMP loc_1A051 */

loc_1A04C:                                   /* $A04C */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */

loc_1A051:                                   /* $A051 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A05E  sub_1A05E - start the "chase" state of enemy_type_22
 *
 *  Original: src/bank6.asm:6185-6208 (bank 6)
 *      LDA $528,X / CMP #$96 / BNE loc_1A077 / LDA #4 / STA $74A,X /
 *      JSR calculate_slope_and_speed / LDA #$C / STA $716,X / LDA #0 /
 *      STA $6E2,X
 *  loc_1A077:
 *      JSR check_enemy_overflow / BCS loc_1A081 / LDA #$25 /
 *      JSR spawn_projectile
 *  loc_1A081:
 *      LDA $660,X / ORA #2 / STA $660,X / RTS
 * ------------------------------------------------------------------------ */
void sub_1A05E(void)                         /* $A05E */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0x96);                              /* CMP #$96 */
    BNE(loc_1A077);                          /* BNE loc_1A077 */
    LDAI(0x04);                              /* LDA #4 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    calculate_slope_and_speed();             /* JSR calculate_slope_and_speed */
    LDAI(0x0C);                              /* LDA #$C */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */

loc_1A077:                                   /* $A077 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_1A081);                          /* BCS loc_1A081 */
    LDAI(0x25);                              /* LDA #$25 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_1A081:                                   /* $A081 */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ORAI(0x02);                              /* ORA #2 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A08A  enemy_type_23 - sub-type setup, then the loc_19FF9/loc_1A01B blocks
 *
 *  Original: src/bank6.asm:6214-6237 (bank 6)
 *      LDA $528,X / BMI loc_1A01B / ORA #$80 / STA $528,X / JSR enemy_prng /
 *      LDY #4 / LSR / BCC loc_1A09E / LDY #6
 *  loc_1A09E:
 *      LDA #$1B / STA $660,X / LDA #$10 / STA $730,X / LDA #3 / STA $590,X /
 *      LDA #$38 / STA $74A,X / LDA $9FCF,Y / STA $6FC,X / JMP loc_19FF9
 *
 *  loc_19FF9 and loc_1A01B live inside enemy_type_22, so those blocks are
 *  reproduced here (see the file header and PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void enemy_type_23(void)                     /* $A08A */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1A01B);                          /* BMI loc_1A01B */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    LDYI(4);                                 /* LDY #4 */
    LSR_A();                                 /* LSR */
    BCC(loc_1A09E);                          /* BCC loc_1A09E */
    LDYI(6);                                 /* LDY #6 */

loc_1A09E:                                   /* $A09E */
    LDAI(0x1B);                              /* LDA #$1B */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDAI(0x03);                              /* LDA #3 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x38);                              /* LDA #$38 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDA(ABSY(0x9FCF));                       /* LDA $9FCF,Y */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    goto loc_19FF9;                          /* JMP loc_19FF9 */

loc_19FF9:                                   /* $9FF9 - shared, owned by enemy_type_22 */
    LDA(ABSY(0x9FCE));                       /* LDA $9FCE,Y */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDA(ABSY(0x9FCF));                       /* LDA $9FCF,Y */
    STA(ABSX(byte_646));                     /* STA $646,X */
    LDAI(0x05);                              /* LDA #5 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDAI(0x14);                              /* LDA #$14 */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(0x1F);                              /* LDA #$1F */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(0x20);                              /* LDA #$20 */
    LDYI(5);                                 /* LDY #5 */
    sub_1A2CB();                             /* JSR sub_1A2CB */
    /* falls through into loc_1A01B, exactly like $A01B below */

loc_1A01B:                                   /* $A01B */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BEQ(loc_1A026);                          /* BEQ loc_1A026 */
    CMPI(0x01);                              /* CMP #1 */
    BNE(loc_1A051);                          /* BNE loc_1A051 */
    BEQ(loc_1A033);                          /* BEQ loc_1A033 */

loc_1A026:                                   /* $A026 */
    DEC(ABSX(0x074A));                       /* DEC $74A,X */
    BNE(loc_1A051);                          /* BNE loc_1A051 */
    sub_1A05E();                             /* JSR sub_1A05E */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */

loc_1A033:                                   /* $A033 */
    sub_183BC();                             /* JSR sub_183BC */
    BCS(loc_1A04C);                          /* BCS loc_1A04C */
    LDA(ABSY(0xA054));                       /* LDA $A054,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSX(byte_590));                     /* LDA $590,X */
    ANDI(0x03);                              /* AND #3 */
    ORA(ABSY(0xA059));                       /* ORA $A059,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    goto loc_1A051;                          /* JMP loc_1A051 */

loc_1A04C:                                   /* $A04C */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */

loc_1A051:                                   /* $A051 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A0BB  enemy_type_26 - spawn from the top and continue in sub_1A0F9
 *
 *  Original: src/bank6.asm:6243-6269 (bank 6)
 *      LDA $528,X / BMI sub_1A110 / ORA #$80 / STA $528,X /
 *      JSR spawn_top_random_x_position / JSR enemy_prng / AND #$7F /
 *      ADC #$10 / STA $542,X / LDA #$1B / STA $660,X / LDA #$48 /
 *      STA $6E2,X / LDA #$80 / STA $6FC,X / LDA #$10 / STA $716,X /
 *      LDA #$10 / STA $730,X / LDA #0 / STA $7CC,X / LDA #$78 / STA $7B2,X /
 *      LDY #3 / LDA #$28
 *  (no RTS: execution continues into sub_1A0F9)
 *
 *  `BMI sub_1A110` at $A0BE branches to the entry of another procedure; the
 *  local label below calls it and returns, which reproduces the flow.
 * ------------------------------------------------------------------------ */
void enemy_type_26(void)                     /* $A0BB */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(sub_1A110);                          /* BMI sub_1A110 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    ANDI(0x7F);                              /* AND #$7F */
    ADCI(0x10);                              /* ADC #$10 */
    STA(ABSX(byte_542));                     /* STA $542,X */
    LDAI(0x1B);                              /* LDA #$1B */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x48);                              /* LDA #$48 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_7CC));                     /* STA $7CC,X */
    LDAI(0x78);                              /* LDA #$78 */
    STA(ABSX(byte_7B2));                     /* STA $7B2,X */
    LDYI(3);                                 /* LDY #3 */
    LDAI(0x28);                              /* LDA #$28 */
    sub_1A0F9();                             /* falls through into $A0F9 */
    RTS();

sub_1A110:                                   /* $A110 - another procedure's entry; the
                                              * BMI above becomes this call */
    sub_1A110();                             /* JMP sub_1A110 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A0F9  sub_1A0F9 - finish a sub-type setup and continue in sub_1A110
 *
 *  Original: src/bank6.asm:6275-6287 (bank 6)
 *      STA $5AA,X / LSR / STA $74A,X / TYA / STA $764,X / LDA #$20 /
 *      LDY #$10 / JSR sub_1A2CB / LDA #2 / STA $590,X
 *  (no RTS: execution continues into sub_1A110)
 * ------------------------------------------------------------------------ */
void sub_1A0F9(void)                         /* $A0F9 */
{
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LSR_A();                                 /* LSR */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    TYA();                                   /* TYA */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDAI(0x20);                              /* LDA #$20 */
    LDYI(0x10);                              /* LDY #$10 */
    sub_1A2CB();                             /* JSR sub_1A2CB */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    sub_1A110();                             /* falls through into $A110 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A110  sub_1A110 - the common handler tail of the "chase" objects
 *
 *  Original: src/bank6.asm:6293-6342 (bank 6)
 *      JSR sub_183BC / STA $10 / AND #3 / TAY / LDA unk_1A165,Y /
 *      STA $576,X / LDA $528,X / CMP #$9A / BNE loc_1A14D / LDA $7CC,X /
 *      BNE loc_1A139 / LDA #$20 / STA $590,X / DEC $7B2,X / BNE loc_1A149 /
 *      LDA #$FF / STA $7CC,X
 *  loc_1A139:
 *      LDA $10 / LSR / LSR / LSR / TAY / LDA $A169,Y / STA $590,X /
 *      AND #$20 / BEQ loc_1A14D
 *  loc_1A149:
 *      JSR process_enemy_flags_sprites / RTS
 *  loc_1A14D:
 *      DEC $74A,X / BNE loc_1A15B / LDA $5AA,X / STA $74A,X / JSR sub_1A264
 *  loc_1A15B:
 *      JSR process_enemy_flags_sprites / JSR sub_181F6 /
 *      JSR decrease_enemy_health / RTS
 * ------------------------------------------------------------------------ */
void sub_1A110(void)                         /* $A110 */
{
    sub_183BC();                             /* JSR sub_183BC */
    STA(byte_10);                            /* STA $10 */
    ANDI(0x03);                              /* AND #3 */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_1A165));                    /* LDA unk_1A165,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0x9A);                              /* CMP #$9A */
    BNE(loc_1A14D);                          /* BNE loc_1A14D */
    LDA(ABSX(byte_7CC));                     /* LDA $7CC,X */
    BNE(loc_1A139);                          /* BNE loc_1A139 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    DEC(ABSX(byte_7B2));                     /* DEC $7B2,X */
    BNE(loc_1A149);                          /* BNE loc_1A149 */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_7CC));                     /* STA $7CC,X */

loc_1A139:                                   /* $A139 */
    LDA(byte_10);                            /* LDA $10 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    TAY();                                   /* TAY */
    LDA(ABSY(0xA169));                       /* LDA $A169,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_1A14D);                          /* BEQ loc_1A14D */

loc_1A149:                                   /* $A149 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    RTS();                                   /* RTS */

loc_1A14D:                                   /* $A14D */
    DEC(ABSX(0x074A));                       /* DEC $74A,X */
    BNE(loc_1A15B);                          /* BNE loc_1A15B */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    sub_1A264();                             /* JSR sub_1A264 */

loc_1A15B:                                   /* $A15B */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JSR sub_181F6 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A16B  enemy_type_27 - set up the object, then continue in sub_1A0F9
 *
 *  Original: src/bank6.asm:6355-6382 (bank 6)
 *      LDA $528,X
 *  loc_1A16E:
 *      BMI sub_1A110 / ORA #$80 / STA $528,X / LDA #$20 / STA $55C,X /
 *      LDA #2 / STA $646,X / JSR enemy_prng / LSR / BCC loc_1A18F /
 *      LDA #$E0 / STA $55C,X / LDA #$FE / STA $646,X
 *  loc_1A18F:
 *      LDA #2 / STA $660,X / LDA #48 / STA $542,X / LDY #1 / LDA #$10 /
 *      JMP sub_1A0F9
 *
 *  loc_1A16E is also the branch target of enemy_type_28.  `BMI sub_1A110`
 *  branches to the entry of another procedure; the local label below calls it.
 * ------------------------------------------------------------------------ */
void enemy_type_27(void)                     /* $A16B */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */

    /* $A16E (loc_1A16E) is only a fall-through point here - enemy_type_28
     * branches to it and carries its own copy (PORTING.md section 5), exactly
     * like locret_C7EA in bank7/C6AB.c. */
    BMI(sub_1A110);                          /* BMI sub_1A110 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(0x20);                              /* LDA #$20 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_646));                     /* STA $646,X */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    LSR_A();                                 /* LSR */
    BCC(loc_1A18F);                          /* BCC loc_1A18F */
    LDAI(0xE0);                              /* LDA #$E0 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0xFE);                              /* LDA #$FE */
    STA(ABSX(byte_646));                     /* STA $646,X */

loc_1A18F:                                   /* $A18F */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x30);                              /* LDA #48 */
    STA(ABSX(byte_542));                     /* STA $542,X */
    LDYI(1);                                 /* LDY #1 */
    LDAI(0x10);                              /* LDA #$10 */
    sub_1A0F9();                             /* JMP sub_1A0F9 */
    RTS();

sub_1A110:                                   /* $A110 - another procedure's entry; the
                                              * BMI above becomes this call */
    sub_1A110();                             /* JMP sub_1A110 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A1A0  enemy_type_28 - spawn from the top and continue in sub_1A0F9
 *
 *  Original: src/bank6.asm:6388-6409 (bank 6)
 *      LDA $528,X / BMI loc_1A16E / ORA #$80 / STA $528,X /
 *      JSR spawn_top_random_x_position / LDA #1 / STA $660,X / LDA #1 /
 *      STA $612,X / LDY #6 / LDA $49 / CMP #$50 / BCS loc_1A1C4 /
 *      LDA #$28 / JMP sub_1A0F9
 *  loc_1A1C4:
 *      LDA #$20 / JMP sub_1A0F9
 *
 *  loc_1A16E lives inside enemy_type_27; the BMI at $A1A2 enters it, so the
 *  block is reproduced here (see the file header and PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void enemy_type_28(void)                     /* $A1A0 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1A16E);                          /* BMI loc_1A16E */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    spawn_top_random_x_position();           /* JSR spawn_top_random_x_position */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDYI(6);                                 /* LDY #6 */
    LDA(byte_49);                            /* LDA $49 */
    CMPI(0x50);                              /* CMP #$50 */
    BCS(loc_1A1C4);                          /* BCS loc_1A1C4 */
    LDAI(0x28);                              /* LDA #$28 */
    sub_1A0F9();                             /* JMP sub_1A0F9 */
    RTS();

loc_1A1C4:                                   /* $A1C4 */
    LDAI(0x20);                              /* LDA #$20 */
    sub_1A0F9();                             /* JMP sub_1A0F9 */
    RTS();

loc_1A16E:                                   /* $A16E - shared, owned by enemy_type_27 */
    BMI(sub_1A110);                          /* BMI sub_1A110 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    LDAI(0x20);                              /* LDA #$20 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_646));                     /* STA $646,X */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    LSR_A();                                 /* LSR */
    BCC(loc_1A18F);                          /* BCC loc_1A18F */
    LDAI(0xE0);                              /* LDA #$E0 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0xFE);                              /* LDA #$FE */
    STA(ABSX(byte_646));                     /* STA $646,X */

loc_1A18F:                                   /* $A18F */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x30);                              /* LDA #48 */
    STA(ABSX(byte_542));                     /* STA $542,X */
    LDYI(1);                                 /* LDY #1 */
    LDAI(0x10);                              /* LDA #$10 */
    sub_1A0F9();                             /* JMP sub_1A0F9 */
    RTS();

sub_1A110:                                   /* $A110 - another procedure's entry; the
                                              * BMI above becomes this call */
    sub_1A110();                             /* JMP sub_1A110 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A1C9  enemy_type_29_effine_T4 - "Effine Type4 (shoot Sig)"
 *
 *  Original: src/bank6.asm:6416-6493 (bank 6)
 *      LDA $528,X / BMI loc_1A201 / ORA #$80 / STA $528,X / JSR enemy_prng /
 *      AND #$7F / ADC #64 / STA $55C,X / LDA #8 / STA $542,X / LDA #1 /
 *      STA $74A,X / LDA #3 / STA $660,X / JSR calculate_slope / STA $716,X /
 *      JSR set_speed_by_slope / LDA #80 / STA $730,X / LDY #2 / LDA #$50 /
 *      JMP sub_1A0F9
 *  loc_1A201:
 *      DEC $730,X / LDA $730,X / BNE loc_1A20F / INC $730,X / JMP sub_1A110
 *  loc_1A20F:
 *      PHA / LDA $49 / LDY #8 / CMP #$20 / BCC loc_1A226 / LDY #$C /
 *      CMP #$50 / BCC loc_1A226 / LDY #$10 / CMP #$80 / BCC loc_1A226 /
 *      LDY #$14
 *  loc_1A226:
 *      STY $10 / PLA / CMP $10 / BCS loc_1A25B / AND #3 / BNE loc_1A25B /
 *      LDA $716,X / STA $14 / JSR check_enemy_overflow / BCS loc_1A25B /
 *      LDA #12 / STA $74A,Y / LDA #41 / JSR spawn_projectile /
 *      LDA $55C,Y / CLC / ADC #$FA / STA $55C,Y / LDA $542,Y / CLC /
 *      ADC #$28 / STA $542,Y / TXA / STA $5AA,Y
 *  loc_1A25B:
 *      JMP sub_1A110
 * ------------------------------------------------------------------------ */
void enemy_type_29_effine_T4(void)           /* $A1C9 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1A201);                          /* BMI loc_1A201 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    ANDI(0x7F);                              /* AND #$7F */
    ADCI(0x40);                              /* ADC #64 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0x08);                              /* LDA #8 */
    STA(ABSX(byte_542));                     /* STA $542,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDAI(0x03);                              /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    calculate_slope();                       /* JSR calculate_slope */
    STA(ABSX(byte_716));                     /* STA $716,X */
    set_speed_by_slope();                    /* JSR set_speed_by_slope */
    LDAI(0x50);                              /* LDA #80 */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDYI(2);                                 /* LDY #2 */
    LDAI(0x50);                              /* LDA #$50 */
    sub_1A0F9();                             /* JMP sub_1A0F9 */
    RTS();

loc_1A201:                                   /* $A201 */
    DEC(ABSX(0x0730));                       /* DEC $730,X */
    LDA(ABSX(0x0730));                       /* LDA $730,X */
    BNE(loc_1A20F);                          /* BNE loc_1A20F */
    INC(ABSX(0x0730));                       /* INC $730,X */
    sub_1A110();                             /* JMP sub_1A110 */
    RTS();

loc_1A20F:                                   /* $A20F */
    PHA();                                   /* PHA */
    LDA(byte_49);                            /* LDA $49 */
    LDYI(8);                                 /* LDY #8 */
    CMPI(0x20);                              /* CMP #$20 */
    BCC(loc_1A226);                          /* BCC loc_1A226 */
    LDYI(0x0C);                              /* LDY #$C */
    CMPI(0x50);                              /* CMP #$50 */
    BCC(loc_1A226);                          /* BCC loc_1A226 */
    LDYI(0x10);                              /* LDY #$10 */
    CMPI(0x80);                              /* CMP #$80 */
    BCC(loc_1A226);                          /* BCC loc_1A226 */
    LDYI(0x14);                              /* LDY #$14 */

loc_1A226:                                   /* $A226 */
    STY(byte_10);                            /* STY $10 */
    PLA();                                   /* PLA */
    CMP(byte_10);                            /* CMP $10 */
    BCS(loc_1A25B);                          /* BCS loc_1A25B */
    ANDI(0x03);                              /* AND #3 */
    BNE(loc_1A25B);                          /* BNE loc_1A25B */
    LDA(ABSX(byte_716));                     /* LDA $716,X */
    STA(byte_14);                            /* STA $14 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_1A25B);                          /* BCS loc_1A25B */
    LDAI(0x0C);                              /* LDA #12 */
    STA(ABSY(0x074A));                       /* STA $74A,Y */
    LDAI(0x29);                              /* LDA #41 */
    spawn_projectile();                      /* JSR spawn_projectile */
    LDA(ABSY(byte_55C));                     /* LDA $55C,Y */
    CLC();                                   /* CLC */
    ADCI(0xFA);                              /* ADC #$FA */
    STA(ABSY(byte_55C));                     /* STA $55C,Y */
    LDA(ABSY(byte_542));                     /* LDA $542,Y */
    CLC();                                   /* CLC */
    ADCI(0x28);                              /* ADC #$28 */
    STA(ABSY(byte_542));                     /* STA $542,Y */
    TXA();                                   /* TXA */
    STA(ABSY(byte_5AA));                     /* STA $5AA,Y */

loc_1A25B:                                   /* $A25B */
    sub_1A110();                             /* JMP sub_1A110 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A264  sub_1A264 - launch the object's special projectile
 *
 *  Original: src/bank6.asm:6506-6554 (bank 6)
 *      JSR check_enemy_overflow / BCS locret_1A2B0 / LDA $528,X / CMP #$9A /
 *      BNE loc_1A275 / LDA #$25 / JMP spawn_projectile
 *  loc_1A275:
 *      CMP #$9B / BNE loc_1A283 / JSR calculate_slope / STA $14 /
 *      LDA #$15 / JMP spawn_projectile
 *  loc_1A283:
 *      CMP #$9C / BNE loc_1A2A7 / LDA #$28 / STA $528,Y / STY $15 /
 *      JSR check_enemy_overflow / BCS locret_1A2B0 / LDA #2 / STA $14 /
 *      LDA #$26 / JSR spawn_projectile / LDY $15 / LDA #6 / STA $14 /
 *      LDA #$26 / JMP spawn_projectile
 *  loc_1A2A7:
 *      LDA #4 / STA $14 / LDA #$3B / JMP spawn_projectile
 *  locret_1A2B0:
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1A264(void)                         /* $A264 */
{
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_1A2B0);                       /* BCS locret_1A2B0 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0x9A);                              /* CMP #$9A */
    BNE(loc_1A275);                          /* BNE loc_1A275 */
    LDAI(0x25);                              /* LDA #$25 */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();

loc_1A275:                                   /* $A275 */
    CMPI(0x9B);                              /* CMP #$9B */
    BNE(loc_1A283);                          /* BNE loc_1A283 */
    calculate_slope();                       /* JSR calculate_slope */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x15);                              /* LDA #$15 */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();

loc_1A283:                                   /* $A283 */
    CMPI(0x9C);                              /* CMP #$9C */
    BNE(loc_1A2A7);                          /* BNE loc_1A2A7 */
    LDAI(0x28);                              /* LDA #$28 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    STY(byte_15);                            /* STY $15 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(locret_1A2B0);                       /* BCS locret_1A2B0 */
    LDAI(0x02);                              /* LDA #2 */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x26);                              /* LDA #$26 */
    spawn_projectile();                      /* JSR spawn_projectile */
    LDY(byte_15);                            /* LDY $15 */
    LDAI(0x06);                              /* LDA #6 */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x26);                              /* LDA #$26 */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();

loc_1A2A7:                                   /* $A2A7 */
    LDAI(0x04);                              /* LDA #4 */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x3B);                              /* LDA #$3B */
    spawn_projectile();                      /* JMP spawn_projectile */
    RTS();

locret_1A2B0:                                /* $A2B0 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A2B1  decrease_enemy_health - "For tough enemies"
 *
 *  Original: src/bank6.asm:6561-6581 (bank 6)
 *      LDA $528,X / BPL loc_1A2B9 / PLA / PLA
 *  locret_1A2B8:
 *      RTS
 *  loc_1A2B9:
 *      DEC $764,X / BEQ locret_1A2B8 / LDA #20 / JSR j_apu_play / LDA $88 /
 *      ORA #$80 / STA $528,X / RTS
 *
 *  The PLA/PLA at $A2B6-$A2B7 is a "double return": the pops discard this
 *  routine's own JSR return address, so the RTS at $A2B8 resumes the enemy
 *  slot loop at $8807 and the calling handler is abandoned (bank6_report.md
 *  section 3.3, PORTING.md section 4c).  A C call keeps that return address on
 *  the CPU stack, so - like the return-address PLAs of print_string
 *  (PORTING.md section 4) - the two pops are modelled instead of executed:
 *  returning here unwinds the handler chain exactly as the original RTS did.
 *  The callers in this file (sub_1A110, enemy_type_34_65_66) call this routine
 *  as their last statement, so the handler is abandoned as in the original.
 *  TODO: unclear for a caller that tests the flags right after the JSR (e.g.
 *  enemy_type_90 in bank 6): the C model has no 6502 return frame to unwind.
 * ------------------------------------------------------------------------ */
void decrease_enemy_health(void)             /* $A2B1 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BPL(loc_1A2B9);                          /* BPL loc_1A2B9 */
    /* PLA / PLA ($A2B6-$A2B7) then the RTS below: the pops discard this
     * routine's JSR frame, so the RTS resumes the dispatcher's slot loop
     * ($8807) and the calling handler is abandoned - the port's longjmp. */
    ZANAC_UNWIND();
    RTS();                                   /* RTS ($A2B8 locret_1A2B8) */

locret_1A2B8:                                /* $A2B8 (the normal-return path) */
    RTS();                                   /* RTS */

loc_1A2B9:                                   /* $A2B9 */
    DEC(ABSX(byte_764));                     /* DEC $764,X */
    BEQ(locret_1A2B8);                       /* BEQ locret_1A2B8 */
    LDAI(0x14);                              /* LDA #20 */
    j_apu_play();                            /* JSR j_apu_play */
    LDA(0x0088);                             /* LDA $88 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A2CB  sub_1A2CB - store the sprite/speed pair of a sub-type
 *
 *  Original: src/bank6.asm:6587-6596 (bank 6)
 *      STA $67A,X / TYA / STA $6C8,X / LDA #0 / STA $6AE,X / STA $694,X / RTS
 * ------------------------------------------------------------------------ */
void sub_1A2CB(void)                         /* $A2CB */
{
    STA(ABSX(unk_67A));                      /* STA $67A,X */
    TYA();                                   /* TYA */
    STA(ABSX(0x06C8));                       /* STA $6C8,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A2DB  enemy_type_30_32 - the two-object "docking" pair
 *
 *  Original: src/bank6.asm:6602-6713 (bank 6)
 *      LDA $528,X / BPL loc_1A2E3 / JMP loc_1A379
 *  loc_1A2E3:
 *      ORA #$80 / STA $528,X / JSR sub_1876A / LDA $528,X / ADC #1 /
 *      STA $528,Y / LDA #1 / STA $660,X / STA $660,Y / LDA #$80 /
 *      STA $5F8,X / STA $62C,X / STA $5F8,Y / LDA #$80 / STA $62C,Y /
 *      LDA #1 / STA $612,X / STA $646,X / STA $612,Y / LDA #$FE /
 *      STA $646,Y / LDA #$20 / STA $55C,X / LDA #$E0 / STA $55C,Y /
 *      LDA #$23 / STA $576,X / STA $576,Y / LDA #2 / STA $590,X /
 *      LDA #$42 / STA $590,Y / LDA $528,X / CMP #$9E / BEQ loc_1A376 /
 *      LDA #$40 / STA $5AA,X / STA $5AA,Y / LDA #0 / STA $5F8,X /
 *      STA $5F8,Y / STA $62C,X / STA $62C,Y / LDA #$FF / STA $612,X /
 *      STA $612,Y / STA $646,Y / LDA #1 / STA $646,X / LDA #$F0 /
 *      STA $542,X / STA $542,Y / LDA #$83 / STA $590,X / LDA #$C3 /
 *      STA $590,Y
 *  loc_1A376:
 *      JMP loc_1A3C5
 *  loc_1A379:
 *      LDA $5AA,X / BMI loc_1A3C5 / LDA $764,X / TAY / LDA $528,X / CLC /
 *      ADC #1 / CMP $528,Y / BNE loc_1A3C8 / JSR sub_1A3D7 / BCS loc_1A39A /
 *      LDA #2 / STA $660,X / STA $660,Y
 *  loc_1A39A:
 *      LDA $55C,Y / SEC / SBC $55C,X / CMP #$B / BCS loc_1A3C5 /
 *      LDA $5AA,X / ORA #$80 / STA $5AA,X / LDA #$22 / STA $576,X /
 *      LDA $55C,X / CLC / ADC #5 / STA $55C,X / LDA #1 / STA $660,X /
 *      LDA #$28 / STA $528,Y
 *  loc_1A3C5:
 *      JMP sub_197B4
 *  loc_1A3C8:
 *      INC $528,X
 *  (no RTS: execution continues into enemy_type_31_33)
 * ------------------------------------------------------------------------ */
void enemy_type_30_32(void)                  /* $A2DB */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BPL(loc_1A2E3);                          /* BPL loc_1A2E3 */
    goto loc_1A379;                          /* JMP loc_1A379 */

loc_1A2E3:                                   /* $A2E3 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    sub_1876A();                             /* JSR sub_1876A */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ADCI(0x01);                              /* ADC #1 */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    STA(ABSY(byte_660));                     /* STA $660,Y */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    STA(ABSY(byte_5F8));                     /* STA $5F8,Y */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSY(byte_62C));                     /* STA $62C,Y */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_612));                     /* STA $612,X */
    STA(ABSX(byte_646));                     /* STA $646,X */
    STA(ABSY(byte_612));                     /* STA $612,Y */
    LDAI(0xFE);                              /* LDA #$FE */
    STA(ABSY(byte_646));                     /* STA $646,Y */
    LDAI(0x20);                              /* LDA #$20 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0xE0);                              /* LDA #$E0 */
    STA(ABSY(byte_55C));                     /* STA $55C,Y */
    LDAI(0x23);                              /* LDA #$23 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    STA(ABSY(byte_576));                     /* STA $576,Y */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x42);                              /* LDA #$42 */
    STA(ABSY(byte_590));                     /* STA $590,Y */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0x9E);                              /* CMP #$9E */
    BEQ(loc_1A376);                          /* BEQ loc_1A376 */
    LDAI(0x40);                              /* LDA #$40 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    STA(ABSY(byte_5AA));                     /* STA $5AA,Y */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    STA(ABSY(byte_5F8));                     /* STA $5F8,Y */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    STA(ABSY(byte_62C));                     /* STA $62C,Y */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(byte_612));                     /* STA $612,X */
    STA(ABSY(byte_612));                     /* STA $612,Y */
    STA(ABSY(byte_646));                     /* STA $646,Y */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_646));                     /* STA $646,X */
    LDAI(0xF0);                              /* LDA #$F0 */
    STA(ABSX(byte_542));                     /* STA $542,X */
    STA(ABSY(byte_542));                     /* STA $542,Y */
    LDAI(0x83);                              /* LDA #$83 */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0xC3);                              /* LDA #$C3 */
    STA(ABSY(byte_590));                     /* STA $590,Y */

loc_1A376:                                   /* $A376 */
    goto loc_1A3C5;                          /* JMP loc_1A3C5 */

loc_1A379:                                   /* $A379 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BMI(loc_1A3C5);                          /* BMI loc_1A3C5 */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    TAY();                                   /* TAY */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CLC();                                   /* CLC */
    ADCI(0x01);                              /* ADC #1 */
    CMP(ABSY(byte_528));                     /* CMP $528,Y */
    BNE(loc_1A3C8);                          /* BNE loc_1A3C8 */
    sub_1A3D7();                             /* JSR sub_1A3D7 */
    BCS(loc_1A39A);                          /* BCS loc_1A39A */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    STA(ABSY(byte_660));                     /* STA $660,Y */

loc_1A39A:                                   /* $A39A */
    LDA(ABSY(byte_55C));                     /* LDA $55C,Y */
    SEC();                                   /* SEC */
    SBC(ABSX(byte_55C));                     /* SBC $55C,X */
    CMPI(0x0B);                              /* CMP #$B */
    BCS(loc_1A3C5);                          /* BCS loc_1A3C5 */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LDAI(0x22);                              /* LDA #$22 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    CLC();                                   /* CLC */
    ADCI(0x05);                              /* ADC #5 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x28);                              /* LDA #$28 */
    STA(ABSY(byte_528));                     /* STA $528,Y */

loc_1A3C5:                                   /* $A3C5 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();

loc_1A3C8:                                   /* $A3C8 */
    INC(ABSX(byte_528));                     /* INC $528,X */
    enemy_type_31_33();                      /* falls through into $A3CB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A3CB  enemy_type_31_33 - docked pair variant
 *
 *  Original: src/bank6.asm:6719-6725 (bank 6)
 *      JSR sub_1A3D7 / BCS loc_1A3C5 / LDA #2 / STA $660,X / BNE loc_1A3C5
 *
 *  loc_1A3C5 lives inside enemy_type_30_32; both branches enter it, so the
 *  shared tail is reproduced here (see the file header, PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void enemy_type_31_33(void)                  /* $A3CB */
{
    sub_1A3D7();                             /* JSR sub_1A3D7 */
    BCS(loc_1A3C5);                          /* BCS loc_1A3C5 */
    LDAI(0x02);                              /* LDA #2 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    BNE(loc_1A3C5);                          /* BNE loc_1A3C5 */

loc_1A3C5:                                   /* $A3C5 - shared, owned by enemy_type_30_32 */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $A3D7  sub_1A3D7 - compare the object's Y with Zanac's Y (C = result)
 *
 *  Original: src/bank6.asm:6731-6745 (bank 6)
 *      LDA $5AA,X / AND #$40 / BEQ loc_1A3E5 / LDA $542,X / CMP $542 / RTS
 *  loc_1A3E5:
 *      LDA $542 / CMP $542,X / RTS
 * ------------------------------------------------------------------------ */
void sub_1A3D7(void)                         /* $A3D7 */
{
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    ANDI(0x40);                              /* AND #$40 */
    BEQ(loc_1A3E5);                          /* BEQ loc_1A3E5 */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMP(byte_542);                           /* CMP $542 */
    RTS();                                   /* RTS */

loc_1A3E5:                                   /* $A3E5 */
    LDA(byte_542);                           /* LDA $542 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A3EC  enemy_type_34_65_66 - the chasing projectile launcher
 *
 *  Original: src/bank6.asm:6751-6872 (bank 6)
 *      LDA $528,X / BMI loc_1A44A / ORA #$80 / STA $528,X / JSR enemy_prng /
 *      AND #6 / TAY / LDA $A4DF,Y / STA $55C,X / LDA #1 / STA $74A,X /
 *      LDA $A4E0,Y / JSR set_speed_by_slope / LDA #3 / STA $660,X /
 *      LDA $528,X / LDY #0 / CMP #$A2 / BEQ loc_1A423 / LDY #6 / CMP #$C1 /
 *      BEQ loc_1A423 / LDY #$C
 *  loc_1A423:
 *      LDA $A4E7,Y / STA $716,X / STA $730,X / LDA $A4E8,Y / STA $6E2,X /
 *      LDA $A4E9,Y / STA $6FC,X / LDA $A4EA,Y / STA $764,X / LDA $A4EB,Y /
 *      STA $590,X / LDA $A4EC,Y / STA $576,X
 *  loc_1A44A:
 *      DEC $730,X / BEQ loc_1A452 / JMP loc_1A4D5
 *  loc_1A452:
 *      LDA $716,X / STA $730,X / LDA $6E2,X / STA $15 / LDA $528,X /
 *      CMP #$C2 / BNE loc_1A4A9 / LDA #$15 / JSR j_apu_play /
 *      JSR calculate_slope / STA $14 / LDY #0
 *  loc_1A470:
 *      STY $16 / JSR check_enemy_overflow / BCS loc_1A4D5 / STY $17 /
 *      LDY $16 / LDA $A4FF,Y / PHA / LDA $A500,Y / LDY $17 / CLC /
 *      ADC $55C,X / STA $55C,Y / PLA / CLC / ADC $542,X / STA $542,Y /
 *      LDA #$3B / STA $528,Y / LDA $14 / STA $77E,Y / INC $16 / INC $16 /
 *      LDY $16 / DEC $15 / BNE loc_1A470 / BEQ loc_1A4D5
 *  loc_1A4A9:
 *      LDA $542 / CMP $542,X / LDY #0 / BCC loc_1A4B5 / LDY #3
 *  loc_1A4B5:
 *      LDA $A4F9,Y / STA $14 / INY / STY $16 / JSR check_enemy_overflow /
 *      BCS loc_1A4D5 / LDA $6FC,X / CMP #$14 / BNE loc_1A4CC /
 *      JSR sub_1981D
 *  loc_1A4CC:
 *      JSR spawn_projectile / LDY $16 / DEC $15 / BNE loc_1A4B5
 *  loc_1A4D5:
 *      JSR process_enemy_flags_sprites / JSR sub_181F6 /
 *      JSR decrease_enemy_health / RTS
 * ------------------------------------------------------------------------ */
void enemy_type_34_65_66(void)               /* $A3EC */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1A44A);                          /* BMI loc_1A44A */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    ANDI(0x06);                              /* AND #6 */
    TAY();                                   /* TAY */
    LDA(ABSY(0xA4DF));                       /* LDA $A4DF,Y */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(0x074A));                       /* STA $74A,X */
    LDA(ABSY(0xA4E0));                       /* LDA $A4E0,Y */
    set_speed_by_slope();                    /* JSR set_speed_by_slope */
    LDAI(0x03);                              /* LDA #3 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    LDYI(0);                                 /* LDY #0 */
    CMPI(0xA2);                              /* CMP #$A2 */
    BEQ(loc_1A423);                          /* BEQ loc_1A423 */
    LDYI(6);                                 /* LDY #6 */
    CMPI(0xC1);                              /* CMP #$C1 */
    BEQ(loc_1A423);                          /* BEQ loc_1A423 */
    LDYI(0x0C);                              /* LDY #$C */

loc_1A423:                                   /* $A423 */
    LDA(ABSY(0xA4E7));                       /* LDA $A4E7,Y */
    STA(ABSX(byte_716));                     /* STA $716,X */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDA(ABSY(0xA4E8));                       /* LDA $A4E8,Y */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    LDA(ABSY(0xA4E9));                       /* LDA $A4E9,Y */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    LDA(ABSY(0xA4EA));                       /* LDA $A4EA,Y */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDA(ABSY(0xA4EB));                       /* LDA $A4EB,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDA(ABSY(0xA4EC));                       /* LDA $A4EC,Y */
    STA(ABSX(byte_576));                     /* STA $576,X */

loc_1A44A:                                   /* $A44A */
    DEC(ABSX(0x0730));                       /* DEC $730,X */
    BEQ(loc_1A452);                          /* BEQ loc_1A452 */
    goto loc_1A4D5;                          /* JMP loc_1A4D5 */

loc_1A452:                                   /* $A452 */
    LDA(ABSX(byte_716));                     /* LDA $716,X */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDA(ABSX(byte_6E2));                     /* LDA $6E2,X */
    STA(byte_15);                            /* STA $15 */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    CMPI(0xC2);                              /* CMP #$C2 */
    BNE(loc_1A4A9);                          /* BNE loc_1A4A9 */
    LDAI(0x15);                              /* LDA #$15 */
    j_apu_play();                            /* JSR j_apu_play */
    calculate_slope();                       /* JSR calculate_slope */
    STA(byte_14);                            /* STA $14 */
    LDYI(0);                                 /* LDY #0 */

loc_1A470:                                   /* $A470 */
    STY(byte_16);                            /* STY $16 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_1A4D5);                          /* BCS loc_1A4D5 */
    STY(byte_17);                            /* STY $17 */
    LDY(byte_16);                            /* LDY $16 */
    LDA(ABSY(0xA4FF));                       /* LDA $A4FF,Y */
    PHA();                                   /* PHA */
    LDA(ABSY(0xA500));                       /* LDA $A500,Y */
    LDY(byte_17);                            /* LDY $17 */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_55C));                     /* ADC $55C,X */
    STA(ABSY(byte_55C));                     /* STA $55C,Y */
    PLA();                                   /* PLA */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_542));                     /* ADC $542,X */
    STA(ABSY(byte_542));                     /* STA $542,Y */
    LDAI(0x3B);                              /* LDA #$3B */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDA(byte_14);                            /* LDA $14 */
    STA(ABSY(byte_77E));                     /* STA $77E,Y */
    INC(byte_16);                            /* INC $16 */
    INC(byte_16);                            /* INC $16 */
    LDY(byte_16);                            /* LDY $16 */
    DEC(byte_15);                            /* DEC $15 */
    BNE(loc_1A470);                          /* BNE loc_1A470 */
    BEQ(loc_1A4D5);                          /* BEQ loc_1A4D5 */

loc_1A4A9:                                   /* $A4A9 */
    LDA(byte_542);                           /* LDA $542 */
    CMP(ABSX(byte_542));                     /* CMP $542,X */
    LDYI(0);                                 /* LDY #0 */
    BCC(loc_1A4B5);                          /* BCC loc_1A4B5 */
    LDYI(3);                                 /* LDY #3 */

loc_1A4B5:                                   /* $A4B5 */
    LDA(ABSY(0xA4F9));                       /* LDA $A4F9,Y */
    STA(byte_14);                            /* STA $14 */
    INY();                                   /* INY */
    STY(byte_16);                            /* STY $16 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_1A4D5);                          /* BCS loc_1A4D5 */
    LDA(ABSX(byte_6FC));                     /* LDA $6FC,X */
    CMPI(0x14);                              /* CMP #$14 */
    BNE(loc_1A4CC);                          /* BNE loc_1A4CC */
    sub_1981D();                             /* JSR sub_1981D */

loc_1A4CC:                                   /* $A4CC */
    spawn_projectile();                      /* JSR spawn_projectile */
    LDY(byte_16);                            /* LDY $16 */
    DEC(byte_15);                            /* DEC $15 */
    BNE(loc_1A4B5);                          /* BNE loc_1A4B5 */

loc_1A4D5:                                   /* $A4D5 */
    process_enemy_flags_sprites();           /* JSR process_enemy_flags_sprites */
    sub_181F6();                             /* JSR sub_181F6 */
    decrease_enemy_health();                 /* JSR decrease_enemy_health */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $A509  enemy_type_46_47 - the mirrored diving pair
 *
 *  Original: src/bank6.asm:6921-7038 (bank 6)
 *      LDA $528,X / BMI loc_1A56E / ORA #$80 / STA $528,X / SEC / SBC #$AE /
 *      ASL / ASL / TAY / LDA unk_1A601,Y / STA $590,X / LDA #$30 /
 *      STA $6E2,X / STA $6FC,X / LDA $A602,Y / STA $5AA,X / LDA $A603,Y /
 *      STA $716,X / LDA $A604,Y / STA $67A,X / LDA #$17 / STA $576,X /
 *      LDA #1 / STA $660,X / STA $612,X / STA $730,X / LDA #$50 /
 *      STA $5F8,X / LDA #$E0 / STA $55C,X / JSR enemy_prng / LSR /
 *      BCS loc_1A56E / LDA #$20 / STA $55C,X / LDA #$FF / STA $730,X /
 *      LDA $67A,X / EOR #$FF / SEC / SBC #7 / STA $67A,X
 *  loc_1A56E:
 *      LDA $5AA,X / BPL loc_1A5AE / CMP #$80 / BNE loc_1A59A /
 *      LDA $542,X / CMP $542 / BCC loc_1A5FE / LDA #$18 / STA $576,X /
 *      JSR check_enemy_overflow / BCS loc_1A593 / LDA $67A,X / STA $14 /
 *      LDA #$15 / JSR spawn_projectile
 *  loc_1A593:
 *      LDA #$C0 / STA $5AA,X / BNE loc_1A5FE
 *  loc_1A59A:
 *      LDA $542,X / CMP $542 / BCS loc_1A5FE / LDA #$80 / STA $5AA,X /
 *      LDA #$17 / STA $576,X / BNE loc_1A5FE
 *  loc_1A5AE:
 *      BNE loc_1A5D6 / DEC $6FC,X / BNE loc_1A5FE / LDA #$18 / STA $576,X /
 *      LDA #0 / STA $660,X / LDA $6E2,X / STA $6FC,X / LDA #$40 /
 *      STA $5AA,X / LDA $67A,X / STA $764,X / LDA $716,X / STA $694,X
 *  loc_1A5D6:
 *      JSR check_enemy_overflow / BCS loc_1A5E5 / LDA $764,X / STA $14 /
 *      LDA #$15 / JSR spawn_projectile
 *  loc_1A5E5:
 *      LDA $764,X / CLC / ADC $730,X / STA $764,X / DEC $694,X /
 *      BNE loc_1A5FE / LDA #1 / STA $660,X / LDA #0 / STA $5AA,X
 *  loc_1A5FE:
 *      JMP sub_197B4
 * ------------------------------------------------------------------------ */
void enemy_type_46_47(void)                  /* $A509 */
{
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    BMI(loc_1A56E);                          /* BMI loc_1A56E */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    SEC();                                   /* SEC */
    SBCI(0xAE);                              /* SBC #$AE */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_1A601));                    /* LDA unk_1A601,Y */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDAI(0x30);                              /* LDA #$30 */
    STA(ABSX(byte_6E2));                     /* STA $6E2,X */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    LDA(ABSY(0xA602));                       /* LDA $A602,Y */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LDA(ABSY(0xA603));                       /* LDA $A603,Y */
    STA(ABSX(byte_716));                     /* STA $716,X */
    LDA(ABSY(0xA604));                       /* LDA $A604,Y */
    STA(ABSX(unk_67A));                      /* STA $67A,X */
    LDAI(0x17);                              /* LDA #$17 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    STA(ABSX(byte_612));                     /* STA $612,X */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDAI(0x50);                              /* LDA #$50 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDAI(0xE0);                              /* LDA #$E0 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    ENEMY_PRNG_SP(0xF5);                            /* JSR enemy_prng */
    LSR_A();                                 /* LSR */
    BCS(loc_1A56E);                          /* BCS loc_1A56E */
    LDAI(0x20);                              /* LDA #$20 */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSX(0x0730));                       /* STA $730,X */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    EORI(0xFF);                              /* EOR #$FF */
    SEC();                                   /* SEC */
    SBCI(0x07);                              /* SBC #7 */
    STA(ABSX(unk_67A));                      /* STA $67A,X */

loc_1A56E:                                   /* $A56E */
    LDA(ABSX(byte_5AA));                     /* LDA $5AA,X */
    BPL(loc_1A5AE);                          /* BPL loc_1A5AE */
    CMPI(0x80);                              /* CMP #$80 */
    BNE(loc_1A59A);                          /* BNE loc_1A59A */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMP(byte_542);                           /* CMP $542 */
    BCC(loc_1A5FE);                          /* BCC loc_1A5FE */
    LDAI(0x18);                              /* LDA #$18 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_1A593);                          /* BCS loc_1A593 */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x15);                              /* LDA #$15 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_1A593:                                   /* $A593 */
    LDAI(0xC0);                              /* LDA #$C0 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    BNE(loc_1A5FE);                          /* BNE loc_1A5FE */

loc_1A59A:                                   /* $A59A */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMP(byte_542);                           /* CMP $542 */
    BCS(loc_1A5FE);                          /* BCS loc_1A5FE */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LDAI(0x17);                              /* LDA #$17 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    BNE(loc_1A5FE);                          /* BNE loc_1A5FE */

loc_1A5AE:                                   /* $A5AE */
    BNE(loc_1A5D6);                          /* BNE loc_1A5D6 */
    DEC(ABSX(byte_6FC));                     /* DEC $6FC,X */
    BNE(loc_1A5FE);                          /* BNE loc_1A5FE */
    LDAI(0x18);                              /* LDA #$18 */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDA(ABSX(byte_6E2));                     /* LDA $6E2,X */
    STA(ABSX(byte_6FC));                     /* STA $6FC,X */
    LDAI(0x40);                              /* LDA #$40 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    STA(ABSX(byte_764));                     /* STA $764,X */
    LDA(ABSX(byte_716));                     /* LDA $716,X */
    STA(ABSX(0x0694));                       /* STA $694,X */

loc_1A5D6:                                   /* $A5D6 */
    check_enemy_overflow();                  /* JSR check_enemy_overflow */
    BCS(loc_1A5E5);                          /* BCS loc_1A5E5 */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    STA(byte_14);                            /* STA $14 */
    LDAI(0x15);                              /* LDA #$15 */
    spawn_projectile();                      /* JSR spawn_projectile */

loc_1A5E5:                                   /* $A5E5 */
    LDA(ABSX(byte_764));                     /* LDA $764,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0730));                       /* ADC $730,X */
    STA(ABSX(byte_764));                     /* STA $764,X */
    DEC(ABSX(0x0694));                       /* DEC $694,X */
    BNE(loc_1A5FE);                          /* BNE loc_1A5FE */
    LDAI(0x01);                              /* LDA #1 */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_5AA));                     /* STA $5AA,X */

loc_1A5FE:                                   /* $A5FE */
    sub_197B4();                             /* JMP sub_197B4 */
    RTS();
}
