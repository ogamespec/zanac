/* bank6/8024.c - bank 6, $8024-$85CE: the slope/speed calculation, the
 * shot/enemy collision detection family, the object movement and flag helpers
 * and the sprite buffer helpers.
 *
 * Ported from src/bank6.asm lines 129-1233 (addresses from Build/bank6.lst).
 *
 * Deviations from a plain line-by-line translation (PORTING.md sections 4c/5):
 *
 *   - loc_1820C ($820C) is the shared "object was hit" tail that lives inside
 *     sub_18204 but is branched to from sub_181E4, sub_181ED and sub_181F6.  It
 *     is reproduced verbatim in each of those procedures under a local C label
 *     of the same name.
 *   - loc_1828E/locret_1828F (inside collision_detect_all_shots) and
 *     locret_18367 (inside sub_18347) are shared tail labels reached from other
 *     procedures; a local C label with the same name reproduces them where they
 *     are used.
 *   - sub_181ED ($81F0) does `JSR loc_18244`, the second entry point inside
 *     collision_detect_special_shot_and_zanac ($8230).  $8244 is not a
 *     procedure entry, so it has no zanac_call_at() target; the block is
 *     reproduced in place and its continuation address $81F3 gets the local
 *     label loc_181F3.
 *   - enemy_erase_and_continue ($82FA) is four PLAs plus a fall-through into
 *     enemy_erase: the PLAs pop the return addresses of the two JSR levels that
 *     lead to process_y_speed/sub_18347, so that enemy_erase's RTS returns to
 *     the object processing cycle, skipping the rest of
 *     process_enemy_flags_sprites.  A C call keeps no return address on the
 *     emulated 6502 stack, so the unwind is not reproduced (section 4c): the
 *     PLAs are dropped and process_y_speed/sub_18347 return one level less far
 *     than the original.
 *   - A branch to a *procedure* entry (enemy_erase_and_continue,
 *     add_enemy_meta_sprite) becomes a goto to a local label of the same name
 *     that calls the procedure and then returns.
 *   - A shared label reached only by fall-through inside its own procedure
 *     (loc_1820C in sub_18204, locret_18203 in sub_181F6, locret_18367 in
 *     sub_18347) is kept as a comment rather than as an unused C label.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $8024  calculate_slope_and_speed - slope -> speed, the common pair
 *
 *  Original: src/bank6.asm:129-133
 *      JSR calculate_slope
 *      JMP set_speed_by_slope
 * ------------------------------------------------------------------------ */
void calculate_slope_and_speed(void)         /* $8024 */
{
    calculate_slope();                       /* JSR calculate_slope */
    set_speed_by_slope();                    /* JMP set_speed_by_slope */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $802A  calculate_slope - slope from the object's X/Y to Zanac's X/Y
 *
 *  Original: src/bank6.asm:140-210
 *      STY $78 / LDA #0 / STA $44 / LDA $542 / SEC / SBC $542,X / STA $42 /
 *      BCS loc_18049 / ... / loc_1807C: LDY $43 / LDA $42 / JSR slope_div /
 *      LDY #3 / loc_18085: CMP $8120,Y / BCC loc_1808D / DEY / BNE loc_18085 /
 *      loc_1808D: TYA / ORA $44 / TAY / LDA $8124,Y / LDY $78 / RTS
 *
 *  A: slope (0..15).  $44 accumulates the quadrant bits (4/8/$10).
 * ------------------------------------------------------------------------ */
void calculate_slope(void)                   /* $802A */
{
    STY(0x0078);                             /* STY $78 */
    LDAI(0x00);                              /* LDA #0 */
    STA(unk_44);                             /* STA $44 */
    LDA(byte_542);                           /* LDA $542 */
    SEC();                                   /* SEC */
    SBC(ABSX(byte_542));                     /* SBC $542,X */
    STA(unk_42);                             /* STA $42 */
    BCS(loc_18049);                          /* BCS loc_18049 */
    LDA(unk_44);                             /* LDA $44 */
    ORAI(0x04);                              /* ORA #4 */
    STA(unk_44);                             /* STA $44 */
    LDA(unk_42);                             /* LDA $42 */
    EORI(0xFF);                              /* EOR #$FF */
    ADCI(0x01);                              /* ADC #1 */
    STA(unk_42);                             /* STA $42 */

loc_18049:                                   /* $8049 */
    BNE(loc_1804D);                          /* BNE loc_1804D */
    INC(unk_42);                             /* INC $42 */

loc_1804D:                                   /* $804D */
    LDA(byte_55C);                           /* LDA $55C */
    SEC();                                   /* SEC */
    SBC(ABSX(byte_55C));                     /* SBC $55C,X */
    STA(0x0043);                             /* STA $43 */
    BCS(loc_18066);                          /* BCS loc_18066 */
    LDA(unk_44);                             /* LDA $44 */
    ORAI(0x08);                              /* ORA #8 */
    STA(unk_44);                             /* STA $44 */
    LDA(0x0043);                             /* LDA $43 */
    EORI(0xFF);                              /* EOR #$FF */
    ADCI(0x01);                              /* ADC #1 */
    STA(0x0043);                             /* STA $43 */

loc_18066:                                   /* $8066 */
    BNE(loc_1806A);                          /* BNE loc_1806A */
    INC(0x0043);                             /* INC $43 */

loc_1806A:                                   /* $806A */
    LDA(0x0043);                             /* LDA $43 */
    CMP(unk_42);                             /* CMP $42 */
    BCC(loc_1807C);                          /* BCC loc_1807C */
    LDY(unk_42);                             /* LDY $42 */
    STY(0x0043);                             /* STY $43 */
    STA(unk_42);                             /* STA $42 */
    LDA(unk_44);                             /* LDA $44 */
    ORAI(0x10);                              /* ORA #$10 */
    STA(unk_44);                             /* STA $44 */

loc_1807C:                                   /* $807C */
    LDY(0x0043);                             /* LDY $43 */
    LDA(unk_42);                             /* LDA $42 */
    slope_div();                             /* JSR slope_div */
    LDYI(3);                                 /* LDY #3 */

loc_18085:                                   /* $8085 */
    CMP(ABSY(0x8120));                       /* CMP $8120,Y */
    BCC(loc_1808D);                          /* BCC loc_1808D */
    DEY();                                   /* DEY */
    BNE(loc_18085);                          /* BNE loc_18085 */

loc_1808D:                                   /* $808D */
    TYA();                                   /* TYA */
    ORA(unk_44);                             /* ORA $44 */
    TAY();                                   /* TAY */
    LDA(ABSY(0x8124));                       /* LDA $8124,Y */
    LDY(0x0078);                             /* LDY $78 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8097  set_speed_by_slope - turn the slope into the object's Y/X speeds
 *
 *  Original: src/bank6.asm:217-291
 *      AND #$F / STY $65 / ASL / ASL / TAY /
 *      LDA tab_18144,Y / STA $12 / ... tab_18144+3,Y / STA $11 /
 *      LDA $74A,X / BPL loc_180CF / (six ASL/ROL pairs, doubled twice when
 *      $74A,X has bit 7 / bit 6 set) / loc_180E3: LDA $74A,X / AND #$3F /
 *      BEQ loc_1811E / TAY / clear $5F8/$612/$62C/$646,X /
 *      loc_180F9: add the 16-bit speeds, DEY, BNE / loc_1811E: LDY $65 / RTS
 * ------------------------------------------------------------------------ */
void set_speed_by_slope(void)                /* $8097 */
{
    ANDI(0x0F);                              /* AND #$F */
    STY(byte_65);                            /* STY $65 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_18144));                    /* LDA tab_18144,Y */
    STA(byte_12);                            /* STA $12 */
    LDA(ABSY(tab_18144 + 1));                /* LDA tab_18144+1,Y */
    STA(byte_10);                            /* STA $10 */
    LDA(ABSY(tab_18144 + 2));                /* LDA tab_18144+2,Y */
    STA(byte_13);                            /* STA $13 */
    LDA(ABSY(tab_18144 + 3));                /* LDA tab_18144+3,Y */
    STA(byte_11);                            /* STA $11 */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    BPL(loc_180CF);                          /* BPL loc_180CF */
    ASL(byte_12);                            /* ASL $12 */
    ROL(byte_10);                            /* ROL $10 */
    ASL(byte_12);                            /* ASL $12 */
    ROL(byte_10);                            /* ROL $10 */
    ASL(byte_12);                            /* ASL $12 */
    ROL(byte_10);                            /* ROL $10 */
    ASL(byte_13);                            /* ASL $13 */
    ROL(byte_11);                            /* ROL $11 */
    ASL(byte_13);                            /* ASL $13 */
    ROL(byte_11);                            /* ROL $11 */
    ASL(byte_13);                            /* ASL $13 */
    ROL(byte_11);                            /* ROL $11 */

loc_180CF:                                   /* $80CF */
    ANDI(0x40);                              /* AND #$40 */
    BEQ(loc_180E3);                          /* BEQ loc_180E3 */
    ASL(byte_12);                            /* ASL $12 */
    ROL(byte_10);                            /* ROL $10 */
    ASL(byte_12);                            /* ASL $12 */
    ROL(byte_10);                            /* ROL $10 */
    ASL(byte_13);                            /* ASL $13 */
    ROL(byte_11);                            /* ROL $11 */
    ASL(byte_13);                            /* ASL $13 */
    ROL(byte_11);                            /* ROL $11 */

loc_180E3:                                   /* $80E3 */
    LDA(ABSX(0x074A));                       /* LDA $74A,X */
    ANDI(0x3F);                              /* AND #$3F */
    BEQ(loc_1811E);                          /* BEQ loc_1811E */
    TAY();                                   /* TAY */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    STA(ABSX(byte_612));                     /* STA $612,X */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    STA(ABSX(byte_646));                     /* STA $646,X */

loc_180F9:                                   /* $80F9 */
    LDA(byte_12);                            /* LDA $12 */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_5F8));                     /* ADC $5F8,X */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    LDA(byte_10);                            /* LDA $10 */
    ADC(ABSX(byte_612));                     /* ADC $612,X */
    STA(ABSX(byte_612));                     /* STA $612,X */
    LDA(byte_13);                            /* LDA $13 */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_62C));                     /* ADC $62C,X */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    LDA(byte_11);                            /* LDA $11 */
    ADC(ABSX(byte_646));                     /* ADC $646,X */
    STA(ABSX(byte_646));                     /* STA $646,X */
    DEY();                                   /* DEY */
    BNE(loc_180F9);                          /* BNE loc_180F9 */

loc_1811E:                                   /* $811E */
    LDY(byte_65);                            /* LDY $65 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $818C  slope_div - A/Y ratio, 8-bit restoring division
 *
 *  Original: src/bank6.asm:352-373
 *      STA byte_10 / LDA #0 / STA byte_11 / TYA / LDY #8
 *  loc_18195:
 *      ASL byte_11 / ROL / CMP byte_10 / BCC loc_181A0 / SBC byte_10 /
 *      INC byte_11
 *  loc_181A0:
 *      DEY / BNE loc_18195 / TAY / LDA byte_11 / RTS
 * ------------------------------------------------------------------------ */
void slope_div(void)                         /* $818C */
{
    STA(byte_10);                            /* STA byte_10 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_11);                            /* STA byte_11 */
    TYA();                                   /* TYA */
    LDYI(8);                                 /* LDY #8 */

loc_18195:                                   /* $8195 */
    ASL(byte_11);                            /* ASL byte_11 */
    ROL_A();                                 /* ROL */
    CMP(byte_10);                            /* CMP byte_10 */
    BCC(loc_181A0);                          /* BCC loc_181A0 */
    SBC(byte_10);                            /* SBC byte_10 */
    INC(byte_11);                            /* INC byte_11 */

loc_181A0:                                   /* $81A0 */
    DEY();                                   /* DEY */
    BNE(loc_18195);                          /* BNE loc_18195 */
    TAY();                                   /* TAY */
    LDA(byte_11);                            /* LDA byte_11 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $81A7  sub_181A7 - build the collision box of slot X
 *
 *  Original: src/bank6.asm:379-416
 *      LDA $576,X / TAY / LDA $542,X / CLC / ADC $C0FF,Y / BCC loc_181B6 /
 *      LDA #$FF / loc_181B6: STA $7CC,X / ... same with SBC -> $7B2,X, then
 *      $55C,X +/- unk_C152,Y -> $798,X / $77E,X / RTS
 * ------------------------------------------------------------------------ */
void sub_181A7(void)                         /* $81A7 */
{
    LDA(ABSX(byte_576));                     /* LDA $576,X */
    TAY();                                   /* TAY */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CLC();                                   /* CLC */
    ADC(ABSY(0xC0FF));                       /* ADC $C0FF,Y */
    BCC(loc_181B6);                          /* BCC loc_181B6 */
    LDAI(0xFF);                              /* LDA #$FF */

loc_181B6:                                   /* $81B6 */
    STA(ABSX(byte_7CC));                     /* STA $7CC,X */
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    SEC();                                   /* SEC */
    SBC(ABSY(0xC0FF));                       /* SBC $C0FF,Y */
    BCS(loc_181C4);                          /* BCS loc_181C4 */
    LDAI(0x00);                              /* LDA #0 */

loc_181C4:                                   /* $81C4 */
    STA(ABSX(byte_7B2));                     /* STA $7B2,X */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    CLC();                                   /* CLC */
    ADC(ABSY(unk_C152));                     /* ADC unk_C152,Y */
    BCC(loc_181D2);                          /* BCC loc_181D2 */
    LDAI(0xFF);                              /* LDA #$FF */

loc_181D2:                                   /* $81D2 */
    STA(ABSX(byte_798));                     /* STA $798,X */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    SEC();                                   /* SEC */
    SBC(ABSY(unk_C152));                     /* SBC unk_C152,Y */
    BCS(loc_181E0);                          /* BCS loc_181E0 */
    LDAI(0x00);                              /* LDA #0 */

loc_181E0:                                   /* $81E0 */
    STA(ABSX(byte_77E));                     /* STA $77E,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $81E4  sub_181E4 - collision of slot X with the special shot and Zanac
 *
 *  Original: src/bank6.asm:422-428
 *      JSR sub_181A7
 *      JSR collision_detect_special_shot_and_zanac
 *      BCS loc_1820C
 *      RTS
 *
 *  loc_1820C ($820C) lives in sub_18204 and is reproduced here.
 * ------------------------------------------------------------------------ */
void sub_181E4(void)                         /* $81E4 */
{
    sub_181A7();                             /* JSR sub_181A7 */
    collision_detect_special_shot_and_zanac(); /* JSR collision_detect_special_shot_and_zanac */
    BCS(loc_1820C);                          /* BCS loc_1820C */
    RTS();                                   /* RTS */

loc_1820C:                                   /* $820C - shared hit tail (see sub_18204) */
    STY(0x008A);                             /* STY $8A */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0089);                             /* STA $89 */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    LDY(0x008A);                             /* LDY $8A */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0088);                             /* STA $88 */
    STA(ABSX(0x07E6));                       /* STA $7E6,X */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    STA(ABSX(byte_528));                     /* STA $528,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $81ED  sub_181ED - collision of slot X, second entry point of the hit code
 *
 *  Original: src/bank6.asm:434-440
 *      JSR sub_181A7
 *      JSR loc_18244           ; the second entry point inside $8230
 *      BCS loc_1820C
 *      RTS
 *
 *  $8244 is not a procedure entry, so it has no zanac_call_at() target: its
 *  body is reproduced here.  Both of its exits return to $81F3 (the
 *  instruction after the JSR), reproduced with the local label loc_181F3.
 *  loc_1820C ($820C) lives in sub_18204 and is reproduced here.
 * ------------------------------------------------------------------------ */
void sub_181ED(void)                         /* $81ED */
{
    sub_181A7();                             /* JSR sub_181A7 */
    /* loc_18244 ($8244) - reproduced from collision_detect_special_shot_and_zanac */
    LDA(byte_528);                           /* LDA $528 */
    CMPI(0x81);                              /* CMP #$81 */
    BNE(loc_1828E);                          /* BNE loc_1828E */
    LDYI(0);                                 /* LDY #0 */
    collision_detect_single();               /* JMP collision_detect_single */
    goto loc_181F3;                          /* RTS of $82B0 returns to $81F3 */

loc_1828E:                                   /* $828E - shared tail (see collision_detect_all_shots) */
    CLC();                                   /* CLC */

loc_181F3:                                   /* $81F3 - after the JSR loc_18244 */
    BCS(loc_1820C);                          /* BCS loc_1820C */
    RTS();                                   /* RTS */

loc_1820C:                                   /* $820C - shared hit tail (see sub_18204) */
    STY(0x008A);                             /* STY $8A */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0089);                             /* STA $89 */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    LDY(0x008A);                             /* LDY $8A */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0088);                             /* STA $88 */
    STA(ABSX(0x07E6));                       /* STA $7E6,X */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    STA(ABSX(byte_528));                     /* STA $528,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $81F6  sub_181F6 - collision of slot X with every shot, then with Zanac
 *
 *  Original: src/bank6.asm:446-456
 *      JSR sub_181A7
 *      JSR collision_detect_special_shot_and_zanac
 *      BCS loc_1820C
 *      JSR collision_detect_all_shots
 *      BCS loc_1820C
 *  locret_18203:
 *      RTS
 *
 *  locret_18203 ($8203) is the shared RTS that sub_18204 branches to, and
 *  loc_1820C ($820C) is the shared hit tail of sub_18204; both are reproduced
 *  here.
 * ------------------------------------------------------------------------ */
void sub_181F6(void)                         /* $81F6 */
{
    sub_181A7();                             /* JSR sub_181A7 */
    collision_detect_special_shot_and_zanac(); /* JSR collision_detect_special_shot_and_zanac */
    BCS(loc_1820C);                          /* BCS loc_1820C */
    collision_detect_all_shots();            /* JSR collision_detect_all_shots */
    BCS(loc_1820C);                          /* BCS loc_1820C */
    /* locret_18203 ($8203) - shared RTS (sub_18204 branches here) */
    RTS();                                   /* RTS */

loc_1820C:                                   /* $820C - shared hit tail (see sub_18204) */
    STY(0x008A);                             /* STY $8A */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0089);                             /* STA $89 */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    LDY(0x008A);                             /* LDY $8A */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0088);                             /* STA $88 */
    STA(ABSX(0x07E6));                       /* STA $7E6,X */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    STA(ABSX(byte_528));                     /* STA $528,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8204  sub_18204 - collision of slot X with every shot
 *
 *  Original: src/bank6.asm:462-486
 *      JSR sub_181A7
 *      JSR collision_detect_all_shots
 *      BCC locret_18203
 *  loc_1820C:                          ; STY $8A / LDA $528,Y / AND #$7F /
 *      STA $89 / TAY / LDA $86CF,Y / LDY $8A / STA $528,Y /
 *      LDA $528,X / AND #$7F / STA $88 / STA $7E6,X / TAY /
 *      LDA $86CF,Y / STA $528,X / RTS
 *  locret_18203:
 *      RTS
 *
 *  loc_1820C and locret_18203 are the shared hit tail / shared RTS reproduced
 *  by sub_181E4, sub_181ED and sub_181F6.
 * ------------------------------------------------------------------------ */
void sub_18204(void)                         /* $8204 */
{
    sub_181A7();                             /* JSR sub_181A7 */
    collision_detect_all_shots();            /* JSR collision_detect_all_shots */
    BCC(locret_18203);                       /* BCC locret_18203 */
    /* loc_1820C ($820C) - shared hit tail (reproduced by sub_181E4/$81ED/$81F6) */
    STY(0x008A);                             /* STY $8A */
    LDA(ABSY(byte_528));                     /* LDA $528,Y */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0089);                             /* STA $89 */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    LDY(0x008A);                             /* LDY $8A */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    LDA(ABSX(byte_528));                     /* LDA $528,X */
    ANDI(0x7F);                              /* AND #$7F */
    STA(0x0088);                             /* STA $88 */
    STA(ABSX(0x07E6));                       /* STA $7E6,X */
    TAY();                                   /* TAY */
    LDA(ABSY(tab_186CF));                    /* LDA $86CF,Y */
    STA(ABSX(byte_528));                     /* STA $528,X */
    RTS();                                   /* RTS */

locret_18203:                                /* $8203 - shared RTS (sub_181F6 falls into it) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $8230  collision_detect_special_shot_and_zanac - special shot vs Zanac
 *
 *  Original: src/bank6.asm:493-512
 *      LDA $5F / AND #1 / BEQ loc_18244 / LDA $52C / CMP #$83 /
 *      BNE loc_18244 / LDY #4 / JSR collision_detect_single /
 *      BCS locret_1828F /
 *  loc_18244:
 *      LDA $528 / CMP #$81 / BNE loc_1828E / LDY #0 /
 *      JMP collision_detect_single
 *
 *  loc_18244 is also called from sub_181ED; loc_1828E/locret_1828F live in
 *  collision_detect_all_shots and are reproduced here.
 * ------------------------------------------------------------------------ */
void collision_detect_special_shot_and_zanac(void) /* $8230 */
{
    LDA(0x005F);                             /* LDA $5F */
    ANDI(0x01);                              /* AND #1 */
    BEQ(loc_18244);                          /* BEQ loc_18244 */
    LDA(byte_52C);                           /* LDA $52C */
    CMPI(0x83);                              /* CMP #$83 */
    BNE(loc_18244);                          /* BNE loc_18244 */
    LDYI(4);                                 /* LDY #4 */
    collision_detect_single();               /* JSR collision_detect_single */
    BCS(locret_1828F);                       /* BCS locret_1828F */

loc_18244:                                   /* $8244 */
    LDA(byte_528);                           /* LDA $528 */
    CMPI(0x81);                              /* CMP #$81 */
    BNE(loc_1828E);                          /* BNE loc_1828E */
    LDYI(0);                                 /* LDY #0 */
    collision_detect_single();               /* JMP collision_detect_single */
    RTS();                                   /* RTS (tail call) */

loc_1828E:                                   /* $828E - shared tail (see collision_detect_all_shots) */
    CLC();                                   /* CLC */

locret_1828F:                                /* $828F - shared RTS */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $8250  collision_detect_all_shots - the four shot slots against slot X
 *
 *  Original: src/bank6.asm:518-561
 *      LDA $529 / CMP #$82 / BNE loc_1825E / LDY #1 /
 *      JSR collision_detect_single / BCS locret_1828F /
 *  loc_1825E:  ... $52A, LDY #2 ... / loc_1826C: ... $52B, LDY #3 ... /
 *  loc_1827A: LDA $5F / AND #2 / BEQ loc_1828E / LDA $52C / CMP #$83 /
 *      BNE loc_1828E / LDY #4 / JSR collision_detect_single /
 *      BCS locret_1828F /
 *  loc_1828E: CLC
 *  locret_1828F: RTS
 * ------------------------------------------------------------------------ */
void collision_detect_all_shots(void)        /* $8250 */
{
    LDA(0x0529);                             /* LDA $529 */
    CMPI(0x82);                              /* CMP #$82 */
    BNE(loc_1825E);                          /* BNE loc_1825E */
    LDYI(1);                                 /* LDY #1 */
    collision_detect_single();               /* JSR collision_detect_single */
    BCS(locret_1828F);                       /* BCS locret_1828F */

loc_1825E:                                   /* $825E */
    LDA(0x052A);                             /* LDA $52A */
    CMPI(0x82);                              /* CMP #$82 */
    BNE(loc_1826C);                          /* BNE loc_1826C */
    LDYI(2);                                 /* LDY #2 */
    collision_detect_single();               /* JSR collision_detect_single */
    BCS(locret_1828F);                       /* BCS locret_1828F */

loc_1826C:                                   /* $826C */
    LDA(0x052B);                             /* LDA $52B */
    CMPI(0x82);                              /* CMP #$82 */
    BNE(loc_1827A);                          /* BNE loc_1827A */
    LDYI(3);                                 /* LDY #3 */
    collision_detect_single();               /* JSR collision_detect_single */
    BCS(locret_1828F);                       /* BCS locret_1828F */

loc_1827A:                                   /* $827A */
    LDA(0x005F);                             /* LDA $5F */
    ANDI(0x02);                              /* AND #2 */
    BEQ(loc_1828E);                          /* BEQ loc_1828E */
    LDA(byte_52C);                           /* LDA $52C */
    CMPI(0x83);                              /* CMP #$83 */
    BNE(loc_1828E);                          /* BNE loc_1828E */
    LDYI(4);                                 /* LDY #4 */
    collision_detect_single();               /* JSR collision_detect_single */
    BCS(locret_1828F);                       /* BCS locret_1828F */

loc_1828E:                                   /* $828E */
    CLC();                                   /* CLC */

locret_1828F:                                /* $828F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $8290  collision_detect_single - box of slot X against box of slot Y
 *
 *  Original: src/bank6.asm:567-582
 *      LDA $798,Y / CMP $77E,X / BCC locret_1828F / LDA $798,X / CMP $77E,Y /
 *      BCC locret_1828F / LDA $7CC,Y / CMP $7B2,X / BCC locret_1828F /
 *      LDA $7CC,X / CMP $7B2,Y / BCC locret_1828F / RTS
 *
 *  Carry set on return means the boxes overlap.  locret_1828F ($828F) lives in
 *  collision_detect_all_shots and is reproduced here.
 * ------------------------------------------------------------------------ */
void collision_detect_single(void)           /* $8290 */
{
    LDA(ABSY(byte_798));                     /* LDA $798,Y */
    CMP(ABSX(byte_77E));                     /* CMP $77E,X */
    BCC(locret_1828F);                       /* BCC locret_1828F */
    LDA(ABSX(byte_798));                     /* LDA $798,X */
    CMP(ABSY(byte_77E));                     /* CMP $77E,Y */
    BCC(locret_1828F);                       /* BCC locret_1828F */
    LDA(ABSY(byte_7CC));                     /* LDA $7CC,Y */
    CMP(ABSX(byte_7B2));                     /* CMP $7B2,X */
    BCC(locret_1828F);                       /* BCC locret_1828F */
    LDA(ABSX(byte_7CC));                     /* LDA $7CC,X */
    CMP(ABSY(byte_7B2));                     /* CMP $7B2,Y */
    BCC(locret_1828F);                       /* BCC locret_1828F */
    RTS();                                   /* RTS */

locret_1828F:                                /* $828F - shared RTS (in collision_detect_all_shots) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $82B1  process_enemy_flags_sprites - apply the speed flags of the object
 *
 *  Original: src/bank6.asm:589-613
 *      LDA $660,X / AND #8 / BEQ loc_182BB / JSR sub_18368
 *  loc_182BB:
 *      LDA $660,X / AND #$10 / BEQ loc_182C5 / JSR sub_18392
 *  loc_182C5:
 *      LDA $660,X / AND #1 / BEQ loc_182CF / JSR process_y_speed
 *  loc_182CF:
 *      LDA $660,X / AND #2 / BEQ add_enemy_meta_sprite / JSR sub_18347
 *  (no RTS: execution continues into add_enemy_meta_sprite at $82D9)
 * ------------------------------------------------------------------------ */
void process_enemy_flags_sprites(void)       /* $82B1 */
{
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ANDI(0x08);                              /* AND #8 */
    BEQ(loc_182BB);                          /* BEQ loc_182BB */
    sub_18368();                             /* JSR sub_18368 */

loc_182BB:                                   /* $82BB */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_182C5);                          /* BEQ loc_182C5 */
    sub_18392();                             /* JSR sub_18392 */

loc_182C5:                                   /* $82C5 */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ANDI(0x01);                              /* AND #1 */
    BEQ(loc_182CF);                          /* BEQ loc_182CF */
    process_y_speed();                       /* JSR process_y_speed */

loc_182CF:                                   /* $82CF */
    LDA(ABSX(byte_660));                     /* LDA $660,X */
    ANDI(0x02);                              /* AND #2 */
    BEQ(add_enemy_meta_sprite);              /* BEQ add_enemy_meta_sprite */
    sub_18347();                             /* JSR sub_18347 */

add_enemy_meta_sprite:                       /* $82D9 - the BEQ above and the fall-through of
                                              * the JSR sub_18347 at $82D6 both land here */
    add_enemy_meta_sprite();                 /* falls through into $82D9 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $82D9  add_enemy_meta_sprite - draw the meta-sprite of the object
 *
 *  Original: src/bank6.asm:619-642
 *      LDA $576,X / BEQ locret_182F9 / TAY / LDA $C1A5,Y / STA $18 /
 *      LDA $C1F8,Y / STA $19 / LDY #0 / LDA ($18),Y / BEQ locret_182F9 /
 *      STA $14 / INY /
 *  loc_182F2:
 *      JSR add_next_sprite / DEC $14 / BNE loc_182F2
 *  locret_182F9:
 *      RTS
 * ------------------------------------------------------------------------ */
void add_enemy_meta_sprite(void)             /* $82D9 */
{
    LDA(ABSX(byte_576));                     /* LDA $576,X */
    BEQ(locret_182F9);                       /* BEQ locret_182F9 */
    TAY();                                   /* TAY */
    LDA(ABSY(0xC1A5));                       /* LDA $C1A5,Y (tab_C1A6 - 1) */
    STA(byte_18);                            /* STA $18 */
    LDA(ABSY(0xC1F8));                       /* LDA $C1F8,Y (tab_C1F9 - 1) */
    STA(byte_19);                            /* STA $19 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    BEQ(locret_182F9);                       /* BEQ locret_182F9 */
    STA(byte_14);                            /* STA $14 */
    INY();                                   /* INY */

loc_182F2:                                   /* $82F2 */
    add_next_sprite();                       /* JSR add_next_sprite */
    DEC(byte_14);                            /* DEC $14 */
    BNE(loc_182F2);                          /* BNE loc_182F2 */

locret_182F9:                                /* $82F9 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $82FA  enemy_erase_and_continue - release the slot and leave the handler
 *
 *  Original: src/bank6.asm:649-655
 *      PLA
 *      PLA
 *      PLA
 *      PLA
 *  (no RTS: execution continues into enemy_erase at $82FE)
 *
 *  The four PLAs pop the return addresses of the JSR that entered
 *  process_y_speed/sub_18347 and of the JSR that entered
 *  process_enemy_flags_sprites, so the RTS at the end of enemy_erase returns
 *  straight to the object processing cycle, skipping the rest of
 *  process_enemy_flags_sprites.  C calls keep no return address on the emulated
 *  6502 stack, so the unwind cannot be reproduced (PORTING.md section 4c); the
 *  PLAs are dropped and the erase + return is kept.  See the file header.
 * ------------------------------------------------------------------------ */
void enemy_erase_and_continue(void)          /* $82FA */
{
    /* PLA / PLA / PLA / PLA: the four pops discard this call's frame and the
     * caller's, so enemy_erase's RTS resumes the dispatcher's slot loop
     * ($8807) and the calling handler is abandoned - the port's longjmp
     * (see zanac_ram.h). */
    enemy_erase();                           /* falls through into $82FE */
    ZANAC_UNWIND();                          /* the RTS of enemy_erase */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $82FE  enemy_erase - clear every per-slot variable of slot X
 *
 *  Original: src/bank6.asm:662-677
 *      LDA #0
 *      STA $528,X / $542,X / $5C4,X / $5DE,X / $5F8,X / $62C,X / $612,X /
 *      $646,X / $5AA,X / $6E2,X / $6FC,X
 *      RTS
 * ------------------------------------------------------------------------ */
void enemy_erase(void)                       /* $82FE */
{
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(byte_528));                     /* STA byte_528,X */
    STA(ABSX(byte_542));                     /* STA byte_542,X */
    STA(ABSX(byte_5C4));                     /* STA byte_5C4,X */
    STA(ABSX(byte_5DE));                     /* STA byte_5DE,X */
    STA(ABSX(byte_5F8));                     /* STA byte_5F8,X */
    STA(ABSX(byte_62C));                     /* STA byte_62C,X */
    STA(ABSX(byte_612));                     /* STA byte_612,X */
    STA(ABSX(byte_646));                     /* STA byte_646,X */
    STA(ABSX(byte_5AA));                     /* STA byte_5AA,X */
    STA(ABSX(byte_6E2));                     /* STA byte_6E2,X */
    STA(ABSX(byte_6FC));                     /* STA byte_6FC,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8322  process_y_speed - move slot X along Y, erase it when it leaves
 *
 *  Original: src/bank6.asm:683-703
 *      LDA $5F8,X / CLC / ADC $5C4,X / STA $5C4,X / LDA $612,X / BMI loc_1833E /
 *      ADC $542,X / STA $542,X / BCS enemy_erase_and_continue / CMP #$F0 /
 *      BCS enemy_erase_and_continue / RTS
 *  loc_1833E:
 *      ADC $542,X / STA $542,X / BCC enemy_erase_and_continue / RTS
 *
 *  The branches to the enemy_erase_and_continue *procedure* ($82FA) go to a
 *  local label that calls it and returns; see the file header.
 * ------------------------------------------------------------------------ */
void process_y_speed(void)                   /* $8322 */
{
    LDA(ABSX(byte_5F8));                     /* LDA $5F8,X */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_5C4));                     /* ADC $5C4,X */
    STA(ABSX(byte_5C4));                     /* STA $5C4,X */
    LDA(ABSX(byte_612));                     /* LDA $612,X */
    BMI(loc_1833E);                          /* BMI loc_1833E */
    ADC(ABSX(byte_542));                     /* ADC $542,X */
    STA(ABSX(byte_542));                     /* STA $542,X */
    BCS(enemy_erase_and_continue);           /* BCS enemy_erase_and_continue */
    CMPI(0xF0);                              /* CMP #$F0 */
    BCS(enemy_erase_and_continue);           /* BCS enemy_erase_and_continue */
    RTS();                                   /* RTS */

loc_1833E:                                   /* $833E */
    ADC(ABSX(byte_542));                     /* ADC $542,X */
    STA(ABSX(byte_542));                     /* STA $542,X */
    BCC(enemy_erase_and_continue);           /* BCC enemy_erase_and_continue */
    RTS();                                   /* RTS */

enemy_erase_and_continue:                    /* $82FA - entry of the preceding procedure */
    enemy_erase_and_continue();              /* call $82FA, then leave process_y_speed */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $8347  sub_18347 - move slot X along X, erase it when it leaves
 *
 *  Original: src/bank6.asm:709-730
 *      LDA $62C,X / CLC / ADC $5DE,X / STA $5DE,X / LDA $646,X / BMI loc_1835F /
 *      ADC $55C,X / STA $55C,X / BCS enemy_erase_and_continue / RTS
 *  loc_1835F:
 *      ADC $55C,X / STA $55C,X / BCC enemy_erase_and_continue
 *  locret_18367:
 *      RTS
 *
 *  locret_18367 ($8367) is the shared RTS that sub_18368 and sub_18392 branch
 *  to; they reproduce it locally.
 * ------------------------------------------------------------------------ */
void sub_18347(void)                         /* $8347 */
{
    LDA(ABSX(byte_62C));                     /* LDA $62C,X */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_5DE));                     /* ADC $5DE,X */
    STA(ABSX(byte_5DE));                     /* STA $5DE,X */
    LDA(ABSX(byte_646));                     /* LDA $646,X */
    BMI(loc_1835F);                          /* BMI loc_1835F */
    ADC(ABSX(byte_55C));                     /* ADC $55C,X */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    BCS(enemy_erase_and_continue);           /* BCS enemy_erase_and_continue */
    RTS();                                   /* RTS */

loc_1835F:                                   /* $835F */
    ADC(ABSX(byte_55C));                     /* ADC $55C,X */
    STA(ABSX(byte_55C));                     /* STA $55C,X */
    BCC(enemy_erase_and_continue);           /* BCC enemy_erase_and_continue */
    RTS();                                   /* RTS ($8367, locret_18367) */

enemy_erase_and_continue:                    /* $82FA - entry of the erase-and-continue proc */
    enemy_erase_and_continue();              /* call $82FA, then leave sub_18347 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $8368  sub_18368 - chase slot X towards its target Y position
 *
 *  Original: src/bank6.asm:736-762
 *      LDA $542,X / CMP $6E2,X / BEQ locret_18367 / BCS loc_18382 /
 *      LDA $5F8,X / CLC / ADC $716,X / STA $5F8,X / BCC locret_18381 /
 *      INC $612,X /
 *  locret_18381:
 *      RTS
 *  loc_18382:
 *      LDA $5F8,X / SEC / SBC $716,X / STA $5F8,X / BCS locret_18391 /
 *      DEC $612,X /
 *  locret_18391:
 *      RTS
 *
 *  locret_18367 ($8367) lives in sub_18347 and is reproduced here.
 * ------------------------------------------------------------------------ */
void sub_18368(void)                         /* $8368 */
{
    LDA(ABSX(byte_542));                     /* LDA $542,X */
    CMP(ABSX(byte_6E2));                     /* CMP $6E2,X */
    BEQ(locret_18367);                       /* BEQ locret_18367 */
    BCS(loc_18382);                          /* BCS loc_18382 */
    LDA(ABSX(byte_5F8));                     /* LDA $5F8,X */
    CLC();                                   /* CLC */
    ADC(ABSX(byte_716));                     /* ADC $716,X */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    BCC(locret_18381);                       /* BCC locret_18381 */
    INC(ABSX(byte_612));                     /* INC $612,X */

locret_18381:                                /* $8381 */
    RTS();                                   /* RTS */

loc_18382:                                   /* $8382 */
    LDA(ABSX(byte_5F8));                     /* LDA $5F8,X */
    SEC();                                   /* SEC */
    SBC(ABSX(byte_716));                     /* SBC $716,X */
    STA(ABSX(byte_5F8));                     /* STA $5F8,X */
    BCS(locret_18391);                       /* BCS locret_18391 */
    DEC(ABSX(byte_612));                     /* DEC $612,X */

locret_18391:                                /* $8391 */
    RTS();                                   /* RTS */

locret_18367:                                /* $8367 - shared RTS (in sub_18347) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $8392  sub_18392 - chase slot X towards its target X position
 *
 *  Original: src/bank6.asm:768-794
 *      LDA $55C,X / CMP $6FC,X / BEQ locret_18367 / BCS loc_183AC /
 *      LDA $62C,X / CLC / ADC $730,X / STA $62C,X / BCC locret_183AB /
 *      INC $646,X /
 *  locret_183AB:
 *      RTS
 *  loc_183AC:
 *      LDA $62C,X / SEC / SBC $730,X / STA $62C,X / BCS locret_183BB /
 *      DEC $646,X /
 *  locret_183BB:
 *      RTS
 *
 *  locret_18367 ($8367) lives in sub_18347 and is reproduced here.
 * ------------------------------------------------------------------------ */
void sub_18392(void)                         /* $8392 */
{
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    CMP(ABSX(byte_6FC));                     /* CMP $6FC,X */
    BEQ(locret_18367);                       /* BEQ locret_18367 */
    BCS(loc_183AC);                          /* BCS loc_183AC */
    LDA(ABSX(byte_62C));                     /* LDA $62C,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0730));                     /* ADC $730,X */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    BCC(locret_183AB);                       /* BCC locret_183AB */
    INC(ABSX(byte_646));                     /* INC $646,X */

locret_183AB:                                /* $83AB */
    RTS();                                   /* RTS */

loc_183AC:                                   /* $83AC */
    LDA(ABSX(byte_62C));                     /* LDA $62C,X */
    SEC();                                   /* SEC */
    SBC(ABSX(0x0730));                     /* SBC $730,X */
    STA(ABSX(byte_62C));                     /* STA $62C,X */
    BCS(locret_183BB);                       /* BCS locret_183BB */
    DEC(ABSX(byte_646));                     /* DEC $646,X */

locret_183BB:                                /* $83BB */
    RTS();                                   /* RTS */

locret_18367:                                /* $8367 - shared RTS (in sub_18347) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $83BC  sub_183BC - animation frame counter of slot X
 *
 *  Original: src/bank6.asm:800-819
 *      LDA $67A,X / CLC / ADC $694,X / STA $694,X / BCC loc_183D8 /
 *      INC $6AE,X / LDA $6AE,X / CMP $6C8,X / BCC loc_183D8 / LDA #0 /
 *      STA $6AE,X /
 *  loc_183D8:
 *      LDA $6AE,X / TAY / RTS
 * ------------------------------------------------------------------------ */
void sub_183BC(void)                         /* $83BC */
{
    LDA(ABSX(unk_67A));                      /* LDA $67A,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0694));                       /* ADC $694,X */
    STA(ABSX(0x0694));                       /* STA $694,X */
    BCC(loc_183D8);                          /* BCC loc_183D8 */
    INC(ABSX(0x06AE));                       /* INC $6AE,X */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    CMP(ABSX(0x06C8));                       /* CMP $6C8,X */
    BCC(loc_183D8);                          /* BCC loc_183D8 */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSX(0x06AE));                       /* STA $6AE,X */

loc_183D8:                                   /* $83D8 */
    LDA(ABSX(0x06AE));                       /* LDA $6AE,X */
    TAY();                                   /* TAY */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $83DD  sub_183DD - clear the OAM buffer and reset its pointers
 *
 *  Original: src/bank6.asm:825-835
 *      JSR j_clear_oam_buffer
 *      LDA $83 / AND #$FE / STA $83
 *      LDA #$10 / STA $82
 *      LDA #$EC / STA $81
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_183DD(void)                         /* $83DD */
{
    j_clear_oam_buffer();                    /* JSR j_clear_oam_buffer */
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0xFE);                              /* AND #$FE */
    STA(byte_83);                            /* STA $83 */
    LDAI(0x10);                              /* LDA #$10 */
    STA(0x0082);                             /* STA $82 */
    LDAI(0xEC);                              /* LDA #$EC */
    STA(0x0081);                             /* STA $81 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $83EF  sub_183EF - update the sprite-buffer write pointers
 *
 *  Original: src/bank6.asm:841-867
 *      LDA $83 / AND #$7D / STA $83 / AND #1 / BNE loc_183FE /
 *      LDA #$10 / STA $80 / RTS
 *  loc_183FE:
 *      LDA $82 / CMP #$10 / BNE loc_1840C / LDA $83 / ORA #$80 / STA $83 /
 *      LDA #$F0 /
 *  loc_1840C:
 *      SEC / SBC #4 / STA $80 / RTS
 * ------------------------------------------------------------------------ */
void sub_183EF(void)                         /* $83EF */
{
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0x7D);                              /* AND #$7D */
    STA(byte_83);                            /* STA $83 */
    ANDI(0x01);                              /* AND #1 */
    BNE(loc_183FE);                          /* BNE loc_183FE */
    LDAI(0x10);                              /* LDA #$10 */
    STA(unk_80);                             /* STA $80 */
    RTS();                                   /* RTS */

loc_183FE:                                   /* $83FE */
    LDA(0x0082);                             /* LDA $82 */
    CMPI(0x10);                              /* CMP #$10 */
    BNE(loc_1840C);                          /* BNE loc_1840C */
    LDA(byte_83);                            /* LDA $83 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_83);                            /* STA $83 */
    LDAI(0xF0);                              /* LDA #$F0 */

loc_1840C:                                   /* $840C */
    SEC();                                   /* SEC */
    SBCI(4);                                 /* SBC #4 */
    STA(unk_80);                             /* STA $80 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8412  add_next_sprite - append one 4-byte sprite to the $200 buffer
 *
 *  Original: src/bank6.asm:874-984
 *      STX $65 / LDA $83 / AND #2 / BNE loc_18435 / LDA $55C,X / PHA /
 *      LDA $590,X / PHA / LDA ($18),Y / CLC / BMI loc_1842E /
 *      ADC $542,X / BCS loc_18433 / BCC loc_1843A /
 *  loc_1842E: ADC $542,X / BCS loc_1843A /
 *  loc_18433: PLA / PLA /
 *  loc_18435: INY / INY / INY / INY / RTS /
 *  loc_1843A: INY / LDX $80 / STA $200,X / ... / LDX $65 / RTS
 * ------------------------------------------------------------------------ */
void add_next_sprite(void)                   /* $8412 */
{
    STX(byte_65);                            /* STX $65 */
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0x02);                              /* AND #2 */
    BNE(loc_18435);                          /* BNE loc_18435 */
    LDA(ABSX(byte_55C));                     /* LDA $55C,X */
    PHA();                                   /* PHA */
    LDA(ABSX(byte_590));                     /* LDA $590,X */
    PHA();                                   /* PHA */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    CLC();                                   /* CLC */
    BMI(loc_1842E);                          /* BMI loc_1842E */
    ADC(ABSX(byte_542));                     /* ADC $542,X */
    BCS(loc_18433);                          /* BCS loc_18433 */
    BCC(loc_1843A);                          /* BCC loc_1843A */

loc_1842E:                                   /* $842E */
    ADC(ABSX(byte_542));                     /* ADC $542,X */
    BCS(loc_1843A);                          /* BCS loc_1843A */

loc_18433:                                   /* $8433 */
    PLA();                                   /* PLA */
    PLA();                                   /* PLA */

loc_18435:                                   /* $8435 */
    INY();                                   /* INY */
    INY();                                   /* INY */
    INY();                                   /* INY */
    INY();                                   /* INY */
    RTS();                                   /* RTS */

loc_1843A:                                   /* $843A */
    INY();                                   /* INY */
    LDX(unk_80);                             /* LDX $80 */
    STA(ABSX(byte_200));                     /* STA $200,X */
    INX();                                   /* INX */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    INY();                                   /* INY */
    STA(ABSX(byte_200));                     /* STA $200,X */
    INX();                                   /* INX */
    PLA();                                   /* PLA */
    STA(byte_10);                            /* STA $10 */
    EOR(INDY(byte_18));                      /* EOR ($18),Y */
    INY();                                   /* INY */
    STA(ABSX(byte_200));                     /* STA $200,X */
    INX();                                   /* INX */
    PLA();                                   /* PLA */
    STA(byte_11);                            /* STA $11 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    BIT(byte_10);                            /* BIT $10 */
    BVC(loc_18460);                          /* BVC loc_18460 */
    EORI(0xFF);                              /* EOR #$FF */
    SEC();                                   /* SEC */
    SBCI(7);                                 /* SBC #7 */

loc_18460:                                   /* $8460 */
    CMPI(0x80);                              /* CMP #$80 */
    BCS(loc_1846A);                          /* BCS loc_1846A */
    ADC(byte_11);                            /* ADC $11 */
    BCS(loc_1846F);                          /* BCS loc_1846F */
    BCC(loc_18477);                          /* BCC loc_18477 */

loc_1846A:                                   /* $846A */
    CLC();                                   /* CLC */
    ADC(byte_11);                            /* ADC $11 */
    BCS(loc_18477);                          /* BCS loc_18477 */

loc_1846F:                                   /* $846F */
    INY();                                   /* INY */
    LDAI(0xF8);                              /* LDA #$F8 */
    STA(ABSX(0x01FD));                       /* STA $1FD,X */
    BNE(loc_184A4);                          /* BNE loc_184A4 */

loc_18477:                                   /* $8477 */
    INY();                                   /* INY */
    STA(ABSX(byte_200));                     /* STA $200,X */
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0x01);                              /* AND #1 */
    BNE(loc_18490);                          /* BNE loc_18490 */
    INX();                                   /* INX */
    STX(unk_80);                             /* STX $80 */
    CPXI(0xF0);                              /* CPX #$F0 */
    BNE(loc_184A4);                          /* BNE loc_184A4 */
    LDA(byte_83);                            /* LDA $83 */
    ORAI(0x82);                              /* ORA #$82 */
    STA(byte_83);                            /* STA $83 */
    BNE(loc_184A4);                          /* BNE loc_184A4 */

loc_18490:                                   /* $8490 */
    TXA();                                   /* TXA */
    SEC();                                   /* SEC */
    SBCI(7);                                 /* SBC #7 */
    STA(unk_80);                             /* STA $80 */
    CMPI(0x10);                              /* CMP #$10 */
    BCS(loc_184A4);                          /* BCS loc_184A4 */
    LDA(byte_83);                            /* LDA $83 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_83);                            /* STA $83 */
    LDAI(0xEC);                              /* LDA #$EC */
    STA(unk_80);                             /* STA $80 */

loc_184A4:                                   /* $84A4 */
    LDX(byte_65);                            /* LDX $65 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $84A7  sub_184A7 - clear the unused part of the $200 sprite buffer
 *
 *  Original: src/bank6.asm:990-1093
 *      LDA $83 / AND #1 / BNE loc_18501 / LDX $80 / LDA $83 / BPL loc_184B8 /
 *      LDA #$F0 / JMP loc_184EA /
 *  loc_184B8: LDA $82 / CMP #$F0 / BEQ loc_184D9 / LDA #$F8 /
 *  loc_184C0: CPX $82 / BCS loc_184CD / STA $200,X / INX x4 / BNE loc_184C0 /
 *  loc_184CD: ... / loc_184D9: ... / loc_184E8: LDA $80 /
 *  loc_184EA: STA $82 / ... / loc_18501: ... / loc_18523: LDA $83 / EOR #1 /
 *      STA $83 / RTS
 * ------------------------------------------------------------------------ */
void sub_184A7(void)                         /* $84A7 */
{
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0x01);                              /* AND #1 */
    BNE(loc_18501);                          /* BNE loc_18501 */
    LDX(unk_80);                             /* LDX $80 */
    LDA(byte_83);                            /* LDA $83 */
    BPL(loc_184B8);                          /* BPL loc_184B8 */
    LDAI(0xF0);                              /* LDA #$F0 */
    goto loc_184EA;                          /* JMP loc_184EA */

loc_184B8:                                   /* $84B8 */
    LDA(0x0082);                             /* LDA $82 */
    CMPI(0xF0);                              /* CMP #$F0 */
    BEQ(loc_184D9);                          /* BEQ loc_184D9 */
    LDAI(0xF8);                              /* LDA #$F8 */

loc_184C0:                                   /* $84C0 */
    CPX(0x0082);                             /* CPX $82 */
    BCS(loc_184CD);                          /* BCS loc_184CD */
    STA(ABSX(byte_200));                     /* STA $200,X */
    INX();                                   /* INX */
    INX();                                   /* INX */
    INX();                                   /* INX */
    INX();                                   /* INX */
    BNE(loc_184C0);                          /* BNE loc_184C0 */

loc_184CD:                                   /* $84CD */
    CPX(0x0081);                             /* CPX $81 */
    BCS(loc_184D3);                          /* BCS loc_184D3 */
    LDX(0x0081);                             /* LDX $81 */

loc_184D3:                                   /* $84D3 */
    LDA(0x0081);                             /* LDA $81 */
    CMPI(0xEC);                              /* CMP #$EC */
    BEQ(loc_184E8);                          /* BEQ loc_184E8 */

loc_184D9:                                   /* $84D9 */
    LDAI(0xF8);                              /* LDA #$F8 */

loc_184DB:                                   /* $84DB */
    CPXI(0xF0);                              /* CPX #$F0 */
    BEQ(loc_184E8);                          /* BEQ loc_184E8 */
    STA(ABSX(byte_200));                     /* STA $200,X */
    INX();                                   /* INX */
    INX();                                   /* INX */
    INX();                                   /* INX */
    INX();                                   /* INX */
    BNE(loc_184DB);                          /* BNE loc_184DB */

loc_184E8:                                   /* $84E8 */
    LDA(unk_80);                             /* LDA $80 */

loc_184EA:                                   /* $84EA */
    STA(0x0082);                             /* STA $82 */
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0x84);                              /* AND #$84 */
    BEQ(loc_18523);                          /* BEQ loc_18523 */
    CMPI(4);                                 /* CMP #4 */
    BNE(loc_18523);                          /* BNE loc_18523 */
    LDA(ppu_status_in_vblank);               /* LDA $7F */
    ANDI(0x20);                              /* AND #$20 */
    BNE(loc_18523);                          /* BNE loc_18523 */
    LDAI(0xEC);                              /* LDA #$EC */
    STA(0x0081);                             /* STA $81 */
    RTS();                                   /* RTS */

loc_18501:                                   /* $8501 */
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0x80);                              /* AND #$80 */
    BEQ(loc_1850E);                          /* BEQ loc_1850E */
    LDA(unk_80);                             /* LDA $80 */
    STA(0x0081);                             /* STA $81 */
    goto loc_18523;                          /* JMP loc_18523 */

loc_1850E:                                   /* $850E */
    LDXI(0x10);                              /* LDX #$10 */
    LDAI(0xF8);                              /* LDA #$F8 */

loc_18512:                                   /* $8512 */
    STA(ABSX(byte_200));                     /* STA $200,X */
    CPX(unk_80);                             /* CPX $80 */
    BEQ(loc_1851F);                          /* BEQ loc_1851F */
    INX();                                   /* INX */
    INX();                                   /* INX */
    INX();                                   /* INX */
    INX();                                   /* INX */
    BNE(loc_18512);                          /* BNE loc_18512 */

loc_1851F:                                   /* $851F */
    LDAI(0xEC);                              /* LDA #$EC */
    STA(0x0081);                             /* STA $81 */

loc_18523:                                   /* $8523 */
    LDA(byte_83);                            /* LDA $83 */
    EORI(0x01);                              /* EOR #1 */
    STA(byte_83);                            /* STA $83 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $852A  clear_main_menu_sprites - clear the PONY logo sprites in $200
 *
 *  Original: src/bank6.asm:1100-1113
 *      LDY #$10
 *  loc_1852C:
 *      DEY
 *      DEY
 *      LDA #0
 *      STA $200,Y
 *      STA $2F0,Y
 *      DEY
 *      DEY
 *      BNE loc_1852C
 *      RTS
 * ------------------------------------------------------------------------ */
void clear_main_menu_sprites(void)           /* $852A */
{
    LDYI(0x10);                              /* LDY #$10 */

loc_1852C:                                   /* $852C */
    DEY();                                   /* DEY */
    DEY();                                   /* DEY */
    LDAI(0x00);                              /* LDA #0 */
    STA(ABSY(byte_200));                     /* STA $200,Y */
    STA(ABSY(0x02F0));                       /* STA $2F0,Y */
    DEY();                                   /* DEY */
    DEY();                                   /* DEY */
    BNE(loc_1852C);                          /* BNE loc_1852C */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $853B  update_score_onscreen - draw the score as sprites at the top
 *
 *  Original: src/bank6.asm:1120-1174
 *      LDA #0 / STA $11 / LDX #$F0 / LDA $83 / AND #1 / BNE loc_18549 /
 *      LDX #0 /
 *  loc_18549: LDY #0 / LDA #$20 / STA $10 /
 *  loc_1854F: CPX #$10 / BNE loc_18555 / LDX #$F0 /
 *  loc_18555: LDA #$10 / STA $200,X / INX / LDA $11 / BNE loc_18571 /
 *      LDA $17A,Y / STA $11 / BNE loc_18571 / CPY #7 / BEQ loc_18571 /
 *      LDA #$F8 / STA $1FF,X / BNE loc_18578 /
 *  loc_18571: LDA $17A,Y / ASL / STA $200,X /
 *  loc_18578: INX / INX / LDA $10 / STA $200,X / CLC / ADC #9 / STA $10 /
 *      INX / INY / CPY #8 / BNE loc_1854F / RTS
 * ------------------------------------------------------------------------ */
void update_score_onscreen(void)             /* $853B */
{
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_11);                            /* STA $11 */
    LDXI(0xF0);                              /* LDX #$F0 */
    LDA(byte_83);                            /* LDA $83 */
    ANDI(0x01);                              /* AND #1 */
    BNE(loc_18549);                          /* BNE loc_18549 */
    LDXI(0x00);                              /* LDX #0 */

loc_18549:                                   /* $8549 */
    LDYI(0x00);                              /* LDY #0 */
    LDAI(0x20);                              /* LDA #$20 */
    STA(byte_10);                            /* STA $10 */

loc_1854F:                                   /* $854F */
    CPXI(0x10);                              /* CPX #$10 */
    BNE(loc_18555);                          /* BNE loc_18555 */
    LDXI(0xF0);                              /* LDX #$F0 */

loc_18555:                                   /* $8555 */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(byte_200));                     /* STA $200,X */
    INX();                                   /* INX */
    LDA(byte_11);                            /* LDA $11 */
    BNE(loc_18571);                          /* BNE loc_18571 */
    LDA(ABSY(byte_17A));                     /* LDA $17A,Y */
    STA(byte_11);                            /* STA $11 */
    BNE(loc_18571);                          /* BNE loc_18571 */
    CPYI(7);                                 /* CPY #7 */
    BEQ(loc_18571);                          /* BEQ loc_18571 */
    LDAI(0xF8);                              /* LDA #$F8 */
    STA(ABSX(0x01FF));                       /* STA $1FF,X */
    BNE(loc_18578);                          /* BNE loc_18578 */

loc_18571:                                   /* $8571 */
    LDA(ABSY(byte_17A));                     /* LDA $17A,Y */
    ASL_A();                                 /* ASL */
    STA(ABSX(byte_200));                     /* STA $200,X */

loc_18578:                                   /* $8578 */
    INX();                                   /* INX */
    INX();                                   /* INX */
    LDA(byte_10);                            /* LDA $10 */
    STA(ABSX(byte_200));                     /* STA $200,X */
    CLC();                                   /* CLC */
    ADCI(9);                                 /* ADC #9 */
    STA(byte_10);                            /* STA $10 */
    INX();                                   /* INX */
    INY();                                   /* INY */
    CPYI(8);                                 /* CPY #8 */
    BNE(loc_1854F);                          /* BNE loc_1854F */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $858B  update_player_lives - draw the remaining lives
 *
 *  Original: src/bank6.asm:1180-1220
 *      LDA #$84 / STA $18 / LDA #0 / STA $19 / LDA #$D0 / STA $84 /
 *      LDA #$C6 / STA $85 / LDA #0 / STA $86 / LDA #$EA / STA $87 /
 *      LDY #0 / JSR add_sprite / LDY $32 / BEQ loc_185AD / DEY /
 *  loc_185AD: CPY #10 / LDA #0 / STA $86 / LDA #$CC / STA $84 /
 *      BCC loc_185C1 / LDA #$CD / STA $87 / TYA / JMP sub_18E29 /
 *  loc_185C1: TYA / ASL / STA $85 / LDA #$DE / STA $87 / LDY #0 /
 *      JMP add_sprite
 * ------------------------------------------------------------------------ */
void update_player_lives(void)               /* $858B */
{
    LDAI(0x84);                              /* LDA #$84 */
    STA(byte_18);                            /* STA $18 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_19);                            /* STA $19 */
    LDAI(0xD0);                              /* LDA #$D0 */
    STA(byte_84);                            /* STA $84 */
    LDAI(0xC6);                              /* LDA #$C6 */
    STA(byte_85);                            /* STA $85 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_86);                            /* STA $86 */
    LDAI(0xEA);                              /* LDA #$EA */
    STA(byte_87);                            /* STA $87 */
    LDYI(0);                                 /* LDY #0 */
    add_sprite();                            /* JSR add_sprite */
    LDY(byte_32);                            /* LDY $32 */
    BEQ(loc_185AD);                          /* BEQ loc_185AD */
    DEY();                                   /* DEY */

loc_185AD:                                   /* $85AD */
    CPYI(10);                                /* CPY #10 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_86);                            /* STA $86 */
    LDAI(0xCC);                              /* LDA #$CC */
    STA(byte_84);                            /* STA $84 */
    BCC(loc_185C1);                          /* BCC loc_185C1 */
    LDAI(0xCD);                              /* LDA #$CD */
    STA(byte_87);                            /* STA $87 */
    TYA();                                   /* TYA */
    sub_18E29();                             /* JMP sub_18E29 */
    RTS();

loc_185C1:                                   /* $85C1 */
    TYA();                                   /* TYA */
    ASL_A();                                 /* ASL */
    STA(byte_85);                            /* STA $85 */
    LDAI(0xDE);                              /* LDA #$DE */
    STA(byte_87);                            /* STA $87 */
    LDYI(0);                                 /* LDY #0 */
    add_sprite();                            /* JMP add_sprite */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $85CE  add_sprite_from_zeropage - add the sprite pointed at by $84
 *
 *  Original: src/bank6.asm:1227-1233
 *      LDA #$84 / STA $18
 *      LDA #0 / STA $19
 *      LDY #0
 *  (no RTS: execution continues into add_sprite at $85D8)
 * ------------------------------------------------------------------------ */
void add_sprite_from_zeropage(void)          /* $85CE */
{
    LDAI(0x84);                              /* LDA #$84 */
    STA(byte_18);                            /* STA $18 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_19);                            /* STA $19 */
    LDYI(0);                                 /* LDY #0 */
    add_sprite();                            /* falls through into $85D8 */
    RTS();
}
