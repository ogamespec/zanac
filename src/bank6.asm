    processor 6502
	org $8000,255

    include "../src/zp.asm"
    include "../src/bank7_api.asm"

; Segment type: Pure code
                ;.segment seg002

; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_18000:                              ; CODE XREF: sub_D0BF+32↑P
                                        ; sub_EA0B:loc_EA25↑P
                JMP     sub_187C7
; End of function sub_18000


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_18003:                              ; CODE XREF: sub_DFBF+CC↑P
                                        ; reset+95↑P
                JMP     sub_18835
; End of function sub_18003


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_18006:                              ; CODE XREF: reset+68↑P
                JMP     sub_183DD
; End of function sub_18006


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_18009:                              ; CODE XREF: sub_DFBF+C9↑P
                                        ; reset+92↑P
                JMP     sub_183EF
; End of function sub_18009


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_1800C:                              ; CODE XREF: sub_DFBF+CF↑P
                                        ; reset+98↑P
                JMP     sub_184A7
; End of function sub_1800C


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_1800F:                              ; CODE XREF: sub_CB4B+C3↑P
                JMP     sub_19493
; End of function sub_1800F


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_18012:                              ; CODE XREF: sub_CB4B+7F↑P
                JMP     sub_1948B
; End of function sub_18012


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_set_default_weapon:                   ; CODE XREF: sub_CB4B+82↑P
                JMP     set_default_weapon ; Takes three numbers from the array (addressed by the variable $33) and places them in $37,$35,$36
; End of function j_set_default_weapon


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_18018:                              ; CODE XREF: reset+6B↑P
                JMP     sub_1852A
; End of function sub_18018


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_1801B:                              ; CODE XREF: sub_D3F4+3↑P
                                        ; reset+6E↑P
                JMP     sub_1853B
; End of function sub_1801B


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_1801E:                              ; CODE XREF: reset+C4↑P
                JMP     sub_1B8E5
; End of function sub_1801E


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_check_enemy_overflow:                 ; CODE XREF: sub_D463↑P
                                        ; sub_D472↑P ...
                JMP     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
; End of function j_check_enemy_overflow


; =============== S U B R O U T I N E =======================================


sub_18024:                              ; CODE XREF: sub_19AF7+38↓p
                                        ; sub_1A05E+C↓p ...
                JSR     sub_1802A
                JMP     sub_18097
; End of function sub_18024


; =============== S U B R O U T I N E =======================================


sub_1802A:                              ; CODE XREF: sub_18024↑p
                                        ; sub_19DAF+8↓p ...
                STY     $78
                LDA     #0
                STA     $44
                LDA     $542
                SEC
                SBC     $542,X
                STA     $42
                BCS     loc_18049
                LDA     $44
                ORA     #4
                STA     $44
                LDA     $42
                EOR     #$FF
                ADC     #1
                STA     $42

loc_18049:                              ; CODE XREF: sub_1802A+F↑j
                BNE     loc_1804D
                INC     $42

loc_1804D:                              ; CODE XREF: sub_1802A:loc_18049↑j
                LDA     $55C
                SEC
                SBC     $55C,X
                STA     $43
                BCS     loc_18066
                LDA     $44
                ORA     #8
                STA     $44
                LDA     $43
                EOR     #$FF
                ADC     #1
                STA     $43

loc_18066:                              ; CODE XREF: sub_1802A+2C↑j
                BNE     loc_1806A
                INC     $43

loc_1806A:                              ; CODE XREF: sub_1802A:loc_18066↑j
                LDA     $43
                CMP     $42
                BCC     loc_1807C
                LDY     $42
                STY     $43
                STA     $42
                LDA     $44
                ORA     #$10
                STA     $44

loc_1807C:                              ; CODE XREF: sub_1802A+44↑j
                LDY     $43
                LDA     $42
                JSR     sub_1818C
                LDY     #3

loc_18085:                              ; CODE XREF: sub_1802A+61↓j
                CMP     $8120,Y
                BCC     loc_1808D
                DEY
                BNE     loc_18085

loc_1808D:                              ; CODE XREF: sub_1802A+5E↑j
                TYA
                ORA     $44
                TAY
                LDA     $8124,Y
                LDY     $78
                RTS
; End of function sub_1802A


; =============== S U B R O U T I N E =======================================


sub_18097:                              ; CODE XREF: sub_18024+3↑j
                                        ; sub_18CA4+4B↓p ...
                AND     #$F
                STY     $65
                ASL
                ASL
                TAY
                LDA     $8144,Y
                STA     $12
                LDA     $8145,Y
                STA     $10
                LDA     $8146,Y
                STA     $13
                LDA     $8147,Y
                STA     $11
                LDA     $74A,X
                BPL     loc_180CF
                ASL     $12
                ROL     $10
                ASL     $12
                ROL     $10
                ASL     $12
                ROL     $10
                ASL     $13
                ROL     $11
                ASL     $13
                ROL     $11
                ASL     $13
                ROL     $11

loc_180CF:                              ; CODE XREF: sub_18097+1E↑j
                AND     #$40 ; '@'
                BEQ     loc_180E3
                ASL     $12
                ROL     $10
                ASL     $12
                ROL     $10
                ASL     $13
                ROL     $11
                ASL     $13
                ROL     $11

loc_180E3:                              ; CODE XREF: sub_18097+3A↑j
                LDA     $74A,X
                AND     #$3F ; '?'
                BEQ     loc_1811E
                TAY
                LDA     #0
                STA     $5F8,X
                STA     $612,X
                STA     $62C,X
                STA     $646,X
; End of function sub_18097


; =============== S U B R O U T I N E =======================================


sub_180F9:                              ; CODE XREF: sub_180F9+23↓j
                LDA     $12
                CLC
                ADC     $5F8,X
                STA     $5F8,X
                LDA     $10
                ADC     $612,X
                STA     $612,X
                LDA     $13
                CLC
                ADC     $62C,X
                STA     $62C,X
                LDA     $11
                ADC     $646,X
                STA     $646,X
                DEY
                BNE     sub_180F9

loc_1811E:                              ; CODE XREF: sub_18097+51↑j
                LDY     $65
                RTS
; End of function sub_180F9

; ---------------------------------------------------------------------------
                .BYTE $AB
                .BYTE $6A ; j
                .BYTE $32 ; 2
                .BYTE   2
                .BYTE   3
                .BYTE   3
                .BYTE   4
                .BYTE  $E
                .BYTE  $D
                .BYTE  $D
                .BYTE  $C
                .BYTE   6
                .BYTE   5
                .BYTE   5
                .BYTE   4
                .BYTE  $A
                .BYTE  $B
                .BYTE  $B
                .BYTE  $C
                .BYTE   2
                .BYTE   1
                .BYTE   1
                .BYTE   0
                .BYTE  $E
                .BYTE  $F
                .BYTE  $F
                .BYTE   0
                .BYTE   6
                .BYTE   7
                .BYTE   7
                .BYTE   8
                .BYTE  $A
                .BYTE   9
                .BYTE   9
                .BYTE   8
                .BYTE   0
                .BYTE   0
                .BYTE $78 ; x
                .BYTE   0
                .BYTE $2D ; -
                .BYTE   0
                .BYTE $6E ; n
                .BYTE   0
                .BYTE $54 ; T
                .BYTE   0
                .BYTE $54 ; T
                .BYTE   0
                .BYTE $6E ; n
                .BYTE   0
                .BYTE $2D ; -
                .BYTE   0
                .BYTE $78 ; x
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $6E ; n
                .BYTE   0
                .BYTE $D3
                .BYTE $FF
                .BYTE $54 ; T
                .BYTE   0
                .BYTE $AC
                .BYTE $FF
                .BYTE $2D ; -
                .BYTE   0
                .BYTE $92
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE $88
                .BYTE $FF
                .BYTE $D3
                .BYTE $FF
                .BYTE $92
                .BYTE $FF
                .BYTE $AC
                .BYTE $FF
                .BYTE $AC
                .BYTE $FF
                .BYTE $92
                .BYTE $FF
                .BYTE $D3
                .BYTE $FF
                .BYTE $88
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE $92
                .BYTE $FF
                .BYTE $2D ; -
                .BYTE   0
                .BYTE $AC
                .BYTE $FF
                .BYTE $54 ; T
                .BYTE   0
                .BYTE $D3
                .BYTE $FF
                .BYTE $6E ; n
                .BYTE   0
                .BYTE $85
                .BYTE $10
                .BYTE $84
                .BYTE $11
                .BYTE $A9
                .BYTE   0
                .BYTE $F0
                .BYTE   7

; =============== S U B R O U T I N E =======================================


sub_1818C:                              ; CODE XREF: sub_1802A+56↑p
                STA     $10
                LDA     #0
                STA     $11
                TYA
                LDY     #8

loc_18195:                              ; CODE XREF: sub_1818C+15↓j
                ASL     $11
                ROL
                CMP     $10
                BCC     loc_181A0
                SBC     $10
                INC     $11

loc_181A0:                              ; CODE XREF: sub_1818C+E↑j
                DEY
                BNE     loc_18195
                TAY
                LDA     $11
                RTS
; End of function sub_1818C


; =============== S U B R O U T I N E =======================================


sub_181A7:                              ; CODE XREF: sub_181E4↓p
                                        ; sub_181ED↓p ...
                LDA     $576,X
                TAY
                LDA     $542,X
                CLC
                ADC     unk_C0FF,Y
                BCC     loc_181B6
                LDA     #$FF

loc_181B6:                              ; CODE XREF: sub_181A7+B↑j
                STA     $7CC,X
                LDA     $542,X
                SEC
                SBC     unk_C0FF,Y
                BCS     loc_181C4
                LDA     #0

loc_181C4:                              ; CODE XREF: sub_181A7+19↑j
                STA     $7B2,X
                LDA     $55C,X
                CLC
                ADC     unk_C152,Y
                BCC     loc_181D2
                LDA     #$FF

loc_181D2:                              ; CODE XREF: sub_181A7+27↑j
                STA     $798,X
                LDA     $55C,X
                SEC
                SBC     unk_C152,Y
                BCS     loc_181E0
                LDA     #0

loc_181E0:                              ; CODE XREF: sub_181A7+35↑j
                STA     $77E,X
                RTS
; End of function sub_181A7


; =============== S U B R O U T I N E =======================================


sub_181E4:                              ; CODE XREF: sub_1AA67+22↓j
                                        ; sub_1AAA8+BF↓j ...
                JSR     sub_181A7
                JSR     sub_18230
                BCS     loc_1820C
                RTS
; End of function sub_181E4


; =============== S U B R O U T I N E =======================================


sub_181ED:                              ; CODE XREF: sub_1961B+D0↓p
                                        ; sub_1AD36+46↓p ...
                JSR     sub_181A7
                JSR     loc_18244
                BCS     loc_1820C
                RTS
; End of function sub_181ED


; =============== S U B R O U T I N E =======================================


sub_181F6:                              ; CODE XREF: sub_1961B+59↓p
                                        ; sub_1973F+78↓j ...
                JSR     sub_181A7
                JSR     sub_18230
                BCS     loc_1820C
                JSR     sub_18250
                BCS     loc_1820C
; End of function sub_181F6

; START OF FUNCTION CHUNK FOR sub_18204

locret_18203:                           ; CODE XREF: sub_18204+6↓j
                RTS
; END OF FUNCTION CHUNK FOR sub_18204

; =============== S U B R O U T I N E =======================================


sub_18204:                              ; CODE XREF: sub_1ACDE:loc_1AD05↓p
                                        ; sub_1AD25:loc_1AD2F↓p ...

; FUNCTION CHUNK AT 8203 SIZE 00000001 BYTES

                JSR     sub_181A7
                JSR     sub_18250
                BCC     locret_18203

loc_1820C:                              ; CODE XREF: sub_181E4+6↑j
                                        ; sub_181ED+6↑j ...
                STY     $8A
                LDA     $528,Y
                AND     #$7F
                STA     $89
                TAY
                LDA     $86CF,Y
                LDY     $8A
                STA     $528,Y
                LDA     $528,X
                AND     #$7F
                STA     $88
                STA     $7E6,X
                TAY
                LDA     $86CF,Y
                STA     $528,X
                RTS
; End of function sub_18204


; =============== S U B R O U T I N E =======================================


sub_18230:                              ; CODE XREF: sub_181E4+3↑p
                                        ; sub_181F6+3↑p
                LDA     $5F
                AND     #1
                BEQ     loc_18244
                LDA     $52C
                CMP     #$83
                BNE     loc_18244
                LDY     #4
                JSR     sub_18290
                BCS     locret_1828F

loc_18244:                              ; CODE XREF: sub_181ED+3↑p
                                        ; sub_18230+4↑j ...
                LDA     $528
                CMP     #$81
                BNE     loc_1828E
                LDY     #0
                JMP     sub_18290
; End of function sub_18230


; =============== S U B R O U T I N E =======================================


sub_18250:                              ; CODE XREF: sub_181F6+8↑p
                                        ; sub_18204+3↑p
                LDA     $529
                CMP     #$82
                BNE     loc_1825E
                LDY     #1
                JSR     sub_18290
                BCS     locret_1828F

loc_1825E:                              ; CODE XREF: sub_18250+5↑j
                LDA     $52A
                CMP     #$82
                BNE     loc_1826C
                LDY     #2
                JSR     sub_18290
                BCS     locret_1828F

loc_1826C:                              ; CODE XREF: sub_18250+13↑j
                LDA     $52B
                CMP     #$82
                BNE     loc_1827A
                LDY     #3
                JSR     sub_18290
                BCS     locret_1828F

loc_1827A:                              ; CODE XREF: sub_18250+21↑j
                LDA     $5F
                AND     #2
                BEQ     loc_1828E
                LDA     $52C
                CMP     #$83
                BNE     loc_1828E
                LDY     #4
                JSR     sub_18290
                BCS     locret_1828F

loc_1828E:                              ; CODE XREF: sub_18230+19↑j
                                        ; sub_18250+2E↑j ...
                CLC

locret_1828F:                           ; CODE XREF: sub_18230+12↑j
                                        ; sub_18250+C↑j ...
                RTS
; End of function sub_18250


; =============== S U B R O U T I N E =======================================


sub_18290:                              ; CODE XREF: sub_18230+F↑p
                                        ; sub_18230+1D↑j ...
                LDA     $798,Y
                CMP     $77E,X
                BCC     locret_1828F
                LDA     $798,X
                CMP     $77E,Y
                BCC     locret_1828F
                LDA     $7CC,Y
                CMP     $7B2,X
                BCC     locret_1828F
                LDA     $7CC,X
                CMP     $7B2,Y
                BCC     locret_1828F
                RTS
; End of function sub_18290


; =============== S U B R O U T I N E =======================================


sub_182B1:                              ; CODE XREF: sub_18ECA+4F↓p
                                        ; sub_19039:loc_19068↓p ...
                LDA     $660,X
                AND     #8
                BEQ     loc_182BB
                JSR     sub_18368

loc_182BB:                              ; CODE XREF: sub_182B1+5↑j
                LDA     $660,X
                AND     #$10
                BEQ     loc_182C5
                JSR     sub_18392

loc_182C5:                              ; CODE XREF: sub_182B1+F↑j
                LDA     $660,X
                AND     #1
                BEQ     loc_182CF
                JSR     sub_18322

loc_182CF:                              ; CODE XREF: sub_182B1+19↑j
                LDA     $660,X
                AND     #2
                BEQ     sub_182D9
                JSR     sub_18347
; End of function sub_182B1


; =============== S U B R O U T I N E =======================================


sub_182D9:                              ; CODE XREF: sub_182B1+23↑j
                                        ; sub_18CA4+14A↓p ...
                LDA     $576,X
                BEQ     locret_182F9
                TAY
                LDA     unk_C1A5,Y
                STA     $18
                LDA     unk_C1F8,Y

loc_182E7:
                STA     $19
                LDY     #0
                LDA     ($18),Y
                BEQ     locret_182F9
                STA     $14
                INY

loc_182F2:                              ; CODE XREF: sub_182D9+1E↓j
                JSR     sub_18412
                DEC     $14
                BNE     loc_182F2

locret_182F9:                           ; CODE XREF: sub_182D9+3↑j
                                        ; sub_182D9+14↑j
                RTS
; End of function sub_182D9

; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR sub_18322
;   ADDITIONAL PARENT FUNCTION sub_18347

loc_182FA:                              ; CODE XREF: sub_18322+15↓j
                                        ; sub_18322+19↓j ...
                PLA
                PLA
                PLA
                PLA
; END OF FUNCTION CHUNK FOR sub_18322

; =============== S U B R O U T I N E =======================================


sub_182FE:                              ; CODE XREF: sub_1876A+11↓j
                                        ; sub_19372+23↓j ...
                LDA     #0
                STA     $528,X
                STA     $542,X
                STA     $5C4,X
                STA     $5DE,X
                STA     $5F8,X
                STA     $62C,X
                STA     $612,X
                STA     $646,X
                STA     $5AA,X
                STA     $6E2,X
                STA     $6FC,X
                RTS
; End of function sub_182FE


; =============== S U B R O U T I N E =======================================


sub_18322:                              ; CODE XREF: sub_182B1+1B↑p

; FUNCTION CHUNK AT 82FA SIZE 00000004 BYTES

                LDA     $5F8,X
                CLC
                ADC     $5C4,X
                STA     $5C4,X
                LDA     $612,X
                BMI     loc_1833E
                ADC     $542,X
                STA     $542,X
                BCS     loc_182FA
                CMP     #$F0
                BCS     loc_182FA
                RTS
; ---------------------------------------------------------------------------

loc_1833E:                              ; CODE XREF: sub_18322+D↑j
                ADC     $542,X
                STA     $542,X
                BCC     loc_182FA
                RTS
; End of function sub_18322


; =============== S U B R O U T I N E =======================================


sub_18347:                              ; CODE XREF: sub_182B1+25↑p

; FUNCTION CHUNK AT 82FA SIZE 00000004 BYTES

                LDA     $62C,X
                CLC
                ADC     $5DE,X
                STA     $5DE,X
                LDA     $646,X
                BMI     loc_1835F
                ADC     $55C,X
                STA     $55C,X
                BCS     loc_182FA
                RTS
; ---------------------------------------------------------------------------

loc_1835F:                              ; CODE XREF: sub_18347+D↑j
                ADC     $55C,X
                STA     $55C,X
                BCC     loc_182FA

locret_18367:                           ; CODE XREF: sub_18368+6↓j
                                        ; sub_18392+6↓j
                RTS
; End of function sub_18347


; =============== S U B R O U T I N E =======================================


sub_18368:                              ; CODE XREF: sub_182B1+7↑p
                LDA     $542,X
                CMP     $6E2,X
                BEQ     locret_18367
                BCS     loc_18382
                LDA     $5F8,X
                CLC
                ADC     $716,X
                STA     $5F8,X
                BCC     locret_18381
                INC     $612,X

locret_18381:                           ; CODE XREF: sub_18368+14↑j
                RTS
; ---------------------------------------------------------------------------

loc_18382:                              ; CODE XREF: sub_18368+8↑j
                LDA     $5F8,X
                SEC
                SBC     $716,X
                STA     $5F8,X
                BCS     locret_18391
                DEC     $612,X

locret_18391:                           ; CODE XREF: sub_18368+24↑j
                RTS
; End of function sub_18368


; =============== S U B R O U T I N E =======================================


sub_18392:                              ; CODE XREF: sub_182B1+11↑p
                LDA     $55C,X
                CMP     $6FC,X
                BEQ     locret_18367
                BCS     loc_183AC
                LDA     $62C,X
                CLC
                ADC     $730,X
                STA     $62C,X
                BCC     locret_183AB
                INC     $646,X

locret_183AB:                           ; CODE XREF: sub_18392+14↑j
                RTS
; ---------------------------------------------------------------------------

loc_183AC:                              ; CODE XREF: sub_18392+8↑j
                LDA     $62C,X
                SEC
                SBC     $730,X
                STA     $62C,X
                BCS     locret_183BB
                DEC     $646,X

locret_183BB:                           ; CODE XREF: sub_18392+24↑j
                RTS
; End of function sub_18392


; =============== S U B R O U T I N E =======================================


sub_183BC:                              ; CODE XREF: sub_18F78:loc_18FF6↓p
                                        ; sub_19FD6:loc_1A033↓p ...
                LDA     $67A,X
                CLC
                ADC     $694,X
                STA     $694,X
                BCC     loc_183D8
                INC     $6AE,X
                LDA     $6AE,X
                CMP     $6C8,X
                BCC     loc_183D8
                LDA     #0
                STA     $6AE,X

loc_183D8:                              ; CODE XREF: sub_183BC+A↑j
                                        ; sub_183BC+15↑j
                LDA     $6AE,X
                TAY
                RTS
; End of function sub_183BC


; =============== S U B R O U T I N E =======================================


sub_183DD:                              ; CODE XREF: sub_18006↑j
                JSR     sub_C015
                LDA     $83
                AND     #$FE
                STA     $83
                LDA     #$10
                STA     $82
                LDA     #$EC
                STA     $81
                RTS
; End of function sub_183DD


; =============== S U B R O U T I N E =======================================


sub_183EF:                              ; CODE XREF: sub_18009↑j
                                        ; sub_187C7+3↓p
                LDA     $83
                AND     #$7D ; '}'
                STA     $83
                AND     #1
                BNE     loc_183FE
                LDA     #$10
                STA     $80
                RTS
; ---------------------------------------------------------------------------

loc_183FE:                              ; CODE XREF: sub_183EF+8↑j
                LDA     $82
                CMP     #$10
                BNE     loc_1840C
                LDA     $83
                ORA     #$80
                STA     $83
                LDA     #$F0

loc_1840C:                              ; CODE XREF: sub_183EF+13↑j
                SEC
                SBC     #4
                STA     $80
                RTS
; End of function sub_183EF


; =============== S U B R O U T I N E =======================================


sub_18412:                              ; CODE XREF: sub_182D9:loc_182F2↑p
                                        ; sub_1860C:loc_18616↓j
                STX     $65
                LDA     $83
                AND     #2
                BNE     loc_18435
                LDA     $55C,X
                PHA
                LDA     $590,X
                PHA
                LDA     ($18),Y
                CLC
                BMI     loc_1842E
                ADC     $542,X
                BCS     loc_18433
                BCC     loc_1843A

loc_1842E:                              ; CODE XREF: sub_18412+13↑j
                ADC     $542,X
                BCS     loc_1843A

loc_18433:                              ; CODE XREF: sub_18412+18↑j
                PLA
                PLA

loc_18435:                              ; CODE XREF: sub_18412+6↑j
                                        ; sub_185D8+8↓j
                INY
                INY
                INY
                INY
                RTS
; ---------------------------------------------------------------------------

loc_1843A:                              ; CODE XREF: sub_18412+1A↑j
                                        ; sub_18412+1F↑j
                INY
                LDX     $80
                STA     $200,X
                INX
                LDA     ($18),Y
                INY
                STA     $200,X
                INX
                PLA
                STA     $10
                EOR     ($18),Y
                INY
                STA     $200,X
                INX
                PLA
                STA     $11
                LDA     ($18),Y
                BIT     $10
                BVC     loc_18460
                EOR     #$FF
                SEC
                SBC     #7

loc_18460:                              ; CODE XREF: sub_18412+47↑j
                CMP     #$80
                BCS     loc_1846A
                ADC     $11
                BCS     loc_1846F
                BCC     loc_18477

loc_1846A:                              ; CODE XREF: sub_18412+50↑j
                CLC
                ADC     $11
                BCS     loc_18477

loc_1846F:                              ; CODE XREF: sub_18412+54↑j
                INY
                LDA     #$F8
                STA     $1FD,X
                BNE     loc_184A4

loc_18477:                              ; CODE XREF: sub_18412+56↑j
                                        ; sub_18412+5B↑j ...
                INY
                STA     $200,X
                LDA     $83
                AND     #1
                BNE     loc_18490
                INX
                STX     $80
                CPX     #$F0
                BNE     loc_184A4
                LDA     $83
                ORA     #$82
                STA     $83
                BNE     loc_184A4

loc_18490:                              ; CODE XREF: sub_18412+6D↑j
                TXA
                SEC
                SBC     #7
                STA     $80
                CMP     #$10
                BCS     loc_184A4
                LDA     $83
                ORA     #$80
                STA     $83
                LDA     #$EC
                STA     $80

loc_184A4:                              ; CODE XREF: sub_18412+63↑j
                                        ; sub_18412+74↑j ...
                LDX     $65
                RTS
; End of function sub_18412


; =============== S U B R O U T I N E =======================================


sub_184A7:                              ; CODE XREF: sub_1800C↑j
                                        ; sub_187C7:loc_18822↓j
                LDA     $83
                AND     #1
                BNE     loc_18501
                LDX     $80
                LDA     $83
                BPL     loc_184B8
                LDA     #$F0
                JMP     loc_184EA
; ---------------------------------------------------------------------------

loc_184B8:                              ; CODE XREF: sub_184A7+A↑j
                LDA     $82
                CMP     #$F0
                BEQ     loc_184D9
                LDA     #$F8

loc_184C0:                              ; CODE XREF: sub_184A7+24↓j
                CPX     $82
                BCS     loc_184CD
                STA     $200,X
                INX
                INX
                INX
                INX
                BNE     loc_184C0

loc_184CD:                              ; CODE XREF: sub_184A7+1B↑j
                CPX     $81
                BCS     loc_184D3
                LDX     $81

loc_184D3:                              ; CODE XREF: sub_184A7+28↑j
                LDA     $81
                CMP     #$EC
                BEQ     loc_184E8

loc_184D9:                              ; CODE XREF: sub_184A7+15↑j
                LDA     #$F8

loc_184DB:                              ; CODE XREF: sub_184A7+3F↓j
                CPX     #$F0
                BEQ     loc_184E8
                STA     $200,X
                INX
                INX
                INX
                INX
                BNE     loc_184DB

loc_184E8:                              ; CODE XREF: sub_184A7+30↑j
                                        ; sub_184A7+36↑j
                LDA     $80

loc_184EA:                              ; CODE XREF: sub_184A7+E↑j
                STA     $82
                LDA     $83
                AND     #$84
                BEQ     loc_18523
                CMP     #4
                BNE     loc_18523
                LDA     $7F
                AND     #$20 ; ' '
                BNE     loc_18523
                LDA     #$EC
                STA     $81
                RTS
; ---------------------------------------------------------------------------

loc_18501:                              ; CODE XREF: sub_184A7+4↑j
                LDA     $83
                AND     #$80
                BEQ     loc_1850E
                LDA     $80
                STA     $81
                JMP     loc_18523
; ---------------------------------------------------------------------------

loc_1850E:                              ; CODE XREF: sub_184A7+5E↑j
                LDX     #$10
                LDA     #$F8

loc_18512:                              ; CODE XREF: sub_184A7+76↓j
                STA     $200,X
                CPX     $80
                BEQ     loc_1851F
                INX
                INX
                INX
                INX
                BNE     loc_18512

loc_1851F:                              ; CODE XREF: sub_184A7+70↑j
                LDA     #$EC
                STA     $81

loc_18523:                              ; CODE XREF: sub_184A7+49↑j
                                        ; sub_184A7+4D↑j ...
                LDA     $83
                EOR     #1
                STA     $83
                RTS
; End of function sub_184A7


; =============== S U B R O U T I N E =======================================


sub_1852A:                              ; CODE XREF: sub_18018↑j
                LDY     #$10

loc_1852C:                              ; CODE XREF: sub_1852A+E↓j
                DEY
                DEY
                LDA     #0
                STA     $200,Y
                STA     $2F0,Y
                DEY
                DEY
                BNE     loc_1852C
                RTS
; End of function sub_1852A


; =============== S U B R O U T I N E =======================================


sub_1853B:                              ; CODE XREF: sub_1801B↑j
                                        ; sub_187C7+6↓p
                LDA     #0
                STA     $11
                LDX     #$F0
                LDA     $83
                AND     #1
                BNE     loc_18549
                LDX     #0

loc_18549:                              ; CODE XREF: sub_1853B+A↑j
                LDY     #0
                LDA     #$20 ; ' '
                STA     $10

loc_1854F:                              ; CODE XREF: sub_1853B+4D↓j
                CPX     #$10
                BNE     loc_18555
                LDX     #$F0

loc_18555:                              ; CODE XREF: sub_1853B+16↑j
                LDA     #$10
                STA     $200,X
                INX
                LDA     $11
                BNE     loc_18571
                LDA     $17A,Y
                STA     $11
                BNE     loc_18571
                CPY     #7
                BEQ     loc_18571
                LDA     #$F8
                STA     $1FF,X
                BNE     loc_18578

loc_18571:                              ; CODE XREF: sub_1853B+22↑j
                                        ; sub_1853B+29↑j ...
                LDA     $17A,Y
                ASL
                STA     $200,X

loc_18578:                              ; CODE XREF: sub_1853B+34↑j
                INX
                INX
                LDA     $10
                STA     $200,X
                CLC
                ADC     #9
                STA     $10
                INX
                INY
                CPY     #8
                BNE     loc_1854F
                RTS
; End of function sub_1853B


; =============== S U B R O U T I N E =======================================


sub_1858B:                              ; CODE XREF: sub_187C7+9↓p
                LDA     #$84
                STA     $18
                LDA     #0
                STA     $19
                LDA     #$D0
                STA     $84
                LDA     #$C6
                STA     $85
                LDA     #0
                STA     $86
                LDA     #$EA
                STA     $87
                LDY     #0
                JSR     sub_185D8
                LDY     $32
                BEQ     loc_185AD
                DEY

loc_185AD:                              ; CODE XREF: sub_1858B+1F↑j
                CPY     #$A
                LDA     #0
                STA     $86
                LDA     #$CC
                STA     $84
                BCC     loc_185C1
                LDA     #$CD
                STA     $87
                TYA
                JMP     sub_18E29
; ---------------------------------------------------------------------------

loc_185C1:                              ; CODE XREF: sub_1858B+2C↑j
                TYA
                ASL
                STA     $85
                LDA     #$DE
                STA     $87
                LDY     #0
                JMP     sub_185D8
; End of function sub_1858B

; ---------------------------------------------------------------------------
                .BYTE $A9
                .BYTE $84
                .BYTE $85
                .BYTE $18
                .BYTE $A9
                .BYTE   0
                .BYTE $85
                .BYTE $19
                .BYTE $A0
                .BYTE   0

; =============== S U B R O U T I N E =======================================


sub_185D8:                              ; CODE XREF: sub_1858B+1A↑p
                                        ; sub_1858B+40↑j ...
                STX     $65
                LDA     $83
                AND     #2
                BEQ     loc_185E3
                JMP     loc_18435
; ---------------------------------------------------------------------------

loc_185E3:                              ; CODE XREF: sub_185D8+6↑j
                LDX     $80
                LDA     ($18),Y
                STA     $200,X
                INX
                INY
                LDA     ($18),Y
                STA     $200,X
                INX
                INY
                LDA     ($18),Y
                STA     $200,X
                INX
                INY
                LDA     ($18),Y
                JMP     loc_18477
; End of function sub_185D8


; =============== S U B R O U T I N E =======================================


sub_185FF:                              ; CODE XREF: sub_18835+8B↓p
                LDA     ($18),Y
                STA     $14
                INY

loc_18604:                              ; CODE XREF: sub_185FF+A↓j
                JSR     sub_185D8
                DEC     $14
                BNE     loc_18604
                RTS
; End of function sub_185FF


; =============== S U B R O U T I N E =======================================


sub_1860C:                              ; CODE XREF: sub_1A779+7E↓p
                                        ; sub_1AE99+1C↓p ...
                LDA     #0
                STA     $19
                LDA     #$84
                STA     $18
                LDY     #0

loc_18616:
                JMP     sub_18412
; End of function sub_1860C

; ---------------------------------------------------------------------------
unk_18619:      .BYTE $67 ; g
                .BYTE $38 ; 8
                .BYTE $7E ; ~
                .BYTE $1A
                .BYTE $1A
                .BYTE $1A
                .BYTE $3E ; >
                .BYTE $C6
                .BYTE $FC
                .BYTE $28 ; (
                .BYTE $C5
                .BYTE $3B ; ;
                .BYTE $3B ; ;
                .BYTE $F5
                .BYTE $F6
                .BYTE $98
                .BYTE $48 ; H
                .BYTE $6B ; k
                .BYTE $34 ; 4
                .BYTE $A2
                .BYTE $47 ; G
                .BYTE $D5
                .BYTE $89
                .BYTE $79 ; y
                .BYTE $37 ; 7
                .BYTE $BA
                .BYTE $6A ; j
                .BYTE $9F
                .BYTE $C8
                .BYTE $DA
                .BYTE $CA
                .BYTE $DA
                .BYTE $CA
                .BYTE $EB
                .BYTE $E9
                .BYTE $F0
                .BYTE $61 ; a
                .BYTE $8B
                .BYTE $9E
                .BYTE $FD
                .BYTE $A7
                .BYTE $A1
                .BYTE $A9
                .BYTE $4B ; K
                .BYTE $E7
                .BYTE   8
                .BYTE   8
                .BYTE $89
                .BYTE $36 ; 6
                .BYTE $36 ; 6
                .BYTE $2A ; *
                .BYTE $36 ; 6
                .BYTE $DD
                .BYTE $24 ; $
                .BYTE $35 ; 5
                .BYTE   8
                .BYTE $44 ; D
                .BYTE $A4
                .BYTE $C0
                .BYTE $77 ; w
                .BYTE $78 ; x
                .BYTE $19
                .BYTE $E7
                .BYTE $CE
                .BYTE $EB
                .BYTE $EB
                .BYTE $35 ; 5
                .BYTE $8D
                .BYTE $5F ; _
                .BYTE $C0
                .BYTE $C0
                .BYTE $67 ; g
                .BYTE $41 ; A
                .BYTE $41 ; A
                .BYTE $41 ; A
                .BYTE $41 ; A
                .BYTE $41 ; A
                .BYTE $41 ; A
                .BYTE $36 ; 6
                .BYTE $9A
                .BYTE $C0
                .BYTE $98
                .BYTE $B5
                .BYTE $2B ; +
                .BYTE $2B ; +
                .BYTE $2B ; +
                .BYTE $2B ; +
                .BYTE $C0
                .BYTE $C0
                .BYTE $B7
unk_18673:      .BYTE $84
                .BYTE $8C
                .BYTE $90
                .BYTE $90
                .BYTE $96
                .BYTE $96
                .BYTE $96
                .BYTE $97
                .BYTE $97
                .BYTE $97
                .BYTE $98
                .BYTE $98
                .BYTE $99
                .BYTE $99
                .BYTE $99
                .BYTE $9A
                .BYTE $9E
                .BYTE $9F
                .BYTE $9F
                .BYTE $94
                .BYTE $AC
                .BYTE $AC
                .BYTE $9F
                .BYTE $A0
                .BYTE $9D
                .BYTE $9E
                .BYTE $A0
                .BYTE $A1
                .BYTE $A1
                .BYTE $A1
                .BYTE $A2
                .BYTE $A3
                .BYTE $A2
                .BYTE $A3
                .BYTE $A3
                .BYTE $A8
                .BYTE $A6
                .BYTE $AA
                .BYTE $AA
                .BYTE $AA
                .BYTE $82
                .BYTE $AA
                .BYTE $AB
                .BYTE $AB
                .BYTE $A7
                .BYTE $AB
                .BYTE $A5
                .BYTE $A5
                .BYTE $9B
                .BYTE $9C
                .BYTE $9C
                .BYTE $9C
                .BYTE $9C
                .BYTE $AC
                .BYTE $AD
                .BYTE $AD
                .BYTE $A6
                .BYTE $A6
                .BYTE $A6
                .BYTE $A6
                .BYTE $8F
                .BYTE $A7
                .BYTE $AE
                .BYTE $96
                .BYTE $A6
                .BYTE $A3
                .BYTE $A3
                .BYTE $A8
                .BYTE $95
                .BYTE $98
                .BYTE $AE
                .BYTE $AE
                .BYTE $B2
                .BYTE $B3
                .BYTE $B3
                .BYTE $B3
                .BYTE $B3
                .BYTE $B3
                .BYTE $B3
                .BYTE $B3
                .BYTE $B6
                .BYTE $AE
                .BYTE $AE
                .BYTE $B6
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $AE
                .BYTE $AE
                .BYTE $B7
                .BYTE $A9
                .BYTE   0
                .BYTE $3C ; <
                .BYTE $28 ; (
                .BYTE $13
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $27 ; '
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $50 ; P
                .BYTE $37 ; 7
                .BYTE $37 ; 7
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $3C ; <
                .BYTE $23 ; #
                .BYTE $28 ; (
                .BYTE $28 ; (
                .BYTE $28 ; (
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $23 ; #
                .BYTE $28 ; (
                .BYTE $28 ; (
                .BYTE $48 ; H
                .BYTE $48 ; H
                .BYTE $28 ; (
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $28 ; (
                .BYTE $50 ; P
                .BYTE $53 ; S
                .BYTE $28 ; (
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $50 ; P
                .BYTE $53 ; S
                .BYTE $50 ; P
                .BYTE $5B ; [

; =============== S U B R O U T I N E =======================================


sub_1872B:                              ; CODE XREF: sub_18746↓p
                                        ; sub_18F78+38↓p ...
                STX     $10
                LDX     $7E
                SEC
                ROL
                EOR     unk_18619,X
                ADC     $10,X
                STA     $7E
                TSX
                TXA
                LDX     $10
                ADC     $7E
                ADC     $528,X
                ADC     $A2
                STA     $7E
                RTS
; End of function sub_1872B


; =============== S U B R O U T I N E =======================================


sub_18746:                              ; CODE XREF: sub_19829+A↓p
                                        ; sub_19860+F↓p ...
                JSR     sub_1872B
                STY     $10
                AND     #$7F
                STA     $55C,X
                ROR
                TAY
                ADC     unk_18619,Y
                STA     $7E
                AND     #$3F ; '?'
                CLC
                ADC     $55C,X
                ADC     #$20 ; ' '
                STA     $55C,X
                LDA     #8
                STA     $542,X
                LDY     $10
                RTS
; End of function sub_18746


; =============== S U B R O U T I N E =======================================


sub_1876A:                              ; CODE XREF: sub_1A2DB+D↓p
                                        ; sub_1A645+5↓p ...
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_18779
                TYA
                STA     $764,X
                LDA     #$27 ; '''
                STA     $528,Y
                RTS
; ---------------------------------------------------------------------------

loc_18779:                              ; CODE XREF: sub_1876A+3↑j
                PLA
                PLA
                JMP     sub_182FE
; End of function sub_1876A


; =============== S U B R O U T I N E =======================================


sub_1877E:                              ; CODE XREF: sub_1A645:loc_1A659↓p
                LDA     $764,X
                TAY
                LDA     #2
                STA     $542,Y
                RTS
; End of function sub_1877E


; =============== S U B R O U T I N E =======================================


sub_18788:                              ; CODE XREF: sub_18CA4+CE↓p
                                        ; sub_1A8EA+13↓p ...
                INC     $47
                BNE     loc_1878F
                DEC     $47

locret_1878E:                           ; CODE XREF: sub_18796+2↓j
                RTS
; ---------------------------------------------------------------------------

loc_1878F:                              ; CODE XREF: sub_18788+2↑j
                                        ; sub_18796+6↓j
                LDA     $62
                ORA     #1
                STA     $62
                RTS
; End of function sub_18788


; =============== S U B R O U T I N E =======================================


sub_18796:                              ; CODE XREF: sub_1A779:loc_1A803↓p
                                        ; sub_1A779+8D↓p ...
                LDA     $47
                BEQ     locret_1878E
                DEC     $47
                JMP     loc_1878F
; End of function sub_18796


; =============== S U B R O U T I N E =======================================


sub_1879F:                              ; CODE XREF: sub_18CA4+D9↓p
                                        ; sub_1A8EA+2B↓p ...
                INC     $49
                BNE     locret_187A5
                DEC     $49

locret_187A5:                           ; CODE XREF: sub_1879F+2↑j
                                        ; sub_187A6+2↓j
                RTS
; End of function sub_1879F


; =============== S U B R O U T I N E =======================================


sub_187A6:                              ; CODE XREF: sub_196F3+2C↓p
                                        ; sub_196F3+2F↓p ...
                LDA     $49
                BEQ     locret_187A5
                DEC     $49
                RTS
; End of function sub_187A6

; ---------------------------------------------------------------------------
unk_187AD:      .BYTE   0
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   4
                .BYTE $19
                .BYTE $18
                .BYTE $17
                .BYTE $16
                .BYTE $15
                .BYTE $14
                .BYTE $13
                .BYTE $12
                .BYTE $11
                .BYTE $10
                .BYTE  $F
                .BYTE  $E
                .BYTE  $D
                .BYTE  $C
                .BYTE  $B
                .BYTE  $A
                .BYTE   9
                .BYTE   8
                .BYTE   7
                .BYTE   6
                .BYTE   5

; =============== S U B R O U T I N E =======================================


sub_187C7:                              ; CODE XREF: sub_18000↑j
                LDA     $83
                PHA
                JSR     sub_183EF
                JSR     sub_1853B
                JSR     sub_1858B
                LDX     #0
                STX     $6A
                PLA
                AND     #$80
                BNE     loc_187E2
                LDA     $7F
                AND     #$20 ; ' '
                BEQ     loc_187E4

loc_187E2:                              ; CODE XREF: sub_187C7+13↑j
                INC     $6B

loc_187E4:                              ; CODE XREF: sub_187C7+19↑j
                                        ; sub_187C7+46↓j
                LDA     $6B
                AND     #2
                BEQ     loc_187EE
                LDA     unk_187AD,X
                TAX

loc_187EE:                              ; CODE XREF: sub_187C7+21↑j
                STX     $40
                LDA     $528,X
                BEQ     loc_18807
                AND     #$7F
                TAY
                LDA     #$88
                PHA
                LDA     #6
                PHA
                LDA     unk_18673,Y
                PHA
                LDA     loc_18616+2,Y
                PHA
                RTS
; ---------------------------------------------------------------------------

loc_18807:                              ; CODE XREF: sub_187C7+2C↑j
                INC     $6A
                LDX     $6A
                CPX     #$1A
                BNE     loc_187E4
                JSR     sub_18835
                LDA     $19B
                BEQ     loc_18822
                LDA     $F5
                AND     #$20 ; ' '
                BEQ     loc_18822
                LDA     $7A
                JSR     sub_19493

loc_18822:                              ; CODE XREF: sub_187C7+4E↑j
                                        ; sub_187C7+54↑j
                JMP     sub_184A7
; End of function sub_187C7


; =============== S U B R O U T I N E =======================================

; Checks that the enemy list is complete. If overflowed, sets Carry flag.

check_enemy_overflow:                   ; CODE XREF: j_check_enemy_overflow↑j
                                        ; sub_1876A↑p ...
                LDY     #5

loc_18827:                              ; CODE XREF: check_enemy_overflow+A↓j
                LDA     $528,Y
                BEQ     loc_18833
                INY
                CPY     #$1A
                BNE     loc_18827
                SEC
                RTS
; ---------------------------------------------------------------------------

loc_18833:                              ; CODE XREF: check_enemy_overflow+5↑j
                CLC

locret_18834:                           ; CODE XREF: sub_18835+5↓j
                                        ; sub_18835+B↓j
                RTS
; End of function check_enemy_overflow


; =============== S U B R O U T I N E =======================================


sub_18835:                              ; CODE XREF: sub_18003↑j
                                        ; sub_187C7+48↑p
                JSR     sub_C033
                LDA     $66
                BEQ     locret_18834
                BMI     loc_18874
                LDA     $2B
                BNE     locret_18834
                LDA     #0
                STA     $79
                LDA     $66
                ORA     #$80
                STA     $66
                CMP     #$86
                BCC     loc_18852
                LDA     #6

loc_18852:                              ; CODE XREF: sub_18835+19↑j
                ASL
                ASL
                TAY
                LDA     #$F
                STA     $2F
                LDA     #$80
                STA     $2E
                LDA     $89B1,Y
                STA     $2C
                LDA     $89B2,Y
                STA     $2D
                LDA     #6
                STA     $26
                LDA     #2
                STA     $2B
                LDA     $89B4,Y
                STA     $67

loc_18874:                              ; CODE XREF: sub_18835+7↑j
                LDA     $79
                BNE     loc_18889
                LDA     $2B
                BNE     locret_18888
                INC     $79
                LDA     $66
                CMP     #$86
                BCC     loc_18889
                SBC     #$86
                STA     $66

locret_18888:                           ; CODE XREF: sub_18835+45↑j
                                        ; sub_18835+58↓j
                RTS
; ---------------------------------------------------------------------------

loc_18889:                              ; CODE XREF: sub_18835+41↑j
                                        ; sub_18835+4D↑j
                LDA     $66
                CMP     #$86
                BCS     locret_18888
                LDA     $67
                BEQ     loc_1889C
                DEC     $67
                BNE     loc_1889C
                LDA     #0
                STA     $66
                RTS
; ---------------------------------------------------------------------------

loc_1889C:                              ; CODE XREF: sub_18835+5C↑j
                                        ; sub_18835+60↑j
                LDA     $66
                LDY     #0
                CMP     #$81
                BEQ     loc_188B8
                LDY     #$11
                CMP     #$82
                BEQ     loc_188B8
                LDY     #$22 ; '"'
                CMP     #$83
                BEQ     loc_188B8
                LDY     #$33 ; '3'
                CMP     #$84
                BEQ     loc_188B8
                LDY     #$44 ; 'D'

loc_188B8:                              ; CODE XREF: sub_18835+6D↑j
                                        ; sub_18835+73↑j ...
                LDA     #$60 ; '`'
                STA     $18
                LDA     #$89
                STA     $19
                JSR     sub_185FF
                LDA     $66
                CMP     #$83
                BNE     loc_188CA
                RTS
; ---------------------------------------------------------------------------

loc_188CA:                              ; CODE XREF: sub_18835+92↑j
                CMP     #$85
                BNE     loc_188CF
                RTS
; ---------------------------------------------------------------------------

loc_188CF:                              ; CODE XREF: sub_18835+97↑j
                LDY     #0
                STY     $19
                LDY     #$84
                STY     $18
                CMP     #$81
                BNE     loc_1890C
                LDA     #$6B ; 'k'
                STA     $84
                LDA     $91
                CMP     #$A
                PHP
                BCC     loc_188E8
                LDA     #1

loc_188E8:                              ; CODE XREF: sub_18835+AF↑j
                ASL
                STA     $85
                LDA     #0
                STA     $86
                LDA     #$88
                STA     $87
                LDY     #0
                JSR     sub_185D8
                PLP
                BCS     loc_188FC
                RTS
; ---------------------------------------------------------------------------

loc_188FC:                              ; CODE XREF: sub_18835+C4↑j
                LDA     $91
                SBC     #$A
                ASL
                STA     $85
                LDA     #$91
                STA     $87
                LDY     #0
                JMP     sub_185D8
; ---------------------------------------------------------------------------

loc_1890C:                              ; CODE XREF: sub_18835+A4↑j
                CMP     #$82
                BNE     loc_1891D
                LDA     #$38 ; '8'
                STA     $84
                LDA     #$C8
                STA     $87
                LDA     $C5
                JMP     sub_18E29
; ---------------------------------------------------------------------------

loc_1891D:                              ; CODE XREF: sub_18835+D9↑j
                LDA     #$A0
                STA     $84
                LDA     #0
                STA     $86
                LDA     #$70 ; 'p'
                STA     $87
                LDA     $CB
                STA     $10
                ASL
                ADC     $10
                ASL
                TAX
                LDA     #6
                STA     $14

loc_18936:                              ; CODE XREF: sub_18835+109↓j
                JSR     sub_C048
                BNE     loc_18941
                INX
                DEC     $14
                BNE     loc_18936
                RTS
; ---------------------------------------------------------------------------

loc_18941:                              ; CODE XREF: sub_18835+104↑j
                                        ; sub_18835+121↓j
                JSR     sub_C048
                INX
                ASL
                STA     $85
                LDY     #0
                JSR     sub_185D8
                LDA     $87
                CLC
                ADC     #9
                STA     $87
                DEC     $14
                BNE     loc_18941
                LDA     #0
                STA     $85
                TAY
                JMP     sub_185D8
; End of function sub_18835

; ---------------------------------------------------------------------------
                .BYTE   4
                .BYTE $70 ; p
                .BYTE $F8
                .BYTE   0
                .BYTE $60 ; `
                .BYTE $70 ; p
                .BYTE $FA
                .BYTE   0
                .BYTE $69 ; i
                .BYTE $70 ; p
                .BYTE $FC
                .BYTE   0
                .BYTE $72 ; r
                .BYTE $70 ; p
                .BYTE $FE
                .BYTE   0
                .BYTE $7B ; {
                .BYTE   4
                .BYTE $28 ; (
                .BYTE $F8
                .BYTE   0
                .BYTE $C0
                .BYTE $28 ; (
                .BYTE $FA
                .BYTE   0
                .BYTE $C9
                .BYTE $28 ; (
                .BYTE $FC
                .BYTE   0
                .BYTE $D2
                .BYTE $28 ; (
                .BYTE $FE
                .BYTE   0
                .BYTE $DB
                .BYTE   4
                .BYTE $70 ; p
                .BYTE $F8
                .BYTE   0
                .BYTE $68 ; h
                .BYTE $70 ; p
                .BYTE $FA
                .BYTE   0
                .BYTE $71 ; q
                .BYTE $70 ; p
                .BYTE $FC
                .BYTE   0
                .BYTE $7A ; z
                .BYTE $70 ; p
                .BYTE $FE
                .BYTE   0
                .BYTE $83
                .BYTE   4
                .BYTE $90
                .BYTE $F8
                .BYTE   0
                .BYTE $60 ; `
                .BYTE $90
                .BYTE $FA
                .BYTE   0
                .BYTE $68 ; h
                .BYTE $90
                .BYTE $FC
                .BYTE   0
                .BYTE $70 ; p
                .BYTE $90
                .BYTE $FE
                .BYTE   0
                .BYTE $78 ; x
                .BYTE   4
                .BYTE $AC
                .BYTE $F8
                .BYTE   0
                .BYTE $70 ; p
                .BYTE $AC
                .BYTE $FA
                .BYTE   0
                .BYTE $78 ; x
                .BYTE $AC
                .BYTE $FC
                .BYTE   0
                .BYTE $80
unk_189B1:      .BYTE $AC
unk_189B2:      .BYTE $FE
                .BYTE   0
unk_189B4:      .BYTE $88
                .BYTE $60 ; `
                .BYTE $8B
                .BYTE  $B
                .BYTE $FF
                .BYTE $D4
                .BYTE $89
                .BYTE  $B
                .BYTE   0
                .BYTE $DC
                .BYTE $8A
                .BYTE $10
                .BYTE   0
                .BYTE $58 ; X
                .BYTE $8A
                .BYTE  $B
                .BYTE   0
                .BYTE $E4
                .BYTE $8B
                .BYTE $10
                .BYTE   0
                .BYTE $CD
                .BYTE $89
                .BYTE $10
                .BYTE   0
                .BYTE $FE
                .BYTE $FF
                .BYTE $FE
                .BYTE   0
                .BYTE $80
                .BYTE $FE
                .BYTE $FE
                .BYTE $AA
                .BYTE $BB
                .BYTE   0
                .BYTE $E7
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $FF
                .BYTE $FF
                .BYTE $18
                .BYTE $18
                .BYTE $18
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $18
                .BYTE $18
                .BYTE $18
                .BYTE $18
                .BYTE $18
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $36 ; 6
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $FE
                .BYTE $FF
                .BYTE $C9
                .BYTE $C9
                .BYTE $C9
                .BYTE $E9
                .BYTE $E9
                .BYTE $E9
                .BYTE   0
                .BYTE   0
                .BYTE $E9
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $E9
                .BYTE $E9
                .BYTE $E9
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $3F ; ?
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $1F
                .BYTE   0
                .BYTE   0
                .BYTE $FF
                .BYTE $FF
                .BYTE $C0
                .BYTE $C0
                .BYTE $FF
                .BYTE $FF
                .BYTE $E0
                .BYTE $E0
                .BYTE   0
                .BYTE   0
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $E0
                .BYTE $FF
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $AA
                .BYTE $AA
                .BYTE $AA
                .BYTE $BB
                .BYTE   0
                .BYTE $70 ; p
                .BYTE   0
                .BYTE   0
                .BYTE   4
                .BYTE $70 ; p
                .BYTE   0
                .BYTE   0
                .BYTE $F8
                .BYTE $FD
                .BYTE $8D
                .BYTE $8D
                .BYTE $FD
                .BYTE $FD
                .BYTE $8D
                .BYTE $8D
                .BYTE   0
                .BYTE   5
                .BYTE $F8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $8D
                .BYTE $FD
                .BYTE $F8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $70 ; p
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $F3
                .BYTE $FB
                .BYTE $8B
                .BYTE $8B
                .BYTE $8B
                .BYTE $8B
                .BYTE $8B
                .BYTE $8B
                .BYTE   0
                .BYTE   8
                .BYTE $F3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $8B
                .BYTE $FB
                .BYTE $F3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $C0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $CC
                .BYTE $EC
                .BYTE $2C ; ,
                .BYTE $2C ; ,
                .BYTE $2C ; ,
                .BYTE $2C ; ,
                .BYTE $2C ; ,
                .BYTE $2C ; ,
                .BYTE   0
                .BYTE   8
                .BYTE $27 ; '
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $2C ; ,
                .BYTE $2F ; /
                .BYTE $27 ; '
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE  $E
                .BYTE   1
                .BYTE   0
                .BYTE $20
                .BYTE $1C
                .BYTE   0
                .BYTE   0
                .BYTE $9E
                .BYTE $BF
                .BYTE $B1
                .BYTE $B0
                .BYTE $BE
                .BYTE $9F
                .BYTE $83
                .BYTE $83
                .BYTE   0
                .BYTE $A1
                .BYTE $1E
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $A3
                .BYTE $BF
                .BYTE $1E
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $AA
                .BYTE $AA
                .BYTE $AA
                .BYTE $BB
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE   6
                .BYTE   0
                .BYTE $18
                .BYTE   0
                .BYTE   2
                .BYTE $7E ; ~
                .BYTE $7C ; |
                .BYTE $FE
                .BYTE $C6
                .BYTE $DE
                .BYTE $DE
                .BYTE $C6
                .BYTE $7E ; ~
                .BYTE $7E ; ~
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $82
                .BYTE $7C ; |
                .BYTE $7C ; |
                .BYTE $FE
                .BYTE $C6
                .BYTE $C6
                .BYTE $C6
                .BYTE $C6
                .BYTE $FE
                .BYTE $7C ; |
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE $C6
                .BYTE $7C ; |
                .BYTE $FE
                .BYTE $C6
                .BYTE $C6
                .BYTE $FE
                .BYTE $FE
                .BYTE $C6
                .BYTE $C6
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $82
                .BYTE $44 ; D
                .BYTE $38 ; 8
                .BYTE $C6
                .BYTE $C6
                .BYTE $C6
                .BYTE $C6
                .BYTE $C6
                .BYTE $EE
                .BYTE $7C ; |
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $28 ; (
                .BYTE $10
                .BYTE   0
                .BYTE   0
                .BYTE $C6
                .BYTE $C6
                .BYTE $EE
                .BYTE $FE
                .BYTE $FE
                .BYTE $D6
                .BYTE $C6
                .BYTE $C6
                .BYTE $C6
                .BYTE   0
                .BYTE $3E ; >
                .BYTE   0
                .BYTE   0
                .BYTE $3E ; >
                .BYTE   0
                .BYTE   0
                .BYTE $FE
                .BYTE $FE
                .BYTE $FE
                .BYTE $C0
                .BYTE $FE
                .BYTE $FE
                .BYTE $C0
                .BYTE $FE
                .BYTE $FE
                .BYTE   0
                .BYTE $3E ; >
                .BYTE   0
                .BYTE   0
                .BYTE $3E ; >
                .BYTE   0
                .BYTE   0
                .BYTE $FE
                .BYTE $FE
                .BYTE $FE
                .BYTE $C0
                .BYTE $FE
                .BYTE $FE
                .BYTE $C0
                .BYTE $FE
                .BYTE $FE
                .BYTE   0
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE   0
                .BYTE   2
                .BYTE $38 ; 8
                .BYTE   0
                .BYTE $C6
                .BYTE $FC
                .BYTE $FE
                .BYTE $C6
                .BYTE $C6
                .BYTE $FE
                .BYTE $FE
                .BYTE $C6
                .BYTE $C6
                .BYTE $AA
                .BYTE $AA
                .BYTE $AA
                .BYTE $BB
                .BYTE   0
                .BYTE $3E ; >
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $1E
                .BYTE $7E ; ~
                .BYTE $FF
                .BYTE $C1
                .BYTE $C1
                .BYTE $C1
                .BYTE $C1
                .BYTE $FF
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE $E1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $E1
                .BYTE $E1
                .BYTE $E1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $3E ; >
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE $3C ; <
                .BYTE   0
                .BYTE $FE
                .BYTE $FF
                .BYTE $C1
                .BYTE $C1
                .BYTE $C1
                .BYTE $FF
                .BYTE $FF
                .BYTE $C3
                .BYTE   0
                .BYTE   0
                .BYTE $C3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $C3
                .BYTE $C3
                .BYTE $C3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $3F ; ?
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $1F
                .BYTE   0
                .BYTE   0
                .BYTE $FF
                .BYTE $FF
                .BYTE $C0
                .BYTE $C0
                .BYTE $FF
                .BYTE $FF
                .BYTE $E0
                .BYTE $E0
                .BYTE   0
                .BYTE   0
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $E0
                .BYTE $FF
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $3E ; >
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE $3C ; <
                .BYTE   0
                .BYTE $FE
                .BYTE $FF
                .BYTE $C1
                .BYTE $C1
                .BYTE $C1
                .BYTE $FF
                .BYTE $FF
                .BYTE $C3
                .BYTE   0
                .BYTE   0
                .BYTE $C3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $C3
                .BYTE $C3
                .BYTE $C3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $AA
                .BYTE $AA
                .BYTE $AA
                .BYTE $BB
                .BYTE $1F
                .BYTE $7F ; 
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $FF
                .BYTE $FF
                .BYTE   0
                .BYTE $71 ; q
                .BYTE $4A ; J
                .BYTE $72 ; r
                .BYTE $41 ; A
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $F0
                .BYTE $FC
                .BYTE $FC
                .BYTE $FE
                .BYTE $FE
                .BYTE $FE
                .BYTE $FE
                .BYTE $FC
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $FC
                .BYTE $F0
                .BYTE   0
                .BYTE $92
                .BYTE $5A ; Z
                .BYTE $56 ; V
                .BYTE $92
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $1F
                .BYTE $7F ; 
                .BYTE $7F ; 
                .BYTE $FF
                .BYTE $FC
                .BYTE $FC
                .BYTE $FF
                .BYTE $7F ; 
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $7F ; 
                .BYTE $1F
                .BYTE   0
                .BYTE $93
                .BYTE $94
                .BYTE $64 ; d
                .BYTE $63 ; c
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $F0
                .BYTE $FC
                .BYTE $FC
                .BYTE $FE
                .BYTE   0
                .BYTE   0
                .BYTE $FE
                .BYTE $FE
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $FC
                .BYTE $F0
                .BYTE   0
                .BYTE $98
                .BYTE $24 ; $
                .BYTE $3C ; <
                .BYTE $A4
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $AA
                .BYTE $AA
                .BYTE $AD
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $30 ; 0
                .BYTE $37 ; 7
                .BYTE   9
                .BYTE $80
                .BYTE $8D
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $A9
                .BYTE $A0
                .BYTE $8D
                .BYTE $42 ; B
                .BYTE   5
                .BYTE $A9
                .BYTE $80
                .BYTE $8D
                .BYTE $5C ; \
                .BYTE   5
                .BYTE $A9
                .BYTE   0
                .BYTE $8D
                .BYTE $90
                .BYTE   5
                .BYTE $A9
                .BYTE   5
                .BYTE $8D
                .BYTE $7A ; z
                .BYTE   6
                .BYTE $A9
                .BYTE 5
                .BYTE $8D
                .BYTE $4A ; J
                .BYTE   7
                .BYTE $A9
                .BYTE $40
                .BYTE $8D
                .BYTE $64 ; d
                .BYTE   7
                .BYTE $A9
                .BYTE $80
                .BYTE $8D
                .BYTE $AA
                .BYTE   5
                .BYTE $A5
                .BYTE $5C ; \
                .BYTE $C9
                .BYTE   2
                .BYTE $90
                .BYTE   9
                .BYTE $C9
                .BYTE   4
                .BYTE $B0
                .BYTE   5
                .BYTE $A9
                .BYTE   3
                .BYTE $8D
                .BYTE $2C ; ,
                .BYTE   5

; =============== S U B R O U T I N E =======================================


sub_18CA4:                              ; CODE XREF: sub_18F78+14↓j
                LDA     $31
                BEQ     loc_18CAB
                JMP     sub_18ECA
; ---------------------------------------------------------------------------

loc_18CAB:                              ; CODE XREF: sub_18CA4+2↑j
                JSR     $C02D
                LDY     $34
                LDA     $8E74,Y
                BNE     loc_18CC8
                LDA     $67A
                CMP     #5
                BEQ     loc_18CDD
                BCS     loc_18CC3
                INC     $67A
                BPL     loc_18CDD

loc_18CC3:                              ; CODE XREF: sub_18CA4+18↑j
                DEC     $67A
                BPL     loc_18CDD

loc_18CC8:                              ; CODE XREF: sub_18CA4+F↑j
                CLC
                ADC     $67A
                STA     $67A
                BPL     loc_18CD6
                INC     $67A
                BPL     loc_18CDD

loc_18CD6:                              ; CODE XREF: sub_18CA4+2B↑j
                CMP     #$B
                BNE     loc_18CDD
                DEC     $67A

loc_18CDD:                              ; CODE XREF: sub_18CA4+16↑j
                                        ; sub_18CA4+1D↑j ...
                LDY     $67A
                LDA     unk_18E86,Y
                STA     $576
                LDY     $34
                CPY     #4
                BEQ     loc_18D3E
                LDA     $8E7D,Y
                JSR     sub_18097
                LDA     $5F8
                CLC
                ADC     $5C4
                TAY
                LDA     $612
                ADC     $542
                CMP     #$18
                BCS     loc_18D0A
                LDA     #$18
                LDY     #0
                BEQ     loc_18D12

loc_18D0A:                              ; CODE XREF: sub_18CA4+5E↑j
                CMP     #$D8
                BCC     loc_18D12
                LDA     #$D8
                LDY     #0

loc_18D12:                              ; CODE XREF: sub_18CA4+64↑j
                                        ; sub_18CA4+68↑j
                STA     $542
                STY     $5C4
                LDA     $62C
                CLC
                ADC     $5DE
                TAY
                LDA     $646
                ADC     $55C
                CMP     #$10
                BCS     loc_18D30
                LDA     #$10
                LDY     #0
                BEQ     loc_18D38

loc_18D30:                              ; CODE XREF: sub_18CA4+84↑j
                CMP     #$F0
                BCC     loc_18D38
                LDA     #$F0
                LDY     #0

loc_18D38:                              ; CODE XREF: sub_18CA4+8A↑j
                                        ; sub_18CA4+8E↑j
                STA     $55C
                STY     $5DE

loc_18D3E:                              ; CODE XREF: sub_18CA4+46↑j
                INC     $54
                BNE     loc_18D44
                DEC     $54

loc_18D44:                              ; CODE XREF: sub_18CA4+9C↑j
                LDA     $F7
                ORA     $F8
                AND     #$40 ; '@'
                BNE     loc_18D52
                LDA     #1
                STA     $38
                BNE     loc_18DAE

loc_18D52:                              ; CODE XREF: sub_18CA4+A6↑j
                DEC     $38
                BNE     loc_18DAE
                LDA     $51
                STA     $38
                LDA     $54
                CMP     #$12
                BCC     loc_18D64
                LDA     #1
                BNE     loc_18D6A

loc_18D64:                              ; CODE XREF: sub_18CA4+BA↑j
                SBC     #1
                TAY
                LDA     $8E91,Y

loc_18D6A:                              ; CODE XREF: sub_18CA4+BE↑j
                PHA
                CLC
                ADC     $48
                STA     $48
                BCC     loc_18D75
                JSR     sub_18788

loc_18D75:                              ; CODE XREF: sub_18CA4+CC↑j
                PLA
                CLC
                ADC     $4A
                STA     $4A
                BCC     loc_18D80
                JSR     sub_1879F

loc_18D80:                              ; CODE XREF: sub_18CA4+D7↑j
                LDA     #0
                STA     $54
                INC     $56
                BNE     loc_18D8A
                DEC     $56

loc_18D8A:                              ; CODE XREF: sub_18CA4+E2↑j
                LDY     #1
                LDX     $35

loc_18D8E:                              ; CODE XREF: sub_18CA4+F1↓j
                LDA     $528,Y
                BEQ     loc_18D99
                INY
                DEX
                BNE     loc_18D8E
                BEQ     loc_18DAC

loc_18D99:                              ; CODE XREF: sub_18CA4+ED↑j
                LDA     #2
                STA     $528,Y
                LDA     $542
                STA     $542,Y
                LDA     $55C
                STA     $55C,Y
                INC     $55

loc_18DAC:                              ; CODE XREF: sub_18CA4+F3↑j
                LDX     $40

loc_18DAE:                              ; CODE XREF: sub_18CA4+AC↑j
                                        ; sub_18CA4+B0↑j
                LDA     $F7
                ORA     $F8
                AND     #$80
                BEQ     loc_18DD1
                LDA     $52C
                BNE     loc_18DD1
                LDA     $34
                STA     $782
                LDA     #3
                STA     $52C
                LDA     $542
                STA     $546
                LDA     $55C
                STA     $560

loc_18DD1:                              ; CODE XREF: sub_18CA4+110↑j
                                        ; sub_18CA4+115↑j
                LDA     $5AA
                BPL     loc_18DEB
                LDA     $764
                AND     #3
                STA     $590
                DEC     $764
                BNE     loc_18DEB
                LDA     #0
                STA     $590
                STA     $5AA

loc_18DEB:                              ; CODE XREF: sub_18CA4+130↑j
                                        ; sub_18CA4+13D↑j
                JSR     sub_181A7
                JSR     sub_182D9
                LDA     $5C
                BEQ     locret_18DF9
                CMP     #5
                BNE     loc_18DFA

locret_18DF9:                           ; CODE XREF: sub_18CA4+14F↑j
                RTS
; ---------------------------------------------------------------------------

loc_18DFA:                              ; CODE XREF: sub_18CA4+153↑j
                LDY     $52C
                BMI     loc_18E17
                CPY     #$13
                BEQ     loc_18E17
                CMP     #4
                BNE     loc_18E08
                RTS
; ---------------------------------------------------------------------------

loc_18E08:                              ; CODE XREF: sub_18CA4+161↑j
                CMP     #1
                BEQ     loc_18E10
                CMP     #6
                BNE     loc_18E17

loc_18E10:                              ; CODE XREF: sub_18CA4+166↑j
                LDA     $5E
                BNE     loc_18E17
                JMP     sub_1948B
; ---------------------------------------------------------------------------

loc_18E17:                              ; CODE XREF: sub_18CA4+159↑j
                                        ; sub_18CA4+15D↑j ...
                LDA     #$84
                STA     $18
                LDA     #0
                STA     $19
                LDA     #$BC
                STA     $84
                LDA     #$D6
                STA     $87
                LDA     $5E
; End of function sub_18CA4


; =============== S U B R O U T I N E =======================================


sub_18E29:                              ; CODE XREF: sub_1858B+33↑j
                                        ; sub_18835+E5↑j
                LDY     #0
                STY     $86
                STA     $15
                SEC
                SBC     #$64 ; 'd'
                BCC     loc_18E47
                LDY     #2
                CMP     #$64 ; 'd'
                BCC     loc_18E3E
                SBC     #$64 ; 'd'
                LDY     #4

loc_18E3E:                              ; CODE XREF: sub_18E29+F↑j
                STY     $85
                STA     $15
                LDY     #0
                JSR     sub_185D8

loc_18E47:                              ; CODE XREF: sub_18E29+9↑j
                LDA     $87
                CLC
                ADC     #9
                STA     $87
                LDY     #0
                LDA     $15

loc_18E52:                              ; CODE XREF: sub_18E29+31↓j
                CMP     #$A
                BCC     loc_18E5C
                SBC     #$A
                INY
                INY
                BCS     loc_18E52

loc_18E5C:                              ; CODE XREF: sub_18E29+2B↑j
                STY     $85
                PHA
                LDY     #0
                JSR     sub_185D8
                PLA
                ASL
                STA     $85
                LDA     $87
                CLC
                ADC     #9
                STA     $87
                LDY     #0
                JMP     sub_185D8
; End of function sub_18E29

; ---------------------------------------------------------------------------
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   1
                .BYTE   1
                .BYTE   6
                .BYTE   8
                .BYTE  $A
                .BYTE   4
                .BYTE  $C
                .BYTE  $C
                .BYTE   2
                .BYTE   0
                .BYTE  $E
unk_18E86:      .BYTE $30 ; 0
                .BYTE $31 ; 1
                .BYTE $31 ; 1
                .BYTE $31 ; 1
                .BYTE $31 ; 1
                .BYTE $32 ; 2
                .BYTE $33 ; 3
                .BYTE $33 ; 3
                .BYTE $33 ; 3
                .BYTE $33 ; 3
                .BYTE $34 ; 4
                .BYTE $20
                .BYTE $10
                .BYTE  $A
                .BYTE   8
                .BYTE   6
                .BYTE   5
                .BYTE   4
                .BYTE   4
                .BYTE   3
                .BYTE   3
                .BYTE   2
                .BYTE   2
                .BYTE   2
                .BYTE   2
                .BYTE   2
                .BYTE   2

; =============== S U B R O U T I N E =======================================

; Takes three numbers from the array (addressed by the variable $33) and places them in $37,$35,$36

set_default_weapon:                     ; CODE XREF: j_set_default_weapon↑j
                                        ; sub_18F78+A6↓j ...
                LDA     $33
                ASL
                CLC
                ADC     $33
                TAY
                LDA     byte_18EB8,Y
                STA     $37
                LDA     byte_18EB8+1,Y
                STA     $35
                LDA     byte_18EB8+2,Y
                STA     $36
                RTS
; End of function set_default_weapon

; ---------------------------------------------------------------------------
byte_18EB8:     .BYTE $FB, 2, 1
                .BYTE $F9, 3, 1
                .BYTE $F8, 2, 2
                .BYTE $F8, 3, 2
                .BYTE $F7, 2, 3
                .BYTE $F7, 3, 3

; =============== S U B R O U T I N E =======================================


sub_18ECA:                              ; CODE XREF: sub_18CA4+4↑j
                BMI     loc_18EDA
                ORA     #$80
                STA     $31
                LDA     #0
                STA     $590
                LDA     #$32 ; '2'
                STA     $576

loc_18EDA:                              ; CODE XREF: sub_18ECA↑j
                LDA     $31
                CMP     #$82
                BEQ     loc_18F16
                LDA     #$E1
                CLC
                ADC     $542
                ROR
                CLC
                ADC     $542
                ROR
                CMP     $542
                BEQ     loc_18EF7
                STA     $542
                JMP     loc_18F10
; ---------------------------------------------------------------------------

loc_18EF7:                              ; CODE XREF: sub_18ECA+25↑j
                INC     $31
                LDA     #0
                STA     $5F8
                STA     $612
                STA     $6E2
                LDA     #9
                STA     $660
                LDA     #8
                STA     $716
                BNE     loc_18F16

loc_18F10:                              ; CODE XREF: sub_18ECA+2A↑j
                JSR     sub_181A7
                JMP     sub_182D9
; ---------------------------------------------------------------------------

loc_18F16:                              ; CODE XREF: sub_18ECA+14↑j
                                        ; sub_18ECA+44↑j
                JSR     sub_181A7
                JSR     sub_182B1
                LDA     $5F8
                STA     $16
                LDA     $612
                STA     $17
                ASL     $16
                ROL     $17
                ASL     $16
                ROL     $17
                LDA     #$18
                STA     $14
                LDA     #0
                STA     $15
                STA     $86
                LDA     $542
                STA     $84
                LDA     #$84
                STA     $18
                LDA     #0
                STA     $19

loc_18F45:                              ; CODE XREF: sub_18ECA+AB↓j
                LDA     $15
                CLC
                ADC     $16
                STA     $15
                LDA     $84
                ADC     $17
                STA     $84
                BCC     locret_18F77
                LDA     #$14
                STA     $85
                LDA     $55C
                SEC
                SBC     #8
                STA     $87
                LDY     #0
                JSR     sub_185D8
                LDA     #$16
                STA     $85
                LDA     $55C
                STA     $87
                LDY     #0
                JSR     sub_185D8
                DEC     $14
                BNE     loc_18F45

locret_18F77:                           ; CODE XREF: sub_18ECA+88↑j
                RTS
; End of function sub_18ECA


; =============== S U B R O U T I N E =======================================


sub_18F78:
                LDA     $528
                BMI     loc_18FF6
                ORA     #$80
                STA     $528
                LDA     $5AA
                BPL     loc_18F8F
                LDA     #$81
                STA     $528
                JMP     sub_18CA4
; ---------------------------------------------------------------------------

loc_18F8F:                              ; CODE XREF: sub_18F78+D↑j
                DEC     $32
                BNE     loc_18F96
                JSR     $C063

loc_18F96:                              ; CODE XREF: sub_18F78+19↑j
                JSR     sub_1948B
                LSR     $4B
                LSR     $47
                JSR     $C07B
                LDA     #$10
                JSR     $C030
                LDA     #$38 ; '8'
                LDY     #$18
                JSR     sub_1A2CB
                LDA     #$10
                STA     $16
                JSR     sub_1872B
                STA     $17

loc_18FB5:                              ; CODE XREF: sub_18F78+7C↓j
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_18FF6
                LDA     #$A3
                JSR     sub_1B631
                TXA
                PHA
                TYA
                TAX
                LDA     #3
                STA     $660,X
                LDA     #2
                STA     $74A,X
                LDA     #0
                STA     $7E6,X
                LDA     $17
                JSR     sub_18097
                JSR     sub_1872B
                PHA
                AND     #3
                STA     $590,X
                PLA
                AND     #$1F
                ADC     #$E
                LDY     #7
                JSR     sub_1A2CB
                PLA
                TAX
                INC     $17
                INC     $17
                INC     $17
                DEC     $16
                BNE     loc_18FB5

loc_18FF6:                              ; CODE XREF: sub_18F78+3↑j
                                        ; sub_18F78+40↑j
                JSR     sub_183BC
                BCS     loc_1900F
                CPY     #$C
                BCC     loc_19000
                RTS
; ---------------------------------------------------------------------------

loc_19000:                              ; CODE XREF: sub_18F78+85↑j
                LDA     unk_19021,Y
                STA     $576
                LDA     unk_1902D,Y
                STA     $590
                JMP     sub_182D9
; ---------------------------------------------------------------------------

loc_1900F:                              ; CODE XREF: sub_18F78+81↑j
                LDA     $30
                ORA     #1
                STA     $30
                LDA     #0
                STA     $528
                STA     $49
                STA     $33
                JMP     set_default_weapon ; Takes three numbers from the array (addressed by the variable $33) and places them in $37,$35,$36
; End of function sub_18F78

; ---------------------------------------------------------------------------
unk_19021:      .BYTE   5
                .BYTE   6
                .BYTE   6
                .BYTE   7
                .BYTE   7
                .BYTE   8
                .BYTE   8
                .BYTE   7
                .BYTE   7
                .BYTE   6
                .BYTE   6
                .BYTE   5
unk_1902D:      .BYTE   0
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   0
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   0
                .BYTE   1
                .BYTE   2
                .BYTE   3

; =============== S U B R O U T I N E =======================================


sub_19039:
                LDA     $528,X
                BMI     loc_19068
                ORA     #$80
                STA     $528,X
                LDA     $36
                STA     $576,X
                LDA     $33
                LSR
                CLC
                ADC     #$D
                JSR     sub_C030
                LDA     #0
                STA     $590,X
                LDA     #0
                STA     $5AA,X
                STA     $5F8,X
                LDA     #1
                STA     $660,X
                LDA     $37
                STA     $612,X

loc_19068:                              ; CODE XREF: sub_19039+3↑j
                JSR     sub_182B1
                JMP     sub_181A7
; End of function sub_19039


; =============== S U B R O U T I N E =======================================


sub_1906E:                              ; CODE XREF: sub_1916A↓p
                                        ; sub_191EB+B↓p ...
                DEC     $768
                BNE     locret_1907E
                LDA     #$3C ; '<'
                STA     $768
                DEC     $5E
                LDA     $5E
                CMP     #$FF

locret_1907E:                           ; CODE XREF: sub_1906E+3↑j
                RTS
; End of function sub_1906E


; =============== S U B R O U T I N E =======================================


sub_1907F:
                LDA     $52C
                BMI     sub_190DD
                ORA     #$80
                STA     $52C
                LDA     $60
                CMP     #5
                BNE     loc_19099
                LDA     $5C
                CMP     #5
                BNE     loc_19099
                LDA     #$18
                BNE     loc_1909F

loc_19099:                              ; CODE XREF: sub_1907F+E↑j
                                        ; sub_1907F+14↑j
                LDA     #6
                CMP     $5C
                BEQ     loc_190A2

loc_1909F:                              ; CODE XREF: sub_1907F+18↑j
                JSR     sub_C030

loc_190A2:                              ; CODE XREF: sub_1907F+1E↑j
                LDA     $5C
                ASL
                ASL
                ASL
                TAY
                LDA     byte_190F2,Y
                STA     $664
                LDA     unk_190F3,Y
                STA     $594
                TYA
                CLC
                ADC     $61
                TAY
                LDA     unk_190F4,Y
                STA     $57A
                LDA     unk_190F7,Y
                STA     $616
                STA     $74E
                LDA     $5C
                JSR     j_switch_case
; ---------------------------------------------------------------------------
                .WORD $9139
                .WORD $91A0
                .WORD $91E5
                .WORD $925B
                .WORD $92F0
                .WORD $935B
                .WORD $940F
                .WORD $914C
; End of function sub_1907F


; =============== S U B R O U T I N E =======================================


sub_190DD:                              ; CODE XREF: sub_1907F+3↑j
                                        ; sub_1914D+1A↓j
                LDA     $5C
                JSR     j_switch_case
; End of function sub_190DD

; ---------------------------------------------------------------------------
                .WORD $919A
                .WORD $9171
                .WORD $91EA
                .WORD $9277
                .WORD $932E
                .WORD $9371
                .WORD $942E
                .WORD $9169
byte_190F2:     .BYTE 3
unk_190F3:      .BYTE   2
unk_190F4:      .BYTE $36 ; 6
                .BYTE $38 ; 8
                .BYTE $3A
unk_190F7:      .BYTE $83
                .BYTE $C1
                .BYTE $C1
                .BYTE   1
                .BYTE   0
                .BYTE $36 ; 6
                .BYTE $38 ; 8
                .BYTE $40 ; @
                .BYTE $FE
                .BYTE $FE
                .BYTE $FC
                .BYTE   0
                .BYTE   0
                .BYTE $3D ; =
                .BYTE $41 ; A
                .BYTE $42 ; B
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   3
                .BYTE $36 ; 6
                .BYTE $38 ; 8
                .BYTE $3A ; :
                .BYTE $85
                .BYTE $85
                .BYTE $85
                .BYTE  $B
                .BYTE   1
                .BYTE $3E ; >
                .BYTE $3F ; ?
                .BYTE $40 ; @
                .BYTE $FE
                .BYTE $FE
                .BYTE $FE
                .BYTE   9
                .BYTE   3
                .BYTE $36 ; 6
                .BYTE $38 ; 8
                .BYTE $3A ; :
                .BYTE $FE
                .BYTE $FE
                .BYTE $FE
                .BYTE   1
                .BYTE   3
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE   3
                .BYTE   1
                .BYTE $36 ; 6
                .BYTE $36 ; 6
                .BYTE $36 ; 6
                .BYTE $C3
                .BYTE $C2
                .BYTE $85
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6

; =============== S U B R O U T I N E =======================================


sub_1913A:
                LDA     $782
                TAY
                LDA     $8E7D,Y
                LDY     $60
                CPY     #4
                BCC     loc_19164
                LDY     #3
                STY     $5F
                BNE     loc_19164
; End of function sub_1913A


; =============== S U B R O U T I N E =======================================


sub_1914D:
                LDA     $60
                CMP     #3
                BCC     loc_1915D
                LDA     #$40 ; '@'
                STA     $57A
                LDA     #$C3
                STA     $74E

loc_1915D:                              ; CODE XREF: sub_1914D+4↑j
                LDA     $782
                TAY
                LDA     $91DD,Y

loc_19164:                              ; CODE XREF: sub_1913A+B↑j
                                        ; sub_1913A+11↑j
                JSR     sub_18097
                JMP     sub_190DD
; End of function sub_1914D


; =============== S U B R O U T I N E =======================================


sub_1916A:
                JSR     sub_1906E
                BNE     sub_1919B
                JMP     sub_1948B
; End of function sub_1916A


; =============== S U B R O U T I N E =======================================


sub_19172:
                LDA     $60
                CMP     #5
                BNE     loc_1918F
                LDA     $6B2
                JSR     sub_18097
                LDA     $5FC
                CLC
                ADC     #$80
                STA     $5FC
                BCS     loc_1918C
                DEC     $616

loc_1918C:                              ; CODE XREF: sub_19172+15↑j
                DEC     $6B2

loc_1918F:                              ; CODE XREF: sub_19172+4↑j
                LDA     $57A
                CMP     #$40 ; '@'
                BEQ     sub_1919B
                EOR     #1
                STA     $57A
; End of function sub_19172


; =============== S U B R O U T I N E =======================================


sub_1919B:                              ; CODE XREF: sub_1916A+3↑j
                                        ; sub_19172+22↑j ...
                JSR     sub_182B1
                JMP     sub_181A7
; End of function sub_1919B


; =============== S U B R O U T I N E =======================================


sub_191A1:
                DEC     $5E
                LDA     $60
                CMP     #5
                BNE     sub_1919B
                LDA     $560
                SEC
                SBC     #8
                CMP     #$D0
                BCC     loc_191B7
                LDA     #$D0
                BNE     loc_191BD

loc_191B7:                              ; CODE XREF: sub_191A1+10↑j
                CMP     #$20 ; ' '
                BCS     loc_191BD
                LDA     #$20 ; ' '

loc_191BD:                              ; CODE XREF: sub_191A1+14↑j
                                        ; sub_191A1+18↑j
                STA     $560
                LDA     $546
                CMP     #$68 ; 'h'
                BCS     loc_191CC
                LDA     #$68 ; 'h'
                STA     $546

loc_191CC:                              ; CODE XREF: sub_191A1+24↑j
                LDA     #3
                STA     $664
                LDA     #0
                STA     $6B2
                LDA     #$C1
                STA     $74E
                BNE     sub_1919B
; End of function sub_191A1

; ---------------------------------------------------------------------------
                .BYTE  $B
                .BYTE  $B
                .BYTE  $B
                .BYTE  $C
                .BYTE  $C
                .BYTE  $C
                .BYTE  $D
                .BYTE  $D
                .BYTE  $D

; =============== S U B R O U T I N E =======================================


sub_191E6:
                LDA     #$3C ; '<'
                STA     $768
; End of function sub_191E6


; =============== S U B R O U T I N E =======================================


sub_191EB:                              ; CODE XREF: sub_19410+5C↓j
                LDA     $60
                CMP     #5
                BNE     loc_1921E
                LDA     #$80
                STA     $5AA
                JSR     sub_1906E
                BNE     loc_191FE
                JMP     sub_1948B
; ---------------------------------------------------------------------------

loc_191FE:                              ; CODE XREF: sub_191EB+E↑j
                LDA     $768
                ORA     #$80
                STA     $764
                LDA     #0
                STA     $5F
                LDA     $542
                STA     $546
                LDA     $55C
                STA     $560
                LDA     #$40 ; '@'
                STA     $576,X
                JMP     sub_182D9
; ---------------------------------------------------------------------------

loc_1921E:                              ; CODE XREF: sub_191EB+4↑j
                LDA     $542
                SEC
                SBC     #8
                STA     $546
                LDA     $55C
                STA     $560
                DEC     $768
                LDA     $5E
                LSR
                LSR
                LSR
                LSR
                CMP     #8
                BCC     loc_1923C
                LDA     #7

loc_1923C:                              ; CODE XREF: sub_191EB+4D↑j
                STA     $10
                LDA     $768
                AND     #1
                ADC     $10
                TAY
                LDA     $9252,Y
                STA     $57A
                JSR     sub_182D9
                JMP     sub_181A7
; End of function sub_191EB

; ---------------------------------------------------------------------------
                .BYTE $3C ; <
                .BYTE $3C ; <
                .BYTE $3D ; =
                .BYTE $3D ; =
                .BYTE $3E ; >
                .BYTE $3E ; >
                .BYTE $41 ; A
                .BYTE $41 ; A
                .BYTE $42 ; B
                .BYTE $42 ; B

; =============== S U B R O U T I N E =======================================


sub_1925C:
                LDA     #0
                STA     $6B2
                STA     $67E
                LDA     #$F6
                STA     $6CC
                LDA     #$C0
                STA     $698
                LDA     #1
                STA     $6E6
                LDA     #$FF
                STA     $700
; End of function sub_1925C


; =============== S U B R O U T I N E =======================================


sub_19278:
                JSR     sub_1906E
                BNE     loc_19280
                JMP     sub_1948B
; ---------------------------------------------------------------------------

loc_19280:                              ; CODE XREF: sub_19278+3↑j
                DEC     $6E6
                BNE     loc_19295
                LDA     #1
                STA     $6E6
                INC     $700
                LDA     $700
                AND     #$F
                JSR     sub_18097

loc_19295:                              ; CODE XREF: sub_19278+B↑j
                LDA     $5FC
                CLC
                ADC     $67E
                STA     $67E
                LDA     $616
                ADC     $698
                STA     $698
                LDA     $542
                CMP     #$48 ; 'H'
                BCS     loc_192B3
                LDA     #$48 ; 'H'
                BNE     loc_192B9

loc_192B3:                              ; CODE XREF: sub_19278+35↑j
                CMP     #$C8
                BCC     loc_192B9
                LDA     #$C7

loc_192B9:                              ; CODE XREF: sub_19278+39↑j
                                        ; sub_19278+3D↑j
                CLC
                ADC     $698
                STA     $546
                LDA     $630
                CLC
                ADC     $6B2
                STA     $6B2
                LDA     $64A
                ADC     $6CC
                STA     $6CC
                LDA     $55C
                CMP     #$40 ; '@'
                BCS     loc_192DE
                LDA     #$40 ; '@'
                BNE     loc_192E4

loc_192DE:                              ; CODE XREF: sub_19278+60↑j
                CMP     #$C8
                BCC     loc_192E4
                LDA     #$C7

loc_192E4:                              ; CODE XREF: sub_19278+64↑j
                                        ; sub_19278+68↑j
                CLC
                ADC     $6CC
                STA     $560
                JSR     sub_182D9
                JMP     sub_181A7
; End of function sub_19278


; =============== S U B R O U T I N E =======================================


sub_192F1:
                LDA     #4
                STA     $71A
                LDA     #$FF
                STA     $6E6
                LDA     #$F4
                STA     $64A
                LDA     $55C
                CMP     #$48 ; 'H'
                BCS     loc_1930B
                LDA     #$48 ; 'H'
                BNE     loc_19311

loc_1930B:                              ; CODE XREF: sub_192F1+14↑j
                CMP     #$CA
                BCC     loc_19311
                LDA     #$C9

loc_19311:                              ; CODE XREF: sub_192F1+18↑j
                                        ; sub_192F1+1C↑j
                STA     $700
                CLC
                ADC     #$18
                STA     $560
                LDA     $546
                CMP     #$50 ; 'P'
                BCS     loc_19326
                LDA     #$50 ; 'P'
                STA     $546

loc_19326:                              ; CODE XREF: sub_192F1+2E↑j
                LDA     #$46 ; 'F'
                STA     $74E
                LDA     #$3C ; '<'
                STA     $5E
; End of function sub_192F1


; =============== S U B R O U T I N E =======================================


sub_1932F:                              ; CODE XREF: sub_19410:loc_1945E↓j
                DEC     $74E
                BNE     loc_19339
                LDA     #2
                STA     $664

loc_19339:                              ; CODE XREF: sub_1932F+3↑j
                LDA     $560
                CMP     $700
                BCS     loc_1934D
                LDA     $64A
                CLC
                ADC     #4
                STA     $64A
                JMP     loc_19356
; ---------------------------------------------------------------------------

loc_1934D:                              ; CODE XREF: sub_1932F+10↑j
                LDA     $64A
                SEC
                SBC     #4
                STA     $64A

loc_19356:                              ; CODE XREF: sub_1932F+1B↑j
                JSR     sub_182B1
                JMP     sub_181A7
; End of function sub_1932F


; =============== S U B R O U T I N E =======================================


sub_1935C:
                LDA     #4
                STA     $71A
                LDA     #$FF
                STA     $6E6
                LDA     $546
                SEC
                SBC     #$10
                STA     $7B6
                STA     $7D0
; End of function sub_1935C


; =============== S U B R O U T I N E =======================================


sub_19372:
                LDA     $60
                CMP     #5
                BCS     loc_193A6
                LDA     $546
                CMP     #$10
                BCS     loc_19384
                LDA     #$10
                STA     $546

loc_19384:                              ; CODE XREF: sub_19372+B↑j
                LDA     $55C
                STA     $560
                LDA     $542
                CLC
                ADC     #8
                CMP     $546
                BCS     loc_19398
                JMP     sub_182FE
; ---------------------------------------------------------------------------

loc_19398:                              ; CODE XREF: sub_19372+21↑j
                LDA     $57A
                EOR     #1
                STA     $57A
                JSR     sub_182B1
                JMP     sub_181A7
; ---------------------------------------------------------------------------

loc_193A6:                              ; CODE XREF: sub_19372+4↑j
                LDA     $55C
                SEC
                SBC     #8
                STA     $782
                CLC
                ADC     #$10
                STA     $79C
                LDA     $7B6
                CMP     #$10
                BCC     loc_193C9
                LDA     $7D0
                SEC
                SBC     $7B6
                BCC     loc_1940D
                CMP     $77
                BCC     loc_193D4

loc_193C9:                              ; CODE XREF: sub_19372+48↑j
                LDA     $7D0
                SEC
                SBC     #$C
                BCC     loc_1940D
                STA     $7D0

loc_193D4:                              ; CODE XREF: sub_19372+55↑j
                LDA     $7B6
                SEC
                SBC     #$C
                BCS     loc_193DE
                ADC     #$10

loc_193DE:                              ; CODE XREF: sub_19372+68↑j
                STA     $7B6
                STA     $84
                LDA     $55C
                SEC
                SBC     #4
                STA     $87
                LDA     #$7E ; '~'
                STA     $85
                LDA     #0
                STA     $86
                LDA     #$84
                STA     $18
                LDA     #0
                STA     $19

loc_193FB:                              ; CODE XREF: sub_19372+98↓j
                LDY     #0
                JSR     sub_185D8
                LDA     $84
                CLC
                ADC     #$10
                STA     $84
                CMP     $7D0
                BCC     loc_193FB
                RTS
; ---------------------------------------------------------------------------

loc_1940D:                              ; CODE XREF: sub_19372+51↑j
                                        ; sub_19372+5D↑j
                JMP     sub_182FE
; End of function sub_19372


; =============== S U B R O U T I N E =======================================


sub_19410:
                LDA     $F5
                ORA     $F6
                AND     #$80
                BNE     loc_1941B
                JMP     sub_182FE
; ---------------------------------------------------------------------------

loc_1941B:                              ; CODE XREF: sub_19410+6↑j
                LDA     #6
                JSR     sub_C030
                DEC     $5E
                LDA     $60
                CMP     #5
                BEQ     loc_1942F
                CMP     #2
                BCC     loc_1942F
                JMP     loc_1946F
; ---------------------------------------------------------------------------

loc_1942F:                              ; CODE XREF: sub_19410+16↑j
                                        ; sub_19410+1A↑j
                JSR     sub_182B1
                JMP     sub_181A7
; ---------------------------------------------------------------------------
                LDA     #$83
                STA     $52C
                LDA     $5C
                JSR     j_switch_case
; ---------------------------------------------------------------------------
                .WORD $82FD
                .WORD $9171
                .WORD $9463
                .WORD $9277
                .WORD $944E
                .WORD $9371
                .WORD $946E
                .WORD $9169
; ---------------------------------------------------------------------------
                DEC     $5E
                BEQ     loc_19461
                LDA     $5E
                CMP     #$A
                BNE     loc_1945E
                LDA     #$3C ; '<'
                STA     $57A

loc_1945E:                              ; CODE XREF: sub_19410+47↑j
                JMP     sub_1932F
; ---------------------------------------------------------------------------

loc_19461:                              ; CODE XREF: sub_19410+41↑j
                JMP     sub_182FE
; ---------------------------------------------------------------------------
                DEC     $5E
                LDA     $5E
                CMP     #$FF
                BEQ     sub_1948B
                JMP     sub_191EB
; ---------------------------------------------------------------------------

loc_1946F:                              ; CODE XREF: sub_19410+1C↑j
                LDA     $60
                CMP     #5
                BCC     loc_19480
                LDA     #$3E ; '>'
                JSR     loc_1B2EF
                JSR     sub_1948B
                JMP     loc_19483
; ---------------------------------------------------------------------------

loc_19480:                              ; CODE XREF: sub_19410+63↑j
                JSR     sub_1B2ED

loc_19483:                              ; CODE XREF: sub_19410+6D↑j
                LDA     #$13
                JSR     sub_C030
                JMP     sub_182FE
; End of function sub_19410


; =============== S U B R O U T I N E =======================================


sub_1948B:                              ; CODE XREF: sub_18012↑j
                                        ; sub_18CA4+170↑j ...
                LDA     #$FF
                STA     $5C
                LDA     #0
                STA     $60
; End of function sub_1948B


; =============== S U B R O U T I N E =======================================


sub_19493:                              ; CODE XREF: sub_1800F↑j
                                        ; sub_187C7+58↑p ...
                TAY
                LDA     $9586,Y
                STA     $5F
                LDA     #$14
                STA     $5E
                LDA     $5C
                STY     $5C
                CMP     $5C
                BEQ     loc_194BD
                LDA     $49
                SEC
                SBC     #8
                BCS     loc_194AE
                LDA     #0

loc_194AE:                              ; CODE XREF: sub_19493+17↑j
                STA     $49
                LDA     #$28 ; '('
                STA     $52C
                LDA     #0
                STA     $60
                STA     $61
                BEQ     loc_19515

loc_194BD:                              ; CODE XREF: sub_19493+10↑j
                LDA     $52C
                BEQ     loc_194D2
                TXA
                PHA
                LDX     #4
                JSR     sub_182FE
                PLA
                TAX
                CPY     #3
                BNE     loc_194D2
                STY     $52C

loc_194D2:                              ; CODE XREF: sub_19493+2D↑j
                                        ; sub_19493+3A↑j
                INC     $60
                LDA     $60
                CMP     #3
                BCC     loc_194DC
                LDA     #2

loc_194DC:                              ; CODE XREF: sub_19493+45↑j
                STA     $61
                LDA     $60
                CMP     #5
                BCC     loc_19515
                LDA     $9578,Y
                AND     $5D
                BNE     loc_19511
                LDA     $5D
                ORA     $9578,Y
                STA     $5D
                LDA     #5
                STA     $60
                LDA     $5C
                CMP     #5
                BNE     loc_19507
                LDA     $77
                CLC
                ADC     #$20 ; ' '
                BCS     loc_19515
                STA     $77
                BNE     loc_19515

loc_19507:                              ; CODE XREF: sub_19493+67↑j
                CMP     #6
                BNE     loc_19515
                LDA     #1
                STA     $5E
                BNE     loc_19515

loc_19511:                              ; CODE XREF: sub_19493+56↑j
                LDA     #4
                STA     $60

loc_19515:                              ; CODE XREF: sub_19493+28↑j
                                        ; sub_19493+4F↑j ...
                CPY     #2
                BEQ     loc_19558
                CPY     #1
                BEQ     loc_19553
                LDA     #$3C ; '<'
                STA     $768
                CPY     #7
                BEQ     loc_1954E
                CPY     #3
                BNE     locret_19557
                LDA     #$FA
                STA     $5E
                LDA     $60
                LSR
                STA     $61
                BEQ     locret_19557
                LDA     $91
                LSR
                LSR
                BEQ     locret_19557
                LDY     #$18
                CMP     2
                BCC     loc_1954B
                JSR     sub_1872B
                LDY     #$1B
                LSR
                BCS     loc_1954B
                LDY     #$1E

loc_1954B:                              ; CODE XREF: sub_19493+AC↑j
                                        ; sub_19493+B4↑j
                JMP     loc_19574
; ---------------------------------------------------------------------------

loc_1954E:                              ; CODE XREF: sub_19493+91↑j
                LDA     #$C8
                STA     $5E
                RTS
; ---------------------------------------------------------------------------

loc_19553:                              ; CODE XREF: sub_19493+88↑j
                LDA     #$C8
                STA     $5E

locret_19557:                           ; CODE XREF: sub_19493+95↑j
                                        ; sub_19493+A0↑j ...
                RTS
; ---------------------------------------------------------------------------

loc_19558:                              ; CODE XREF: sub_19493+84↑j
                LDY     $60
                LDA     unk_19580,Y
                STA     $5E
                LDA     #3
                STA     $52C
                LDA     $60
                ASL
                CLC
                ADC     $60
                TAY
                LDA     $91
                CMP     #5
                BCC     loc_19574
                INY
                INY
                INY

loc_19574:                              ; CODE XREF: sub_19493:loc_1954B↑j
                                        ; sub_19493+DC↑j
                TYA
                JMP     sub_19918
; End of function sub_19493

; ---------------------------------------------------------------------------
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   2
                .BYTE   0
unk_19580:      .BYTE $32 ; 2
                .BYTE $50 ; P
                .BYTE $64 ; d
                .BYTE $78 ; x
                .BYTE $8C
                .BYTE $50 ; P
                .BYTE   2
                .BYTE   3
                .BYTE   1
                .BYTE   1
                .BYTE   3
                .BYTE   3
                .BYTE   3
                .BYTE   3

; =============== S U B R O U T I N E =======================================


sub_1958E:
                JSR     sub_1872B
                AND     #$7F
                ADC     #$20 ; ' '
                STA     $14
                LDA     $17E
                JSR     sub_19615
                STA     $15
                LDA     $17D
                JSR     sub_19615
                STA     $16
                LDA     #3
                STA     $17

loc_195AB:                              ; CODE XREF: sub_1958E+46↓j
                LDY     $15
                LDA     unk_195D9,Y
                INC     $15
                STA     $528,X
                LDA     $14
                STA     $55C,X
                CLC
                ADC     #$20 ; ' '
                STA     $14
                LDY     $16
                LDA     unk_195F7,Y
                STA     $542,X
                INC     $16
                DEC     $17
                BEQ     loc_195D6
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_195D6
                TYA
                TAX
                BCC     loc_195AB

loc_195D6:                              ; CODE XREF: sub_1958E+3D↑j
                                        ; sub_1958E+42↑j
                LDX     $40
                RTS
; End of function sub_1958E

; ---------------------------------------------------------------------------
unk_195D9:      .BYTE   4
                .BYTE   6
                .BYTE   5
                .BYTE   4
                .BYTE   5
                .BYTE   6
                .BYTE   6
                .BYTE   4
                .BYTE   6
                .BYTE   5
                .BYTE   5
                .BYTE   5
                .BYTE   4
                .BYTE   6
                .BYTE   4
                .BYTE   6
                .BYTE   4
                .BYTE   4
                .BYTE   6
                .BYTE   5
                .BYTE   4
                .BYTE   4
                .BYTE   5
                .BYTE   6
                .BYTE   5
                .BYTE   4
                .BYTE   6
                .BYTE   4
                .BYTE   4
                .BYTE   6
unk_195F7:      .BYTE   1
                .BYTE $21 ; !
                .BYTE   1
                .BYTE $21 ; !
                .BYTE   1
                .BYTE $21 ; !
                .BYTE   1
                .BYTE $21 ; !
                .BYTE $41 ; A
                .BYTE $41 ; A
                .BYTE $21 ; !
                .BYTE   1
                .BYTE $11
                .BYTE   1
                .BYTE $11
                .BYTE   1
                .BYTE $11
                .BYTE   1
                .BYTE $41 ; A
                .BYTE   1
                .BYTE $41 ; A
                .BYTE   1
                .BYTE $41 ; A
                .BYTE   1
                .BYTE   1
                .BYTE $11
                .BYTE $21 ; !
                .BYTE $21 ; !
                .BYTE $11
                .BYTE   1

; =============== S U B R O U T I N E =======================================


sub_19615:                              ; CODE XREF: sub_1958E+C↑p
                                        ; sub_1958E+14↑p
                STA     $10
                ASL
                ADC     $10
                RTS
; End of function sub_19615


; =============== S U B R O U T I N E =======================================


sub_1961B:
                LDA     $528,X
                BMI     loc_19644
                DEC     $542,X
                BEQ     loc_19626
                RTS
; ---------------------------------------------------------------------------

loc_19626:                              ; CODE XREF: sub_1961B+8↑j
                ORA     #$80
                STA     $528,X
                LDA     #$2F ; '/'
                STA     $576,X
                LDA     #2
                STA     $590,X
                LDA     #5
                STA     $764,X
                LDA     #$C0
                STA     $5F8,X
                LDA     #1
                STA     $660,X

loc_19644:                              ; CODE XREF: sub_1961B+3↑j
                LDA     $764,X
                CMP     #3
                BCS     loc_1966C
                INC     $6AE,X
                LDA     $6AE,X
                AND     #1
                ASL
                STA     $10
                LDA     $528,X
                AND     #$7F
                ASL
                ASL
                ADC     $10
                TAY
                LDA     $96CC,Y
                STA     $576,X
                LDA     $96CD,Y
                STA     $590,X

loc_1966C:                              ; CODE XREF: sub_1961B+2E↑j
                JSR     sub_182B1
                LDA     #$2F ; '/'
                STA     $576,X
                JSR     sub_181F6
                JSR     sub_1A2B1
                LDA     $8A
                BNE     loc_196A2
                LDA     $88
                CMP     #6
                BNE     loc_196A2
                LDA     #$3F ; '?'
                STA     $7E6,X
                LDA     $764,X
                CMP     #3
                BCC     loc_1969C
                LDY     #4

loc_19692:                              ; CODE XREF: sub_1961B+7F↓j
                TYA
                PHA
                JSR     sub_196F3
                PLA
                TAY
                DEY
                BNE     loc_19692

loc_1969C:                              ; CODE XREF: sub_1961B+73↑j
                JSR     sub_196F3
                JMP     sub_182FE
; ---------------------------------------------------------------------------

loc_196A2:                              ; CODE XREF: sub_1961B+61↑j
                                        ; sub_1961B+67↑j
                LDA     $764,X
                BNE     locret_196DB
                LDA     $88
                CMP     #5
                BEQ     locret_196DB
                CMP     #4
                BEQ     loc_196C3
                LDA     #$BF
                STA     $528,X
                LDA     #1
                STA     $590,X
                LDA     #$2E ; '.'
                STA     $576,X
                JMP     loc_196E8
; ---------------------------------------------------------------------------

loc_196C3:                              ; CODE XREF: sub_1961B+94↑j
                LDA     #$26 ; '&'
                STA     $528,X
                LDA     #3
                STA     $77E,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_196DB
                LDA     #5
                STA     $14
                LDA     #$26 ; '&'
                JMP     sub_1B631
; ---------------------------------------------------------------------------

locret_196DB:                           ; CODE XREF: sub_1961B+8A↑j
                                        ; sub_1961B+90↑j ...
                RTS
; ---------------------------------------------------------------------------
                .BYTE $2F ; /
                .BYTE $80
                .BYTE $2F ; /
                .BYTE   2
                .BYTE $2F ; /
                .BYTE   1
                .BYTE $2F ; /
                .BYTE   2
                .BYTE $2E ; .
                .BYTE   1
                .BYTE $2F ; /
                .BYTE   2
; ---------------------------------------------------------------------------

loc_196E8:                              ; CODE XREF: sub_1961B+A5↑j
                JSR     sub_182B1
                JSR     sub_181ED
                LDA     $528,X
                BMI     locret_196DB
; End of function sub_1961B


; =============== S U B R O U T I N E =======================================


sub_196F3:                              ; CODE XREF: sub_1961B+79↑p
                                        ; sub_1961B:loc_1969C↑p
                JSR     sub_C02A
                LDA     #$17
                JSR     sub_C030
                LDA     #$81
                STA     $528
                LDA     $5AA
                ORA     #$80
                STA     $5AA
                LDA     #$40 ; '@'
                STA     $764
                INC     $33
                LDA     $33
                CMP     #6
                BCC     loc_1973C
                DEC     $33
                INC     $5A
                LDA     $5A
                AND     #7
                BNE     loc_19725
                JSR     sub_187A6
                JSR     sub_187A6

loc_19725:                              ; CODE XREF: sub_196F3+2A↑j
                LDA     $5A
                CMP     #$D
                BNE     locret_196DB
                LDA     $5C
                BNE     locret_196DB

loc_1972F:                              ; CODE XREF: sub_1ACDE+3F↓j
                LDA     #4
                STA     $36
                LDA     #$F5
                STA     $37
                LDA     #3
                STA     $35
                RTS
; ---------------------------------------------------------------------------

loc_1973C:                              ; CODE XREF: sub_196F3+20↑j
                JMP     set_default_weapon ; Takes three numbers from the array (addressed by the variable $33) and places them in $37,$35,$36
; End of function sub_196F3


; =============== S U B R O U T I N E =======================================


sub_1973F:
                LDA     $528,X
                BMI     loc_19777

loc_19744:                              ; CODE XREF: sub_197FD+5↓j
                ORA     #$80
                STA     $528,X
                LDA     #$80
                STA     $55C,X
                LDA     #3
                STA     $612,X
                LDA     #$10
                STA     $716,X
                LDA     #9
                STA     $660,X
                LDA     #0
                STA     $6E2,X
                LDA     $528,X
                TAY
                LDA     $973A,Y
                STA     $576,X
                LDA     $973D,Y
                STA     $590,X
                LDA     #8
                STA     $6FC,X

loc_19777:                              ; CODE XREF: sub_1973F+3↑j
                LDA     $612,X
                BEQ     loc_19787
                CMP     #$FF
                BNE     loc_197B4
                LDA     #$19
                STA     $576,X
                BNE     loc_197B4

loc_19787:                              ; CODE XREF: sub_1973F+3B↑j
                LDA     #$1A
                STA     $576,X
                LDA     $5F8,X
                ORA     $612,X
                BNE     loc_197B4
                LDA     $528,X
                CMP     #$88
                BEQ     loc_197CF
                LDA     #7
                STA     $15

loc_1979F:                              ; CODE XREF: sub_1973F+73↓j
                LDY     $15
                LDA     $97B9,Y
                STA     $14
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_197B4
                LDA     #$26 ; '&'
                JSR     sub_1B631
                DEC     $15
                BNE     loc_1979F

loc_197B4:                              ; CODE XREF: sub_1973F+3F↑j
                                        ; sub_1973F+46↑j ...
                JSR     sub_182B1
                JMP     sub_181F6
; ---------------------------------------------------------------------------
                .BYTE   1
                .BYTE   6
                .BYTE   3
                .BYTE   7
                .BYTE   2
                .BYTE   5
                .BYTE   4
                .BYTE $19
                .BYTE $19
                .BYTE $1A
                .BYTE   2
                .BYTE   0
                .BYTE   3
                .BYTE $BD
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $30 ; 0
                .BYTE $AB
                .BYTE $4C ; L
                .BYTE $44 ; D
                .BYTE $97
; ---------------------------------------------------------------------------

loc_197CF:                              ; CODE XREF: sub_1973F+5A↑j
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_197B4
                LDA     #$28 ; '('
                STA     $528,Y
                STY     $15
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_197B4
                LDA     #5
                STA     $14
                JSR     sub_197F3
                LDY     $15
                LDA     #$83
                STA     $14
                JSR     sub_197F3
                JMP     loc_197B4
; End of function sub_1973F


; =============== S U B R O U T I N E =======================================


sub_197F3:                              ; CODE XREF: sub_1973F+A5↑p
                                        ; sub_1973F+AE↑p ...
                LDA     #0
                STA     $74A,Y
                LDA     #$29 ; ')'
                JMP     sub_1B631
; End of function sub_197F3


; =============== S U B R O U T I N E =======================================


sub_197FD:
                LDA     $528,X
                BMI     loc_19805
                JMP     loc_19744
; ---------------------------------------------------------------------------

loc_19805:                              ; CODE XREF: sub_197FD+3↑j
                DEC     $6FC,X
                BNE     loc_197B4
                LDA     #8
                STA     $6FC,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_197B4
                JSR     sub_1981D
                JSR     sub_1B631
                JMP     loc_197B4
; End of function sub_197FD


; =============== S U B R O U T I N E =======================================


sub_1981D:                              ; CODE XREF: sub_197FD+17↑p
                                        ; sub_1A3EC+DD↓p
                JSR     sub_1872B
                AND     #3
                SBC     #1
                STA     $14
                LDA     #$14
                RTS
; End of function sub_1981D


; =============== S U B R O U T I N E =======================================


sub_19829:
                LDA     $528,X
                BMI     loc_1985D
                ORA     #$80
                STA     $528,X
                JSR     sub_18746
                LDY     #0
                LDA     $55C,X
                CMP     #$80
                BCS     loc_19840
                DEY

loc_19840:                              ; CODE XREF: sub_19829+14↑j
                TYA
                STA     $6FC,X
                LDA     #8
                STA     $730,X
                LDA     #$2C ; ','
                STA     $576,X
                LDA     #2
                STA     $590,X
                LDA     #$13
                STA     $660,X
                LDA     #3
                STA     $612,X

loc_1985D:                              ; CODE XREF: sub_19829+3↑j
                JMP     loc_197B4
; End of function sub_19829


; =============== S U B R O U T I N E =======================================


sub_19860:                              ; CODE XREF: sub_198C6+1E↓j
                LDA     $528,X
                BMI     loc_19892
                ORA     #$80
                STA     $528,X
                LDA     #1
                STA     $74A,X
                JSR     sub_18746
                LDA     #0
                STA     $542,X
                LDA     #2
                STA     $62C,X
                LDA     #3
                STA     $646,X
                LDA     $55C,X
                CMP     #$80
                BCC     loc_19892
                LDA     #$FE
                STA     $62C,X
                LDA     #5
                STA     $646,X

loc_19892:                              ; CODE XREF: sub_19860+3↑j
                                        ; sub_19860+26↑j
                LDA     $62
                AND     #8
                BNE     locret_198C5
                DEC     $74A,X
                BNE     locret_198C5
                LDA     $660,X
                STA     $74A,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_198C5
                LDA     $646,X
                STA     $14
                LDA     $576,X
                JSR     sub_1B631
                DEC     $590,X
                BNE     loc_198BB
                JMP     sub_182FE
; ---------------------------------------------------------------------------

loc_198BB:                              ; CODE XREF: sub_19860+56↑j
                LDA     $55C,X
                CLC
                ADC     $62C,X
                STA     $55C,X

locret_198C5:                           ; CODE XREF: sub_19860+36↑j
                                        ; sub_19860+3B↑j ...
                RTS
; End of function sub_19860


; =============== S U B R O U T I N E =======================================


sub_198C6:
                LDA     $49
                LSR
                LSR
                LSR
                AND     #$E
                TAY
                LDA     #$45 ; 'E'
                STA     $528,X
                LDA     word_198E7,Y
                STA     $576,X
                LDA     word_198E7+1,Y
                STA     $590,X
                LDA     #$28 ; '('
                STA     $660,X
                JMP     sub_19860
; End of function sub_198C6

; ---------------------------------------------------------------------------
word_198E7:     .WORD $1E0A
                .WORD $810
                .WORD $A16
                .WORD $817
                .WORD $630
                .WORD $808
                .WORD $641
                .WORD $1E24
unk_198F7:      .BYTE $1E
byte_198F8:     .BYTE $A
unk_198F9:      .BYTE $1E
                .BYTE $42
                .BYTE   2
                .BYTE $78
                .BYTE $3A ; :
                .BYTE $1E
                .BYTE $3C ; <
                .BYTE $A
                .BYTE $30 ; 0
                .BYTE $3C
                .BYTE  $F
                .BYTE $A
                .BYTE $C8
                .BYTE $43
                .BYTE  $A
                .BYTE $50
                .BYTE $42 ; B
                .BYTE $A
                .BYTE $50 ; P
                .BYTE 8
                .BYTE  $A
                .BYTE $14
                .BYTE $43 ; C
                .BYTE $A
                .BYTE $C8
                .BYTE $1D
                .BYTE   8
                .BYTE $FA
                .BYTE $43 ; C
                .BYTE $14
                .BYTE $B4

; =============== S U B R O U T I N E =======================================


sub_19918:                              ; CODE XREF: sub_19493+E2↑j
                PHA
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                PLA
                BCS     locret_1993B
                STX     $10
                TAX
                LDA     #$45 ; 'E'
                STA     $528,Y
                LDA     unk_198F7,X
                STA     $576,Y
                LDA     byte_198F8,X
                STA     $590,Y
                LDA     unk_198F9,X
                STA     $660,Y
                LDX     $10

locret_1993B:                           ; CODE XREF: sub_19918+5↑j
                RTS
; End of function sub_19918


; =============== S U B R O U T I N E =======================================


sub_1993C:
                LDA     $528,X
                BMI     loc_1998D
                ORA     #$80
                STA     $528,X
                AND     #$7F
                PHA
                JSR     sub_1872B
                STA     $5AA,X
                PLA
                TAY
                LDA     $99C4,Y
                TAY
                STA     $6E2,X
                LDA     $99D2,Y
                STA     $542,X
                LDA     $99D3,Y
                STA     $55C,X
                LDA     $99D4,Y
                STA     $590,X
                LDA     #1
                STA     $6FC,X
                LDA     #3
                STA     $660,X
                LDA     #4
                STA     $74A,X
                STA     $764,X
                LDA     $5AA,X
                LSR
                BCC     loc_1998D
                LDA     $55C,X
                EOR     #$FF
                CLC
                ADC     #1
                STA     $55C,X

loc_1998D:                              ; CODE XREF: sub_1993C+3↑j
                                        ; sub_1993C+44↑j
                INC     $764,X
                LDA     $764,X
                LSR
                LSR
                LSR
                AND     #1
                CLC
                ADC     #$1B
                STA     $576,X
                DEC     $6FC,X
                BNE     loc_199CD
                LDA     $6E2,X
                INC     $6E2,X
                TAY
                LDA     $99D5,Y
                BPL     loc_199B2
                DEC     $6E2,X

loc_199B2:                              ; CODE XREF: sub_1993C+71↑j
                TAY

loc_199B3:
                LDA     $5AA,X
                LSR
                TYA
                PHA
                BCC     loc_199C0
                EOR     #$FF
                SEC
                SBC     #7

loc_199C0:                              ; CODE XREF: sub_1993C+7D↑j
                AND     #$F
                JSR     sub_18097
                PLA
                LSR
                LSR
                AND     #$1C
                STA     $6FC,X

loc_199CD:                              ; CODE XREF: sub_1993C+65↑j
                JMP     loc_197B4
; End of function sub_1993C

; ---------------------------------------------------------------------------
                .BYTE   0
                .BYTE $13
                .BYTE $90
                .BYTE $FF
                .BYTE   0
                .BYTE $68 ; h
                .BYTE $68 ; h
                .BYTE $27 ; '
                .BYTE $26 ; &
                .BYTE $25 ; %
                .BYTE $24 ; $
                .BYTE $23 ; #
                .BYTE $22 ; "
                .BYTE $21 ; !
                .BYTE $40 ; @
                .BYTE $2F ; /
                .BYTE $2E ; .
                .BYTE $2D ; -
                .BYTE $2C ; ,
                .BYTE $2B ; +
                .BYTE $AA
                .BYTE   0
                .BYTE $E8
                .BYTE   1
                .BYTE $76 ; v
                .BYTE $35 ; 5
                .BYTE $34 ; 4
                .BYTE $33 ; 3
                .BYTE $42 ; B
                .BYTE $33 ; 3
                .BYTE $44 ; D
                .BYTE $36 ; 6
                .BYTE $38 ; 8
                .BYTE $7B ; {
                .BYTE $2A ; *
                .BYTE $28 ; (
                .BYTE $26 ; &
                .BYTE $B4

; =============== S U B R O U T I N E =======================================


sub_199F6:
                LDA     $528,X
                BMI     loc_19A21
                ORA     #$80
                STA     $528,X
                JSR     sub_18746
                LDA     #1
                STA     $612,X
                LDA     #$C0
                STA     $5F8,X
                LDA     #1
                STA     $660,X
                LDA     #$26 ; '&'
                STA     $576,X
                LDA     #2
                STA     $590,X
                LDA     #0
                STA     $5AA,X

loc_19A21:                              ; CODE XREF: sub_199F6+3↑j
                LDA     $5AA,X
                BNE     loc_19A8C
                LDA     $542
                SEC
                SBC     $542,X
                BCC     loc_19A36
                CMP     #$60 ; '`'
                BCC     loc_19A36
                JMP     loc_197B4
; ---------------------------------------------------------------------------

loc_19A36:                              ; CODE XREF: sub_199F6+37↑j
                                        ; sub_199F6+3B↑j
                INC     $5AA,X
                LDA     #$15
                JSR     $C030
                LDA     #3
                STA     $660,X
                STA     $74A,X
                LDA     #0
                STA     $612,X
                LDA     #$C
                STA     $716,X
                LDA     #$FF
                STA     $730,X
                LDA     #$30 ; '0'
                STA     $764,X
                LDA     #$27 ; '''
                STA     $576,X
                LDA     #1
                STA     $6E2,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19AC9
                LDA     #4
                STA     $14
                LDA     #$3B ; ';'
                JSR     sub_1B631
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19AC9
                INC     $14
                LDA     #$3B ; ';'
                JSR     sub_1B631
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19AC9
                LDA     #3
                STA     $14
                LDA     #$3B ; ';'
                JSR     sub_1B631

loc_19A8C:                              ; CODE XREF: sub_199F6+2E↑j
                LDA     $5AA,X
                CMP     #1
                BNE     loc_19AC9
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19AC9
                INC     $5AA,X
                LDA     #$8E
                JSR     sub_1B631
                LDA     #3
                STA     $660,Y
                STA     $5AA,Y
                STA     $74A,Y
                LDA     #$C
                STA     $716,Y
                LDA     #1
                STA     $730,Y
                STA     $6E2,Y
                LDA     #$27 ; '''
                STA     $576,Y
                LDA     #$42 ; 'B'
                STA     $590,Y
                LDA     #$30 ; '0'
                STA     $764,Y
                BNE     loc_19AE6

loc_19AC9:                              ; CODE XREF: sub_199F6+71↑j
                                        ; sub_199F6+7F↑j ...
                DEC     $6E2,X
                BNE     loc_19AE6
                LDA     #6
                STA     $6E2,X
                LDA     $716,X
                CMP     #4
                BEQ     loc_19AE6
                CLC
                ADC     $730,X
                AND     #$F
                STA     $716,X
                JSR     sub_18097

loc_19AE6:                              ; CODE XREF: sub_199F6+D1↑j
                                        ; sub_199F6+D6↑j ...
                JSR     sub_182B1
                JSR     sub_181F6
                JSR     sub_1A2B1
                BNE     locret_19AF6
                LDA     #$B
                JMP     sub_C057
; ---------------------------------------------------------------------------

locret_19AF6:                           ; CODE XREF: sub_199F6+F9↑j
                RTS
; End of function sub_199F6


; =============== S U B R O U T I N E =======================================


sub_19AF7:
                LDA     $528,X
                BMI     loc_19B25
                ORA     #$80
                STA     $528,X
                LDA     $55C
                STA     $55C,X
                LDA     #$43 ; 'C'
                STA     $576,X
                LDA     #3
                STA     $660,X
                LDA     #3
                STA     $74A,X
                LDA     #$A
                STA     $764,X
                LDA     #5
                STA     $694,X
                LDA     #1
                STA     $6AE,X

loc_19B25:                              ; CODE XREF: sub_19AF7+3↑j
                DEC     $6AE,X
                BNE     loc_19B53
                LDA     #$A
                STA     $6AE,X
                JSR     sub_18024
                LDA     #$40 ; '@'
                STA     $5F8,X
                LDA     #0
                STA     $612,X
                LDA     #2
                STA     $590,X
                LDA     $542,X
                CMP     $542
                BCC     loc_19B53
                LDA     #$FF
                STA     $612,X
                LDA     #$82
                STA     $590,X

loc_19B53:                              ; CODE XREF: sub_19AF7+31↑j
                                        ; sub_19AF7+50↑j
                LDA     $55C
                SEC
                SBC     $55C,X
                CLC
                ADC     #$10
                CMP     #$1F
                BCS     loc_19B80
                DEC     $694,X
                BNE     loc_19B80
                LDA     #5
                STA     $694,X
                LDY     #4
                LDA     $590,X
                BPL     loc_19B74
                LDY     #$C

loc_19B74:                              ; CODE XREF: sub_19AF7+79↑j
                STY     $14
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19B80
                LDA     #$3B ; ';'
                JSR     sub_1B631

loc_19B80:                              ; CODE XREF: sub_19AF7+68↑j
                                        ; sub_19AF7+6D↑j ...
                JSR     sub_182B1
                JSR     sub_181F6
                JSR     sub_1A2B1
                RTS
; End of function sub_19AF7


; =============== S U B R O U T I N E =======================================


sub_19B8A:
                LDA     $528,X
                BMI     loc_19BA1
                JSR     sub_19CE6
                LDA     #1
                STA     $694,X
                LDA     #0
                STA     $660,X
                LDA     #8
                STA     $67A,X

loc_19BA1:                              ; CODE XREF: sub_19B8A+3↑j
                JSR     sub_19C97
                JSR     sub_19CAB
                DEC     $694,X
                BNE     loc_19BE6
                INC     $660,X
                LDA     $660,X
                AND     #3
                STA     $660,X
                TAY
                CMP     #1
                BNE     loc_19BCC
                DEC     $67A,X
                JSR     sub_18746
                JSR     sub_1872B
                AND     #$7F
                ADC     #$10
                STA     $542,X

loc_19BCC:                              ; CODE XREF: sub_19B8A+30↑j
                LDA     byte_19C27,Y
                STA     $694,X
                CPY     #2
                BCC     loc_19C04
                LDA     $67A,X
                BNE     loc_19BE1
                INC     $67A,X
                STA     $694,X

loc_19BE1:                              ; CODE XREF: sub_19B8A+4F↑j
                LDA     #$10
                STA     $74A,X

loc_19BE6:                              ; CODE XREF: sub_19B8A+20↑j
                LDA     $74A,X
                BEQ     loc_19C04
                DEC     $74A,X
                LDA     $6AE,X
                INC     $6AE,X
                AND     #7
                SBC     #3
                STA     $14
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19C04
                LDA     #$14
                JSR     sub_1B631

loc_19C04:                              ; CODE XREF: sub_19B8A+4A↑j
                                        ; sub_19B8A+5F↑j ...
                LDA     $660,X
                BEQ     locret_19C26
                CMP     #2
                BEQ     loc_19C17
                LDA     $694,X
                AND     #1
                BEQ     locret_19C26
                JMP     sub_182D9
; ---------------------------------------------------------------------------

loc_19C17:                              ; CODE XREF: sub_19B8A+81↑j
                JSR     sub_182B1
                JSR     sub_181F6
                JSR     sub_19D23
                JSR     sub_1A2B1
                JMP     loc_19C85
; ---------------------------------------------------------------------------

locret_19C26:                           ; CODE XREF: sub_19B8A+7D↑j
                                        ; sub_19B8A+88↑j
                RTS
; End of function sub_19B8A

; ---------------------------------------------------------------------------
byte_19C27:     .BYTE $3C
                .BYTE $78 ; x
                .BYTE $64 ; d
                .BYTE $50 ; P

; =============== S U B R O U T I N E =======================================


sub_19C2B:
                LDA     $694,X
                CMP     #$78 ; 'x'
                BNE     loc_19C37
                LDA     #$45 ; 'E'
                STA     $576,X

loc_19C37:                              ; CODE XREF: sub_19C2B+5↑j
                LDA     $528,X
                BMI     loc_19C56
                JSR     sub_19CE6
                JSR     sub_18746
                LDA     #3
                STA     $660,X
                LDA     #1
                STA     $6E2,X
                LDA     #$1E
                STA     $716,X
                LDA     #$64 ; 'd'
                STA     $6FC,X

loc_19C56:                              ; CODE XREF: sub_19C2B+F↑j
                JSR     sub_1A8D1
                BCC     loc_19C65
                JSR     sub_1872B
                AND     #$F
                ADC     #$14
                STA     $6E2,X

loc_19C65:                              ; CODE XREF: sub_19C2B+2E↑j
                DEC     $694,X
                BNE     loc_19C73
                LDA     $67A,X
                STA     $694,X
                JSR     sub_19DAF

loc_19C73:                              ; CODE XREF: sub_19C2B+3D↑j
                JSR     sub_19C97
                JSR     sub_182B1
                JSR     sub_181F6
                JSR     sub_19D23
                JSR     sub_1A2B1
                JMP     loc_19C85
; ---------------------------------------------------------------------------

loc_19C85:                              ; CODE XREF: sub_19B8A+99↑j
                                        ; sub_19C2B+57↑j
                BNE     locret_19C96
                LDA     #2
                STA     $5AA,X
                LDA     #0
                STA     $6AE,X
                LDA     #$B0
                STA     $528,X

locret_19C96:                           ; CODE XREF: sub_19C2B:loc_19C85↑j
                                        ; sub_19CAB+3↓j ...
                RTS
; End of function sub_19C2B


; =============== S U B R O U T I N E =======================================


sub_19C97:                              ; CODE XREF: sub_19B8A:loc_19BA1↑p
                                        ; sub_19C2B:loc_19C73↑p
                LDA     $764,X
                CMP     #$C
                BCC     loc_19C9F
                RTS
; ---------------------------------------------------------------------------

loc_19C9F:                              ; CODE XREF: sub_19C97+5↑j
                CMP     #4
                LDA     #3
                BCS     loc_19CA7
                LDA     #1

loc_19CA7:                              ; CODE XREF: sub_19C97+C↑j
                STA     $590,X
                RTS
; End of function sub_19C97


; =============== S U B R O U T I N E =======================================


sub_19CAB:                              ; CODE XREF: sub_19B8A+1A↑p
                LDA     $5AA,X
                BEQ     locret_19C96
                PLA
                PLA
                JSR     sub_182D9
                DEC     $6AE,X
                BEQ     loc_19CC4
                LDA     $6AE,X
                AND     #7
                BNE     locret_19C96
                JMP     loc_1B48F
; ---------------------------------------------------------------------------

loc_19CC4:                              ; CODE XREF: sub_19CAB+D↑j
                LDA     #$5B ; '['
                STA     $528,X
                JSR     sub_1872B
                STA     $14
                LDA     #8
                STA     $15

loc_19CD2:                              ; CODE XREF: sub_19CAB+37↓j
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_19C96
                LDA     #$3B ; ';'
                JSR     sub_1B631
                INC     $14
                INC     $14
                DEC     $15
                BNE     loc_19CD2
                BEQ     locret_19C96
; End of function sub_19CAB


; =============== S U B R O U T I N E =======================================


sub_19CE6:                              ; CODE XREF: sub_19B8A+5↑p
                                        ; sub_19C2B+11↑p
                TAY
                ORA     #$80
                STA     $528,X
                LDA     $9CDF,Y
                STA     $576,X
                LDA     $9CE4,Y
                STA     $764,X
                LDA     $9CE9,Y
                STA     $67A,X
                STA     $694,X
                LDA     $9CEE,Y
                STA     $74A,X
                LDA     #2
                STA     $590,X
                JMP     sub_18746
; End of function sub_19CE6

; ---------------------------------------------------------------------------
                .BYTE $48 ; H
                .BYTE $49 ; I
                .BYTE $44 ; D
                .BYTE $45 ; E
                .BYTE $4A ; J
                .BYTE $19
                .BYTE $28 ; (
                .BYTE $3C ; <
                .BYTE $28 ; (
                .BYTE $64 ; d
                .BYTE   1
                .BYTE $19
                .BYTE $1E
                .BYTE $96
                .BYTE $1E
                .BYTE   0
                .BYTE   2
                .BYTE   1
                .BYTE   2
                .BYTE   1

; =============== S U B R O U T I N E =======================================


sub_19D23:                              ; CODE XREF: sub_19B8A+93↑p
                                        ; sub_19C2B+51↑p
                LDA     $528,X
                BMI     locret_19D79
                LDA     #3
                LDY     $8A
                BEQ     loc_19D4A
                CPY     #4
                BNE     loc_19D5A
                LDA     $5C
                CMP     #1
                BNE     loc_19D40
                LDA     $60
                CMP     #5
                BEQ     loc_19D48
                BNE     loc_19D5A

loc_19D40:                              ; CODE XREF: sub_19D23+13↑j
                CMP     #5
                BNE     loc_19D5A
                CMP     $60
                BNE     loc_19D5A

loc_19D48:                              ; CODE XREF: sub_19D23+19↑j
                LDA     #1

loc_19D4A:                              ; CODE XREF: sub_19D23+9↑j
                DEC     $730,X
                AND     $730,X
                BEQ     locret_19D79
                LDA     $88
                ORA     #$80
                STA     $528,X
                RTS
; ---------------------------------------------------------------------------

loc_19D5A:                              ; CODE XREF: sub_19D23+D↑j
                                        ; sub_19D23+1B↑j ...
                LDA     $89
                STA     $7E6,Y
                LDA     #$18
                STA     $528,Y
                LDA     #$A
                STA     $716,Y
                LDA     #$42 ; 'B'
                STA     $74A,Y
                JSR     sub_1872B
                STA     $77E,Y
                LDA     #0
                STA     $5AA,Y

locret_19D79:                           ; CODE XREF: sub_19D23+3↑j
                                        ; sub_19D23+2D↑j
                RTS
; End of function sub_19D23


; =============== S U B R O U T I N E =======================================


sub_19D7A:
                LDA     $528,X
                BMI     loc_19D94
                ORA     #$80
                STA     $528,X
                LDA     $5AA,X
                BNE     loc_19D94
                LDA     #3
                STA     $660,X
                LDA     $77E,X
                JSR     sub_18097

loc_19D94:                              ; CODE XREF: sub_19D7A+3↑j
                                        ; sub_19D7A+D↑j
                DEC     $716,X
                BEQ     loc_19D9D

loc_19D99:                              ; CODE XREF: sub_19D7A+25↓j
                JSR     sub_182B1
                RTS
; ---------------------------------------------------------------------------

loc_19D9D:                              ; CODE XREF: sub_19D7A+1D↑j
                CPX     #4
                BEQ     loc_19D99
                LDA     $7E6,X
                ORA     #$80
                STA     $528,X
                JSR     sub_182B1
                JMP     sub_181A7
; End of function sub_19D7A


; =============== S U B R O U T I N E =======================================


sub_19DAF:                              ; CODE XREF: sub_19C2B+45↑p
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCC     loc_19DB5
                RTS
; ---------------------------------------------------------------------------

loc_19DB5:                              ; CODE XREF: sub_19DAF+3↑j
                STY     $19
                JSR     sub_1802A
                STA     $14
                LDY     $19
                LDA     $528,X
                CMP     #$B1
                BNE     loc_19DCB
                JSR     sub_19E2A
                JMP     sub_197F3
; ---------------------------------------------------------------------------

loc_19DCB:                              ; CODE XREF: sub_19DAF+14↑j
                CMP     #$B2
                BNE     loc_19DEB
                LDA     #$4B ; 'K'
                STA     $576,Y
                LDA     #$18
                STA     $764,Y
                LDA     #3
                STA     $660,Y
                JSR     sub_1872B
                STA     $14
                LDA     #$24 ; '$'
                STA     $5AA,Y
                JMP     sub_1B631
; ---------------------------------------------------------------------------

loc_19DEB:                              ; CODE XREF: sub_19DAF+1E↑j
                CMP     #$B3
                BNE     loc_19E08
                LDA     #$46 ; 'F'
                STA     $576,X
                LDA     #$26 ; '&'
                STA     $18
                LDA     #$10
                STA     $16
                LDA     #$C
                STA     $17
                JSR     sub_1B5CF
                LDA     #$16
                JMP     sub_C030
; ---------------------------------------------------------------------------

loc_19E08:                              ; CODE XREF: sub_19DAF+3E↑j
                JSR     sub_19E2A
                LDA     #$29 ; ')'
                JSR     sub_1B631
                INC     $6C8,X
                LDA     $6C8,X
                AND     #3
                TAY
                LDA     unk_19E26,Y
                LDY     $19
                STA     $74A,Y
                TXA
                STA     $5AA,Y
                RTS
; End of function sub_19DAF

; ---------------------------------------------------------------------------
unk_19E26:      .BYTE   0
                .BYTE   0
                .BYTE   6
                .BYTE  $C

; =============== S U B R O U T I N E =======================================


sub_19E2A:                              ; CODE XREF: sub_19DAF+16↑p
                                        ; sub_19DAF:loc_19E08↑p
                LDA     $6FC,X
                AND     #1
                BEQ     locret_19E37
                LDA     $14
                ORA     #$80
                STA     $14

locret_19E37:                           ; CODE XREF: sub_19E2A+5↑j
                RTS
; End of function sub_19E2A


; =============== S U B R O U T I N E =======================================


sub_19E38:
                LDA     $528,X
                BMI     loc_19E64
                ORA     #$80
                STA     $528,X
                JSR     sub_18746
                LDA     #1
                STA     $590,X
                STA     $6E2,X
                STA     $74A,X
                LDA     #$4C ; 'L'
                STA     $576,X
                LDA     #3
                STA     $660,X
                LDA     #$14
                STA     $716,X
                LDA     #$FF
                STA     $6FC,X

loc_19E64:                              ; CODE XREF: sub_19E38+3↑j
                JSR     sub_1A8D1
                JSR     sub_182B1
                INC     $730,X
                LDA     $730,X
                AND     #$10
                BEQ     loc_19E76
                LDA     #1

loc_19E76:                              ; CODE XREF: sub_19E38+3A↑j
                CLC
                ADC     #$4C ; 'L'
                STA     $576,X
                JSR     sub_181F6
                LDA     $528,X
                BMI     locret_19E98
                LDY     $8A
                BNE     loc_19E8E
                LDA     #$81
                STA     $528
                RTS
; ---------------------------------------------------------------------------

loc_19E8E:                              ; CODE XREF: sub_19E38+4E↑j
                LDA     #$28 ; '('
                STA     $528,Y
                LDA     #$99
                STA     $528,X

locret_19E98:                           ; CODE XREF: sub_19E38+4A↑j
                RTS
; End of function sub_19E38


; =============== S U B R O U T I N E =======================================


sub_19E99:
                LDA     $528,X
                BMI     loc_19F05
                LDA     #1
                STA     $660,X
                LDA     #$80
                STA     $5F8,X
                LDA     #1
                STA     $612,X
                LDA     #$C0
                STA     $67A,X
                LDY     #0

loc_19EB4:                              ; CODE XREF: sub_19E99+D0↓j
                                        ; sub_19E99+F8↓j
                STY     $14
                JSR     sub_1872B
                AND     #1
                BEQ     loc_19EBF
                INC     $14

loc_19EBF:                              ; CODE XREF: sub_19E99+22↑j
                LDA     #$15
                STA     $576,X
                LDY     $14
                LDA     $9FBC,Y
                STA     $55C,X
                LDA     $9FBE,Y
                STA     $590,X
                LDA     $528,X
                ORA     #$80
                STA     $528,X
                CMP     #$92
                BEQ     loc_19EF0
                LSR
                LDA     $9FC0,Y
                STA     $764,X
                BCC     loc_19F05
                LDA     $9FBC,Y
                STA     $6FC,X
                JMP     loc_19F05
; ---------------------------------------------------------------------------

loc_19EF0:                              ; CODE XREF: sub_19E99+43↑j
                LDA     #3
                STA     $646,X
                LDA     $9FC0,Y
                STA     $6FC,X
                BEQ     loc_19F02
                LDA     #$FD
                STA     $646,X

loc_19F02:                              ; CODE XREF: sub_19E99+62↑j
                JMP     loc_19F94
; ---------------------------------------------------------------------------

loc_19F05:                              ; CODE XREF: sub_19E99+3↑j
                                        ; sub_19E99+4C↑j ...
                LDA     $542,X
                CLC
                ADC     #$18
                AND     $67A,X
                SEC
                SBC     #$18
                CMP     $542,X
                BEQ     loc_19F41
                LDA     $542,X
                CLC
                ADC     #$10
                AND     $67A,X
                SEC
                SBC     #$10
                CMP     $542,X
                BEQ     loc_19F2A
                JMP     loc_197B4
; ---------------------------------------------------------------------------

loc_19F2A:                              ; CODE XREF: sub_19E99+8C↑j
                LDA     #$16
                STA     $576,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19F3E
                LDA     $764,X
                STA     $14
                LDA     #$26 ; '&'
                JSR     sub_1B631

loc_19F3E:                              ; CODE XREF: sub_19E99+99↑j
                JMP     loc_197B4
; ---------------------------------------------------------------------------

loc_19F41:                              ; CODE XREF: sub_19E99+7B↑j
                LDA     #$15
                STA     $576,X
                JMP     loc_197B4
; ---------------------------------------------------------------------------
                LDA     $528,X
                BMI     loc_19F05
                LDA     #$13
                STA     $660,X
                LDA     #2
                STA     $612,X
                LDA     #$FA
                STA     $646,X
                LDA     #$FF
                STA     $730,X
                LDA     #$E0
                STA     $67A,X
                LDY     #6
                JMP     loc_19EB4
; ---------------------------------------------------------------------------
                LDA     $528,X
                BMI     loc_19F94
                LDA     #2
                STA     $612,X
                LDA     #$1B
                STA     $660,X
                LDA     #$30 ; '0'
                STA     $764,X
                LDA     #$E
                STA     $730,X
                LDA     #$44 ; 'D'
                STA     $6E2,X
                LDA     #8
                STA     $716,X
                LDY     #$C
                JMP     loc_19EB4
; ---------------------------------------------------------------------------

loc_19F94:                              ; CODE XREF: sub_19E99:loc_19F02↑j
                                        ; sub_19E99+D6↑j
                DEC     $764,X
                BNE     loc_19FAD
                LDA     #$30 ; '0'
                STA     $764,X
                LDA     #$15
                STA     $576,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_19FAD
                LDA     #$25 ; '%'
                JSR     sub_1B631

loc_19FAD:                              ; CODE XREF: sub_19E99+FE↑j
                                        ; sub_19E99+10D↑j
                LDA     $764,X
                CMP     #8
                BNE     loc_19FB9
                LDA     #$16
                STA     $576,X

loc_19FB9:                              ; CODE XREF: sub_19E99+119↑j
                JMP     loc_197B4
; End of function sub_19E99

; ---------------------------------------------------------------------------
                .BYTE $20
                .BYTE $E0
                .BYTE   2
                .BYTE   2
                .BYTE   1
                .BYTE   7
                .BYTE $20
                .BYTE $E0
                .BYTE   3
                .BYTE   3
                .BYTE   1
                .BYTE   7
                .BYTE $60 ; `
                .BYTE $A0
                .BYTE $80
                .BYTE $80
                .BYTE $FF
                .BYTE   0
                .BYTE $E0
                .BYTE $FF
                .BYTE $20
                .BYTE   1
                .BYTE $C8
                .BYTE $FD
                .BYTE $38 ; 8
                .BYTE   3

; =============== S U B R O U T I N E =======================================


sub_19FD6:
                LDA     $528,X
                BMI     loc_1A01B
                ORA     #$80
                STA     $528,X
                JSR     sub_1872B
                LDY     #0
                LSR
                BCC     loc_19FEA
                LDY     #2

loc_19FEA:                              ; CODE XREF: sub_19FD6+10↑j
                LDA     #9
                STA     $660,X
                LDA     #2
                STA     $590,X
                LDA     #$38 ; '8'
                STA     $74A,X

loc_19FF9:                              ; CODE XREF: sub_1A08A+2E↓j
                LDA     $9FCE,Y
                STA     $55C,X
                LDA     $9FCF,Y
                STA     $646,X
                LDA     #5
                STA     $612,X
                LDA     #$14
                STA     $716,X
                LDA     #$1F
                STA     $576,X
                LDA     #$20 ; ' '
                LDY     #5
                JSR     sub_1A2CB

loc_1A01B:                              ; CODE XREF: sub_19FD6+3↑j
                                        ; sub_1A08A+3↓j
                LDA     $5AA,X
                BEQ     loc_1A026
                CMP     #1
                BNE     loc_1A051
                BEQ     loc_1A033

loc_1A026:                              ; CODE XREF: sub_19FD6+48↑j
                DEC     $74A,X
                BNE     loc_1A051
                JSR     sub_1A05E
                LDA     #1
                STA     $5AA,X

loc_1A033:                              ; CODE XREF: sub_19FD6+4E↑j
                JSR     sub_183BC
                BCS     loc_1A04C
                LDA     $A054,Y
                STA     $576,X
                LDA     $590,X
                AND     #3
                ORA     $A059,Y
                STA     $590,X
                JMP     loc_1A051
; ---------------------------------------------------------------------------

loc_1A04C:                              ; CODE XREF: sub_19FD6+60↑j
                LDA     #2
                STA     $5AA,X

loc_1A051:                              ; CODE XREF: sub_19FD6+4C↑j
                                        ; sub_19FD6+53↑j ...
                JMP     loc_197B4
; End of function sub_19FD6

; ---------------------------------------------------------------------------
                .BYTE $1F
                .BYTE $20
                .BYTE $21 ; !
                .BYTE $20
                .BYTE $1F
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $80
                .BYTE $80

; =============== S U B R O U T I N E =======================================


sub_1A05E:                              ; CODE XREF: sub_19FD6+55↑p
                LDA     $528,X
                CMP     #$96
                BNE     loc_1A077
                LDA     #4
                STA     $74A,X
                JSR     sub_18024
                LDA     #$C
                STA     $716,X
                LDA     #0
                STA     $6E2,X

loc_1A077:                              ; CODE XREF: sub_1A05E+5↑j
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_1A081
                LDA     #$25 ; '%'
                JSR     sub_1B631

loc_1A081:                              ; CODE XREF: sub_1A05E+1C↑j
                LDA     $660,X
                ORA     #2
                STA     $660,X
                RTS
; End of function sub_1A05E


; =============== S U B R O U T I N E =======================================


sub_1A08A:
                LDA     $528,X
                BMI     loc_1A01B
                ORA     #$80
                STA     $528,X
                JSR     sub_1872B
                LDY     #4
                LSR
                BCC     loc_1A09E
                LDY     #6

loc_1A09E:                              ; CODE XREF: sub_1A08A+10↑j
                LDA     #$1B
                STA     $660,X
                LDA     #$10
                STA     $730,X
                LDA     #3
                STA     $590,X
                LDA     #$38 ; '8'
                STA     $74A,X
                LDA     $9FCF,Y
                STA     $6FC,X
                JMP     loc_19FF9
; End of function sub_1A08A


; =============== S U B R O U T I N E =======================================


sub_1A0BB:
                LDA     $528,X
                BMI     loc_1A110
                ORA     #$80
                STA     $528,X
                JSR     sub_18746
                JSR     sub_1872B
                AND     #$7F
                ADC     #$10
                STA     $542,X
                LDA     #$1B
                STA     $660,X
                LDA     #$48 ; 'H'
                STA     $6E2,X
                LDA     #$80
                STA     $6FC,X
                LDA     #$10
                STA     $716,X
                LDA     #$10
                STA     $730,X
                LDA     #0
                STA     $7CC,X
                LDA     #$78 ; 'x'
                STA     $7B2,X
                LDY     #3
                LDA     #$28 ; '('

loc_1A0F9:                              ; CODE XREF: sub_1A16B+32↓j
                                        ; sub_1A1A0+21↓j ...
                STA     $5AA,X
                LSR
                STA     $74A,X
                TYA
                STA     $764,X
                LDA     #$20 ; ' '
                LDY     #$10
                JSR     sub_1A2CB
                LDA     #2
                STA     $590,X

loc_1A110:                              ; CODE XREF: sub_1A0BB+3↑j
                                        ; sub_1A16B:loc_1A16E↓j ...
                JSR     sub_183BC
                STA     $10
                AND     #3
                TAY
                LDA     $A165,Y
                STA     $576,X
                LDA     $528,X
                CMP     #$9A
                BNE     loc_1A14D
                LDA     $7CC,X
                BNE     loc_1A139
                LDA     #$20 ; ' '
                STA     $590,X
                DEC     $7B2,X
                BNE     loc_1A149
                LDA     #$FF
                STA     $7CC,X

loc_1A139:                              ; CODE XREF: sub_1A0BB+6D↑j
                LDA     $10
                LSR
                LSR
                LSR
                TAY
                LDA     $A169,Y
                STA     $590,X
                AND     #$20 ; ' '
                BEQ     loc_1A14D

loc_1A149:                              ; CODE XREF: sub_1A0BB+77↑j
                JSR     sub_182B1
                RTS
; ---------------------------------------------------------------------------

loc_1A14D:                              ; CODE XREF: sub_1A0BB+68↑j
                                        ; sub_1A0BB+8C↑j
                DEC     $74A,X
                BNE     loc_1A15B
                LDA     $5AA,X
                STA     $74A,X
                JSR     sub_1A264

loc_1A15B:                              ; CODE XREF: sub_1A0BB+95↑j
                JSR     sub_182B1
                JSR     sub_181F6
                JSR     sub_1A2B1
                RTS
; End of function sub_1A0BB

; ---------------------------------------------------------------------------
                .BYTE $11
                .BYTE $12
                .BYTE $13
                .BYTE $14
                .BYTE   2
                .BYTE $22 ; "

; =============== S U B R O U T I N E =======================================


sub_1A16B:
                LDA     $528,X

loc_1A16E:                              ; CODE XREF: sub_1A1A0+3↓j
                BMI     loc_1A110
                ORA     #$80
                STA     $528,X
                LDA     #$20 ; ' '
                STA     $55C,X
                LDA     #2
                STA     $646,X
                JSR     sub_1872B
                LSR
                BCC     loc_1A18F
                LDA     #$E0
                STA     $55C,X
                LDA     #$FE
                STA     $646,X

loc_1A18F:                              ; CODE XREF: sub_1A16B+18↑j
                LDA     #2
                STA     $660,X
                LDA     #$30 ; '0'
                STA     $542,X
                LDY     #1
                LDA     #$10
                JMP     loc_1A0F9
; End of function sub_1A16B


; =============== S U B R O U T I N E =======================================


sub_1A1A0:
                LDA     $528,X
                BMI     loc_1A16E
                ORA     #$80
                STA     $528,X
                JSR     sub_18746
                LDA     #1
                STA     $660,X
                LDA     #1
                STA     $612,X
                LDY     #6
                LDA     $49
                CMP     #$50 ; 'P'
                BCS     loc_1A1C4
                LDA     #$28 ; '('
                JMP     loc_1A0F9
; ---------------------------------------------------------------------------

loc_1A1C4:                              ; CODE XREF: sub_1A1A0+1D↑j
                LDA     #$20 ; ' '
                JMP     loc_1A0F9
; End of function sub_1A1A0


; =============== S U B R O U T I N E =======================================


sub_1A1C9:
                LDA     $528,X
                BMI     loc_1A201
                ORA     #$80
                STA     $528,X
                JSR     sub_1872B
                AND     #$7F
                ADC     #$40 ; '@'
                STA     $55C,X
                LDA     #8
                STA     $542,X
                LDA     #1
                STA     $74A,X
                LDA     #3
                STA     $660,X
                JSR     sub_1802A
                STA     $716,X
                JSR     sub_18097
                LDA     #$50 ; 'P'
                STA     $730,X
                LDY     #2
                LDA     #$50 ; 'P'
                JMP     loc_1A0F9
; ---------------------------------------------------------------------------

loc_1A201:                              ; CODE XREF: sub_1A1C9+3↑j
                DEC     $730,X
                LDA     $730,X
                BNE     loc_1A20F
                INC     $730,X
                JMP     loc_1A110
; ---------------------------------------------------------------------------

loc_1A20F:                              ; CODE XREF: sub_1A1C9+3E↑j
                PHA
                LDA     $49
                LDY     #8
                CMP     #$20 ; ' '
                BCC     loc_1A226
                LDY     #$C
                CMP     #$50 ; 'P'
                BCC     loc_1A226
                LDY     #$10
                CMP     #$80
                BCC     loc_1A226
                LDY     #$14

loc_1A226:                              ; CODE XREF: sub_1A1C9+4D↑j
                                        ; sub_1A1C9+53↑j ...
                STY     $10
                PLA
                CMP     $10
                BCS     loc_1A25B
                AND     #3
                BNE     loc_1A25B
                LDA     $716,X
                STA     $14
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_1A25B
                LDA     #$C
                STA     $74A,Y
                LDA     #$29 ; ')'
                JSR     sub_1B631
                LDA     $55C,Y
                CLC
                ADC     #$FA
                STA     $55C,Y
                LDA     $542,Y
                CLC
                ADC     #$28 ; '('
                STA     $542,Y
                TXA
                STA     $5AA,Y

loc_1A25B:                              ; CODE XREF: sub_1A1C9+62↑j
                                        ; sub_1A1C9+66↑j ...
                JMP     loc_1A110
; End of function sub_1A1C9

; ---------------------------------------------------------------------------
                .BYTE   8
                .BYTE   8
                .BYTE  $C
                .BYTE  $C
                .BYTE $10
                .BYTE $10

; =============== S U B R O U T I N E =======================================


sub_1A264:                              ; CODE XREF: sub_1A0BB+9D↑p
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_1A2B0
                LDA     $528,X
                CMP     #$9A
                BNE     loc_1A275
                LDA     #$25 ; '%'
                JMP     sub_1B631
; ---------------------------------------------------------------------------

loc_1A275:                              ; CODE XREF: sub_1A264+A↑j
                CMP     #$9B
                BNE     loc_1A283
                JSR     sub_1802A
                STA     $14
                LDA     #$15
                JMP     sub_1B631
; ---------------------------------------------------------------------------

loc_1A283:                              ; CODE XREF: sub_1A264+13↑j
                CMP     #$9C
                BNE     loc_1A2A7
                LDA     #$28 ; '('
                STA     $528,Y
                STY     $15
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_1A2B0
                LDA     #2
                STA     $14
                LDA     #$26 ; '&'
                JSR     sub_1B631
                LDY     $15
                LDA     #6
                STA     $14
                LDA     #$26 ; '&'
                JMP     sub_1B631
; ---------------------------------------------------------------------------

loc_1A2A7:                              ; CODE XREF: sub_1A264+21↑j
                LDA     #4
                STA     $14
                LDA     #$3B ; ';'
                JMP     sub_1B631
; ---------------------------------------------------------------------------

locret_1A2B0:                           ; CODE XREF: sub_1A264+3↑j
                                        ; sub_1A264+2D↑j
                RTS
; End of function sub_1A264


; =============== S U B R O U T I N E =======================================


sub_1A2B1:                              ; CODE XREF: sub_1961B+5C↑p
                                        ; sub_199F6+F6↑p ...
                LDA     $528,X
                BPL     loc_1A2B9
                PLA
                PLA

locret_1A2B8:                           ; CODE XREF: sub_1A2B1+B↓j
                RTS
; ---------------------------------------------------------------------------

loc_1A2B9:                              ; CODE XREF: sub_1A2B1+3↑j
                DEC     $764,X
                BEQ     locret_1A2B8
                LDA     #$14
                JSR     sub_C030
                LDA     $88
                ORA     #$80
                STA     $528,X
                RTS
; End of function sub_1A2B1


; =============== S U B R O U T I N E =======================================


sub_1A2CB:                              ; CODE XREF: sub_18F78+31↑p
                                        ; sub_18F78+6F↑p ...
                STA     $67A,X
                TYA
                STA     $6C8,X
                LDA     #0
                STA     $6AE,X
                STA     $694,X
                RTS
; End of function sub_1A2CB


; =============== S U B R O U T I N E =======================================


sub_1A2DB:
                LDA     $528,X
                BPL     loc_1A2E3
                JMP     loc_1A379
; ---------------------------------------------------------------------------

loc_1A2E3:                              ; CODE XREF: sub_1A2DB+3↑j
                ORA     #$80
                STA     $528,X
                JSR     sub_1876A
                LDA     $528,X
                ADC     #1
                STA     $528,Y
                LDA     #1
                STA     $660,X
                STA     $660,Y
                LDA     #$80
                STA     $5F8,X
                STA     $62C,X
                STA     $5F8,Y
                LDA     #$80
                STA     $62C,Y
                LDA     #1
                STA     $612,X
                STA     $646,X
                STA     $612,Y
                LDA     #$FE
                STA     $646,Y
                LDA     #$20 ; ' '
                STA     $55C,X
                LDA     #$E0
                STA     $55C,Y
                LDA     #$23 ; '#'
                STA     $576,X
                STA     $576,Y
                LDA     #2
                STA     $590,X
                LDA     #$42 ; 'B'
                STA     $590,Y
                LDA     $528,X
                CMP     #$9E
                BEQ     loc_1A376
                LDA     #$40 ; '@'
                STA     $5AA,X
                STA     $5AA,Y
                LDA     #0
                STA     $5F8,X
                STA     $5F8,Y
                STA     $62C,X
                STA     $62C,Y
                LDA     #$FF
                STA     $612,X
                STA     $612,Y
                STA     $646,Y
                LDA     #1
                STA     $646,X
                LDA     #$F0
                STA     $542,X
                STA     $542,Y
                LDA     #$83
                STA     $590,X
                LDA     #$C3
                STA     $590,Y

loc_1A376:                              ; CODE XREF: sub_1A2DB+61↑j
                JMP     loc_1A3C5
; ---------------------------------------------------------------------------

loc_1A379:                              ; CODE XREF: sub_1A2DB+5↑j
                LDA     $5AA,X
                BMI     loc_1A3C5
                LDA     $764,X
                TAY
                LDA     $528,X
                CLC
                ADC     #1
                CMP     $528,Y
                BNE     loc_1A3C8
                JSR     sub_1A3D7
                BCS     loc_1A39A
                LDA     #2
                STA     $660,X
                STA     $660,Y

loc_1A39A:                              ; CODE XREF: sub_1A2DB+B5↑j
                LDA     $55C,Y
                SEC
                SBC     $55C,X
                CMP     #$B
                BCS     loc_1A3C5
                LDA     $5AA,X
                ORA     #$80
                STA     $5AA,X
                LDA     #$22 ; '"'
                STA     $576,X
                LDA     $55C,X
                CLC
                ADC     #5
                STA     $55C,X
                LDA     #1
                STA     $660,X
                LDA     #$28 ; '('
                STA     $528,Y

loc_1A3C5:                              ; CODE XREF: sub_1A2DB:loc_1A376↑j
                                        ; sub_1A2DB+A1↑j ...
                JMP     loc_197B4
; ---------------------------------------------------------------------------

loc_1A3C8:                              ; CODE XREF: sub_1A2DB+B0↑j
                INC     $528,X
                JSR     sub_1A3D7
                BCS     loc_1A3C5
                LDA     #2
                STA     $660,X
                BNE     loc_1A3C5
; End of function sub_1A2DB


; =============== S U B R O U T I N E =======================================


sub_1A3D7:                              ; CODE XREF: sub_1A2DB+B2↑p
                                        ; sub_1A2DB+F0↑p
                LDA     $5AA,X
                AND     #$40 ; '@'
                BEQ     loc_1A3E5
                LDA     $542,X
                CMP     $542
                RTS
; ---------------------------------------------------------------------------

loc_1A3E5:                              ; CODE XREF: sub_1A3D7+5↑j
                LDA     $542
                CMP     $542,X
                RTS
; End of function sub_1A3D7


; =============== S U B R O U T I N E =======================================


sub_1A3EC:
                LDA     $528,X
                BMI     loc_1A44A
                ORA     #$80
                STA     $528,X
                JSR     sub_1872B
                AND     #6
                TAY
                LDA     $A4DF,Y
                STA     $55C,X
                LDA     #1
                STA     $74A,X
                LDA     $A4E0,Y
                JSR     sub_18097
                LDA     #3
                STA     $660,X
                LDA     $528,X
                LDY     #0
                CMP     #$A2
                BEQ     loc_1A423
                LDY     #6
                CMP     #$C1
                BEQ     loc_1A423
                LDY     #$C

loc_1A423:                              ; CODE XREF: sub_1A3EC+2D↑j
                                        ; sub_1A3EC+33↑j
                LDA     $A4E7,Y
                STA     $716,X
                STA     $730,X
                LDA     $A4E8,Y
                STA     $6E2,X
                LDA     $A4E9,Y
                STA     $6FC,X
                LDA     $A4EA,Y
                STA     $764,X
                LDA     $A4EB,Y
                STA     $590,X
                LDA     $A4EC,Y
                STA     $576,X

loc_1A44A:                              ; CODE XREF: sub_1A3EC+3↑j
                DEC     $730,X
                BEQ     loc_1A452
                JMP     loc_1A4D5
; ---------------------------------------------------------------------------

loc_1A452:                              ; CODE XREF: sub_1A3EC+61↑j
                LDA     $716,X
                STA     $730,X
                LDA     $6E2,X
                STA     $15
                LDA     $528,X
                CMP     #$C2
                BNE     loc_1A4A9
                LDA     #$15
                JSR     sub_C030
                JSR     sub_1802A
                STA     $14
                LDY     #0

loc_1A470:                              ; CODE XREF: sub_1A3EC+B9↓j
                STY     $16
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_1A4D5
                STY     $17
                LDY     $16
                LDA     $A4FF,Y
                PHA
                LDA     $A500,Y
                LDY     $17
                CLC
                ADC     $55C,X
                STA     $55C,Y
                PLA
                CLC
                ADC     $542,X
                STA     $542,Y
                LDA     #$3B ; ';'
                STA     $528,Y
                LDA     $14
                STA     $77E,Y
                INC     $16
                INC     $16
                LDY     $16
                DEC     $15
                BNE     loc_1A470
                BEQ     loc_1A4D5

loc_1A4A9:                              ; CODE XREF: sub_1A3EC+76↑j
                LDA     $542
                CMP     $542,X
                LDY     #0
                BCC     loc_1A4B5
                LDY     #3

loc_1A4B5:                              ; CODE XREF: sub_1A3EC+C5↑j
                                        ; sub_1A3EC+E7↓j
                LDA     $A4F9,Y
                STA     $14
                INY
                STY     $16
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_1A4D5
                LDA     $6FC,X
                CMP     #$14
                BNE     loc_1A4CC
                JSR     sub_1981D

loc_1A4CC:                              ; CODE XREF: sub_1A3EC+DB↑j
                JSR     sub_1B631
                LDY     $16
                DEC     $15
                BNE     loc_1A4B5

loc_1A4D5:                              ; CODE XREF: sub_1A3EC+63↑j
                                        ; sub_1A3EC+89↑j ...
                JSR     sub_182B1
                JSR     sub_181F6
                JSR     sub_1A2B1
                RTS
; End of function sub_1A3EC

; ---------------------------------------------------------------------------
                .BYTE $20
                .BYTE   2
                .BYTE $D0
                .BYTE   6
                .BYTE $50 ; P
                .BYTE   4
                .BYTE $A0
                .BYTE   4
                .BYTE $20
                .BYTE   3
                .BYTE $26 ; &
                .BYTE   7
                .BYTE   3
                .BYTE $28 ; (
                .BYTE $18
                .BYTE   2
                .BYTE $14
                .BYTE   4
                .BYTE   2
                .BYTE $29 ; )
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $3B ; ;
                .BYTE   7
                .BYTE   0
                .BYTE $2A ; *
                .BYTE  $C
                .BYTE  $A
                .BYTE  $E
                .BYTE   4
                .BYTE   2
                .BYTE   6
                .BYTE $18
                .BYTE   0
                .BYTE   0
                .BYTE $E8
                .BYTE   0
                .BYTE $18
                .BYTE $E8
                .BYTE   0
                .BYTE   0
                .BYTE   0

; =============== S U B R O U T I N E =======================================


sub_1A509:
                LDA     $528,X
                BMI     loc_1A56E
                ORA     #$80
                STA     $528,X
                SEC
                SBC     #$AE
                ASL
                ASL
                TAY
                LDA     unk_1A601,Y
                STA     $590,X
                LDA     #$30 ; '0'
                STA     $6E2,X
                STA     $6FC,X
                LDA     $A602,Y
                STA     $5AA,X
                LDA     $A603,Y
                STA     $716,X
                LDA     $A604,Y
                STA     $67A,X
                LDA     #$17
                STA     $576,X
                LDA     #1
                STA     $660,X
                STA     $612,X
                STA     $730,X
                LDA     #$50 ; 'P'
                STA     $5F8,X
                LDA     #$E0
                STA     $55C,X
                JSR     sub_1872B
                LSR
                BCS     loc_1A56E
                LDA     #$20 ; ' '
                STA     $55C,X
                LDA     #$FF
                STA     $730,X
                LDA     $67A,X
                EOR     #$FF
                SEC
                SBC     #7
                STA     $67A,X

loc_1A56E:                              ; CODE XREF: sub_1A509+3↑j
                                        ; sub_1A509+4E↑j
                LDA     $5AA,X
                BPL     loc_1A5AE
                CMP     #$80
                BNE     loc_1A59A
                LDA     $542,X
                CMP     $542
                BCC     loc_1A5FE
                LDA     #$18
                STA     $576,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_1A593
                LDA     $67A,X
                STA     $14
                LDA     #$15
                JSR     sub_1B631

loc_1A593:                              ; CODE XREF: sub_1A509+7E↑j
                LDA     #$C0
                STA     $5AA,X
                BNE     loc_1A5FE

loc_1A59A:                              ; CODE XREF: sub_1A509+6C↑j
                LDA     $542,X
                CMP     $542
                BCS     loc_1A5FE
                LDA     #$80
                STA     $5AA,X
                LDA     #$17
                STA     $576,X
                BNE     loc_1A5FE

loc_1A5AE:                              ; CODE XREF: sub_1A509+68↑j
                BNE     loc_1A5D6
                DEC     $6FC,X
                BNE     loc_1A5FE
                LDA     #$18
                STA     $576,X
                LDA     #0
                STA     $660,X
                LDA     $6E2,X
                STA     $6FC,X
                LDA     #$40 ; '@'
                STA     $5AA,X
                LDA     $67A,X
                STA     $764,X
                LDA     $716,X
                STA     $694,X

loc_1A5D6:                              ; CODE XREF: sub_1A509:loc_1A5AE↑j
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_1A5E5
                LDA     $764,X
                STA     $14
                LDA     #$15
                JSR     sub_1B631

loc_1A5E5:                              ; CODE XREF: sub_1A509+D0↑j
                LDA     $764,X
                CLC
                ADC     $730,X
                STA     $764,X
                DEC     $694,X
                BNE     loc_1A5FE
                LDA     #1
                STA     $660,X
                LDA     #0
                STA     $5AA,X

loc_1A5FE:                              ; CODE XREF: sub_1A509+74↑j
                                        ; sub_1A509+8F↑j ...
                JMP     loc_197B4
; End of function sub_1A509

; ---------------------------------------------------------------------------
unk_1A601:      .BYTE   2
                .BYTE $80
                .BYTE   1
                .BYTE   8
                .BYTE   3
                .BYTE   0
                .BYTE   8
                .BYTE   4

; =============== S U B R O U T I N E =======================================


sub_1A609:
                LDA     $528,X
                BMI     loc_1A638
                JSR     sub_18746
                LDY     #4

loc_1A613:                              ; CODE XREF: sub_1A6C1+B↓j
                LDA     #$E
                STA     $576,X

loc_1A618:                              ; CODE XREF: sub_1A645+12↓j
                                        ; sub_1A6A5+19↓j
                LDA     #5
                STA     $74A,X
                TYA
                JSR     sub_18097
                LDA     #3
                STA     $660,X
                LDA     #2
                STA     $590,X
                LDA     #$20 ; ' '
                STA     $716,X
                LDA     $528,X
                ORA     #$80
                STA     $528,X

loc_1A638:                              ; CODE XREF: sub_1A609+3↑j
                                        ; sub_1A645:loc_1A66F↓j
                LDA     $612,X
                BPL     loc_1A642
                LDA     #$82
                STA     $590,X

loc_1A642:                              ; CODE XREF: sub_1A609+32↑j
                JMP     loc_197B4
; End of function sub_1A609


; =============== S U B R O U T I N E =======================================


sub_1A645:
                LDA     $528,X
                BMI     loc_1A659
                JSR     sub_1876A
                JSR     sub_18746
                LDA     #$F
                STA     $576,X
                LDY     #4
                BNE     loc_1A618

loc_1A659:                              ; CODE XREF: sub_1A645+3↑j
                                        ; sub_1A6A5+3↓j
                JSR     sub_1877E
                LDA     $528,X
                CMP     #$B9
                BEQ     loc_1A66C
                LDA     $730,X
                TAY
                LDA     #2
                STA     $542,Y

loc_1A66C:                              ; CODE XREF: sub_1A645+1C↑j
                DEC     $716,X

loc_1A66F:                              ; CODE XREF: sub_1A6C1+3↓j
                BNE     loc_1A638
                LDA     #$15
                JSR     sub_C030
                JSR     sub_1802A
                STA     $14
                DEC     $14
                LDA     $764,X
                TAY
                LDA     #$3B ; ';'
                JSR     sub_1B631
                INC     $14
                LDA     $528,X
                CMP     #$B9
                BEQ     loc_1A698
                LDA     $730,X
                TAY
                LDA     #$3B ; ';'
                JSR     sub_1B631

loc_1A698:                              ; CODE XREF: sub_1A645+48↑j
                LDA     #$3B ; ';'
                STA     $528,X
                INC     $14
                LDA     $14
                STA     $77E,X
                RTS
; End of function sub_1A645


; =============== S U B R O U T I N E =======================================


sub_1A6A5:
                LDA     $528,X
                BMI     loc_1A659
                JSR     sub_1876A
                TYA
                STA     $730,X
                JSR     sub_1876A
                JSR     sub_18746
                LDA     #$10
                STA     $576,X
                LDY     #4
                JMP     loc_1A618
; End of function sub_1A6A5


; =============== S U B R O U T I N E =======================================


sub_1A6C1:
                LDA     $528,X
                BMI     loc_1A66F
                LDA     $77E,X
                AND     #$F
                TAY
                JMP     loc_1A613
; End of function sub_1A6C1


; =============== S U B R O U T I N E =======================================


sub_1A6CF:
                JSR     sub_1872B
                AND     #3
                STA     $10
                LDA     $49
                LSR
                LSR
                CLC
                ADC     $10
                CMP     #$3C ; '<'
                BCC     loc_1A6E3
                LDA     #$3B ; ';'

loc_1A6E3:                              ; CODE XREF: sub_1A6CF+10↑j
                TAY
                JSR     $C078
                CMP     #$40 ; '@'
                BNE     loc_1A6ED
                LDA     #$2C ; ','

loc_1A6ED:                              ; CODE XREF: sub_1A6CF+1A↑j
                STA     $528,X
                RTS
; End of function sub_1A6CF


; =============== S U B R O U T I N E =======================================


sub_1A6F1:
                LDA     $528,X
                BMI     loc_1A72A
                ORA     #$80
                STA     $528,X
                LDA     $5AA,X
                BEQ     loc_1A70E
                LDA     #1
                STA     $74A,X
                LDA     $77E,X
                JSR     sub_18097
                JMP     loc_1A720
; ---------------------------------------------------------------------------

loc_1A70E:                              ; CODE XREF: sub_1A6F1+D↑j
                JSR     sub_18746
                LDA     #$10
                STA     $764,X
                LDA     #1
                STA     $660,X
                LDA     #$80
                STA     $5F8,X

loc_1A720:                              ; CODE XREF: sub_1A6F1+1A↑j
                LDA     #2
                STA     $590,X
                LDA     #$10
                STA     $764,X

loc_1A72A:                              ; CODE XREF: sub_1A6F1+3↑j
                LDA     $5AA,X
                BNE     loc_1A740
                INC     $6AE,X
                LDA     $6AE,X
                LSR
                LSR
                AND     #1
                TAY
                LDA     unk_1A74A,Y
                STA     $576,X

loc_1A740:                              ; CODE XREF: sub_1A6F1+3C↑j
                                        ; sub_1ABE8+59↓j
                JSR     sub_182B1
                JSR     sub_181F6
                JSR     sub_1A2B1
                RTS
; End of function sub_1A6F1

; ---------------------------------------------------------------------------
unk_1A74A:      .BYTE $1D
                .BYTE $1E

; =============== S U B R O U T I N E =======================================


sub_1A74C:
                LDA     $528,X
                BMI     loc_1A776
                ORA     #$80
                STA     $528,X
                JSR     sub_18746
                JSR     sub_1872B
                AND     #3
                STA     $74A,X
                INC     $74A,X
                JSR     sub_18024
                LDA     #3
                STA     $660,X
                LDA     #$2C ; ','
                STA     $576,X
                LDA     #3
                STA     $590,X

loc_1A776:                              ; CODE XREF: sub_1A74C+3↑j
                JMP     loc_197B4
; End of function sub_1A74C


; =============== S U B R O U T I N E =======================================


sub_1A779:
                LDA     $528,X
                BMI     loc_1A7C2
                ORA     #$80
                STA     $528,X
                LDA     $55C
                CMP     #$80
                LDY     #$30 ; '0'
                BCS     loc_1A78E
                LDY     #$D0

loc_1A78E:                              ; CODE XREF: sub_1A779+11↑j
                TYA
                STA     $55C,X
                LDA     #1
                STA     $612,X
                LDA     #$80
                STA     $5F8,X
                LDA     #2
                STA     $590,X
                LDA     #1
                STA     $660,X
                LDA     $5B
                INC     $5B
                AND     #7
                STA     $764,X
                LDA     #$20 ; ' '
                STA     $74A,X
                LDA     $5A
                LDY     #$2B ; '+'
                CMP     #5
                BCC     loc_1A7BE
                LDY     #$2D ; '-'

loc_1A7BE:                              ; CODE XREF: sub_1A779+41↑j
                TYA
                STA     $576,X

loc_1A7C2:                              ; CODE XREF: sub_1A779+3↑j
                LDA     $542,X
                CMP     #$81
                BNE     loc_1A7DB
                LDA     #0
                STA     $660,X
                DEC     $74A,X
                BNE     loc_1A7DB
                INC     $660,X
                LDA     #$FC
                STA     $612,X

loc_1A7DB:                              ; CODE XREF: sub_1A779+4E↑j
                                        ; sub_1A779+58↑j
                JSR     sub_182B1
                LDA     $576,X
                CMP     #$2D ; '-'
                BNE     loc_1A7FA
                LDA     $764,X
                ASL
                STA     $85
                LDA     #$F1
                STA     $84
                LDA     #3
                STA     $86
                LDA     #$FC
                STA     $87
                JSR     sub_1860C

loc_1A7FA:                              ; CODE XREF: sub_1A779+6A↑j
                JSR     sub_181F6
                LDA     $528,X
                BPL     loc_1A803
                RTS
; ---------------------------------------------------------------------------

loc_1A803:                              ; CODE XREF: sub_1A779+87↑j
                JSR     sub_18796
                JSR     sub_18796
                LDA     $55
                AND     #$3F ; '?'
                STA     $10
                LDA     $17E
                AND     #$3F ; '?'
                CMP     $10
                BNE     loc_1A821
                JSR     sub_C02A
                LDA     #$3E ; '>'
                STA     $528,X
                RTS
; ---------------------------------------------------------------------------

loc_1A821:                              ; CODE XREF: sub_1A779+9D↑j
                LDA     $576,X
                CMP     #$2D ; '-'
                BNE     locret_1A835
                JSR     sub_C02A
                LDA     #$53 ; 'S'
                STA     $528,X
                LDA     #0
                STA     $5AA,X

locret_1A835:                           ; CODE XREF: sub_1A779+AD↑j
                RTS
; End of function sub_1A779


; =============== S U B R O U T I N E =======================================


sub_1A836:
                LDA     $528,X
                BMI     loc_1A86F
                ORA     #$80
                STA     $528,X
                JSR     sub_18746
                LDA     $55C,X
                PHA
                JSR     sub_18746
                PLA
                STA     $542,X
                LDA     #$1D
                STA     $576,X
                LDA     #5
                STA     $764,X
                LDA     #3
                STA     $74A,X
                STA     $660,X
                LDA     #$78 ; 'x'
                STA     $6E2,X
                LDA     #$32 ; '2'
                STA     $716,X
                LDA     #$1E
                STA     $6FC,X

loc_1A86F:                              ; CODE XREF: sub_1A836+3↑j
                LDA     $5AA,X
                BMI     loc_1A87C
                LDA     $6E2,X
                AND     #3
                STA     $590,X

loc_1A87C:                              ; CODE XREF: sub_1A836+3C↑j
                LDA     $6E2,X
                LSR
                LSR
                LSR
                AND     #1
                CLC
                ADC     #$1D
                STA     $576,X
                JSR     sub_1A8D1
                BCC     loc_1A899
                LDA     #1
                STA     $590,X
                LDA     #$80
                STA     $5AA,X

loc_1A899:                              ; CODE XREF: sub_1A836+57↑j
                LDA     $5AA,X
                BMI     loc_1A8A1
                JMP     sub_182D9
; ---------------------------------------------------------------------------

loc_1A8A1:                              ; CODE XREF: sub_1A836+66↑j
                JSR     sub_181F6
                LDA     $528,X
                BMI     loc_1A8CD
                LDA     #$14
                JSR     sub_C030
                LDA     $8A
                CMP     #4
                BNE     loc_1A8C2
                LDA     $5C
                CMP     #2
                BNE     loc_1A8C2
                LDA     #$C3
                STA     $528,X
                JMP     sub_182D9
; ---------------------------------------------------------------------------

loc_1A8C2:                              ; CODE XREF: sub_1A836+7C↑j
                                        ; sub_1A836+82↑j
                DEC     $764,X
                BNE     loc_1A8C8
                RTS
; ---------------------------------------------------------------------------

loc_1A8C8:                              ; CODE XREF: sub_1A836+8F↑j
                LDA     #$C3
                STA     $528,X

loc_1A8CD:                              ; CODE XREF: sub_1A836+71↑j
                JSR     sub_182B1
                RTS
; End of function sub_1A836


; =============== S U B R O U T I N E =======================================


sub_1A8D1:                              ; CODE XREF: sub_19C2B:loc_19C56↑p
                                        ; sub_19E38:loc_19E64↑p ...
                CLC
                LDA     $6FC,X
                BEQ     locret_1A8E9
                DEC     $6E2,X
                BNE     locret_1A8E9
                LDA     $716,X
                STA     $6E2,X
                JSR     sub_18024
                DEC     $6FC,X
                SEC

locret_1A8E9:                           ; CODE XREF: sub_1A8D1+4↑j
                                        ; sub_1A8D1+9↑j
                RTS
; End of function sub_1A8D1


; =============== S U B R O U T I N E =======================================


sub_1A8EA:
                LDA     $528,X
                BMI     loc_1A96C
                ORA     #$80
                STA     $528,X
                LDA     $48
                CLC
                ADC     #$10
                STA     $48
                BCC     loc_1A900
                JSR     sub_18788

loc_1A900:                              ; CODE XREF: sub_1A8EA+11↑j
                LDA     $4C
                CMP     #$11
                BCC     loc_1A90A
                LDA     #1
                BNE     loc_1A90E

loc_1A90A:                              ; CODE XREF: sub_1A8EA+1A↑j
                TAY

loc_1A90B:
                LDA     $8E92,Y

loc_1A90E:                              ; CODE XREF: sub_1A8EA+1E↑j
                CLC
                ADC     $4A
                STA     $4A
                BCC     loc_1A918
                JSR     sub_1879F

loc_1A918:                              ; CODE XREF: sub_1A8EA+29↑j
                LDA     $56
                CMP     #8
                BCS     loc_1A928
                ASL
                ASL
                EOR     #$FF
                CLC
                ADC     #$25 ; '%'
                JMP     loc_1A92A
; ---------------------------------------------------------------------------

loc_1A928:                              ; CODE XREF: sub_1A8EA+32↑j
                LDA     #1

loc_1A92A:                              ; CODE XREF: sub_1A8EA+3B↑j
                CLC
                ADC     $4A
                STA     $4A
                BCC     loc_1A934
                JSR     sub_1879F

loc_1A934:                              ; CODE XREF: sub_1A8EA+45↑j
                LDA     #0
                STA     $4C
                STA     $56
                LDA     #$11
                JSR     sub_C030

loc_1A93F:                              ; CODE XREF: sub_1B69B+12↓j
                JSR     sub_C02A
                LDA     #0
                STA     $660,X
                LDA     #$40 ; '@'
                LDY     #7
                JSR     sub_1A2CB
                LDA     #1
                STA     $590,X
                LDA     $7E6,X
                BEQ     loc_1A96C
                CMP     #$25 ; '%'
                BEQ     loc_1A96C
                CMP     #$26 ; '&'
                BEQ     loc_1A96C
                DEC     $45
                BNE     loc_1A96C
                LDA     #$10
                STA     $45
                LDA     #1
                STA     $46

loc_1A96C:                              ; CODE XREF: sub_1A8EA+3↑j
                                        ; sub_1A8EA+6C↑j ...
                JSR     sub_183BC
                BCS     loc_1A97B
                LDA     unk_1A97E,Y
                STA     $576,X
                JSR     sub_182B1
                RTS
; ---------------------------------------------------------------------------

loc_1A97B:                              ; CODE XREF: sub_1A8EA+85↑j
                                        ; sub_1A985+1E↓j
                JMP     sub_182FE
; End of function sub_1A8EA

; ---------------------------------------------------------------------------
unk_1A97E:      .BYTE   6
                .BYTE   7
                .BYTE   8
                .BYTE   8
                .BYTE   7
                .BYTE   6
                .BYTE   5

; =============== S U B R O U T I N E =======================================


sub_1A985:
                LDA     $528,X
                BMI     loc_1A9A0
                ORA     #$80
                STA     $528,X
                LDA     #$12
                JSR     sub_C030
                LSR     $4B
                JSR     sub_C02A
                LDA     #$60 ; '`'
                LDY     #$10
                JSR     sub_1A2CB

loc_1A9A0:                              ; CODE XREF: sub_1A985+3↑j
                JSR     sub_183BC
                BCS     loc_1A97B
                LDA     $55C,X
                STA     $17
                LDA     $542,X
                STA     $16
                LDA     #0
                STA     $15

loc_1A9B3:                              ; CODE XREF: sub_1A985+88↓j
                LDY     $15
                LDA     $6AE,X
                SEC
                SBC     unk_1AA26,Y
                BCC     loc_1AA07
                CMP     #$C
                BCS     loc_1AA07
                TAY
                LDA     unk_1AA56,Y
                STA     $576,X
                LDY     $15
                CLC
                LDA     unk_1AA36,Y
                BMI     loc_1A9D7
                ADC     $17
                BCS     loc_1AA07
                BCC     loc_1A9DB

loc_1A9D7:                              ; CODE XREF: sub_1A985+4A↑j
                ADC     $17
                BCC     loc_1AA07

loc_1A9DB:                              ; CODE XREF: sub_1A985+50↑j
                STA     $55C,X
                CLC
                LDA     unk_1AA46,Y
                BMI     loc_1A9EA
                ADC     $16
                BCS     loc_1AA07
                BCC     loc_1A9EE

loc_1A9EA:                              ; CODE XREF: sub_1A985+5D↑j
                ADC     $16
                BCC     loc_1AA07

loc_1A9EE:                              ; CODE XREF: sub_1A985+63↑j
                STA     $542,X
                LDA     #1
                STA     $590,X
                TYA
                AND     #7
                BNE     loc_1AA04
                TYA
                LSR
                LSR
                LSR
                ADC     #1
                STA     $590,X

loc_1AA04:                              ; CODE XREF: sub_1A985+74↑j
                JSR     sub_182D9

loc_1AA07:                              ; CODE XREF: sub_1A985+37↑j
                                        ; sub_1A985+3B↑j ...
                INC     $15
                LDA     $15
                CMP     #$10
                BNE     loc_1A9B3
                LDA     $17
                STA     $55C,X
                LDA     $16
                STA     $542,X
                LDA     $6AE,X
                AND     #7
                BNE     locret_1AA25
                LDA     #$12
                JSR     $C030

locret_1AA25:                           ; CODE XREF: sub_1A985+99↑j
                RTS
; End of function sub_1A985

; ---------------------------------------------------------------------------
unk_1AA26:      .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   1
                .BYTE   1
                .BYTE 1
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   3
                .BYTE   3
                .BYTE   3
                .BYTE   3
                .BYTE   3
                .BYTE   4
                .BYTE   4
unk_1AA36:      .BYTE  $C
                .BYTE $F4
                .BYTE   4
                .BYTE $F5
                .BYTE $10
                .BYTE   6
                .BYTE   0
                .BYTE $F2
                .BYTE   8
                .BYTE   1
                .BYTE $F3
                .BYTE $F0
                .BYTE $F6
                .BYTE   4
                .BYTE $10
                .BYTE   5
unk_1AA46:      .BYTE   0
                .BYTE  $C
                .BYTE $F0
                .BYTE $F2
                .BYTE $10
                .BYTE $F8
                .BYTE   6
                .BYTE $12
                .BYTE $F4
                .BYTE $F0
                .BYTE  $C
                .BYTE $FE
                .BYTE $10
                .BYTE   3
                .BYTE $F8
                .BYTE   8
unk_1AA56:      .BYTE   6
                .BYTE   7
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   7
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   7
                .BYTE   6
                .BYTE   5
; ---------------------------------------------------------------------------
                LDA     $528,X
                BMI     loc_1AA86

; =============== S U B R O U T I N E =======================================


sub_1AA67:                              ; CODE XREF: sub_1ABA2↓p
                LDA     #3
                STA     $74A,X
                JSR     sub_18024

loc_1AA6F:                              ; CODE XREF: sub_1AA91+B↓j
                LDA     #3
                STA     $660,X
                LDA     #$47 ; 'G'
                STA     $576,X
                LDA     #1
                STA     $590,X
                LDA     $528,X
                ORA     #$80
                STA     $528,X

loc_1AA86:                              ; CODE XREF: seg002:AA65↑j
                                        ; sub_1AA8C+3↓j
                JSR     sub_182B1
                JMP     sub_181E4
; End of function sub_1AA67


; =============== S U B R O U T I N E =======================================


sub_1AA8C:
                LDA     $528,X
                BMI     loc_1AA86
; End of function sub_1AA8C


; =============== S U B R O U T I N E =======================================


sub_1AA91:                              ; CODE XREF: sub_1ABAA↓p
                LDA     #3
                STA     $74A,X

loc_1AA96:                              ; CODE XREF: sub_1ABE8+16↓p
                LDA     $77E,X
                JSR     sub_18097
                JMP     loc_1AA6F
; End of function sub_1AA91

; ---------------------------------------------------------------------------
                .BYTE $DE
                .BYTE $42 ; B
                .BYTE   5
                .BYTE $F0
                .BYTE   1
                .BYTE $60 ; `
                .BYTE $4C ; L
                .BYTE $FE
                .BYTE $82

; =============== S U B R O U T I N E =======================================


sub_1AAA8:
                LDA     $528,X
                BMI     loc_1AB18
                ORA     #$80
                STA     $528,X
                LDA     #3
                STA     $660,X
                LDA     $74A,X
                PHA
                TAY
                LDA     unk_1AB90,Y
                STA     $74A,X
                CLC
                LDA     $77E,X
                STA     $67A,X
                JSR     sub_18097
                LDA     $5F8,X
                STA     $716,X
                LDA     $612,X
                STA     $730,X
                LDA     $62C,X
                STA     $6AE,X
                LDA     $646,X
                STA     $6C8,X
                LDA     #8
                STA     $694,X
                LDA     $67A,X
                BPL     loc_1AAF3
                LDA     #0
                STA     $694,X

loc_1AAF3:                              ; CODE XREF: sub_1AAA8+44↑j
                PLA
                TAY
                LDA     $AB91,Y
                STA     $74A,X
                LDA     $AB92,Y
                STA     $6FC,X
                LDA     $AB93,Y
                STA     $576,X
                LDA     $AB94,Y
                STA     $590,X
                LDA     $AB95,Y
                STA     $764,X
                LDA     #1
                STA     $6E2,X

loc_1AB18:                              ; CODE XREF: sub_1AAA8+3↑j
                DEC     $6E2,X
                BNE     loc_1AB5D
                LDA     $6FC,X
                STA     $6E2,X
                LDA     $67A,X
                BPL     loc_1AB2E
                DEC     $694,X
                JMP     loc_1AB31
; ---------------------------------------------------------------------------

loc_1AB2E:                              ; CODE XREF: sub_1AAA8+7E↑j
                INC     $694,X

loc_1AB31:                              ; CODE XREF: sub_1AAA8+83↑j
                LDA     $694,X
                JSR     sub_18097
                LDA     $5F8,X
                CLC
                ADC     $716,X
                STA     $5F8,X
                LDA     $612,X
                ADC     $730,X
                STA     $612,X
                LDA     $62C,X
                CLC
                ADC     $6AE,X
                STA     $62C,X
                LDA     $646,X
                ADC     $6C8,X
                STA     $646,X

loc_1AB5D:                              ; CODE XREF: sub_1AAA8+73↑j
                JSR     sub_182B1
                LDA     $576,X
                CMP     #$47 ; 'G'
                BNE     loc_1AB6A
                JMP     sub_181E4
; ---------------------------------------------------------------------------

loc_1AB6A:                              ; CODE XREF: sub_1AAA8+BD↑j
                LDA     $5AA,X
                TAY
                LDA     $528,Y
                AND     #$7F
                CMP     #$23 ; '#'
                BEQ     loc_1AB7B
                CMP     #$50 ; 'P'
                BNE     loc_1AB89

loc_1AB7B:                              ; CODE XREF: sub_1AAA8+CD↑j
                LDA     #$23 ; '#'
                STA     $528,X
                LDA     $528,X
                AND     #$7F
                STA     $7E6,X
                RTS
; ---------------------------------------------------------------------------

loc_1AB89:                              ; CODE XREF: sub_1AAA8+D1↑j
                JSR     sub_181F6
                JSR     sub_1A2B1
                RTS
; End of function sub_1AAA8

; ---------------------------------------------------------------------------
unk_1AB90:      .BYTE   2
                .BYTE   4
                .BYTE   3
                .BYTE $47 ; G
                .BYTE   1
                .BYTE   0
                .BYTE   1
                .BYTE   4
                .BYTE   5
                .BYTE $1B
                .BYTE   2
                .BYTE   6
                .BYTE   1
                .BYTE $83
                .BYTE   1
                .BYTE   8
                .BYTE   0
                .BYTE   6

; =============== S U B R O U T I N E =======================================


sub_1ABA2:
                JSR     sub_1AA67
                LDA     #$A5
                JMP     sub_1ABAF
; End of function sub_1ABA2


; =============== S U B R O U T I N E =======================================


sub_1ABAA:
                JSR     sub_1AA91
                LDA     #$A6
; End of function sub_1ABAA


; =============== S U B R O U T I N E =======================================


sub_1ABAF:                              ; CODE XREF: sub_1ABA2+5↑j
                STA     $528,X
                JSR     sub_1872B
                AND     #$7F
                ADC     $62C,X
                STA     $62C,X
                LDA     $646,X
                ADC     #0
                STA     $646,X
                LDA     $62C,X
                ADC     #$C0
                STA     $62C,X
                LDA     $646,X
                ADC     #$FF
                STA     $646,X
                ORA     $62C,X
                BNE     locret_1ABE7
                LDA     $5F8,X
                ORA     $612,X
                BNE     locret_1ABE7
                LDA     #$40 ; '@'
                STA     $5F8,X

locret_1ABE7:                           ; CODE XREF: sub_1ABAF+29↑j
                                        ; sub_1ABAF+31↑j
                RTS
; End of function sub_1ABAF


; =============== S U B R O U T I N E =======================================


sub_1ABE8:
                LDA     $528,X
                BMI     loc_1AC11
                JSR     sub_1872B
                AND     #1
                CLC
                ADC     #2
                STA     $74A,X
                LDA     $77E,X
                STA     $6FC,X
                JSR     loc_1AA96
                LDA     $528,X
                BMI     loc_1AC07
                RTS
; ---------------------------------------------------------------------------

loc_1AC07:                              ; CODE XREF: sub_1ABE8+1C↑j
                LDA     #3
                STA     $764,X

loc_1AC0C:                              ; CODE XREF: sub_1ABE8+31↓j
                                        ; sub_1ABE8+44↓j
                LDA     #$28 ; '('
                STA     $6E2,X

loc_1AC11:                              ; CODE XREF: sub_1ABE8+3↑j
                DEC     $6E2,X
                BNE     loc_1AC2F
                JSR     sub_1872B
                BPL     loc_1AC0C
                AND     #8
                CLC
                ADC     $6FC,X
                SEC
                SBC     #4
                AND     #$F
                STA     $6FC,X
                JSR     sub_18097
                JMP     loc_1AC0C
; ---------------------------------------------------------------------------

loc_1AC2F:                              ; CODE XREF: sub_1ABE8+2C↑j
                LDA     $6E2,X
                AND     #1
                TAY
                LDA     unk_1AC44,Y
                STA     $576,X
                LDA     $AC46,Y
                STA     $590,X
                JMP     loc_1A740
; End of function sub_1ABE8

; ---------------------------------------------------------------------------
unk_1AC44:      .BYTE   7
                .BYTE   9
                .BYTE   0
                .BYTE   2

; =============== S U B R O U T I N E =======================================


sub_1AC48:
                LDA     $528,X
                BMI     loc_1AC6E
                ORA     #$80
                STA     $528,X
                LDA     #4
                STA     $74A,X
                LDA     $77E,X
                JSR     sub_18097
                LDA     #3
                STA     $660,X
                LDA     #$60 ; '`'
                LDY     #$10
                JSR     sub_1A2CB
                LDA     #$16
                JSR     $C030

loc_1AC6E:                              ; CODE XREF: sub_1AC48+3↑j
                JSR     sub_183BC
                LDA     unk_1AC83,Y
                STA     $576,X
                LDA     $AC93,Y
                STA     $590,X
                JSR     sub_182B1
                JMP     sub_181F6
; End of function sub_1AC48

; ---------------------------------------------------------------------------
unk_1AC83:      .BYTE   9
                .BYTE  $A
                .BYTE  $B
                .BYTE  $C
                .BYTE  $D
                .BYTE  $C
                .BYTE  $B
                .BYTE  $A
                .BYTE   9
                .BYTE  $A
                .BYTE  $B
                .BYTE  $C
                .BYTE  $D
                .BYTE  $C
                .BYTE  $B
                .BYTE  $A
                .BYTE   0
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   0
                .BYTE $41 ; A
                .BYTE $42 ; B
                .BYTE $43 ; C
                .BYTE   0
                .BYTE $C1
                .BYTE $C2
                .BYTE $C3
                .BYTE   0
                .BYTE $81
                .BYTE $82
                .BYTE $83

; =============== S U B R O U T I N E =======================================


sub_1ACA3:
                LDA     $528,X
                BMI     loc_1ACD8
                ORA     #$80
                STA     $528,X
                STA     $5F8,X
                LDA     #$47 ; 'G'
                STA     $576,X
                LDA     #1
                STA     $590,X
                LDA     #$B
                STA     $660,X
                LDA     #$FF
                STA     $6E2,X
                STA     $612,X
                LDA     #$C
                STA     $716,X
                LDA     $77E,X
                STA     $646,X
                JSR     sub_1872B
                STA     $62C,X

loc_1ACD8:                              ; CODE XREF: sub_1ACA3+3↑j
                JSR     sub_182B1
                JMP     sub_181E4
; End of function sub_1ACA3


; =============== S U B R O U T I N E =======================================


sub_1ACDE:
                JSR     sub_1B87F
                BCC     loc_1ACE8
                LDA     #$1E
                STA     $764,X

loc_1ACE8:                              ; CODE XREF: sub_1ACDE+3↑j
                LDA     $5AA,X
                BEQ     loc_1AD05
                LDA     $6E2,X
                CMP     #9
                BCS     loc_1AD05
                INC     $6E2,X
                TAY
                AND     #3
                BNE     loc_1AD05
                TYA
                AND     #$C
                ADC     #$70 ; 'p'
                TAY
                JSR     sub_1B1CD

loc_1AD05:                              ; CODE XREF: sub_1ACDE+D↑j
                                        ; sub_1ACDE+14↑j ...
                JSR     sub_18204
                JSR     sub_1A2B1
                BNE     loc_1AD13
                JSR     sub_C02A
                JMP     sub_C02A
; ---------------------------------------------------------------------------

loc_1AD13:                              ; CODE XREF: sub_1ACDE+2D↑j
                LDA     $5AA,X
                INC     $5AA,X
                CMP     #0
                BNE     loc_1AD20
                JMP     loc_1972F
; ---------------------------------------------------------------------------

loc_1AD20:                              ; CODE XREF: sub_1ACDE+3D↑j
                LDA     $5C
                JMP     sub_19493
; End of function sub_1ACDE


; =============== S U B R O U T I N E =======================================


sub_1AD25:
                JSR     sub_1B87F
                BCC     loc_1AD2F
                LDA     #$A
                STA     $764,X

loc_1AD2F:                              ; CODE XREF: sub_1AD25+3↑j
                JSR     sub_18204
                JSR     sub_1A2B1
                RTS
; End of function sub_1AD25


; =============== S U B R O U T I N E =======================================


sub_1AD36:
                LDA     $528,X
                BMI     loc_1AD68
                ORA     #$80
                STA     $528,X
                LDA     #1
                STA     $6E2,X
                STA     $6AE,X
                STA     $660,X
                STA     $74A,X
                STA     $764,X
                LDA     #$FF
                STA     $612,X
                LDA     #$FE
                STA     $5F8,X
                LDA     #$A
                STA     $716,X
                LDA     #0
                STA     $590,X
                STA     $6C8,X

loc_1AD68:                              ; CODE XREF: sub_1AD36+3↑j
                INC     $6AE,X
                LDA     $6AE,X
                AND     #2
                LSR
                CLC
                ADC     #$4E ; 'N'
                STA     $576,X
                LDA     $590,X
                BNE     loc_1AD9D
                JSR     sub_181ED
                LDA     $528,X
                BPL     loc_1AD88
                JSR     sub_182B1
                RTS
; ---------------------------------------------------------------------------

loc_1AD88:                              ; CODE XREF: sub_1AD36+4C↑j
                ORA     #$80
                STA     $528,X
                INC     $590,X
                LDA     #$81
                STA     $528
                JSR     sub_1B2ED
                LDA     #3
                STA     $660,X

loc_1AD9D:                              ; CODE XREF: sub_1AD36+44↑j
                LDA     $764,X
                BEQ     loc_1ADA7
                LDA     #2
                STA     $6FC,X

loc_1ADA7:                              ; CODE XREF: sub_1AD36+6A↑j
                JSR     sub_1A8D1
                BCC     loc_1ADB7
                JSR     sub_1872B
                AND     #$F
                ADC     $6E2,X
                STA     $6E2,X

loc_1ADB7:                              ; CODE XREF: sub_1AD36+74↑j
                LDA     $6C8,X
                TAY
                LDA     $528,Y
                CMP     #$B0
                BCC     loc_1ADC6
                CMP     #$B5
                BCC     loc_1ADD4

loc_1ADC6:                              ; CODE XREF: sub_1AD36+8A↑j
                INY
                CPY     #$1A
                BCC     loc_1ADCD
                LDY     #5

loc_1ADCD:                              ; CODE XREF: sub_1AD36+93↑j
                TYA
                STA     $6C8,X
                JMP     loc_1AE07
; ---------------------------------------------------------------------------

loc_1ADD4:                              ; CODE XREF: sub_1AD36+8E↑j
                LDA     $6AE,X
                AND     #7
                BNE     loc_1AE07
                LDA     $542,X
                CMP     $542,Y
                PHP
                LDA     $542,X
                CMP     $542
                LDA     #0
                ROL
                STA     $10
                LDA     #0
                PLP
                ROL
                EOR     $10
                BEQ     loc_1AE07
                LDA     $764,Y
                SEC
                SBC     #1
                BNE     loc_1ADFF
                LDA     #1

loc_1ADFF:                              ; CODE XREF: sub_1AD36+C5↑j
                STA     $764,Y
                LDA     #$14
                JSR     sub_C030

loc_1AE07:                              ; CODE XREF: sub_1AD36+9B↑j
                                        ; sub_1AD36+A3↑j ...
                LDA     $C0
                AND     #$22 ; '"'
                CMP     #2
                BNE     loc_1AE16
                LDA     #0
                STA     $C2
                DEC     $764,X

loc_1AE16:                              ; CODE XREF: sub_1AD36+D7↑j
                JSR     sub_182B1
                RTS
; End of function sub_1AD36


; =============== S U B R O U T I N E =======================================


sub_1AE1A:
                LDA     $528,X
                BMI     loc_1AE3D
                ORA     #$80
                STA     $528,X
                LDA     #$FF
                STA     $612,X
                LDA     #$80
                STA     $5F8,X
                LDA     #0
                STA     $590,X
                LDA     #1
                STA     $660,X
                LDA     #6
                STA     $764,X

loc_1AE3D:                              ; CODE XREF: sub_1AE1A+3↑j
                INC     $6AE,X
                LDA     $6AE,X
                LSR
                LSR
                LSR
                LSR
                AND     #1
                CLC
                ADC     #$24 ; '$'
                STA     $576,X
                JSR     sub_182B1
                JSR     sub_181F6
                LDA     $528,X
                BMI     locret_1AE98
                LDA     $8A
                BEQ     loc_1AE74
                DEC     $764,X
                BNE     loc_1AE6D
                LDA     #0
                STA     $612,X
                LDA     #1
                STA     $590,X

loc_1AE6D:                              ; CODE XREF: sub_1AE1A+47↑j
                LDA     #$BE
                STA     $528,X
                BNE     locret_1AE98

loc_1AE74:                              ; CODE XREF: sub_1AE1A+42↑j
                LDA     $590,X
                BEQ     loc_1AE88
                LDY     $51
                CPY     #6
                DEY
                BNE     loc_1AE81
                INY

loc_1AE81:                              ; CODE XREF: sub_1AE1A+64↑j
                BCC     loc_1AE85
                LDY     #5

loc_1AE85:                              ; CODE XREF: sub_1AE1A:loc_1AE81↑j
                TYA
                STA     $51

loc_1AE88:                              ; CODE XREF: sub_1AE1A+5D↑j
                LDA     #8
                JSR     $C030
                LDA     #$81
                STA     $528
                INC     $32
                BNE     locret_1AE98
                DEC     $32

locret_1AE98:                           ; CODE XREF: sub_1AE1A+3E↑j
                                        ; sub_1AE1A+58↑j ...
                RTS
; End of function sub_1AE1A


; =============== S U B R O U T I N E =======================================


sub_1AE99:
                JSR     sub_1B87F
                BCS     loc_1AEC6
                LDA     $716,X
                ASL
                STA     $85
                LDA     #$F7
                STA     $84
                LDA     #$EC
                STA     $87
                LDA     #1
                STA     $86
                LDA     #0
                STA     $590,X
                JSR     sub_1860C
                LDA     $C0
                ORA     #$80
                STA     $C0
                JMP     loc_1AED8
; ---------------------------------------------------------------------------
                .BYTE $20
                .BYTE $7F ; 
                .BYTE $B8
                .BYTE $90
                .BYTE $12
; ---------------------------------------------------------------------------

loc_1AEC6:                              ; CODE XREF: sub_1AE99+3↑j
                                        ; sub_1B72C+F↓j
                LDA     $528,X
                AND     #$7F
                TAY
                LDA     $AF54,Y
                STA     $764,X
                LDA     $AF68,Y
                STA     $576,X

loc_1AED8:                              ; CODE XREF: sub_1AE99+25↑j
                                        ; sub_1B72C:loc_1B775↓j
                JSR     sub_18204
                JSR     sub_1A2B1
                BEQ     loc_1AEE1
                RTS
; ---------------------------------------------------------------------------

loc_1AEE1:                              ; CODE XREF: sub_1AE99+45↑j
                LDA     $88
                CMP     #$51 ; 'Q'
                BCS     loc_1AF21
                STA     $67A,X
                JSR     sub_187A6
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCC     loc_1AEF3
                RTS
; ---------------------------------------------------------------------------

loc_1AEF3:                              ; CODE XREF: sub_1AE99+57↑j
                LDA     #$D1
                JSR     sub_1B631
                LDA     #8
                STA     $576,Y
                LDA     #0
                STA     $764,Y
                LDA     $5F8,X
                STA     $5F8,Y
                LDA     $612,X
                STA     $612,Y
                LDA     $62C,X
                STA     $62C,Y
                LDA     $646,X
                STA     $646,Y
                LDA     $74A,X
                STA     $74A,Y
                RTS
; ---------------------------------------------------------------------------

loc_1AF21:                              ; CODE XREF: sub_1AE99+4C↑j
                BNE     loc_1AF28
                LDY     #$4C ; 'L'
                JMP     sub_1B1E8
; ---------------------------------------------------------------------------

loc_1AF28:                              ; CODE XREF: sub_1AE99:loc_1AF21↑j
                CMP     #$52 ; 'R'
                BNE     loc_1AF49
                JSR     sub_C02A
                LDA     #1
                STA     $5AA,X
                LDA     $716,X

loc_1AF37:                              ; CODE XREF: sub_1AE99+F9↓j
                STA     $764,X
                LDA     #$12
                JSR     sub_C030
                LDA     #0
                JSR     sub_1B169
                LDY     #$11
                JMP     sub_1B09A
; ---------------------------------------------------------------------------

loc_1AF49:                              ; CODE XREF: sub_1AE99+91↑j
                CMP     #$58 ; 'X'
                BCS     loc_1AF83
                CMP     #$54 ; 'T'
                BNE     loc_1AF5B
                LDA     #0
                JSR     sub_1B169
                LDY     #$11
                JMP     sub_1B09A
; ---------------------------------------------------------------------------

loc_1AF5B:                              ; CODE XREF: sub_1AE99+B6↑j
                CMP     #$57 ; 'W'
                BNE     loc_1AF6B
                LDA     $74A,X
                AND     #2
                ASL
                ADC     #$10
                TAY
                JMP     sub_1B1E2
; ---------------------------------------------------------------------------

loc_1AF6B:                              ; CODE XREF: sub_1AE99+C4↑j
                CMP     #$55 ; 'U'
                BNE     loc_1AF79
                LDA     #0
                JSR     sub_1B169
                LDY     #$21 ; '!'
                JMP     sub_1B09A
; ---------------------------------------------------------------------------

loc_1AF79:                              ; CODE XREF: sub_1AE99+D4↑j
                LDA     #$40 ; '@'
                JSR     sub_1B169
                LDY     #$12
                JMP     sub_1B09A
; ---------------------------------------------------------------------------

loc_1AF83:                              ; CODE XREF: sub_1AE99+B2↑j
                BEQ     loc_1AF95
                JSR     sub_C02A
                LDA     #1
                STA     $5AA,X
                JSR     sub_1872B
                AND     #7
                JMP     loc_1AF37
; ---------------------------------------------------------------------------

loc_1AF95:                              ; CODE XREF: sub_1AE99:loc_1AF83↑j
                LDY     #$C
                JMP     sub_1B1E8
; End of function sub_1AE99

; ---------------------------------------------------------------------------
                .BYTE   6
                .BYTE   6
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   4
                .BYTE   0
                .BYTE   4
                .BYTE   4
                .BYTE   4
                .BYTE   8
                .BYTE   3
                .BYTE   3
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   8
                .BYTE   8
                .BYTE $3E ; >
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE $3E ; >
                .BYTE $F0
                .BYTE $F2
                .BYTE $F1
                .BYTE $F3
                .BYTE $F4
                .BYTE $F6
                .BYTE $F5
                .BYTE $F7
                .BYTE $F8
                .BYTE $FA
                .BYTE $F9
                .BYTE $FB
                .BYTE $FC
                .BYTE $FE
                .BYTE $FD
                .BYTE $FF
                .BYTE $E0
                .BYTE $E2
                .BYTE $E1
                .BYTE $E3
                .BYTE $E4
                .BYTE $E6
                .BYTE $E5
                .BYTE $E7
                .BYTE $E8
                .BYTE $EA
                .BYTE $E9
                .BYTE $EB
                .BYTE $EC
                .BYTE $EE
                .BYTE $ED
                .BYTE $EF
                .BYTE $DC
                .BYTE $BE
                .BYTE $BE
                .BYTE $BE
                .BYTE $DD
                .BYTE $BE
                .BYTE $BE
                .BYTE $BE
                .BYTE $DE
                .BYTE $BE
                .BYTE $BE
                .BYTE $BE
                .BYTE $DF
                .BYTE $BE
                .BYTE $BE
                .BYTE $BE
                .BYTE $D8
                .BYTE $D8
                .BYTE $D8
                .BYTE $D8
                .BYTE $D9
                .BYTE $D9
                .BYTE $D9
                .BYTE $D9
                .BYTE $DA
                .BYTE $DA
                .BYTE $DA
                .BYTE $DA
                .BYTE $DB
                .BYTE $DB
                .BYTE $DB
                .BYTE $DB
                .BYTE $D4
                .BYTE $D4
                .BYTE $D4
                .BYTE $D4
                .BYTE $D5
                .BYTE $D5
                .BYTE $D5
                .BYTE $D5
                .BYTE $D6
                .BYTE $D6
                .BYTE $D6
                .BYTE $D6
                .BYTE $D7
                .BYTE $D7
                .BYTE $D7
                .BYTE $D7
                .BYTE $DC
                .BYTE $DC
                .BYTE $DC
                .BYTE $DC
                .BYTE $DD
                .BYTE $DD
                .BYTE $DD
                .BYTE $DD
                .BYTE $DE
                .BYTE $DE
                .BYTE $DE
                .BYTE $DE
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $C7
                .BYTE $CE
                .BYTE $DC
                .BYTE $C5
                .BYTE $C7
                .BYTE $CE
                .BYTE $DD
                .BYTE $C5
                .BYTE $C7
                .BYTE $CE
                .BYTE $DE
                .BYTE $C5
                .BYTE $C7
                .BYTE $CE
                .BYTE $DF
                .BYTE $C5
                .BYTE $DE
                .BYTE $DE
                .BYTE $85
                .BYTE $87
                .BYTE $DD
                .BYTE $DD
                .BYTE $85
                .BYTE $87
                .BYTE $DC
                .BYTE $DC
                .BYTE $85
                .BYTE $87
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $A4
                .BYTE $A6
                .BYTE $A5
                .BYTE $A7
                .BYTE $83
                .BYTE $83
                .BYTE $83
                .BYTE $83
                .BYTE $A0
                .BYTE $A2
                .BYTE $A1
                .BYTE $A3
                .BYTE $A3
                .BYTE $A2
                .BYTE $A1
                .BYTE $A3
                .BYTE $A0
                .BYTE $A4
                .BYTE $A4
                .BYTE $A3
                .BYTE $9C
                .BYTE $21 ; !
                .BYTE $21 ; !
                .BYTE $9C
                .BYTE $21 ; !
                .BYTE $9D
                .BYTE $9D
                .BYTE $21 ; !
                .BYTE $A2
                .BYTE $21 ; !
                .BYTE $A3
                .BYTE $21 ; !
                .BYTE $A1
                .BYTE $A3
                .BYTE $21 ; !
                .BYTE $21 ; !
                .BYTE $21 ; !
                .BYTE $A0
                .BYTE $21 ; !
                .BYTE $A1
                .BYTE $21 ; !
                .BYTE $21 ; !
                .BYTE $A0
                .BYTE $A2
                .BYTE $A3
                .BYTE $21 ; !
                .BYTE $21 ; !
                .BYTE $A0
                .BYTE $21 ; !
                .BYTE $A1
                .BYTE $A2
                .BYTE $21 ; !
                .BYTE $AC
                .BYTE $A2
                .BYTE $83
                .BYTE $AF
                .BYTE $A7
                .BYTE $A5
                .BYTE $A6
                .BYTE $A4
                .BYTE $A7
                .BYTE $A6
                .BYTE $A4
                .BYTE $A6
                .BYTE $A2
                .BYTE $A1
                .BYTE $AD
                .BYTE $AF
                .BYTE $AE
                .BYTE $A0
                .BYTE $AE
                .BYTE $A2
                .BYTE $A7
                .BYTE $A2
                .BYTE $A6
                .BYTE $A3
                .BYTE $AC
                .BYTE $A2
                .BYTE $83
                .BYTE $A3
                .BYTE $A0
                .BYTE $A2
                .BYTE $83
                .BYTE $83
                .BYTE $C7
                .BYTE $CE
                .BYTE $83
                .BYTE $C5

; =============== S U B R O U T I N E =======================================


sub_1B09A:                              ; CODE XREF: sub_1AE99+AD↑j
                                        ; sub_1AE99+BF↑j ...
                LDA     $174
                STA     $76
                STY     $10
                TYA
                LSR
                LSR
                LSR
                LSR
                BNE     sub_1B0AA
                LDA     #$10
; End of function sub_1B09A


; =============== S U B R O U T I N E =======================================


sub_1B0AA:                              ; CODE XREF: sub_1B09A+C↑j
                                        ; sub_1B8E5+1B0↓p
                STA     $70
                LDA     $10
                AND     #$F
                BNE     loc_1B0B4
                LDA     #$10

loc_1B0B4:                              ; CODE XREF: sub_1B0AA+6↑j
                STA     $6F
                STA     $71
                LDA     $18
                AND     #$1F
                LSR
                STA     $72

loc_1B0BF:                              ; CODE XREF: sub_1B0AA+BB↓j
                LDA     $72
                STA     $74
                LDA     $14
                STA     $73
                LDA     $18
                STA     $75

loc_1B0CB:                              ; CODE XREF: sub_1B0AA+72↓j
                LDA     $15
                ASL
                ASL
                ASL
                STA     $10
                LDA     $74
                LSR
                CLC
                ADC     $10
                TAY
                LDA     $74
                LSR
                LDA     $4B0,Y
                BCS     loc_1B0E5
                LSR
                LSR
                LSR
                LSR

loc_1B0E5:                              ; CODE XREF: sub_1B0AA+35↑j
                AND     #$F
                BEQ     loc_1B10C
                CMP     #$F
                BNE     loc_1B0F5
                JSR     sub_1872B
                AND     #3
                CLC
                ADC     #$F

loc_1B0F5:                              ; CODE XREF: sub_1B0AA+41↑j
                ASL
                ASL
                ADC     #$3E ; '>'
                STA     $1A
                LDA     #$B0
                ADC     #0
                STA     $1B
                LDY     $76
                JSR     sub_1B20E
                LDA     $C0
                AND     #$10
                BEQ     loc_1B10C

loc_1B10C:                              ; CODE XREF: sub_1B0AA+3D↑j
                                        ; sub_1B0AA+60↑j
                INC     $74
                INC     $18
                INC     $18
                LDA     $18
                AND     #2
                BNE     loc_1B11A
                INC     $14

loc_1B11A:                              ; CODE XREF: sub_1B0AA+6C↑j
                DEC     $71
                BNE     loc_1B0CB
                LDA     $6F
                STA     $71
                LDA     $73
                STA     $14
                LDA     $15
                INC     $15
                CMP     #$E
                BEQ     loc_1B146
                LSR
                BCC     loc_1B138
                LDA     $14
                CLC
                ADC     #8
                STA     $14

loc_1B138:                              ; CODE XREF: sub_1B0AA+85↑j
                LDA     $75
                CLC
                ADC     #$40 ; '@'
                STA     $18
                BCC     loc_1B143
                INC     $19

loc_1B143:                              ; CODE XREF: sub_1B0AA+95↑j
                JMP     loc_1B161
; ---------------------------------------------------------------------------

loc_1B146:                              ; CODE XREF: sub_1B0AA+82↑j
                LDA     $14
                CLC
                ADC     #8
                AND     #$3F ; '?'
                STA     $14
                LDA     #0
                STA     $15
                LDA     $75
                AND     #$3F ; '?'
                STA     $18
                LDA     $19
                AND     #$28 ; '('
                EOR     #8
                STA     $19

loc_1B161:                              ; CODE XREF: sub_1B0AA:loc_1B143↑j
                DEC     $70
                BEQ     locret_1B168
                JMP     loc_1B0BF
; ---------------------------------------------------------------------------

locret_1B168:                           ; CODE XREF: sub_1B0AA+B9↑j
                RTS
; End of function sub_1B0AA


; =============== S U B R O U T I N E =======================================


sub_1B169:                              ; CODE XREF: sub_1AE99+A8↑p
                                        ; sub_1AE99+BA↑p ...
                STA     $10
                LDA     $5F8,X
                STA     $18
                LDA     $612,X
                STA     $19
                LDA     $62C,X
                STA     $15
                LDA     $646,X
                STA     $14
                BIT     $10
                BVC     loc_1B18F
                DEC     $18
                DEC     $18
                LDA     $18
                AND     #2
                BEQ     loc_1B18F
                DEC     $14

loc_1B18F:                              ; CODE XREF: sub_1B169+18↑j
                                        ; sub_1B169+22↑j
                BIT     $10
                BPL     locret_1B1AE
                LDA     $15
                BEQ     loc_1B1AF
                DEC     $15
                LSR
                BCS     loc_1B1A3
                LDA     $14
                SEC
                SBC     #8
                STA     $14

loc_1B1A3:                              ; CODE XREF: sub_1B169+31↑j
                LDA     $18
                SEC
                SBC     #$40 ; '@'
                STA     $18
                BCS     locret_1B1AE
                DEC     $19

locret_1B1AE:                           ; CODE XREF: sub_1B169+28↑j
                                        ; sub_1B169+41↑j
                RTS
; ---------------------------------------------------------------------------

loc_1B1AF:                              ; CODE XREF: sub_1B169+2C↑j
                LDA     #$E
                STA     $15
                LDA     $18
                AND     #$3F ; '?'
                ORA     #$80
                STA     $18
                LDA     $19
                EOR     #8
                ORA     #3
                STA     $19
                LDA     $14
                SEC
                SBC     #8
                AND     #$3F ; '?'
                STA     $14
                RTS
; End of function sub_1B169


; =============== S U B R O U T I N E =======================================


sub_1B1CD:                              ; CODE XREF: sub_1ACDE+24↑p
                                        ; sub_1B4D5+2F↓j
                LDA     $16F
; End of function sub_1B1CD


; =============== S U B R O U T I N E =======================================


sub_1B1D0:                              ; CODE XREF: sub_1B4D5+21↓j
                PHA
                TYA
                CLC
                ADC     #$C2
                STA     $1A
                LDA     #$AF
                ADC     #0
                STA     $1B
                PLA
                TAY
                JMP     sub_1B1FA
; End of function sub_1B1D0


; =============== S U B R O U T I N E =======================================


sub_1B1E2:                              ; CODE XREF: sub_1AE99+CF↑j
                LDA     $171
                JMP     sub_1B1EB
; End of function sub_1B1E2


; =============== S U B R O U T I N E =======================================


sub_1B1E8:                              ; CODE XREF: sub_1AE99+8C↑j
                                        ; sub_1AE99+FE↑j
                LDA     $174
; End of function sub_1B1E8


; =============== S U B R O U T I N E =======================================


sub_1B1EB:                              ; CODE XREF: sub_1B1E2+3↑j
                                        ; sub_1B507+12↓j ...
                PHA
                TYA
                CLC
                ADC     #$3E ; '>'
                STA     $1A
                LDA     #$B0
                ADC     #0
                STA     $1B
                PLA
                TAY
; End of function sub_1B1EB


; =============== S U B R O U T I N E =======================================


sub_1B1FA:                              ; CODE XREF: sub_1B1D0+F↑j
                LDA     $5F8,X
                STA     $18
                LDA     $612,X
                STA     $19
                LDA     $62C,X
                STA     $15
                LDA     $646,X
                STA     $14
; End of function sub_1B1FA


; =============== S U B R O U T I N E =======================================


sub_1B20E:                              ; CODE XREF: sub_1B0AA+59↑p
                STX     $65
                STY     $17
                LDA     #9
                JSR     sub_C04B
                JSR     sub_C03C
                TAX
                LDA     $19
                AND     #$F
                ORA     #$E0
                STA     $460,X
                LDA     $18

loc_1B226:
                STA     $461,X
                LDA     $14
                ORA     #$C0
                STA     $466,X
                LDA     $15
                AND     #1
                ASL
                TAY
                LDA     $18
                AND     #2
                BEQ     loc_1B23D
                INY

loc_1B23D:                              ; CODE XREF: sub_1B20E+2C↑j
                LDA     unk_1B260,Y
                STA     $468,X
                LDA     $17
                AND     unk_1B264,Y
                STA     $467,X
                LDY     #0

loc_1B24D:                              ; CODE XREF: sub_1B20E+48↓j
                LDA     ($1A),Y
                STA     $462,X
                INX
                INY
                CPY     #4
                BNE     loc_1B24D
                JSR     sub_C03F
                LDX     $65
                LDY     $17
                RTS
; End of function sub_1B20E

; ---------------------------------------------------------------------------
unk_1B260:      .BYTE $FC
                .BYTE $F3
                .BYTE $CF
                .BYTE $3F ; ?
unk_1B264:      .BYTE   3
                .BYTE  $C
                .BYTE $30 ; 0
                .BYTE $C0
                .BYTE $BD
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $30 ; 0
                .BYTE $1C
                .BYTE   9
                .BYTE $80
                .BYTE $9D
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $A9
                .BYTE $FF
                .BYTE $9D
                .BYTE $12
                .BYTE   6
                .BYTE $A9
                .BYTE $F8
                .BYTE $9D
                .BYTE $F8
                .BYTE   5
                .BYTE $9D
                .BYTE $E2
                .BYTE   6
                .BYTE $A9
                .BYTE   1
                .BYTE $9D
                .BYTE $60 ; `
                .BYTE   6
                .BYTE $A9
                .BYTE   1
                .BYTE $9D
                .BYTE $FC
                .BYTE   6
                .BYTE $DE
                .BYTE $E2
                .BYTE   6
                .BYTE $D0
                .BYTE $14
                .BYTE $BD
                .BYTE $FC
                .BYTE   6
                .BYTE $F0
                .BYTE  $F
                .BYTE $DE
                .BYTE $FC
                .BYTE   6
                .BYTE $D0
                .BYTE  $A
                .BYTE $BD
                .BYTE $7A ; z
                .BYTE   6
                .BYTE $C9
                .BYTE $46 ; F
                .BYTE $D0
                .BYTE   3
                .BYTE $DE
                .BYTE $FC
                .BYTE   6
                .BYTE $BD
                .BYTE $E2
                .BYTE   6
                .BYTE $29 ; )
                .BYTE   3
                .BYTE $9D
                .BYTE $90
                .BYTE   5
                .BYTE $18
                .BYTE $69 ; i
                .BYTE $3C ; <
                .BYTE $9D
                .BYTE $76 ; v
                .BYTE   5
                .BYTE $20
                .BYTE $B1
                .BYTE $82
                .BYTE $BD
                .BYTE $90
                .BYTE   5
                .BYTE $29 ; )
                .BYTE   1
                .BYTE $18
                .BYTE $69 ; i
                .BYTE $36 ; 6
                .BYTE $9D
                .BYTE $76 ; v
                .BYTE   5
                .BYTE $BD
                .BYTE $FC
                .BYTE   6
                .BYTE $F0
                .BYTE   3
                .BYTE $20
                .BYTE $D9
                .BYTE $82
                .BYTE $20
                .BYTE $ED
                .BYTE $81
                .BYTE $BD
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $10
                .BYTE   1
                .BYTE $60 ; `
                .BYTE $A9
                .BYTE $81
                .BYTE $8D
                .BYTE $28 ; (
                .BYTE   5
                .BYTE $BD
                .BYTE $FC
                .BYTE   6
                .BYTE $F0
                .BYTE   8
                .BYTE $20
                .BYTE $ED
                .BYTE $B2
                .BYTE $A9
                .BYTE $13
                .BYTE $4C ; L
                .BYTE $30 ; 0
                .BYTE $C0
                .BYTE $BD
                .BYTE $16
                .BYTE   7
                .BYTE $85
                .BYTE $92
                .BYTE $A5
                .BYTE $30 ; 0
                .BYTE   9
                .BYTE $20
                .BYTE $85
                .BYTE $30 ; 0
                .BYTE $60 ; `

; =============== S U B R O U T I N E =======================================


sub_1B2ED:                              ; CODE XREF: sub_19410:loc_19480↑p
                                        ; sub_1AD36+5F↑p ...
                LDA     #$23 ; '#'

loc_1B2EF:                              ; CODE XREF: sub_19410+67↑p
                PHA
                LDA     $145
                PHA
                LDA     #$30 ; '0'
                JSR     sub_1B32E
                LDY     #5
                JSR     j_all_enemies_destroyed
                PLA
                STA     $10
                LDY     #5

loc_1B303:                              ; CODE XREF: sub_1B2ED+38↓j
                LDA     $528,Y
                AND     #$7F
                BEQ     loc_1B322
                CMP     #$46 ; 'F'
                BCS     loc_1B322
                CMP     #$28 ; '('
                BEQ     loc_1B322
                CMP     #$30 ; '0'
                BCC     loc_1B31A
                CMP     #$38 ; '8'
                BCC     loc_1B322

loc_1B31A:                              ; CODE XREF: sub_1B2ED+27↑j
                STA     $7E6,Y
                PLA
                PHA
                STA     $528,Y

loc_1B322:                              ; CODE XREF: sub_1B2ED+1B↑j
                                        ; sub_1B2ED+1F↑j ...
                INY
                CPY     #$1A
                BNE     loc_1B303
                LDA     $10
                JSR     sub_1B32E
                PLA
                RTS
; End of function sub_1B2ED


; =============== S U B R O U T I N E =======================================


sub_1B32E:                              ; CODE XREF: sub_1B2ED+9↑p
                                        ; sub_1B2ED+3C↑p ...
                STA     byte_145
                LDA     #3
                STA     cram_update_mode
                RTS
; End of function sub_1B32E


; =============== S U B R O U T I N E =======================================


sub_1B337:
                LDA     $528,X
                BPL     sub_1B355
                JSR     sub_182D9
                JMP     loc_1B3A5
; End of function sub_1B337


; =============== S U B R O U T I N E =======================================


sub_1B342:
                JSR     sub_1B8A3
                LDA     $528,X
                BPL     loc_1B352
                LDA     $542,X
                CMP     #$F0
                JMP     loc_1B39E
; ---------------------------------------------------------------------------

loc_1B352:                              ; CODE XREF: sub_1B342+6↑j
                BCS     sub_1B355
                RTS
; End of function sub_1B342


; =============== S U B R O U T I N E =======================================


sub_1B355:                              ; CODE XREF: sub_1B337+3↑j
                                        ; sub_1B342:loc_1B352↑j ...
                LDA     #3
                STA     $590,X
                LDA     #0
                STA     $5AA,X
                STA     $6AE,X
                STA     $6FC,X
                STA     $5C4,X
                STA     $716,X
                STA     $730,X
                LDA     $528,X
                ORA     #$80
                STA     $528,X
                SEC
                SBC     #$C9
                STA     $10
                ASL
                ADC     $10
                TAY
                LDA     byte_1B646,Y
                STA     $576,X
                LDA     byte_1B646+1,Y
                STA     $764,X
                LDA     byte_1B646+2,Y
                STA     $6E2,X
                INC     $C1
                INC     $C2
                LDA     $C1
                STA     $6C8,X
                JSR     sub_1B667
                CLC

loc_1B39E:                              ; CODE XREF: sub_1B342+D↑j
                BCC     loc_1B3A5
                DEC     $C2
                JMP     sub_182FE
; ---------------------------------------------------------------------------

loc_1B3A5:                              ; CODE XREF: sub_1B337+8↑j
                                        ; sub_1B355:loc_1B39E↑j
                LDA     $C0
                ORA     #$80
                STA     $C0
                AND     #6
                BNE     loc_1B3B0
                RTS
; ---------------------------------------------------------------------------

loc_1B3B0:                              ; CODE XREF: sub_1B355+58↑j
                LDA     $5AA,X
                AND     #2
                BEQ     loc_1B3BA
                JMP     sub_1B482
; ---------------------------------------------------------------------------

loc_1B3BA:                              ; CODE XREF: sub_1B355+60↑j
                LDA     $528,X
                CMP     #$CF
                BNE     loc_1B3C4
                JMP     loc_1B41B
; ---------------------------------------------------------------------------

loc_1B3C4:                              ; CODE XREF: sub_1B355+6A↑j
                LDA     $C0
                AND     #8
                BEQ     loc_1B3E3
                LDA     $5AA,X
                AND     #$FE
                CMP     $5AA,X
                BNE     loc_1B3DC
                ORA     #1
                STA     $5AA,X
                ASL     $6E2,X

loc_1B3DC:                              ; CODE XREF: sub_1B355+7D↑j
                LDA     $6AE,X
                CMP     #3
                BEQ     loc_1B41B

loc_1B3E3:                              ; CODE XREF: sub_1B355+73↑j
                LDA     $67A,X
                CLC
                ADC     $694,X
                STA     $694,X
                BCC     loc_1B411
                INC     $6AE,X
                LDA     $6AE,X
                CMP     #6
                BNE     loc_1B3FE
                LDA     #0
                STA     $6AE,X

loc_1B3FE:                              ; CODE XREF: sub_1B355+A2↑j
                TAY
                LDA     $B65B,Y
                STA     $716,X
                LDA     $B661,Y
                STA     $730,X
                JSR     sub_1B667
                JSR     sub_1B4D5

loc_1B411:                              ; CODE XREF: sub_1B355+98↑j
                LDA     $730,X
                BNE     loc_1B417
                RTS
; ---------------------------------------------------------------------------

loc_1B417:                              ; CODE XREF: sub_1B355+BF↑j
                CMP     #3
                BNE     loc_1B42A

loc_1B41B:                              ; CODE XREF: sub_1B355+6C↑j
                                        ; sub_1B355+8C↑j
                LDA     $6E2,X
                CLC
                ADC     $6FC,X
                STA     $6FC,X
                BCC     loc_1B42A
                JSR     sub_1B533

loc_1B42A:                              ; CODE XREF: sub_1B355+C4↑j
                                        ; sub_1B355+D0↑j
                LDA     $528,X
                CMP     #$CF
                BNE     loc_1B434
                JSR     sub_1B44B

loc_1B434:                              ; CODE XREF: sub_1B355+DA↑j
                JSR     sub_18204
                JSR     sub_1A2B1
                BEQ     sub_1B467
                CMP     #$CF
                BEQ     loc_1B441
                RTS
; ---------------------------------------------------------------------------

loc_1B441:                              ; CODE XREF: sub_1B355+E9↑j
                LDA     #$11
                JSR     sub_C030
                LDA     #$F
                JSR     sub_1B491
; End of function sub_1B355


; =============== S U B R O U T I N E =======================================


sub_1B44B:                              ; CODE XREF: sub_1B355+DC↑p
                LDY     #$50 ; 'P'
                LDA     $764,X
                CMP     #$1E
                BCS     loc_1B462
                LDY     #$51 ; 'Q'
                CMP     #$14
                BCS     loc_1B462
                LDY     #$52 ; 'R'
                CMP     #$A
                BCS     loc_1B462
                LDY     #$53 ; 'S'

loc_1B462:                              ; CODE XREF: sub_1B44B+7↑j
                                        ; sub_1B44B+D↑j ...
                TYA
                STA     $576,X
                RTS
; End of function sub_1B44B


; =============== S U B R O U T I N E =======================================


sub_1B467:                              ; CODE XREF: sub_1B355+E5↑j
                LDA     $88
                CMP     #$4F ; 'O'
                BNE     loc_1B478
                LDA     #$CF
                STA     $528,X
                LDA     #2
                STA     $5AA,X
                RTS
; ---------------------------------------------------------------------------

loc_1B478:                              ; CODE XREF: sub_1B467+4↑j
                                        ; sub_1B482+3↓j
                LDA     #$50 ; 'P'
                STA     $528,X
                DEC     $C2
                JMP     sub_1B507
; End of function sub_1B467


; =============== S U B R O U T I N E =======================================


sub_1B482:                              ; CODE XREF: sub_1B355+62↑j
                DEC     $764,X
                BEQ     loc_1B478
                LDA     $764,X
                AND     #3
                BEQ     loc_1B48F
                RTS
; ---------------------------------------------------------------------------

loc_1B48F:                              ; CODE XREF: sub_19CAB+16↑j
                                        ; sub_1B482+A↑j
                LDA     #$1F
; End of function sub_1B482


; =============== S U B R O U T I N E =======================================


sub_1B491:                              ; CODE XREF: sub_1B355+F3↑p
                STA     $14
                STA     $15
                LDA     $55C,X
                STA     $19
                LDA     $542,X
                STA     $18
; End of function sub_1B491


; =============== S U B R O U T I N E =======================================


sub_1B49F:                              ; CODE XREF: sub_1BCE0+29↓p
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_1B4D4
                LDA     #$23 ; '#'
                STA     $528,Y
                JSR     sub_1872B
                AND     $14
                PHA
                LDA     $14
                LSR
                STA     $10
                PLA
                SBC     $10
                ADC     $18
                STA     $542,Y
                JSR     sub_1872B
                AND     $15
                PHA
                LDA     $15
                LSR
                STA     $10
                PLA
                SBC     $10
                ADC     $19
                STA     $55C,Y
                LDA     #0
                STA     $7E6,Y

locret_1B4D4:                           ; CODE XREF: sub_1B49F+3↑j
                RTS
; End of function sub_1B49F


; =============== S U B R O U T I N E =======================================


sub_1B4D5:                              ; CODE XREF: sub_1B355+B9↑p
                LDA     $528,X
                CMP     #$CF
                BNE     loc_1B4DD

locret_1B4DC:                           ; CODE XREF: sub_1B507+5↓j
                RTS
; ---------------------------------------------------------------------------

loc_1B4DD:                              ; CODE XREF: sub_1B4D5+5↑j
                CMP     #$CB
                BNE     loc_1B4F9
                LDA     $74A,X
                CMP     #$12
                LDA     $528,X
                BCC     loc_1B4F9
                LDA     $730,X
                ASL
                ASL
                ADC     #$60 ; '`'
                TAY
                LDA     $171
                JMP     sub_1B1D0
; ---------------------------------------------------------------------------

loc_1B4F9:                              ; CODE XREF: sub_1B4D5+A↑j
                                        ; sub_1B4D5+14↑j
                SEC
                SBC     #$C9
                ASL
                ASL
                ADC     $730,X
                ASL
                ASL
                TAY
                JMP     sub_1B1CD
; End of function sub_1B4D5


; =============== S U B R O U T I N E =======================================


sub_1B507:                              ; CODE XREF: sub_1B467+18↑j
                LDA     $7E6,X
                CMP     #$4F ; 'O'
                BEQ     locret_1B4DC
                CMP     #$4B ; 'K'
                BEQ     loc_1B51C
                BCS     loc_1B52B
                LDY     #4
                LDA     $171
                JMP     sub_1B1EB
; ---------------------------------------------------------------------------

loc_1B51C:                              ; CODE XREF: sub_1B507+9↑j
                LDY     #$58 ; 'X'
                LDA     $74A,X
                CMP     #4
                BEQ     loc_1B52B
                LDA     $171
                JMP     sub_1B1EB
; ---------------------------------------------------------------------------

loc_1B52B:                              ; CODE XREF: sub_1B507+B↑j
                                        ; sub_1B507+1C↑j
                LDY     #8
                LDA     $171
                JMP     sub_1B1EB
; End of function sub_1B507


; =============== S U B R O U T I N E =======================================


sub_1B533:                              ; CODE XREF: sub_1B355+D2↑p
                LDA     $528,X
                SEC
                SBC     #$C9
                JSR     j_switch_case
; End of function sub_1B533

; ---------------------------------------------------------------------------
                .WORD $B549
                .WORD $B575
                .WORD $B591
                .WORD $B5AF
                .WORD $B599
                .WORD $B5BD
                .WORD $B610

; =============== S U B R O U T I N E =======================================


sub_1B54A:
                LDA     $660,X

loc_1B54D:                              ; CODE XREF: sub_1B54A+15↓j
                CLC
                ADC     #3
                STA     $660,X
                AND     #$F
                CMP     #$F
                BCS     loc_1B56E
                CMP     #$E
                BCS     sub_1B592
                CMP     #9
                BCS     loc_1B54D
                STA     $14
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCC     loc_1B569

locret_1B568:                           ; CODE XREF: sub_1B54A+27↓j
                                        ; sub_1B576+3↓j ...
                RTS
; ---------------------------------------------------------------------------

loc_1B569:                              ; CODE XREF: sub_1B54A+1C↑j
                LDA     #$15
                JMP     sub_1B631
; ---------------------------------------------------------------------------

loc_1B56E:                              ; CODE XREF: sub_1B54A+D↑j
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_1B568
                JMP     loc_1B62B
; End of function sub_1B54A


; =============== S U B R O U T I N E =======================================


sub_1B576:
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_1B568
                TYA
                PHA
                JSR     sub_1802A
                STA     $14
                PLA
                TAY
                LDA     #6
                STA     $74A,Y
                TXA
                STA     $5AA,Y
                LDA     #$29 ; ')'
                JMP     sub_1B631
; End of function sub_1B576


; =============== S U B R O U T I N E =======================================


sub_1B592:                              ; CODE XREF: sub_1B54A+11↑j
                                        ; sub_1B611+A↓j
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_1B568
                JMP     loc_1B62B
; End of function sub_1B592


; =============== S U B R O U T I N E =======================================


sub_1B59A:
                LDA     $660,X
                INC     $660,X
                STA     $14
                LDA     #4
                STA     $16
                LDA     #3
                STA     $17
                LDA     #$15
                STA     $18
                BNE     sub_1B5CF
; End of function sub_1B59A


; =============== S U B R O U T I N E =======================================


sub_1B5B0:
                LDA     #3
                STA     $16
                LDA     #0
                STA     $17
                LDA     #$3B ; ';'
                STA     $18
                BNE     loc_1B5CA
; End of function sub_1B5B0


; =============== S U B R O U T I N E =======================================


sub_1B5BE:
                LDA     #5
                STA     $16
                LDA     #7
                STA     $17
                LDA     #$2B ; '+'
                STA     $18

loc_1B5CA:                              ; CODE XREF: sub_1B5B0+C↑j
                JSR     sub_1802A
                STA     $14
; End of function sub_1B5BE


; =============== S U B R O U T I N E =======================================


sub_1B5CF:                              ; CODE XREF: sub_19DAF+51↑p
                                        ; sub_1B59A+14↑j
                LDA     #0
                STA     $15

loc_1B5D3:                              ; CODE XREF: sub_1B5CF+23↓j
                LDA     $15
                CLC
                ADC     $17
                TAY
                LDA     $14
                CLC
                ADC     $B5F5,Y
                STA     $14
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCC     loc_1B5E7

locret_1B5E6:                           ; CODE XREF: sub_1B5CF+21↓j
                                        ; sub_1B611+14↓j
                RTS
; ---------------------------------------------------------------------------

loc_1B5E7:                              ; CODE XREF: sub_1B5CF+15↑j
                LDA     $18
                JSR     sub_1B631
                INC     $15
                DEC     $16
                BEQ     locret_1B5E6
                JMP     loc_1B5D3
; End of function sub_1B5CF

; ---------------------------------------------------------------------------
                .BYTE   0
                .BYTE $FE
                .BYTE   4
                .BYTE   0
                .BYTE   4
                .BYTE   4
                .BYTE   4
                .BYTE   0
                .BYTE $FF
                .BYTE   2
                .BYTE $FD
                .BYTE   4
                .BYTE   0
                .BYTE $FF
                .BYTE   2
                .BYTE $FD
                .BYTE   4
                .BYTE $FB
                .BYTE   6
                .BYTE $F9
                .BYTE   8
                .BYTE $F7
                .BYTE  $A
                .BYTE $F5
                .BYTE  $C
                .BYTE $F3
                .BYTE  $E
                .BYTE $F1

; =============== S U B R O U T I N E =======================================


sub_1B611:                              ; CODE XREF: sub_1B7B8+4D↓p
                INC     $660,X
                LDA     $660,X
                AND     #3
                BNE     loc_1B61E
                JMP     sub_1B592
; ---------------------------------------------------------------------------

loc_1B61E:                              ; CODE XREF: sub_1B611+8↑j
                LDA     #4
                STA     $14
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     locret_1B5E6
                LDA     #$2D ; '-'
                BNE     sub_1B631

loc_1B62B:                              ; CODE XREF: sub_1B54A+29↑j
                                        ; sub_1B592+5↑j
                LDA     #$2A ; '*'
                BNE     sub_1B631
                LDA     #$2B ; '+'
; End of function sub_1B611


; =============== S U B R O U T I N E =======================================


sub_1B631:                              ; CODE XREF: sub_18F78+44↑p
                                        ; sub_1961B+BD↑j ...
                STA     $528,Y
                LDA     $14
                STA     $77E,Y
                LDA     $542,X
                STA     $542,Y
                LDA     $55C,X
                STA     $55C,Y
                RTS
; End of function sub_1B631

; ---------------------------------------------------------------------------
byte_1B646:     .BYTE 7, $28, $7F
                .BYTE 7, $1E, 6
                .BYTE 6, $A, 8
                .BYTE 7, $14, 6
                .BYTE 7, $14, 6
                .BYTE 7, $28, 6
                .BYTE $50, $63, $12
                .BYTE 0, 1, 1
                .BYTE 2, 1, 1
                .BYTE 0, 1, 2
                .BYTE 3, 2, 1

; =============== S U B R O U T I N E =======================================


sub_1B667:                              ; CODE XREF: sub_1B355+45↑p
                                        ; sub_1B355+B6↑p
                LDA     $6C8,X
                AND     #7
                STA     $10
                ASL
                ADC     $10
                ASL
                ADC     $716,X
                ADC     $5C4,X
                TAY
                LDA     unk_1B8AD,Y
                STA     $67A,X
                LDA     $716,X
                CMP     #2
                BNE     loc_1B695
                LDA     $5C4,X
                CLC
                ADC     #3
                CMP     #6
                BNE     loc_1B692
                LDA     #0

loc_1B692:                              ; CODE XREF: sub_1B667+27↑j
                STA     $5C4

loc_1B695:                              ; CODE XREF: sub_1B667+1D↑j
                LDA     #0
                STA     $694,X
                RTS
; End of function sub_1B667


; =============== S U B R O U T I N E =======================================


sub_1B69B:
                LDA     $528,X
                BMI     loc_1B6B0
                ORA     #$80
                STA     $528,X
                JSR     sub_18796
                LDA     #$12
                JSR     $C030
                JMP     loc_1A93F
; ---------------------------------------------------------------------------

loc_1B6B0:                              ; CODE XREF: sub_1B69B+3↑j
                JSR     loc_1B895
                JMP     loc_1A96C
; End of function sub_1B69B


; =============== S U B R O U T I N E =======================================


sub_1B6B6:
                LDA     $528,X
                BMI     loc_1B6D9
                ORA     #$80
                STA     $528,X
                LDA     #1
                STA     $660,X
                LDA     #$FF
                STA     $612,X
                LDA     #$E0
                STA     $5F8,X
                LDA     #3
                STA     $590,X
                LDA     #$35 ; '5'
                STA     $576,X

loc_1B6D9:                              ; CODE XREF: sub_1B6B6+3↑j
                JSR     sub_182B1
                LDA     #$FC
                STA     $87
                LDY     #3
                LDA     $5AA,X
                BEQ     loc_1B6E9
                LDY     #2

loc_1B6E9:                              ; CODE XREF: sub_1B6B6+2F↑j
                STY     $86
                LDA     #$F6
                STA     $84
                LDA     $764,X
                ASL
                STA     $85
                JSR     sub_1860C
                JSR     sub_181ED
                LDA     $528,X
                BPL     loc_1B701
                RTS
; ---------------------------------------------------------------------------

loc_1B701:                              ; CODE XREF: sub_1B6B6+48↑j
                LDA     #$81
                STA     $528
                LDA     #0
                STA     $764
                LDA     #$80
                STA     $5AA
                LDA     $5AA,X
                BNE     loc_1B720
                LDA     $5A
                SEC
                SBC     #5
                BCS     loc_1B71E
                LDA     #0

loc_1B71E:                              ; CODE XREF: sub_1B6B6+64↑j
                STA     $5A

loc_1B720:                              ; CODE XREF: sub_1B6B6+5D↑j
                JSR     sub_1879F
                LDA     $764,X
                JSR     sub_19493
                JMP     sub_182FE
; End of function sub_1B6B6


; =============== S U B R O U T I N E =======================================


sub_1B72C:
                JSR     sub_1B87F
                BCC     loc_1B73E
                LDA     #3
                STA     $6E2,X
                LDA     #$18
                STA     $6FC,X
                JMP     loc_1AEC6
; ---------------------------------------------------------------------------

loc_1B73E:                              ; CODE XREF: sub_1B72C+3↑j
                DEC     $6E2,X
                BNE     loc_1B775
                LDA     $6FC,X
                STA     $6E2,X
                JSR     check_enemy_overflow ; Checks that the enemy list is complete. If overflowed, sets Carry flag.
                BCS     loc_1B775
                LDA     $528,X
                CMP     #$D4
                BEQ     loc_1B79F
                CMP     #$D5
                BEQ     loc_1B778
                CMP     #$D7
                BEQ     loc_1B7A3
                LDA     #8
                STA     $6E2,X
                INC     $660,X
                LDA     $660,X
                AND     #3
                ASL
                ASL
                ADC     #2
                STA     $14

loc_1B770:                              ; CODE XREF: sub_1B72C+68↓j
                                        ; sub_1B72C+71↓j
                LDA     #$15

loc_1B772:                              ; CODE XREF: sub_1B72C+75↓j
                                        ; sub_1B72C+86↓j
                JSR     sub_1B631

loc_1B775:                              ; CODE XREF: sub_1B72C+15↑j
                                        ; sub_1B72C+20↑j
                JMP     loc_1AED8
; ---------------------------------------------------------------------------

loc_1B778:                              ; CODE XREF: sub_1B72C+2B↑j
                LDA     $55C
                CMP     $55C,X
                LDA     #0
                STA     $14
                LDA     #1
                STA     $15
                BCS     loc_1B790
                LDA     #8
                STA     $14
                LDA     #$FF
                STA     $15

loc_1B790:                              ; CODE XREF: sub_1B72C+5A↑j
                LDA     $AA
                ORA     $AB
                BEQ     loc_1B770
                LDA     $14
                CLC
                ADC     $15
                STA     $14
                BNE     loc_1B770

loc_1B79F:                              ; CODE XREF: sub_1B72C+27↑j
                LDA     #$25 ; '%'
                BNE     loc_1B772

loc_1B7A3:                              ; CODE XREF: sub_1B72C+2F↑j
                STY     $10
                LDA     $74A,X
                TAY
                LDA     $B7A7,Y
                STA     $14
                LDY     $10
                LDA     #$15
                BNE     loc_1B772
; End of function sub_1B72C

; ---------------------------------------------------------------------------
                .BYTE  $A
                .BYTE   6
                .BYTE  $E
                .BYTE   2

; =============== S U B R O U T I N E =======================================


sub_1B7B8:
                LDA     $528,X
                BMI     loc_1B7D4
                ORA     #$80
                STA     $528,X
                LDA     #$A
                STA     $764,X
                STA     $694,X
                LDA     #8
                STA     $576,X
                LDA     #0
                STA     $6AE,X

loc_1B7D4:                              ; CODE XREF: sub_1B7B8+3↑j
                LDA     $52B,X
                CMP     #$CF
                BEQ     loc_1B7DE
                JMP     loc_1B860
; ---------------------------------------------------------------------------

loc_1B7DE:                              ; CODE XREF: sub_1B7B8+21↑j
                LDA     $767,X
                LDY     #0
                CMP     #$28 ; '('
                BCS     loc_1B7FB
                LDY     #$18
                CMP     #$1E
                BCS     loc_1B7FB
                LDY     #$30 ; '0'
                CMP     #$14
                BCS     loc_1B7FB
                LDY     #$60 ; '`'
                CMP     #$A
                BCS     loc_1B7FB
                LDY     #$F0

loc_1B7FB:                              ; CODE XREF: sub_1B7B8+2D↑j
                                        ; sub_1B7B8+33↑j ...
                TYA
                CLC
                ADC     $694,X
                STA     $694,X
                BCC     loc_1B860
                JSR     sub_1B611
                INC     $767,X
                INC     $6AE,X
                LDA     $6AE,X
                AND     #3
                TAY
                LDA     $646,X
                ORA     #$C0
                STA     $14
                LDA     #5
                JSR     sub_C039
                BCS     loc_1B860
                JSR     sub_C03C
                STX     $10
                TAX
                LDA     #2
                STA     $460,X
                LDA     #$23 ; '#'
                STA     $461,X
                LDA     $14
                STA     $462,X
                LDA     $10
                CMP     #$14
                BEQ     loc_1B84F
                LDA     unk_1B873,Y
                STA     $463,X
                DEC     $460,X
                DEC     $25
                LDX     $10
                JSR     sub_C03F
                BNE     loc_1B860

loc_1B84F:                              ; CODE XREF: sub_1B7B8+83↑j
                LDA     $B877,Y
                STA     $463,X
                LDA     $B87B,Y
                STA     $464,X
                LDX     $10
                JSR     sub_C03F

loc_1B860:                              ; CODE XREF: sub_1B7B8+23↑j
                                        ; sub_1B7B8+4B↑j ...
                JSR     sub_18204
                JSR     sub_1A2B1
                BEQ     loc_1B869
                RTS
; ---------------------------------------------------------------------------

loc_1B869:                              ; CODE XREF: sub_1B7B8+AE↑j
                LDA     #0
                JSR     sub_1B169
                LDY     #$22 ; '"'
                JMP     sub_1B09A
; End of function sub_1B7B8

; ---------------------------------------------------------------------------
unk_1B873:      .BYTE $AA
                .BYTE $FF
                .BYTE   0
                .BYTE $55 ; U
                .BYTE $BB
                .BYTE $FF
                .BYTE $33 ; 3
                .BYTE $77 ; w
                .BYTE $EE
                .BYTE $FF
                .BYTE $CC
                .BYTE $DD

; =============== S U B R O U T I N E =======================================


sub_1B87F:                              ; CODE XREF: sub_1ACDE↑p
                                        ; sub_1AD25↑p ...
                LDA     $528,X
                BMI     loc_1B895
                JSR     sub_1B8A3
                BCS     loc_1B88C
                PLA
                PLA
                RTS
; ---------------------------------------------------------------------------

loc_1B88C:                              ; CODE XREF: sub_1B87F+8↑j
                LDA     $528,X
                ORA     #$80
                STA     $528,X
                RTS
; ---------------------------------------------------------------------------

loc_1B895:                              ; CODE XREF: sub_1B69B:loc_1B6B0↑p
                                        ; sub_1B87F+3↑j
                JSR     sub_1B8A3
                BCS     loc_1B89B
                RTS
; ---------------------------------------------------------------------------

loc_1B89B:                              ; CODE XREF: sub_1B87F+19↑j
                PLA
                PLA
                JSR     sub_18788
                JMP     sub_182FE
; End of function sub_1B87F


; =============== S U B R O U T I N E =======================================


sub_1B8A3:                              ; CODE XREF: sub_1B342↑p
                                        ; sub_1B87F+5↑p ...
                LDA     $AE
                CLC
                ADC     $542,X
                STA     $542,X
                RTS
; End of function sub_1B8A3

; ---------------------------------------------------------------------------
unk_1B8AD:      .BYTE   4
                .BYTE $30 ; 0
                .BYTE   3
                .BYTE   4
                .BYTE $30 ; 0
                .BYTE   2
                .BYTE   4
                .BYTE $20
                .BYTE   4
                .BYTE   4
                .BYTE $20
                .BYTE   3
                .BYTE   4
                .BYTE $1C
                .BYTE   3
                .BYTE   4
                .BYTE $30 ; 0
                .BYTE   4
                .BYTE   4
                .BYTE $28 ; (
                .BYTE   5
                .BYTE   4
                .BYTE $38 ; 8
                .BYTE   2
                .BYTE   5
                .BYTE $40 ; @
                .BYTE   8
                .BYTE  $A
                .BYTE $30 ; 0
                .BYTE   6
                .BYTE   4
                .BYTE $10
                .BYTE   9
                .BYTE   3
                .BYTE $20
                .BYTE   5
                .BYTE   8
                .BYTE $20
                .BYTE   8
                .BYTE   4
                .BYTE $28 ; (
                .BYTE   6
                .BYTE   3
                .BYTE $20
                .BYTE   8
                .BYTE   6
                .BYTE $50 ; P
                .BYTE   6
unk_1B8DD:      .BYTE   6
                .BYTE $16
                .BYTE $17
                .BYTE $26 ; &
                .BYTE $27 ; '
                .BYTE $26 ; &
                .BYTE $17
                .BYTE $16

; =============== S U B R O U T I N E =======================================


sub_1B8E5:                              ; CODE XREF: sub_1801E↑j
                LDA     $C0
                AND     #$A0
                BEQ     loc_1B90F
                INC     $CC
                LDA     $CC
                AND     #7
                BNE     loc_1B90F
                LDA     $C0
                AND     #$7F
                STA     $C0
                LDA     $CC
                LSR
                LSR
                LSR
                AND     #7
                TAY
                LDA     unk_1B8DD,Y
                STA     $14B
                LDA     #1
                ORA     $144
                STA     $144

loc_1B90F:                              ; CODE XREF: sub_1B8E5+4↑j
                                        ; sub_1B8E5+C↑j
                LDA     $C0
                AND     #$20 ; ' '
                BNE     locret_1B924
                LDA     $C0
                AND     #2
                BEQ     loc_1B91E
                JMP     loc_1B9AC
; ---------------------------------------------------------------------------

loc_1B91E:                              ; CODE XREF: sub_1B8E5+34↑j
                LDA     $C0
                AND     #1
                BNE     loc_1B925

locret_1B924:                           ; CODE XREF: sub_1B8E5+2E↑j
                                        ; sub_1B8E5+44↓j ...
                RTS
; ---------------------------------------------------------------------------

loc_1B925:                              ; CODE XREF: sub_1B8E5+3D↑j
                LDA     $90
                AND     #1
                BEQ     locret_1B924
                DEC     $C6
                BEQ     loc_1B95F
                LDA     $C6
                CMP     #8
                BCS     locret_1B924
                ASL
                TAY
                LDA     $B950,Y
                CMP     $AB
                BEQ     loc_1B940
                BCS     locret_1B924

loc_1B940:                              ; CODE XREF: sub_1B8E5+57↑j
                LDA     $B94F,Y
                BCC     loc_1B949
                CMP     $AA
                BCS     locret_1B924

loc_1B949:                              ; CODE XREF: sub_1B8E5+5E↑j
                STA     $AA
                LDA     $B950,Y
                STA     $AB
                RTS
; ---------------------------------------------------------------------------
                .BYTE $40 ; @
                .BYTE   0
                .BYTE $80
                .BYTE   0
                .BYTE $C0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE $80
                .BYTE   1
                .BYTE   0
                .BYTE   2
                .BYTE $80
                .BYTE   2
; ---------------------------------------------------------------------------

loc_1B95F:                              ; CODE XREF: sub_1B8E5+48↑j
                LDA     #0
                STA     $AA
                STA     $AB
                LDA     $C7
                AND     #$20 ; ' '
                BEQ     loc_1B96E
                JSR     $C072

loc_1B96E:                              ; CODE XREF: sub_1B8E5+84↑j
                LDA     #$60 ; '`'
                STA     $CA
                LDA     #$3C ; '<'
                STA     $C4
                LDA     $C5
                STA     $C9
                LDA     $C7
                AND     #$1F
                CMP     #$10
                BCC     loc_1B988
                LDY     #0
                STY     $AC
                STY     $AD

loc_1B988:                              ; CODE XREF: sub_1B8E5+9B↑j
                LDA     #2
                STA     $C0
                LDA     $C8
                AND     #2
                BNE     loc_1B99C
                LDA     $62
                ORA     #8
                STA     $62
                LDA     $C5
                STA     $C9

loc_1B99C:                              ; CODE XREF: sub_1B8E5+AB↑j
                LDA     $C7
                AND     #$1F
                CMP     #$10
                BCS     loc_1B9AC
                LDA     $30
                BMI     loc_1B9AC
                LDY     #8
                STY     $66

loc_1B9AC:                              ; CODE XREF: sub_1B8E5+36↑j
                                        ; sub_1B8E5+BD↑j ...
                LDA     $CA
                BEQ     loc_1B9BF
                DEC     $CA
                BNE     loc_1B9BF
                LDA     $C7
                AND     #$20 ; ' '
                BEQ     loc_1B9BF
                LDA     #$1A
                JSR     $C030

loc_1B9BF:                              ; CODE XREF: sub_1B8E5+C9↑j
                                        ; sub_1B8E5+CD↑j ...
                LDA     $C7
                AND     #$1F
                CMP     #$10
                BCS     loc_1B9EB
                DEC     $C4
                BNE     loc_1B9EB
                LDA     #$3C ; '<'
                STA     $C4
                DEC     $C5
                BNE     loc_1B9D6
                JMP     sub_1BE08
; ---------------------------------------------------------------------------

loc_1B9D6:                              ; CODE XREF: sub_1B8E5+EC↑j
                LDA     $C9
                SEC
                SBC     #3
                STA     $C9
                BCC     loc_1B9EB
                LDA     $C8
                AND     #1
                BEQ     loc_1B9EB
                LDA     $62
                AND     #$F7
                STA     $62

loc_1B9EB:                              ; CODE XREF: sub_1B8E5+E0↑j
                                        ; sub_1B8E5+E4↑j ...
                LDA     $C2
                BEQ     loc_1B9FD
                CMP     $C3
                BEQ     loc_1B9F6
                BCC     loc_1B9F6
                RTS
; ---------------------------------------------------------------------------

loc_1B9F6:                              ; CODE XREF: sub_1B8E5+10C↑j
                                        ; sub_1B8E5+10E↑j
                LDA     $C0
                ORA     #8
                STA     $C0
                RTS
; ---------------------------------------------------------------------------

loc_1B9FD:                              ; CODE XREF: sub_1B8E5+108↑j
                LDA     $47
                LSR
                LSR
                STA     $10
                LDA     $47
                SEC
                SBC     $10
                STA     $47
                LDA     $4B
                SBC     #8
                BCS     loc_1BA12
                LDA     #0

loc_1BA12:                              ; CODE XREF: sub_1B8E5+129↑j
                STA     $4B
                JSR     sub_18796
                LDA     $62
                AND     #$F7
                STA     $62
                LDA     #0
                STA     $C0
                JSR     sub_1B2ED
                LDA     $145
                PHA
                LDA     #$30 ; '0'
                JSR     sub_1B32E
                LDA     $C7
                AND     #$80
                BNE     loc_1BAA1
                LDY     #5

loc_1BA35:                              ; CODE XREF: sub_1B8E5+176↓j
                LDA     $528,Y
                AND     #$7F
                CMP     #$53 ; 'S'
                BEQ     loc_1BA58
                CMP     #$49 ; 'I'
                BCC     loc_1BA58
                CMP     #$4F ; 'O'
                BCC     loc_1BA4E
                CMP     #$52 ; 'R'
                BCC     loc_1BA58
                CMP     #$5A ; 'Z'
                BCS     loc_1BA58

loc_1BA4E:                              ; CODE XREF: sub_1B8E5+15F↑j
                LDA     #$50 ; 'P'
                STA     $528,Y
                LDA     #0
                STA     $7E6,Y

loc_1BA58:                              ; CODE XREF: sub_1B8E5+157↑j
                                        ; sub_1B8E5+15B↑j ...
                INY
                CPY     #$1A
                BNE     loc_1BA35

loc_1BA5D:                              ; CODE XREF: sub_1B8E5+17D↓j
                LDA     $144
                ORA     $9C
                BNE     loc_1BA5D
                LDY     #$A
                JSR     j_all_enemies_destroyed
                LDA     $C0
                ORA     #$10
                STA     $C0
                JSR     sub_C00F
                LDA     $A1
                CMP     #$E
                BNE     loc_1BA7A
                LDA     #$FF

loc_1BA7A:                              ; CODE XREF: sub_1B8E5+191↑j
                STA     byte_15
                INC     byte_15
                LDA     byte_9D
                STA     byte_14
                LDA     byte_9F
                STA     byte_18
                LDA     byte_A0
                STA     byte_19
                LDA     #0
                STA     byte_10
                LDA     $175
                STA     $76
                LDA     #$F
                JSR     sub_1B0AA
                LDA     $C0
                AND     #$EF
                STA     $C0
                JSR     sub_C012

loc_1BAA1:                              ; CODE XREF: sub_1B8E5+14C↑j
                PLA
                JSR     sub_1B32E

loc_1BAA5:                              ; CODE XREF: sub_1B8E5+1C3↓j
                LDA     cram_update_mode
                BNE     loc_1BAA5
                LDY     #$1B
                LDA     byte_C7
                AND     #$DF
                CMP     byte_C7
                BNE     loc_1BAB6
                LDY     #$1C

loc_1BAB6:                              ; CODE XREF: sub_1B8E5+1CD↑j
                AND     #$1F
                CMP     #$11
                BEQ     loc_1BACF
                PHA
                JSR     sub_C063
                PLA
                CMP     #$10
                BEQ     loc_1BACF
                TYA
                JSR     sub_C030
                JSR     sub_C090
                JSR     sub_C063

loc_1BACF:                              ; CODE XREF: sub_1B8E5+1D5↑j
                                        ; sub_1B8E5+1DE↑j
                LDA     byte_C7
                AND     #$BF
                CMP     byte_C7
                BNE     loc_1BADB
                LDY     #$A
                STY     byte_66

loc_1BADB:                              ; CODE XREF: sub_1B8E5+1F0↑j
                AND     #$1F
                TAY
                CMP     #$10
                BCC     loc_1BAE8
                LDA     byte_30
                ORA     #4
                STA     byte_30

loc_1BAE8:                              ; CODE XREF: sub_1B8E5+1FB↑j
                LDA     unk_1BDF5,Y
                STA     $CB
                JSR     sub_C057
                LDY     #$64 ; 'd'
                LDA     byte_C7
                AND     #$1F
                AND     #$1E
                CMP     #$10
                BEQ     loc_1BAFF
                JSR     sub_C024

loc_1BAFF:                              ; CODE XREF: sub_1B8E5+215↑j
                JSR     sub_C090
                LDY     #$28 ; '('
                JSR     sub_C024
                LDY     #6
                LDA     byte_30
                BPL     loc_1BB0F
                LDY     #9

loc_1BB0F:                              ; CODE XREF: sub_1B8E5+226↑j
                STY     byte_66
                LDA     byte_30
                AND     #2
                BEQ     loc_1BB18
                RTS
; ---------------------------------------------------------------------------

loc_1BB18:                              ; CODE XREF: sub_1B8E5+230↑j
                LDA     byte_30
                AND     #$FB
                STA     byte_30
                LDA     byte_C7
                AND     #$1F
                CMP     #$10
                BCS     loc_1BB29
                JMP     sub_C07E
; ---------------------------------------------------------------------------

loc_1BB29:                              ; CODE XREF: sub_1B8E5+23F↑j
                JMP     sub_1BB56
; End of function sub_1B8E5

; ---------------------------------------------------------------------------
                .BYTE $22 ; "
                .BYTE $12
                .BYTE   2
                .BYTE $21 ; !
                .BYTE $11
                .BYTE   1
                .BYTE $22 ; "
                .BYTE $12
                .BYTE   1
                .BYTE $23 ; #
                .BYTE $14
                .BYTE   3
                .BYTE $22 ; "
                .BYTE $12
                .BYTE   1
                .BYTE $21 ; !
                .BYTE $1C
                .BYTE  $C
                .BYTE $22 ; "
                .BYTE $11
                .BYTE   1
                .BYTE $24 ; $
                .BYTE $16
                .BYTE   5
                .BYTE $22 ; "
                .BYTE $11
                .BYTE   1
                .BYTE $2C ; ,
                .BYTE $1C
                .BYTE  $C
                .BYTE $21 ; !
                .BYTE $11
                .BYTE   1
                .BYTE $16
                .BYTE $17
                .BYTE   6
                .BYTE $21 ; !
                .BYTE $11
                .BYTE   1
byte_1BB53:     .BYTE $36               ; DATA XREF: sub_1BB56:loc_1BBD3↓r
byte_1BB54:     .BYTE $26               ; DATA XREF: sub_1BB56+83↓r
byte_1BB55:     .BYTE $16               ; DATA XREF: sub_1BB56+89↓r

; =============== S U B R O U T I N E =======================================


sub_1BB56:                              ; CODE XREF: sub_1B8E5:loc_1BB29↑j
                BEQ     loc_1BB5B
                JMP     sub_1BCE0
; ---------------------------------------------------------------------------

loc_1BB5B:                              ; CODE XREF: sub_1BB56↑j
                LDA     #$C
                JSR     sub_C030
                LDX     #3

loc_1BB62:                              ; CODE XREF: sub_1BB56+1C↓j
                LDA     $BB2B,X
                STA     $145,X
                STA     $148,X
                STA     $14B,X
                STA     $14E,X
                DEX
                BNE     loc_1BB62
                LDA     #1
                STA     $144
                LDY     #$1E
                JSR     sub_C024
                JSR     sub_C084
                JSR     sub_1BC4E
                LDA     #$F
                STA     $6F

loc_1BB88:                              ; CODE XREF: sub_1BB56+4D↓j
                LDY     #4
                JSR     sub_C024
                LDA     $6F
                JSR     sub_C08A
                LDA     $6F
                PHA
                EOR     #$FF
                AND     #$1F
                STA     $6F
                JSR     sub_C08A
                PLA
                STA     $6F
                DEC     $6F
                BPL     loc_1BB88
                LDA     #0
                STA     $6F

loc_1BBA9:                              ; CODE XREF: sub_1BB56+7B↓j
                LDY     #8
                JSR     sub_C024
                LDA     $6F
                INC     $6F
                CMP     #3
                BEQ     loc_1BBD3
                STA     $10
                ASL
                ADC     $10
                ASL
                ASL
                TAX
                LDY     #0

loc_1BBC0:                              ; CODE XREF: sub_1BB56+74↓j
                LDA     $BB2F,X
                STA     $146,Y
                INX
                INY
                CPY     #$C
                BNE     loc_1BBC0
                LDA     #1
                STA     $144
                BNE     loc_1BBA9

loc_1BBD3:                              ; CODE XREF: sub_1BB56+5E↑j
                LDA     byte_1BB53
                STA     $15B
                LDA     byte_1BB54
                STA     $15C
                LDA     byte_1BB55
                STA     $15D
                LDA     #3
                STA     $144
                LDA     #$D1
                STA     $C7
                LDA     #2
                STA     $C0
                LDA     #0
                STA     $C1
                STA     $C2
                LDA     #4
                STA     $C3
                LDA     #$C
                STA     $6F
                LDX     #$19
                LDY     #0
; End of function sub_1BB56


; =============== S U B R O U T I N E =======================================


sub_1BC04:                              ; CODE XREF: sub_1BC04+47↓j
                                        ; sub_1BCE0+7C↓p
                LDA     byte_1BC77,Y
                STA     $528,X
                INY
                LDA     byte_1BC77,Y
                STA     $542,X
                INY
                LDA     byte_1BC77,Y
                STA     $55C,X
                INY
                LDA     byte_1BC77,Y
                STA     $5F8,X
                INY
                LDA     byte_1BC77,Y
                STA     $612,X
                INY
                LDA     byte_1BC77,Y
                STA     $62C,X
                INY
                LDA     byte_1BC77,Y
                STA     $646,X
                INY
                TYA
                PHA
                LDA     #4
                STA     $74A,X
                LDA     $528,X
                CMP     #$5A ; 'Z'
                BEQ     loc_1BC46
                JSR     sub_1B355

loc_1BC46:                              ; CODE XREF: sub_1BC04+3D↑j
                PLA
                TAY
                DEX
                DEC     $6F
                BNE     sub_1BC04
                RTS
; End of function sub_1BC04


; =============== S U B R O U T I N E =======================================


sub_1BC4E:                              ; CODE XREF: sub_1BB56+2B↑p
                LDA     #$43 ; 'C'
                JSR     sub_C04B
                JSR     sub_C03C
                TAX
                LDA     #$40 ; '@'
                STA     $460,X
                LDA     #$23 ; '#'
                STA     $461,X
                LDA     #$C0
                STA     $462,X
                LDY     #0

loc_1BC68:                              ; CODE XREF: sub_1BC4E+24↓j
                LDA     $104,Y
                STA     $463,X
                INY
                INX
                CPY     #$40 ; '@'
                BNE     loc_1BC68
                JMP     sub_C03F
; End of function sub_1BC4E

; ---------------------------------------------------------------------------
byte_1BC77:     .BYTE $4E, $28, $48, $88, $20, 2, $A
                .BYTE $4C, $28, $58, $8A, $20, 2, $A
                .BYTE $4B, $38, $48, $C8, $20, 3, $A
                .BYTE $4C, $28, $A8, $94, $20, 2, $D
                .BYTE $4E, $28, $B8, $96, $20, 2, $D
                .BYTE $4B, $38, $B8, $D6, $20, 3, $D
                .BYTE $4A, $48, $68, $C, $21, 4, $13
                .BYTE $4B, $48, $78, $E, $21, 4, $13
                .BYTE $4B, $48, $88, $10, $21, 4, $14
                .BYTE $49, $48, $98, $12, $21, 4, $14
                .BYTE $49, $68, $68, $8C, $21, 6, $1B
                .BYTE $4A, $68, $98, $92, $21, 6, $1C
                .BYTE $5A, $50, $30, 4, $21, 4, $11
                .BYTE $5A, $50, $D0, $18, $21, 4, $16
                .BYTE $5A, $30, $80, $8E, $20, 2, $B

; =============== S U B R O U T I N E =======================================


sub_1BCE0:                              ; CODE XREF: sub_1BB56+2↑j
                CMP     #$11
                BEQ     loc_1BCE7
                JMP     sub_1BDC9
; ---------------------------------------------------------------------------

loc_1BCE7:                              ; CODE XREF: sub_1BCE0+2↑j
                LDA     #$D
                STA     $6F

loc_1BCEB:                              ; CODE XREF: sub_1BCE0+3D↓j
                JSR     sub_C08D
                LDY     #4

loc_1BCF0:                              ; CODE XREF: sub_1BCE0+39↓j
                TYA
                PHA
                LDY     #3

loc_1BCF4:                              ; CODE XREF: sub_1BCE0+2F↓j
                TYA
                PHA
                LDA     #$F
                STA     $14
                LDA     #$7F
                STA     $15
                LDA     #$80
                STA     $19
                LDA     $6F
                ASL
                ASL
                ASL
                STA     $18
                JSR     sub_1B49F
                PLA
                TAY
                DEY
                BNE     loc_1BCF4
                LDY     #4
                JSR     sub_C024
                PLA
                TAY
                DEY
                BNE     loc_1BCF0
                DEC     $6F
                BNE     loc_1BCEB
                LDA     #$B2
                STA     $C7
                LDA     #2
                STA     $C0
                LDA     #0
                STA     $C1
                STA     $C2
                STA     $C3
                LDA     #$4F ; 'O'
                STA     $53F
                STA     $540
                STA     $541
                LDA     #$30 ; '0'
                STA     $55A
                STA     $55B
                LDA     #$50 ; 'P'
                STA     $559
                STA     $575
                LDA     #$B0
                STA     $574
                LDA     #$80
                STA     $573
                LDA     #3
                STA     $6F
                LDX     #$16
                LDY     #$54 ; 'T'
                JSR     sub_1BC04
                LDY     #0

loc_1BD61:                              ; CODE XREF: sub_1BCE0+8E↓j
                LDX     byte_1BDB9,Y
                INY
                LDA     byte_1BDB9,Y
                INY
                STA     $4B0,X
                CPY     #$10
                BNE     loc_1BD61
                LDA     #$D
                JSR     sub_C04B
                JSR     sub_C03C
                TAX
                LDA     #2
                STA     $460,X
                LDA     #1
                STA     $465,X
                STA     $469,X
                LDA     #$23 ; '#'
                STA     $461,X
                STA     $466,X
                STA     $46A,X
                LDA     #$CB
                STA     $462,X
                LDA     #$D1
                STA     $467,X
                LDA     #$D6
                STA     $46B,X
                LDA     #$77 ; 'w'
                STA     $463,X
                LDA     #$DD
                STA     $464,X
                LDA     #$55 ; 'U'
                STA     $468,X
                STA     $46C,X
                INC     byte_24
                INC     byte_24
                JMP     sub_C03F
; End of function sub_1BCE0

; ---------------------------------------------------------------------------
byte_1BDB9:     .BYTE $13, $F
                .BYTE $14, $F0
                .BYTE $1B, $F
                .BYTE $1C, $F0
                .BYTE $21, $FF
                .BYTE $29, $FF
                .BYTE $26, $FF
                .BYTE $2E, $FF

; =============== S U B R O U T I N E =======================================


sub_1BDC9:                              ; CODE XREF: sub_1BCE0+4↑j
                LDA     $30
                ORA     #$18
                STA     $30
                LDA     #0
                STA     $92
                LDY     #$1E
                JSR     sub_C024

loc_1BDD8:                              ; CODE XREF: sub_1BDC9+29↓j
                LDA     #$14
                JSR     sub_C030
                LDY     #$A

loc_1BDDF:                              ; CODE XREF: sub_1BDC9+25↓j
                TYA
                PHA
                LDA     #$D
                JSR     sub_C081
                LDY     #1
                JSR     sub_C024
                PLA
                TAY
                DEY
                BNE     loc_1BDDF
                DEC     $32
                BNE     loc_1BDD8
                RTS
; End of function sub_1BDC9

; ---------------------------------------------------------------------------
unk_1BDF5:      .BYTE   9
                .BYTE  $A
                .BYTE  $B
                .BYTE  $C
                .BYTE  $D
                .BYTE  $E
                .BYTE  $F
                .BYTE $10
                .BYTE $11
                .BYTE $12
                .BYTE $13
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE $13
                .BYTE $13
                .BYTE $14
                .BYTE $15

; =============== S U B R O U T I N E =======================================


sub_1BE08:                              ; CODE XREF: sub_1B8E5+EE↑j
                LDA     $62
                AND     #$F7
                STA     $62
                LDA     $47
                CLC
                ADC     #$10
                STA     $47
                JSR     sub_18788
                LDA     #6
                STA     $66
                LDA     #$C
                STA     $C0
                LDA     $C7
                AND     #$20 ; ' '
                BEQ     loc_1BE2C
                JSR     sub_C063
                JSR     sub_C07E

loc_1BE2C:                              ; CODE XREF: sub_1BE08+1C↑j
                LDA     $C7
                AND     #$1F
                CMP     #$F
                BNE     locret_1BE40
                LDA     #$B
                STA     $92
                LDA     $30
                AND     #$EF
                ORA     #$20 ; ' '
                STA     $30

locret_1BE40:                           ; CODE XREF: sub_1BE08+2A↑j
                RTS
; End of function sub_1BE08

; ---------------------------------------------------------------------------
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
; end of 'seg002'