# Enemy Meta Sprites

It is organized like [Enemy Jump Table](enemy_jump_table.md). It contains 83 entries.

```
ROM:C1A6 tab_C1A6:       .BYTE $4C ; L           ; xref sub_182D9
ROM:C1A7                 .BYTE $51 ; Q
ROM:C1A8                 .BYTE $5A ; Z
ROM:C1A9                 .BYTE $63 ; c
ROM:C1AA                 .BYTE $70 ; p
ROM:C1AB                 .BYTE $75 ; u
ROM:C1AC                 .BYTE $7E ; ~
ROM:C1AD                 .BYTE $87
ROM:C1AE                 .BYTE $90
ROM:C1AF                 .BYTE $99
ROM:C1B0                 .BYTE $A2
ROM:C1B1                 .BYTE $AB
ROM:C1B2                 .BYTE $B4
ROM:C1B3                 .BYTE $B9
ROM:C1B4                 .BYTE $BE
ROM:C1B5                 .BYTE $C7
ROM:C1B6                 .BYTE $D4
ROM:C1B7                 .BYTE $DD
ROM:C1B8                 .BYTE $E6
ROM:C1B9                 .BYTE $EB
ROM:C1BA                 .BYTE $F4
ROM:C1BB                 .BYTE $FD
ROM:C1BC                 .BYTE   6
ROM:C1BD                 .BYTE  $F
ROM:C1BE                 .BYTE $18
ROM:C1BF                 .BYTE $21 ; !
ROM:C1C0                 .BYTE $2A ; *
ROM:C1C1                 .BYTE $33 ; 3
ROM:C1C2                 .BYTE $3C ; <
ROM:C1C3                 .BYTE $45 ; E
ROM:C1C4                 .BYTE $4E ; N
ROM:C1C5                 .BYTE $57 ; W
ROM:C1C6                 .BYTE $60 ; `
ROM:C1C7                 .BYTE $69 ; i
ROM:C1C8                 .BYTE $72 ; r
ROM:C1C9                 .BYTE $77 ; w
ROM:C1CA                 .BYTE $80
ROM:C1CB                 .BYTE $89
ROM:C1CC                 .BYTE $92
ROM:C1CD                 .BYTE $9B
ROM:C1CE                 .BYTE $A8
ROM:C1CF                 .BYTE $B5
ROM:C1D0                 .BYTE $C2
ROM:C1D1                 .BYTE $CB
ROM:C1D2                 .BYTE $D4
ROM:C1D3                 .BYTE $DD
ROM:C1D4                 .BYTE $E6
ROM:C1D5                 .BYTE $EF
ROM:C1D6                 .BYTE $F8
ROM:C1D7                 .BYTE   1
ROM:C1D8                 .BYTE  $A
ROM:C1D9                 .BYTE $13
ROM:C1DA                 .BYTE $1C
ROM:C1DB                 .BYTE $25 ; %
ROM:C1DC                 .BYTE $2E ; .
ROM:C1DD                 .BYTE $37 ; 7
ROM:C1DE                 .BYTE $48 ; H
ROM:C1DF                 .BYTE $59 ; Y
ROM:C1E0                 .BYTE $6A ; j
ROM:C1E1                 .BYTE $7B ; {
ROM:C1E2                 .BYTE $84
ROM:C1E3                 .BYTE $8D
ROM:C1E4                 .BYTE $A6
ROM:C1E5                 .BYTE $C7
ROM:C1E6                 .BYTE $E8
ROM:C1E7                 .BYTE $F9
ROM:C1E8                 .BYTE  $A
ROM:C1E9                 .BYTE $13
ROM:C1EA                 .BYTE $3C ; <
ROM:C1EB                 .BYTE $5D ; ]
ROM:C1EC                 .BYTE $7E ; ~
ROM:C1ED                 .BYTE $83
ROM:C1EE                 .BYTE $A4
ROM:C1EF                 .BYTE $C9
ROM:C1F0                 .BYTE  $A
ROM:C1F1                 .BYTE $13
ROM:C1F2                 .BYTE $1C
ROM:C1F3                 .BYTE $25 ; %
ROM:C1F4                 .BYTE $2E ; .
ROM:C1F5                 .BYTE $37 ; 7
ROM:C1F6                 .BYTE $58 ; X
ROM:C1F7                 .BYTE $79 ; y
ROM:C1F8                 .BYTE $9A
ROM:C1F9 tab_C1F9:       .BYTE $C2               ; $C24C  (xref sub_182D9)
ROM:C1FA                 .BYTE $C2               ; $C251
ROM:C1FB                 .BYTE $C2               ; $C25A
ROM:C1FC                 .BYTE $C2               ; $C263
ROM:C1FD                 .BYTE $C2               ; $C270
ROM:C1FE                 .BYTE $C2               ; $C275
ROM:C1FF                 .BYTE $C2               ; $C27E
ROM:C200                 .BYTE $C2               ; $C287
ROM:C201                 .BYTE $C2               ; $C290
ROM:C202                 .BYTE $C2               ; $C299
ROM:C203                 .BYTE $C2               ; $C2A2
ROM:C204                 .BYTE $C2               ; $C2AB
ROM:C205                 .BYTE $C2               ; $C2B4
ROM:C206                 .BYTE $C2               ; $C2B9
ROM:C207                 .BYTE $C2               ; $C2BE
ROM:C208                 .BYTE $C2               ; $C2C7
ROM:C209                 .BYTE $C2               ; $C2D4
ROM:C20A                 .BYTE $C2               ; $C2DD
ROM:C20B                 .BYTE $C2               ; $C2E6
ROM:C20C                 .BYTE $C2               ; $C2EB
ROM:C20D                 .BYTE $C2               ; $C2F4
ROM:C20E                 .BYTE $C2               ; $C2FD
ROM:C20F                 .BYTE $C3               ; $C306
ROM:C210                 .BYTE $C3               ; $C30F
ROM:C211                 .BYTE $C3               ; $C318
ROM:C212                 .BYTE $C3               ; $C321
ROM:C213                 .BYTE $C3               ; $C32A
ROM:C214                 .BYTE $C3               ; $C333
ROM:C215                 .BYTE $C3               ; $C33C
ROM:C216                 .BYTE $C3               ; $C345
ROM:C217                 .BYTE $C3               ; $C34E
ROM:C218                 .BYTE $C3               ; $C357
ROM:C219                 .BYTE $C3               ; $C360
ROM:C21A                 .BYTE $C3               ; $C369
ROM:C21B                 .BYTE $C3               ; $C372
ROM:C21C                 .BYTE $C3               ; $C377
ROM:C21D                 .BYTE $C3               ; $C380
ROM:C21E                 .BYTE $C3               ; $C389
ROM:C21F                 .BYTE $C3               ; $C392
ROM:C220                 .BYTE $C3               ; $C39B
ROM:C221                 .BYTE $C3               ; $C3A8
ROM:C222                 .BYTE $C3               ; $C3B5
ROM:C223                 .BYTE $C3               ; $C3C2
ROM:C224                 .BYTE $C3               ; $C3CB
ROM:C225                 .BYTE $C3               ; $C3D4
ROM:C226                 .BYTE $C3               ; $C3DD
ROM:C227                 .BYTE $C3               ; $C3E6
ROM:C228                 .BYTE $C3               ; $C3EF
ROM:C229                 .BYTE $C3               ; $C3F8
ROM:C22A                 .BYTE $C4               ; $C401
ROM:C22B                 .BYTE $C4               ; $C40A
ROM:C22C                 .BYTE $C4               ; $C413
ROM:C22D                 .BYTE $C4               ; $C41C
ROM:C22E                 .BYTE $C4               ; $C425
ROM:C22F                 .BYTE $C4               ; $C42E
ROM:C230                 .BYTE $C4               ; $C437
ROM:C231                 .BYTE $C4               ; $C448
ROM:C232                 .BYTE $C4               ; $C459
ROM:C233                 .BYTE $C4               ; $C46A
ROM:C234                 .BYTE $C4               ; $C47B
ROM:C235                 .BYTE $C4               ; $C484
ROM:C236                 .BYTE $C4               ; $C48D
ROM:C237                 .BYTE $C4               ; $C4A6
ROM:C238                 .BYTE $C4               ; $C4C7
ROM:C239                 .BYTE $C4               ; $C4E8
ROM:C23A                 .BYTE $C4               ; $C4F9
ROM:C23B                 .BYTE $C5               ; $C50A
ROM:C23C                 .BYTE $C5               ; $C513
ROM:C23D                 .BYTE $C5               ; $C53C
ROM:C23E                 .BYTE $C5               ; $C55D
ROM:C23F                 .BYTE $C5               ; $C57E
ROM:C240                 .BYTE $C5               ; $C583
ROM:C241                 .BYTE $C5               ; $C5A4
ROM:C242                 .BYTE $C5               ; $C5C9
ROM:C243                 .BYTE $C6               ; $C60A
ROM:C244                 .BYTE $C6               ; $C613
ROM:C245                 .BYTE $C6               ; $C61C
ROM:C246                 .BYTE $C6               ; $C625
ROM:C247                 .BYTE $C6               ; $C62E
ROM:C248                 .BYTE $C6               ; $C637
ROM:C249                 .BYTE $C6               ; $C658
ROM:C24A                 .BYTE $C6               ; $C679
ROM:C24B                 .BYTE $C6               ; $C69A
ROM:C24C tab_C24C:       .BYTE 1, $F8, $7C, 0, $FC
ROM:C251 tab_C251:       .BYTE 2, $F8, $7C, 0, $F8, $F8, $7C, $40, 0
ROM:C25A tab_C25A:       .BYTE 2, $F8, $80, 0, $F8, $F8, $80, $40, 0
ROM:C263 tab_C263:       .BYTE 3, 4, $7C, 1, $E8, $F4, $7C, 1, $FC, 4, $7C, 1, $10
ROM:C270 tab_C270:       .BYTE 1, $F8, $20, 0, $FC
ROM:C275 tab_C275:       .BYTE 2, $F8, $22, 0, $F8, $F8, $22, $40, 0
ROM:C27E tab_C27E:       .BYTE 2, $F8, $24, 0, $F8, $F8, $24, $40, 0
ROM:C287 tab_C287:       .BYTE 2, $F8, $26, 0, $F8, $F8, $26, $40, 0
ROM:C290 tab_C290:       .BYTE 2, $F8, $28, 0, $F8, $F8, $28, $40, 0
ROM:C299 tab_C299:       .BYTE 2, $F8, $2A, $80, $F8, $F8, $2A, $40, 0
ROM:C2A2 tab_C2A2:       .BYTE 2, $F8, $2C, $80, $F8, $F8, $2C, $40, 0
ROM:C2AB tab_C2AB:       .BYTE 2, $F8, $2E, $80, $F8, $F8, $2E, $40, 0
ROM:C2B4 tab_C2B4:       .BYTE 1, $F8, $3A, 0, $FC
ROM:C2B9 tab_C2B9:       .BYTE 1, $F8, $A8, 0, $FC
ROM:C2BE tab_C2BE:       .BYTE 2, $F8, $A8, 0, $F8, $F8, $A8, 0, 0
ROM:C2C7 tab_C2C7:       .BYTE 3, $F8, $A8, 0, $F6, $FC, $A8, 0, $FC, $F8, $A8
ROM:C2C7                 .BYTE 0, 2
ROM:C2D4 tab_C2D4:       .BYTE 2, $F8, $30, 0, $F8, $F8, $32, 0, 0
ROM:C2DD tab_C2DD:       .BYTE 2, $F8, $34, 0, $F8, $F8, $36, 0, 0
ROM:C2E6 tab_C2E6:       .BYTE 1, $F8, $38, 0, $FC
ROM:C2EB tab_C2EB:       .BYTE 2, $F8, $36, $40, $F8, $F8, $34, $40, 0
ROM:C2F4 tab_C2F4:       .BYTE 2, $F8, $3C, 0, $F8, $F8, $3C, $40, 0
ROM:C2FD tab_C2FD:       .BYTE 2, $F8, $3E, 0, $F8, $F8, $3E, $40, 0
ROM:C306 tab_C306:       .BYTE 2, $F8, $40, 0, $F8, $F8, $40, $40, 0
ROM:C30F tab_C30F:       .BYTE 2, $F8, $42, 0, $F8, $F8, $42, $40, 0
ROM:C318 tab_C318:       .BYTE 2, $F8, $44, 0, $F8, $F8, $44, $40, 0
ROM:C321 tab_C321:       .BYTE 2, $F8, $46, 0, $F8, $F8, $46, $40, 0
ROM:C32A tab_C32A:       .BYTE 2, $F8, $48, 0, $F8, $F8, $48, $40, 0
ROM:C333 tab_C333:       .BYTE 2, $F8, $4A, 0, $F8, $F8, $4A, $40, 0
ROM:C33C tab_C33C:       .BYTE 2, $F8, $4C, 0, $F8, $F8, $4C, $40, 0
ROM:C345 tab_C345:       .BYTE 2, $F8, $4E, 0, $F8, $F8, $4E, $40, 0
ROM:C34E tab_C34E:       .BYTE 2, $F8, $50, 0, $F8, $F8, $50, $40, 0
ROM:C357 tab_C357:       .BYTE 2, $F8, $52, 0, $F8, $F8, $52, $40, 0
ROM:C360 tab_C360:       .BYTE 2, $F8, $54, 0, $F8, $F8, $54, $40, 0
ROM:C369 tab_C369:       .BYTE 2, $F8, $56, 0, $F8, $F8, $56, $40, 0
ROM:C372 tab_C372:       .BYTE 1, $F8, $56, 0, $FC
ROM:C377 tab_C377:       .BYTE 2, $F8, $58, 0, $F8, $F8, $5A, 0, 0
ROM:C380 tab_C380:       .BYTE 2, $F8, $5C, 0, $F8, $F8, $5E, 0, 0
ROM:C389 tab_C389:       .BYTE 2, $F8, $60, 0, $F8, $F8, $60, $40, 0
ROM:C392 tab_C392:       .BYTE 2, $F8, $62, 0, $F8, $F8, $64, 0, 0
ROM:C39B tab_C39B:       .BYTE 3, $F8, $6A, 0, $F4, $F8, $8A, 1, $FC, $F8, $6A
ROM:C39B                 .BYTE $40, 4
ROM:C3A8 tab_C3A8:       .BYTE 3, $F8, $66, 0, $F4, $F8, $86, 1, $FC, $F8, $66
ROM:C3A8                 .BYTE $40, 4
ROM:C3B5 tab_C3B5:       .BYTE 3, $F8, $88, 0, $F4, $F8, $86, 1, $FC, $F8, $8C
ROM:C3B5                 .BYTE 0, 4
ROM:C3C2 tab_C3C2:       .BYTE 2, $F8, $6C, 0, $F8, $F8, $6C, $40, 0
ROM:C3CB tab_C3CB:       .BYTE 2, $F8, $6E, 0, $F8, $F8, $70, 0, 0
ROM:C3D4 tab_C3D4:       .BYTE 2, $F8, $72, 0, $F8, $F8, $72, $40, 0
ROM:C3DD tab_C3DD:       .BYTE 2, $F8, $74, 0, $F8, $F8, $76, 0, 0
ROM:C3E6 tab_C3E6:       .BYTE 2, $F8, $78, 0, $F8, $F8, $7A, 0, 0
ROM:C3EF tab_C3EF:       .BYTE 2, $F8, $1C, 0, $F8, $F8, $1E, 0, 0
ROM:C3F8 tab_C3F8:       .BYTE 2, $F8, $18, 0, $F8, $F8, $1A, 0, 0
ROM:C401 tab_C401:       .BYTE 2, $F8, $14, 0, $F8, $F8, $16, 0, 0
ROM:C40A tab_C40A:       .BYTE 2, $F8, $1A, $40, $F8, $F8, $18, $40, 0
ROM:C413 tab_C413:       .BYTE 2, $F8, $1E, $40, $F8, $F8, $1C, $40, 0
ROM:C41C tab_C41C:       .BYTE 2, $F8, $AE, 0, $F8, $F8, $B0, 0, 0
ROM:C425 tab_C425:       .BYTE 2, $F8, $C4, 0, $F8, $F8, $C4, $40, 0
ROM:C42E tab_C42E:       .BYTE 2, $F8, $C2, 0, $F8, $F8, $C2, $40, 0
ROM:C437 tab_C437:       .BYTE 4, $F8, $C4, 0, $F0, $F8, $C4, $40, $F8, $F8, $C4
ROM:C437                 .BYTE 0, 0, $F8, $C4, $40, 8
ROM:C448 tab_C448:       .BYTE 4, $F8, $C2, 0, $F0, $F8, $C2, $40, $F8, $F8, $C2
ROM:C448                 .BYTE 0, 0, $F8, $C2, $40, 8
ROM:C459 tab_C459:       .BYTE 4, $F8, $C4, 0, $E8, $F8, $C4, $40, $F0, $F8, $C4
ROM:C459                 .BYTE 0, 8, $F8, $C4, $40, $10
ROM:C46A tab_C46A:       .BYTE 4, $F8, $C2, 0, $E8, $F8, $C2, $40, $F0, $F8, $C2
ROM:C46A                 .BYTE 0, 8, $F8, $C2, $40, $10
ROM:C47B tab_C47B:       .BYTE 2, $F8, $C0, 0, $F8, $F8, $C0, $40, 0
ROM:C484 tab_C484:       .BYTE 2, $F8, $BE, 0, $F8, $F8, $BE, $40, 0
ROM:C48D tab_C48D:       .BYTE 6, $F0, $BA, 0, $F4, $F0, $BC, 0, $FC, $F0, $BA
ROM:C48D                 .BYTE $40, 4, 0, $BA, $80, $F4, 0, $BC, $80, $FC, 0, $BA
ROM:C48D                 .BYTE $C0, 4
ROM:C4A6 tab_C4A6:       .BYTE 8, $F0, $B6, 0, $F0, $E9, $B8, 0, $F8, $E9, $B8
ROM:C4A6                 .BYTE $40, 0, $F0, $B6, $40, 8, 0, $B6, $80, $F0, 7, $B8
ROM:C4A6                 .BYTE $80, $F8, 7, $B8, $C0, 0, 0, $B6, $C0, 8
ROM:C4C7 tab_C4C7:       .BYTE 8, $F0, $B2, 0, $F0, $E6, $B4, 0, $F8, $E6, $B4
ROM:C4C7                 .BYTE $40, 0, $F0, $B2, $40, 8, 0, $B2, $80, $F0, $A, $B4
ROM:C4C7                 .BYTE $80, $F8, $A, $B4, $C0, 0, 0, $B2, $C0, 8
ROM:C4E8 tab_C4E8:       .BYTE 4, $F4, $B6, 0, $F0, $ED, $B8, 0, $F8, $ED, $B8
ROM:C4E8                 .BYTE $40, 0, $F4, $B6, $40, 8
ROM:C4F9 tab_C4F9:       .BYTE 4, $F4, $B2, 0, $F0, $EA, $B4, 0, $F8, $EA, $B4
ROM:C4F9                 .BYTE $40, 0, $F4, $B2, $40, 8
ROM:C50A tab_C50A:       .BYTE 2, $F8, $68, 0, $F8, $F8, $68, $40, 0
ROM:C513 tab_C513:       .BYTE $A, $E8, $84, 0, $E8, $E8, $84, $40, $F0, $F8, $84
ROM:C513                 .BYTE 0, $F8, $F8, $84, $40, 0, $E8, $84, 0, 8, $E8, $84
ROM:C513                 .BYTE $40, $10, $10, $82, $80, $E8, 8, $82, $80, $F0, 8
ROM:C513                 .BYTE $82, $C0, 8, $10, $82, $C0, $10
ROM:C53C tab_C53C:       .BYTE 8, $F0, $98, 0, $F0, $F0, $9A, 0, $F8, $F0, $9C
ROM:C53C                 .BYTE 0, 0, $F0, $9E, 0, 8, 0, $98, $80, $F0, 0, $9A, $80
ROM:C53C                 .BYTE $F8, 0, $9C, $80, 0, 0, $9E, $80, 8
ROM:C55D tab_C55D:       .BYTE 8, $F0, $A0, 0, $F0, $F0, $A2, 0, $F8, $F0, $A4
ROM:C55D                 .BYTE 0, 0, $F0, $A6, 0, 8, 0, $A0, $80, $F0, 0, $A2, $80
ROM:C55D                 .BYTE $F8, 0, $A4, $80, 0, 0, $A6, $80, 8
ROM:C57E tab_C57E:       .BYTE 1, $F8, $D8, 0, $FC
ROM:C583 tab_C583:       .BYTE 8, $F0, $92, 0, $E7, $F0, $94, 0, $EF, $F0, $8E
ROM:C583                 .BYTE 0, 9, $F0, $90, 0, $11, 0, $96, $40, $F4, $F8, $AA
ROM:C583                 .BYTE $80, $FC, 0, $96, 0, 4, 8, $AC, 0, $FC
ROM:C5A4 tab_C5A4:       .BYTE 9, $F0, $92, 0, $EC, $F0, $94, 0, $F4, $F0, $8E
ROM:C5A4                 .BYTE 0, 4, $F0, $90, 0, $C, 0, $92, $80, $EC, 0, $94
ROM:C5A4                 .BYTE $80, $F4, 0, $8E, $80, 4, 0, $90, $80, $C, $F8, $86
ROM:C5A4                 .BYTE 1, $FC
ROM:C5C9 tab_C5C9:       .BYTE $10, $D8, $92, 0, $EB, $D8, $94, 0, $F3, $D8, $8E
ROM:C5C9                 .BYTE 0, 5, $D8, $90, 0, $D, $E8, $D0, $80, $ED, $E8, $D2
ROM:C5C9                 .BYTE $80, $F5, $E8, $D4, $80, 3, $E8, $D6, $80, $B, $F8
ROM:C5C9                 .BYTE $CC, 0, $ED, $F8, $C8, 1, $F8, $F8, $CA, 1, 0, $F8
ROM:C5C9                 .BYTE $CE, 0, $B, 8, $D0, 0, $ED, 8, $D2, 0, $F5, 8, $D4
ROM:C5C9                 .BYTE 0, 3, 8, $D6, 0, $B
ROM:C60A tab_C60A:       .BYTE 2, $F8, $84, 0, $F8, $F8, $84, $40, 0
ROM:C613 tab_C613:       .BYTE 2, $F8, $F4, $40, $F8, $F8, $F4, 0, 0
ROM:C61C tab_C61C:       .BYTE 2, $F8, $F6, $40, $F8, $F8, $F6, 0, 0
ROM:C625 tab_C625:       .BYTE 2, $F8, $EC, 0, $F8, $F8, $EE, 0, 0
ROM:C62E tab_C62E:       .BYTE 2, $F8, $F0, 0, $F8, $F8, $F2, 0, 0
ROM:C637 tab_C637:       .BYTE 8, $F0, $DC, $40, $F0, $F0, $DA, $40, $F8, $F0, $DA
ROM:C637                 .BYTE 0, 0, $F0, $DC, 0, 8, 0, $E0, $40, $F0, 0, $DE, $40
ROM:C637                 .BYTE $F8, 0, $DE, 0, 0, 0, $E0, 0, 8
ROM:C658 tab_C658:       .BYTE 8, $F0, $E4, $40, $F0, $F0, $E2, $40, $F8, $F0, $E2
ROM:C658                 .BYTE 0, 0, $F0, $E4, 0, 8, 0, $E4, $C0, $F0, 0, $E2, $80
ROM:C658                 .BYTE $F8, 0, $E2, $C0, 0, 0, $E4, $80, 8
ROM:C679 tab_C679:       .BYTE 8, $F0, $E8, $40, $F0, $F0, $E6, 0, $F8, $F0, $E6
ROM:C679                 .BYTE $40, 0, $F0, $E8, 0, 8, 0, $E8, $C0, $F0, 0, $E6
ROM:C679                 .BYTE $80, $F8, 0, $E6, $C0, 0, 0, $E8, $80, 8
ROM:C69A tab_C69A:       .BYTE 4, $F0, $EA, $40, $F8, $F0, $EA, 0, 0, 0, $EA, $C0, $F8, 0, $EA, $80, 0
```

Example of VLC records referenced by the table:

```
ROM:C24C tab_C24C:       .BYTE 1, $F8, $7C, 0, $FC
ROM:C251 tab_C251:       .BYTE 2, $F8, $7C, 0, $F8, $F8, $7C, $40, 0
```

The first byte defines the number of entries of 4 bytes each (sprite for OAM).
