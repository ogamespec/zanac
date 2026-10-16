/* bank7/EB58.c - bank 7, $EB58-$FEE8: the APU/music engine.
 *
 * Ported from src/bank7.asm lines 7861-9508 (addresses from Build/bank7.lst).
 *
 * This chunk is the sound driver that `apu_update` (bank7/E229.c, the entry
 * point called from the VBlank handler) drives:
 *
 *   - sub_EB7A ($EB7A) interprets one channel's sound script, read through
 *     $1C/$1D, and dispatches the byte values $80-$AF through word_EBAF
 *     ($EBAF-$EBEA, 30 words that hold handler-1) with an RTS to a computed
 *     address;
 *   - the apu_fx_* handlers and sub_EC2F/sub_EC6B/sub_ED08/sub_ED0C/sub_ED10/
 *     sub_ED25/sub_ED2F/sub_ED33 write the channel state $31A-$3EC, which
 *     apu_update finally flushes to the APU registers $4000-$400F;
 *   - sub_EFA6 loads a song from apu_sound_tab ($F0FE, 36 words) and
 *     apu_play/apu_stop/apu_reset are its callers.
 *
 * The tables of this range (word_EBAF $EBAF, unk_EF42/unk_EF46 $EF42/$EF46,
 * apu_length_tab $F01C, apu_sound_tab $F0FE, the duration table at $F16A,
 * word_F190 $F190 and the song data $F180-$FE71) are not defined here: they
 * still live in the extracted bank image and every read goes through
 * MEM8()/ABSY().
 *
 * Review notes (PORTING.md sections 4c/5):
 *
 *  - computed control flow:
 *      sub_EB7A's `PHA ... PHA / RTS` through word_EBAF becomes
 *      zanac_rts_dispatch(), which pops the pushed word and calls
 *      zanac_call_at(word_EBAF[X] + 1) (the table holds handler-1);
 *      sub_EFA6's `LDA byte_1F / PHA / LDA byte_1E / PHA / RTS` becomes
 *      zanac_call_at(byte_1E | byte_1F << 8);
 *  - apu_fx_5 ($EC10) does `PLA / PLA / PLA / PLA / JMP loc_EB17`; the two
 *    frames it discards cannot be reproduced (see its comment), it is the one
 *    documented deviation of this file;
 *  - a jump into another procedure's inner label is reproduced where it is
 *    used: loc_ED56 ($ED56, the note entry inside apu_fx_26_27) is duplicated
 *    in sub_EB7A, loc_EE3C ($EE3C, the silent-channel tail of sub_EDFC) in
 *    sub_EE43 and loc_F0F6 ($F0F6, the tail of sub_F0F4) in sub_F0FA;
 *  - a branch into another procedure's *entry* becomes a local label of the
 *    same name that calls the procedure and returns: apu_fx_1 -> apu_fx_0,
 *    apu_fx_4 -> sub_EC2F, apu_fx_10 -> sub_EC6B, apu_fx_14_15 -> sub_ED08 and
 *    sub_ED10, apu_fx_20 -> sub_ED25, sub_ED2F and sub_ED33;
 *  - sub_EEAA's `BEQ locret_EEA9` ($EEA9 is the RTS at the end of sub_EE94)
 *    uses BEQ_RET(), so that RTS stays a comment instead of a C label;
 *  - fall-through between procedures: apu_fx_6 -> apu_fx_2 -> apu_fx_5,
 *    apu_fx_4 -> apu_fx_29 -> sub_EC2F, apu_fx_10 -> sub_EC6B -> apu_fx_7,
 *    apu_fx_23_24_25 -> apu_fx_12, sub_ED08 -> sub_ED0C, sub_ED25 ->
 *    sub_ED2F and apu_reset -> apu_stop, all as a call plus RTS();
 *  - sub_FEE8's trailing `JMP sub_FEE8` is a self-loop and uses the entry
 *    label loc_FEE8 (the idiom of bank7/D2FD.c).
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $EB58  sub_EB58 - subtract $1E/$1F from the 16-bit value at $39E,X
 *
 *  Original: src/bank7.asm:7861-7883 (bank 7)
 *      CMP #8 / BCS loc_EB71 / RTS
 *      LDA $39E,X / SEC / SBC byte_1E / STA $39E,X / LDA $3A4,X /
 *      SBC byte_1F / STA $3A4,X / BCC loc_EB71 / RTS
 *      loc_EB71: LDA #0 / STA $39E,X / STA $3A4,X / RTS
 *
 *  The first RTS makes the $EB5D block unreachable from $EB58 itself and no
 *  other reference to either address exists in the bank; the listing is kept
 *  literally.
 * ------------------------------------------------------------------------ */
void sub_EB58(void)                          /* $EB58 */
{
    CMPI(8);                                 /* CMP #8 */
    BCS(loc_EB71);                           /* BCS loc_EB71 */
    RTS();                                   /* RTS */

    /* $EB5D: unreachable from the entry above; kept literally. */
    LDA(ABSX(0x039E));                       /* LDA $39E,X */
    SEC();                                   /* SEC */
    SBC(byte_1E);                            /* SBC byte_1E */
    STA(ABSX(0x039E));                       /* STA $39E,X */
    LDA(ABSX(0x03A4));                       /* LDA $3A4,X */
    SBC(byte_1F);                            /* SBC byte_1F */
    STA(ABSX(0x03A4));                       /* STA $3A4,X */
    BCC(loc_EB71);                           /* BCC loc_EB71 */
    RTS();                                   /* RTS */

loc_EB71:                                    /* $EB71 */
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x039E));                       /* STA $39E,X */
    STA(ABSX(0x03A4));                       /* STA $3A4,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EB7A  sub_EB7A - the sound-script interpreter
 *
 *  Original: src/bank7.asm:7889-7923 (bank 7)
 *      LDA $344,X / STA byte_1C / LDA $34A,X / STA byte_1D / LDY #$FF
 *      loc_EB86: INY / LDX byte_303 / LDA ($1C),Y / BMI loc_EB91 /
 *      JMP loc_ED56
 *      loc_EB91: CMP #$B0 / BCC loc_EB98 / JMP apu_fx_26_27
 *      loc_EB98: ASL / TAX / LDA #$EB / PHA / LDA #$85 / PHA /
 *      LDA $EBB0,X / PHA / LDA $EBAF,X / PHA / LDX byte_303 / INY /
 *      LDA ($1C),Y / RTS
 *
 *  $EB86 is the entry (after LDY #$FF) *and* the address a dispatched handler
 *  returns to: $EB9A-$EBA7 push $EB85 (the address of the $EB86 tail) and then
 *  word_EBAF[X] (handler-1, so the RTS lands on the handler); the handler's
 *  own RTS pops the $EB85 frame and resumes at $EB86.  The port pushes the same
 *  frames and uses zanac_rts_dispatch() for the computed RTS, then pops the
 *  $EB85 frame itself - which is what the handler's RTS did on the 6502 - and
 *  jumps back to the $EB86 tail.
 *
 *  loc_ED56 ($ED56, the note entry inside apu_fx_26_27) is reproduced at the
 *  end of this function because a C label is function scoped (PORTING.md
 *  section 5), the same idiom bank6/9E2A.c uses for loc_19F05.
 * ------------------------------------------------------------------------ */
void sub_EB7A(void)                          /* $EB7A */
{
    LDA(ABSX(0x0344));                       /* LDA $344,X */
    STA(byte_1C);                            /* STA byte_1C */
    LDA(ABSX(0x034A));                       /* LDA $34A,X */
    STA(byte_1D);                            /* STA byte_1D */
    LDYI(0xFF);                              /* LDY #$FF */

loc_EB86:                                    /* $EB86 - entry, and the RTS target of every handler */
    INY();                                   /* INY */
    LDX(byte_303);                           /* LDX byte_303 */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    BMI(loc_EB91);                           /* BMI loc_EB91 */
    goto loc_ED56;                           /* JMP loc_ED56 (inside apu_fx_26_27) */

loc_EB91:                                    /* $EB91 */
    CMPI(0xB0);                              /* CMP #$B0 */
    BCC(loc_EB98);                           /* BCC loc_EB98 */
    apu_fx_26_27();                          /* JMP apu_fx_26_27 */
    RTS();

loc_EB98:                                    /* $EB98 */
    ASL_A();                                 /* ASL */
    TAX();                                   /* TAX */
    /* $EB9A-$EB9F: LDA #$EB / PHA / LDA #$85 / PHA pushes the $EB85 return
     * address of the dispatch (the handler's RTS lands at $EB86). */
    LDAI(0xEB);                              /* LDA #$EB */
    PHA();                                   /* PHA */
    LDAI(0x85);                              /* LDA #$85 */
    PHA();                                   /* PHA */
    /* $EBA0-$EBA7: LDA $EBB0,X / PHA / LDA $EBAF,X / PHA pushes word_EBAF[X];
     * the table ($EBAF-$EBEA) holds handler-1, so the RTS at $EBAE reaches
     * the handler. */
    LDA(ABSX(0xEBB0));                       /* LDA $EBB0,X */
    PHA();                                   /* PHA */
    LDA(ABSX(0xEBAF));                       /* LDA $EBAF,X */
    PHA();                                   /* PHA */
    LDX(byte_303);                           /* LDX byte_303 */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    zanac_rts_dispatch();                    /* RTS to word_EBAF[X]+1: the fx handler */
    PLA();                                   /* PLA - what the handler's RTS popped ($EB85) */
    PLA();                                   /* PLA */
    goto loc_EB86;                           /* the handler's RTS resumes at $EB86 */

    /* ---- loc_ED56 ($ED56) - the note entry reproduced from apu_fx_26_27 - */
loc_ED56:                                    /* $ED56 */
    STA(ABSX(0x0362));                       /* STA $362,X */
    INY();                                   /* INY */
    /* $ED5A loc_ED5A - reached only by fall-through here, so it stays a
     * comment instead of an unused C label. */
    sub_EDB9();                              /* JSR sub_EDB9 */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    CMPI(0xE0);                              /* CMP #$E0 */
    BCS(loc_ED69);                           /* BCS loc_ED69 */
    LDA(ABSX(0x036E));                       /* LDA $36E,X */
    goto loc_ED7F;                           /* JMP loc_ED7F */

loc_ED69:                                    /* $ED69 */
    SEC();                                   /* SEC */
    SBCI(0xE0);                              /* SBC #$E0 */
    BNE(loc_ED74);                           /* BNE loc_ED74 */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    goto loc_ED7E;                           /* JMP loc_ED7E */

loc_ED74:                                    /* $ED74 */
    STY(byte_304);                           /* STY byte_304 */
    TAY();                                   /* TAY */
    LDA(ABSY(0xF16A));                       /* LDA $F16A,Y */
    LDY(byte_304);                           /* LDY byte_304 */

loc_ED7E:                                    /* $ED7E */
    INY();                                   /* INY */

loc_ED7F:                                    /* $ED7F */
    STA(ABSX(0x035C));                       /* STA $35C,X */
    STA(ABSX(0x036E));                       /* STA $36E,X */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADC(byte_1C);                            /* ADC byte_1C */
    STA(ABSX(0x0344));                       /* STA $344,X */
    LDA(byte_1D);                            /* LDA byte_1D */
    ADCI(0);                                 /* ADC #0 */
    STA(ABSX(0x034A));                       /* STA $34A,X */
    LDA(ABSX(0x0338));                       /* LDA $338,X */
    STA(ABSX(0x0368));                       /* STA $368,X */
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x0356));                       /* STA $356,X */
    STA(ABSX(0x0398));                       /* STA $398,X */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_EDAD);                           /* BEQ loc_EDAD */
    LDA(ABSX(0x0374));                       /* LDA $374,X */
    BNE(loc_EDB0);                           /* BNE loc_EDB0 */

loc_EDAD:                                    /* $EDAD */
    STA(ABSX(0x0392));                       /* STA $392,X */

loc_EDB0:                                    /* $EDB0 */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(0x03EC));                       /* STA $3EC,X */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EBEB  apu_fx_0 - set the script pointer from the next two bytes
 *
 *  Original: src/bank7.asm:7960-7969 (bank 7)
 *      TAX / INY / LDA ($1C),Y / STA byte_1D / STX byte_1C / LDY #$FF / RTS
 *
 *  A = the new pointer high byte, X = the new pointer low byte; the LDY #$FF
 *  makes the $EB86 tail's INY read the first byte of the new block.  Entered
 *  from apu_fx_1's BNE and apu_fx_21's JMP.
 * ------------------------------------------------------------------------ */
void apu_fx_0(void)                          /* $EBEB */
{
    TAX();                                   /* TAX */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    STA(byte_1D);                            /* STA byte_1D */
    STX(byte_1C);                            /* STX byte_1C */
    LDYI(0xFF);                              /* LDY #$FF */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EBF6  apu_fx_1 - repeat the following block until a counter expires
 *
 *  Original: src/bank7.asm:7975-7985 (bank 7)
 *      CLC / ADC byte_303 / TAX / INY / LDA ($1C),Y / DEC $31A,X /
 *      BNE apu_fx_0 / INY / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_1(void)                          /* $EBF6 */
{
    CLC();                                   /* CLC */
    ADC(byte_303);                           /* ADC byte_303 */
    TAX();                                   /* TAX */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    DEC(ABSX(0x031A));                       /* DEC $31A,X */
    BNE(apu_fx_0);                           /* BNE apu_fx_0 */
    INY();                                   /* INY */
    RTS();                                   /* RTS */

apu_fx_0:                                    /* $EBEB - BNE target */
    apu_fx_0();                              /* BNE apu_fx_0 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC05  apu_fx_6 - restart the channel's note
 *
 *  Original: src/bank7.asm:7991-7994 (bank 7)
 *      LDA $3C2,X / STA byte_300
 *  (falls through into apu_fx_2)
 * ------------------------------------------------------------------------ */
void apu_fx_6(void)                          /* $EC05 */
{
    LDA(ABSX(0x03C2));                       /* LDA $3C2,X */
    STA(byte_300);                           /* STA byte_300 */
    apu_fx_2();                              /* falls through into apu_fx_2 ($EC0B) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC0B  apu_fx_2 - silence the channel
 *
 *  Original: src/bank7.asm:8000-8003 (bank 7)
 *      LDA #0 / STA $31A,X
 *  (falls through into apu_fx_5)
 * ------------------------------------------------------------------------ */
void apu_fx_2(void)                          /* $EC0B */
{
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x031A));                       /* STA $31A,X */
    apu_fx_5();                              /* falls through into apu_fx_5 ($EC10) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC10  apu_fx_5 - abandon the slot's update and rejoin apu_update at $EB17
 *
 *  Original: src/bank7.asm:8009-8015 (bank 7)
 *      PLA / PLA / PLA / PLA / JMP loc_EB17
 *
 *  The four pops throw away the $EB85 frame that sub_EB7A's RTS dispatch
 *  pushed *and* the return address of apu_update's `JSR sub_EB7A`; the JMP
 *  then re-enters apu_update at loc_EB17 ($EB17), so the rest of this slot's
 *  update (the sub_EDFC/sub_EEAA calls and the $350/$3AA/$320 block at
 *  $EADF-$EB16) and the remaining script bytes of the frame are skipped.
 *
 *  DEVIATION (PORTING.md section 4c): a C call leaves no return address on the
 *  emulated 6502 stack, so there is no frame to pop, and the JMP target lives
 *  inside apu_update (bank7/E229.c), a file this chunk must not edit.
 *  ZANAC_UNWIND() cannot be used either: the only armed setjmp is the enemy
 *  dispatcher's slot loop (game_logic_processing), and apu_update runs from
 *  the VBlank handler, which the game enters re-entrantly while that jump is
 *  armed (an enemy handler that calls all_enemies_destroyed pumps the engine),
 *  so the longjmp would land in the wrong procedure.  The pops are therefore
 *  dropped and the handler returns normally: sub_EB7A - which always pops the
 *  $EB85 frame after a handler returns - keeps processing the script and
 *  apu_update finishes the slot.  The fx $85 byte occurs once in the whole song
 *  data, in the script of sound 28 (bank7 $FA45, data at $FA2D).
 * ------------------------------------------------------------------------ */
void apu_fx_5(void)                          /* $EC10 */
{
    /* PLA ($EC10) / PLA ($EC11) / PLA ($EC12) / PLA ($EC13) - dropped, and
     * JMP loc_EB17 ($EB17, inside apu_update) - see the note above */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC17  apu_fx_3 - set the channel volume
 *
 *  Original: src/bank7.asm:8021-8024 (bank 7):  STA $32C,X / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_3(void)                          /* $EC17 */
{
    STA(ABSX(0x032C));                       /* STA $32C,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC1B  apu_fx_8 - set the channel's instrument/sound number
 *
 *  Original: src/bank7.asm:8030-8033 (bank 7):  STA $326,X / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_8(void)                          /* $EC1B */
{
    STA(ABSX(0x0326));                       /* STA $326,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC1F  apu_fx_4 - set the low nibble of $320,X
 *
 *  Original: src/bank7.asm:8039-8044 (bank 7)
 *      STA byte_1E / LDA $320,X / AND #$F / BPL sub_EC2F
 *  (falls through into apu_fx_29)
 *
 *  AND #$F always clears bit 7, so the BPL is always taken; the fall-through
 *  is kept literally.
 * ------------------------------------------------------------------------ */
void apu_fx_4(void)                          /* $EC1F */
{
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0x0F);                              /* AND #$F */
    BPL(sub_EC2F);                           /* BPL sub_EC2F */
    apu_fx_29();                             /* falls through into apu_fx_29 ($EC28) */
    RTS();

sub_EC2F:                                    /* $EC2F - BPL target */
    sub_EC2F();                              /* BPL sub_EC2F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC28  apu_fx_29 - set the high nibble of $320,X
 *
 *  Original: src/bank7.asm:8050-8054 (bank 7)
 *      STA byte_1E / LDA $320,X / AND #$F0
 *  (falls through into sub_EC2F)
 * ------------------------------------------------------------------------ */
void apu_fx_29(void)                         /* $EC28 */
{
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0xF0);                              /* AND #$F0 */
    sub_EC2F();                              /* falls through into sub_EC2F ($EC2F) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC2F  sub_EC2F - merge byte_1E into the high nibble of $320,X
 *
 *  Original: src/bank7.asm:8060-8064 (bank 7)
 *      ORA byte_1E / STA $320,X / RTS
 * ------------------------------------------------------------------------ */
void sub_EC2F(void)                          /* $EC2F */
{
    ORA(byte_1E);                            /* ORA byte_1E */
    STA(ABSX(0x0320));                       /* STA $320,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC35  apu_fx_9 - add to the channel's frequency offset
 *
 *  Original: src/bank7.asm:8070-8078 (bank 7)
 *      BEQ loc_EC3B / CLC / ADC $332,X / loc_EC3B: STA $332,X / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_9(void)                          /* $EC35 */
{
    BEQ(loc_EC3B);                           /* BEQ loc_EC3B */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0332));                       /* ADC $332,X */

loc_EC3B:                                    /* $EC3B */
    STA(ABSX(0x0332));                       /* STA $332,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC3F  apu_fx_13 - start another channel at the same position
 *
 *  Original: src/bank7.asm:8084-8092 (bank 7)
 *      CLC / ADC byte_303 / TAX / INY / LDA ($1C),Y / STA $31A,X / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_13(void)                         /* $EC3F */
{
    CLC();                                   /* CLC */
    ADC(byte_303);                           /* ADC byte_303 */
    TAX();                                   /* TAX */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    STA(ABSX(0x031A));                       /* STA $31A,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC4B  apu_fx_16 - set the channel's flags
 *
 *  Original: src/bank7.asm:8098-8101 (bank 7):  STA $31A,X / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_16(void)                         /* $EC4B */
{
    STA(ABSX(0x031A));                       /* STA $31A,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC4F  apu_fx_10 - add to the low nibble of $320,X
 *
 *  Original: src/bank7.asm:8107-8125 (bank 7)
 *      PHA / LDA $320,X / AND #$F / STA byte_1E / PLA / CLC /
 *      BPL loc_EC63 / ADC byte_1E / BCS sub_EC6B / LDA #0 / BEQ sub_EC6B
 *      loc_EC63: ADC byte_1E / CMP #$10 / BCC sub_EC6B / LDA #$F
 *  (falls through into sub_EC6B)
 * ------------------------------------------------------------------------ */
void apu_fx_10(void)                         /* $EC4F */
{
    PHA();                                   /* PHA */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0x0F);                              /* AND #$F */
    STA(byte_1E);                            /* STA byte_1E */
    PLA();                                   /* PLA */
    CLC();                                   /* CLC */
    BPL(loc_EC63);                           /* BPL loc_EC63 */
    ADC(byte_1E);                            /* ADC byte_1E */
    BCS(sub_EC6B);                           /* BCS sub_EC6B */
    LDAI(0);                                 /* LDA #0 */
    BEQ(sub_EC6B);                           /* BEQ sub_EC6B */

loc_EC63:                                    /* $EC63 */
    ADC(byte_1E);                            /* ADC byte_1E */
    CMPI(0x10);                              /* CMP #$10 */
    BCC(sub_EC6B);                           /* BCC sub_EC6B */
    LDAI(0x0F);                              /* LDA #$F */
    sub_EC6B();                              /* falls through into sub_EC6B ($EC6B) */
    RTS();

sub_EC6B:                                    /* $EC6B - the BCS/BEQ/BCC target */
    sub_EC6B();                              /* BCS/BEQ/BCC sub_EC6B */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC6B  sub_EC6B - merge byte_1E into the low nibble of $320,X
 *
 *  Original: src/bank7.asm:8131-8137 (bank 7)
 *      STA byte_1E / LDA $320,X / AND #$F0 / ORA byte_1E
 *  (falls through into apu_fx_7)
 * ------------------------------------------------------------------------ */
void sub_EC6B(void)                          /* $EC6B */
{
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0xF0);                              /* AND #$F0 */
    ORA(byte_1E);                            /* ORA byte_1E */
    apu_fx_7();                              /* falls through into apu_fx_7 ($EC74) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EC74  apu_fx_7 - store $320,X
 *
 *  Original: src/bank7.asm:8143-8146 (bank 7):  STA $320,X / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_7(void)                          /* $EC74 */
{
    STA(ABSX(0x0320));                       /* STA $320,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC78  apu_fx_17 - add flags to byte_300
 *
 *  Original: src/bank7.asm:8152-8156 (bank 7)
 *      ORA byte_300 / STA byte_300 / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_17(void)                         /* $EC78 */
{
    ORA(byte_300);                           /* ORA byte_300 */
    STA(byte_300);                           /* STA byte_300 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EC7F  apu_fx_19 - start the volume envelope of the channel
 *
 *  Original: src/bank7.asm:8162-8181 (bank 7)
 *      STA byte_1E / AND #$3F / STA $3B6,X / LDA $350,X / AND #$CD /
 *      STA $350,X / LDA #0 / STA $3B0,X / INY / LDA ($1C),Y / STA $3AA,X /
 *      BEQ locret_ECA3 / LDA $350,X / ORA #$20 / STA $350,X /
 *      locret_ECA3: RTS
 * ------------------------------------------------------------------------ */
void apu_fx_19(void)                         /* $EC7F */
{
    STA(byte_1E);                            /* STA byte_1E */
    ANDI(0x3F);                              /* AND #$3F */
    STA(ABSX(0x03B6));                       /* STA $3B6,X */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(0xCD);                              /* AND #$CD */
    STA(ABSX(0x0350));                       /* STA $350,X */
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x03B0));                       /* STA $3B0,X */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    STA(ABSX(0x03AA));                       /* STA $3AA,X */
    BEQ(locret_ECA3);                        /* BEQ locret_ECA3 */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ORAI(0x20);                              /* ORA #$20 */
    STA(ABSX(0x0350));                       /* STA $350,X */

locret_ECA3:                                 /* $ECA3 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ECA4  apu_fx_21 - repeat or skip according to the channel gate
 *
 *  Original: src/bank7.asm:8187-8193 (bank 7)
 *      LDA $350,X / AND #2 / BNE sub_ECB0 / LDA ($1C),Y / JMP apu_fx_0
 * ------------------------------------------------------------------------ */
void apu_fx_21(void)                         /* $ECA4 */
{
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(2);                                 /* AND #2 */
    BNE(sub_ECB0);                           /* BNE sub_ECB0 */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    apu_fx_0();                              /* JMP apu_fx_0 */
    RTS();

sub_ECB0:                                    /* $ECB0 - BNE target */
    sub_ECB0();                              /* BNE sub_ECB0 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ECB0  sub_ECB0 - skip the next script byte
 *
 *  Original: src/bank7.asm:8199-8202 (bank 7):  INY / RTS
 * ------------------------------------------------------------------------ */
void sub_ECB0(void)                          /* $ECB0 */
{
    INY();                                   /* INY */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ECB2  apu_fx_18 - add a signed step from a table to $332,X
 *
 *  Original: src/bank7.asm:8208-8226 (bank 7)
 *      CLC / DEC $37A,X / ADC $37A,X / INC $37A,X / STA byte_1E / INY /
 *      LDA ($1C),Y / ADC #0 / STA byte_1F / STY byte_304 / LDY #0 /
 *      LDA ($1E),Y / LDY byte_304 / CLC / ADC $332,X / STA $332,X / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_18(void)                         /* $ECB2 */
{
    CLC();                                   /* CLC */
    DEC(ABSX(0x037A));                       /* DEC $37A,X */
    ADC(ABSX(0x037A));                       /* ADC $37A,X */
    INC(ABSX(0x037A));                       /* INC $37A,X */
    STA(byte_1E);                            /* STA byte_1E */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    ADCI(0);                                 /* ADC #0 */
    STA(byte_1F);                            /* STA byte_1F */
    STY(byte_304);                           /* STY byte_304 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    LDY(byte_304);                           /* LDY byte_304 */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0332));                       /* ADC $332,X */
    STA(ABSX(0x0332));                       /* STA $332,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ECD7  apu_fx_22 - enable the hardware sweep of the channel
 *
 *  Original: src/bank7.asm:8232-8247 (bank 7)
 *      STA $386,X / LDA #$80 / STA $38C,X / LDA $350,X / AND #$FE /
 *      STA $350,X / LDA $386,X / BEQ locret_ECF4 / LDA $350,X / ORA #1 /
 *      STA $350,X / locret_ECF4: RTS
 * ------------------------------------------------------------------------ */
void apu_fx_22(void)                         /* $ECD7 */
{
    STA(ABSX(0x0386));                       /* STA $386,X */
    LDAI(0x80);                              /* LDA #$80 */
    STA(ABSX(0x038C));                       /* STA $38C,X */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(0xFE);                              /* AND #$FE */
    STA(ABSX(0x0350));                       /* STA $350,X */
    LDA(ABSX(0x0386));                       /* LDA $386,X */
    BEQ(locret_ECF4);                        /* BEQ locret_ECF4 */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ORAI(1);                                 /* ORA #1 */
    STA(ABSX(0x0350));                       /* STA $350,X */

locret_ECF4:                                 /* $ECF4 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ECF5  apu_fx_14_15 - transpose $3CE,X
 *
 *  Original: src/bank7.asm:8253-8265 (bank 7)
 *      STA byte_1E / ASL / BEQ sub_ED10 / LDA $3CE,X / PHP / CLC /
 *      ADC byte_1E / AND #$F / PLP / BPL sub_ED08 / ORA #$80
 *  (falls through into sub_ED08)
 * ------------------------------------------------------------------------ */
void apu_fx_14_15(void)                      /* $ECF5 */
{
    STA(byte_1E);                            /* STA byte_1E */
    ASL_A();                                 /* ASL */
    BEQ(sub_ED10);                           /* BEQ sub_ED10 */
    LDA(ABSX(0x03CE));                       /* LDA $3CE,X */
    PHP();                                   /* PHP */
    CLC();                                   /* CLC */
    ADC(byte_1E);                            /* ADC byte_1E */
    ANDI(0x0F);                              /* AND #$F */
    PLP();                                   /* PLP */
    BPL(sub_ED08);                           /* BPL sub_ED08 */
    ORAI(0x80);                              /* ORA #$80 */
    sub_ED08();                              /* falls through into sub_ED08 ($ED08) */
    RTS();

sub_ED10:                                    /* $ED10 - BEQ target */
    sub_ED10();                              /* BEQ sub_ED10 */
    RTS();

sub_ED08:                                    /* $ED08 - BPL target */
    sub_ED08();                              /* BPL sub_ED08 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ED08  sub_ED08 - store the transposed value
 *
 *  Original: src/bank7.asm:8271-8274 (bank 7)
 *      BCC sub_ED0C / EOR #$80
 *  (falls through into sub_ED0C)
 * ------------------------------------------------------------------------ */
void sub_ED08(void)                          /* $ED08 */
{
    BCC(sub_ED0C);                           /* BCC sub_ED0C */
    EORI(0x80);                              /* EOR #$80 */
    sub_ED0C();                              /* falls through into sub_ED0C ($ED0C) */
    RTS();

sub_ED0C:                                    /* $ED0C - BCC target */
    sub_ED0C();                              /* BCC sub_ED0C */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ED0C  sub_ED0C - store $3CE,X
 *
 *  Original: src/bank7.asm:8280-8283 (bank 7):  STA $3CE,X / RTS
 * ------------------------------------------------------------------------ */
void sub_ED0C(void)                          /* $ED0C */
{
    STA(ABSX(0x03CE));                       /* STA $3CE,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ED10  sub_ED10 - halve $3CE,X (fx 14/15 with a zero argument)
 *
 *  Original: src/bank7.asm:8289-8293 (bank 7):  LSR / STA $3CE,X / RTS
 * ------------------------------------------------------------------------ */
void sub_ED10(void)                          /* $ED10 */
{
    LSR_A();                                 /* LSR */
    STA(ABSX(0x03CE));                       /* STA $3CE,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ED15  apu_fx_20 - set or clear the channel's sweep direction
 *
 *  Original: src/bank7.asm:8299-8307 (bank 7)
 *      BMI sub_ED25 / BEQ sub_ED33 / STA $3BC,X / LDA $350,X / AND #$BF /
 *      ORA #$80 / BMI sub_ED2F
 *  (falls through into sub_ED25)
 *
 *  A dispatched fx byte is always $80-$AF, so the first BMI is always taken
 *  and the rest of the block is dead; kept literally.
 * ------------------------------------------------------------------------ */
void apu_fx_20(void)                         /* $ED15 */
{
    BMI(sub_ED25);                           /* BMI sub_ED25 */
    BEQ(sub_ED33);                           /* BEQ sub_ED33 */
    STA(ABSX(0x03BC));                       /* STA $3BC,X */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(0xBF);                              /* AND #$BF */
    ORAI(0x80);                              /* ORA #$80 */
    BMI(sub_ED2F);                           /* BMI sub_ED2F */
    sub_ED25();                              /* falls through into sub_ED25 ($ED25) */
    RTS();

sub_ED25:                                    /* $ED25 - BMI target */
    sub_ED25();                              /* BMI sub_ED25 */
    RTS();

sub_ED33:                                    /* $ED33 - BEQ target */
    sub_ED33();                              /* BEQ sub_ED33 */
    RTS();

sub_ED2F:                                    /* $ED2F - BMI target */
    sub_ED2F();                              /* BMI sub_ED2F */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ED25  sub_ED25 - sweep upwards ($3BC,X = A & $7F)
 *
 *  Original: src/bank7.asm:8313-8318 (bank 7)
 *      AND #$7F / STA $3BC,X / LDA $350,X / ORA #$C0
 *  (falls through into sub_ED2F)
 * ------------------------------------------------------------------------ */
void sub_ED25(void)                          /* $ED25 */
{
    ANDI(0x7F);                              /* AND #$7F */
    STA(ABSX(0x03BC));                       /* STA $3BC,X */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ORAI(0xC0);                              /* ORA #$C0 */
    sub_ED2F();                              /* falls through into sub_ED2F ($ED2F) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ED2F  sub_ED2F - store $350,X
 *
 *  Original: src/bank7.asm:8324-8327 (bank 7):  STA $350,X / RTS
 * ------------------------------------------------------------------------ */
void sub_ED2F(void)                          /* $ED2F */
{
    STA(ABSX(0x0350));                       /* STA $350,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ED33  sub_ED33 - clear the sweep enable of the channel
 *
 *  Original: src/bank7.asm:8333-8338 (bank 7)
 *      LDA $350,X / AND #$7F / STA $350,X / RTS
 * ------------------------------------------------------------------------ */
void sub_ED33(void)                          /* $ED33 */
{
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ANDI(0x7F);                              /* AND #$7F */
    STA(ABSX(0x0350));                       /* STA $350,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ED3C  apu_fx_23_24_25 - toggle channel flag $10
 *
 *  Original: src/bank7.asm:8344-8348 (bank 7)
 *      LDA unk_31A,X / EOR #$10 / STA unk_31A,X
 *  (falls through into apu_fx_12)
 * ------------------------------------------------------------------------ */
void apu_fx_23_24_25(void)                   /* $ED3C */
{
    LDA(ABSX(unk_31A));                      /* LDA unk_31A,X */
    EORI(0x10);                              /* EOR #$10 */
    STA(ABSX(unk_31A));                      /* STA unk_31A,X */
    apu_fx_12();                             /* falls through into apu_fx_12 ($ED44) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $ED44  apu_fx_12 - back up one script byte
 *
 *  Original: src/bank7.asm:8354-8357 (bank 7):  DEY / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_12(void)                         /* $ED44 */
{
    DEY();                                   /* DEY */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ED46  apu_fx_28 - toggle channel flag $04 and back up one byte
 *
 *  Original: src/bank7.asm:8363-8369 (bank 7)
 *      LDA unk_31A,X / EOR #4 / STA unk_31A,X / DEY / RTS
 * ------------------------------------------------------------------------ */
void apu_fx_28(void)                         /* $ED46 */
{
    LDA(ABSX(unk_31A));                      /* LDA unk_31A,X */
    EORI(4);                                 /* EOR #4 */
    STA(ABSX(unk_31A));                      /* STA unk_31A,X */
    DEY();                                   /* DEY */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $ED50  apu_fx_26_27 - fx 26/27, and the note/duration handler
 *
 *  Original: src/bank7.asm:8375-8440 (bank 7)
 *      apu_fx_26_27: LDA $362,X / JMP loc_ED5A
 *      loc_ED56: STA $362,X / INY
 *      loc_ED5A: JSR sub_EDB9 / LDA ($1C),Y / CMP #$E0 / BCS loc_ED69 /
 *      LDA $36E,X / JMP loc_ED7F
 *      loc_ED69: SEC / SBC #$E0 / BNE loc_ED74 / INY / LDA ($1C),Y /
 *      JMP loc_ED7E
 *      loc_ED74: STY byte_304 / TAY / LDA $F16A,Y / LDY byte_304
 *      loc_ED7E: INY
 *      loc_ED7F: STA $35C,X / STA $36E,X / TYA / CLC / ADC byte_1C /
 *      STA $344,X / LDA byte_1D / ADC #0 / STA $34A,X / LDA $338,X /
 *      STA $368,X / LDA #0 / STA $356,X / STA $398,X / LDA $31A,X /
 *      AND #$10 / BEQ loc_EDAD / LDA $374,X / BNE loc_EDB0 /
 *      loc_EDAD: STA $392,X /
 *      loc_EDB0: LDA $3EC,X / ORA #$80 / STA $3EC,X / RTS
 *
 *  loc_ED56 is not used inside this procedure (sub_EB7A jumps there for every
 *  note byte and reproduces it), so it stays a comment here.
 * ------------------------------------------------------------------------ */
void apu_fx_26_27(void)                      /* $ED50 */
{
    LDA(ABSX(0x0362));                       /* LDA $362,X */
    /* JMP loc_ED5A - loc_ED56 ($ED56) is the note entry used by sub_EB7A */
    goto loc_ED5A;

loc_ED5A:                                    /* $ED5A */
    sub_EDB9();                              /* JSR sub_EDB9 */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    CMPI(0xE0);                              /* CMP #$E0 */
    BCS(loc_ED69);                           /* BCS loc_ED69 */
    LDA(ABSX(0x036E));                       /* LDA $36E,X */
    goto loc_ED7F;                           /* JMP loc_ED7F */

loc_ED69:                                    /* $ED69 */
    SEC();                                   /* SEC */
    SBCI(0xE0);                              /* SBC #$E0 */
    BNE(loc_ED74);                           /* BNE loc_ED74 */
    INY();                                   /* INY */
    LDA(INDY(byte_1C));                      /* LDA ($1C),Y */
    goto loc_ED7E;                           /* JMP loc_ED7E */

loc_ED74:                                    /* $ED74 */
    STY(byte_304);                           /* STY byte_304 */
    TAY();                                   /* TAY */
    LDA(ABSY(0xF16A));                       /* LDA $F16A,Y */
    LDY(byte_304);                           /* LDY byte_304 */

loc_ED7E:                                    /* $ED7E */
    INY();                                   /* INY */

loc_ED7F:                                    /* $ED7F */
    STA(ABSX(0x035C));                       /* STA $35C,X */
    STA(ABSX(0x036E));                       /* STA $36E,X */
    TYA();                                   /* TYA */
    CLC();                                   /* CLC */
    ADC(byte_1C);                            /* ADC byte_1C */
    STA(ABSX(0x0344));                       /* STA $344,X */
    LDA(byte_1D);                            /* LDA byte_1D */
    ADCI(0);                                 /* ADC #0 */
    STA(ABSX(0x034A));                       /* STA $34A,X */
    LDA(ABSX(0x0338));                       /* LDA $338,X */
    STA(ABSX(0x0368));                       /* STA $368,X */
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x0356));                       /* STA $356,X */
    STA(ABSX(0x0398));                       /* STA $398,X */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_EDAD);                           /* BEQ loc_EDAD */
    LDA(ABSX(0x0374));                       /* LDA $374,X */
    BNE(loc_EDB0);                           /* BNE loc_EDB0 */

loc_EDAD:                                    /* $EDAD */
    STA(ABSX(0x0392));                       /* STA $392,X */

loc_EDB0:                                    /* $EDB0 */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    ORAI(0x80);                              /* ORA #$80 */
    STA(ABSX(0x03EC));                       /* STA $3EC,X */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EDB9  sub_EDB9 - load the channel's period step
 *
 *  Original: src/bank7.asm:8446-8484 (bank 7)
 *      STA $39E,X / AND #$FF / BNE loc_EDC4 / STA $3A4,X / RTS
 *      loc_EDC4: LDA $33E,X / CMP #3 / BNE loc_EDE0 / LDA #1 /
 *      STA $3A4,X / DEC $39E,X / LDA $39E,X / CMP #$10 / BCC locret_EDDF /
 *      ADC #$6F / STA $39E,X /
 *      locret_EDDF: RTS
 *      loc_EDE0: LDA $39E,X / CLC / ADC $332,X / STY byte_304 / ASL / TAY /
 *      LDA $F002,Y / STA $39E,X / LDA $F003,Y / STA $3A4,X / LDY byte_304 /
 *      RTS
 *
 *  $F002/$F003 are read literally ($F002 is inside sub_EFA6); the table is
 *  indexed by twice the period, exactly like the original.
 * ------------------------------------------------------------------------ */
void sub_EDB9(void)                          /* $EDB9 */
{
    STA(ABSX(0x039E));                       /* STA $39E,X */
    ANDI(0xFF);                              /* AND #$FF */
    BNE(loc_EDC4);                           /* BNE loc_EDC4 */
    STA(ABSX(0x03A4));                       /* STA $3A4,X */
    RTS();                                   /* RTS */

loc_EDC4:                                    /* $EDC4 */
    LDA(ABSX(0x033E));                       /* LDA $33E,X */
    CMPI(3);                                 /* CMP #3 */
    BNE(loc_EDE0);                           /* BNE loc_EDE0 */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(0x03A4));                       /* STA $3A4,X */
    DEC(ABSX(0x039E));                       /* DEC $39E,X */
    LDA(ABSX(0x039E));                       /* LDA $39E,X */
    CMPI(0x10);                              /* CMP #$10 */
    BCC(locret_EDDF);                        /* BCC locret_EDDF */
    ADCI(0x6F);                              /* ADC #$6F */
    STA(ABSX(0x039E));                       /* STA $39E,X */

locret_EDDF:                                 /* $EDDF */
    RTS();

loc_EDE0:                                    /* $EDE0 */
    LDA(ABSX(0x039E));                       /* LDA $39E,X */
    CLC();                                   /* CLC */
    ADC(ABSX(0x0332));                       /* ADC $332,X */
    STY(byte_304);                           /* STY byte_304 */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(ABSY(0xF002));                       /* LDA $F002,Y */
    STA(ABSX(0x039E));                       /* STA $39E,X */
    LDA(ABSY(0xF003));                       /* LDA $F003,Y */
    STA(ABSX(0x03A4));                       /* STA $3A4,X */
    LDY(byte_304);                           /* LDY byte_304 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EDFC  sub_EDFC - update $374,X from the period table word_F190
 *
 *  Original: src/bank7.asm:8490-8529 (bank 7)
 *      LDA $320,X / AND #$10 / BEQ loc_EE3C / LDA $326,X / BEQ loc_EE3C /
 *      ASL / TAY / LDA word_F190,Y / STA byte_1E / LDA word_F190+1,Y /
 *      STA byte_1F / LDY $392,X / JSR sub_EE71 / INY / STA byte_1E /
 *      LDA $320,X / AND #$F / CLC / ADC byte_1E / SBC #$E /
 *      STA apu_sound_length / TYA / STA $392,X / LDA $320,X / AND #$F0 /
 *      BCC loc_EE38 / ORA apu_sound_length /
 *      loc_EE38: STA $374,X / RTS
 *      loc_EE3C: LDA $320,X / STA $374,X / RTS
 * ------------------------------------------------------------------------ */
void sub_EDFC(void)                          /* $EDFC */
{
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0x10);                              /* AND #$10 */
    BEQ(loc_EE3C);                           /* BEQ loc_EE3C */
    LDA(ABSX(0x0326));                       /* LDA $326,X */
    BEQ(loc_EE3C);                           /* BEQ loc_EE3C */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(ABSY(word_F190));                    /* LDA word_F190,Y */
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSY(word_F190 + 1));                /* LDA word_F190+1,Y */
    STA(byte_1F);                            /* STA byte_1F */
    LDY(ABSX(0x0392));                       /* LDY $392,X */
    sub_EE71();                              /* JSR sub_EE71 */
    INY();                                   /* INY */
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0x0F);                              /* AND #$F */
    CLC();                                   /* CLC */
    ADC(byte_1E);                            /* ADC byte_1E */
    SBCI(0x0E);                              /* SBC #$E */
    STA(apu_sound_length);                   /* STA apu_sound_length */
    TYA();                                   /* TYA */
    STA(ABSX(0x0392));                       /* STA $392,X */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    ANDI(0xF0);                              /* AND #$F0 */
    BCC(loc_EE38);                           /* BCC loc_EE38 */
    ORA(apu_sound_length);                   /* ORA apu_sound_length */

loc_EE38:                                    /* $EE38 */
    STA(ABSX(0x0374));                       /* STA $374,X */
    RTS();

loc_EE3C:                                    /* $EE3C */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    STA(ABSX(0x0374));                       /* STA $374,X */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EE43  sub_EE43 - the same as sub_EDFC, for the ongoing note
 *
 *  Original: src/bank7.asm:8535-8560 (bank 7)
 *      LDA $326,X / BEQ loc_EE3C / ASL / TAY / LDA word_F190,Y /
 *      STA byte_1E / LDA word_F190+1,Y / STA byte_1F / LDY $392,X /
 *      JSR sub_EE71 / INY / STA byte_1E / LDA $320,X / CLC / ADC byte_1E /
 *      SBC #$1F / BCS loc_EE69 / LDA #0 /
 *      loc_EE69: STA $374,X / TYA / STA $392,X / RTS
 *
 *  loc_EE3C ($EE3C) is the shared tail inside sub_EDFC, so it is reproduced
 *  here (PORTING.md section 5).
 * ------------------------------------------------------------------------ */
void sub_EE43(void)                          /* $EE43 */
{
    LDA(ABSX(0x0326));                       /* LDA $326,X */
    BEQ(loc_EE3C);                           /* BEQ loc_EE3C (inside sub_EDFC) */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDA(ABSY(word_F190));                    /* LDA word_F190,Y */
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSY(word_F190 + 1));                /* LDA word_F190+1,Y */
    STA(byte_1F);                            /* STA byte_1F */
    LDY(ABSX(0x0392));                       /* LDY $392,X */
    sub_EE71();                              /* JSR sub_EE71 */
    INY();                                   /* INY */
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    CLC();                                   /* CLC */
    ADC(byte_1E);                            /* ADC byte_1E */
    SBCI(0x1F);                              /* SBC #$1F */
    BCS(loc_EE69);                           /* BCS loc_EE69 */
    LDAI(0);                                 /* LDA #0 */

loc_EE69:                                    /* $EE69 */
    STA(ABSX(0x0374));                       /* STA $374,X */
    TYA();                                   /* TYA */
    STA(ABSX(0x0392));                       /* STA $392,X */
    RTS();

loc_EE3C:                                    /* $EE3C - the shared tail inside sub_EDFC */
    LDA(ABSX(0x0320));                       /* LDA $320,X */
    STA(ABSX(0x0374));                       /* STA $374,X */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EE71  sub_EE71 - read one compass entry at ($1E),Y
 *
 *  Original: src/bank7.asm:8566-8601 (bank 7)
 *      LDA ($1E),Y / BMI loc_EE76 / RTS
 *      loc_EE76: CMP #$80 / BNE loc_EE7E / DEY / LDA ($1E),Y / RTS
 *      loc_EE7E: CMP #$81 / BNE loc_EE8B / TYA / INY / SEC / SBC ($1E),Y /
 *      TAY / LDA ($1E),Y / RTS
 *      loc_EE8B: CMP #$82 / BNE locret_EE93 / LDY #0 / LDA ($1E),Y /
 *      locret_EE93: RTS
 * ------------------------------------------------------------------------ */
void sub_EE71(void)                          /* $EE71 */
{
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    BMI(loc_EE76);                           /* BMI loc_EE76 */
    RTS();                                   /* RTS */

loc_EE76:                                    /* $EE76 */
    CMPI(0x80);                              /* CMP #$80 */
    BNE(loc_EE7E);                           /* BNE loc_EE7E */
    DEY();                                   /* DEY */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    RTS();                                   /* RTS */

loc_EE7E:                                    /* $EE7E */
    CMPI(0x81);                              /* CMP #$81 */
    BNE(loc_EE8B);                           /* BNE loc_EE8B */
    TYA();                                   /* TYA */
    INY();                                   /* INY */
    SEC();                                   /* SEC */
    SBC(INDY(byte_1E));                      /* SBC ($1E),Y */
    TAY();                                   /* TAY */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    RTS();                                   /* RTS */

loc_EE8B:                                    /* $EE8B */
    CMPI(0x82);                              /* CMP #$82 */
    BNE(locret_EE93);                        /* BNE locret_EE93 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */

locret_EE93:                                 /* $EE93 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EE94  sub_EE94 - reset the per-frame channel accumulators
 *
 *  Original: src/bank7.asm:8607-8619 (bank 7)
 *      LDY #$FF / STY byte_30C / STY byte_310 / STY byte_314 / STY byte_318 /
 *      INY / STY byte_307 / STY byte_319 / locret_EEA9: RTS
 *
 *  locret_EEA9 ($EEA9) is the RTS here; sub_EEAA branches into it with
 *  BEQ_RET(), so it stays a comment instead of an (unused) C label.
 * ------------------------------------------------------------------------ */
void sub_EE94(void)                          /* $EE94 */
{
    LDYI(0xFF);                              /* LDY #$FF */
    STY(byte_30C);                           /* STY byte_30C */
    STY(byte_310);                           /* STY byte_310 */
    STY(byte_314);                           /* STY byte_314 */
    STY(byte_318);                           /* STY byte_318 */
    INY();                                   /* INY */
    STY(byte_307);                           /* STY byte_307 */
    STY(byte_319);                           /* STY byte_319 */
    /* $EEA9 locret_EEA9 - the RTS; sub_EEAA branches into it (BEQ_RET) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EEAA  sub_EEAA - publish the channel state to the APU shadow registers
 *
 *  Original: src/bank7.asm:8625-8703 (bank 7)
 *      LDA byte_319 / AND #$20 / BEQ loc_EEBA
 *      loc_EEB1: LDA $3EC,X / ORA #$40 / STA $3EC,X / RTS
 *      loc_EEBA: LDA $31A,X / ORA byte_319 / STA byte_319 / LDA $31A,X /
 *      AND #3 / BEQ locret_EEA9 / LDY $33E,X / LDA unk_EF42,Y /
 *      AND byte_307 / BNE loc_EEB1 / LDA unk_EF46,Y / AND byte_307 /
 *      STA byte_307 / TYA / ASL / ASL / TAY / LDA #$FF / STA $30C,Y /
 *      LDA $39E,X / ORA $3A4,X / BEQ loc_EF39 / LDA $31A,X / AND #2 /
 *      BEQ loc_EEF9 / JSR sub_EF4A /
 *      loc_EEF9: LDA $31A,X / AND #1 / BEQ loc_EF39 / LDA $374,X /
 *      STA $309,Y / LDA $32C,X / STA $30A,Y / LDA $39E,X / STA $30B,Y /
 *      LDA $3EC,X / BMI loc_EF22 / AND #$40 / BNE loc_EF22 / LDA $32C,X /
 *      BEQ loc_EF2D / BMI loc_EF2D /
 *      loc_EF22: LDA $326,X / AND #$F8 / ORA $3A4,X / STA $30C,Y /
 *      loc_EF2D: LDY $33E,X / LDA unk_EF42,Y / ORA byte_307 /
 *      STA byte_307 /
 *      loc_EF39: LDA $3EC,X / AND #$BF / STA $3EC,X / RTS
 * ------------------------------------------------------------------------ */
void sub_EEAA(void)                          /* $EEAA */
{
    LDA(byte_319);                           /* LDA byte_319 */
    ANDI(0x20);                              /* AND #$20 */
    BEQ(loc_EEBA);                           /* BEQ loc_EEBA */

loc_EEB1:                                    /* $EEB1 */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    ORAI(0x40);                              /* ORA #$40 */
    STA(ABSX(0x03EC));                       /* STA $3EC,X */
    RTS();                                   /* RTS */

loc_EEBA:                                    /* $EEBA */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    ORA(byte_319);                           /* ORA byte_319 */
    STA(byte_319);                           /* STA byte_319 */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    ANDI(3);                                 /* AND #3 */
    BEQ_RET(locret_EEA9);                    /* BEQ locret_EEA9 (RTS in sub_EE94) */
    LDY(ABSX(0x033E));                       /* LDY $33E,X */
    LDA(ABSY(unk_EF42));                     /* LDA unk_EF42,Y */
    AND(byte_307);                           /* AND byte_307 */
    BNE(loc_EEB1);                           /* BNE loc_EEB1 */
    LDA(ABSY(unk_EF46));                     /* LDA unk_EF46,Y */
    AND(byte_307);                           /* AND byte_307 */
    STA(byte_307);                           /* STA byte_307 */
    TYA();                                   /* TYA */
    ASL_A();                                 /* ASL */
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    LDAI(0xFF);                              /* LDA #$FF */
    STA(ABSY(0x030C));                       /* STA $30C,Y */
    LDA(ABSX(0x039E));                       /* LDA $39E,X */
    ORA(ABSX(0x03A4));                       /* ORA $3A4,X */
    BEQ(loc_EF39);                           /* BEQ loc_EF39 */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    ANDI(2);                                 /* AND #2 */
    BEQ(loc_EEF9);                           /* BEQ loc_EEF9 */
    sub_EF4A();                              /* JSR sub_EF4A */

loc_EEF9:                                    /* $EEF9 */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    ANDI(1);                                 /* AND #1 */
    BEQ(loc_EF39);                           /* BEQ loc_EF39 */
    LDA(ABSX(0x0374));                       /* LDA $374,X */
    STA(ABSY(0x0309));                       /* STA $309,Y */
    LDA(ABSX(0x032C));                       /* LDA $32C,X */
    STA(ABSY(0x030A));                       /* STA $30A,Y */
    LDA(ABSX(0x039E));                       /* LDA $39E,X */
    STA(ABSY(0x030B));                       /* STA $30B,Y */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    BMI(loc_EF22);                           /* BMI loc_EF22 */
    ANDI(0x40);                              /* AND #$40 */
    BNE(loc_EF22);                           /* BNE loc_EF22 */
    LDA(ABSX(0x032C));                       /* LDA $32C,X */
    BEQ(loc_EF2D);                           /* BEQ loc_EF2D */
    BMI(loc_EF2D);                           /* BMI loc_EF2D */

loc_EF22:                                    /* $EF22 */
    LDA(ABSX(0x0326));                       /* LDA $326,X */
    ANDI(0xF8);                              /* AND #$F8 */
    ORA(ABSX(0x03A4));                       /* ORA $3A4,X */
    STA(ABSY(0x030C));                       /* STA $30C,Y */

loc_EF2D:                                    /* $EF2D */
    LDY(ABSX(0x033E));                       /* LDY $33E,X */
    LDA(ABSY(unk_EF42));                     /* LDA unk_EF42,Y */
    ORA(byte_307);                           /* ORA byte_307 */
    STA(byte_307);                           /* STA byte_307 */

loc_EF39:                                    /* $EF39 */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    ANDI(0xBF);                              /* AND #$BF */
    STA(ABSX(0x03EC));                       /* STA $3EC,X */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EF4A  sub_EF4A - publish the ongoing note of channel X
 *
 *  Original: src/bank7.asm:8718-8737 (bank 7)
 *      LDA byte_307 / AND #8 / BNE locret_EF70 / LDA byte_307 / ORA #8 /
 *      STA byte_307 / LDA $374,X / STA byte_315 / LDA $3CE,X / STA byte_317 /
 *      LDA $3EC,X / BPL locret_EF70 / LDA $326,X / STA byte_318 /
 *      locret_EF70: RTS
 * ------------------------------------------------------------------------ */
void sub_EF4A(void)                          /* $EF4A */
{
    LDA(byte_307);                           /* LDA byte_307 */
    ANDI(8);                                 /* AND #8 */
    BNE(locret_EF70);                        /* BNE locret_EF70 */
    LDA(byte_307);                           /* LDA byte_307 */
    ORAI(8);                                 /* ORA #8 */
    STA(byte_307);                           /* STA byte_307 */
    LDA(ABSX(0x0374));                       /* LDA $374,X */
    STA(byte_315);                           /* STA byte_315 */
    LDA(ABSX(0x03CE));                       /* LDA $3CE,X */
    STA(byte_317);                           /* STA byte_317 */
    LDA(ABSX(0x03EC));                       /* LDA $3EC,X */
    BPL(locret_EF70);                        /* BPL locret_EF70 */
    LDA(ABSX(0x0326));                       /* LDA $326,X */
    STA(byte_318);                           /* STA byte_318 */

locret_EF70:                                 /* $EF70 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EF71  apu_reset - silence the APU and drop every voice
 *
 *  Original: src/bank7.asm:8743-8748 (bank 7)
 *      LDA #0 / STA byte_302 / STA byte_308
 *  (falls through into apu_stop)
 * ------------------------------------------------------------------------ */
void apu_reset(void)                         /* $EF71 */
{
    LDAI(0);                                 /* LDA #0 */
    STA(byte_302);                           /* STA byte_302 */
    STA(byte_308);                           /* STA byte_308 */
    apu_stop();                              /* falls through into apu_stop ($EF79) */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $EF79  apu_stop - stop all six voices and mute the APU
 *
 *  Original: src/bank7.asm:8754-8771 (bank 7)
 *      TXA / PHA / LDX #6 / loc_EF7D: LDA #0 / STA $319,X / DEX /
 *      BNE loc_EF7D / PLA / TAX / LDA #0 / STA APU_MASTERCTRL_REG /
 *      STA APU_DELTA_REG / RTS
 * ------------------------------------------------------------------------ */
void apu_stop(void)                          /* $EF79 */
{
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */
    LDXI(6);                                 /* LDX #6 */

loc_EF7D:                                    /* $EF7D */
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x0319));                       /* STA $319,X */
    DEX();                                   /* DEX */
    BNE(loc_EF7D);                           /* BNE loc_EF7D */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    LDAI(0);                                 /* LDA #0 */
    STA(APU_MASTERCTRL_REG);                 /* STA APU_MASTERCTRL_REG */
    STA(APU_DELTA_REG);                      /* STA APU_DELTA_REG */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EF90  apu_play - start sound number A (1...35)
 *
 *  Original: src/bank7.asm:8778-8794 (bank 7)
 *      INC byte_308 / STA byte_1E / TXA / PHA / TYA / PHA / LDA byte_1E /
 *      JSR sub_EFA6 / DEC byte_308 / PLA / TAY / PLA / TAX / RTS
 * ------------------------------------------------------------------------ */
void apu_play(void)                          /* $EF90 */
{
    INC(byte_308);                           /* INC byte_308 */
    STA(byte_1E);                            /* STA byte_1E */
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDA(byte_1E);                            /* LDA byte_1E */
    sub_EFA6();                              /* JSR sub_EFA6 */
    DEC(byte_308);                           /* DEC byte_308 */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $EFA6  sub_EFA6 - load song A: apu_sound_tab[A] -> $1E/$1F
 *
 *  Original: src/bank7.asm:8800-8873 (bank 7)
 *      STA byte_305 / ASL / TAX / LDA apu_sound_tab,X / STA byte_1E /
 *      LDA apu_sound_tab+1,X / STA byte_1F / LDY #0 / LDA ($1E),Y /
 *      BNE loc_EFC2 / LDA byte_1F / PHA / LDA byte_1E / PHA / RTS
 *      loc_EFC2: INY / STA apu_sound_length
 *      loc_EFC6: LDA ($1E),Y / INY / TAX / LDA ($1E),Y / BEQ loc_F012 /
 *      LDA $31A,X / AND #$40 / BNE locret_F01B / LDA byte_305 /
 *      STA $3C2,X / TXA / PHA / LDA #9 / STA byte_305
 *      loc_EFE2: LDA ($1E),Y / INY / STA $31A,X / TXA / CLC / ADC #6 / TAX /
 *      DEC byte_305 / BNE loc_EFE2 / PLA / TAX / LDA #0 / STA $350,X /
 *      STA $3EC,X / STA $356,X / STA $3CE,X / LDA $3C2,X / STA byte_305 /
 *      LDA #1 / STA $35C,X / STA $368,X / BNE loc_F016
 *      loc_F012: STA $31A,X / INY
 *      loc_F016: DEC apu_sound_length / BNE loc_EFC6
 *      locret_F01B: RTS
 *
 *  $EFB2-$EFB6 pushes byte_1E/byte_1F and RTSes into it: it is the original's
 *  "the song data is a handler" case (the RTS would land at byte_1E/1F + 1).
 *  The port dispatches through zanac_call_at(), the same way switch_case and
 *  sub_EB7A reach their computed addresses.  The path is never taken in this
 *  game: it needs the song data to start with a zero length byte, and all 36
 *  apu_sound_tab entries start with a non-zero one.
 * ------------------------------------------------------------------------ */
void sub_EFA6(void)                          /* $EFA6 */
{
    STA(byte_305);                           /* STA byte_305 */
    ASL_A();                                 /* ASL */
    TAX();                                   /* TAX */
    LDA(ABSX(apu_sound_tab));                /* LDA apu_sound_tab,X */
    STA(byte_1E);                            /* STA byte_1E */
    LDA(ABSX(apu_sound_tab + 1));            /* LDA apu_sound_tab+1,X */
    STA(byte_1F);                            /* STA byte_1F */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    BNE(loc_EFC2);                           /* BNE loc_EFC2 */
    /* $EFB2-$EFB6: LDA byte_1F / PHA / LDA byte_1E / PHA / RTS pushes the
     * pointer and RTSes into it (see the note above). */
    zanac_call_at((uint16_t)(MEM8(0x1E) | (MEM8(0x1F) << 8)));  /* RTS */
    RTS();

loc_EFC2:                                    /* $EFC2 */
    INY();                                   /* INY */
    STA(apu_sound_length);                   /* STA apu_sound_length */

loc_EFC6:                                    /* $EFC6 */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    INY();                                   /* INY */
    TAX();                                   /* TAX */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    BEQ(loc_F012);                           /* BEQ loc_F012 */
    LDA(ABSX(0x031A));                       /* LDA $31A,X */
    ANDI(0x40);                              /* AND #$40 */
    BNE(locret_F01B);                        /* BNE locret_F01B */
    LDA(byte_305);                           /* LDA byte_305 */
    STA(ABSX(0x03C2));                       /* STA $3C2,X */
    TXA();                                   /* TXA */
    PHA();                                   /* PHA */
    LDAI(9);                                 /* LDA #9 */
    STA(byte_305);                           /* STA byte_305 */

loc_EFE2:                                    /* $EFE2 */
    LDA(INDY(byte_1E));                      /* LDA ($1E),Y */
    INY();                                   /* INY */
    STA(ABSX(0x031A));                       /* STA $31A,X */
    TXA();                                   /* TXA */
    CLC();                                   /* CLC */
    ADCI(6);                                 /* ADC #6 */
    TAX();                                   /* TAX */
    DEC(byte_305);                           /* DEC byte_305 */
    BNE(loc_EFE2);                           /* BNE loc_EFE2 */
    PLA();                                   /* PLA */
    TAX();                                   /* TAX */
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x0350));                       /* STA $350,X */
    STA(ABSX(0x03EC));                       /* STA $3EC,X */
    STA(ABSX(0x0356));                       /* STA $356,X */
    STA(ABSX(0x03CE));                       /* STA $3CE,X */
    LDA(ABSX(0x03C2));                       /* LDA $3C2,X */
    STA(byte_305);                           /* STA byte_305 */
    LDAI(1);                                 /* LDA #1 */
    STA(ABSX(0x035C));                       /* STA $35C,X */
    STA(ABSX(0x0368));                       /* STA $368,X */
    BNE(loc_F016);                           /* BNE loc_F016 */

loc_F012:                                    /* $F012 */
    STA(ABSX(0x031A));                       /* STA $31A,X */
    INY();                                   /* INY */

loc_F016:                                    /* $F016 */
    DEC(apu_sound_length);                   /* DEC apu_sound_length */
    BNE(loc_EFC6);                           /* BNE loc_EFC6 */

locret_F01B:                                 /* $F01B */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $F0F4  sub_F0F4 - pause/stop the sound engine (byte_302 = 3)
 *
 *  Original: src/bank7.asm:8988-8995 (bank 7)
 *      LDA #3
 *      loc_F0F6: STA byte_302 / RTS
 * ------------------------------------------------------------------------ */
void sub_F0F4(void)                          /* $F0F4 */
{
    LDAI(3);                                 /* LDA #3 */
    /* $F0F6 loc_F0F6 - the shared tail; sub_F0FA branches into it (see
     * that procedure), so it stays a comment instead of an unused C label. */
    STA(byte_302);                           /* STA byte_302 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $F0FA  sub_F0FA - resume the sound engine (byte_302 = 1)
 *
 *  Original: src/bank7.asm:9001-9005 (bank 7)
 *      LDA #1 / BNE loc_F0F6
 *
 *  The branch is always taken (LDA #1 clears Z); loc_F0F6 is the shared tail
 *  inside sub_F0F4 and is reproduced here.
 * ------------------------------------------------------------------------ */
void sub_F0FA(void)                          /* $F0FA */
{
    LDAI(1);                                 /* LDA #1 */
    BNE(loc_F0F6);                           /* BNE loc_F0F6 (inside sub_F0F4) */

loc_F0F6:                                    /* $F0F6 - the shared tail inside sub_F0F4 */
    STA(byte_302);                           /* STA byte_302 */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $F146  sub_F146 - start the boss music envelope on channels 0-2
 *
 *  Original: src/bank7.asm:9048-9068 (bank 7)
 *      LDA #1 / STA byte_308 / LDX #2
 *      loc_F14D: LDA $350,X / ORA #$30 / STA $350,X / LDA #$10 /
 *      STA $3AA,X / LDA #8 / STA $3B6,X / LDA #0 / STA $3B0,X / DEX /
 *      BPL loc_F14D / STA byte_308 / RTS
 * ------------------------------------------------------------------------ */
void sub_F146(void)                          /* $F146 */
{
    LDAI(1);                                 /* LDA #1 */
    STA(byte_308);                           /* STA byte_308 */
    LDXI(2);                                 /* LDX #2 */

loc_F14D:                                    /* $F14D */
    LDA(ABSX(0x0350));                       /* LDA $350,X */
    ORAI(0x30);                              /* ORA #$30 */
    STA(ABSX(0x0350));                       /* STA $350,X */
    LDAI(0x10);                              /* LDA #$10 */
    STA(ABSX(0x03AA));                       /* STA $3AA,X */
    LDAI(8);                                 /* LDA #8 */
    STA(ABSX(0x03B6));                       /* STA $3B6,X */
    LDAI(0);                                 /* LDA #0 */
    STA(ABSX(0x03B0));                       /* STA $3B0,X */
    DEX();                                   /* DEX */
    BPL(loc_F14D);                           /* BPL loc_F14D */
    STA(byte_308);                           /* STA byte_308 (A is still 0) */
    RTS();                                   /* RTS */
}

/* ------------------------------------------------------------------------ *
 *  $FEE8  sub_FEE8 - wait for the boss music envelope to finish
 *
 *  Original: src/bank7.asm:9478-9508 (bank 7)
 *      LDA byte_31C / BEQ loc_FEF8 / LDA byte_3C4 / CMP #$1B / BEQ loc_FF09 /
 *      CMP #$1C / BEQ loc_FF09 /
 *      loc_FEF8: LDA byte_31D / BNE loc_FEFE / locret_FEFD: RTS /
 *      loc_FEFE: LDA byte_3C5 / CMP #8 / BEQ loc_FF09 / CMP #9 /
 *      BNE locret_FEFD /
 *      loc_FF09: LDY #1 / JSR sub_EA0B / JMP sub_FEE8
 *
 *  The trailing JMP is a self-loop, so it uses the entry label loc_FEE8 (the
 *  idiom of bank7/D2FD.c) instead of a recursive tail call.
 * ------------------------------------------------------------------------ */
void sub_FEE8(void)                          /* $FEE8 */
{
loc_FEE8:                                    /* $FEE8 (JMP sub_FEE8) */
    LDA(byte_31C);                           /* LDA byte_31C */
    BEQ(loc_FEF8);                           /* BEQ loc_FEF8 */
    LDA(byte_3C4);                           /* LDA byte_3C4 */
    CMPI(0x1B);                              /* CMP #$1B */
    BEQ(loc_FF09);                           /* BEQ loc_FF09 */
    CMPI(0x1C);                              /* CMP #$1C */
    BEQ(loc_FF09);                           /* BEQ loc_FF09 */

loc_FEF8:                                    /* $FEF8 */
    LDA(byte_31D);                           /* LDA byte_31D */
    BNE(loc_FEFE);                           /* BNE loc_FEFE */

locret_FEFD:                                 /* $FEFD */
    RTS();

loc_FEFE:                                    /* $FEFE */
    LDA(byte_3C5);                           /* LDA byte_3C5 */
    CMPI(8);                                 /* CMP #8 */
    BEQ(loc_FF09);                           /* BEQ loc_FF09 */
    CMPI(9);                                 /* CMP #9 */
    BNE(locret_FEFD);                        /* BNE locret_FEFD */

loc_FF09:                                    /* $FF09 */
    LDYI(1);                                 /* LDY #1 */
    sub_EA0B();                              /* JSR sub_EA0B */
    goto loc_FEE8;                           /* JMP sub_FEE8 */
}
