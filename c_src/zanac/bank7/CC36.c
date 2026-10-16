/* bank7/CC36.c - bank 7, $CC36-$D2FC: the frame engine.
 *
 * Ported from src/bank7.asm lines 2222-3503 (addresses from Build/bank7.lst).
 *
 * This chunk is the heart of the port: the VBlank handler (nmi_handler), the
 * name-table updater it calls (ppu_update_background), the palette uploader
 * (ppu_cram_update), the frame limiter (vsync) and the pad/score helpers.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $CC36  continue_level_check - choose the level to play and store it
 *
 *  Original: src/bank7.asm:2222-2245 (entered by a BNE from start_game_check,
 *  which is why it is a separate procedure in the port too)
 *      JSR sub_CB40 / LDY byte_7D / LDA byte_199 / BEQ loc_CC50 /
 *      LDA pad_buttons_1 / ORA pad_buttons_2 / AND #8 / BNE loc_CC52 /
 *      CPY #11 / BCC loc_CC4E / LDY #10 / loc_CC4E: BNE loc_CC52 /
 *      loc_CC50: LDY #1 / loc_CC52: STY byte_7D / STY current_level / RTS
 * ------------------------------------------------------------------------ */
void continue_level_check(void)              /* $CC36 */
{
    sub_CB40();                              /* JSR sub_CB40 */
    LDY(byte_7D);                            /* LDY byte_7D */
    LDA(byte_199);                           /* LDA byte_199 */
    BEQ(loc_CC50);                           /* BEQ loc_CC50 */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ORA(pad_buttons_2);                      /* ORA pad_buttons_2 */
    ANDI(8);                                 /* AND #8 */
    BNE(loc_CC52);                           /* BNE loc_CC52 */
    CPYI(11);                                /* CPY #11 */
    BCC(loc_CC4E);                           /* BCC loc_CC4E */
    LDYI(10);                                /* LDY #10 */

loc_CC4E:                                    /* $CC4E */
    BNE(loc_CC52);                           /* BNE loc_CC52 */

loc_CC50:                                    /* $CC50 */
    LDYI(1);                                 /* LDY #1 */

loc_CC52:                                    /* $CC52 */
    STY(byte_7D);                            /* STY byte_7D */
    STY(current_level);                      /* STY current_level */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CC57  sub_CC57 - start a level: load its pointers and switch to bank 6
 *
 *  Original: src/bank7.asm:2252-2288
 *      LDA #0 / STA warp_sequence_counter / LDX #5 / LDY #7 /
 *      JSR mem_page_fill / LDA #$10 / STA byte_19 / LDA #0 / STA byte_18 /
 *      LDY current_level / LDA unk_CCAD,Y / LDY #4 / JSR sub_C8DF /
 *      JSR sub_C8BA / JSR switch_bank_5 / LDA current_level / ASL / TAY /
 *      LDA $8030,Y / STA byte_18 / LDA $8031,Y / STA byte_19 /
 *      LDY current_level / LDX unk_CCBA,Y / LDA $8030,X / STA byte_B8 /
 *      LDA $8031,X / STA byte_B9 / LDX unk_CCC7,Y / LDA $8030,X /
 *      STA byte_BA / LDA $8031,X / STA byte_BB / JSR switch_bank_6 /
 *      JSR sub_D6DF / JMP sub_D753
 * ------------------------------------------------------------------------ */
void sub_CC57(void)                          /* $CC57 */
{
    LDAI(0);                                 /* LDA #0 */
    STA(warp_sequence_counter);              /* STA warp_sequence_counter */
    LDXI(5);                                 /* LDX #5 */
    LDYI(7);                                 /* LDY #7 */
    mem_page_fill();                         /* JSR mem_page_fill */
    LDAI(0x10);                              /* LDA #$10 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_18);                            /* STA byte_18 */
    LDY(current_level);                      /* LDY current_level */
    LDA(ABSY(unk_CCAD));                     /* LDA unk_CCAD,Y */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */
    sub_C8BA();                              /* JSR sub_C8BA */
    switch_bank_5();                         /* JSR switch_bank_5 */
    LDA(current_level);                      /* LDA current_level */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(ABSY(0x8030));                       /* LDA $8030,Y (bank 5) */
    STA(byte_18);                            /* STA byte_18 */
    LDA(ABSY(0x8031));                       /* LDA $8031,Y */
    STA(byte_19);                            /* STA byte_19 */
    LDY(current_level);                      /* LDY current_level */
    LDX(ABSY(unk_CCBA));                     /* LDX unk_CCBA,Y */
    LDA(ABSX(0x8030));                       /* LDA $8030,X */
    STA(byte_B8);                            /* STA byte_B8 */
    LDA(ABSX(0x8031));                       /* LDA $8031,X */
    STA(byte_B9);                            /* STA byte_B9 */
    LDX(ABSY(unk_CCC7));                     /* LDX unk_CCC7,Y */
    LDA(ABSX(0x8030));                       /* LDA $8030,X */
    STA(byte_BA);                            /* STA byte_BA */
    LDA(ABSX(0x8031));                       /* LDA $8031,X */
    STA(byte_BB);                            /* STA byte_BB */
    switch_bank_6();                         /* JSR switch_bank_6 */
    sub_D6DF();                              /* JSR sub_D6DF */
    sub_D753();                              /* JMP sub_D753 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CCD4  clear_oam_buffer_unused - not referenced by anything
 *
 *  Original: src/bank7.asm:2335-2337
 *      JSR clear_oam_buffer / RTS
 * ------------------------------------------------------------------------ */
void clear_oam_buffer_unused(void)           /* $CCD4 */
{
    clear_oam_buffer();                      /* JSR clear_oam_buffer */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CCD8  sub_CCD8 - fill $0104-$0183 with A
 *
 *  Original: src/bank7.asm:2344-2352
 *      LDY #0 / loc_CCDA: STA $104,Y / INY / CPY #$80 / BNE loc_CCDA / RTS
 * ------------------------------------------------------------------------ */
void sub_CCD8(void)                          /* $CCD8 */
{
    LDYI(0);                                 /* LDY #0 */

loc_CCDA:                                    /* $CCDA */
    STA(ABSY(0x0104));                       /* STA $104,Y */
    INY();                                   /* INY */
    CPYI(0x80);                              /* CPY #$80 */
    BNE(loc_CCDA);                           /* BNE loc_CCDA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CCE3  vsync - wait for the next VBlank to start
 *
 *  Original: src/bank7.asm:2360-2373
 *      LDA byte_19B / BEQ loc_CCED / LDA vblank_wait_flag / STA byte_181 /
 *      loc_CCED: LDA vblank_wait_flag / BEQ loc_CCED / LDA #0 /
 *      STA vblank_wait_flag / RTS
 *
 *  The spin on vblank_wait_flag is what limits the game to one logic step per
 *  TV frame: the NMI handler increments the flag.  In the port the frame has to
 *  be advanced for that to happen, so the spin calls neslib_wait_vblank(),
 *  which runs the engine up to the next vblank and dispatches the ported NMI
 *  handler (this is the documented equivalent of the original busy-wait).
 * ------------------------------------------------------------------------ */
void vsync(void)                             /* $CCE3 */
{
    LDA(byte_19B);                           /* LDA byte_19B */
    BEQ(loc_CCED);                           /* BEQ loc_CCED */
    LDA(vblank_wait_flag);                   /* LDA vblank_wait_flag */
    STA(byte_181);                           /* STA byte_181 */

loc_CCED:                                    /* $CCED */
    LDA(vblank_wait_flag);                   /* LDA vblank_wait_flag */
    BNE(loc_CCF2);                           /* BNE (the NMI ran) */
    neslib_wait_vblank();                    /* BEQ loc_CCED, via the engine */
    goto loc_CCED;

loc_CCF2:                                    /* $CCF2 */
    LDAI(0);                                 /* LDA #0 */
    STA(vblank_wait_flag);                   /* STA vblank_wait_flag */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CCF6  handle_pad_buttons - apply the pad state to the ship direction
 *
 *  Original: src/bank7.asm:2381-2423 (in DEMO mode the pad state is faked
 *  from demo_buttons_tab)
 *      STX byte_10 / LDA game_mode / BPL loc_CD2C / DEC demo_counter_1 /
 *      BNE loc_CD07 / DEC demo_counter_2 / BNE loc_CD07 / JMP main_menu_loop /
 *      loc_CD07: DEC demo_button_update_counter / BNE loc_CD22 /
 *      JSR randomize / AND #$F / ADC #$18 / STA demo_button_update_counter /
 *      LDA demo_button_tab_pointer / AND #$1F / STA demo_button_tab_pointer /
 *      INC demo_button_tab_pointer / TAY / LDA demo_buttons_tab,Y /
 *      STA demo_last_buttons / loc_CD22: LDA demo_last_buttons /
 *      STA pad_buttons_1 / STA pad_buttons_2 / STA pad_test1 /
 *      STA pad_test2 / loc_CD2C: LDA pad_buttons_1 / ORA pad_buttons_2 /
 *      AND #$F / TAY / LDA pmove_direction_tab,Y / STA pmove_direction /
 *      LDX byte_10 / RTS
 * ------------------------------------------------------------------------ */
void handle_pad_buttons(void)                /* $CCF6 */
{
    STX(byte_10);                            /* STX byte_10 */
    LDA(game_mode);                          /* LDA game_mode */
    BPL(loc_CD2C);                           /* BPL loc_CD2C */
    DEC(demo_counter_1);                     /* DEC demo_counter_1 */
    BNE(loc_CD07);                           /* BNE loc_CD07 */
    DEC(demo_counter_2);                     /* DEC demo_counter_2 */
    BNE(loc_CD07);                           /* BNE loc_CD07 */
    main_menu_loop();                        /* JMP main_menu_loop */
    RTS();

loc_CD07:                                    /* $CD07 */
    DEC(demo_button_update_counter);         /* DEC demo_button_update_counter */
    BNE(loc_CD22);                           /* BNE loc_CD22 */
    randomize();                             /* JSR randomize */
    ANDI(0x0F);                              /* AND #$F */
    ADCI(0x18);                              /* ADC #$18 */
    STA(demo_button_update_counter);         /* STA demo_button_update_counter */
    LDA(demo_button_tab_pointer);            /* LDA demo_button_tab_pointer */
    ANDI(0x1F);                              /* AND #$1F */
    STA(demo_button_tab_pointer);            /* STA demo_button_tab_pointer */
    INC(demo_button_tab_pointer);            /* INC demo_button_tab_pointer */
    TAY();                                   /* TAY */
    LDA(ABSY(demo_buttons_tab));             /* LDA demo_buttons_tab,Y */
    STA(demo_last_buttons);                  /* STA demo_last_buttons */

loc_CD22:                                    /* $CD22 */
    LDA(demo_last_buttons);                  /* LDA demo_last_buttons */
    STA(pad_buttons_1);                      /* STA pad_buttons_1 */
    STA(pad_buttons_2);                      /* STA pad_buttons_2 */
    STA(pad_test1);                          /* STA pad_test1 */
    STA(pad_test2);                          /* STA pad_test2 */

loc_CD2C:                                    /* $CD2C */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ORA(pad_buttons_2);                      /* ORA pad_buttons_2 */
    ANDI(0x0F);                              /* AND #$F */
    TAY();                                   /* TAY */
    LDA(ABSY(pmove_direction_tab));          /* LDA pmove_direction_tab,Y */
    STA(pmove_direction);                    /* STA pmove_direction */
    LDX(byte_10);                            /* LDX byte_10 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CD6B  sub_CD6B - decompress a tile string into the $460/$470 buffers
 *
 *  Original: src/bank7.asm:2436-2622.  This is the routine that turns the
 *  level's compressed name-table data into the buffers the VBlank handler
 *  copies to VRAM ($460 = a buffered row, $470 = the visible name-table part).
 * ------------------------------------------------------------------------ */
void sub_CD6B(void)                          /* $CD6B */
{
    LDA(last_bank_num);                      /* LDA last_bank_num */
    PHA();                                   /* PHA */
    LDA(byte_26);                            /* LDA byte_26 */
    switch_bank();                           /* JSR switch_bank */
    LDA(byte_2B);                            /* LDA byte_2B */
    LSR_A();                                 /* LSR */
    BCS(loc_CD7E);                           /* BCS loc_CD7E */
    LSR_A();                                 /* LSR */
    BCC(loc_CD8F);                           /* BCC loc_CD8F */
    sub_CE72();                              /* JSR sub_CE72 */

loc_CD7E:                                    /* $CD7E */
    LDA(byte_9C);                            /* LDA byte_9C */
    BEQ(loc_CD86);                           /* BEQ loc_CD86 */
    LDAI(0x0D);                              /* LDA #$D */
    BNE(loc_CD88);                           /* BNE loc_CD88 */

loc_CD86:                                    /* $CD86 */
    LDAI(0x4D);                              /* LDA #$4D */

loc_CD88:                                    /* $CD88 */
    SEC();                                   /* SEC */
    SBC(byte_21);                            /* SBC byte_21 */
    BEQ(loc_CD8F);                           /* BEQ loc_CD8F */
    BCS(loc_CD92);                           /* BCS loc_CD92 */

loc_CD8F:                                    /* $CD8F */
    goto loc_CE6E;                           /* JMP loc_CE6E */

loc_CD92:                                    /* $CD92 */
    PHA();                                   /* PHA */
    CLC();                                   /* CLC */
    ADCI(3);                                 /* ADC #3 */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAX();                                   /* TAX */
    PLA();                                   /* PLA */
    STA(byte_12);                            /* STA byte_12 */
    STA(byte_13);                            /* STA byte_13 */
    LDYI(0);                                 /* LDY #0 */
    LDA(byte_2B);                            /* LDA byte_2B */
    ANDI(0x20);                              /* AND #$20 */
    BNE(loc_CE21);                           /* BNE loc_CE21 */

loc_CDA7:                                    /* $CDA7 */
    LDA(INDY(byte_2C));                      /* LDA ($2C),Y */
    INY();                                   /* INY */
    CMP(byte_29);                            /* CMP byte_29 */
    BNE(loc_CDC6);                           /* BNE loc_CDC6 */
    LDA(byte_2B);                            /* LDA byte_2B */
    EORI(0x80);                              /* EOR #$80 */
    STA(byte_2B);                            /* STA byte_2B */
    LDA(INDY(byte_2C));                      /* LDA ($2C),Y */
    CMP(byte_29);                            /* CMP byte_29 */
    BNE(loc_CDA7);                           /* BNE loc_CDA7 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_2B);                            /* STA byte_2B */
    LDA(byte_13);                            /* LDA byte_13 */
    SEC();                                   /* SEC */
    SBC(byte_12);                            /* SBC byte_12 */
    goto loc_CE40;                           /* JMP loc_CE40 */

loc_CDC6:                                    /* $CDC6 */
    CMP(byte_2A);                            /* CMP byte_2A */
    BNE(loc_CDE7);                           /* BNE loc_CDE7 */
    LDA(byte_2B);                            /* LDA byte_2B */
    EORI(0x40);                              /* EOR #$40 */
    STA(byte_2B);                            /* STA byte_2B */
    ANDI(0x40);                              /* AND #$40 */
    BEQ(loc_CDE3);                           /* BEQ loc_CDE3 */
    LDA(INDY(byte_2C));                      /* LDA ($2C),Y */
    PHA();                                   /* PHA */
    INY();                                   /* INY */
    LDA(INDY(byte_2C));                      /* LDA ($2C),Y */
    STA(byte_2D);                            /* STA byte_2D */
    PLA();                                   /* PLA */
    STA(byte_2C);                            /* STA byte_2C */
    LDYI(0);                                 /* LDY #0 */
    BEQ(loc_CDA7);                           /* BEQ loc_CDA7 */

loc_CDE3:                                    /* $CDE3 */
    INY();                                   /* INY */
    INY();                                   /* INY */
    BNE(loc_CDA7);                           /* BNE loc_CDA7 */

loc_CDE7:                                    /* $CDE7 */
    BIT(byte_2B);                            /* BIT byte_2B */
    BVC(loc_CE05);                           /* BVC loc_CE05 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    ASL_A();                                 /* ASL */
    ROR(byte_10);                            /* ROR byte_10 */
    LDA(byte_10);                            /* LDA byte_10 */

loc_CE05:                                    /* $CE05 */
    BIT(byte_2B);                            /* BIT byte_2B */
    BMI(loc_CE14);                           /* BMI loc_CE14 */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    DEC(byte_12);                            /* DEC byte_12 */
    BEQ(loc_CE3E);                           /* BEQ loc_CE3E */
    goto loc_CDA7;                           /* JMP loc_CDA7 */

loc_CE14:                                    /* $CE14 */
    STA(byte_27);                            /* STA byte_27 */
    LDA(byte_2B);                            /* LDA byte_2B */
    EORI(0x20);                              /* EOR #$20 */
    STA(byte_2B);                            /* STA byte_2B */
    LDA(INDY(byte_2C));                      /* LDA ($2C),Y */
    INY();                                   /* INY */
    STA(byte_28);                            /* STA byte_28 */

loc_CE21:                                    /* $CE21 */
    LDA(byte_27);                            /* LDA byte_27 */

loc_CE23:                                    /* $CE23 */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    DEC(byte_28);                            /* DEC byte_28 */
    BEQ(loc_CE31);                           /* BEQ loc_CE31 */
    DEC(byte_12);                            /* DEC byte_12 */
    BNE(loc_CE23);                           /* BNE loc_CE23 */
    BEQ(loc_CE3E);                           /* BEQ loc_CE3E */

loc_CE31:                                    /* $CE31 */
    LDA(byte_2B);                            /* LDA byte_2B */
    EORI(0x20);                              /* EOR #$20 */
    STA(byte_2B);                            /* STA byte_2B */
    DEC(byte_12);                            /* DEC byte_12 */
    BEQ(loc_CE3E);                           /* BEQ loc_CE3E */
    goto loc_CDA7;                           /* JMP loc_CDA7 */

loc_CE3E:                                    /* $CE3E */
    LDA(byte_13);                            /* LDA byte_13 */

loc_CE40:                                    /* $CE40 */
    LDX(byte_21);                            /* LDX byte_21 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    CLC();                                   /* CLC */
    ADCI(3);                                 /* ADC #3 */
    STA(byte_25);                            /* STA byte_25 */
    LDA(byte_2F);                            /* LDA byte_2F */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(byte_2E);                            /* LDA byte_2E */
    STA(ABSX(0x0462));                       /* STA $462,X */
    LDA(byte_13);                            /* LDA byte_13 */
    PHA();                                   /* PHA */
    sub_C807();                              /* JSR sub_C807 */
    PLA();                                   /* PLA */
    CLC();                                   /* CLC */
    ADC(byte_2E);                            /* ADC byte_2E */
    STA(byte_2E);                            /* STA byte_2E */
    BCC(loc_CE64);                           /* BCC loc_CE64 */
    INC(byte_2F);                            /* INC byte_2F */

loc_CE64:                                    /* $CE64 */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADC(byte_2C);                            /* ADC byte_2C */
    STA(byte_2C);                            /* STA byte_2C */
    BCC(loc_CE6E);                           /* BCC loc_CE6E */
    INC(byte_2D);                            /* INC byte_2D */

loc_CE6E:                                    /* $CE6E */
    PLA();                                   /* PLA */
    switch_bank();                           /* JMP switch_bank */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CE72  sub_CE72 - read the two escape bytes of the string
 *
 *  Original: src/bank7.asm:2629-2646
 *      LDY #0 / LDA ($2C),Y / STA byte_29 / INY / LDA ($2C),Y / STA byte_2A /
 *      LDA #1 / STA byte_2B / LDA byte_2C / CLC / ADC #2 / STA byte_2C /
 *      BCC locret_CE8C / INC byte_2D / RTS
 * ------------------------------------------------------------------------ */
void sub_CE72(void)                          /* $CE72 */
{
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_2C));                      /* LDA ($2C),Y */
    STA(byte_29);                            /* STA byte_29 */
    INY();                                   /* INY */
    LDA(INDY(byte_2C));                      /* LDA ($2C),Y */
    STA(byte_2A);                            /* STA byte_2A */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_2B);                            /* STA byte_2B */
    LDA(byte_2C);                            /* LDA byte_2C */
    CLC();                                   /* CLC */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_2C);                            /* STA byte_2C */
    BCC(locret_CE8C);                        /* BCC locret_CE8C */
    INC(byte_2D);                            /* INC byte_2D */

locret_CE8C:                                 /* $CE8C */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CE8D  nmi_handler - the VBlank callback of the engine
 *
 *  Original: src/bank7.asm:2653-2683
 *      PHA / TXA / PHA / TYA / PHA / LDA last_ppu_ctrl2 / PHA / AND #$E7 /
 *      STA last_ppu_ctrl2 / STA PPU_CTRL_REG2 / INC vblank_wait_flag /
 *      LDA PPU_STATUS / STA ppu_status_in_vblank / LDA #0 / STA PPU_SPR_ADDR /
 *      LDA #2 / STA PPU_SPR_DMA / JSR ppu_update_background /
 *      JSR ppu_cram_update / JSR ppu_update_scroll_regs / PLA /
 *      STA last_ppu_ctrl2 / STA PPU_CTRL_REG2 / JSR apu_update /
 *      PLA / TAY / PLA / TAX / PLA
 *  (falls through into dummy_irq_handler's RTI)
 *
 *  The first store disables BG and sprites (forced blank), so the whole
 *  name-table and palette update happens with rendering off and the picture is
 *  restored before the callback returns.
 * ------------------------------------------------------------------------ */
void nmi_handler(void)                       /* $CE8D */
{
    PHA();                                   /* PHA */
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDA(last_ppu_ctrl2);                     /* LDA last_ppu_ctrl2 */
    PHA();                                   /* PHA */
    ANDI(0xE7);                              /* AND #$E7 (forced blank) */
    STA(last_ppu_ctrl2);                     /* STA last_ppu_ctrl2 */
    STA(PPU_CTRL_REG2);                      /* STA PPU_CTRL_REG2 */
    INC(vblank_wait_flag);                   /* INC vblank_wait_flag */
    LDA(PPU_STATUS);                         /* LDA PPU_STATUS */
    STA(ppu_status_in_vblank);               /* STA ppu_status_in_vblank */
    LDAI(0);                                 /* LDA #0 */
    STA(PPU_SPR_ADDR);                       /* STA PPU_SPR_ADDR */
    LDAI(2);                                 /* LDA #2 (OAM DMA from page 2) */
    STA(PPU_SPR_DMA);                        /* STA PPU_SPR_DMA */
    ppu_update_background();                 /* JSR ppu_update_background */
    ppu_cram_update();                       /* JSR ppu_cram_update */
    ppu_update_scroll_regs();                /* JSR ppu_update_scroll_regs */
    PLA();                                   /* PLA */
    STA(last_ppu_ctrl2);                     /* STA last_ppu_ctrl2 */
    STA(PPU_CTRL_REG2);                      /* STA PPU_CTRL_REG2 */
    apu_update();                            /* JSR apu_update */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    PLA();                                   /* PLA */
    /* falls through into dummy_irq_handler ($CEC4): RTI */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $CEC4  dummy_irq_handler - the unused IRQ vector, an RTI
 * ------------------------------------------------------------------------ */
void dummy_irq_handler(void)                 /* $CEC4 */
{
    RTS();                                   /* RTI */
}

/* ------------------------------------------------------------------------ *
 *  $CEC5  ppu_update_background - upload the buffered name-table rows
 *
 *  Original: src/bank7.asm:2698-2890.  Called from the VBlank handler (and from
 *  sub_C8BA while waiting for a scroll window): copies the row buffers the
 *  level decompressor fills ($104 for the attribute rows and $470 for the tile
 *  rows) into VRAM through $2006/$2007.
 * ------------------------------------------------------------------------ */
void ppu_update_background(void)             /* $CEC5 */
{
    LDA(byte_90);                            /* LDA byte_90 */
    BPL(loc_CECC);                           /* BPL loc_CECC */
    sub_C6B5();                              /* JSR sub_C6B5 */

loc_CECC:                                    /* $CECC */
    LDA(byte_9C);                            /* LDA byte_9C */
    BNE(loc_CED3);                           /* BNE loc_CED3 */
    goto loc_CF70;                           /* JMP loc_CF70 */

loc_CED3:                                    /* $CED3 */
    CMPI(1);                                 /* CMP #1 */
    BNE(loc_CF27);                           /* BNE loc_CF27 */
    INC(byte_9C);                            /* INC byte_9C */
    LDA(byte_A0);                            /* LDA byte_A0 */
    ORAI(3);                                 /* ORA #3 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(byte_9E);                            /* LDA byte_9E */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDY(byte_9D);                            /* LDY byte_9D */
    LDA(ABSY(0x0104));                       /* LDA $104,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0105));                       /* LDA $105,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0106));                       /* LDA $106,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0107));                       /* LDA $107,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0108));                       /* LDA $108,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0109));                       /* LDA $109,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x010A));                       /* LDA $10A,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x010B));                       /* LDA $10B,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(byte_A0);                            /* LDA byte_A0 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(byte_9F);                            /* LDA byte_9F */
    ORAI(0x20);                              /* ORA #$20 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDYI(0x20);                              /* LDY #$20 */
    BNE(loc_CF37);                           /* BNE loc_CF37 */

loc_CF27:                                    /* $CF27 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_9C);                            /* STA byte_9C */
    LDA(byte_A0);                            /* LDA byte_A0 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(byte_9F);                            /* LDA byte_9F */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDYI(0);                                 /* LDY #0 */

loc_CF37:                                    /* $CF37 */
    CLC();                                   /* CLC */

loc_CF38:                                    /* $CF38 */
    LDA(ABSY(0x0470));                       /* LDA $470,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0471));                       /* LDA $471,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0472));                       /* LDA $472,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0473));                       /* LDA $473,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0474));                       /* LDA $474,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0475));                       /* LDA $475,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0476));                       /* LDA $476,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0477));                       /* LDA $477,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    TYA();                                   /* TYA */
    ADCI(8);                                 /* ADC #8 */
    TAY();                                   /* TAY */
    ANDI(0x1F);                              /* AND #$1F */
    BNE(loc_CF38);                           /* BNE loc_CF38 */

loc_CF70:                                    /* $CF70 */
    LDA(byte_20);                            /* LDA byte_20 */
    BNE(loc_CF75);                           /* BNE loc_CF75 */
    RTS();

loc_CF75:                                    /* $CF75 */
    LDXI(0);                                 /* LDX #0 */
    STX(byte_21);                            /* STX byte_21 */

loc_CF79:                                    /* $CF79 */
    LDA(ABSX(0x0460));                       /* LDA $460,X */
    INX();                                   /* INX */
    TAY();                                   /* TAY */
    BPL(loc_CFDE);                           /* BPL loc_CFDE */
    ANDI(0x7F);                              /* AND #$7F */
    TAY();                                   /* TAY */
    ASL_A();                                 /* ASL */
    BPL(loc_CFD7);                           /* BPL loc_CFD7 */
    ASL_A();                                 /* ASL */
    BPL(loc_CF8C);                           /* BPL loc_CF8C */
    goto loc_CFFE;                           /* JMP loc_CFFE */

loc_CF8C:                                    /* $CF8C */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ADCI(0x20);                              /* ADC #$20 */
    TAY();                                   /* TAY */
    ORAI(3);                                 /* ORA #3 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(ABSX(0x0465));                       /* LDA $465,X */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(ABSX(0x0466));                       /* LDA $466,X */
    STA(PPU_DATA);                           /* STA PPU_DATA */

loc_CFA2:                                    /* $CFA2 */
    STY(PPU_ADDRESS);                        /* STY PPU_ADDRESS */
    LDA(ABSX(0x0460));                       /* LDA $460,X */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(ABSX(0x0461));                       /* LDA $461,X */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSX(0x0462));                       /* LDA $462,X */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    STY(PPU_ADDRESS);                        /* STY PPU_ADDRESS */
    LDA(ABSX(0x0460));                       /* LDA $460,X */
    ORAI(0x20);                              /* ORA #$20 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(ABSX(0x0463));                       /* LDA $463,X */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSX(0x0464));                       /* LDA $464,X */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    TXA();                                   /* TXA */
    ADCI(7);                                 /* ADC #7 */
    TAX();                                   /* TAX */

loc_CFD2:                                    /* $CFD2 */
    DEC(byte_20);                            /* DEC byte_20 */
    BNE(loc_CF79);                           /* BNE loc_CF79 */
    RTS();

loc_CFD7:                                    /* $CFD7 */
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    ORAI(4);                                 /* ORA #4 */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */

loc_CFDE:                                    /* $CFDE */
    LDA(ABSX(0x0460));                       /* LDA $460,X */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    INX();                                   /* INX */
    LDA(ABSX(0x0460));                       /* LDA $460,X */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    INX();                                   /* INX */

loc_CFEC:                                    /* $CFEC */
    LDA(ABSX(0x0460));                       /* LDA $460,X */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    INX();                                   /* INX */
    DEY();                                   /* DEY */
    BNE(loc_CFEC);                           /* BNE loc_CFEC */
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */
    goto loc_CFD2;                           /* JMP loc_CFD2 */

loc_CFFE:                                    /* $CFFE */
    LSR_A();                                 /* LSR */
    LSR_A();                                 /* LSR */
    ANDI(0x0F);                              /* AND #$F */
    ADCI(0x20);                              /* ADC #$20 */
    PHA();                                   /* PHA */
    ORAI(3);                                 /* ORA #3 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    TAY();                                   /* TAY */
    LDA(ABSX(0x0465));                       /* LDA $465,X */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(PPU_DATA);                           /* LDA PPU_DATA */
    LDA(PPU_DATA);                           /* LDA PPU_DATA */
    AND(ABSX(0x0467));                       /* AND $467,X */
    ORA(ABSX(0x0466));                       /* ORA $466,X */
    PHA();                                   /* PHA */
    STY(PPU_ADDRESS);                        /* STY PPU_ADDRESS */
    LDA(ABSX(0x0465));                       /* LDA $465,X */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    PLA();                                   /* PLA */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    SEC();                                   /* SEC */
    goto loc_CFA2;                           /* JMP loc_CFA2 */
}

/* ------------------------------------------------------------------------ *
 *  $D031  ppu_cram_update - upload the palette (CRAM)
 *
 *  Original: src/bank7.asm:2897-2954.  cram_update_mode selects a full upload
 *  ($145 holds a fresh 25-byte palette) or one of the partial modes.
 * ------------------------------------------------------------------------ */
void ppu_cram_update(void)                   /* $D031 */
{
    LDA(cram_update_mode);                   /* LDA cram_update_mode */
    BEQ(locret_D0A3);                        /* BEQ locret_D0A3 */
    CMPI(2);                                 /* CMP #2 */
    BEQ(loc_D04C);                           /* BEQ loc_D04C */
    LDYI(0);                                 /* LDY #0 */
    LDXI(0x3F);                              /* LDX #$3F */
    STX(PPU_ADDRESS);                        /* STX PPU_ADDRESS */
    STY(PPU_ADDRESS);                        /* STY PPU_ADDRESS */
    LDXI(4);                                 /* LDX #4 */
    CMPI(3);                                 /* CMP #3 */
    BEQ(loc_D05A);                           /* BEQ loc_D05A */
    BNE(loc_D058);                           /* BNE loc_D058 */

loc_D04C:                                    /* $D04C */
    LDAI(0x3F);                              /* LDA #$3F */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDAI(0x10);                              /* LDA #$10 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDYI(0x0C);                              /* LDY #$C */

loc_D058:                                    /* $D058 */
    LDXI(2);                                 /* LDX #2 */

loc_D05A:                                    /* $D05A */
    CLC();                                   /* CLC */

loc_D05B:                                    /* $D05B */
    LDA(cram_new_palette);                   /* LDA cram_new_palette */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0146));                       /* LDA $146,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0147));                       /* LDA $147,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0148));                       /* LDA $148,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(cram_new_palette);                   /* LDA cram_new_palette */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x0149));                       /* LDA $149,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x014A));                       /* LDA $14A,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    LDA(ABSY(0x014B));                       /* LDA $14B,Y */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    TYA();                                   /* TYA */
    ADCI(6);                                 /* ADC #6 */
    TAY();                                   /* TAY */
    DEX();                                   /* DEX */
    BNE(loc_D05B);                           /* BNE loc_D05B */
    LDAI(0x3F);                              /* LDA #$3F */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    STX(PPU_ADDRESS);                        /* STX PPU_ADDRESS */
    STX(PPU_ADDRESS);                        /* STX PPU_ADDRESS */
    STX(PPU_ADDRESS);                        /* STX PPU_ADDRESS */
    STX(cram_update_mode);                   /* STX cram_update_mode */

locret_D0A3:                                 /* $D0A3 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D0A4  sub_D0A4 - advance the game mode (demo/live) by one
 *
 *  Original: src/bank7.asm:2961-2984
 *      LDA game_mode / AND #$FE / CMP game_mode / BEQ locret_D0BE /
 *      LDY byte_32 / BNE loc_D0BA / ORA #2 / STA game_mode / BPL locret_D0B9 /
 *      JMP main_menu_loop / locret_D0B9: RTS / loc_D0BA: ORA #$40 /
 *      STA game_mode / locret_D0BE: RTS
 * ------------------------------------------------------------------------ */
void sub_D0A4(void)                          /* $D0A4 */
{
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0xFE);                              /* AND #$FE */
    CMP(game_mode);                          /* CMP game_mode */
    BEQ(locret_D0BE);                        /* BEQ locret_D0BE */
    LDY(byte_32);                            /* LDY byte_32 */
    BNE(loc_D0BA);                           /* BNE loc_D0BA */
    ORAI(2);                                 /* ORA #2 */
    STA(game_mode);                          /* STA game_mode */
    BPL(locret_D0B9);                        /* BPL locret_D0B9 */
    main_menu_loop();                        /* JMP main_menu_loop */
    RTS();

locret_D0B9:                                 /* $D0B9 */
    RTS();

loc_D0BA:                                    /* $D0BA */
    ORAI(0x40);                              /* ORA #$40 */
    STA(game_mode);                          /* STA game_mode */

locret_D0BE:                                 /* $D0BE */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D0BF  sub_D0BF - the "game over / continue?" loop
 *
 *  Original: src/bank7.asm:2991-3030
 *      LDA game_mode / AND #2 / BEQ locret_D0BE / LDA #9 / STA byte_66 /
 *      JSR set_new_high_score / JSR apu_stop / LDY #4 / LDA pad_buttons_1 /
 *      AND #$20 / BEQ loc_D0D9 / LDY #$23 / loc_D0D9: TYA / JSR apu_play /
 *      PLA / PLA / LDA #3 / STA byte_6F / LDA #0 / STA byte_300 /
 *      loc_D0E8: JSR sub_D7E3 / JSR vsync / JSR pad_read /
 *      JSR j_game_logic_processing / JSR clear_first_last_sprites /
 *      LDA byte_300 / BEQ loc_D0FF / JMP main_menu_loop /
 *      loc_D0FF: LDA pad_test1 / AND #$10 / BEQ loc_D0E8 /
 *      JMP main_menu_loop
 *
 *  The two PLAs discard the return address of the JSR that entered this
 *  routine (it never returns normally: it ends with JMP main_menu_loop).  A C
 *  call leaves nothing on the emulated stack, so they are omitted here; doing
 *  them would unbalance the stack pointer used by PHA/PLA elsewhere.
 * ------------------------------------------------------------------------ */
void sub_D0BF(void)                          /* $D0BF */
{
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(2);                                 /* AND #2 */
    BEQ(locret_D0BE);                        /* BEQ locret_D0BE */
    LDAI(9);                                 /* LDA #9 */
    STA(byte_66);                            /* STA byte_66 */
    set_new_high_score();                    /* JSR set_new_high_score */
    apu_stop();                              /* JSR apu_stop */
    LDYI(4);                                 /* LDY #4 */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_D0D9);                           /* BEQ loc_D0D9 */
    LDYI(0x23);                              /* LDY #$23 */

loc_D0D9:                                    /* $D0D9 */
    TYA();                                   /* TYA */
    apu_play();                              /* JSR apu_play */
    /* PLA / PLA - see the note above: the caller's return address. */
    LDAI(3);                                 /* LDA #3 */
    STA(byte_6F);                            /* STA byte_6F */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_300);                           /* STA byte_300 */

loc_D0E8:                                    /* $D0E8 */
    sub_D7E3();                              /* JSR sub_D7E3 */
    vsync();                                 /* JSR vsync */
    pad_read();                              /* JSR pad_read */
    j_game_logic_processing();               /* JSR j_game_logic_processing */
    clear_first_last_sprites();              /* JSR clear_first_last_sprites */
    LDA(byte_300);                           /* LDA byte_300 */
    BEQ(loc_D0FF);                           /* BEQ loc_D0FF */
    main_menu_loop();                        /* JMP main_menu_loop */
    RTS();

loc_D0FF:                                    /* $D0FF */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_D0E8);                           /* BEQ loc_D0E8 */
    main_menu_loop();                        /* JMP main_menu_loop */

locret_D0BE:                                 /* $D0BE (shared RTS of sub_D0A4) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D108  check_high_score_reached - compare the score with the top score
 *
 *  Original: src/bank7.asm:3037-3060
 *      BIT byte_39 / BMI locret_D12B / BVS locret_D12B / LDY #7 / SEC /
 *      loc_D111: LDA $179,Y / SBC $181,Y / DEY / BNE loc_D111 /
 *      BCC locret_D12B / LDA #$40 / STA byte_39 / LDA game_mode / AND #4 /
 *      BNE locret_D12B / LDA #9 / JSR apu_play / locret_D12B: RTS
 * ------------------------------------------------------------------------ */
void check_high_score_reached(void)          /* $D108 */
{
    BIT(byte_39);                            /* BIT byte_39 */
    BMI(locret_D12B);                        /* BMI locret_D12B */
    BVS(locret_D12B);                        /* BVS locret_D12B */
    LDYI(7);                                 /* LDY #7 */
    SEC();                                   /* SEC */

loc_D111:                                    /* $D111 */
    LDA(ABSY(0x0179));                       /* LDA $179,Y (score) */
    SBC(ABSY(0x0181));                       /* SBC $181,Y (top score) */
    DEY();                                   /* DEY */
    BNE(loc_D111);                           /* BNE loc_D111 */
    BCC(locret_D12B);                        /* BCC locret_D12B */
    LDAI(0x40);                              /* LDA #$40 */
    STA(byte_39);                            /* STA byte_39 */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(4);                                 /* AND #4 */
    BNE(locret_D12B);                        /* BNE locret_D12B */
    LDAI(9);                                 /* LDA #9 (high score sound) */
    apu_play();                              /* JSR apu_play */

locret_D12B:                                 /* $D12B */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D12C  sub_D12C - award an extra life
 *
 *  Original: src/bank7.asm:3067-3129 (entered by sub_D193 and loops on
 *  itself with JMP sub_D12C while the score keeps qualifying)
 * ------------------------------------------------------------------------ */
void sub_D12C(void)                          /* $D12C */
{
loc_D12C:                                    /* $D12C */
    LDA(byte_32);                            /* LDA byte_32 */
    BEQ(locret_D12B);                        /* BEQ locret_D12B (shared RTS) */
    LDYI(4);                                 /* LDY #4 */
    SEC();                                   /* SEC */

loc_D133:                                    /* $D133 */
    LDA(ABSY(0x0179));                       /* LDA $179,Y */
    SBC(ABSY(0x0175));                       /* SBC $175,Y */
    DEY();                                   /* DEY */
    BNE(loc_D133);                           /* BNE loc_D133 */
    BCC(locret_D12B);                        /* BCC locret_D12B */
    LDA(byte_176);                           /* LDA byte_176 */
    BNE(loc_D15B);                           /* BNE loc_D15B */
    LDA(byte_177);                           /* LDA byte_177 */
    BNE(loc_D15B);                           /* BNE loc_D15B */
    LDA(byte_178);                           /* LDA byte_178 */
    BNE(loc_D15B);                           /* BNE loc_D15B */
    LDA(byte_179);                           /* LDA byte_179 */
    CMPI(2);                                 /* CMP #2 */
    BNE(loc_D15B);                           /* BNE loc_D15B */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_179);                           /* STA byte_179 */
    BNE(loc_D177);                           /* BNE loc_D177 */

loc_D15B:                                    /* $D15B */
    CLC();                                   /* CLC */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_10);                            /* STA byte_10 */
    LDYI(4);                                 /* LDY #4 */

loc_D162:                                    /* $D162 */
    LDA(ABSY(0x0175));                       /* LDA $175,Y */
    ADC(byte_10);                            /* ADC byte_10 */
    CMPI(0x0A);                              /* CMP #$A */
    BCC(loc_D16D);                           /* BCC loc_D16D */
    SBCI(0x0A);                              /* SBC #$A */

loc_D16D:                                    /* $D16D */
    STA(ABSY(0x0175));                       /* STA $175,Y */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_10);                            /* STA byte_10 */
    DEY();                                   /* DEY */
    BNE(loc_D162);                           /* BNE loc_D162 */

loc_D177:                                    /* $D177 */
    INC(byte_32);                            /* INC byte_32 */
    BNE(loc_D17E);                           /* BNE loc_D17E */
    DEC(byte_32);                            /* DEC byte_32 */
    RTS();

loc_D17E:                                    /* $D17E */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(4);                                 /* AND #4 */
    BNE(loc_D189);                           /* BNE loc_D189 */
    LDAI(8);                                 /* LDA #8 (+1 life sound) */
    apu_play();                              /* JSR apu_play */

loc_D189:                                    /* $D189 */
    goto loc_D12C;                           /* JMP sub_D12C */

locret_D12B:                                 /* $D12B (shared RTS of check_high_score_reached) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D18C  sub_D18C - award score from a table entry
 *
 *  Original: src/bank7.asm:3136-3139 (falls through into sub_D193)
 *      LDA $7E6,X / TAY / LDA unk_D1E1,Y
 * ------------------------------------------------------------------------ */
void sub_D18C(void)                          /* $D18C */
{
    LDA(ABSX(0x07E6));                       /* LDA $7E6,X */
    TAY();                                   /* TAY */
    LDA(ABSY(unk_D1E1));                     /* LDA unk_D1E1,Y */
    sub_D193();                              /* falls through into sub_D193 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D193  sub_D193 - add A points to the score
 *
 *  Original: src/bank7.asm:3146-3154
 *      STA byte_10 / LDA game_mode / AND #$80 / BNE locret_D1E0 /
 *      LDA byte_10 / JSR sub_D1A6 / JSR check_high_score_reached /
 *      JMP sub_D12C
 * ------------------------------------------------------------------------ */
void sub_D193(void)                          /* $D193 */
{
    STA(byte_10);                            /* STA byte_10 */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0x80);                              /* AND #$80 (demo mode?) */
    BNE(locret_D1E0);                        /* BNE locret_D1E0 */
    LDA(byte_10);                            /* LDA byte_10 */
    sub_D1A6();                              /* JSR sub_D1A6 */
    check_high_score_reached();              /* JSR check_high_score_reached */
    sub_D12C();                              /* JMP sub_D12C */
    RTS();

locret_D1E0:                                 /* $D1E0 (shared RTS of sub_D1A6) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D1A6  sub_D1A6 - add A to the 6-digit BCD score at $17B
 *
 *  Original: src/bank7.asm:3161-3203
 *      STA byte_10 / ASL / CLC / ADC byte_10 / ASL / ADC #6 / TAY /
 *      STX byte_10 / LDX #5 / CLC /
 *      loc_D1B5: LDA unk_D23B,Y / ADC $17B,X / CMP #$A / BCC loc_D1C1 /
 *      SBC #$A / loc_D1C1: STA $17B,X / DEY / DEX / BPL loc_D1B5 /
 *      LDA #0 / ADC byte_17A / STA byte_17A / CMP #$A / BCC loc_D1DE /
 *      LDA #9 / LDY #7 / loc_D1D8: STA $179,Y / DEY / BNE loc_D1D8 /
 *      loc_D1DE: LDX byte_10 / locret_D1E0: RTS
 * ------------------------------------------------------------------------ */
void sub_D1A6(void)                          /* $D1A6 */
{
    STA(byte_10);                            /* STA byte_10 */
    ASL_A();                                 /* ASL */
    CLC();                                   /* CLC */
    ADC(byte_10);                            /* ADC byte_10 */
    ASL_A();                                 /* ASL */
    ADCI(6);                                 /* ADC #6 */
    TAY();                                   /* TAY */
    STX(byte_10);                            /* STX byte_10 */
    LDXI(5);                                 /* LDX #5 */
    CLC();                                   /* CLC */

loc_D1B5:                                    /* $D1B5 */
    LDA(ABSY(unk_D23B));                     /* LDA unk_D23B,Y */
    ADC(ABSX(0x017B));                       /* ADC $17B,X */
    CMPI(0x0A);                              /* CMP #$A */
    BCC(loc_D1C1);                           /* BCC loc_D1C1 */
    SBCI(0x0A);                              /* SBC #$A */

loc_D1C1:                                    /* $D1C1 */
    STA(ABSX(0x017B));                       /* STA $17B,X */
    DEY();                                   /* DEY */
    DEX();                                   /* DEX */
    BPL(loc_D1B5);                           /* BPL loc_D1B5 */
    LDAI(0);                                 /* LDA #0 */
    ADC(byte_17A);                           /* ADC byte_17A */
    STA(byte_17A);                           /* STA byte_17A */
    CMPI(0x0A);                              /* CMP #$A */
    BCC(loc_D1DE);                           /* BCC loc_D1DE */
    LDAI(9);                                 /* LDA #9 */
    LDYI(7);                                 /* LDY #7 */

loc_D1D8:                                    /* $D1D8 */
    STA(ABSY(0x0179));                       /* STA $179,Y */
    DEY();                                   /* DEY */
    BNE(loc_D1D8);                           /* BNE loc_D1D8 */

loc_D1DE:                                    /* $D1DE */
    LDX(byte_10);                            /* LDX byte_10 */
    /* locret_D1E0 ($D1E0) is the shared RTS; sub_D193 branches into it. */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D2C0  sub_D2C0 - read a sprite table byte by X
 *
 *  Original: src/bank7.asm:3434-3436:  LDA unk_D23C,X / RTS
 * ------------------------------------------------------------------------ */
void sub_D2C0(void)                          /* $D2C0 */
{
    LDA(ABSX(unk_D23C));                     /* LDA unk_D23C,X */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D2C4  sub_D2C4 - the same by Y (not referenced by anything)
 *
 *  Original: src/bank7.asm:3443-3445:  LDA unk_D23C,Y / RTS
 * ------------------------------------------------------------------------ */
void sub_D2C4(void)                          /* $D2C4 */
{
    LDA(ABSY(unk_D23C));                     /* LDA unk_D23C,Y */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D2C8  clear_first_last_sprites - park the last 4 and first 4 sprites
 *
 *  Original: src/bank7.asm:3453-3476
 *      BIT byte_39 / BVC locret_D2E3 / LDA byte_39 / AND #4 / BNE loc_D2E1 /
 *      LDA #$F8 / LDY #$F0 /
 *      loc_D2D6: STA $200,Y / INY / INY / INY / INY / CPY #$10 /
 *      BNE loc_D2D6 / loc_D2E1: INC byte_39 / locret_D2E3: RTS
 * ------------------------------------------------------------------------ */
void clear_first_last_sprites(void)           /* $D2C8 */
{
    BIT(byte_39);                            /* BIT byte_39 */
    BVC(locret_D2E3);                        /* BVC locret_D2E3 */
    LDA(byte_39);                            /* LDA byte_39 */
    ANDI(4);                                 /* AND #4 */
    BNE(loc_D2E1);                           /* BNE loc_D2E1 */
    LDAI(0xF8);                              /* LDA #$F8 */
    LDYI(0xF0);                              /* LDY #$F0 */

loc_D2D6:                                    /* $D2D6 */
    STA(ABSY(0x0200));                       /* STA $200,Y */
    INY();                                   /* INY */
    INY();                                   /* INY */
    INY();                                   /* INY */
    INY();                                   /* INY */
    CPYI(0x10);                              /* CPY #$10 */
    BNE(loc_D2D6);                           /* BNE loc_D2D6 */

loc_D2E1:                                    /* $D2E1 */
    INC(byte_39);                            /* INC byte_39 */

locret_D2E3:                                 /* $D2E3 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $D2E4  set_new_high_score - copy the score over the top score
 *
 *  Original: src/bank7.asm:3483-3502
 *      LDY #7 / loc_D2E6: LDA $179,Y / SBC $181,Y / DEY / BNE loc_D2E6 /
 *      BCC locret_D2FC / LDY #7 /
 *      loc_D2F3: LDA $179,Y / STA $181,Y / DEY / BNE loc_D2F3 /
 *      locret_D2FC: RTS
 *
 *  Note that the loop starts at Y=7 and stops before Y=0 (DEY sets Z at 0),
 *  exactly like the original; the carry into the first SBC comes from the
 *  caller (there is no SEC here).
 * ------------------------------------------------------------------------ */
void set_new_high_score(void)                /* $D2E4 */
{
    LDYI(7);                                 /* LDY #7 */

loc_D2E6:                                    /* $D2E6 */
    LDA(ABSY(0x0179));                       /* LDA $179,Y */
    SBC(ABSY(0x0181));                       /* SBC $181,Y */
    DEY();                                   /* DEY */
    BNE(loc_D2E6);                           /* BNE loc_D2E6 */
    BCC(locret_D2FC);                        /* BCC locret_D2FC */
    LDYI(7);                                 /* LDY #7 */

loc_D2F3:                                    /* $D2F3 */
    LDA(ABSY(0x0179));                       /* LDA $179,Y */
    STA(ABSY(0x0181));                       /* STA $181,Y */
    DEY();                                   /* DEY */
    BNE(loc_D2F3);                           /* BNE loc_D2F3 */

locret_D2FC:                                 /* $D2FC */
    RTS();
}
