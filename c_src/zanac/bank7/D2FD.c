/* bank7/D2FD.c - bank 7, $D2FD-$D8DC: the buffered-VRAM string/number
 * helpers, the START-button pause loop, the fortress/level object spawner and
 * its script interpreter, and the background scroll / name-table row engine.
 *
 * Ported from src/bank7.asm lines 3509-4721 (addresses from Build/bank7.lst).
 *
 * Review notes (PORTING.md; c_src/analysis/bank7_report.md sections 2.1-2.5,
 * 3.1 and 4):
 *
 *  - fall-through into the next procedure: sub_D480 -> sub_D4FA, sub_D56B ->
 *    sub_D5AB, sub_D7E3 -> sub_D824, sub_D824 -> sub_D84A and sub_D84A ->
 *    sub_D84F end with a call plus RTS();
 *  - "JSR switch_case" with the inline word table: sub_D4FA (table $D506,
 *    5 words) and sub_D84F (table $D874, 9 words) become SWITCH_CASE(...);
 *  - branches into another procedure's *entry* (not a call): sub_D6AF BCSes
 *    to sub_D69F, sub_D4FA BCSes to sub_D56B, sub_D56B BPLs to sub_D5AB
 *    and sub_D7E3 branches to sub_D824, which sub_D824 also falls into; each
 *    is a local label plus a call at the end of the branching procedure;
 *  - branches into another procedure's locret_* RTS: sub_D3FF's BEQ
 *    locret_D3F3 ($D3F3, the RTS at the end of sub_D3A8) and sub_D77B's
 *    BEQ/BCC locret_D77A ($D77A, the RTS at the end of sub_D753) use
 *    BEQ_RET()/BCC_RET(), so those two RTSes stay comments here;
 *  - self-loops: sub_D753 ("BNE sub_D753") and sub_D77B ("JMP sub_D77B")
 *    re-enter their own first instruction; they use the local labels
 *    loc_D753/loc_D77B, the same idiom C6AB.c uses for loc_C7EB;
 *  - "JSR loc_D714" ($D714, inside sub_D6DF): a C label is function scoped,
 *    so sub_D8BF reproduces that block (PORTING.md section 5), exactly like
 *    sub_181ED in bank6/8024.c; it ends with "JMP switch_bank", which
 *    returns to the instruction after the JSR ($D8D1);
 *  - sub_D6DF reads tab_D74B through the literal base $D74A (X runs 8..1;
 *    bank7_report.md section 2.9), so the port keeps "LDA $D74A,X";
 *  - sub_D886/sub_D894/sub_D8A1/sub_D8DC tail-call sub_DA28 and sub_D84F
 *    tail-calls sub_DA36; both belong to the chunk that owns $D920+
 *    (sub_D8BF's tail call is sub_D84A, defined here).
 */
#include "zanac.h"
#include "savestate.h"

/* ------------------------------------------------------------------------ *
 *  $D2FD  sub_D2FD - print a zero-suppressed 6-digit string from the ($16)
 *                    block plus a trailing '0' into the $460 write buffer
 *
 *  Original: src/bank7.asm:3509-3556 (bank 7)
 *      LDA #$A / JSR sub_C7D8 / BCS locret_D346 / JSR sub_C7F8 /
 *      STX byte_10 / TAX / LDA #0 / STA byte_11 / TAY / LDA #7 /
 *      STA $460,X / LDA #$19 / STA $461,X / LDA #$18 / STA $462,X
 *  loc_D31E: LDA byte_11 / BNE loc_D32E / LDA ($16),Y / ORA byte_11 /
 *      STA byte_11 / BNE loc_D330 / LDA #$20 / BNE loc_D332
 *  loc_D32E: LDA ($16),Y
 *  loc_D330: ADC #$30
 *  loc_D332: STA $463,X / INY / INX / CPY #6 / BNE loc_D31E / LDA #$30 /
 *      STA $463,X / LDX byte_65 / JSR sub_C807 / CLC
 *  locret_D346: RTS
 * ------------------------------------------------------------------------ */
void sub_D2FD(void)                          /* $D2FD */
{
    LDAI(0x0A);                              /* LDA #$A */
    sub_C7D8();                              /* JSR sub_C7D8 */
    BCS(locret_D346);                        /* BCS locret_D346 */
    sub_C7F8();                              /* JSR sub_C7F8 */
    STX(byte_10);                            /* STX byte_10 */
    TAX();                                   /* TAX */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_11);                            /* STA byte_11 */
    TAY();                                   /* TAY */
    LDAI(7);                                 /* LDA #7 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDAI(0x19);                              /* LDA #$19 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDAI(0x18);                              /* LDA #$18 */
    STA(ABSX(0x0462));                       /* STA $462,X */

loc_D31E:                                    /* $D31E */
    LDA(byte_11);                            /* LDA byte_11 */
    BNE(loc_D32E);                           /* BNE loc_D32E */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    ORA(byte_11);                            /* ORA byte_11 */
    STA(byte_11);                            /* STA byte_11 */
    BNE(loc_D330);                           /* BNE loc_D330 */
    LDAI(0x20);                              /* LDA #$20 */
    BNE(loc_D332);                           /* BNE loc_D332 */

loc_D32E:                                    /* $D32E */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */

loc_D330:                                    /* $D330 */
    ADCI(0x30);                              /* ADC #$30 */

loc_D332:                                    /* $D332 */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INY();                                   /* INY */
    INX();                                   /* INX */
    CPYI(6);                                 /* CPY #6 */
    BNE(loc_D31E);                           /* BNE loc_D31E */
    LDAI(0x30);                              /* LDA #$30 */
    STA(ABSX(0x0463));                       /* STA $463,X */
    LDX(byte_65);                            /* LDX byte_65 */
    sub_C807();                              /* JSR sub_C807 */
    CLC();                                   /* CLC */

locret_D346:                                 /* $D346 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D347  sub_D347 - print A as a three-digit decimal number in the row buffer
 *
 *  Original: src/bank7.asm:3562-3619 (bank 7)
 *      LDY #$30 / STY byte_11 / STY byte_12 / SEC / SBC #$64 /
 *      BCC loc_D35C / INC byte_11 / SBC #$64 / BCC loc_D35C / INC byte_11 /
 *      SBC #$64 / loc_D35C: ADC #$64 /
 *      loc_D35E: SBC #$A / BCC loc_D366 / INC byte_12 / BNE loc_D35E /
 *      loc_D366: ADC #$3A / STA byte_13 / LDA byte_11 / CMP #$30 /
 *      BNE loc_D37E / LDA #$20 / STA byte_11 / LDA byte_12 / CMP #$30 /
 *      BNE loc_D37E / LDA #$20 / STA byte_12 /
 *      loc_D37E: LDA #6 / JSR sub_C7EB / JSR sub_C7F8 / TAY / LDA #3 /
 *      STA $460,Y / LDA byte_19 / STA $461,Y / LDA byte_18 / STA $462,Y /
 *      LDA byte_11 / STA $463,Y / LDA byte_12 / STA $464,Y /
 *      LDA byte_13 / STA $465,Y / JMP sub_C807
 * ------------------------------------------------------------------------ */
void sub_D347(void)                          /* $D347 */
{
    LDYI(0x30);                              /* LDY #$30 */
    STY(byte_11);                            /* STY byte_11 */
    STY(byte_12);                            /* STY byte_12 */
    SEC();                                   /* SEC */
    SBCI(0x64);                              /* SBC #$64 */
    BCC(loc_D35C);                           /* BCC loc_D35C */
    INC(byte_11);                            /* INC byte_11 */
    SBCI(0x64);                              /* SBC #$64 */
    BCC(loc_D35C);                           /* BCC loc_D35C */
    INC(byte_11);                            /* INC byte_11 */
    SBCI(0x64);                              /* SBC #$64 */

loc_D35C:                                    /* $D35C */
    ADCI(0x64);                              /* ADC #$64 */

loc_D35E:                                    /* $D35E */
    SBCI(0x0A);                              /* SBC #$A */
    BCC(loc_D366);                           /* BCC loc_D366 */
    INC(byte_12);                            /* INC byte_12 */
    BNE(loc_D35E);                           /* BNE loc_D35E */

loc_D366:                                    /* $D366 */
    ADCI(0x3A);                              /* ADC #$3A */
    STA(byte_13);                            /* STA byte_13 */
    LDA(byte_11);                            /* LDA byte_11 */
    CMPI(0x30);                              /* CMP #$30 */
    BNE(loc_D37E);                           /* BNE loc_D37E */
    LDAI(0x20);                              /* LDA #$20 */
    STA(byte_11);                            /* STA byte_11 */
    LDA(byte_12);                            /* LDA byte_12 */
    CMPI(0x30);                              /* CMP #$30 */
    BNE(loc_D37E);                           /* BNE loc_D37E */
    LDAI(0x20);                              /* LDA #$20 */
    STA(byte_12);                            /* STA byte_12 */

loc_D37E:                                    /* $D37E */
    LDAI(6);                                 /* LDA #6 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAY();                                   /* TAY */
    LDAI(3);                                 /* LDA #3 */
    STA(ABSY(0x0460));                       /* STA $460,Y */
    LDA(byte_19);                            /* LDA byte_19 */
    STA(ABSY(0x0461));                       /* STA $461,Y */
    LDA(byte_18);                            /* LDA byte_18 */
    STA(ABSY(0x0462));                       /* STA $462,Y */
    LDA(byte_11);                            /* LDA byte_11 */
    STA(ABSY(0x0463));                       /* STA $463,Y */
    LDA(byte_12);                            /* LDA byte_12 */
    STA(ABSY(0x0464));                       /* STA $464,Y */
    LDA(byte_13);                            /* LDA byte_13 */
    STA(ABSY(0x0465));                       /* STA $465,Y */
    sub_C807();                              /* JMP sub_C807 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D3A8  sub_D3A8 - the START-button pause loop (called from sub_EA0B)
 *
 *  Original: src/bank7.asm:3625-3678 (bank 7)
 *      LDA pad_test1 / AND #$10 / BNE loc_D3AF / RTS
 *  loc_D3AF: JSR sub_F0F4 / LDA byte_83 / PHA / LDA byte_17A / PHA
 *  loc_D3B9: JSR sub_D3F4 / LDA byte_19B / BEQ loc_D3D5 /
 *      LDA pad_test1 / AND #$20 / BEQ loc_D3D2 / INC byte_7A / LDA byte_7A /
 *      AND #7 / STA byte_7A / STA byte_17A /
 *  loc_D3D2: JSR sub_D3FF /
 *  loc_D3D5: LDA pad_test1 / AND #$10 / BEQ loc_D3B9 / JSR sub_F0FA /
 *      LDY #$1E /
 *  loc_D3E0: TYA / PHA / JSR sub_D3F4 / PLA / TAY / DEY / BNE loc_D3E0 /
 *      PLA / STA byte_17A / PLA / EOR #4 / STA byte_83
 *  locret_D3F3: RTS
 *
 *  locret_D3F3 ($D3F3) is the RTS here; sub_D3FF branches into it with
 *  BEQ_RET(), so it stays a comment instead of an (unused) C label.
 * ------------------------------------------------------------------------ */
void sub_D3A8(void)                          /* $D3A8 */
{
    /* Pause save resume: the prologue above loc_D3B9 already ran in the saved
     * state (sound paused, byte_83/byte_17A on the emulated stack). */
    if (zanac_resume_pause) {
        zanac_resume_pause = 0;
        goto loc_D3B9;
    }
    LDA(pad_test1);                          /* LDA pad_test1 */
    ANDI(0x10);                              /* AND #$10 */
    BNE(loc_D3AF);                           /* BNE loc_D3AF */
    RTS();                                   /* RTS */

loc_D3AF:                                    /* $D3AF */
    sub_F0F4();                              /* JSR sub_F0F4 */
    LDA(byte_83);                            /* LDA byte_83 */
    PHA();                                   /* PHA */
    LDA(byte_17A);                           /* LDA byte_17A */
    PHA();                                   /* PHA */

loc_D3B9:                                    /* $D3B9 */
    /* The pause loop is the third restartable boundary: a state saved here is
     * resumed by sub_D3A8 at this label (see zanac_ram.h/savestate.h). */
    zanac_resume_kind = ZANAC_RESUME_PAUSE;
    zanac_state_checkpoint();
    sub_D3F4();                              /* JSR sub_D3F4 */
    LDA(byte_19B);                           /* LDA byte_19B */
    BEQ(loc_D3D5);                           /* BEQ loc_D3D5 */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_D3D2);                           /* BEQ loc_D3D2 */
    INC(byte_7A);                            /* INC byte_7A */
    LDA(byte_7A);                            /* LDA byte_7A */
    ANDI(7);                                 /* AND #7 */
    STA(byte_7A);                            /* STA byte_7A */
    STA(byte_17A);                           /* STA byte_17A */

loc_D3D2:                                    /* $D3D2 */
    sub_D3FF();                              /* JSR sub_D3FF */

loc_D3D5:                                    /* $D3D5 */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_D3B9);                           /* BEQ loc_D3B9 */
    sub_F0FA();                              /* JSR sub_F0FA */
    LDYI(0x1E);                              /* LDY #$1E */

loc_D3E0:                                    /* $D3E0 */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    sub_D3F4();                              /* JSR sub_D3F4 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEY();                                   /* DEY */
    BNE(loc_D3E0);                           /* BNE loc_D3E0 */
    PLA();                                   /* PLA */
    STA(byte_17A);                           /* STA byte_17A */
    PLA();                                   /* PLA */
    EORI(4);                                 /* EOR #4 */
    STA(byte_83);                            /* STA byte_83 */
    /* $D3F3 locret_D3F3 - the RTS; sub_D3FF branches into it (BEQ_RET) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D3F4  sub_D3F4 - one frame of the pause screen
 *
 *  Original: src/bank7.asm:3684-3690 (bank 7)
 *      JSR vsync / JSR j_update_score_onscreen / INC byte_83 / JMP pad_read
 * ------------------------------------------------------------------------ */
void sub_D3F4(void)                          /* $D3F4 */
{
    vsync();                                 /* JSR vsync */
    j_update_score_onscreen();               /* JSR j_update_score_onscreen */
    INC(byte_83);                            /* INC byte_83 */
    pad_read();                              /* JMP pad_read */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D3FF  sub_D3FF - handle the pause-screen pad state
 *
 *  Original: src/bank7.asm:3696-3763 (bank 7)
 *      JSR sub_E76C / BEQ locret_D3F3 / LDA temp_0 / AND #1 / BEQ loc_D40E /
 *      LDA #$FF / STA byte_32 /
 *      loc_D40E: LDA temp_0 / AND #$80 / BEQ loc_D427 / LDA byte_7D /
 *      CLC / ADC #1 / CMP #$D / BNE loc_D41F / LDA #1 /
 *      loc_D41F: STA byte_92 / LDA game_mode / ORA #$30 / STA game_mode /
 *      loc_D427: LDA temp_0 / AND #$40 / BEQ loc_D43E / LDA byte_7D / SEC /
 *      SBC #1 / BNE loc_D436 / LDA #$C /
 *      loc_D436: STA byte_92 / LDA game_mode / ORA #$30 / STA game_mode /
 *      loc_D43E: LDA temp_1 / AND #2 / BEQ loc_D454 / LDA #1 / STA byte_51 /
 *      LDA byte_C0 / AND #$22 / CMP #2 / BNE loc_D454 / LDA #0 / STA byte_C2 /
 *      loc_D454: LDA byte_8 / AND #2 / BEQ locret_D462 / LDA #0 / STA byte_AC /
 *      LDA #8 / STA byte_AD /
 *      locret_D462: RTS
 * ------------------------------------------------------------------------ */
void sub_D3FF(void)                          /* $D3FF */
{
    sub_E76C();                              /* JSR sub_E76C */
    BEQ_RET(locret_D3F3);                    /* BEQ locret_D3F3 (RTS in sub_D3A8) */
    LDA(temp_0);                             /* LDA temp_0 */
    ANDI(1);                                 /* AND #1 */
    BEQ(loc_D40E);                           /* BEQ loc_D40E */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(byte_32);                            /* STA byte_32 */

loc_D40E:                                    /* $D40E */
    LDA(temp_0);                             /* LDA temp_0 */
    ANDI(0x80);                              /* AND #$80 */
    BEQ(loc_D427);                           /* BEQ loc_D427 */
    LDA(byte_7D);                            /* LDA byte_7D */
    CLC();                                   /* CLC */
    ADCI(1);                                 /* ADC #1 */
    CMPI(0x0D);                              /* CMP #$D */
    BNE(loc_D41F);                           /* BNE loc_D41F */
    LDAI(1);                                 /* LDA #1 */

loc_D41F:                                    /* $D41F */
    STA(byte_92);                            /* STA byte_92 */
    LDA(game_mode);                          /* LDA game_mode */
    ORAI(0x30);                              /* ORA #$30 */
    STA(game_mode);                          /* STA game_mode */

loc_D427:                                    /* $D427 */
    LDA(temp_0);                             /* LDA temp_0 */
    ANDI(0x40);                              /* AND #$40 */
    BEQ(loc_D43E);                           /* BEQ loc_D43E */
    LDA(byte_7D);                            /* LDA byte_7D */
    SEC();                                   /* SEC */
    SBCI(1);                                 /* SBC #1 */
    BNE(loc_D436);                           /* BNE loc_D436 */
    LDAI(0x0C);                              /* LDA #$C */

loc_D436:                                    /* $D436 */
    STA(byte_92);                            /* STA byte_92 */
    LDA(game_mode);                          /* LDA game_mode */
    ORAI(0x30);                              /* ORA #$30 */
    STA(game_mode);                          /* STA game_mode */

loc_D43E:                                    /* $D43E */
    LDA(temp_1);                             /* LDA temp_1 */
    ANDI(2);                                 /* AND #2 */
    BEQ(loc_D454);                           /* BEQ loc_D454 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_51);                            /* STA byte_51 */
    LDA(byte_C0);                            /* LDA byte_C0 */
    ANDI(0x22);                              /* AND #$22 */
    CMPI(2);                                 /* CMP #2 */
    BNE(loc_D454);                           /* BNE loc_D454 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_C2);                            /* STA byte_C2 */

loc_D454:                                    /* $D454 */
    LDA(byte_8);                             /* LDA byte_8 */
    ANDI(2);                                 /* AND #2 */
    BEQ(locret_D462);                        /* BEQ locret_D462 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_AC);                            /* STA byte_AC */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_AD);                            /* STA byte_AD */

locret_D462:                                 /* $D462 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D463  sub_D463 - spawn object type $44 (erase marker?)
 *
 *  Original: src/bank7.asm:3769-3779 (bank 7)
 *      JSR j_check_enemy_overflow / BCS locret_D471 / LDA #0 / STA byte_46 /
 *      LDA #$44 / STA $528,Y
 *  locret_D471: RTS
 * ------------------------------------------------------------------------ */
void sub_D463(void)                          /* $D463 */
{
    j_check_enemy_overflow();                /* JSR j_check_enemy_overflow */
    BCS(locret_D471);                        /* BCS locret_D471 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_46);                            /* STA byte_46 */
    LDAI(0x44);                              /* LDA #$44 */
    STA(ABSY(byte_528));                     /* STA $528,Y */

locret_D471:                                 /* $D471 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D472  sub_D472 - spawn object type $3D or decrement byte_4D
 *
 *  Original: src/bank7.asm:3785-3796 (bank 7)
 *      JSR j_check_enemy_overflow / BCS loc_D47D / LDA #$3D / STA $528,Y /
 *      RTS
 *  loc_D47D: DEC byte_4D / RTS
 * ------------------------------------------------------------------------ */
void sub_D472(void)                          /* $D472 */
{
    j_check_enemy_overflow();                /* JSR j_check_enemy_overflow */
    BCS(loc_D47D);                           /* BCS loc_D47D */
    LDAI(0x3D);                              /* LDA #$3D */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    RTS();                                   /* RTS */

loc_D47D:                                    /* $D47D */
    DEC(byte_4D);                            /* DEC byte_4D */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D480  sub_D480 - fortress/level object spawner and state machine
 *
 *  Original: src/bank7.asm:3802-3885 (bank 7)
 *      LDA game_mode / AND #8 / BEQ loc_D487
 *  locret_D486: RTS
 *  loc_D487: LDA byte_62 / AND #1 / BEQ loc_D490 / JSR sub_D5D7 /
 *  loc_D490: LDA game_mode / AND #$10 / BEQ loc_D49C / LDA byte_C0 /
 *      AND #2 / BEQ locret_D486 /
 *  loc_D49C: LDA byte_46 / BEQ loc_D4A3 / JSR sub_D463 /
 *  loc_D4A3: LDA byte_528 / CMP #$81 / BNE loc_D4DE / DEC byte_52 /
 *      BNE loc_D4DE / LDA byte_53 / STA byte_52 / LDA byte_4D / INC byte_4D /
 *      AND #$F / BNE loc_D4BD / JSR sub_D472 /
 *  loc_D4BD: LDA byte_62 / AND #$A / CMP #2 / BNE loc_D4DE /
 *      JSR j_check_enemy_overflow / BCS loc_D4DE / LDX byte_50 /
 *      LDA random_enemy_tab,X / STA $528,Y / JSR sub_D5C1 / INX /
 *      CPX byte_4F / BNE loc_D4DC / LDX byte_4E /
 *  loc_D4DC: STX byte_50 /
 *  loc_D4DE: LDA byte_90 / AND #1 / BEQ locret_D4F0 / LDA byte_A2 /
 *      AND #$1F / BNE locret_D4F0 / LDA byte_6E / BEQ loc_D4F1 /
 *      DEC byte_6E /
 *  locret_D4F0: RTS
 *  loc_D4F1: JSR switch_bank_5 / LDY #0 / LDA #1 / STA byte_14
 *  (no RTS: falls through into sub_D4FA)
 * ------------------------------------------------------------------------ */
void sub_D480(void)                          /* $D480 */
{
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(8);                                 /* AND #8 */
    BEQ(loc_D487);                           /* BEQ loc_D487 */

locret_D486:                                 /* $D486 */
    RTS();                                   /* RTS */

loc_D487:                                    /* $D487 */
    LDA(byte_62);                            /* LDA byte_62 */
    ANDI(1);                                 /* AND #1 */
    BEQ(loc_D490);                           /* BEQ loc_D490 */
    sub_D5D7();                              /* JSR sub_D5D7 */

loc_D490:                                    /* $D490 */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_D49C);                           /* BEQ loc_D49C */
    LDA(byte_C0);                            /* LDA byte_C0 */
    ANDI(2);                                 /* AND #2 */
    BEQ(locret_D486);                        /* BEQ locret_D486 */

loc_D49C:                                    /* $D49C */
    LDA(byte_46);                            /* LDA byte_46 */
    BEQ(loc_D4A3);                           /* BEQ loc_D4A3 */
    sub_D463();                              /* JSR sub_D463 */

loc_D4A3:                                    /* $D4A3 */
    LDA(byte_528);                           /* LDA byte_528 */
    CMPI(0x81);                              /* CMP #$81 */
    BNE(loc_D4DE);                           /* BNE loc_D4DE */
    DEC(byte_52);                            /* DEC byte_52 */
    BNE(loc_D4DE);                           /* BNE loc_D4DE */
    LDA(byte_53);                            /* LDA byte_53 */
    STA(byte_52);                            /* STA byte_52 */
    LDA(byte_4D);                            /* LDA byte_4D */
    INC(byte_4D);                            /* INC byte_4D */
    ANDI(0x0F);                              /* AND #$F */
    BNE(loc_D4BD);                           /* BNE loc_D4BD */
    sub_D472();                              /* JSR sub_D472 */

loc_D4BD:                                    /* $D4BD */
    LDA(byte_62);                            /* LDA byte_62 */
    ANDI(0x0A);                              /* AND #$A */
    CMPI(2);                                 /* CMP #2 */
    BNE(loc_D4DE);                           /* BNE loc_D4DE */
    j_check_enemy_overflow();                /* JSR j_check_enemy_overflow */
    BCS(loc_D4DE);                           /* BCS loc_D4DE */
    LDX(byte_50);                            /* LDX byte_50 */
    LDA(ABSX(random_enemy_tab));             /* LDA random_enemy_tab,X */
    STA(ABSY(byte_528));                     /* STA $528,Y */
    sub_D5C1();                              /* JSR sub_D5C1 */
    INX();                                   /* INX */
    CPX(byte_4F);                            /* CPX byte_4F */
    BNE(loc_D4DC);                           /* BNE loc_D4DC */
    LDX(byte_4E);                            /* LDX byte_4E */

loc_D4DC:                                    /* $D4DC */
    STX(byte_50);                            /* STX byte_50 */

loc_D4DE:                                    /* $D4DE */
    LDA(byte_90);                            /* LDA byte_90 */
    ANDI(1);                                 /* AND #1 */
    BEQ(locret_D4F0);                        /* BEQ locret_D4F0 */
    LDA(byte_A2);                            /* LDA byte_A2 */
    ANDI(0x1F);                              /* AND #$1F */
    BNE(locret_D4F0);                        /* BNE locret_D4F0 */
    LDA(byte_6E);                            /* LDA byte_6E */
    BEQ(loc_D4F1);                           /* BEQ loc_D4F1 */
    DEC(byte_6E);                            /* DEC byte_6E */

locret_D4F0:                                 /* $D4F0 */
    RTS();                                   /* RTS */

loc_D4F1:                                    /* $D4F1 */
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDYI(0);                                 /* LDY #0 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_14);                            /* STA byte_14 */
    sub_D4FA();                              /* falls through into $D4FA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D4FA  sub_D4FA - read one script opcode and dispatch it
 *
 *  Original: src/bank7.asm:3891-3898 (bank 7)
 *      LDA ($6C),Y / INY / STY byte_15 / CMP #5 / BCS sub_D56B /
 *      JSR switch_case
 *      .WORD $D50F, $D519, $D527, $D54C, $D558   ; sub_D510-1 .. sub_D559-1
 *
 *  The 5-word case table lives at $D506, immediately after the JSR
 *  (bank7_report.md section 2.5b).
 * ------------------------------------------------------------------------ */
void sub_D4FA(void)                          /* $D4FA */
{
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    INY();                                   /* INY */
    STY(byte_15);                            /* STY byte_15 */
    CMPI(5);                                 /* CMP #5 */
    BCS(sub_D56B);                           /* BCS sub_D56B */
    SWITCH_CASE(0xD506);                     /* JSR switch_case ; table of 5 words at $D506 */
    RTS();

sub_D56B:                                    /* $D56B - entry of the next-but-one procedure */
    sub_D56B();                              /* BCS sub_D56B */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D510  sub_D510 - script opcode 0: set the object timer
 *
 *  Original: src/bank7.asm:3910-3916 (bank 7)
 *      LDY byte_15 / LDA ($6C),Y / STA byte_6E / INY / JMP sub_D5AB
 * ------------------------------------------------------------------------ */
void sub_D510(void)                          /* $D510 */
{
    LDY(byte_15);                            /* LDY byte_15 */
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    STA(byte_6E);                            /* STA byte_6E */
    INY();                                   /* INY */
    sub_D5AB();                              /* JMP sub_D5AB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D51A  sub_D51A - script opcode 1: OR the flags into byte_62
 *
 *  Original: src/bank7.asm:3922-3930 (bank 7)
 *      LDY byte_15 / LDA byte_62 / AND #$F9 / ORA ($6C),Y / STA byte_62 /
 *      INY / JMP sub_D5AB
 * ------------------------------------------------------------------------ */
void sub_D51A(void)                          /* $D51A */
{
    LDY(byte_15);                            /* LDY byte_15 */
    LDA(byte_62);                            /* LDA byte_62 */
    ANDI(0xF9);                              /* AND #$F9 */
    ORA(INDY(byte_6C));                      /* ORA ($6C),Y */
    STA(byte_62);                            /* STA byte_62 */
    INY();                                   /* INY */
    sub_D5AB();                              /* JMP sub_D5AB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D528  sub_D528 - script opcode 2: add a signed delta to byte_4B
 *
 *  Original: src/bank7.asm:3936-3965 (bank 7)
 *      LDY byte_15 / LDA ($6C),Y / BMI loc_D538 / CLC / ADC byte_4B /
 *      BCC loc_D535 / LDA #$FF /
 *  loc_D535: JMP loc_D541
 *  loc_D538: CLC / ADC byte_4B / BCS loc_D53F / LDA #0 /
 *  loc_D53F: STA byte_47 /
 *  loc_D541: STA byte_4B / LDA byte_62 / ORA #1 / STA byte_62 / INY /
 *      JMP sub_D5AB
 * ------------------------------------------------------------------------ */
void sub_D528(void)                          /* $D528 */
{
    LDY(byte_15);                            /* LDY byte_15 */
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    BMI(loc_D538);                           /* BMI loc_D538 */
    CLC();                                   /* CLC */
    ADC(byte_4B);                            /* ADC byte_4B */
    BCC(loc_D535);                           /* BCC loc_D535 */
    LDAI(0xFF);                              /* LDA #$FF */

loc_D535:                                    /* $D535 */
    goto loc_D541;                           /* JMP loc_D541 */

loc_D538:                                    /* $D538 */
    CLC();                                   /* CLC */
    ADC(byte_4B);                            /* ADC byte_4B */
    BCS(loc_D53F);                           /* BCS loc_D53F */
    LDAI(0);                                 /* LDA #0 */

loc_D53F:                                    /* $D53F */
    STA(byte_47);                            /* STA byte_47 */

loc_D541:                                    /* $D541 */
    STA(byte_4B);                            /* STA byte_4B */
    LDA(byte_62);                            /* LDA byte_62 */
    ORAI(1);                                 /* ORA #1 */
    STA(byte_62);                            /* STA byte_62 */
    INY();                                   /* INY */
    sub_D5AB();                              /* JMP sub_D5AB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D54D  sub_D54D - script opcode 3: set the repeat counter
 *
 *  Original: src/bank7.asm:3971-3978 (bank 7)
 *      LDY byte_15 / LDA ($6C),Y / STA byte_14 / INC byte_14 / INY /
 *      JMP sub_D5AB
 * ------------------------------------------------------------------------ */
void sub_D54D(void)                          /* $D54D */
{
    LDY(byte_15);                            /* LDY byte_15 */
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    STA(byte_14);                            /* STA byte_14 */
    INC(byte_14);                            /* INC byte_14 */
    INY();                                   /* INY */
    sub_D5AB();                              /* JMP sub_D5AB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D559  sub_D559 - script opcode 4: jump to a new script address
 *
 *  Original: src/bank7.asm:3984-3995 (bank 7)
 *      LDY byte_15 / LDA ($6C),Y / PHA / INY / LDA ($6C),Y / STA byte_6D /
 *      PLA / STA byte_6C / LDY #0 / JMP sub_D5AB
 * ------------------------------------------------------------------------ */
void sub_D559(void)                          /* $D559 */
{
    LDY(byte_15);                            /* LDY byte_15 */
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    PHA();                                   /* PHA */
    INY();                                   /* INY */
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    STA(byte_6D);                            /* STA byte_6D */
    PLA();                                   /* PLA */
    STA(byte_6C);                            /* STA byte_6C */
    LDYI(0);                                 /* LDY #0 */
    sub_D5AB();                              /* JMP sub_D5AB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D56B  sub_D56B - script opcode >= 5: spawn an object from the script
 *
 *  Original: src/bank7.asm:4001-4039 (bank 7)
 *      STA byte_16 / LDA byte_62 / AND #8 / BEQ loc_D57C
 *  loc_D573: LDA byte_16 / BPL sub_D5AB / INY / INY / JMP sub_D5AB
 *  loc_D57C: STY byte_15 / JSR sub_DD9E / STY byte_17 / LDY byte_15 /
 *      BCS loc_D573 / JSR sub_D5C1 / LDX byte_17 / LDA byte_16 /
 *      BMI loc_D595 / STA $528,X / BPL sub_D5AB
 *  loc_D595: AND #$7F / STA $576,X / LDA ($6C),Y / INY / STA $590,X /
 *      LDA ($6C),Y / INY / STA $660,X / LDA #$45 / STA $528,X
 *  (no RTS: falls through into sub_D5AB)
 * ------------------------------------------------------------------------ */
void sub_D56B(void)                          /* $D56B */
{
    STA(byte_16);                            /* STA byte_16 */
    LDA(byte_62);                            /* LDA byte_62 */
    ANDI(8);                                 /* AND #8 */
    BEQ(loc_D57C);                           /* BEQ loc_D57C */

loc_D573:                                    /* $D573 */
    LDA(byte_16);                            /* LDA byte_16 */
    BPL(sub_D5AB);                           /* BPL sub_D5AB */
    INY();                                   /* INY */
    INY();                                   /* INY */
    goto sub_D5AB;                           /* JMP sub_D5AB */

loc_D57C:                                    /* $D57C */
    STY(byte_15);                            /* STY byte_15 */
    sub_DD9E();                              /* JSR sub_DD9E */
    STY(byte_17);                            /* STY byte_17 */
    LDY(byte_15);                            /* LDY byte_15 */
    BCS(loc_D573);                           /* BCS loc_D573 */
    sub_D5C1();                              /* JSR sub_D5C1 */
    LDX(byte_17);                            /* LDX byte_17 */
    LDA(byte_16);                            /* LDA byte_16 */
    BMI(loc_D595);                           /* BMI loc_D595 */
    STA(ABSX(byte_528));                     /* STA $528,X */
    BPL(sub_D5AB);                           /* BPL sub_D5AB */

loc_D595:                                    /* $D595 */
    ANDI(0x7F);                              /* AND #$7F */
    STA(ABSX(byte_576));                     /* STA $576,X */
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    INY();                                   /* INY */
    STA(ABSX(byte_590));                     /* STA $590,X */
    LDA(INDY(byte_6C));                      /* LDA ($6C),Y */
    INY();                                   /* INY */
    STA(ABSX(byte_660));                     /* STA $660,X */
    LDAI(0x45);                              /* LDA #$45 */
    STA(ABSX(byte_528));                     /* STA $528,X */

sub_D5AB:                                    /* $D5AB - entry of the next procedure */
    sub_D5AB();                              /* falls through into $D5AB / BPL-JMP target */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D5AB  sub_D5AB - advance the script pointer or restart the script
 *
 *  Original: src/bank7.asm:4045-4061 (bank 7)
 *      DEC byte_14 / BEQ loc_D5B2 / JMP sub_D4FA
 *  loc_D5B2: TYA / CLC / ADC byte_6C / STA byte_6C / LDA byte_6D / ADC #0 /
 *      STA byte_6D / JMP switch_bank_6
 * ------------------------------------------------------------------------ */
void sub_D5AB(void)                          /* $D5AB */
{
    DEC(byte_14);                            /* DEC byte_14 */
    BEQ(loc_D5B2);                           /* BEQ loc_D5B2 */
    sub_D4FA();                              /* JMP sub_D4FA */
    RTS();

loc_D5B2:                                    /* $D5B2 */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADC(byte_6C);                            /* ADC byte_6C */
    STA(byte_6C);                            /* STA byte_6C */
    LDA(byte_6D);                            /* LDA byte_6D */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_6D);                            /* STA byte_6D */
    switch_bank_6();                         /* JMP switch_bank_6 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D5C1  sub_D5C1 - advance the random-enemy index and the spawn counters
 *
 *  Original: src/bank7.asm:4067-4086 (bank 7)
 *      LDA byte_48 / CLC / ADC #4 / BCC loc_D5CE / INC byte_47 /
 *      BNE loc_D5CE / DEC byte_47 /
 *  loc_D5CE: STA byte_48 / INC byte_4C / BNE locret_D5D6 / DEC byte_4C /
 *  locret_D5D6: RTS
 * ------------------------------------------------------------------------ */
void sub_D5C1(void)                          /* $D5C1 */
{
    LDA(byte_48);                            /* LDA byte_48 */
    CLC();                                   /* CLC */
    ADCI(4);                                 /* ADC #4 */
    BCC(loc_D5CE);                           /* BCC loc_D5CE */
    INC(byte_47);                            /* INC byte_47 */
    BNE(loc_D5CE);                           /* BNE loc_D5CE */
    DEC(byte_47);                            /* DEC byte_47 */

loc_D5CE:                                    /* $D5CE */
    STA(byte_48);                            /* STA byte_48 */
    INC(byte_4C);                            /* INC byte_4C */
    BNE(locret_D5D6);                        /* BNE locret_D5D6 */
    DEC(byte_4C);                            /* DEC byte_4C */

locret_D5D6:                                 /* $D5D6 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D5D7  sub_D5D7 - recompute the fortress enemy pattern from the scroll
 *
 *  Original: src/bank7.asm:4092-4145 (bank 7)
 *      LDA byte_62 / AND #$FE / STA byte_62 / LDA byte_4B / CLC /
 *      ADC byte_47 / BCC loc_D5E6 / LDA #$FF /
 *  loc_D5E6: LSR / CMP #$10 / BCC loc_D5F3 / SBC #$10 / LSR / LSR / TAY /
 *      LDA unk_D61F,Y /
 *  loc_D5F3: STA byte_4E / CMP byte_50 / BCC loc_D5FB / STA byte_50 /
 *  loc_D5FB: LDA byte_47 / CLC / ADC byte_49 / BCC loc_D604 / LDA #$FF /
 *  loc_D604: LSR / LSR / LSR / LSR / TAY / LDA unk_D63B,Y / CLC /
 *      ADC byte_4E / STA byte_4F / CMP byte_50 / BCS loc_D617 / STA byte_50 /
 *  loc_D617: LDA unk_D64B,Y / STA byte_53 / STA byte_52 / RTS
 * ------------------------------------------------------------------------ */
void sub_D5D7(void)                          /* $D5D7 */
{
    LDA(byte_62);                            /* LDA byte_62 */
    ANDI(0xFE);                              /* AND #$FE */
    STA(byte_62);                            /* STA byte_62 */
    LDA(byte_4B);                            /* LDA byte_4B */
    CLC();                                   /* CLC */
    ADC(byte_47);                            /* ADC byte_47 */
    BCC(loc_D5E6);                           /* BCC loc_D5E6 */
    LDAI(0xFF);                              /* LDA #$FF */

loc_D5E6:                                    /* $D5E6 */
    LSR_A();                                 /* LSR */
    CMPI(0x10);                              /* CMP #$10 */
    BCC(loc_D5F3);                           /* BCC loc_D5F3 */
    SBCI(0x10);                              /* SBC #$10 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_D61F));                     /* LDA unk_D61F,Y */

loc_D5F3:                                    /* $D5F3 */
    STA(byte_4E);                            /* STA byte_4E */
    CMP(byte_50);                            /* CMP byte_50 */
    BCC(loc_D5FB);                           /* BCC loc_D5FB */
    STA(byte_50);                            /* STA byte_50 */

loc_D5FB:                                    /* $D5FB */
    LDA(byte_47);                            /* LDA byte_47 */
    CLC();                                   /* CLC */
    ADC(byte_49);                            /* ADC byte_49 */
    BCC(loc_D604);                           /* BCC loc_D604 */
    LDAI(0xFF);                              /* LDA #$FF */

loc_D604:                                    /* $D604 */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_D63B));                     /* LDA unk_D63B,Y */
    CLC();                                   /* CLC */
    ADC(byte_4E);                            /* ADC byte_4E */
    STA(byte_4F);                            /* STA byte_4F */
    CMP(byte_50);                            /* CMP byte_50 */
    BCS(loc_D617);                           /* BCS loc_D617 */
    STA(byte_50);                            /* STA byte_50 */

loc_D617:                                    /* $D617 */
    LDA(ABSY(unk_D64B));                     /* LDA unk_D64B,Y */
    STA(byte_53);                            /* STA byte_53 */
    STA(byte_52);                            /* STA byte_52 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D69B  get_random_enemy - fetch random_enemy_tab[Y]
 *
 *  Original: src/bank7.asm:4219-4222 (bank 7); Y is a random index 0..63
 *      LDA random_enemy_tab,Y / RTS
 * ------------------------------------------------------------------------ */
void get_random_enemy(void)                  /* $D69B */
{
    LDA(ABSY(random_enemy_tab));             /* LDA random_enemy_tab,Y */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D69F  sub_D69F - force an immediate scroll update
 *
 *  Original: src/bank7.asm:4228-4238 (bank 7)
 *      LDA byte_90 / ORA #$80 / STA byte_90 / JSR sub_C6B5 / LDA byte_90 /
 *      AND #$7F / STA byte_90 / RTS
 * ------------------------------------------------------------------------ */
void sub_D69F(void)                          /* $D69F */
{
    LDA(byte_90);                            /* LDA byte_90 */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_90);                            /* STA byte_90 */
    sub_C6B5();                              /* JSR sub_C6B5 */
    LDA(byte_90);                            /* LDA byte_90 */
    ANDI(0x7F);                              /* AND #$7F */
    STA(byte_90);                            /* STA byte_90 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D6AF  sub_D6AF - step the vertical scroll by byte_AE and update it
 *
 *  Original: src/bank7.asm:4244-4276 (bank 7)
 *      LDA byte_A9 / SEC / SBC byte_AA / STA byte_A9 / ROL / EOR #1 / ROR /
 *      LDA #0 / ADC byte_AB / STA byte_AE / LDA byte_98 / SEC / SBC byte_AE /
 *      PHP / CMP byte_98 / BEQ loc_D6DD / PLP / STA byte_98 / BCS sub_D69F /
 *      LDA byte_99 / EOR #1 / STA byte_99 / LDA byte_98 / SBC #$F /
 *      STA byte_98 / BCS sub_D69F /
 *  loc_D6DD: PLP / RTS
 *
 *  The two "BCS sub_D69F" branches are branches into another procedure's
 *  entry: they are reproduced with the local label sub_D69F below (the
 *  PHP at $D6BB is already balanced by the PLP at $D6C5).
 * ------------------------------------------------------------------------ */
void sub_D6AF(void)                          /* $D6AF */
{
    LDA(byte_A9);                            /* LDA byte_A9 */
    SEC();                                   /* SEC */
    SBC(byte_AA);                            /* SBC byte_AA */
    STA(byte_A9);                            /* STA byte_A9 */
    ROL_A();                                 /* ROL */
    EORI(1);                                 /* EOR #1 */
    ROR_A();                                 /* ROR */
    LDAI(0);                                 /* LDA #0 */
    ADC(byte_AB);                            /* ADC byte_AB */
    STA(byte_AE);                            /* STA byte_AE */
    LDA(byte_98);                            /* LDA byte_98 */
    SEC();                                   /* SEC */
    SBC(byte_AE);                            /* SBC byte_AE */
    PHP();                                   /* PHP */
    CMP(byte_98);                            /* CMP byte_98 */
    BEQ(loc_D6DD);                           /* BEQ loc_D6DD */
    PLP();                                   /* PLP */
    STA(byte_98);                            /* STA byte_98 */
    BCS(sub_D69F);                           /* BCS sub_D69F */
    LDA(byte_99);                            /* LDA byte_99 */
    EORI(1);                                 /* EOR #1 */
    STA(byte_99);                            /* STA byte_99 */
    LDA(byte_98);                            /* LDA byte_98 */
    SBCI(0x0F);                              /* SBC #$F */
    STA(byte_98);                            /* STA byte_98 */
    BCS(sub_D69F);                           /* BCS sub_D69F */

loc_D6DD:                                    /* $D6DD */
    PLP();                                   /* PLP */
    RTS();                                   /* RTS */

sub_D69F:                                    /* $D69F - entry of the previous procedure */
    sub_D69F();                              /* BCS sub_D69F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D6DF  sub_D6DF - reset the level engine and load the level header
 *
 *  Original: src/bank7.asm:4282-4341 (bank 7)
 *      LDA #0 / STA byte_AA / STA byte_AB / STA byte_9C / STA byte_90 /
 *      STA byte_B2 / JSR sub_C6AB / LDA #$FF / STA byte_A2 / STA byte_A3 /
 *      STA byte_A8 / STA byte_B2 / LDA #$F / STA byte_A4 / LDA #$D /
 *      STA byte_A1 / LDA #1 / STA byte_B7 / LDX #8 /
 *  loc_D706: LDA #$80 / STA $3FF,X / LDA $D74A,X / STA $16D,X / DEX /
 *      BNE loc_D706 /
 *  loc_D714: LDA last_bank_num / PHA / JSR switch_bank_5 / LDY #0 /
 *      LDA ($18),Y / STA byte_B3 / INY / LDA ($18),Y / STA byte_B4 /
 *      LDA byte_18 / CLC / ADC #2 / STA byte_B5 / LDA byte_19 / ADC #0 /
 *      STA byte_B6 / LDA byte_B3 / SEC / SBC #1 / STA byte_A2 /
 *      LDA byte_B4 / SBC #0 / STA byte_A3 / LDA #0 / STA byte_AC /
 *      LDA #2 / STA byte_AD / PLA / JMP switch_bank
 *
 *  loc_D714 ($D714) is reached by fall-through here and by "JSR loc_D714"
 *  from sub_D8BF; see the file header.
 * ------------------------------------------------------------------------ */
void sub_D6DF(void)                          /* $D6DF */
{
    LDAI(0);                                 /* LDA #0 */
    STA(byte_AA);                            /* STA byte_AA */
    STA(byte_AB);                            /* STA byte_AB */
    STA(byte_9C);                            /* STA byte_9C */
    STA(byte_90);                            /* STA byte_90 */
    STA(byte_B2);                            /* STA byte_B2 */
    sub_C6AB();                              /* JSR sub_C6AB */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(byte_A2);                            /* STA byte_A2 */
    STA(byte_A3);                            /* STA byte_A3 */
    STA(byte_A8);                            /* STA byte_A8 */
    STA(byte_B2);                            /* STA byte_B2 */
    LDAI(0x0F);                              /* LDA #$F */
    STA(byte_A4);                            /* STA byte_A4 */
    LDAI(0x0D);                              /* LDA #$D */
    STA(byte_A1);                            /* STA byte_A1 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_B7);                            /* STA byte_B7 */
    LDXI(8);                                 /* LDX #8 */

loc_D706:                                    /* $D706 */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(0x03FF));                       /* STA $3FF,X */
    LDA(ABSX(0xD74A));                       /* LDA $D74A,X (tab_D74B - 1) */
    STA(ABSX(0x016D));                       /* STA $16D,X */
    DEX();                                   /* DEX */
    BNE(loc_D706);                           /* BNE loc_D706 */

    /* $D714 loc_D714 - reached by fall-through; "JSR loc_D714" from sub_D8BF
     * ($D8BF) reproduces this block, since a C label cannot be entered from
     * another function. */
    LDA(last_bank_num);                      /* LDA last_bank_num */
    PHA();                                   /* PHA */
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(byte_B3);                            /* STA byte_B3 */
    INY();                                   /* INY */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(byte_B4);                            /* STA byte_B4 */
    LDA(byte_18);                            /* LDA byte_18 */
    CLC();                                   /* CLC */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_B5);                            /* STA byte_B5 */
    LDA(byte_19);                            /* LDA byte_19 */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_B6);                            /* STA byte_B6 */
    LDA(byte_B3);                            /* LDA byte_B3 */
    SEC();                                   /* SEC */
    SBCI(1);                                 /* SBC #1 */
    STA(byte_A2);                            /* STA byte_A2 */
    LDA(byte_B4);                            /* LDA byte_B4 */
    SBCI(0);                                 /* SBC #0 */
    STA(byte_A3);                            /* STA byte_A3 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_AC);                            /* STA byte_AC */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_AD);                            /* STA byte_AD */
    PLA();                                   /* PLA */
    switch_bank();                           /* JMP switch_bank */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D753  sub_D753 - the level engine main loop
 *
 *  Original: src/bank7.asm:4356-4382 (bank 7)
 *      LDA byte_AA / PHA / LDA byte_AB / PHA / LDA #0 / STA byte_AA /
 *      LDA #8 / STA byte_AB / JSR sub_D6AF / PLA / STA byte_AB / PLA /
 *      STA byte_AA / JSR sub_D824 / JSR sub_D77B / LDA byte_A1 / CMP #$B /
 *      BNE sub_D753 / LDA byte_B7 / BNE sub_D753
 *  locret_D77A: RTS
 *
 *  The two BNE branches re-enter this procedure's first instruction, which
 *  the port spells with the local label loc_D753 (see the file header).
 *  locret_D77A ($D77A) is branched into by sub_D77B (Bxx_RET), so it stays a
 *  comment instead of an (unused) C label.
 * ------------------------------------------------------------------------ */
void sub_D753(void)                          /* $D753 */
{
loc_D753:                                    /* $D753 */
    LDA(byte_AA);                            /* LDA byte_AA */
    PHA();                                   /* PHA */
    LDA(byte_AB);                            /* LDA byte_AB */
    PHA();                                   /* PHA */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_AA);                            /* STA byte_AA */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_AB);                            /* STA byte_AB */
    sub_D6AF();                              /* JSR sub_D6AF */
    PLA();                                   /* PLA */
    STA(byte_AB);                            /* STA byte_AB */
    PLA();                                   /* PLA */
    STA(byte_AA);                            /* STA byte_AA */
    sub_D824();                              /* JSR sub_D824 */
    sub_D77B();                              /* JSR sub_D77B */
    LDA(byte_A1);                            /* LDA byte_A1 */
    CMPI(0x0B);                              /* CMP #$B */
    BNE(loc_D753);                           /* BNE sub_D753 */
    LDA(byte_B7);                            /* LDA byte_B7 */
    BNE(loc_D753);                           /* BNE sub_D753 */
    /* $D77A locret_D77A - the RTS (sub_D77B branches into it with Bxx_RET) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D77B  sub_D77B - flush pending name-table rows until nothing is left
 *
 *  Original: src/bank7.asm:4388-4401 (bank 7)
 *      LDA byte_9C / BNE loc_D789 / LDA byte_20 / BEQ locret_D77A /
 *      LDA byte_21 / CMP #$10 / BCC locret_D77A /
 *  loc_D789: JSR sub_D78F / JMP sub_D77B
 * ------------------------------------------------------------------------ */
void sub_D77B(void)                          /* $D77B */
{
loc_D77B:                                    /* $D77B - re-entered by "JMP sub_D77B" below */
    LDA(byte_9C);                            /* LDA byte_9C */
    BNE(loc_D789);                           /* BNE loc_D789 */
    LDA(byte_20);                            /* LDA byte_20 */
    BEQ_RET(locret_D77A);                    /* BEQ locret_D77A (RTS in sub_D753) */
    LDA(byte_21);                            /* LDA byte_21 */
    CMPI(0x10);                              /* CMP #$10 */
    BCC_RET(locret_D77A);                    /* BCC locret_D77A */

loc_D789:                                    /* $D789 */
    sub_D78F();                              /* JSR sub_D78F */
    /* The original spins here until the NMI has drained the name-table buffer
     * (sub_D78F does nothing while rendering is on, the NMI does the work).
     * The 6502 gets an NMI by itself; the port has to let the engine run inside
     * the loop, otherwise this is where GAME OVER and the level-complete pause
     * hang forever. */
    ZANAC_WAIT_NMI();
    goto loc_D77B;                           /* JMP sub_D77B */
}

/* ------------------------------------------------------------------------ *
 *  $D78F  sub_D78F - one name-table update with rendering disabled
 *
 *  Original: src/bank7.asm:4407-4420 (bank 7)
 *      LDA last_ppu_ctrl2 / AND #$18 / BNE locret_D7A4 / LDA last_ppu_ctrl1 /
 *      AND #$7F / STA PPU_CTRL_REG1 / JSR ppu_update_background /
 *      LDA last_ppu_ctrl1 / STA PPU_CTRL_REG1
 *  locret_D7A4: RTS
 * ------------------------------------------------------------------------ */
void sub_D78F(void)                          /* $D78F */
{
    LDA(last_ppu_ctrl2);                     /* LDA last_ppu_ctrl2 */
    ANDI(0x18);                              /* AND #$18 */
    BNE(locret_D7A4);                        /* BNE locret_D7A4 */
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    ANDI(0x7F);                              /* AND #$7F */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */
    ppu_update_background();                 /* JSR ppu_update_background */
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */

locret_D7A4:                                 /* $D7A4 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D7A5  set_nametable_addr_after_scroll - point at the row to update next
 *
 *  Original: src/bank7.asm:4427-4441 (bank 7)
 *      LDA byte_A1 / ASL / TAY / AND #$1C / ASL / STA byte_9D / ADC #$C0 /
 *      STA byte_9E / LDA nt_address_tab,Y / STA byte_9F /
 *      LDA nt_address_tab+1,Y / STA byte_A0 / RTS
 * ------------------------------------------------------------------------ */
void set_nametable_addr_after_scroll(void)   /* $D7A5 */
{
    LDA(byte_A1);                            /* LDA byte_A1 */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    ANDI(0x1C);                              /* AND #$1C */
    ASL_A();                                 /* ASL */
    STA(byte_9D);                            /* STA byte_9D */
    ADCI(0xC0);                              /* ADC #$C0 */
    STA(byte_9E);                            /* STA byte_9E */
    LDA(ABSY(nt_address_tab));               /* LDA nt_address_tab,Y */
    STA(byte_9F);                            /* STA byte_9F */
    LDA(ABSY(nt_address_tab + 1));           /* LDA nt_address_tab+1,Y */
    STA(byte_A0);                            /* STA byte_A0 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $D7E3  sub_D7E3 - align the level scroll with the target scroll
 *
 *  Original: src/bank7.asm:4471-4515 (bank 7)
 *      LDA #0 / STA byte_90 / JSR sub_D6AF / LDA byte_C0 / AND #$20 /
 *      BNE loc_D7F6 / LDA byte_C0 / AND #3 / BNE sub_D824 /
 *  loc_D7F6: LDA byte_AB / CMP byte_AD / BCC loc_D80A / BEQ loc_D800 /
 *      BCS loc_D818 /
 *  loc_D800: LDA byte_AA / CMP byte_AC / BCC loc_D80A / BEQ sub_D824 /
 *      BCS loc_D818 /
 *  loc_D80A: LDA byte_AA / ADC #2 / STA byte_AA / LDA byte_AB / ADC #0 /
 *      STA byte_AB / BCC sub_D824 /
 *  loc_D818: LDA byte_AA / SBC #2 / STA byte_AA / LDA byte_AB / SBC #0 /
 *      STA byte_AB
 *  (no RTS: falls through into sub_D824)
 * ------------------------------------------------------------------------ */
void sub_D7E3(void)                          /* $D7E3 */
{
    LDAI(0);                                 /* LDA #0 */
    STA(byte_90);                            /* STA byte_90 */
    sub_D6AF();                              /* JSR sub_D6AF */
    LDA(byte_C0);                            /* LDA byte_C0 */
    ANDI(0x20);                              /* AND #$20 */
    BNE(loc_D7F6);                           /* BNE loc_D7F6 */
    LDA(byte_C0);                            /* LDA byte_C0 */
    ANDI(3);                                 /* AND #3 */
    BNE(sub_D824);                           /* BNE sub_D824 */

loc_D7F6:                                    /* $D7F6 */
    LDA(byte_AB);                            /* LDA byte_AB */
    CMP(byte_AD);                            /* CMP byte_AD */
    BCC(loc_D80A);                           /* BCC loc_D80A */
    BEQ(loc_D800);                           /* BEQ loc_D800 */
    BCS(loc_D818);                           /* BCS loc_D818 */

loc_D800:                                    /* $D800 */
    LDA(byte_AA);                            /* LDA byte_AA */
    CMP(byte_AC);                            /* CMP byte_AC */
    BCC(loc_D80A);                           /* BCC loc_D80A */
    BEQ(sub_D824);                           /* BEQ sub_D824 */
    BCS(loc_D818);                           /* BCS loc_D818 */

loc_D80A:                                    /* $D80A */
    LDA(byte_AA);                            /* LDA byte_AA */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_AA);                            /* STA byte_AA */
    LDA(byte_AB);                            /* LDA byte_AB */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_AB);                            /* STA byte_AB */
    BCC(sub_D824);                           /* BCC sub_D824 */

loc_D818:                                    /* $D818 */
    LDA(byte_AA);                            /* LDA byte_AA */
    SBCI(2);                                 /* SBC #2 */
    STA(byte_AA);                            /* STA byte_AA */
    LDA(byte_AB);                            /* LDA byte_AB */
    SBCI(0);                                 /* SBC #0 */
    STA(byte_AB);                            /* STA byte_AB */

sub_D824:                                    /* $D824 - entry of the next procedure */
    sub_D824();                              /* falls through into $D824 / BNE-BEQ-BCC target */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D824  sub_D824 - advance the meta-tile row scroll by byte_AE
 *
 *  Original: src/bank7.asm:4521-4547 (bank 7)
 *      LDA byte_A4 / CLC / ADC byte_AE / STA byte_A4 / CMP #$10 /
 *      BCS loc_D830 / RTS
 *  loc_D830: SBC #$10 / STA byte_A4 / JSR sub_D77B / INC byte_A2 /
 *      BNE loc_D83D / INC byte_A3 /
 *  loc_D83D: JSR set_nametable_addr_after_scroll / LDA byte_B7 /
 *      BEQ sub_D84A / LDA byte_A0 / ORA #8 / STA byte_A0
 *  (no RTS: falls through into sub_D84A)
 * ------------------------------------------------------------------------ */
void sub_D824(void)                          /* $D824 */
{
    LDA(byte_A4);                            /* LDA byte_A4 */
    CLC();                                   /* CLC */
    ADC(byte_AE);                            /* ADC byte_AE */
    STA(byte_A4);                            /* STA byte_A4 */
    CMPI(0x10);                              /* CMP #$10 */
    BCS(loc_D830);                           /* BCS loc_D830 */
    RTS();                                   /* RTS */

loc_D830:                                    /* $D830 */
    SBCI(0x10);                              /* SBC #$10 */
    STA(byte_A4);                            /* STA byte_A4 */
    sub_D77B();                              /* JSR sub_D77B */
    INC(byte_A2);                            /* INC byte_A2 */
    BNE(loc_D83D);                           /* BNE loc_D83D */
    INC(byte_A3);                            /* INC byte_A3 */

loc_D83D:                                    /* $D83D */
    set_nametable_addr_after_scroll();       /* JSR set_nametable_addr_after_scroll */
    LDA(byte_B7);                            /* LDA byte_B7 */
    BEQ(sub_D84A);                           /* BEQ sub_D84A */
    LDA(byte_A0);                            /* LDA byte_A0 */
    ORAI(8);                                 /* ORA #8 */
    STA(byte_A0);                            /* STA byte_A0 */

sub_D84A:                                    /* $D84A - entry of the next procedure */
    sub_D84A();                              /* falls through into $D84A / BEQ target */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D84A  sub_D84A - select bank 5 and read its script stream
 *
 *  Original: src/bank7.asm:4553-4557 (bank 7)
 *      JSR switch_bank_5 / LDY #0
 *  (no RTS: falls through into sub_D84F)
 * ------------------------------------------------------------------------ */
void sub_D84A(void)                          /* $D84A */
{
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDYI(0);                                 /* LDY #0 */
    sub_D84F();                              /* falls through into $D84F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D84F  sub_D84F - read one level-script opcode and dispatch it
 *
 *  Original: src/bank7.asm:4563-4590 (bank 7)
 *      LDA byte_A2 / CMP byte_B3 / BNE loc_D85B / LDA byte_A3 /
 *      CMP byte_B4 / BEQ loc_D86C /
 *  loc_D85B: TYA / BEQ loc_D869 / CLC / ADC byte_B5 / STA byte_B5 /
 *      LDA byte_B6 / ADC #0 / STA byte_B6 /
 *  loc_D869: JMP sub_DA36
 *  loc_D86C: LDA ($B5),Y / INY / STY byte_16 / JSR switch_case
 *      .WORD $D885, $D893, $D8A0, $D8BE, $D8DB, $D91F, $D937, $D9DF, $DA15
 *
 *  The 9-word case table lives at $D874, immediately after the JSR
 *  (bank7_report.md section 2.5b).  sub_DA36 belongs to the next chunk.
 * ------------------------------------------------------------------------ */
void sub_D84F(void)                          /* $D84F */
{
    LDA(byte_A2);                            /* LDA byte_A2 */
    CMP(byte_B3);                            /* CMP byte_B3 */
    BNE(loc_D85B);                           /* BNE loc_D85B */
    LDA(byte_A3);                            /* LDA byte_A3 */
    CMP(byte_B4);                            /* CMP byte_B4 */
    BEQ(loc_D86C);                           /* BEQ loc_D86C */

loc_D85B:                                    /* $D85B */
    TYA();                                   /* TYA */
    BEQ(loc_D869);                           /* BEQ loc_D869 */
    CLC();                                   /* CLC */
    ADC(byte_B5);                            /* ADC byte_B5 */
    STA(byte_B5);                            /* STA byte_B5 */
    LDA(byte_B6);                            /* LDA byte_B6 */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_B6);                            /* STA byte_B6 */

loc_D869:                                    /* $D869 */
    sub_DA36();                              /* JMP sub_DA36 */
    RTS();

loc_D86C:                                    /* $D86C */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    INY();                                   /* INY */
    STY(byte_16);                            /* STY byte_16 */
    SWITCH_CASE(0xD874);                     /* JSR switch_case ; table of 9 words at $D874 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D886  sub_D886 - level script case 0: set the scroll target
 *
 *  Original: src/bank7.asm:4606-4614 (bank 7)
 *      LDY byte_16 / LDA ($B5),Y / STA byte_AC / INY / LDA ($B5),Y /
 *      STA byte_AD / JMP sub_DA28
 * ------------------------------------------------------------------------ */
void sub_D886(void)                          /* $D886 */
{
    LDY(byte_16);                            /* LDY byte_16 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_AC);                            /* STA byte_AC */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_AD);                            /* STA byte_AD */
    sub_DA28();                              /* JMP sub_DA28 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D894  sub_D894 - level script case 1: set the special-scroll value
 *
 *  Original: src/bank7.asm:4620-4627 (bank 7)
 *      LDY byte_16 / LDA ($B5),Y / STA byte_A8 / LDA #0 / STA byte_B2 /
 *      JMP sub_DA28
 * ------------------------------------------------------------------------ */
void sub_D894(void)                          /* $D894 */
{
    LDY(byte_16);                            /* LDY byte_16 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_A8);                            /* STA byte_A8 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_B2);                            /* STA byte_B2 */
    sub_DA28();                              /* JMP sub_DA28 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D8A1  sub_D8A1 - level script case 2: set the script pointer
 *
 *  Original: src/bank7.asm:4633-4650 (bank 7)
 *      LDY byte_16 / LDA ($B5),Y / STA byte_B0 / INY / LDA ($B5),Y /
 *      STA byte_B1 / INY / LDA ($B5),Y / STA byte_6C / INY / LDA ($B5),Y /
 *      STA byte_6D / LDA #0 / STA byte_AF / STA byte_6E / JMP sub_DA28
 * ------------------------------------------------------------------------ */
void sub_D8A1(void)                          /* $D8A1 */
{
    LDY(byte_16);                            /* LDY byte_16 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_B0);                            /* STA byte_B0 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_B1);                            /* STA byte_B1 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_6C);                            /* STA byte_6C */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_6D);                            /* STA byte_6D */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_AF);                            /* STA byte_AF */
    STA(byte_6E);                            /* STA byte_6E */
    sub_DA28();                              /* JMP sub_DA28 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D8BF  sub_D8BF - level script case 3: switch to another level header
 *
 *  Original: src/bank7.asm:4656-4671 (bank 7)
 *      LDY byte_16 / LDA ($B5),Y / STA byte_18 / INY / LDA ($B5),Y /
 *      STA byte_19 / JSR loc_D714 / LDA byte_B3 / STA byte_A2 /
 *      LDA byte_B4 / STA byte_A3 / LDA #7 / STA byte_66 / JMP sub_D84A
 *
 *  "JSR loc_D714" enters sub_D6DF in the middle ($D714).  A C label is
 *  function scoped, so the block is reproduced here from its original
 *  statements; its "JMP switch_bank" returns to the instruction after the
 *  JSR, which is the LDA byte_B3 below.
 * ------------------------------------------------------------------------ */
void sub_D8BF(void)                          /* $D8BF */
{
    LDY(byte_16);                            /* LDY byte_16 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_18);                            /* STA byte_18 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_19);                            /* STA byte_19 */
    /* ---- $D714 loc_D714 - reproduced from sub_D6DF ---- */
    LDA(last_bank_num);                      /* LDA last_bank_num */
    PHA();                                   /* PHA */
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(byte_B3);                            /* STA byte_B3 */
    INY();                                   /* INY */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(byte_B4);                            /* STA byte_B4 */
    LDA(byte_18);                            /* LDA byte_18 */
    CLC();                                   /* CLC */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_B5);                            /* STA byte_B5 */
    LDA(byte_19);                            /* LDA byte_19 */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_B6);                            /* STA byte_B6 */
    LDA(byte_B3);                            /* LDA byte_B3 */
    SEC();                                   /* SEC */
    SBCI(1);                                 /* SBC #1 */
    STA(byte_A2);                            /* STA byte_A2 */
    LDA(byte_B4);                            /* LDA byte_B4 */
    SBCI(0);                                 /* SBC #0 */
    STA(byte_A3);                            /* STA byte_A3 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_AC);                            /* STA byte_AC */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_AD);                            /* STA byte_AD */
    PLA();                                   /* PLA */
    switch_bank();                           /* JMP switch_bank (returns to $D8D1) */
    /* ---- end of the reproduced $D714 block ---- */
    LDA(byte_B3);                            /* LDA byte_B3 */
    STA(byte_A2);                            /* STA byte_A2 */
    LDA(byte_B4);                            /* LDA byte_B4 */
    STA(byte_A3);                            /* STA byte_A3 */
    LDAI(7);                                 /* LDA #7 */
    STA(byte_66);                            /* STA byte_66 */
    sub_D84A();                              /* JMP sub_D84A */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D8DC  sub_D8DC - level script case 4: load a palette from the script
 *
 *  Original: src/bank7.asm:4677-4721 (bank 7)
 *      LDY byte_16 / LDA ($B5),Y / STA byte_18 / INY / LDA ($B5),Y /
 *      STA byte_19 / INY / LDA ($B5),Y / STY byte_16 / PHA / LDX #0 /
 *      LDY #$18 / LSR / BCS loc_D8FA / LDX #$C / LDY #$C / BNE loc_D8FF /
 *  loc_D8FA: LSR / BCS loc_D8FF / LDY #$C /
 *  loc_D8FF: STY byte_15 / LDY #0 / LDA ($18),Y / STA cram_new_palette /
 *      INY /
 *  loc_D909: LDA ($18),Y / STA $146,X / INY / INX / DEC byte_15 /
 *      BNE loc_D909 / PLA / ORA cram_update_mode / STA cram_update_mode /
 *      LDY byte_16 / JMP sub_DA28
 * ------------------------------------------------------------------------ */
void sub_D8DC(void)                          /* $D8DC */
{
    LDY(byte_16);                            /* LDY byte_16 */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_18);                            /* STA byte_18 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STA(byte_19);                            /* STA byte_19 */
    INY();                                   /* INY */
    LDA(INDY(byte_B5));                      /* LDA ($B5),Y */
    STY(byte_16);                            /* STY byte_16 */
    PHA();                                   /* PHA */
    LDXI(0);                                 /* LDX #0 */
    LDYI(0x18);                              /* LDY #$18 */
    LSR_A();                                 /* LSR */
    BCS(loc_D8FA);                           /* BCS loc_D8FA */
    LDXI(0x0C);                              /* LDX #$C */
    LDYI(0x0C);                              /* LDY #$C */
    BNE(loc_D8FF);                           /* BNE loc_D8FF */

loc_D8FA:                                    /* $D8FA */
    LSR_A();                                 /* LSR */
    BCS(loc_D8FF);                           /* BCS loc_D8FF */
    LDYI(0x0C);                              /* LDY #$C */

loc_D8FF:                                    /* $D8FF */
    STY(byte_15);                            /* STY byte_15 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(cram_new_palette);                   /* STA cram_new_palette */
    INY();                                   /* INY */

loc_D909:                                    /* $D909 */
    LDA(INDY(byte_18));                      /* LDA ($18),Y */
    STA(ABSX(0x0146));                       /* STA $146,X */
    INY();                                   /* INY */
    INX();                                   /* INX */
    DEC(byte_15);                            /* DEC byte_15 */
    BNE(loc_D909);                           /* BNE loc_D909 */
    PLA();                                   /* PLA */
    ORA(cram_update_mode);                   /* ORA cram_update_mode */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    LDY(byte_16);                            /* LDY byte_16 */
    sub_DA28();                              /* JMP sub_DA28 */
    RTS();
}
