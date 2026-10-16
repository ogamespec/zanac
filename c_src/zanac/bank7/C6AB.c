/* bank7/C6AB.c - bank 7, $C6AB-$C930: the string/scroll helper family and the
 * bank switching code.
 *
 * Ported from src/bank7.asm lines 1029-1629 (addresses from Build/bank7.lst).
 *
 * Reference file for the rest of the port: it covers every addressing mode, the
 * chopped-procedure fall-through, a shared RTS, the inline parameter blocks
 * (print_string family), the computed jump (switch_case) and the bank register
 * writes.
 */
#include "zanac.h"

/* ------------------------------------------------------------------------ *
 *  $C6AB  sub_C6AB - clear the background scroll variables
 *
 *  Original: src/bank7.asm:1029-1036
 *      LDA #0
 *      STA byte_98 / byte_99 / byte_9A / byte_9B
 *  (no RTS: execution continues into sub_C6B5)
 * ------------------------------------------------------------------------ */
void sub_C6AB(void)                          /* $C6AB */
{
    LDAI(0x00);                              /* LDA #0      */
    STA(byte_98);                            /* STA byte_98 */
    STA(byte_99);                            /* STA byte_99 */
    STA(byte_9A);                            /* STA byte_9A */
    STA(byte_9B);                            /* STA byte_9B */
    sub_C6B5();                              /* falls through into $C6B5 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C6B5  sub_C6B5 - load the scroll registers from byte_98..byte_9B
 *
 *  Original: src/bank7.asm:1042-1056
 *      LDA byte_9A / STA ppu_scroll_1st_value
 *      LDA byte_98 / STA ppu_scroll_2nd_value
 *      LDA last_ppu_ctrl1 / AND #$FC / LSR / ORA byte_99 / ASL / ORA byte_9B
 *      STA last_ppu_ctrl1
 * ------------------------------------------------------------------------ */
void sub_C6B5(void)                          /* $C6B5 */
{
    LDA(byte_9A);                            /* LDA byte_9A */
    STA(ppu_scroll_1st_value);               /* STA ppu_scroll_1st_value */
    LDA(byte_98);                            /* LDA byte_98 */
    STA(ppu_scroll_2nd_value);               /* STA ppu_scroll_2nd_value */
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    ANDI(0xFC);                              /* AND #$FC */
    LSR_A();                                 /* LSR */
    ORA(byte_99);                            /* ORA byte_99 */
    ASL_A();                                 /* ASL */
    ORA(byte_9B);                            /* ORA byte_9B */
    STA(last_ppu_ctrl1);                     /* STA last_ppu_ctrl1 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C6CA  sub_C6CA - inline-parameter entry (not called anywhere)
 *
 *  Original: src/bank7.asm:1062-1068
 *      PLA / STA byte_16 / PLA / STA byte_17 / JSR sub_C6EA
 * ------------------------------------------------------------------------ */
void sub_C6CA(void)                          /* $C6CA */
{
    /* The original pops the JSR return address into $16/$17, which points at
     * the inline parameter block; the port takes it from zanac_inline_block. */
    A = ZANAC_LO(zanac_inline_block);        /* PLA */
    STA(byte_16);                            /* STA byte_16 */
    A = ZANAC_HI(zanac_inline_block);        /* PLA */
    STA(byte_17);                            /* STA byte_17 */
    sub_C6EA();                              /* JSR sub_C6EA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C6D3  stack_call_return - return to the caller's instruction stream
 *
 *  Original: src/bank7.asm:1075-1082
 *      LDA byte_17 / PHA / LDA byte_16 / PHA / RTS
 * ------------------------------------------------------------------------ */
void stack_call_return(void)                 /* $C6D3 */
{
    /* The original pushes $16/$17 and RTSes into the instruction stream right
     * after the caller's inline parameter block.  The C caller resumes by
     * itself, so the port performs no stack traffic (pushing here would
     * unbalance the emulated stack); the two stores document what it did:
     *      LDA byte_17 / PHA / LDA byte_16 / PHA / RTS                       */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C6DA  print_string - print the text of an inline parameter block
 *
 *  Original: src/bank7.asm:1089-1101
 *      PLA / STA byte_16 / PLA / STA byte_17
 *  loc_C6E0:
 *      JSR sub_C8BA / JSR sub_C6EA / BCS loc_C6E0 / BCC stack_call_return
 *
 *  Call convention (see PORTING.md section 4): the caller stores the address of
 *  the inline block - count, VRAM address, text - in zanac_inline_block.
 * ------------------------------------------------------------------------ */
void print_string(void)                      /* $C6DA */
{
    A = ZANAC_LO(zanac_inline_block);        /* PLA */
    STA(byte_16);                            /* STA byte_16 */
    A = ZANAC_HI(zanac_inline_block);        /* PLA */
    STA(byte_17);                            /* STA byte_17 */

loc_C6E0:                                    /* $C6E0 */
    sub_C8BA();                              /* JSR sub_C8BA */
    sub_C6EA();                              /* JSR sub_C6EA */
    BCS(loc_C6E0);                           /* BCS loc_C6E0 */
    /* BCC stack_call_return: always taken at this point */
    stack_call_return();                     /* JMP stack_call_return */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C6EA  sub_C6EA - print one line of an inline parameter block
 *
 *  Original: src/bank7.asm:1107-1151
 *      LDY #1 / LDA ($16),Y / BPL loc_C6F8 / ASL / BPL loc_C6F7 / LDA #8 /
 *      BNE loc_C6FB / loc_C6F7: LSR / loc_C6F8: CLC / ADC #3 /
 *      loc_C6FB: JSR sub_C7D8 / BCS locret_C724 / STX byte_10 / STA byte_11 /
 *      JSR sub_C7F8 / TAX / loop: LDA ($16),Y / STA $460,X / INY / INX /
 *      DEC byte_11 / BNE loop / JSR sub_C807 / LDX byte_10 / DEY / TYA /
 *      ADC byte_16 / STA byte_16 / LDA byte_17 / ADC #0 / STA byte_17 /
 *  locret_C724: RTS
 * ------------------------------------------------------------------------ */
void sub_C6EA(void)                          /* $C6EA */
{
    LDYI(1);                                 /* LDY #1 */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    BPL(loc_C6F8);                           /* BPL loc_C6F8 */
    ASL_A();                                 /* ASL */
    BPL(loc_C6F7);                           /* BPL loc_C6F7 */
    LDAI(8);                                 /* LDA #8 */
    BNE(loc_C6FB);                           /* BNE loc_C6FB */

loc_C6F7:                                    /* $C6F7 */
    LSR_A();                                 /* LSR */

loc_C6F8:                                    /* $C6F8 */
    CLC();                                   /* CLC */
    ADCI(3);                                 /* ADC #3 */

loc_C6FB:                                    /* $C6FB */
    sub_C7D8();                              /* JSR sub_C7D8 */
    BCS(locret_C724);                        /* BCS locret_C724 */
    STX(byte_10);                            /* STX byte_10 */
    STA(byte_11);                            /* STA byte_11 */
    sub_C7F8();                              /* JSR sub_C7F8 */
    TAX();                                   /* TAX */

loc_C708:                                    /* $C708 */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    STA(ABSX(0x0460));                       /* STA $460,X */
    INY();                                   /* INY */
    INX();                                   /* INX */
    DEC(byte_11);                            /* DEC byte_11 */
    BNE(loc_C708);                           /* BNE loc_C708 */
    sub_C807();                              /* JSR sub_C807 */
    LDX(byte_10);                            /* LDX byte_10 */
    DEY();                                   /* DEY */
    TYA();                                   /* TYA */
    ADC(byte_16);                            /* ADC byte_16 */
    STA(byte_16);                            /* STA byte_16 */
    LDA(byte_17);                            /* LDA byte_17 */
    ADCI(0x00);                              /* ADC #0 */
    STA(byte_17);                            /* STA byte_17 */

locret_C724:                                 /* $C724 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C725  sub_C725 - inline-parameter entry (not called anywhere)
 *
 *  Original: src/bank7.asm:1157-1195
 *      PLA / STA byte_16 / PLA / STA byte_17 / LDY #1 / LDA ($16),Y /
 *      CLC / ADC #3 / JSR sub_C7EB / JSR sub_C7F8 / STX byte_11 / TAX /
 *      LDA ($16),Y / STA byte_10 / STA $460,X / LDA byte_19 / STA $461,X /
 *      LDA byte_18 / STA $462,X / TYA / SEC / ADC byte_16 / STA byte_16 /
 *      BCC loc_C756 / INC byte_17 / loc_C756: LDY #0 / LDA ($16),Y /
 *      loc_C75A: STA $463,X / INX / DEC byte_10 / BNE loc_C75A /
 *      JSR sub_C807 / JMP stack_call_return
 * ------------------------------------------------------------------------ */
void sub_C725(void)                          /* $C725 */
{
    A = ZANAC_LO(zanac_inline_block);        /* PLA */
    STA(byte_16);                            /* STA byte_16 */
    A = ZANAC_HI(zanac_inline_block);        /* PLA */
    STA(byte_17);                            /* STA byte_17 */
    LDYI(1);                                 /* LDY #1 */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    CLC();                                   /* CLC */
    ADCI(3);                                 /* ADC #3 */
    sub_C7EB();                              /* JSR sub_C7EB */
    sub_C7F8();                              /* JSR sub_C7F8 */
    STX(byte_11);                            /* STX byte_11 */
    TAX();                                   /* TAX */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */
    STA(byte_10);                            /* STA byte_10 */
    STA(ABSX(0x0460));                       /* STA $460,X */
    LDA(byte_19);                            /* LDA byte_19 */
    STA(ABSX(0x0461));                       /* STA $461,X */
    LDA(byte_18);                            /* LDA byte_18 */
    STA(ABSX(0x0462));                       /* STA $462,X */
    TYA();                                   /* TYA */
    SEC();                                   /* SEC */
    ADC(byte_16);                            /* ADC byte_16 */
    STA(byte_16);                            /* STA byte_16 */
    BCC(loc_C756);                           /* BCC loc_C756 */
    INC(byte_17);                            /* INC byte_17 */

loc_C756:                                    /* $C756 */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_16));                      /* LDA ($16),Y */

loc_C75A:                                    /* $C75A */
    STA(ABSX(0x0463));                       /* STA $463,X */
    INX();                                   /* INX */
    DEC(byte_10);                            /* DEC byte_10 */
    BNE(loc_C75A);                           /* BNE loc_C75A */
    sub_C807();                              /* JSR sub_C807 */
    stack_call_return();                     /* JMP stack_call_return */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C768  clear_screen - fill the name table with X (and $0300 with Y)
 *
 *  Original: src/bank7.asm:1202-1241
 *      STA PPU_ADDRESS / STA byte_2 / LDA #0 / STA PPU_ADDRESS / STA temp_0 /
 *      LDA byte_2 / CMP #$20 / BCS loc_C77E / STY temp_1 / BCC loc_C786 /
 *      loc_C77E: LDA #$C0 / STA temp_0 / LDA #4 / STA temp_1 /
 *      loc_C786: STX PPU_DATA / DEC temp_0 / BNE loc_C786 / DEC temp_1 /
 *      BNE loc_C786 / LDA byte_2 / CMP #$20 / BCC locret_C7A2 /
 *      LDA #$40 / STA temp_0 /
 *      loc_C79B: STY PPU_DATA / DEC temp_0 / BNE loc_C79B / RTS
 * ------------------------------------------------------------------------ */
void clear_screen(void)                      /* $C768 */
{
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    STA(byte_2);                             /* STA byte_2 */
    LDAI(0x00);                              /* LDA #0 */
    STA(PPU_ADDRESS);                        /* STA PPU_ADDRESS */
    STA(temp_0);                             /* STA temp_0 */
    LDA(byte_2);                             /* LDA byte_2 */
    CMPI(0x20);                              /* CMP #$20 */
    BCS(loc_C77E);                           /* BCS loc_C77E */
    STY(temp_1);                             /* STY temp_1 */
    BCC(loc_C786);                           /* BCC loc_C786 */

loc_C77E:                                    /* $C77E */
    LDAI(0xC0);                              /* LDA #$C0 */
    STA(temp_0);                             /* STA temp_0 */
    LDAI(4);                                 /* LDA #4 */
    STA(temp_1);                             /* STA temp_1 */

loc_C786:                                    /* $C786 */
    STX(PPU_DATA);                           /* STX PPU_DATA */
    DEC(temp_0);                             /* DEC temp_0 */
    BNE(loc_C786);                           /* BNE loc_C786 */
    DEC(temp_1);                             /* DEC temp_1 */
    BNE(loc_C786);                           /* BNE loc_C786 */
    LDA(byte_2);                             /* LDA byte_2 */
    CMPI(0x20);                              /* CMP #$20 */
    BCC(locret_C7A2);                        /* BCC locret_C7A2 */
    LDAI(0x40);                              /* LDA #$40 */
    STA(temp_0);                             /* STA temp_0 */

loc_C79B:                                    /* $C79B */
    STY(PPU_DATA);                           /* STY PPU_DATA */
    DEC(temp_0);                             /* DEC temp_0 */
    BNE(loc_C79B);                           /* BNE loc_C79B */

locret_C7A2:                                 /* $C7A2 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C7A3  ppu_update_scroll_regs - restore $2000/$2005 from the saved values
 *
 *  Original: src/bank7.asm:1247-1256
 * ------------------------------------------------------------------------ */
void ppu_update_scroll_regs(void)            /* $C7A3 */
{
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */
    LDA(ppu_scroll_1st_value);               /* LDA ppu_scroll_1st_value */
    STA(PPU_SCROLL_REG);                     /* STA PPU_SCROLL_REG */
    LDA(ppu_scroll_2nd_value);               /* LDA ppu_scroll_2nd_value */
    STA(PPU_SCROLL_REG);                     /* STA PPU_SCROLL_REG */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C7B3  mem_page_fill - fill memory from page X to page Y with A
 *
 *  Original: src/bank7.asm:1263-1278
 *      STX temp_1 / LDX #0 / STX temp_0 /
 *      loc_C7B9: STA (temp_0,X) / INC temp_0 / BNE loc_C7B9 /
 *      INC temp_1 / CPY temp_1 / BCS loc_C7B9 / RTS
 * ------------------------------------------------------------------------ */
void mem_page_fill(void)                     /* $C7B3 */
{
    STX(temp_1);                             /* STX temp_1 */
    LDXI(0);                                 /* LDX #0 */
    STX(temp_0);                             /* STX temp_0 */

loc_C7B9:                                    /* $C7B9 */
    STA(INDX(temp_0));                       /* STA (temp_0,X) */
    INC(temp_0);                             /* INC temp_0 */
    BNE(loc_C7B9);                           /* BNE loc_C7B9 */
    INC(temp_1);                             /* INC temp_1 */
    CPY(temp_1);                             /* CPY temp_1 */
    BCS(loc_C7B9);                           /* BCS loc_C7B9 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C7C6  switch_case - old-school switch: a table of words follows the JSR
 *
 *  Original: src/bank7.asm:1285-1301
 *      ASL / TAY / PLA / STA temp_0 / PLA / STA temp_1 / INY / INY /
 *      LDA (temp_0),Y / PHA / DEY / LDA (temp_0),Y / PHA / RTS
 *
 *  The table holds (entry - 1) because the final RTS adds one to the address
 *  it pops.  The port takes the table address from zanac_inline_table (set by
 *  the SWITCH_CASE macro at the call site) and dispatches through
 *  zanac_rts_dispatch(), which pops the pushed word like the RTS did.
 * ------------------------------------------------------------------------ */
void switch_case(void)                       /* $C7C6 */
{
    ASL_A();                                 /* ASL */
    TAY();                                   /* TAY */
    /* PLA / STA temp_0 / PLA / STA temp_1: the two PLAs take the return address
     * the JSR pushed, which is the address of its own *last* byte - that is,
     * the inline table minus one.  The INY/INY below then reaches the high byte
     * of the entry and the DEY its low byte.  (The SWITCH_CASE macro names the
     * table itself, so the -1 is applied here; PORTING.md section 4b.) */
    {
        uint16_t base = (uint16_t)(zanac_inline_table - 1);
        A = ZANAC_LO(base);                  /* PLA */
        STA(temp_0);                         /* STA temp_0 */
        A = ZANAC_HI(base);                  /* PLA */
        STA(temp_1);                         /* STA temp_1 */
    }
    INY();                                   /* INY */
    INY();                                   /* INY */
    LDA(INDY(temp_0));                       /* LDA (temp_0),Y */
    PHA();                                   /* PHA */
    DEY();                                   /* DEY */
    LDA(INDY(temp_0));                       /* LDA (temp_0),Y */
    PHA();                                   /* PHA */
    zanac_rts_dispatch();                    /* RTS to the computed address */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C7D8  sub_C7D8 - is the VRAM address in the visible window?
 *
 *  Original: src/bank7.asm:1307-1326
 *      PHA / CLC / ADC byte_21 / CMP #$51 / BCS loc_C7E9 / CMP #$11 /
 *      BCC loc_C7E9 / LDA byte_9C / BNE loc_C7E9 / CLC /
 *      loc_C7E9: PLA / locret_C7EA: RTS
 *
 *  Returns with carry set when the address (A + byte_21) is inside the window
 *  and byte_9C is zero.
 * ------------------------------------------------------------------------ */
void sub_C7D8(void)                          /* $C7D8 */
{
    PHA();                                   /* PHA */
    CLC();                                   /* CLC */
    ADC(byte_21);                            /* ADC byte_21 */
    CMPI(0x51);                              /* CMP #$51 */
    BCS(loc_C7E9);                           /* BCS loc_C7E9 */
    CMPI(0x11);                              /* CMP #$11 */
    BCC(loc_C7E9);                           /* BCC loc_C7E9 */
    LDA(byte_9C);                            /* LDA byte_9C */
    BNE(loc_C7E9);                           /* BNE loc_C7E9 */
    CLC();                                   /* CLC */

loc_C7E9:                                    /* $C7E9 */
    PLA();                                   /* PLA */
    /* locret_C7EA ($C7EA) is the shared RTS; sub_C7EB branches into it. */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C7EB  sub_C7EB - wait until the VRAM address becomes visible
 *
 *  Original: src/bank7.asm:1332-1340
 *      JSR sub_C7D8 / BCC locret_C7EA / PHA / JSR sub_C8BA / PLA / JMP sub_C7EB
 * ------------------------------------------------------------------------ */
void sub_C7EB(void)                          /* $C7EB */
{
loc_C7EB:                                    /* $C7EB */
    sub_C7D8();                              /* JSR sub_C7D8 */
    BCC(locret_C7EA);                        /* BCC locret_C7EA (shared RTS) */
    PHA();                                   /* PHA */
    sub_C8BA();                              /* JSR sub_C8BA */
    PLA();                                   /* PLA */
    goto loc_C7EB;                           /* JMP sub_C7EB */

locret_C7EA:                                 /* $C7EA - the RTS inside sub_C7D8 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C7F8  sub_C7F8 - start a VRAM write of byte_21 bytes
 *
 *  Original: src/bank7.asm:1346-1357
 *      STA byte_25 / LDA byte_21 / PHA / LDA byte_20 / STA byte_24 /
 *      LDA #0 / STA byte_20 / PLA / RTS
 * ------------------------------------------------------------------------ */
void sub_C7F8(void)                          /* $C7F8 */
{
    STA(byte_25);                            /* STA byte_25 */
    LDA(byte_21);                            /* LDA byte_21 */
    PHA();                                   /* PHA */
    LDA(byte_20);                            /* LDA byte_20 */
    STA(byte_24);                            /* STA byte_24 */
    LDAI(0x00);                              /* LDA #0 */
    STA(byte_20);                            /* STA byte_20 */
    PLA();                                   /* PLA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C807  sub_C807 - finish a VRAM write, advancing the position
 *
 *  Original: src/bank7.asm:1363-1373
 *      LDA byte_25 / CLC / ADC byte_21 / STA byte_21 / LDA byte_24 /
 *      ADC #1 / STA byte_20 / RTS
 * ------------------------------------------------------------------------ */
void sub_C807(void)                          /* $C807 */
{
    LDA(byte_25);                            /* LDA byte_25 */
    CLC();                                   /* CLC */
    ADC(byte_21);                            /* ADC byte_21 */
    STA(byte_21);                            /* STA byte_21 */
    LDA(byte_24);                            /* LDA byte_24 */
    ADCI(1);                                 /* ADC #1 */
    STA(byte_20);                            /* STA byte_20 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C815  switch_bank - select the bank visible at $8000-$BFFF
 *
 *  Original: src/bank7.asm:1380-1391
 *      STA last_bank_num / TYA / PHA / LDY last_bank_num /
 *      LDA bank_remap_table,Y / STA bank_remap_table,Y / PLA / TAY / RTS
 *
 *  The store to bank_remap_table,Y is the UxROM bank register write: the
 *  address is inside the fixed bank and the value is the bank number, which
 *  the identity table at $C0F8 lets through the board's bus conflicts.  In the
 *  port a write to $8000-$FFFF simply selects the bank array (zanac_ram.c).
 * ------------------------------------------------------------------------ */
void switch_bank(void)                       /* $C815 */
{
    STA(last_bank_num);                      /* STA last_bank_num */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDY(last_bank_num);                      /* LDY last_bank_num */
    LDA(ABSY(bank_remap_table));             /* LDA bank_remap_table,Y */
    MEMW(ABSY(bank_remap_table), A);         /* STA bank_remap_table,Y (bank register) */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C824-$C872  switch_bank_0_unused .. switch_bank_6 - fixed-bank variants
 *
 *  Original: src/bank7.asm:1397-1491; each is
 *      PHA / LDA #N / STA last_bank_num / LDA bank_remap_table+N /
 *      STA bank_remap_table+N / PLA / RTS
 * ------------------------------------------------------------------------ */
void switch_bank_0_unused(void)              /* $C824 */
{
    PHA();                                   /* PHA */
    LDAI(0);                                 /* LDA #0 */
    STA(last_bank_num);                      /* STA last_bank_num */
    LDA(bank_remap_table + 0);               /* LDA bank_remap_table+0 */
    MEMW(bank_remap_table + 0, A);           /* STA bank_remap_table+0 */
    PLA();                                   /* PLA */
    RTS();
}

void switch_bank_1_unused(void)              /* $C831 */
{
    PHA();                                   /* PHA */
    LDAI(1);                                 /* LDA #1 */
    STA(last_bank_num);                      /* STA last_bank_num */
    LDA(bank_remap_table + 1);               /* LDA bank_remap_table+1 */
    MEMW(bank_remap_table + 1, A);           /* STA bank_remap_table+1 */
    PLA();                                   /* PLA */
    RTS();
}

void switch_bank_2_unused(void)              /* $C83E */
{
    PHA();                                   /* PHA */
    LDAI(2);                                 /* LDA #2 */
    STA(last_bank_num);                      /* STA last_bank_num */
    LDA(bank_remap_table + 2);               /* LDA bank_remap_table+2 */
    MEMW(bank_remap_table + 2, A);           /* STA bank_remap_table+2 */
    PLA();                                   /* PLA */
    RTS();
}

void switch_bank_3_unused(void)              /* $C84B */
{
    PHA();                                   /* PHA */
    LDAI(3);                                 /* LDA #3 */
    STA(last_bank_num);                      /* STA last_bank_num */
    LDA(bank_remap_table + 3);               /* LDA bank_remap_table+3 */
    MEMW(bank_remap_table + 3, A);           /* STA bank_remap_table+3 */
    PLA();                                   /* PLA */
    RTS();
}

void switch_bank_4(void)                     /* $C858 */
{
    PHA();                                   /* PHA */
    LDAI(4);                                 /* LDA #4 */
    STA(last_bank_num);                      /* STA last_bank_num */
    LDA(bank_remap_table + 4);               /* LDA bank_remap_table+4 */
    MEMW(bank_remap_table + 4, A);           /* STA bank_remap_table+4 */
    PLA();                                   /* PLA */
    RTS();
}

void switch_bank_5(void)                     /* $C865 */
{
    PHA();                                   /* PHA */
    LDAI(5);                                 /* LDA #5 */
    STA(last_bank_num);                      /* STA last_bank_num */
    LDA(bank_remap_table + 5);               /* LDA bank_remap_table+5 */
    MEMW(bank_remap_table + 5, A);           /* STA bank_remap_table+5 */
    PLA();                                   /* PLA */
    RTS();
}

void switch_bank_6(void)                     /* $C872 */
{
    PHA();                                   /* PHA */
    LDAI(6);                                 /* LDA #6 */
    STA(last_bank_num);                      /* STA last_bank_num */
    LDA(bank_remap_table + 6);               /* LDA bank_remap_table+6 */
    MEMW(bank_remap_table + 6, A);           /* STA bank_remap_table+6 */
    PLA();                                   /* PLA */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C87F  far_call - bank-relative call (not used by the game)
 *
 *  Original: src/bank7.asm:1498-1519
 *      STA byte_10 / STY byte_11 / PLA / STA byte_12 / CLC / ADC #3 / TAY /
 *      PLA / STA byte_13 / ADC #0 / PHA / TYA / PHA / LDA last_bank_num /
 *      PHA / JSR far_call_prepare / STA byte_10 / STY byte_11 / PLA /
 *      JMP far_call_restore
 *
 *  Nothing in the game reaches far_call (only the unused thunk $C006 targets
 *  it).  The first two PLAs pop the return address of the JMP that entered the
 *  routine, which points at the three inline bytes (bank, target low, target
 *  high); a C call has no return address in the 6502 stack, so the port reads
 *  that block through zanac_inline_block, exactly like the print_string
 *  family.  The rest of the routine, including the three pushes and the
 *  far_call_restore tail, is translated literally.
 * ------------------------------------------------------------------------ */
void far_call(void)                          /* $C87F */
{
    STA(byte_10);                            /* STA byte_10 */
    STY(byte_11);                            /* STY byte_11 */
    A = ZANAC_LO(zanac_inline_block);        /* PLA */
    STA(byte_12);                            /* STA byte_12 */
    CLC();                                   /* CLC */
    ADCI(3);                                 /* ADC #3 */
    TAY();                                   /* TAY */
    A = ZANAC_HI(zanac_inline_block);        /* PLA */
    STA(byte_13);                            /* STA byte_13 */
    ADCI(0);                                 /* ADC #0 */
    PHA();                                   /* PHA */
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */
    LDA(last_bank_num);                      /* LDA last_bank_num */
    PHA();                                   /* PHA */
    far_call_prepare();                      /* JSR far_call_prepare */
    STA(byte_10);                            /* STA byte_10 */
    STY(byte_11);                            /* STY byte_11 */
    PLA();                                   /* PLA */
    far_call_restore();                      /* JMP far_call_restore */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C8A0  far_call_prepare - push the three inline bytes of the target
 *
 *  Original: src/bank7.asm:1525-1534
 *      LDY #3 / LDA ($12),Y / PHA / DEY / LDA ($12),Y / PHA / DEY /
 *      LDA ($12),Y
 * ------------------------------------------------------------------------ */
void far_call_prepare(void)                  /* $C8A0 */
{
    LDYI(3);                                 /* LDY #3 */
    LDA(INDY(byte_12));                      /* LDA ($12),Y */
    PHA();                                   /* PHA */
    DEY();                                   /* DEY */
    LDA(INDY(byte_12));                      /* LDA ($12),Y */
    PHA();                                   /* PHA */
    DEY();                                   /* DEY */
    LDA(INDY(byte_12));                      /* LDA ($12),Y */
    far_call_restore();                      /* falls through into $C8AC */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C8AC  far_call_restore - activate the target bank and return to it
 *
 *  Original: src/bank7.asm:1540-1548
 *      STA last_bank_num / TAY / LDA bank_remap_table,Y /
 *      STA bank_remap_table,Y / LDA byte_10 / LDY byte_11 / RTS
 * ------------------------------------------------------------------------ */
void far_call_restore(void)                  /* $C8AC */
{
    STA(last_bank_num);                      /* STA last_bank_num */
    TAY();                                   /* TAY */
    LDA(ABSY(bank_remap_table));             /* LDA bank_remap_table,Y */
    MEMW(ABSY(bank_remap_table), A);         /* STA bank_remap_table,Y */
    LDA(byte_10);                            /* LDA byte_10 */
    LDY(byte_11);                            /* LDY byte_11 */
    RTS();
}

/* ------------------------------------------------------------------------ *
 *  $C8BA  sub_C8BA - loop while a name-table update is pending
 *
 *  Original: src/bank7.asm:1554-1579
 *      TYA / PHA / loc_C8BC: JSR sub_CD6B / LDA last_ppu_ctrl2 / AND #$18 /
 *      BNE loc_C8D4 / LDA last_ppu_ctrl1 / AND #$7F / STA PPU_CTRL_REG1 /
 *      JSR ppu_update_background / LDA last_ppu_ctrl1 / STA PPU_CTRL_REG1 /
 *      loc_C8D4: LDA byte_2B / ORA byte_9C / ORA byte_20 / BNE loc_C8BC /
 *      PLA / TAY / RTS
 * ------------------------------------------------------------------------ */
void sub_C8BA(void)                          /* $C8BA */
{
    TYA();                                   /* TYA */
    PHA();                                   /* PHA */

loc_C8BC:                                    /* $C8BC */
    sub_CD6B();                              /* JSR sub_CD6B */
    LDA(last_ppu_ctrl2);                     /* LDA last_ppu_ctrl2 */
    ANDI(0x18);                              /* AND #$18 (BG or OBJ enabled?) */
    BNE(loc_C8D4);                           /* BNE loc_C8D4 */
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 */
    ANDI(0x7F);                              /* AND #$7F (disable VBlank) */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */
    ppu_update_background();                 /* JSR ppu_update_background */
    LDA(last_ppu_ctrl1);                     /* LDA last_ppu_ctrl1 (restore) */
    STA(PPU_CTRL_REG1);                      /* STA PPU_CTRL_REG1 */

loc_C8D4:                                    /* $C8D4 */
    LDA(byte_2B);                            /* LDA byte_2B */
    ORA(byte_9C);                            /* ORA byte_9C */
    ORA(byte_20);                            /* ORA byte_20 */
    BNE(loc_C8BC_wait);                      /* BNE loc_C8BC */
    PLA();                                   /* PLA */
    TAY();                                   /* TAY */
    RTS();

loc_C8BC_wait:                               /* back edge of the loc_C8BC loop */
    /* When rendering is enabled the block above skips ppu_update_background,
     * so the buffers this loop waits for are drained by the NMI handler - the
     * spinning 6502 was waiting for it.  The port lets the engine run a frame,
     * which is exactly one NMI, and retries. */
    ZANAC_WAIT_NMI();
    goto loc_C8BC;
}

/* ------------------------------------------------------------------------ *
 *  $C8DF  sub_C8DF - switch to a bank, read a pointer from its $8030 table,
 *                    then switch back
 *
 *  Original: src/bank7.asm:1585-1629 (see the listing for the stack traffic:
 *  the routine pushes last_bank_num, the two halves of the table address and
 *  the Y argument, then pops them back to select and restore the bank).
 * ------------------------------------------------------------------------ */
void sub_C8DF(void)                          /* $C8DF */
{
    STA(byte_15);                            /* STA byte_15 */
    STY(byte_14);                            /* STY byte_14 */
    TYA();                                   /* TYA */
    ANDI(0x0F);                              /* AND #$F */
    TAY();                                   /* TAY */
    LDA(last_bank_num);                      /* LDA last_bank_num */
    PHA();                                   /* PHA (remember the caller's bank) */
    LDA(byte_15);                            /* LDA byte_15 */
    ASL_A();                                 /* ASL */
    ADC(ABSY(unk_C928));                     /* ADC unk_C928,Y */
    PHA();                                   /* PHA (target address low) */
    LDA(ABSY(unk_C930));                     /* LDA unk_C930,Y */
    ADCI(0x00);                              /* ADC #0 */
    PHA();                                   /* PHA (target address high) */
    LDA(byte_14);                            /* LDA byte_14 */
    PHA();                                   /* PHA (the bank argument) */
    LDA(byte_2B);                            /* LDA byte_2B */
    BEQ(loc_C901);                           /* BEQ loc_C901 */
    sub_C8BA();                              /* JSR sub_C8BA */

loc_C901:                                    /* $C901 */
    PLA();                                   /* PLA */
    STA(byte_26);                            /* STA byte_26 (the bank to select) */
    switch_bank();                           /* JSR switch_bank (A = bank) */
    PLA();                                   /* PLA */
    STA(byte_15);                            /* STA byte_15 (target address low) */
    PLA();                                   /* PLA */
    STA(byte_14);                            /* STA byte_14 (target address high) */
    LDYI(0);                                 /* LDY #0 */
    LDA(INDY(byte_14));                      /* LDA ($14),Y */
    STA(byte_2C);                            /* STA byte_2C */
    INY();                                   /* INY */
    LDA(INDY(byte_14));                      /* LDA ($14),Y */
    STA(byte_2D);                            /* STA byte_2D */
    LDA(byte_18);                            /* LDA byte_18 */
    STA(byte_2E);                            /* STA byte_2E */
    LDA(byte_19);                            /* LDA byte_19 */
    STA(byte_2F);                            /* STA byte_2F */
    LDAI(2);                                 /* LDA #2 */
    STA(byte_2B);                            /* STA byte_2B */
    PLA();                                   /* PLA (the caller's bank) */
    switch_bank();                           /* JMP switch_bank (restore) */
    RTS();
}
