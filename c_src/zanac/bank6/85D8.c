/* bank6/85D8.c - bank 6, $85D8-$9020: the sprite-output helpers, the enemy
 * pseudo-random generator, the object-slot helpers, the enemy dispatcher and
 * the Zanac (player) handlers.
 *
 * Ported from src/bank6.asm lines 1240-3367 (addresses from Build/bank6.lst).
 *
 * Computed control flow and deviations in this chunk (PORTING.md 4c):
 *
 *  - game_logic_processing ($87C7) dispatches the enemy handlers through the
 *    two byte tables $8618/$8673 (low/high byte of handler-1, because the RTS
 *    adds one).  The original pushes $8806 with two PHAs, pushes the handler
 *    address (high byte then low byte) and executes the RTS at locret_18806
 *    ($8806).  The port keeps the same table reads and replaces the computed
 *    RTS with zanac_call_at((hi<<8|lo)+1); returning from the called handler is
 *    the original "handler RTS pops $8806 and resumes at loc_18807", i.e. the
 *    port simply continues with the next slot.
 *  - sub_1876A ($876A, loc_18779) unwinds its own JSR frame with PLA/PLA and
 *    jumps to enemy_erase, so that enemy_erase's RTS resumes the dispatcher's
 *    slot loop ($8807) and the calling handler is abandoned.  A C call leaves
 *    no return address on the emulated stack, so the port cannot discard a
 *    frame with PLA; enemy_erase() + RTS() performs the erase and returns to
 *    the calling handler (documented at loc_18779).
 *  - enemy_prng ($872B) reads the 6502 stack pointer with TSX/TXA as an entropy
 *    source.  The port models SP, but C calls do not push return addresses on
 *    the emulated stack, so that byte differs from the original (documented at
 *    the TSX).
 *  - add_sprite ($85D8) ends by jumping into the middle of add_next_sprite
 *    ($8435 / $8477); that shared tail is reproduced here with a comment.
 *  - zanac_main_handler ($8CA4) is entered by JMP from several handlers and
 *    falls through into sub_18E29 at $8E29.
 */
#include <stdio.h>
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $85D8  add_sprite - append one meta-sprite from ($18),Y to the $200 buffer
 *
 *  Original: src/bank6.asm:1240-1265
 *      STX $65 / LDA $83 / AND #2 / BEQ loc_185E3 / JMP loc_18435
 *  loc_185E3:
 *      LDX $80 / LDA ($18),Y / STA $200,X / INX / INY   (three times) /
 *      LDA ($18),Y / JMP loc_18477
 *
 *  loc_18435 and loc_18477 are inside add_next_sprite ($8412); add_sprite
 *  jumps into that shared tail, which is reproduced below (PORTING.md 5).
 * ------------------------------------------------------------------------ */
void add_sprite(void)                          /* $85D8 */
{
    STX(byte_65);                              /* STX $65 */
    LDA(byte_83);                              /* LDA $83 */
    ANDI(0x02);                                /* AND #2 */
    BEQ(loc_185E3);                            /* BEQ loc_185E3 */
    goto loc_18435;                            /* JMP loc_18435 (inside add_next_sprite) */

loc_185E3:                                     /* $85E3 */
    LDX(unk_80);                               /* LDX $80 */
    LDA(INDY(byte_18));                        /* LDA ($18),Y */
    STA(ABSX(0x0200));                         /* STA $200,X */
    INX();                                     /* INX */
    INY();                                     /* INY */
    LDA(INDY(byte_18));                        /* LDA ($18),Y */
    STA(ABSX(0x0200));                         /* STA $200,X */
    INX();                                     /* INX */
    INY();                                     /* INY */
    LDA(INDY(byte_18));                        /* LDA ($18),Y */
    STA(ABSX(0x0200));                         /* STA $200,X */
    INX();                                     /* INX */
    INY();                                     /* INY */
    LDA(INDY(byte_18));                        /* LDA ($18),Y */
    goto loc_18477;                            /* JMP loc_18477 (inside add_next_sprite) */

    /* ---- tail shared with add_next_sprite ($8435, $8477-$84A4) ---------- */
loc_18435:                                     /* $8435 */
    INY();                                     /* INY */
    INY();                                     /* INY */
    INY();                                     /* INY */
    INY();                                     /* INY */
    RTS();                                     /* RTS */

loc_18477:                                     /* $8477 */
    INY();                                     /* INY */
    STA(ABSX(0x0200));                         /* STA $200,X */
    LDA(byte_83);                              /* LDA $83 */
    ANDI(0x01);                                /* AND #1 */
    BNE(loc_18490);                            /* BNE loc_18490 */
    INX();                                     /* INX */
    STX(unk_80);                               /* STX $80 */
    CPXI(0xF0);                                /* CPX #$F0 */
    BNE(loc_184A4);                            /* BNE loc_184A4 */
    LDA(byte_83);                              /* LDA $83 */
    ORAI(0x82);                                /* ORA #$82 */
    STA(byte_83);                              /* STA $83 */
    BNE(loc_184A4);                            /* BNE loc_184A4 */

loc_18490:                                     /* $8490 */
    TXA();                                     /* TXA */
    SEC();                                     /* SEC */
    SBCI(7);                                   /* SBC #7 */
    STA(unk_80);                               /* STA $80 */
    CMPI(0x10);                                /* CMP #$10 */
    BCS(loc_184A4);                            /* BCS loc_184A4 */
    LDA(byte_83);                              /* LDA $83 */
    ORAI(0x80);                                /* ORA #$80 */
    STA(byte_83);                              /* STA $83 */
    LDAI(0xEC);                                /* LDA #$EC */
    STA(unk_80);                               /* STA $80 */

loc_184A4:                                     /* $84A4 */
    LDX(byte_65);                              /* LDX $65 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $85FF  add_more_sprites - add the count in A sprites from the ($18) table
 *
 *  Original: src/bank6.asm:1272-1281
 *      LDA ($18),Y / STA $14 / INY
 *  loc_18604:
 *      JSR add_sprite / DEC $14 / BNE loc_18604 / RTS
 * ------------------------------------------------------------------------ */
void add_more_sprites(void)                    /* $85FF */
{
    LDA(INDY(byte_18));                        /* LDA ($18),Y */
    STA(byte_14);                              /* STA $14 */
    INY();                                     /* INY */

loc_18604:                                     /* $8604 */
    add_sprite();                              /* JSR add_sprite */
    DEC(byte_14);                              /* DEC $14 */
    BNE(loc_18604);                            /* BNE loc_18604 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $860C  add_next_sprite_from_zeropage - point $18/$19 at $84, then
 *                                        add_next_sprite
 *
 *  Original: src/bank6.asm:1288-1298
 *      LDA #0 / STA byte_19 / LDA #$84 / STA byte_18 / LDY #0
 *  loc_18616:
 *      JMP add_next_sprite
 * ------------------------------------------------------------------------ */
void add_next_sprite_from_zeropage(void)       /* $860C */
{
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_19);                              /* STA byte_19 */
    LDAI(0x84);                                /* LDA #$84 */
    STA(byte_18);                              /* STA byte_18 */
    LDYI(0);                                   /* LDY #0 */
    goto loc_18616;                            /* falls through into loc_18616 */

loc_18616:                                     /* $8616 */
    add_next_sprite();                         /* JMP add_next_sprite */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $872B  enemy_prng - enemy pseudo-random generator
 *
 *  Original: src/bank6.asm:1580-1597
 *      STX $10 / LDX $7E / SEC / ROL / EOR tab_18619_lo,X / ADC $10,X /
 *      STA $7E / TSX / TXA / LDX $10 / ADC $7E / ADC $528,X / ADC $A2 /
 *      STA $7E / RTS
 *
 *  In: A = entropy, X = slot index; out: A = random byte, X restored, $7E
 *  updated.  The dispatch tables at $8618/$8674 are reused as an entropy pool
 *  and TSX/TXA uses the 6502 stack pointer as an entropy byte; the port models
 *  SP exactly, but because C calls do not push return addresses onto the
 *  emulated stack that byte is not the same as on the 6502 (deviation).
 * ------------------------------------------------------------------------ */
void enemy_prng(void)                          /* $872B */
{
    STX(byte_10);                              /* STX $10 */
    LDX(last_randomize_result);                /* LDX $7E */
    SEC();                                     /* SEC */
    ROL_A();                                   /* ROL */
    EOR(ABSX(tab_18619_lo));                   /* EOR tab_18619_lo,X */
    ADC(ZPX(byte_10));                         /* ADC $10,X */
    STA(last_randomize_result);                /* STA $7E */
    TSX();                                     /* TSX (stack pointer as entropy) */
    TXA();                                     /* TXA */
    LDX(byte_10);                              /* LDX $10 */
    ADC(last_randomize_result);                /* ADC $7E */
    ADC(ABSX(byte_528));                       /* ADC $528,X */
    ADC(byte_A2);                              /* ADC $A2 */
    STA(last_randomize_result);                /* STA $7E */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8746  spawn_top_random_x_position - random X, Y = 8 at the screen top
 *
 *  Original: src/bank6.asm:1604-1623
 *      JSR enemy_prng / STY $10 / AND #$7F / STA $55C,X / ROR / TAY /
 *      ADC tab_18619_lo,Y / STA $7E / AND #$3F / CLC / ADC $55C,X /
 *      ADC #$20 / STA $55C,X / LDA #8 / STA $542,X / LDY $10 / RTS
 * ------------------------------------------------------------------------ */
void spawn_top_random_x_position(void)         /* $8746 */
{
    enemy_prng();                              /* JSR enemy_prng */
    STY(byte_10);                              /* STY $10 */
    ANDI(0x7F);                                /* AND #$7F */
    STA(ABSX(byte_55C));                       /* STA $55C,X */
    ROR_A();                                   /* ROR */
    TAY();                                     /* TAY */
    ADC(ABSY(tab_18619_lo));                   /* ADC tab_18619_lo,Y */
    STA(last_randomize_result);                /* STA $7E */
    ANDI(0x3F);                                /* AND #$3F */
    CLC();                                     /* CLC */
    ADC(ABSX(byte_55C));                       /* ADC $55C,X */
    ADCI(0x20);                                /* ADC #$20 */
    STA(ABSX(byte_55C));                       /* STA $55C,X */
    LDAI(0x08);                                /* LDA #8 */
    STA(ABSX(byte_542));                       /* STA $542,X */
    LDY(byte_10);                              /* LDY $10 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $876A  sub_1876A - spawn enemy type 39 in the first free slot
 *
 *  Original: src/bank6.asm:1629-1644
 *      JSR check_enemy_overflow / BCS loc_18779 / TYA / STA $764,X /
 *      LDA #39 / STA $528,Y / RTS
 *  loc_18779:
 *      PLA / PLA / JMP enemy_erase
 *
 *  The original's PLA/PLA discards this routine's own JSR frame, so that
 *  enemy_erase's RTS returns to the dispatcher's slot loop ($8807) and the
 *  calling handler is abandoned.  A C call leaves no return address on the
 *  emulated stack, so the port omits the two PLAs; enemy_erase() + RTS() does
 *  the erase.  Deviation: control returns to the calling handler instead of
 *  jumping to the slot loop.
 * ------------------------------------------------------------------------ */
void sub_1876A(void)                           /* $876A */
{
    check_enemy_overflow();                    /* JSR check_enemy_overflow */
    BCS(loc_18779);                            /* BCS loc_18779 */
    TYA();                                     /* TYA */
    STA(ABSX(byte_764));                       /* STA $764,X */
    LDAI(39);                                  /* LDA #39 */
    STA(ABSY(byte_528));                       /* STA $528,Y */
    RTS();                                     /* RTS */

loc_18779:                                     /* $8779 */
    /* PLA / PLA / JMP enemy_erase: the two pops discard this routine's JSR
     * frame, so enemy_erase's RTS resumes the dispatcher's slot loop ($8807)
     * and the calling handler is abandoned - the port's longjmp. */
    enemy_erase();                             /* JMP enemy_erase */
    ZANAC_UNWIND();                            /* the RTS of enemy_erase */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $877E  sub_1877E - put the object whose slot is in $764,X at Y = 2
 *
 *  Original: src/bank6.asm:1650-1655
 *      LDA $764,X / TAY / LDA #2 / STA $542,Y / RTS
 * ------------------------------------------------------------------------ */
void sub_1877E(void)                           /* $877E */
{
    LDA(ABSX(byte_764));                       /* LDA $764,X */
    TAY();                                     /* TAY */
    LDAI(2);                                   /* LDA #2 */
    STA(ABSY(byte_542));                       /* STA $542,Y */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8788  sub_18788 - increment the $47 counter unless it has wrapped
 *
 *  Original: src/bank6.asm:1662-1677
 *      INC $47 / BNE loc_1878F / DEC $47
 *  locret_1878E:
 *      RTS
 *  loc_1878F:
 *      LDA $62 / ORA #1 / STA $62 / RTS
 * ------------------------------------------------------------------------ */
void sub_18788(void)                           /* $8788 */
{
    INC(byte_47);                              /* INC $47 */
    BNE(loc_1878F);                            /* BNE loc_1878F */
    DEC(byte_47);                              /* DEC $47 */
    goto locret_1878E;                         /* falls through into the shared RTS $878E */

locret_1878E:                                  /* $878E */
    RTS();                                     /* RTS */

loc_1878F:                                     /* $878F */
    LDA(byte_62);                              /* LDA $62 */
    ORAI(0x01);                                /* ORA #1 */
    STA(byte_62);                              /* STA $62 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8796  sub_18796 - decrement the $47 counter unless it is zero
 *
 *  Original: src/bank6.asm:1684-1689
 *      LDA $47 / BEQ locret_1878E / DEC $47 / JMP loc_1878F
 *
 *  locret_1878E and loc_1878F are inside sub_18788; the shared tail is
 *  reproduced here with a comment pointing at $878E/$878F.
 * ------------------------------------------------------------------------ */
void sub_18796(void)                           /* $8796 */
{
    LDA(byte_47);                              /* LDA $47 */
    BEQ(locret_1878E);                         /* BEQ locret_1878E (inside sub_18788) */
    DEC(byte_47);                              /* DEC $47 */
    /* JMP loc_1878F: tail of sub_18788, reproduced here. */
    LDA(byte_62);                              /* LDA $62 */
    ORAI(0x01);                                /* ORA #1 */
    STA(byte_62);                              /* STA $62 */
    RTS();

locret_1878E:                                  /* $878E - the RTS inside sub_18788 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $879F  sub_1879F - increment the $49 counter unless it has wrapped
 *
 *  Original: src/bank6.asm:1696-1704
 *      INC $49 / BNE locret_187A5 / DEC $49
 *  locret_187A5:
 *      RTS
 * ------------------------------------------------------------------------ */
void sub_1879F(void)                           /* $879F */
{
    INC(byte_49);                              /* INC $49 */
    BNE(locret_187A5);                         /* BNE locret_187A5 */
    DEC(byte_49);                              /* DEC $49 */

locret_187A5:                                  /* $87A5 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $87A6  sub_187A6 - decrement the $49 counter unless it is zero
 *
 *  Original: src/bank6.asm:1711-1716
 *      LDA $49 / BEQ locret_187A5 / DEC $49 / RTS
 * ------------------------------------------------------------------------ */
void sub_187A6(void)                           /* $87A6 */
{
    LDA(byte_49);                              /* LDA $49 */
    /* BEQ locret_187A5 - the shared RTS of another procedure */
    BEQ_RET(locret_187A5);
    DEC(byte_49);                              /* DEC $49 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $87C7  game_logic_processing - the per-frame object slot loop and enemy
 *                                dispatcher
 *
 *  Original: src/bank6.asm:1751-1813 (only the dispatch itself is quoted):
 *      LDA $83 / PHA / JSR sub_183EF / JSR update_score_onscreen /
 *      JSR update_player_lives / LDX #0 / STX $6A / PLA / AND #$80 /
 *      BNE loc_187E2 / LDA $7F / AND #$20 / BEQ loc_187E4
 *  loc_187E2:
 *      INC $6B
 *  loc_187E4:
 *      LDA $6B / AND #2 / BEQ loc_187EE / LDA unk_187AD,X / TAX
 *  loc_187EE:
 *      STX $40 / LDA $528,X / BEQ loc_18807 / AND #$7F / TAY /
 *      LDA #$88 / PHA / LDA #6 / PHA /           ; push $8806 (locret_18806)
 *      LDA $8673,Y / PHA / LDA $8618,Y / PHA     ; push handler-1
 *  locret_18806:
 *      RTS                                       ; -> (hi<<8|lo)+1
 *  loc_18807:
 *      INC $6A / LDX $6A / CPX #$1A / BNE loc_187E4 / JSR sub_18835 /
 *      LDA $19B / BEQ loc_18822 / LDA $F5 / AND #$20 / BEQ loc_18822 /
 *      LDA $7A / JSR sub_19493
 *  loc_18822:
 *      JMP sub_184A7
 *
 *  The RTS at locret_18806 is a computed jump: the two tables at $8618/$8673
 *  hold handler-1 (an RTS adds one) and the pushed $8806 is the address the
 *  handler's own RTS returns to, i.e. loc_18807 ("next slot").  The port reads
 *  the same two tables and calls zanac_call_at(); returning from the handler is
 *  the next statement, which is loc_18807.
 * ------------------------------------------------------------------------ */
void game_logic_processing(void)               /* $87C7 */
{
    LDA(byte_83);                              /* LDA $83 */
    PHA();                                     /* PHA */
    sub_183EF();                               /* JSR sub_183EF */
    update_score_onscreen();                   /* JSR update_score_onscreen */
    update_player_lives();                     /* JSR update_player_lives */
    LDXI(0);                                   /* LDX #0 */
    STX(0x006A);                               /* STX $6A */
    PLA();                                     /* PLA */
    ANDI(0x80);                                /* AND #$80 */
    BNE(loc_187E2);                            /* BNE loc_187E2 */
    LDA(ppu_status_in_vblank);                 /* LDA $7F */
    ANDI(0x20);                                /* AND #$20 */
    BEQ(loc_187E4);                            /* BEQ loc_187E4 */

loc_187E2:                                     /* $87E2 */
    INC(0x006B);                               /* INC $6B */

loc_187E4:                                     /* $87E4 */
    LDA(0x006B);                               /* LDA $6B */
    ANDI(0x02);                                /* AND #2 */
    BEQ(loc_187EE);                            /* BEQ loc_187EE */
    LDA(ABSX(unk_187AD));                      /* LDA unk_187AD,X */
    TAX();                                     /* TAX */

loc_187EE:                                     /* $87EE */
    STX(byte_40);                              /* STX $40 */
    LDA(ABSX(byte_528));                       /* LDA $528,X */
    BEQ(loc_18807);                            /* BEQ loc_18807 */
    ANDI(0x7F);                                /* AND #$7F */
    TAY();                                     /* TAY */
    /* LDA #$88 / PHA / LDA #6 / PHA push $8806 = locret_18806: that is where
     * the handler's RTS resumes, i.e. the next statement below.  LDA $8673,Y /
     * PHA / LDA $8618,Y / PHA push the handler address and the RTS at
     * locret_18806 jumps to (hi<<8|lo)+1; zanac_call_at() is that computed
     * jump, and both tables still hold handler-1.
     *
     * The setjmp() below is the port's $8806: the few handlers that unwind the
     * 6502 stack with PLA + RTS (see zanac_ram.h) longjmp() back here, which
     * discards the same C frames their pops discarded on the 6502. */
    zanac_unwind_armed = 1;
    if (setjmp(zanac_unwind_env) != 0)
        zanac_unwind_armed = 0;                /* the jump landed here */
    else
        zanac_call_at((uint16_t)(((uint16_t)MEM8(tab_18674_hi - 1 + Y) << 8) |
                             (uint16_t)MEM8(tab_18619_lo - 1 + Y)) + 1);
    zanac_unwind_armed = 0;                    /* the handler returned normally */
    goto locret_18806;                         /* the handler's RTS resumes at $8806 */

locret_18806:                                  /* $8806 - the RTS that performs the dispatch */
    zanac_unwind_armed = 0;
loc_18807:                                     /* $8807 */
    INC(0x006A);                               /* INC $6A */
    LDX(0x006A);                               /* LDX $6A */
    CPXI(0x1A);                                /* CPX #$1A */
    BNE(loc_187E4);                            /* BNE loc_187E4 */
    sub_18835();                               /* JSR sub_18835 */
    LDA(byte_19B);                             /* LDA $19B */
    BEQ(loc_18822);                            /* BEQ loc_18822 */
    LDA(pad_test1);                            /* LDA $F5 */
    ANDI(0x20);                                /* AND #$20 */
    BEQ(loc_18822);                            /* BEQ loc_18822 */
    LDA(byte_7A);                              /* LDA $7A */
    sub_19493();                               /* JSR sub_19493 */

loc_18822:                                     /* $8822 */
    sub_184A7();                               /* JMP sub_184A7 */
    RTS();                                     /* RTS (tail call) */
}

/* ------------------------------------------------------------------------ *
 *  $8825  check_enemy_overflow - Y = first free slot, carry set if none
 *
 *  Original: src/bank6.asm:1820-1840
 *      LDY #5
 *  loc_18827:
 *      LDA $528,Y / BEQ loc_18833 / INY / CPY #26 / BNE loc_18827 /
 *      SEC / RTS
 *  loc_18833:
 *      CLC
 *  locret_18834:
 *      RTS
 *
 *  The first 5 slots are reserved for Zanac (ship + 3 shots + special shot).
 *  locret_18834 ($8834) is also the shared RTS sub_18835 branches to.
 * ------------------------------------------------------------------------ */
void check_enemy_overflow(void)                /* $8825 */
{
    LDYI(5);                                   /* LDY #5 */
    goto loc_18827;                            /* falls through into loc_18827 */

loc_18827:                                     /* $8827 */
    LDA(ABSY(byte_528));                       /* LDA $528,Y */
    BEQ(loc_18833);                            /* BEQ loc_18833 */
    INY();                                     /* INY */
    CPYI(26);                                  /* CPY #26 */
    BNE(loc_18827);                            /* BNE loc_18827 */
    SEC();                                     /* SEC */
    RTS();                                     /* RTS */

loc_18833:                                     /* $8833 */
    CLC();                                     /* CLC */
    goto locret_18834;                         /* falls through into $8834 */

locret_18834:                                  /* $8834 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8835  sub_18835 - add the sprites of the player's special shot
 *
 *  Original: src/bank6.asm:1846-2043.  Table reads are $89B1/$89B2/$89B4
 *  (unlabelled bytes inside tab_89A4), $8960 (tab_8960) and the current level
 *  $91.  The procedure branches to locret_18834, the shared RTS inside
 *  check_enemy_overflow, and ends by jumping to add_sprite.
 * ------------------------------------------------------------------------ */
void sub_18835(void)                           /* $8835 */
{
    sub_C033();                                /* JSR sub_C033 */
    LDA(byte_66);                              /* LDA $66 */
    /* BEQ locret_18834 - the shared RTS of another procedure */
    BEQ_RET(locret_18834);
    BMI(loc_18874);                            /* BMI loc_18874 */
    LDA(byte_2B);                              /* LDA $2B */
    /* BNE locret_18834 - the shared RTS of another procedure */
    BNE_RET(locret_18834);
    LDAI(0x00);                                /* LDA #0 */
    STA(0x0079);                               /* STA $79 */
    LDA(byte_66);                              /* LDA $66 */
    ORAI(0x80);                                /* ORA #$80 */
    STA(byte_66);                              /* STA $66 */
    CMPI(0x86);                                /* CMP #$86 */
    BCC(loc_18852);                            /* BCC loc_18852 */
    LDAI(6);                                   /* LDA #6 */

loc_18852:                                     /* $8852 */
    ASL_A();                                   /* ASL */
    ASL_A();                                   /* ASL */
    TAY();                                     /* TAY */
    LDAI(0x0F);                                /* LDA #$F */
    STA(byte_2F);                              /* STA $2F */
    LDAI(0x80);                                /* LDA #$80 */
    STA(byte_2E);                              /* STA $2E */
    LDA(ABSY(0x89B1));                         /* LDA $89B1,Y */
    STA(byte_2C);                              /* STA $2C */
    LDA(ABSY(0x89B2));                         /* LDA $89B2,Y */
    STA(byte_2D);                              /* STA $2D */
    LDAI(6);                                   /* LDA #6 */
    STA(byte_26);                              /* STA $26 */
    LDAI(2);                                   /* LDA #2 */
    STA(byte_2B);                              /* STA $2B */
    LDA(ABSY(0x89B4));                         /* LDA $89B4,Y */
    STA(0x0067);                               /* STA $67 */

loc_18874:                                     /* $8874 */
    LDA(0x0079);                               /* LDA $79 */
    BNE(loc_18889);                            /* BNE loc_18889 */
    LDA(byte_2B);                              /* LDA $2B */
    BNE(locret_18888);                         /* BNE locret_18888 */
    INC(0x0079);                               /* INC $79 */
    LDA(byte_66);                              /* LDA $66 */
    CMPI(0x86);                                /* CMP #$86 */
    BCC(loc_18889);                            /* BCC loc_18889 */
    SBCI(0x86);                                /* SBC #$86 */
    STA(byte_66);                              /* STA $66 */

locret_18888:                                  /* $8888 */
    RTS();                                     /* RTS */

loc_18889:                                     /* $8889 */
    LDA(byte_66);                              /* LDA $66 */
    CMPI(0x86);                                /* CMP #$86 */
    BCS(locret_18888);                         /* BCS locret_18888 */
    LDA(0x0067);                               /* LDA $67 */
    BEQ(loc_1889C);                            /* BEQ loc_1889C */
    DEC(0x0067);                               /* DEC $67 */
    BNE(loc_1889C);                            /* BNE loc_1889C */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_66);                              /* STA $66 */
    RTS();                                     /* RTS */

loc_1889C:                                     /* $889C */
    LDA(byte_66);                              /* LDA $66 */
    LDYI(0);                                   /* LDY #0 */
    CMPI(0x81);                                /* CMP #$81 */
    BEQ(loc_188B8);                            /* BEQ loc_188B8 */
    LDYI(0x11);                                /* LDY #$11 */
    CMPI(0x82);                                /* CMP #$82 */
    BEQ(loc_188B8);                            /* BEQ loc_188B8 */
    LDYI(0x22);                                /* LDY #$22 */
    CMPI(0x83);                                /* CMP #$83 */
    BEQ(loc_188B8);                            /* BEQ loc_188B8 */
    LDYI(0x33);                                /* LDY #$33 */
    CMPI(0x84);                                /* CMP #$84 */
    BEQ(loc_188B8);                            /* BEQ loc_188B8 */
    LDYI(0x44);                                /* LDY #$44 */

loc_188B8:                                     /* $88B8 */
    LDAI(0x60);                                /* LDA #$60 */
    STA(byte_18);                              /* STA $18 */
    LDAI(0x89);                                /* LDA #$89 */
    STA(byte_19);                              /* STA $19 */
    add_more_sprites();                        /* JSR add_more_sprites */
    LDA(byte_66);                              /* LDA $66 */
    CMPI(0x83);                                /* CMP #$83 */
    BNE(loc_188CA);                            /* BNE loc_188CA */
    RTS();                                     /* RTS */

loc_188CA:                                     /* $88CA */
    CMPI(0x85);                                /* CMP #$85 */
    BNE(loc_188CF);                            /* BNE loc_188CF */
    RTS();                                     /* RTS */

loc_188CF:                                     /* $88CF */
    LDYI(0);                                   /* LDY #0 */
    STY(byte_19);                              /* STY $19 */
    LDYI(0x84);                                /* LDY #$84 */
    STY(byte_18);                              /* STY $18 */
    CMPI(0x81);                                /* CMP #$81 */
    BNE(loc_1890C);                            /* BNE loc_1890C */
    LDAI(0x6B);                                /* LDA #$6B */
    STA(byte_84);                              /* STA $84 */
    LDA(current_level);                        /* LDA $91 */
    CMPI(0x0A);                                /* CMP #$A */
    PHP();                                     /* PHP */
    BCC(loc_188E8);                            /* BCC loc_188E8 */
    LDAI(1);                                   /* LDA #1 */

loc_188E8:                                     /* $88E8 */
    ASL_A();                                   /* ASL */
    STA(byte_85);                              /* STA $85 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_86);                              /* STA $86 */
    LDAI(0x88);                                /* LDA #$88 */
    STA(byte_87);                              /* STA $87 */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JSR add_sprite */
    PLP();                                     /* PLP */
    BCS(loc_188FC);                            /* BCS loc_188FC */
    RTS();                                     /* RTS */

loc_188FC:                                     /* $88FC */
    LDA(current_level);                        /* LDA $91 */
    SBCI(0x0A);                                /* SBC #$A */
    ASL_A();                                   /* ASL */
    STA(byte_85);                              /* STA $85 */
    LDAI(0x91);                                /* LDA #$91 */
    STA(byte_87);                              /* STA $87 */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JMP add_sprite */
    RTS();                                     /* RTS (tail call) */

loc_1890C:                                     /* $890C */
    CMPI(0x82);                                /* CMP #$82 */
    BNE(loc_1891D);                            /* BNE loc_1891D */
    LDAI(0x38);                                /* LDA #$38 */
    STA(byte_84);                              /* STA $84 */
    LDAI(0xC8);                                /* LDA #$C8 */
    STA(byte_87);                              /* STA $87 */
    LDA(byte_C5);                              /* LDA $C5 */
    sub_18E29();                               /* JMP sub_18E29 */
    RTS();                                     /* RTS (tail call) */

loc_1891D:                                     /* $891D */
    LDAI(0xA0);                                /* LDA #$A0 */
    STA(byte_84);                              /* STA $84 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_86);                              /* STA $86 */
    LDAI(0x70);                                /* LDA #$70 */
    STA(byte_87);                              /* STA $87 */
    LDA(byte_CB);                              /* LDA $CB */
    STA(byte_10);                              /* STA $10 */
    ASL_A();                                   /* ASL */
    ADC(byte_10);                              /* ADC $10 */
    ASL_A();                                   /* ASL */
    TAX();                                     /* TAX */
    LDAI(6);                                   /* LDA #6 */
    STA(byte_14);                              /* STA $14 */

loc_18936:                                     /* $8936 */
    sub_C048();                                /* JSR sub_C048 */
    BNE(loc_18941);                            /* BNE loc_18941 */
    INX();                                     /* INX */
    DEC(byte_14);                              /* DEC $14 */
    BNE(loc_18936);                            /* BNE loc_18936 */
    RTS();                                     /* RTS */

loc_18941:                                     /* $8941 */
    sub_C048();                                /* JSR sub_C048 */
    INX();                                     /* INX */
    ASL_A();                                   /* ASL */
    STA(byte_85);                              /* STA $85 */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JSR add_sprite */
    LDA(byte_87);                              /* LDA $87 */
    CLC();                                     /* CLC */
    ADCI(9);                                   /* ADC #9 */
    STA(byte_87);                              /* STA $87 */
    DEC(byte_14);                              /* DEC $14 */
    BNE(loc_18941);                            /* BNE loc_18941 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_85);                              /* STA $85 */
    TAY();                                     /* TAY */
    add_sprite();                              /* JMP add_sprite */
    RTS();                                     /* RTS (tail call) */
}

/* ------------------------------------------------------------------------ *
 *  $8C68  enemy_type_1_zanac - initialise the player object, then run its
 *                              main handler
 *
 *  Original: src/bank6.asm:2747-2773
 *      LDA $528 / BMI zanac_main_handler / ORA #$80 / STA $528 / LDA #$A0 /
 *      STA $542 / LDA #$80 / STA $55C / LDA #0 / STA $590 / LDA #5 /
 *      STA $67A / LDA #5 / STA $74A / LDA #$40 / STA $764 / LDA #$80 /
 *      STA $5AA / LDA $5C / CMP #2 / BCC zanac_main_handler / CMP #4 /
 *      BCS zanac_main_handler / LDA #3 / STA $52C
 *
 *  The three branches to zanac_main_handler target the next procedure, which
 *  is also the fall-through target; the port jumps to the local label
 *  loc_18CA4 that emits the call (PORTING.md 5).
 * ------------------------------------------------------------------------ */
void enemy_type_1_zanac(void)                  /* $8C68 */
{
    LDA(byte_528);                             /* LDA $528 */
    BMI(loc_18CA4);                            /* BMI zanac_main_handler ($8CA4) */
    ORAI(0x80);                                /* ORA #$80 */
    STA(byte_528);                             /* STA $528 */
    LDAI(0xA0);                                /* LDA #$A0 */
    STA(byte_542);                             /* STA $542 */
    LDAI(0x80);                                /* LDA #$80 */
    STA(byte_55C);                             /* STA $55C */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_590);                             /* STA $590 */
    LDAI(5);                                   /* LDA #5 */
    STA(unk_67A);                              /* STA $67A */
    LDAI(5);                                   /* LDA #5 */
    STA(0x074A);                               /* STA $74A */
    LDAI(0x40);                                /* LDA #$40 */
    STA(byte_764);                             /* STA $764 */
    LDAI(0x80);                                /* LDA #$80 */
    STA(byte_5AA);                             /* STA $5AA */
    LDA(special_weapon_type);                  /* LDA $5C */
    CMPI(2);                                   /* CMP #2 */
    BCC(loc_18CA4);                            /* BCC zanac_main_handler */
    CMPI(4);                                   /* CMP #4 */
    BCS(loc_18CA4);                            /* BCS zanac_main_handler */
    LDAI(3);                                   /* LDA #3 */
    STA(byte_52C);                             /* STA $52C */

loc_18CA4:                                     /* $8CA4 - entry of zanac_main_handler */
    zanac_main_handler();                      /* falls through into $8CA4 */
    RTS();                                     /* RTS (tail call) */
}

/* ------------------------------------------------------------------------ *
 *  $8CA4  zanac_main_handler - the player ship: movement, firing and the
 *                              special weapon
 *
 *  Original: src/bank6.asm:2779-3037 (the tail falls through into sub_18E29).
 *  Called directly from the dispatcher (type 1) and by JMP from enemy_type_60
 *  and enemy_type_1_zanac.
 * ------------------------------------------------------------------------ */
void zanac_main_handler(void)                  /* $8CA4 */
{
    LDA(warp_sequence_counter);                /* LDA warp_sequence_counter */
    BEQ(loc_18CAB);                            /* BEQ loc_18CAB */
    warp_sequence();                           /* JMP warp_sequence */
    RTS();                                     /* RTS (tail call) */

loc_18CAB:                                     /* $8CAB */
    j_handle_pad_buttons();                    /* JSR $C02D */
    LDY(pmove_direction);                      /* LDY $34 */
    LDA(ABSY(unk_18E74));                      /* LDA $8E74,Y */
    BNE(loc_18CC8);                            /* BNE loc_18CC8 */
    LDA(unk_67A);                              /* LDA $67A */
    CMPI(5);                                   /* CMP #5 */
    BEQ(loc_18CDD);                            /* BEQ loc_18CDD */
    BCS(loc_18CC3);                            /* BCS loc_18CC3 */
    INC(unk_67A);                              /* INC $67A */
    BPL(loc_18CDD);                            /* BPL loc_18CDD */

loc_18CC3:                                     /* $8CC3 */
    DEC(unk_67A);                              /* DEC $67A */
    BPL(loc_18CDD);                            /* BPL loc_18CDD */

loc_18CC8:                                     /* $8CC8 */
    CLC();                                     /* CLC */
    ADC(unk_67A);                              /* ADC $67A */
    STA(unk_67A);                              /* STA $67A */
    BPL(loc_18CD6);                            /* BPL loc_18CD6 */
    INC(unk_67A);                              /* INC $67A */
    BPL(loc_18CDD);                            /* BPL loc_18CDD */

loc_18CD6:                                     /* $8CD6 */
    CMPI(0x0B);                                /* CMP #$B */
    BNE(loc_18CDD);                            /* BNE loc_18CDD */
    DEC(unk_67A);                              /* DEC $67A */

loc_18CDD:                                     /* $8CDD */
    LDY(unk_67A);                              /* LDY $67A */
    LDA(ABSY(unk_18E86));                      /* LDA $8E86,Y */
    STA(byte_576);                             /* STA $576 */
    LDY(pmove_direction);                      /* LDY $34 */
    CPYI(4);                                   /* CPY #4 */
    BEQ(loc_18D3E);                            /* BEQ loc_18D3E */
    LDA(ABSY(unk_18E7D));                      /* LDA $8E7D,Y */
    set_speed_by_slope();                      /* JSR set_speed_by_slope */
    LDA(byte_5F8);                             /* LDA $5F8 */
    CLC();                                     /* CLC */
    ADC(byte_5C4);                             /* ADC $5C4 */
    TAY();                                     /* TAY */
    LDA(byte_612);                             /* LDA $612 */
    ADC(byte_542);                             /* ADC $542 */
    CMPI(0x18);                                /* CMP #$18 */
    BCS(loc_18D0A);                            /* BCS loc_18D0A */
    LDAI(0x18);                                /* LDA #$18 */
    LDYI(0);                                   /* LDY #0 */
    BEQ(loc_18D12);                            /* BEQ loc_18D12 (always taken) */

loc_18D0A:                                     /* $8D0A */
    CMPI(0xD8);                                /* CMP #$D8 */
    BCC(loc_18D12);                            /* BCC loc_18D12 */
    LDAI(0xD8);                                /* LDA #$D8 */
    LDYI(0);                                   /* LDY #0 */

loc_18D12:                                     /* $8D12 */
    STA(byte_542);                             /* STA $542 */
    STY(byte_5C4);                             /* STY $5C4 */
    LDA(byte_62C);                             /* LDA $62C */
    CLC();                                     /* CLC */
    ADC(byte_5DE);                             /* ADC $5DE */
    TAY();                                     /* TAY */
    LDA(byte_646);                             /* LDA $646 */
    ADC(byte_55C);                             /* ADC $55C */
    CMPI(0x10);                                /* CMP #$10 */
    BCS(loc_18D30);                            /* BCS loc_18D30 */
    LDAI(0x10);                                /* LDA #$10 */
    LDYI(0);                                   /* LDY #0 */
    BEQ(loc_18D38);                            /* BEQ loc_18D38 (always taken) */

loc_18D30:                                     /* $8D30 */
    CMPI(0xF0);                                /* CMP #$F0 */
    BCC(loc_18D38);                            /* BCC loc_18D38 */
    LDAI(0xF0);                                /* LDA #$F0 */
    LDYI(0);                                   /* LDY #0 */

loc_18D38:                                     /* $8D38 */
    STA(byte_55C);                             /* STA $55C */
    STY(byte_5DE);                             /* STY $5DE */

loc_18D3E:                                     /* $8D3E */
    INC(byte_54);                              /* INC $54 */
    BNE(loc_18D44);                            /* BNE loc_18D44 */
    DEC(byte_54);                              /* DEC $54 */

loc_18D44:                                     /* $8D44 */
    LDA(pad_buttons_1);                        /* LDA $F7 */
    ORA(pad_buttons_2);                        /* ORA $F8 */
    ANDI(0x40);                                /* AND #$40 */
    BNE(loc_18D52);                            /* BNE loc_18D52 */
    LDAI(1);                                   /* LDA #1 */
    STA(0x0038);                               /* STA $38 */
    BNE(loc_18DAE);                            /* BNE loc_18DAE (always taken) */

loc_18D52:                                     /* $8D52 */
    DEC(0x0038);                               /* DEC $38 */
    BNE(loc_18DAE);                            /* BNE loc_18DAE */
    LDA(byte_51);                              /* LDA $51 */
    STA(0x0038);                               /* STA $38 */
    LDA(byte_54);                              /* LDA $54 */
    CMPI(0x12);                                /* CMP #$12 */
    BCC(loc_18D64);                            /* BCC loc_18D64 */
    LDAI(1);                                   /* LDA #1 */
    BNE(loc_18D6A);                            /* BNE loc_18D6A (always taken) */

loc_18D64:                                     /* $8D64 */
    SBCI(0x01);                                /* SBC #1 */
    TAY();                                     /* TAY */
    LDA(ABSY(unk_18E91));                      /* LDA $8E91,Y */

loc_18D6A:                                     /* $8D6A */
    PHA();                                     /* PHA */
    CLC();                                     /* CLC */
    ADC(byte_48);                              /* ADC $48 */
    STA(byte_48);                              /* STA $48 */
    BCC(loc_18D75);                            /* BCC loc_18D75 */
    sub_18788();                               /* JSR sub_18788 */

loc_18D75:                                     /* $8D75 */
    PLA();                                     /* PLA */
    CLC();                                     /* CLC */
    ADC(byte_4A);                              /* ADC $4A */
    STA(byte_4A);                              /* STA $4A */
    BCC(loc_18D80);                            /* BCC loc_18D80 */
    sub_1879F();                               /* JSR sub_1879F */

loc_18D80:                                     /* $8D80 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_54);                              /* STA $54 */
    INC(byte_56);                              /* INC $56 */
    BNE(loc_18D8A);                            /* BNE loc_18D8A */
    DEC(byte_56);                              /* DEC $56 */

loc_18D8A:                                     /* $8D8A */
    LDYI(1);                                   /* LDY #1 */
    LDX(0x0035);                               /* LDX $35 */

loc_18D8E:                                     /* $8D8E */
    LDA(ABSY(byte_528));                       /* LDA $528,Y */
    BEQ(loc_18D99);                            /* BEQ loc_18D99 */
    INY();                                     /* INY */
    DEX();                                     /* DEX */
    BNE(loc_18D8E);                            /* BNE loc_18D8E */
    BEQ(loc_18DAC);                            /* BEQ loc_18DAC (always taken) */

loc_18D99:                                     /* $8D99 */
    LDAI(2);                                   /* LDA #2 */
    STA(ABSY(byte_528));                       /* STA $528,Y */
    LDA(byte_542);                             /* LDA $542 */
    STA(ABSY(byte_542));                       /* STA $542,Y */
    LDA(byte_55C);                             /* LDA $55C */
    STA(ABSY(byte_55C));                       /* STA $55C,Y */
    INC(byte_55);                              /* INC $55 */

loc_18DAC:                                     /* $8DAC */
    LDX(byte_40);                              /* LDX $40 */

loc_18DAE:                                     /* $8DAE */
    LDA(pad_buttons_1);                        /* LDA pad_buttons_1 */
    ORA(pad_buttons_2);                        /* ORA pad_buttons_2 */
    ANDI(0x80);                                /* AND #$80 */
    BEQ(loc_18DD1);                            /* BEQ loc_18DD1 */
    LDA(byte_52C);                             /* LDA $52C */
    BNE(loc_18DD1);                            /* BNE loc_18DD1 */
    LDA(pmove_direction);                      /* LDA pmove_direction */
    STA(byte_782);                             /* STA $782 */
    LDAI(3);                                   /* LDA #3 */
    STA(byte_52C);                             /* STA $52C */
    LDA(byte_542);                             /* LDA $542 */
    STA(byte_546);                             /* STA $546 */
    LDA(byte_55C);                             /* LDA $55C */
    STA(byte_560);                             /* STA $560 */

loc_18DD1:                                     /* $8DD1 */
    LDA(byte_5AA);                             /* LDA $5AA */
    BPL(loc_18DEB);                            /* BPL loc_18DEB */
    LDA(byte_764);                             /* LDA $764 */
    ANDI(0x03);                                /* AND #3 */
    STA(byte_590);                             /* STA $590 */
    DEC(byte_764);                             /* DEC $764 */
    BNE(loc_18DEB);                            /* BNE loc_18DEB */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_590);                             /* STA $590 */
    STA(byte_5AA);                             /* STA $5AA */

loc_18DEB:                                     /* $8DEB */
    sub_181A7();                               /* JSR sub_181A7 */
    add_enemy_meta_sprite();                   /* JSR add_enemy_meta_sprite */
    LDA(special_weapon_type);                  /* LDA special_weapon_type */
    BEQ(locret_18DF9);                         /* BEQ locret_18DF9 */
    CMPI(5);                                   /* CMP #5 */
    BNE(loc_18DFA);                            /* BNE loc_18DFA */

locret_18DF9:                                  /* $8DF9 */
    RTS();                                     /* RTS */

loc_18DFA:                                     /* $8DFA */
    LDY(byte_52C);                             /* LDY $52C */
    BMI(loc_18E17);                            /* BMI loc_18E17 */
    CPYI(0x13);                                /* CPY #$13 */
    BEQ(loc_18E17);                            /* BEQ loc_18E17 */
    CMPI(4);                                   /* CMP #4 */
    BNE(loc_18E08);                            /* BNE loc_18E08 */
    RTS();                                     /* RTS */

loc_18E08:                                     /* $8E08 */
    CMPI(1);                                   /* CMP #1 */
    BEQ(loc_18E10);                            /* BEQ loc_18E10 */
    CMPI(6);                                   /* CMP #6 */
    BNE(loc_18E17);                            /* BNE loc_18E17 */

loc_18E10:                                     /* $8E10 */
    LDA(byte_5E);                              /* LDA byte_5E */
    BNE(loc_18E17);                            /* BNE loc_18E17 */
    sub_1948B();                               /* JMP sub_1948B */
    RTS();                                     /* RTS (tail call) */

loc_18E17:                                     /* $8E17 */
    LDAI(0x84);                                /* LDA #$84 */
    STA(byte_18);                              /* STA byte_18 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_19);                              /* STA byte_19 */
    LDAI(0xBC);                                /* LDA #$BC */
    STA(byte_84);                              /* STA byte_84 */
    LDAI(0xD6);                                /* LDA #$D6 */
    STA(byte_87);                              /* STA byte_87 */
    LDA(byte_5E);                              /* LDA byte_5E */
    sub_18E29();                               /* falls through into $8E29 */
    RTS();                                     /* RTS (tail call) */
}

/* ------------------------------------------------------------------------ *
 *  $8E29  sub_18E29 - print A as a three-digit sprite number
 *
 *  Original: src/bank6.asm:3043-3093
 *      LDY #0 / STY byte_86 / STA $15 / SEC / SBC #100 / BCC loc_18E47 /
 *      LDY #2 / CMP #100 / BCC loc_18E3E / SBC #100 / LDY #4
 *  loc_18E3E:
 *      STY byte_85 / STA $15 / LDY #0 / JSR add_sprite
 *  loc_18E47:
 *      LDA byte_87 / CLC / ADC #9 / STA byte_87 / LDY #0 / LDA byte_15
 *  loc_18E52:
 *      CMP #$A / BCC loc_18E5C / SBC #$A / INY / INY / BCS loc_18E52
 *  loc_18E5C:
 *      STY byte_85 / PHA / LDY #0 / JSR add_sprite / PLA / ASL /
 *      STA byte_85 / LDA byte_87 / CLC / ADC #9 / STA byte_87 / LDY #0 /
 *      JMP add_sprite
 * ------------------------------------------------------------------------ */
void sub_18E29(void)                           /* $8E29 */
{
    LDYI(0);                                   /* LDY #0 */
    STY(byte_86);                              /* STY byte_86 */
    STA(byte_15);                              /* STA $15 */
    SEC();                                     /* SEC */
    SBCI(100);                                 /* SBC #100 */
    BCC(loc_18E47);                            /* BCC loc_18E47 */
    LDYI(2);                                   /* LDY #2 */
    CMPI(100);                                 /* CMP #100 */
    BCC(loc_18E3E);                            /* BCC loc_18E3E */
    SBCI(100);                                 /* SBC #100 */
    LDYI(4);                                   /* LDY #4 */

loc_18E3E:                                     /* $8E3E */
    STY(byte_85);                              /* STY byte_85 */
    STA(byte_15);                              /* STA byte_15 */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JSR add_sprite */

loc_18E47:                                     /* $8E47 */
    LDA(byte_87);                              /* LDA byte_87 */
    CLC();                                     /* CLC */
    ADCI(9);                                   /* ADC #9 */
    STA(byte_87);                              /* STA byte_87 */
    LDYI(0);                                   /* LDY #0 */
    LDA(byte_15);                              /* LDA byte_15 */

loc_18E52:                                     /* $8E52 */
    CMPI(0x0A);                                /* CMP #$A */
    BCC(loc_18E5C);                            /* BCC loc_18E5C */
    SBCI(0x0A);                                /* SBC #$A */
    INY();                                     /* INY */
    INY();                                     /* INY */
    BCS(loc_18E52);                            /* BCS loc_18E52 */

loc_18E5C:                                     /* $8E5C */
    STY(byte_85);                              /* STY byte_85 */
    PHA();                                     /* PHA */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JSR add_sprite */
    PLA();                                     /* PLA */
    ASL_A();                                   /* ASL */
    STA(byte_85);                              /* STA byte_85 */
    LDA(byte_87);                              /* LDA byte_87 */
    CLC();                                     /* CLC */
    ADCI(9);                                   /* ADC #9 */
    STA(byte_87);                              /* STA byte_87 */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JMP add_sprite */
    RTS();                                     /* RTS (tail call) */
}

/* ------------------------------------------------------------------------ *
 *  $8EA1  set_default_weapon - load the three weapon parameters of level $33
 *
 *  Original: src/bank6.asm:3146-3159
 *      LDA $33 / ASL / CLC / ADC $33 / TAY / LDA byte_18EB8,Y / STA $37 /
 *      LDA byte_18EB8+1,Y / STA $35 / LDA byte_18EB8+2,Y / STA $36 / RTS
 * ------------------------------------------------------------------------ */
void set_default_weapon(void)                  /* $8EA1 */
{
    LDA(zanac_main_weapon_level);              /* LDA $33 */
    ASL_A();                                   /* ASL */
    CLC();                                     /* CLC */
    ADC(zanac_main_weapon_level);              /* ADC $33 (A = 3 * $33) */
    TAY();                                     /* TAY */
    LDA(ABSY(byte_18EB8));                     /* LDA byte_18EB8,Y */
    STA(zanac_main_weapon_speed_y);            /* STA $37 */
    LDA(ABSY(byte_18EB8 + 1));                 /* LDA byte_18EB8+1,Y */
    STA(0x0035);                               /* STA $35 */
    LDA(ABSY(byte_18EB8 + 2));                 /* LDA byte_18EB8+2,Y */
    STA(unk_36);                               /* STA $36 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8ECA  warp_sequence - the warp animation, run instead of the ship handler
 *
 *  Original: src/bank6.asm:3174-3269
 *      BMI loc_18EDA / ORA #$80 / STA warp_sequence_counter / LDA #0 /
 *      STA $590 / LDA #$32 / STA $576
 *  loc_18EDA:
 *      LDA warp_sequence_counter / CMP #$82 / BEQ loc_18F16 / LDA #$E1 /
 *      CLC / ADC $542 / ROR / CLC / ADC $542 / ROR / CMP $542 /
 *      BEQ loc_18EF7 / STA $542 / JMP loc_18F10
 *  loc_18EF7:
 *      INC warp_sequence_counter / LDA #0 / STA $5F8 / STA $612 / STA $6E2 /
 *      LDA #9 / STA $660 / LDA #8 / STA $716 / BNE loc_18F16
 *  loc_18F10:
 *      JSR sub_181A7 / JMP add_enemy_meta_sprite
 *  loc_18F16:
 *      JSR sub_181A7 / JSR process_enemy_flags_sprites / LDA $5F8 / STA $16 /
 *      LDA $612 / STA $17 / ASL $16 / ROL $17 / ASL $16 / ROL $17 /
 *      LDA #$18 / STA $14 / LDA #0 / STA $15 / STA $86 / LDA $542 / STA $84 /
 *      LDA #$84 / STA $18 / LDA #0 / STA $19
 *  loc_18F45:
 *      LDA $15 / CLC / ADC $16 / STA $15 / LDA $84 / ADC $17 / STA $84 /
 *      BCC locret_18F77 / LDA #$14 / STA $85 / LDA $55C / SEC / SBC #8 /
 *      STA $87 / LDY #0 / JSR add_sprite / LDA #$16 / STA $85 / LDA $55C /
 *      STA $87 / LDY #0 / JSR add_sprite / DEC $14 / BNE loc_18F45
 *  locret_18F77:
 *      RTS
 * ------------------------------------------------------------------------ */
void warp_sequence(void)                       /* $8ECA */
{
    BMI(loc_18EDA);                            /* BMI loc_18EDA */
    ORAI(0x80);                                /* ORA #$80 */
    STA(warp_sequence_counter);                /* STA warp_sequence_counter */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_590);                             /* STA $590 */
    LDAI(0x32);                                /* LDA #$32 */
    STA(byte_576);                             /* STA $576 */

loc_18EDA:                                     /* $8EDA */
    LDA(warp_sequence_counter);                /* LDA warp_sequence_counter */
    CMPI(0x82);                                /* CMP #$82 */
    BEQ(loc_18F16);                            /* BEQ loc_18F16 */
    LDAI(0xE1);                                /* LDA #$E1 */
    CLC();                                     /* CLC */
    ADC(byte_542);                             /* ADC $542 */
    ROR_A();                                   /* ROR */
    CLC();                                     /* CLC */
    ADC(byte_542);                             /* ADC $542 */
    ROR_A();                                   /* ROR */
    CMP(byte_542);                             /* CMP $542 */
    BEQ(loc_18EF7);                            /* BEQ loc_18EF7 */
    STA(byte_542);                             /* STA $542 */
    goto loc_18F10;                            /* JMP loc_18F10 */

loc_18EF7:                                     /* $8EF7 */
    INC(warp_sequence_counter);                /* INC warp_sequence_counter */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_5F8);                             /* STA $5F8 */
    STA(byte_612);                             /* STA $612 */
    STA(byte_6E2);                             /* STA $6E2 */
    LDAI(9);                                   /* LDA #9 */
    STA(byte_660);                             /* STA $660 */
    LDAI(8);                                   /* LDA #8 */
    STA(byte_716);                             /* STA $716 */
    BNE(loc_18F16);                            /* BNE loc_18F16 (always taken) */

loc_18F10:                                     /* $8F10 */
    sub_181A7();                               /* JSR sub_181A7 */
    add_enemy_meta_sprite();                   /* JMP add_enemy_meta_sprite */
    RTS();                                     /* RTS (tail call) */

loc_18F16:                                     /* $8F16 */
    sub_181A7();                               /* JSR sub_181A7 */
    process_enemy_flags_sprites();             /* JSR process_enemy_flags_sprites */
    LDA(byte_5F8);                             /* LDA $5F8 */
    STA(byte_16);                              /* STA $16 */
    LDA(byte_612);                             /* LDA $612 */
    STA(byte_17);                              /* STA $17 */
    ASL(byte_16);                              /* ASL $16 */
    ROL(byte_17);                              /* ROL $17 */
    ASL(byte_16);                              /* ASL $16 */
    ROL(byte_17);                              /* ROL $17 */
    LDAI(0x18);                                /* LDA #$18 */
    STA(byte_14);                              /* STA $14 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_15);                              /* STA $15 */
    STA(byte_86);                              /* STA $86 */
    LDA(byte_542);                             /* LDA $542 */
    STA(byte_84);                              /* STA $84 */
    LDAI(0x84);                                /* LDA #$84 */
    STA(byte_18);                              /* STA $18 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_19);                              /* STA $19 */

loc_18F45:                                     /* $8F45 */
    LDA(byte_15);                              /* LDA $15 */
    CLC();                                     /* CLC */
    ADC(byte_16);                              /* ADC $16 */
    STA(byte_15);                              /* STA $15 */
    LDA(byte_84);                              /* LDA $84 */
    ADC(byte_17);                              /* ADC $17 */
    STA(byte_84);                              /* STA $84 */
    /* BCC locret_18F77 - the shared RTS of another procedure */
    BCC_RET(locret_18F77);
    LDAI(0x14);                                /* LDA #$14 */
    STA(byte_85);                              /* STA $85 */
    LDA(byte_55C);                             /* LDA $55C */
    SEC();                                     /* SEC */
    SBCI(8);                                   /* SBC #8 */
    STA(byte_87);                              /* STA $87 */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JSR add_sprite */
    LDAI(0x16);                                /* LDA #$16 */
    STA(byte_85);                              /* STA $85 */
    LDA(byte_55C);                             /* LDA $55C */
    STA(byte_87);                              /* STA $87 */
    LDYI(0);                                   /* LDY #0 */
    add_sprite();                              /* JSR add_sprite */
    DEC(byte_14);                              /* DEC $14 */
    BNE(loc_18F45);                            /* BNE loc_18F45 */
    goto locret_18F77;                         /* falls through into $8F77 */

locret_18F77:                                  /* $8F77 */
    RTS();                                     /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $8F78  enemy_type_60 - Zanac's explosion (spawns type 35 fragments)
 *
 *  Original: src/bank6.asm:3275-3367
 *      LDA $528 / BMI loc_18FF6 / ORA #$80 / STA $528 / LDA $5AA /
 *      BPL loc_18F8F / LDA #$81 / STA $528 / JMP zanac_main_handler
 *  loc_18F8F:
 *      DEC $32 / BNE loc_18F96 / JSR j_apu_stop
 *  loc_18F96:
 *      JSR sub_1948B / LSR $4B / LSR $47 / JSR sub_C07B / LDA #16 /
 *      JSR j_apu_play / LDA #$38 / LDY #$18 / JSR sub_1A2CB / LDA #$10 /
 *      STA $16 / JSR enemy_prng / STA $17
 *  loc_18FB5:
 *      JSR check_enemy_overflow / BCS loc_18FF6 / LDA #$A3 /
 *      JSR spawn_projectile / TXA / PHA / TYA / TAX / LDA #3 / STA $660,X /
 *      LDA #2 / STA $74A,X / LDA #0 / STA $7E6,X / LDA $17 /
 *      JSR set_speed_by_slope / JSR enemy_prng / PHA / AND #3 / STA $590,X /
 *      PLA / AND #$1F / ADC #$E / LDY #7 / JSR sub_1A2CB / PLA / TAX /
 *      INC $17 / INC $17 / INC $17 / DEC $16 / BNE loc_18FB5
 *  loc_18FF6:
 *      JSR sub_183BC / BCS loc_1900F / CPY #12 / BCC loc_19000 / RTS
 *  loc_19000:
 *      LDA unk_19021,Y / STA $576 / LDA unk_1902D,Y / STA $590 /
 *      JMP add_enemy_meta_sprite
 *  loc_1900F:
 *      LDA $30 / ORA #1 / STA $30 / LDA #0 / STA $528 / STA $49 / STA $33 /
 *      JMP set_default_weapon
 * ------------------------------------------------------------------------ */
void enemy_type_60(void)                       /* $8F78 */
{
    LDA(byte_528);                             /* LDA $528 */
    BMI(loc_18FF6);                            /* BMI loc_18FF6 */
    ORAI(0x80);                                /* ORA #$80 */
    STA(byte_528);                             /* STA $528 */
    LDA(byte_5AA);                             /* LDA $5AA */
    BPL(loc_18F8F);                            /* BPL loc_18F8F */
    LDAI(0x81);                                /* LDA #$81 */
    STA(byte_528);                             /* STA $528 */
    zanac_main_handler();                      /* JMP zanac_main_handler */
    RTS();                                     /* RTS (tail call) */

loc_18F8F:                                     /* $8F8F */
    DEC(byte_32);                              /* DEC $32 */
    BNE(loc_18F96);                            /* BNE loc_18F96 */
    j_apu_stop();                              /* JSR j_apu_stop */

loc_18F96:                                     /* $8F96 */
    sub_1948B();                               /* JSR sub_1948B */
    LSR(byte_4B);                              /* LSR $4B */
    LSR(byte_47);                              /* LSR $47 */
    sub_C07B();                                /* JSR sub_C07B */
    LDAI(16);                                  /* LDA #16 (Zanac explosion sound) */
    j_apu_play();                              /* JSR j_apu_play */
    LDAI(0x38);                                /* LDA #$38 */
    LDYI(0x18);                                /* LDY #$18 */
    sub_1A2CB();                               /* JSR sub_1A2CB */
    LDAI(0x10);                                /* LDA #$10 */
    STA(byte_16);                              /* STA $16 */
    enemy_prng();                              /* JSR enemy_prng */
    STA(byte_17);                              /* STA $17 */

loc_18FB5:                                     /* $8FB5 */
    check_enemy_overflow();                    /* JSR check_enemy_overflow */
    BCS(loc_18FF6);                            /* BCS loc_18FF6 */
    LDAI(0xA3);                                /* LDA #$A3 */
    spawn_projectile();                        /* JSR spawn_projectile */
    TXA();                                     /* TXA */
    PHA();                                     /* PHA */
    TYA();                                     /* TYA */
    TAX();                                     /* TAX */
    LDAI(3);                                   /* LDA #3 */
    STA(ABSX(byte_660));                       /* STA $660,X */
    LDAI(2);                                   /* LDA #2 */
    STA(ABSX(0x074A));                         /* STA $74A,X */
    LDAI(0x00);                                /* LDA #0 */
    STA(ABSX(0x07E6));                         /* STA $7E6,X */
    LDA(byte_17);                              /* LDA $17 */
    set_speed_by_slope();                      /* JSR set_speed_by_slope */
    enemy_prng();                              /* JSR enemy_prng */
    PHA();                                     /* PHA */
    ANDI(0x03);                                /* AND #3 */
    STA(ABSX(byte_590));                       /* STA $590,X */
    PLA();                                     /* PLA */
    ANDI(0x1F);                                /* AND #$1F */
    ADCI(0x0E);                                /* ADC #$E */
    LDYI(7);                                   /* LDY #7 */
    sub_1A2CB();                               /* JSR sub_1A2CB */
    PLA();                                     /* PLA */
    TAX();                                     /* TAX */
    INC(byte_17);                              /* INC $17 */
    INC(byte_17);                              /* INC $17 */
    INC(byte_17);                              /* INC $17 */
    DEC(byte_16);                              /* DEC $16 */
    BNE(loc_18FB5);                            /* BNE loc_18FB5 */

loc_18FF6:                                     /* $8FF6 */
    sub_183BC();                               /* JSR sub_183BC */
    BCS(loc_1900F);                            /* BCS loc_1900F */
    CPYI(12);                                  /* CPY #12 */
    BCC(loc_19000);                            /* BCC loc_19000 */
    RTS();                                     /* RTS */

loc_19000:                                     /* $9000 */
    LDA(ABSY(unk_19021));                      /* LDA unk_19021,Y */
    STA(byte_576);                             /* STA $576 (sprite ID) */
    LDA(ABSY(unk_1902D));                      /* LDA unk_1902D,Y */
    STA(byte_590);                             /* STA $590 */
    add_enemy_meta_sprite();                   /* JMP add_enemy_meta_sprite */
    RTS();                                     /* RTS (tail call) */

loc_1900F:                                     /* $900F */
    LDA(game_mode);                            /* LDA $30 */
    ORAI(0x01);                                /* ORA #1 */
    STA(game_mode);                            /* STA $30 */
    LDAI(0x00);                                /* LDA #0 */
    STA(byte_528);                             /* STA $528 */
    STA(byte_49);                              /* STA $49 */
    STA(zanac_main_weapon_level);              /* STA $33 */
    set_default_weapon();                      /* JMP set_default_weapon */
    RTS();                                     /* RTS (tail call) */
}
