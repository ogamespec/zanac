# Enemy Meta Sprites

It is organized like [Enemy Jump Table](enemy_jump_table.md). It contains 83 entries.

```
ROM:C1A6 4C                            tab_C1A6:       .BYTE $4C ; L
ROM:C1A7 51                                            .BYTE $51 ; Q
ROM:C1A8 5A                                            .BYTE $5A ; Z
ROM:C1A9 63                                            .BYTE $63 ; c
ROM:C1AA 70                                            .BYTE $70 ; p
ROM:C1AB 75                                            .BYTE $75 ; u
ROM:C1AC 7E                                            .BYTE $7E ; ~
ROM:C1AD 87                                            .BYTE $87
ROM:C1AE 90                                            .BYTE $90
ROM:C1AF 99                                            .BYTE $99
ROM:C1B0 A2                                            .BYTE $A2
ROM:C1B1 AB                                            .BYTE $AB
ROM:C1B2 B4                                            .BYTE $B4
ROM:C1B3 B9                                            .BYTE $B9
ROM:C1B4 BE                                            .BYTE $BE
ROM:C1B5 C7                                            .BYTE $C7
ROM:C1B6 D4                                            .BYTE $D4
ROM:C1B7 DD                                            .BYTE $DD
ROM:C1B8 E6                                            .BYTE $E6
ROM:C1B9 EB                                            .BYTE $EB
ROM:C1BA F4                                            .BYTE $F4
ROM:C1BB FD                                            .BYTE $FD
ROM:C1BC 06                                            .BYTE   6
ROM:C1BD 0F                                            .BYTE  $F
ROM:C1BE 18                                            .BYTE $18
ROM:C1BF 21                                            .BYTE $21 ; !
ROM:C1C0 2A                                            .BYTE $2A ; *
ROM:C1C1 33                                            .BYTE $33 ; 3
ROM:C1C2 3C                                            .BYTE $3C ; <
ROM:C1C3 45                                            .BYTE $45 ; E
ROM:C1C4 4E                                            .BYTE $4E ; N
ROM:C1C5 57                                            .BYTE $57 ; W
ROM:C1C6 60                                            .BYTE $60 ; `
ROM:C1C7 69                                            .BYTE $69 ; i
ROM:C1C8 72                                            .BYTE $72 ; r
ROM:C1C9 77                                            .BYTE $77 ; w
ROM:C1CA 80                                            .BYTE $80
ROM:C1CB 89                                            .BYTE $89
ROM:C1CC 92                                            .BYTE $92
ROM:C1CD 9B                                            .BYTE $9B
ROM:C1CE A8                                            .BYTE $A8
ROM:C1CF B5                                            .BYTE $B5
ROM:C1D0 C2                                            .BYTE $C2
ROM:C1D1 CB                                            .BYTE $CB
ROM:C1D2 D4                                            .BYTE $D4
ROM:C1D3 DD                                            .BYTE $DD
ROM:C1D4 E6                                            .BYTE $E6
ROM:C1D5 EF                                            .BYTE $EF
ROM:C1D6 F8                                            .BYTE $F8
ROM:C1D7 01                                            .BYTE   1
ROM:C1D8 0A                                            .BYTE  $A
ROM:C1D9 13                                            .BYTE $13
ROM:C1DA 1C                                            .BYTE $1C
ROM:C1DB 25                                            .BYTE $25 ; %
ROM:C1DC 2E                                            .BYTE $2E ; .
ROM:C1DD 37                                            .BYTE $37 ; 7
ROM:C1DE 48                                            .BYTE $48 ; H
ROM:C1DF 59                                            .BYTE $59 ; Y
ROM:C1E0 6A                                            .BYTE $6A ; j
ROM:C1E1 7B                                            .BYTE $7B ; {
ROM:C1E2 84                                            .BYTE $84
ROM:C1E3 8D                                            .BYTE $8D
ROM:C1E4 A6                                            .BYTE $A6
ROM:C1E5 C7                                            .BYTE $C7
ROM:C1E6 E8                                            .BYTE $E8
ROM:C1E7 F9                                            .BYTE $F9
ROM:C1E8 0A                                            .BYTE  $A
ROM:C1E9 13                                            .BYTE $13
ROM:C1EA 3C                                            .BYTE $3C ; <
ROM:C1EB 5D                                            .BYTE $5D ; ]
ROM:C1EC 7E                                            .BYTE $7E ; ~
ROM:C1ED 83                                            .BYTE $83
ROM:C1EE A4                                            .BYTE $A4
ROM:C1EF C9                                            .BYTE $C9
ROM:C1F0 0A                                            .BYTE  $A
ROM:C1F1 13                                            .BYTE $13
ROM:C1F2 1C                                            .BYTE $1C
ROM:C1F3 25                                            .BYTE $25 ; %
ROM:C1F4 2E                                            .BYTE $2E ; .
ROM:C1F5 37                                            .BYTE $37 ; 7
ROM:C1F6 58                                            .BYTE $58 ; X
ROM:C1F7 79                                            .BYTE $79 ; y
ROM:C1F8 9A                                            .BYTE $9A
ROM:C1F9 C2                            tab_C1F9:       .BYTE $C2
ROM:C1FA C2                                            .BYTE $C2
ROM:C1FB C2                                            .BYTE $C2
ROM:C1FC C2                                            .BYTE $C2
ROM:C1FD C2                                            .BYTE $C2
ROM:C1FE C2                                            .BYTE $C2
ROM:C1FF C2                                            .BYTE $C2
ROM:C200 C2                                            .BYTE $C2
ROM:C201 C2                                            .BYTE $C2
ROM:C202 C2                                            .BYTE $C2
ROM:C203 C2                                            .BYTE $C2
ROM:C204 C2                                            .BYTE $C2
ROM:C205 C2                                            .BYTE $C2
ROM:C206 C2                                            .BYTE $C2
ROM:C207 C2                                            .BYTE $C2
ROM:C208 C2                                            .BYTE $C2
ROM:C209 C2                                            .BYTE $C2
ROM:C20A C2                                            .BYTE $C2
ROM:C20B C2                                            .BYTE $C2
ROM:C20C C2                                            .BYTE $C2
ROM:C20D C2                                            .BYTE $C2
ROM:C20E C2                                            .BYTE $C2
ROM:C20F C3                                            .BYTE $C3
ROM:C210 C3                                            .BYTE $C3
ROM:C211 C3                                            .BYTE $C3
ROM:C212 C3                                            .BYTE $C3
ROM:C213 C3                                            .BYTE $C3
ROM:C214 C3                                            .BYTE $C3
ROM:C215 C3                                            .BYTE $C3
ROM:C216 C3                                            .BYTE $C3
ROM:C217 C3                                            .BYTE $C3
ROM:C218 C3                                            .BYTE $C3
ROM:C219 C3                                            .BYTE $C3
ROM:C21A C3                                            .BYTE $C3
ROM:C21B C3                                            .BYTE $C3
ROM:C21C C3                                            .BYTE $C3
ROM:C21D C3                                            .BYTE $C3
ROM:C21E C3                                            .BYTE $C3
ROM:C21F C3                                            .BYTE $C3
ROM:C220 C3                                            .BYTE $C3
ROM:C221 C3                                            .BYTE $C3
ROM:C222 C3                                            .BYTE $C3
ROM:C223 C3                                            .BYTE $C3
ROM:C224 C3                                            .BYTE $C3
ROM:C225 C3                                            .BYTE $C3
ROM:C226 C3                                            .BYTE $C3
ROM:C227 C3                                            .BYTE $C3
ROM:C228 C3                                            .BYTE $C3
ROM:C229 C3                                            .BYTE $C3
ROM:C22A C4                                            .BYTE $C4
ROM:C22B C4                                            .BYTE $C4
ROM:C22C C4                                            .BYTE $C4
ROM:C22D C4                                            .BYTE $C4
ROM:C22E C4                                            .BYTE $C4
ROM:C22F C4                                            .BYTE $C4
ROM:C230 C4                                            .BYTE $C4
ROM:C231 C4                                            .BYTE $C4
ROM:C232 C4                                            .BYTE $C4
ROM:C233 C4                                            .BYTE $C4
ROM:C234 C4                                            .BYTE $C4
ROM:C235 C4                                            .BYTE $C4
ROM:C236 C4                                            .BYTE $C4
ROM:C237 C4                                            .BYTE $C4
ROM:C238 C4                                            .BYTE $C4
ROM:C239 C4                                            .BYTE $C4
ROM:C23A C4                                            .BYTE $C4
ROM:C23B C5                                            .BYTE $C5
ROM:C23C C5                                            .BYTE $C5
ROM:C23D C5                                            .BYTE $C5
ROM:C23E C5                                            .BYTE $C5
ROM:C23F C5                                            .BYTE $C5
ROM:C240 C5                                            .BYTE $C5
ROM:C241 C5                                            .BYTE $C5
ROM:C242 C5                                            .BYTE $C5
ROM:C243 C6                                            .BYTE $C6
ROM:C244 C6                                            .BYTE $C6
ROM:C245 C6                                            .BYTE $C6
ROM:C246 C6                                            .BYTE $C6
ROM:C247 C6                                            .BYTE $C6
ROM:C248 C6                                            .BYTE $C6
ROM:C249 C6                                            .BYTE $C6
ROM:C24A C6                                            .BYTE $C6
ROM:C24B C6                                            .BYTE $C6
```

Example of VLC records referenced by the table:

```
ROM:C24C tab_C24C:       .BYTE 1, $F8, $7C, 0, $FC
ROM:C251 tab_C251:       .BYTE 2, $F8, $7C, 0, $F8, $F8, $7C, $40, 0
```

The first byte defines the number of entries of 4 bytes each (sprite for OAM).
