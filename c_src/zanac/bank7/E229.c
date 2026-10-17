/* bank7/E229.c - bank 7, $E229-$EB58: startup, main menu, level sequence,
 * ending screen and the APU music engine.
 *
 * Ported from src/bank7.asm lines 6393-7855 (addresses from Build/bank7.lst).
 *
 * `reset` is the engine's Reset callback (see c_src/zanac/main.c) and the chain
 * reset -> main_menu_loop -> sub_E81E is the game's outer loop.
 */
#include "zanac.h"
#include "savestate.h"

/* ------------------------------------------------------------------------ *
 * sub_E8C9 has no RTS: every path tail-jumps back into sub_E81E, either at
 * loc_E89E (which is also where the JSR returns, when it returns), at
 * loc_E880 (the next frame of the level loop) or at loc_E876 (restart the
 * loop's two counter bytes).  In C the two jumps are reported to the caller
 * through this variable so that it can `goto` the matching label.  Entering
 * sub_E81E() here instead would run its setup again (see the comment at the
 * $E8C9 label below), which reset the level while the fortress was alive.
 * ------------------------------------------------------------------------ */
static enum {
    ZANAC_E8C9_RETURN = 0,                   /* no jump: the original's caller
                                              * continues at loc_E89E */
    ZANAC_E8C9_GOTO_E880,                    /* JMP loc_E880: next frame      */
    ZANAC_E8C9_GOTO_E876                     /* JMP loc_E876: restart counters */
} s_e8c9_jump;

/* ------------------------------------------------------------------------ *
 *  $E229  sub_E229 - the "hold this many buttons" cheat check of the menu
 *
 *  Original: src/bank7.asm:6393-6415
 *      LDA reset_counter / CMP #9 / BNE locret_E253 / JSR sub_E76C /
 *      LDY byte_6F / LDX unk_E254,Y / LDA 0,X / AND unk_E25D,Y /
 *      BEQ locret_E253 / INC byte_6F / CPY #8 / BNE locret_E253 /
 *      LDA #$FF / STA byte_19B / LDA #0 / STA byte_6F / LDA #9 / JSR apu_play
 * ------------------------------------------------------------------------ */
void sub_E229(void)                          /* $E229 */
{
    LDA(reset_counter);                      /* LDA reset_counter */
    CMPI(9);                                 /* CMP #9 */
    BNE(locret_E253);                        /* BNE locret_E253 */
    sub_E76C();                              /* JSR sub_E76C */
    LDY(byte_6F);                            /* LDY byte_6F */
    LDX(ABSY(unk_E254));                     /* LDX unk_E254,Y */
    LDA(0x0000 + X);                         /* LDA 0,X */
    AND(ABSY(unk_E25D));                     /* AND unk_E25D,Y */
    BEQ(locret_E253);                        /* BEQ locret_E253 */
    INC(byte_6F);                            /* INC byte_6F */
    CPYI(8);                                 /* CPY #8 */
    BNE(locret_E253);                        /* BNE locret_E253 */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(byte_19B);                           /* STA byte_19B */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_6F);                            /* STA byte_6F */
    LDAI(9);                                 /* LDA #9 */
    apu_play();                              /* JSR apu_play */

locret_E253:                                 /* $E253 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E266  sub_E266 - write a 7-digit BCD number into the name table
 *
 *  Original: src/bank7.asm:6441-6469 (Y is the score index, the caller has set
 *  $2006 already)
 *      LDA #7 / STA byte_10 / LDA #0 / STA byte_11 /
 *      loc_E26E: LDA byte_11 / BNE loc_E27D / LDA $100,Y / STA byte_11 /
 *      BNE loc_E27D / LDA #$20 / BNE loc_E282 /
 *      loc_E27D: LDA $100,Y / ORA #$30 /
 *      loc_E282: STA PPU_DATA / INY / DEC byte_10 / BNE loc_E26E /
 *      LDA #$30 / STA PPU_DATA / RTS
 * ------------------------------------------------------------------------ */
void sub_E266(void)                          /* $E266 */
{
    LDAI(7);                                 /* LDA #7 */
    STA(byte_10);                            /* STA byte_10 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_11);                            /* STA byte_11 */

loc_E26E:                                    /* $E26E */
    LDA(byte_11);                            /* LDA byte_11 */
    BNE(loc_E27D);                           /* BNE loc_E27D */
    LDA(ABSY(0x0100));                       /* LDA $100,Y */
    STA(byte_11);                            /* STA byte_11 */
    BNE(loc_E27D);                           /* BNE loc_E27D */
    LDAI(0x20);                              /* LDA #$20 */
    BNE(loc_E282);                           /* BNE loc_E282 */

loc_E27D:                                    /* $E27D */
    LDA(ABSY(0x0100));                       /* LDA $100,Y */
    ORAI(0x30);                              /* ORA #$30 */

loc_E282:                                    /* $E282 */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    INY();                                   /* INY */
    DEC(byte_10);                            /* DEC byte_10 */
    BNE(loc_E26E);                           /* BNE loc_E26E */
    LDAI(0x30);                              /* LDA #$30 */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E399  sub_E399 - write one of the credit strings of the menu
 *
 *  Original: src/bank7.asm:6597-6616 (byte_E3B8 is a $80-terminated list of
 *  ($23,$xx) address pairs followed by the text)
 *      LDA byte_E3B8,Y / BEQ locret_E3B7 / LDA #$23 / STA PPU_ADDRESS /
 *      LDA byte_E3B8,Y / STA PPU_ADDRESS / INY /
 *      loc_E3AA: LDA byte_E3B8,Y / INY / CMP #$80 / BEQ sub_E399 /
 *      STA PPU_DATA / BNE loc_E3AA / locret_E3B7: RTS
 * ------------------------------------------------------------------------ */
void sub_E399(void)                          /* $E399 */
{
loc_E399:                                    /* $E399 (BEQ from loc_E3AA) */
    LDA(ABSY(byte_E3B8));                    /* LDA byte_E3B8,Y */
    BEQ(locret_E3B7);                        /* BEQ locret_E3B7 */
    LDAI(0x23);                              /* LDA #$23 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDA(ABSY(byte_E3B8));                    /* LDA byte_E3B8,Y */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    INY();                                   /* INY */

loc_E3AA:                                    /* $E3AA */
    LDA(ABSY(byte_E3B8));                    /* LDA byte_E3B8,Y */
    INY();                                   /* INY */
    CMPI(0x80);                              /* CMP #$80 */
    BEQ(loc_E399);                           /* BEQ sub_E399 */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    BNE(loc_E3AA);                           /* BNE loc_E3AA */

locret_E3B7:                                 /* $E3B7 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E3F4  sub_E3F4 - expand a run-length encoded screen ($16/$17 -> $2006)
 *
 *  Original: src/bank7.asm:6631-6678
 *      LDY #0 /
 *      loc_E3F6: LDA ($16),Y / INY / CMP #$FE / BCS loc_E41A / CLC /
 *      ADC byte_18 / TAX / LDA byte_19 / ADC #0 / STA PPU_ADDRESS /
 *      STX PPU_ADDRESS / LDA ($16),Y / INY / TAX /
 *      loc_E40F: LDA ($16),Y / INY / STA PPU_DATA / DEX / BNE loc_E40F /
 *      BEQ loc_E3F6 /
 *      loc_E41A: CMP #$FE / BEQ locret_E438 / TYA / CLC / ADC byte_16 /
 *      STA byte_16 / LDA byte_17 / ADC #0 / STA byte_17 / LDA byte_18 /
 *      ADC #$20 / STA byte_18 / LDA byte_19 / ADC #0 / STA byte_19 /
 *      BCC sub_E3F4 / locret_E438: RTS
 * ------------------------------------------------------------------------ */
void sub_E3F4(void)                          /* $E3F4 */
{
loc_E3F4:                                    /* $E3F4 (BCC from the tail) */
    LDYI(0);                                 /* LDY #0 */

loc_E3F6:                                    /* $E3F6 */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    INY();                                   /* INY */
    CMPI(0xFE);                              /* CMP #$FE */
    BCS(loc_E41A);                           /* BCS loc_E41A */
    CLC();                                   /* CLC */
    ADC(byte_18);                            /* ADC byte_18 */
    TAX();                                   /* TAX */
    LDA(byte_19);                            /* LDA byte_19 */
    ADCI(0);                                 /* ADC #0 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    STX(PPU_ADDRESS);                        /* STX PPU_ADDRESS */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    INY();                                   /* INY */
    TAX();                                   /* TAX */

loc_E40F:                                    /* $E40F */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    INY();                                   /* INY */
    STA(PPU_DATA);                           /* STA PPU_DATA */
    DEX();                                   /* DEX */
    BNE(loc_E40F);                           /* BNE loc_E40F */
    BEQ(loc_E3F6);                           /* BEQ loc_E3F6 */

loc_E41A:                                    /* $E41A */
    CMPI(0xFE);                              /* CMP #$FE */
    BEQ(locret_E438);                        /* BEQ locret_E438 */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADC(byte_16);                            /* ADC byte_16 */
    STA(byte_16);                            /* STA byte_16 */
    LDA(byte_17);                            /* LDA byte_17 */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_17);                            /* STA byte_17 */
    LDA(byte_18);                            /* LDA byte_18 */
    ADCI(0x20);                              /* ADC #$20 */
    STA(byte_18);                            /* STA byte_18 */
    LDA(byte_19);                            /* LDA byte_19 */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_19);                            /* STA byte_19 */
    BCC(loc_E3F4);                           /* BCC sub_E3F4 */

locret_E438:                                 /* $E438 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E439  sound_test_menu - the hidden sound-test screen
 *
 *  Original: src/bank7.asm:6685-6705
 *      JSR pad_read / LDA pad_buttons_1 / ORA pad_buttons_2 / AND #$C0 /
 *      BNE sound_test_menu /
 *      JSR print_string ; inline block at $E6A3 (.byte $A, $20, $88, "SOUND TEST")
 *      LDA #1 / STA byte_6F / LDA #1 / STA byte_70 / JSR sub_C6AB / JSR sub_DDF3
 * ------------------------------------------------------------------------ */
void sound_test_menu(void)                   /* $E439 */
{
loc_E439:                                    /* $E439 */
    pad_read();                              /* JSR pad_read */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ORA(pad_buttons_2);                      /* ORA pad_buttons_2 */
    ANDI(0xC0);                              /* AND #$C0 */
    BNE(loc_E439_wait);                      /* BNE sound_test_menu */
    goto loc_E439_done;

loc_E439_wait:                               /* the loop waits for a button;
                                              * the port must advance the engine
                                              * so the host can press one */
    ZANAC_WAIT_NMI();
    goto loc_E439;

loc_E439_done:                               /* $E439 after the wait */
    /* $E444: JSR print_string ; inline block at $E447
     *   .byte $A, $20, $88, "SOUND TEST", 0 */
    PRINT_STRING(0xE447);
    LDAI(1);                                 /* LDA #1 */
    STA(byte_6F);                            /* STA byte_6F */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_70);                            /* STA byte_70 */
    sub_C6AB();                              /* JSR sub_C6AB */
    sub_DDF3();                              /* JSR sub_DDF3 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E462  sub_E462 - the sound-test loop
 *
 *  Original: src/bank7.asm:6711-6775
 * ------------------------------------------------------------------------ */
void sub_E462(void)                          /* $E462 */
{
loc_E462:                                    /* $E462 (BNE from the tail) */
    LDAI(0x20);                              /* LDA #$20 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0xCD);                              /* LDA #$CD */
    STA(byte_18);                            /* STA byte_18 */
    LDA(byte_6F);                            /* LDA byte_6F */
    sub_D347();                              /* JSR sub_D347 */

loc_E46F:                                    /* $E46F */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    pad_read();                              /* JSR pad_read */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ORA(pad_test2);                          /* ORA pad_test2 */
    BNE(loc_E487);                           /* BNE loc_E487 */
    DEC(byte_70);                            /* DEC byte_70 */
    BNE(loc_E46F);                           /* BNE loc_E46F */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_70);                            /* STA byte_70 */
    BNE(loc_E48B);                           /* BNE loc_E48B */

loc_E487:                                    /* $E487 */
    LDAI(0x0F);                              /* LDA #$F */
    STA(byte_70);                            /* STA byte_70 */

loc_E48B:                                    /* $E48B */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ORA(pad_buttons_2);                      /* ORA pad_buttons_2 */
    STA(pad_buttons_1);                      /* STA pad_buttons_1 */
    BEQ(loc_E46F);                           /* BEQ loc_E46F */
    ANDI(0xC0);                              /* AND #$C0 */
    BEQ(loc_E4A2);                           /* BEQ loc_E4A2 */
    apu_stop();                              /* JSR apu_stop */
    LDA(byte_6F);                            /* LDA byte_6F */
    apu_play();                              /* JSR apu_play */
    goto loc_E46F;                           /* JMP loc_E46F */

loc_E4A2:                                    /* $E4A2 */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ANDI(0x0F);                              /* AND #$F */
    BEQ(loc_E46F);                           /* BEQ loc_E46F */
    apu_stop();                              /* JSR apu_stop */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ANDI(2);                                 /* AND #2 */
    BEQ(loc_E4BB);                           /* BEQ loc_E4BB */
    DEC(byte_6F);                            /* DEC byte_6F */
    BNE(loc_E462);                           /* BNE sub_E462 */
    LDAI(0x23);                              /* LDA #$23 */
    STA(byte_6F);                            /* STA byte_6F */
    BNE(loc_E462);                           /* BNE sub_E462 */

loc_E4BB:                                    /* $E4BB */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ANDI(1);                                 /* AND #1 */
    BEQ(loc_E46F);                           /* BEQ loc_E46F */
    INC(byte_6F);                            /* INC byte_6F */
    LDA(byte_6F);                            /* LDA byte_6F */
    CMPI(0x24);                              /* CMP #$24 */
    BCC(loc_E462);                           /* BCC sub_E462 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_6F);                            /* STA byte_6F */
    BNE(loc_E462);                           /* BNE sub_E462 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E4CF  show_ending_screen - the ending / staff roll
 *
 *  Original: src/bank7.asm:6782-6960 (the congratulation_text list is walked
 *  at $E556; the two print_string blocks are at $E8C6 and $E8D3 below)
 * ------------------------------------------------------------------------ */
void show_ending_screen(void)                /* $E4CF */
{
    LDYI(0x78);                              /* LDY #$78 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    ppu_disable_all();                       /* JSR ppu_disable_all */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_7D);                            /* STA byte_7D */
    set_new_high_score();                    /* JSR set_new_high_score */
    clear_oam_buffer();                      /* JSR clear_oam_buffer */
    LDAI(0x80);                              /* LDA #$80 */
    STA(byte_210);                           /* STA byte_210 */
    STA(byte_213);                           /* STA byte_213 */
    STA(byte_72);                            /* STA byte_72 */
    STA(byte_73);                            /* STA byte_73 */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_74);                            /* STA byte_74 */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(byte_75);                            /* STA byte_75 */
    ppu_nmi_enable();                        /* JSR ppu_nmi_enable */
    LDYI(1);                                 /* LDY #1 */
    sub_E60F();                              /* JSR sub_E60F */
    ppu_disable_all();                       /* JSR ppu_disable_all */
    LDAI(0x12);                              /* LDA #$12 */
    STA(byte_19);                            /* STA byte_19 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(2);                                 /* LDA #2 */
    LDYI(4);                                 /* LDY #4 */
    sub_C8DF();                              /* JSR sub_C8DF */
    sub_C8BA();                              /* JSR sub_C8BA */
    LDAI(0x20);                              /* LDA #$20 */
    LDXI(0x20);                              /* LDX #$20 */
    LDYI(0xAA);                              /* LDY #$AA */
    clear_screen();                          /* JSR clear_screen */
    /* $E51B: JSR print_string ; inline block at $E51E
     *   .byte 5, $20, $89, "SCORE", 0 */
    PRINT_STRING(0xE51E);
    LDAI(0x20);                              /* LDA #$20 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDAI(0x8E);                              /* LDA #$8E */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    LDYI(0x7A);                              /* LDY #$7A */
    sub_E266();                              /* JSR sub_E266 */
    LDYI(0x19);                              /* LDY #$19 */
    sub_E399();                              /* JSR sub_E399 */
    LDAI(0xA6);                              /* LDA #$A6 */
    STA(byte_18);                            /* STA byte_18 */
    LDAI(0xDF);                              /* LDA #$DF */
    STA(byte_19);                            /* STA byte_19 */
    ppu_cram_load();                         /* JSR ppu_cram_load */
    LDAI(10);                                /* LDA #10 (ending music) */
    apu_play();                              /* JSR apu_play */
    sub_C6AB();                              /* JSR sub_C6AB */
    sub_DDF3();                              /* JSR sub_DDF3 */
    LDXI(0);                                 /* LDX #0 */
    LDAI(6);                                 /* LDA #6 */
    STA(byte_6F);                            /* STA byte_6F */

loc_E556:                                    /* $E556 */
    LDA(ABSX(congratulation_text));          /* LDA congratulation_text,X */
    BEQ(loc_E5C5);                           /* BEQ loc_E5C5 */
    INX();                                   /* INX */
    STA(byte_70);                            /* STA byte_70 */

loc_E55E:                                    /* $E55E */
    LDA(byte_70);                            /* LDA byte_70 */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    SBCI(3);                                 /* SBC #3 */
    STA(byte_73);                            /* STA byte_73 */

loc_E567:                                    /* $E567 */
    LDA(byte_20);                            /* LDA byte_20 */
    BNE(loc_E567_wait);                      /* BNE loc_E567 */
    goto loc_E568;

loc_E567_wait:                               /* byte_20 is drained by
                                              * ppu_update_background, which
                                              * the NMI calls */
    ZANAC_WAIT_NMI();
    goto loc_E567;

loc_E568:                                    /* $E568 */
    LDA(byte_6F);                            /* LDA byte_6F */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    PHA();                                   /* PHA */
    SEC();                                   /* SEC */
    SBCI(4);                                 /* SBC #4 */
    STA(byte_72);                            /* STA byte_72 */
    goto loc_E61E;                           /* JSR loc_E61E (inside sub_E60F) */

loc_E61E:                                    /* $E61E (the callee's block) */
    sub_E653();                              /* JSR sub_E653 */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    LDA(byte_210);                           /* LDA byte_210 */
    CMP(byte_72);                            /* CMP byte_72 */
    PHP();                                   /* PHP */
    BEQ(loc_E637);                           /* BEQ loc_E637 */
    CLC();                                   /* CLC */
    ADCI(1);                                 /* ADC #1 */
    STA(byte_210);                           /* STA byte_210 */
    STA(byte_214);                           /* STA byte_214 */

loc_E637:                                    /* $E637 */
    LDA(byte_213);                           /* LDA byte_213 */
    CMP(byte_73);                            /* CMP byte_73 */
    BEQ(loc_E64E);                           /* BEQ loc_E64E */
    CLC();                                   /* CLC */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_213);                           /* STA byte_213 */
    CLC();                                   /* CLC */
    ADCI(8);                                 /* ADC #8 */
    STA(byte_217);                           /* STA byte_217 */
    PLP();                                   /* PLP */
    goto loc_E61E;                           /* JMP loc_E61E */

loc_E64E:                                    /* $E64E */
    PLP();                                   /* PLP */
    BEQ(loc_E61E_ret);                       /* BEQ locret_E670 (return of the JSR) */
    BNE(loc_E61E);                           /* BNE loc_E61E */

loc_E61E_ret:                                /* $E670: the JSR returns here */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_11);                            /* STA byte_11 */
    PLA();                                   /* PLA */
    ASL_A();                                 /* ASL */
    ROL(byte_11);                            /* ROL byte_11 */
    ASL_A();                                 /* ASL */
    ROL(byte_11);                            /* ROL byte_11 */
    ADC(byte_70);                            /* ADC byte_70 */
    STA(byte_10);                            /* STA byte_10 */
    LDA(ABSX(0xE671));                       /* LDA $E671,X */
    BEQ(loc_E5C5);                           /* BEQ loc_E5C5 */
    CMPI(0xFF);                              /* CMP #$FF */
    BEQ(loc_E5CF);                           /* BEQ loc_E5CF */
    INX();                                   /* INX */
    CMPI(0x0A);                              /* CMP #$A */
    BCS(loc_E59D);                           /* BCS loc_E59D */
    ADC(byte_70);                            /* ADC byte_70 */
    STA(byte_70);                            /* STA byte_70 */
    goto loc_E55E;                           /* JMP loc_E55E */

loc_E59D:                                    /* $E59D */
    PHA();                                   /* PHA */
    LDA(byte_10);                            /* LDA byte_10 */
    PHA();                                   /* PHA */
    LDA(byte_11);                            /* LDA byte_11 */
    PHA();                                   /* PHA */
    LDAI(4);                                 /* LDA #4 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAY();                                   /* TAY */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSY(0x0460));                       /* STA $460,Y */
    PLA();                                   /* PLA */
    STA(ABSY(0x0461));                       /* STA $461,Y */
    PLA();                                   /* PLA */
    STA(ABSY(0x0462));                       /* STA $462,Y */
    PLA();                                   /* PLA */
    STA(ABSY(0x0463));                       /* STA $463,Y */
    sub_C807();                              /* JSR sub_C807 */
    INC(byte_70);                            /* INC byte_70 */
    BNE(loc_E55E);                           /* BNE loc_E55E */

loc_E5C5:                                    /* $E5C5 */
    INX();                                   /* INX */
    LDYI(0x14);                              /* LDY #$14 */
    sub_E60F();                              /* JSR sub_E60F */
    INC(byte_6F);                            /* INC byte_6F */
    BNE(loc_E556);                           /* BNE loc_E556 */

loc_E5CF:                                    /* $E5CF */
    LDYI(1);                                 /* LDY #1 */
    sub_E60F();                              /* JSR sub_E60F */
    LDA(byte_74);                            /* LDA byte_74 */
    BNE(loc_E5EA);                           /* BNE loc_E5EA */
    LDA(byte_75);                            /* LDA byte_75 */
    CMPI(0xFE);                              /* CMP #$FE */
    BCC(loc_E5EA);                           /* BCC loc_E5EA */
    LDAI(0xFE);                              /* LDA #$FE */
    STA(byte_75);                            /* STA byte_75 */
    LDAI(0xAC);                              /* LDA #$AC */
    STA(byte_72);                            /* STA byte_72 */
    LDAI(0xC8);                              /* LDA #$C8 */
    STA(byte_73);                            /* STA byte_73 */

loc_E5EA:                                    /* $E5EA */
    pad_read();                              /* JSR pad_read */
    LDA(byte_75);                            /* LDA byte_75 */
    CMPI(0xFE);                              /* CMP #$FE */
    BNE(loc_E606);                           /* BNE loc_E606 */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    AND(pad_buttons_2);                      /* AND pad_buttons_2 */
    ANDI(0xC0);                              /* AND #$C0 */
    CMPI(0xC0);                              /* CMP #$C0 */
    BNE(loc_E606);                           /* BNE loc_E606 */
    LDAI(0xFD);                              /* LDA #$FD */
    STA(byte_75);                            /* STA byte_75 */
    LDXI(0xD8);                              /* LDX #$D8 */
    goto loc_E556;                           /* JMP loc_E556 */

loc_E606:                                    /* $E606 */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_E5CF);                           /* BEQ loc_E5CF */
    main_menu_loop();                        /* JMP main_menu_loop */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E60F  sub_E60F - move the two ending-screen rows
 *
 *  Original: src/bank7.asm:6967-7010
 *      TYA / PHA / JSR sub_E653 / LDY #1 / JSR all_enemies_destroyed /
 *      PLA / TAY / DEY / BNE sub_E60F /
 *      loc_E61E: JSR sub_E653 / LDY #1 / JSR all_enemies_destroyed /
 *      LDA byte_210 / CMP byte_72 / PHP / BEQ loc_E637 / CLC / ADC #1 /
 *      STA byte_210 / STA byte_214 /
 *      loc_E637: LDA byte_213 / CMP byte_73 / BEQ loc_E64E / CLC / ADC #2 /
 *      STA byte_213 / CLC / ADC #8 / STA byte_217 / PLP / JMP loc_E61E /
 *      loc_E64E: PLP / BEQ locret_E670 / BNE loc_E61E
 * ------------------------------------------------------------------------ */
void sub_E60F(void)                          /* $E60F */
{
loc_E60F:                                    /* $E60F (BNE from the tail) */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    sub_E653();                              /* JSR sub_E653 */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEY();                                   /* DEY */
    BNE(loc_E60F);                           /* BNE sub_E60F */

loc_E61E:                                    /* $E61E */
    sub_E653();                              /* JSR sub_E653 */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    LDA(byte_210);                           /* LDA byte_210 */
    CMP(byte_72);                            /* CMP byte_72 */
    PHP();                                   /* PHP */
    BEQ(loc_E637);                           /* BEQ loc_E637 */
    CLC();                                   /* CLC */
    ADCI(1);                                 /* ADC #1 */
    STA(byte_210);                           /* STA byte_210 */
    STA(byte_214);                           /* STA byte_214 */

loc_E637:                                    /* $E637 */
    LDA(byte_213);                           /* LDA byte_213 */
    CMP(byte_73);                            /* CMP byte_73 */
    BEQ(loc_E64E);                           /* BEQ loc_E64E */
    CLC();                                   /* CLC */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_213);                           /* STA byte_213 */
    CLC();                                   /* CLC */
    ADCI(8);                                 /* ADC #8 */
    STA(byte_217);                           /* STA byte_217 */
    PLP();                                   /* PLP */
    goto loc_E61E;                           /* JMP loc_E61E */

loc_E64E:                                    /* $E64E */
    PLP();                                   /* PLP */
    BEQ(locret_E670);                        /* BEQ locret_E670 */
    BNE(loc_E61E);                           /* BNE loc_E61E */

locret_E670:                                 /* $E670 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E653  sub_E653 - animate the ending-screen sprites
 *
 *  Original: src/bank7.asm:7017-7036
 *      INC byte_71 / BNE loc_E659 / DEC byte_74 /
 *      loc_E659: LDA byte_71 / AND #2 / ASL / ADC #$EC / STA byte_211 /
 *      ADC #2 / STA byte_215 / LDA #1 / STA byte_212 / STA byte_216 /
 *      locret_E670: RTS
 * ------------------------------------------------------------------------ */
void sub_E653(void)                          /* $E653 */
{
    INC(byte_71);                            /* INC byte_71 */
    BNE(loc_E659);                           /* BNE loc_E659 */
    DEC(byte_74);                            /* DEC byte_74 */

loc_E659:                                    /* $E659 */
    LDA(byte_71);                            /* LDA byte_71 */
    ANDI(2);                                 /* AND #2 */
    ASL_A();                                 /* ASL */
    ADCI(0xEC);                              /* ADC #$EC */
    STA(byte_211);                           /* STA byte_211 */
    ADCI(2);                                 /* ADC #2 */
    STA(byte_215);                           /* STA byte_215 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_212);                           /* STA byte_212 */
    STA(byte_216);                           /* STA byte_216 */
    /* locret_E670 ($E670) is the shared RTS; sub_E60F branches into it. */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E767  sub_E767 - clear byte_FB (the light-gun port shadow)
 *
 *  Original: src/bank7.asm:7090-7093:  LDA #0 / STA byte_FB / RTS
 * ------------------------------------------------------------------------ */
void sub_E767(void)                          /* $E767 */
{
    LDAI(0);                                 /* LDA #0 */
    STA(byte_FB);                            /* STA byte_FB */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  sub_E76C  $E76C - read the light gun through $4016/$4017
 *
 *  Original: src/bank7.asm:7100-7156.  The original counts CPU cycles with
 *  NOPs and the DEY loops; the port keeps the loops (they are part of the
 *  protocol timing) and the NOPs are dropped with a comment, because in the
 *  port the CPU does not run at a fixed rate anyway.
 * ------------------------------------------------------------------------ */
void sub_E76C(void)                          /* $E76C */
{
    LDA(byte_FB);                            /* LDA byte_FB */
    ANDI(0xF8);                              /* AND #$F8 */
    STA(byte_FB);                            /* STA byte_FB */
    ORAI(5);                                 /* ORA #5 */
    STA(JOYPAD_PORT1);                       /* STA JOYPAD_PORT1 */
    /* six NOPs (timing only) */
    LDXI(8);                                 /* LDX #8 */

loc_E77F:                                    /* $E77F */
    LDA(byte_FB);                            /* LDA byte_FB */
    ORAI(4);                                 /* ORA #4 */
    STA(JOYPAD_PORT1);                       /* STA JOYPAD_PORT1 */
    LDYI(0x0A);                              /* LDY #$A */

loc_E788:                                    /* $E788 */
    DEY();                                   /* DEY */
    BNE(loc_E788);                           /* BNE loc_E788 */
    /* NOP */
    LDY(byte_FB);                            /* LDY byte_FB */
    LDA(JOYPAD_PORT2);                       /* LDA JOYPAD_PORT2 */
    LSR_A();                                 /* LSR */
    ANDI(0x0F);                              /* AND #$F */
    BEQ(loc_E7BB);                           /* BEQ loc_E7BB */
    STA(0x0000 + X);                         /* STA 0,X */
    LDA(byte_FB);                            /* LDA byte_FB */
    ORAI(6);                                 /* ORA #6 */
    STA(JOYPAD_PORT1);                       /* STA JOYPAD_PORT1 */
    LDYI(0x0A);                              /* LDY #$A */

loc_E7A1:                                    /* $E7A1 */
    DEY();                                   /* DEY */
    BNE(loc_E7A1);                           /* BNE loc_E7A1 */
    /* two NOPs (timing only) */
    LDA(JOYPAD_PORT2);                       /* LDA JOYPAD_PORT2 */
    ROL_A();                                 /* ROL */
    ROL_A();                                 /* ROL */
    ROL_A();                                 /* ROL */
    ANDI(0xF0);                              /* AND #$F0 */
    ORA(0x0000 + X);                         /* ORA 0,X */
    EORI(0xFF);                              /* EOR #$FF */
    STA(0x0000 + X);                         /* STA 0,X */
    DEX();                                   /* DEX */
    BPL(loc_E77F);                           /* BPL loc_E77F */
    LDY(byte_FB);                            /* LDY byte_FB */
    ORAI(0xFF);                              /* ORA #$FF */

loc_E7BB:                                    /* $E7BB */
    STY(JOYPAD_PORT1);                       /* STY JOYPAD_PORT1 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E7BF  reset - the Reset callback of the engine
 *
 *  Original: src/bank7.asm:7163-7202
 *      LDA #0 / STA byte_20 / STA cram_update_mode / STA byte_9C /
 *      LDX #$FF / TXS / LDA #$30 / JSR ppu_set_control1 / LDA #6 /
 *      JSR ppu_set_control2 / JSR sub_E767 / LDX #3 /
 *      loc_E7DA: LDA PPU_STATUS / BPL loc_E7DA / DEX / BNE loc_E7DA /
 *      LDA #$C0 / STA JOYPAD_PORT2 / LDA #0 / STA APU_DELTA_REG /
 *      JSR reset_score / LDA byte_7D / PHA / LDX #$A / LDA #0 /
 *      loc_E7F6: STA 0,X / INX / CPX #$F9 / BNE loc_E7F6 /
 *      JSR sub_CB40 / PLA / STA byte_7D / JSR apu_reset
 *  (falls through into main_menu_loop)
 *
 *  The warm-up loop polls $2002 bit 7 three times.  In the port the PPU is not
 *  advanced while the game code runs, so the spin calls
 *  neslib_wait_vblank_flag(), which runs the engine up to the next vblank flag
 *  exactly like the hardware the 6502 was polling (see c_src/PORTING.md).
 * ------------------------------------------------------------------------ */
void reset(void)                             /* $E7BF */
{
    /* A frame boundary inside the warm-up can be saved too; the menu loop is
     * where such a state resumes (see zanac_ram.h and zanac/savestate.c). */
    zanac_resume_kind = ZANAC_RESUME_MENU;
    LDAI(0);                                 /* LDA #0 */
    STA(byte_20);                            /* STA byte_20 */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    STA(byte_9C);                            /* STA byte_9C */
    LDXI(0xFF);                              /* LDX #$FF */
    TXS();                                   /* TXS */
    LDAI(0x30);                              /* LDA #$30 */
    ppu_set_control1();                      /* JSR ppu_set_control1 */
    LDAI(6);                                 /* LDA #6 */
    ppu_set_control2();                      /* JSR ppu_set_control2 */
    sub_E767();                              /* JSR sub_E767 */
    LDXI(3);                                 /* LDX #3 */

loc_E7DA:                                    /* $E7DA */
    LDA(PPU_STATUS);                         /* LDA PPU_STATUS */
    BPL(loc_E7DA_spin);                      /* BPL loc_E7DA */
    DEX();                                   /* DEX */
    BNE(loc_E7DA);                           /* BNE loc_E7DA */
    goto loc_E7FD;                           /* the warm-up is over */

loc_E7DA_spin:                               /* re-entry of $E7DA after a wait */
    /* The original spins on PPU_STATUS bit 7; the port advances the engine to
     * the next vblank flag and retries the read, which then clears it. */
    neslib_wait_vblank_flag();
    goto loc_E7DA;

loc_E7FD:                                    /* $E7FD */
    LDAI(0xC0);                              /* LDA #$C0 */
    STA(JOYPAD_PORT2);                       /* STA JOYPAD_PORT2 */
    LDAI(0);                                 /* LDA #0 */
    STA(APU_DELTA_REG);                      /* STA APU_DELTA_REG */
    reset_score();                           /* JSR reset_score */
    LDA(byte_7D);                            /* LDA byte_7D */
    PHA();                                   /* PHA */
    LDXI(0x0A);                              /* LDX #$A */
    LDAI(0);                                 /* LDA #0 */

loc_E7F6:                                    /* $E7F6 */
    STA(0x0000 + X);                         /* STA 0,X */
    INX();                                   /* INX */
    CPXI(0xF9);                              /* CPX #$F9 */
    BNE(loc_E7F6);                           /* BNE loc_E7F6 */
    sub_CB40();                              /* JSR sub_CB40 */
    PLA();                                   /* PLA */
    STA(byte_7D);                            /* STA byte_7D */
    apu_reset();                             /* JSR apu_reset */
    /* falls through into main_menu_loop ($E806) */
    main_menu_loop();
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E806  main_menu_loop - show the menu, then start whatever it selected
 *
 *  Original: src/bank7.asm:7209-7219
 *      LDX #$FF / TXS / JSR switch_bank_6 / JSR ppu_disable_all /
 *      JSR show_main_menu / JSR switch_bank_6 / JSR apu_stop /
 *      JSR ppu_disable_all / JSR start_game_check
 *  (falls through into sub_E81E)
 * ------------------------------------------------------------------------ */
void main_menu_loop(void)                    /* $E806 */
{
    /* Save states restart here (see zanac_ram.h).  The function re-shows the
     * menu from its first screen, which is exactly what the original does when
     * this loop is entered. */
    zanac_resume_kind = ZANAC_RESUME_MENU;
    LDXI(0xFF);                              /* LDX #$FF */
    TXS();                                   /* TXS */
    switch_bank_6();                         /* JSR switch_bank_6 */
    ppu_disable_all();                       /* JSR ppu_disable_all */
    show_main_menu();                        /* JSR show_main_menu */
    switch_bank_6();                         /* JSR switch_bank_6 */
    apu_stop();                              /* JSR apu_stop */
    ppu_disable_all();                       /* JSR ppu_disable_all */
    start_game_check();                      /* JSR start_game_check */
    /* falls through into sub_E81E ($E81E) */
    sub_E81E();
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E81E  sub_E81E - the level loop (setup, then one frame per vsync)
 *
 *  Original: src/bank7.asm:7226-7301 (falls through from main_menu_loop and is
 *  also reached by JMP from level_completed_sequence)
 * ------------------------------------------------------------------------ */
void sub_E81E(void)                          /* $E81E */
{
    /* Pause save resume: sub_EA0B has just returned; continue the loop after
     * its call. */
    if (zanac_resume_level_after_ea0b) {
        zanac_resume_level_after_ea0b = 0;
        goto loc_E880_after_ea0b;
    }
    /* Resuming a save made inside the level loop: the setup below has already
     * run in the state being restored, so enter the loop directly.  The same
     * branch is used when the save was taken during the setup itself, in which
     * case the flag is not set and the setup is re-run from its first
     * instruction (all of its inputs are in the restored RAM). */
    if (zanac_resume_skip_setup) {
        zanac_resume_skip_setup = 0;
        goto loc_E880;
    }
    zanac_resume_kind = ZANAC_RESUME_LEVEL_SETUP;
    switch_bank_6();                         /* JSR switch_bank_6 */
    ppu_disable_all();                       /* JSR ppu_disable_all */
    clear_oam_buffer();                      /* JSR clear_oam_buffer */
    sub_18006();                             /* JSR sub_18006 */
    j_clear_main_menu_sprites();             /* JSR j_clear_main_menu_sprites */
    j_update_score_onscreen();               /* JSR j_update_score_onscreen */
    sub_CC57();                              /* JSR sub_CC57 */
    LDYI(7);                                 /* LDY #7 */
    LDA(game_mode);                          /* LDA game_mode */
    BPL(loc_E83B);                           /* BPL loc_E83B */
    LDYI(9);                                 /* LDY #9 */

loc_E83B:                                    /* $E83B */
    TYA();                                   /* TYA */
    STA(byte_66);                            /* STA byte_66 */
    sub_DDF9();                              /* JSR sub_DDF9 */
    LDA(last_ppu_ctrl2);                     /* LDA last_ppu_ctrl2 */
    ORAI(0x10);                              /* ORA #$10 */
    ppu_set_control2();                      /* JSR ppu_set_control2 */
    LDYI(0x28);                              /* LDY #$28 */

loc_E84A:                                    /* $E84A */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDYI(1);                                 /* LDY #1 */
    all_enemies_destroyed();                 /* JSR all_enemies_destroyed */
    sub_18009();                             /* JSR sub_18009 */
    sub_18003();                             /* JSR sub_18003 */
    sub_1800C();                             /* JSR sub_1800C */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEY();                                   /* DEY */
    BNE(loc_E84A);                           /* BNE loc_E84A */
    sub_DDF3();                              /* JSR sub_DDF3 */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0x80);                              /* AND #$80 */
    BNE(loc_E876);                           /* BNE loc_E876 */
    LDY(current_level);                      /* LDY current_level */
    LDA(ABSY(level_music_tab));              /* LDA level_music_tab,Y */
    CPYI(1);                                 /* CPY #1 */
    BNE(loc_E873);                           /* BNE loc_E873 */
    LDAI(7);                                 /* LDA #7 */

loc_E873:                                    /* $E873 */
    apu_play();                              /* JSR apu_play */

loc_E876:                                    /* $E876 */
    LDAI(1);                                 /* LDA #1 */
    STA(byte_528);                           /* STA byte_528 */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_5AA);                           /* STA byte_5AA */

loc_E880:                                    /* $E880 */
    /* From here on the per-frame level loop is running: a save state resumes
     * at loc_E880 (see zanac_ram.h).  Setting the kind here (and not once
     * before the label) also covers a resume that rejoins the loop from the
     * pause continuation. */
    zanac_resume_kind = ZANAC_RESUME_LEVEL;
    /* Hand-written save-state boundary (see zanac/savestate.h): the host
     * captures a requested state here, where the whole program state is in
     * RAM/registers and the room has no C call stack.  It is a no-op unless a
     * save is pending. */
    zanac_state_checkpoint();
    sub_D7E3();                              /* JSR sub_D7E3 */
    sub_1801E();                             /* JSR sub_1801E */
    LDYI(1);                                 /* LDY #1 */
    sub_EA0B();                              /* JSR sub_EA0B */

loc_E880_after_ea0b:                         /* resume of a pause save */
    sub_D0BF();                              /* JSR sub_D0BF */
    sub_D480();                              /* JSR sub_D480 */
    LDA(byte_528);                           /* LDA byte_528 */
    CMPI(0x81);                              /* CMP #$81 */
    BNE(loc_E89E);                           /* BNE loc_E89E */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0x30);                              /* AND #$30 */
    BNE(loc_E8C9_call);                      /* BNE sub_E8C9 */
    goto loc_E89E;                           /* falls through into loc_E89E */

loc_E8C9_call:                               /* $E8C9: the branch target is a procedure */
    sub_E8C9();                              /* JSR sub_E8C9 */
    /* The original's sub_E8C9 never returns: it ends with a jump back into
     * this loop.  Follow that jump here (and *not* by calling sub_E81E(),
     * which would re-run the setup above - that reset the level every time
     * the fortress was on screen). */
    if (s_e8c9_jump == ZANAC_E8C9_GOTO_E880) {
        s_e8c9_jump = ZANAC_E8C9_RETURN;
        goto loc_E880;                       /* JMP loc_E880 */
    }
    if (s_e8c9_jump == ZANAC_E8C9_GOTO_E876) {
        s_e8c9_jump = ZANAC_E8C9_RETURN;
        goto loc_E876;                       /* JMP loc_E876 */
    }
    goto loc_E89E;                           /* the JSR's fall-through */

loc_E89E:                                    /* $E89E (also jumped to from sub_E8C9) */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0xBF);                              /* AND #$BF */
    CMP(game_mode);                          /* CMP game_mode */
    BEQ(loc_E880);                           /* BEQ loc_E880 */
    STA(game_mode);                          /* STA game_mode */
    BNE(loc_E876);                           /* BNE loc_E876 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E8AA  sub_E8AA - destroy every remaining object
 *
 *  Original: src/bank7.asm:7308-7328
 *      LDX #5 / LDY #$15 /
 *      loc_E8AE: LDA $528,X / AND #$7F / BEQ loc_E8BA / LDA #$28 /
 *      STA $528,X / loc_E8BA: INX / DEY / BNE loc_E8AE / LDA #0 /
 *      STA byte_C0 / LDA byte_62 / AND #$F7 / STA byte_62 / RTS
 * ------------------------------------------------------------------------ */
void sub_E8AA(void)                          /* $E8AA */
{
    LDXI(5);                                 /* LDX #5 */
    LDYI(0x15);                              /* LDY #$15 */

loc_E8AE:                                    /* $E8AE */
    LDA(ABSX(0x0528));                       /* LDA $528,X */
    ANDI(0x7F);                              /* AND #$7F */
    BEQ(loc_E8BA);                           /* BEQ loc_E8BA */
    LDAI(0x28);                              /* LDA #$28 */
    STA(ABSX(0x0528));                       /* STA $528,X */

loc_E8BA:                                    /* $E8BA */
    INX();                                   /* INX */
    DEY();                                   /* DEY */
    BNE(loc_E8AE);                           /* BNE loc_E8AE */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_C0);                            /* STA byte_C0 */
    LDA(byte_62);                            /* LDA byte_62 */
    ANDI(0xF7);                              /* AND #$F7 */
    STA(byte_62);                            /* STA byte_62 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E8C9  sub_E8C9 - level completed: either warp or the ending
 *
 *  Original: src/bank7.asm:7335-7394 (entered with A = game_mode & $30)
 *      CMP #$10 / BEQ loc_E917 / LDA byte_4B / CLC / ADC #$40 / BCC loc_E8D6 /
 *      LDA #$FF / loc_E8D6: STA byte_4B / LDX #1 /
 *      loc_E8DA: LDA $145,X / AND #$F / TAY / LDA byte_E907,Y /
 *      STA byte_10 / LDA $145,X / AND #$30 / EOR #$30 / ORA byte_10 /
 *      STA $145,X / INX / CPX #$D / BNE loc_E8DA / LDA #3 /
 *      STA cram_update_mode / JSR apu_stop / LDA #11 / JSR apu_play /
 *      LDY #$B3 / BNE level_completed_sequence /
 *      loc_E917: LDA byte_C0 / AND #3 / BNE loc_E89E / LDA game_mode /
 *      AND #8 / BNE loc_E937 / LDA #8 / STA byte_AD / LDA #0 / STA byte_AC /
 *      JSR sub_F146 /
 *      loc_E92E: JSR sub_E995 / LDA byte_AB / CMP #4 / BCC loc_E92E /
 *      loc_E937: LDY #$FF
 *  (falls through into level_completed_sequence)
 * ------------------------------------------------------------------------ */
void sub_E8C9(void)                          /* $E8C9 */
{
    s_e8c9_jump = ZANAC_E8C9_RETURN;         /* no tail jump unless taken below */
    CMPI(0x10);                              /* CMP #$10 */
    BEQ(loc_E917);                           /* BEQ loc_E917 */
    LDA(byte_4B);                            /* LDA byte_4B */
    CLC();                                   /* CLC */
    ADCI(0x40);                              /* ADC #$40 */
    BCC(loc_E8D6);                           /* BCC loc_E8D6 */
    LDAI(0xFF);                              /* LDA #$FF */

loc_E8D6:                                    /* $E8D6 */
    STA(byte_4B);                            /* STA byte_4B */
    LDXI(1);                                 /* LDX #1 */

loc_E8DA:                                    /* $E8DA */
    LDA(ABSX(0x0145));                       /* LDA $145,X */
    ANDI(0x0F);                              /* AND #$F */
    TAY();                                   /* TAY */
    LDA(ABSY(byte_E907));                    /* LDA byte_E907,Y */
    STA(byte_10);                            /* STA byte_10 */
    LDA(ABSX(0x0145));                       /* LDA $145,X */
    ANDI(0x30);                              /* AND #$30 */
    EORI(0x30);                              /* EOR #$30 */
    ORA(byte_10);                            /* ORA byte_10 */
    STA(ABSX(0x0145));                       /* STA $145,X */
    INX();                                   /* INX */
    CPXI(0x0D);                              /* CPX #$D */
    BNE(loc_E8DA);                           /* BNE loc_E8DA */
    LDAI(3);                                 /* LDA #3 */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    apu_stop();                              /* JSR apu_stop */
    LDAI(11);                                /* LDA #11 */
    apu_play();                              /* JSR apu_play */
    LDYI(0xB3);                              /* LDY #$B3 */
    BNE(loc_level_E937);                     /* BNE level_completed_sequence */

loc_E917:                                    /* $E917 */
    LDA(byte_C0);                            /* LDA byte_C0 */
    ANDI(3);                                 /* AND #3 */
    BNE(loc_E89E);                           /* BNE loc_E89E (inside sub_E81E) */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(8);                                 /* AND #8 */
    BNE(loc_E937);                           /* BNE loc_E937 */
    LDAI(8);                                 /* LDA #8 */
    STA(byte_AD);                            /* STA byte_AD */
    LDAI(0);                                 /* LDA #0 */
    STA(byte_AC);                            /* STA byte_AC */
    sub_F146();                              /* JSR sub_F146 */

loc_E92E:                                    /* $E92E */
    sub_E995();                              /* JSR sub_E995 */
    LDA(byte_AB);                            /* LDA byte_AB */
    CMPI(4);                                 /* CMP #4 */
    BCC(loc_E92E);                           /* BCC loc_E92E */

loc_E937:                                    /* $E937 */
    LDYI(0xFF);                              /* LDY #$FF */
    /* falls through into level_completed_sequence ($E939) */
    level_completed_sequence();
    RTS();

loc_E89E:                                    /* $E89E (inside sub_E81E) */
    /* $E8C9 ends with JMP loc_E89E, which is in the middle of sub_E81E.  The
     * original never calls sub_E81E's prologue again from here: it continues
     * with the next frame of the level loop, so report the target to the
     * caller instead of entering sub_E81E() (see the top of this file). */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0xBF);                              /* AND #$BF */
    CMP(game_mode);                          /* CMP game_mode */
    if (F_Z) {                               /* BEQ loc_E880 */
        s_e8c9_jump = ZANAC_E8C9_GOTO_E880;
        RTS();
    }
    STA(game_mode);                          /* STA game_mode */
    s_e8c9_jump = ZANAC_E8C9_GOTO_E876;      /* BNE loc_E876 (always taken) */
    RTS();

loc_level_E937:                              /* $E937 (the LDY #$FF path) */
    level_completed_sequence();
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E939  level_completed_sequence - the level-complete countdown/warp
 *
 *  Original: src/bank7.asm:7402-7460
 *      TYA / PHA / AND #$1F / BNE loc_E953 / CPY #$C0 / BCS loc_E953 /
 *      LDA game_mode / AND #$10 / BNE loc_E950 / CPY #$61 / BCS loc_E950 /
 *      JSR fade_palette_before_warp /
 *      loc_E950: JSR fade_palette_before_warp_another /
 *      loc_E953: JSR sub_E995 / PLA / TAY / CPY #$B4 / BNE loc_E964 /
 *      JSR apu_stop / LDA #30 / JSR apu_play /
 *      loc_E964: DEY / BNE level_completed_sequence /
 *      LDA warp_sequence_counter / ORA #1 / STA warp_sequence_counter /
 *      loc_E96D: LDA #$81 / STA byte_528 / LDY #1 / JSR sub_EA0B /
 *      LDA byte_528 / BNE loc_E96D / JSR sub_E8AA / LDA game_mode /
 *      AND #$CF / STA game_mode / AND #8 / BEQ loc_E98C /
 *      JMP show_ending_screen /
 *      loc_E98C: LDA byte_92 / STA current_level / STA byte_7D /
 *      JMP sub_E81E
 * ------------------------------------------------------------------------ */
void level_completed_sequence(void)          /* $E939 */
{
loc_E939:                                    /* $E939 (BNE from the countdown) */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    ANDI(0x1F);                              /* AND #$1F */
    BNE(loc_E953);                           /* BNE loc_E953 */
    CPYI(0xC0);                              /* CPY #$C0 */
    BCS(loc_E953);                           /* BCS loc_E953 */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0x10);                              /* AND #$10 */
    BNE(loc_E950);                           /* BNE loc_E950 */
    CPYI(0x61);                              /* CPY #$61 */
    BCS(loc_E950);                           /* BCS loc_E950 */
    fade_palette_before_warp();              /* JSR fade_palette_before_warp */

loc_E950:                                    /* $E950 */
    fade_palette_before_warp_another();      /* JSR fade_palette_before_warp_another */

loc_E953:                                    /* $E953 */
    sub_E995();                              /* JSR sub_E995 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    CPYI(0xB4);                              /* CPY #$B4 */
    BNE(loc_E964);                           /* BNE loc_E964 */
    apu_stop();                              /* JSR apu_stop */
    LDAI(30);                                /* LDA #30 (level complete music) */
    apu_play();                              /* JSR apu_play */

loc_E964:                                    /* $E964 */
    DEY();                                   /* DEY */
    BNE(loc_E939);                           /* BNE level_completed_sequence */
    LDA(warp_sequence_counter);              /* LDA warp_sequence_counter */
    ORAI(1);                                 /* ORA #1 */
    STA(warp_sequence_counter);              /* STA warp_sequence_counter */

loc_E96D:                                    /* $E96D */
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA byte_528 */
    LDYI(1);                                 /* LDY #1 */
    sub_EA0B();                              /* JSR sub_EA0B */
    LDA(byte_528);                           /* LDA byte_528 */
    BNE(loc_E96D);                           /* BNE loc_E96D */
    sub_E8AA();                              /* JSR sub_E8AA */
    LDA(game_mode);                          /* LDA game_mode */
    ANDI(0xCF);                              /* AND #$CF */
    STA(game_mode);                          /* STA game_mode */
    ANDI(8);                                 /* AND #8 */
    BEQ(loc_E98C);                           /* BEQ loc_E98C */
    show_ending_screen();                    /* JMP show_ending_screen */
    RTS();

loc_E98C:                                    /* $E98C */
    LDA(byte_92);                            /* LDA byte_92 */
    STA(current_level);                      /* STA current_level */
    STA(byte_7D);                            /* STA byte_7D */
    sub_E81E();                              /* JMP sub_E81E */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E995  sub_E995 - one frame of the "level complete" pause
 *
 *  Original: src/bank7.asm:7466-7472
 *      LDA #$81 / STA byte_528 / JSR sub_D7E3 / LDY #1 / JMP sub_EA0B
 * ------------------------------------------------------------------------ */
void sub_E995(void)                          /* $E995 */
{
    LDAI(0x81);                              /* LDA #$81 */
    STA(byte_528);                           /* STA byte_528 */
    sub_D7E3();                              /* JSR sub_D7E3 */
    LDYI(1);                                 /* LDY #1 */
    sub_EA0B();                              /* JMP sub_EA0B */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E9A2  fade_palette_before_warp_another - fade the palette (mode 1)
 *
 *  Original: src/bank7.asm:7480-7483
 *      LDY #1 / LDA #1 / BNE loc_E9AC
 *  (falls through into fade_palette_before_warp at loc_E9AC)
 * ------------------------------------------------------------------------ */
void fade_palette_before_warp_another(void)  /* $E9A2 */
{
    LDYI(1);                                 /* LDY #1 */
    LDAI(1);                                 /* LDA #1 */
    /* BNE loc_E9AC falls into the shared body below, which lives inside
     * fade_palette_before_warp at $E9AC; it is reproduced here (PORTING.md
     * section 5) and documents the shared entry. */
    goto loc_E9AC;

loc_E9AC:                                    /* $E9AC */
    PHA();                                   /* PHA */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_10);                            /* STA byte_10 */

loc_E9B1:                                    /* $E9B1 */
    LDA(ABSY(0x0145));                       /* LDA $145,Y */
    CMPI(0x10);                              /* CMP #$10 */
    BCC(loc_E9BC);                           /* BCC loc_E9BC */
    SBCI(0x10);                              /* SBC #$10 */
    BCS(loc_E9C0);                           /* BCS loc_E9C0 */

loc_E9BC:                                    /* $E9BC */
    TAX();                                   /* TAX */
    LDA(ABSX(unk_E9D0));                     /* LDA unk_E9D0,X */

loc_E9C0:                                    /* $E9C0 */
    STA(ABSY(0x0145));                       /* STA $145,Y */
    INY();                                   /* INY */
    DEC(byte_10);                            /* DEC byte_10 */
    BNE(loc_E9B1);                           /* BNE loc_E9B1 */
    PLA();                                   /* PLA */
    ORA(cram_update_mode);                   /* ORA cram_update_mode */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E9A8  fade_palette_before_warp - shade the palette before a warp
 *
 *  Original: src/bank7.asm:7491-7519
 *      LDY #$D / LDA #2 /
 *      loc_E9AC: PHA / LDA #$C / STA byte_10 /
 *      loc_E9B1: LDA $145,Y / CMP #$10 / BCC loc_E9BC / SBC #$10 /
 *      BCS loc_E9C0 / loc_E9BC: TAX / LDA unk_E9D0,X /
 *      loc_E9C0: STA $145,Y / INY / DEC byte_10 / BNE loc_E9B1 / PLA /
 *      ORA cram_update_mode / STA cram_update_mode / RTS
 * ------------------------------------------------------------------------ */
void fade_palette_before_warp(void)          /* $E9A8 */
{
    LDYI(0x0D);                              /* LDY #$D */
    LDAI(2);                                 /* LDA #2 */
    /* loc_E9AC ($E9AC) is the shared entry: fade_palette_before_warp_another
     * branches into it with its own Y and A (see that function). */
    PHA();                                   /* PHA */
    LDAI(0x0C);                              /* LDA #$C */
    STA(byte_10);                            /* STA byte_10 */

loc_E9B1:                                    /* $E9B1 */
    LDA(ABSY(0x0145));                       /* LDA $145,Y */
    CMPI(0x10);                              /* CMP #$10 */
    BCC(loc_E9BC);                           /* BCC loc_E9BC */
    SBCI(0x10);                              /* SBC #$10 */
    BCS(loc_E9C0);                           /* BCS loc_E9C0 */

loc_E9BC:                                    /* $E9BC */
    TAX();                                   /* TAX */
    LDA(ABSX(unk_E9D0));                     /* LDA unk_E9D0,X */

loc_E9C0:                                    /* $E9C0 */
    STA(ABSY(0x0145));                       /* STA $145,Y */
    INY();                                   /* INY */
    DEC(byte_10);                            /* DEC byte_10 */
    BNE(loc_E9B1);                           /* BNE loc_E9B1 */
    PLA();                                   /* PLA */
    ORA(cram_update_mode);                   /* ORA cram_update_mode */
    STA(cram_update_mode);                   /* STA cram_update_mode */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $E9E0  update_music_after_fortress_destroy - pick the next level's music
 *
 *  Original: src/bank7.asm:7543-7565
 *      LDA game_mode / BPL loc_E9E5 / RTS /
 *      loc_E9E5: LDY current_level / LDA level_music_tab,Y / CPY #10 /
 *      BNE loc_E9FB / LDA pad_buttons_1 / AND #$20 / BNE loc_E9F9 /
 *      LDA level_music_tab,Y / BNE loc_E9FB / loc_E9F9: LDA #33 /
 *      loc_E9FB: JMP apu_play
 * ------------------------------------------------------------------------ */
void update_music_after_fortress_destroy(void)  /* $E9E0 */
{
    LDA(game_mode);                          /* LDA game_mode */
    BPL(loc_E9E5);                           /* BPL loc_E9E5 */
    RTS();

loc_E9E5:                                    /* $E9E5 */
    LDY(current_level);                      /* LDY current_level */
    LDA(ABSY(level_music_tab));              /* LDA level_music_tab,Y */
    CPYI(10);                                /* CPY #10 */
    BNE(loc_E9FB);                           /* BNE loc_E9FB */
    LDA(pad_buttons_1);                      /* LDA pad_buttons_1 */
    ANDI(0x20);                              /* AND #$20 (SELECT) */
    BNE(loc_E9F9);                           /* BNE loc_E9F9 */
    LDA(ABSY(level_music_tab));              /* LDA level_music_tab,Y */
    BNE(loc_E9FB);                           /* BNE loc_E9FB */

loc_E9F9:                                    /* $E9F9 */
    LDAI(33);                                /* LDA #33 (secret music) */

loc_E9FB:                                    /* $E9FB */
    apu_play();                              /* JMP apu_play */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EA0B  sub_EA0B - run Y frames of the game loop
 *
 *  Original: src/bank7.asm:7586-7617
 *      TXA / PHA /
 *      loc_EA0D: TYA / PHA / JSR vsync / JSR pad_read / LDA game_mode /
 *      BPL loc_EA22 / LDA pad_test1 / AND #$10 / BEQ loc_EA25 /
 *      JMP main_menu_loop / loc_EA22: JSR sub_D3A8 /
 *      loc_EA25: JSR j_game_logic_processing / JSR clear_first_last_sprites /
 *      JSR sub_D0A4 / PLA / TAY / DEY / BNE loc_EA0D / PLA / TAX / RTS
 * ------------------------------------------------------------------------ */
void sub_EA0B(void)                          /* $EA0B */
{
    /* Pause save resume: the caller (zanac_state_resume) has already run
     * sub_D3A8 at loc_D3B9; continue where sub_D3A8 returned (loc_EA25).  The
     * emulated stack still holds this procedure's pushed X and Y. */
    if (zanac_resume_ea0b_after_pause) {
        zanac_resume_ea0b_after_pause = 0;
        goto loc_EA25;
    }
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */

loc_EA0D:                                    /* $EA0D */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    vsync();                                 /* JSR vsync */
    pad_read();                              /* JSR pad_read */
    LDA(game_mode);                          /* LDA game_mode */
    BPL(loc_EA22);                           /* BPL loc_EA22 */
    LDA(pad_test1);                          /* LDA pad_test1 */
    ANDI(0x10);                              /* AND #$10 (START) */
    BEQ(loc_EA25);                           /* BEQ loc_EA25 */
    main_menu_loop();                        /* JMP main_menu_loop */
    RTS();

loc_EA22:                                    /* $EA22 */
    sub_D3A8();                              /* JSR sub_D3A8 */

loc_EA25:                                    /* $EA25 */
    j_game_logic_processing();               /* JSR j_game_logic_processing */
    clear_first_last_sprites();              /* JSR clear_first_last_sprites */
    sub_D0A4();                              /* JSR sub_D0A4 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    DEY();                                   /* DEY */
    BNE(loc_EA0D);                           /* BNE loc_EA0D */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EA36  all_enemies_destroyed - wait Y frames
 *
 *  Original: src/bank7.asm:7625-7630
 *      JSR vsync / DEY / BNE all_enemies_destroyed / RTS
 * ------------------------------------------------------------------------ */
void all_enemies_destroyed(void)             /* $EA36 */
{
loc_EA36:                                    /* $EA36 */
    vsync();                                 /* JSR vsync */
    DEY();                                   /* DEY */
    BNE(loc_EA36);                           /* BNE all_enemies_destroyed */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EA52  apu_update - the music engine, called from the VBlank handler
 *
 *  Original: src/bank7.asm:7701-7854.  It walks six channel slots ($$31A,X),
 *  updates their state and finally flushes the shadow registers $309-$318 to
 *  $4000-$400F.
 * ------------------------------------------------------------------------ */
void apu_update(void)                        /* $EA52 */
{
    LDA(byte_308);                           /* LDA byte_308 */
    BNE(locret_EA90);                        /* BNE locret_EA90 */
    LDA(byte_302);                           /* LDA byte_302 */
    ANDI(0xFE);                              /* AND #$FE */
    CMP(byte_302);                           /* CMP byte_302 */
    BEQ(loc_EA7A);                           /* BEQ loc_EA7A */
    ORAI(0x80);                              /* ORA #$80 */
    STA(byte_302);                           /* STA byte_302 */
    LDAI(0);                                 /* LDA #0 */
    sub_EFA6();                              /* JSR sub_EFA6 */
    LDXI(0);                                 /* LDX #0 */

loc_EA6D:                                    /* $EA6D */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    ORAI(0x40);                              /* ORA #$40 */
    STA(ABSX(0x03EC));                       /* STA $3EC,X */
    INX();                                   /* INX */
    CPXI(6);                                 /* CPX #6 */
    BNE(loc_EA6D);                           /* BNE loc_EA6D */

loc_EA7A:                                    /* $EA7A */
    LDA(byte_302);                           /* LDA byte_302 */
    BPL(loc_EA89);                           /* BPL loc_EA89 */
    LDX(byte_31F);                           /* LDX byte_31F */
    BNE(loc_EA91);                           /* BNE loc_EA91 */
    ANDI(0x7F);                              /* AND #$7F */
    STA(byte_302);                           /* STA byte_302 */

loc_EA89:                                    /* $EA89 */
    LDA(byte_302);                           /* LDA byte_302 */
    ANDI(2);                                 /* AND #2 */
    BEQ(loc_EA91);                           /* BEQ loc_EA91 */

locret_EA90:                                 /* $EA90 */
    RTS();

loc_EA91:                                    /* $EA91 */
    sub_EE94();                              /* JSR sub_EE94 */
    LDYI(4);                                 /* LDY #4 */
    LDA(byte_302);                           /* LDA byte_302 */
    BPL(loc_EA9C);                           /* BPL loc_EA9C */
    INY();                                   /* INY */

loc_EA9C:                                    /* $EA9C */
    STY(byte_303);                           /* STY byte_303 */

loc_EA9F:                                    /* $EA9F */
    LDX(byte_303);                           /* LDX byte_303 */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    BNE(loc_EAAA);                           /* BNE loc_EAAA */
    goto loc_EB17;                           /* JMP loc_EB17 */

loc_EAAA:                                    /* $EAAA */
    TAY();                                   /* TAY */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    ANDI(0x7F);                              /* AND #$7F */
    STA(ABSX(0x03EC));                       /* STA $3EC,X */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(1);                                 /* AND #1 */
    BEQ(loc_EAC6);                           /* BEQ loc_EAC6 */
    LDA(ABSX(0x0386));                       /* LDA $386,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x038C));                       /* ADC $38C,X */
    STA(ABSX(0x038C));                       /* STA $38C,X */
    BCC(loc_EADF);                           /* BCC loc_EADF */

loc_EAC6:                                    /* $EAC6 */
    DEC(ABSX(0x0368));                       /* DEC $368,X */
    BNE(loc_EADF);                           /* BNE loc_EADF */
    LDA(ABSX(0x0338));                       /* LDA $338,X */
    STA(ABSX(0x0368));                       /* STA $368,X */
    INC(ABSX(0x0356));                       /* INC $356,X */
    LDA(ABSX(0x0356));                       /* LDA $356,X */
    CMP(ABSX(0x035C));                       /* CMP $35C,X */
    BNE(loc_EADF);                           /* BNE loc_EADF */
    sub_EB7A();                              /* JSR sub_EB7A */

loc_EADF:                                    /* $EADF */
    sub_EDFC();                              /* JSR sub_EDFC */
    sub_EEAA();                              /* JSR sub_EEAA */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_EB17);                           /* BEQ loc_EB17 */
    LDA(ABSX(0x03AA));                       /* LDA $3AA,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x03B0));                       /* ADC $3B0,X */
    STA(ABSX(0x03B0));                       /* STA $3B0,X */
    BCC(loc_EB17);                           /* BCC loc_EB17 */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0x0F);                              /* AND #$F */
    CMP(ABSX(0x03B6));                       /* CMP $3B6,X */
    BNE(loc_EB0C);                           /* BNE loc_EB0C */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ORAI(2);                                 /* ORA #2 */
    STA(ABSX(0x0350));                       /* STA $350,X */
    BNE(loc_EB17);                           /* BNE loc_EB17 */

loc_EB0C:                                    /* $EB0C */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(0xFD);                              /* AND #$FD */
    STA(ABSX(0x0350));                       /* STA $350,X */
    DEC(ABSX(0x0320));                       /* DEC $320,X */

loc_EB17:                                    /* $EB17 */
    LDA(byte_302);                           /* LDA byte_302 */
    BMI(loc_EB25);                           /* BMI loc_EB25 */
    DEX();                                   /* DEX */
    STX(byte_303);                           /* STX byte_303 */
    BMI(loc_EB25);                           /* BMI loc_EB25 */
    goto loc_EA9F;                           /* JMP loc_EA9F */

loc_EB25:                                    /* $EB25 */
    LDA(byte_307);                           /* LDA byte_307 */
    STA(APU_MASTERCTRL_REG);                 /* STA APU_MASTERCTRL_REG */
    LDXI(0x0F);                              /* LDX #$F */

loc_EB2D:                                    /* $EB2D */
    LDA(ABSX(0x0309));                       /* LDA $309,X */
    CMPI(0xFF);                              /* CMP #$FF */
    BCS(loc_EB37);                           /* BCS loc_EB37 */
    STA(ABSX(APU_REG_BASE));                 /* STA APU_REG_BASE,X */

loc_EB37:                                    /* $EB37 */
    DEX();                                   /* DEX */
    BCS(loc_EB40);                           /* BCS loc_EB40 */
    LDA(ABSX(0x0309));                       /* LDA $309,X */
    STA(ABSX(APU_REG_BASE));                 /* STA APU_REG_BASE,X */

loc_EB40:                                    /* $EB40 */
    DEX();                                   /* DEX */
    BCS(loc_EB4D);                           /* BCS loc_EB4D */
    LDA(ABSX(0x0309));                       /* LDA $309,X */
    BMI(loc_EB4A);                           /* BMI loc_EB4A */
    LDAI(8);                                 /* LDA #8 */

loc_EB4A:                                    /* $EB4A */
    STA(ABSX(APU_REG_BASE));                 /* STA APU_REG_BASE,X */

loc_EB4D:                                    /* $EB4D */
    DEX();                                   /* DEX */
    LDA(ABSX(0x0309));                       /* LDA $309,X */
    STA(ABSX(APU_REG_BASE));                 /* STA APU_REG_BASE,X */
    DEX();                                   /* DEX */
    BPL(loc_EB2D);                           /* BPL loc_EB2D */
    RTS();
}
