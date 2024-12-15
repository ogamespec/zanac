# Enemy Jump Table

Why do you think it didn't turn up right away? Well, here's why.


```asm
seg002:87EE                               loc_187EE:                              ; CODE XREF: sub_187C7+21↑j
seg002:87EE 86 40                                         STX     $40
seg002:87F0 BD 28 05                                      LDA     $528,X
seg002:87F3 F0 12                                         BEQ     loc_18807
seg002:87F5 29 7F                                         AND     #$7F
seg002:87F7 A8                                            TAY
seg002:87F8 A9 88                                         LDA     #$88            ; Push $8806 (return address)
seg002:87FA 48                                            PHA
seg002:87FB A9 06                                         LDA     #6
seg002:87FD 48                                            PHA
seg002:87FE B9 73 86                                      LDA     $8673,Y         ; Switch Case occurs at this point, but 2 Jump tables are used. One stores the low address, the other stores the high address
seg002:8801 48                                            PHA
seg002:8802 B9 18 86                                      LDA     $8618,Y
seg002:8805 48                                            PHA
seg002:8806 60                                            RTS
seg002:8807                               ; ---------------------------------------------------------------------------
seg002:8807
seg002:8807                               loc_18807:                              ; CODE XREF: sub_187C7+2C↑j
```

Table itself:

```
seg002:8619 67                            tab_18619_lo:   .BYTE $67 ; g
seg002:861A 38                                            .BYTE $38 ; 8
seg002:861B 7E                                            .BYTE $7E ; ~
seg002:861C 1A                                            .BYTE $1A
seg002:861D 1A                                            .BYTE $1A
seg002:861E 1A                                            .BYTE $1A
seg002:861F 3E                                            .BYTE $3E ; >
seg002:8620 C6                                            .BYTE $C6
seg002:8621 FC                                            .BYTE $FC
seg002:8622 28                                            .BYTE $28 ; (
seg002:8623 C5                                            .BYTE $C5
seg002:8624 3B                                            .BYTE $3B ; ;
seg002:8625 3B                                            .BYTE $3B ; ;
seg002:8626 F5                                            .BYTE $F5
seg002:8627 F6                                            .BYTE $F6
seg002:8628 98                                            .BYTE $98
seg002:8629 48                                            .BYTE $48 ; H
seg002:862A 6B                                            .BYTE $6B ; k
seg002:862B 34                                            .BYTE $34 ; 4
seg002:862C A2                                            .BYTE $A2
seg002:862D 47                                            .BYTE $47 ; G
seg002:862E D5                                            .BYTE $D5
seg002:862F 89                                            .BYTE $89
seg002:8630 79                                            .BYTE $79 ; y
seg002:8631 37                                            .BYTE $37 ; 7
seg002:8632 BA                                            .BYTE $BA
seg002:8633 6A                                            .BYTE $6A ; j
seg002:8634 9F                                            .BYTE $9F
seg002:8635 C8                                            .BYTE $C8
seg002:8636 DA                                            .BYTE $DA
seg002:8637 CA                                            .BYTE $CA
seg002:8638 DA                                            .BYTE $DA
seg002:8639 CA                                            .BYTE $CA
seg002:863A EB                                            .BYTE $EB
seg002:863B E9                                            .BYTE $E9
seg002:863C F0                                            .BYTE $F0
seg002:863D 61                                            .BYTE $61 ; a
seg002:863E 8B                                            .BYTE $8B
seg002:863F 9E                                            .BYTE $9E
seg002:8640 FD                                            .BYTE $FD
seg002:8641 A7                                            .BYTE $A7
seg002:8642 A1                                            .BYTE $A1
seg002:8643 A9                                            .BYTE $A9
seg002:8644 4B                                            .BYTE $4B ; K
seg002:8645 E7                                            .BYTE $E7
seg002:8646 08                                            .BYTE   8
seg002:8647 08                                            .BYTE   8
seg002:8648 89                                            .BYTE $89
seg002:8649 36                                            .BYTE $36 ; 6
seg002:864A 36                                            .BYTE $36 ; 6
seg002:864B 2A                                            .BYTE $2A ; *
seg002:864C 36                                            .BYTE $36 ; 6
seg002:864D DD                                            .BYTE $DD
seg002:864E 24                                            .BYTE $24 ; $
seg002:864F 35                                            .BYTE $35 ; 5
seg002:8650 08                                            .BYTE   8
seg002:8651 44                                            .BYTE $44 ; D
seg002:8652 A4                                            .BYTE $A4
seg002:8653 C0                                            .BYTE $C0
seg002:8654 77                                            .BYTE $77 ; w
seg002:8655 78                                            .BYTE $78 ; x
seg002:8656 19                                            .BYTE $19
seg002:8657 E7                                            .BYTE $E7
seg002:8658 CE                                            .BYTE $CE
seg002:8659 EB                                            .BYTE $EB
seg002:865A EB                                            .BYTE $EB
seg002:865B 35                                            .BYTE $35 ; 5
seg002:865C 8D                                            .BYTE $8D
seg002:865D 5F                                            .BYTE $5F ; _
seg002:865E C0                                            .BYTE $C0
seg002:865F C0                                            .BYTE $C0
seg002:8660 67                                            .BYTE $67 ; g
seg002:8661 41                                            .BYTE $41 ; A
seg002:8662 41                                            .BYTE $41 ; A
seg002:8663 41                                            .BYTE $41 ; A
seg002:8664 41                                            .BYTE $41 ; A
seg002:8665 41                                            .BYTE $41 ; A
seg002:8666 41                                            .BYTE $41 ; A
seg002:8667 36                                            .BYTE $36 ; 6
seg002:8668 9A                                            .BYTE $9A
seg002:8669 C0                                            .BYTE $C0
seg002:866A 98                                            .BYTE $98
seg002:866B B5                                            .BYTE $B5
seg002:866C 2B                                            .BYTE $2B ; +
seg002:866D 2B                                            .BYTE $2B ; +
seg002:866E 2B                                            .BYTE $2B ; +
seg002:866F 2B                                            .BYTE $2B ; +
seg002:8670 C0                                            .BYTE $C0
seg002:8671 C0                                            .BYTE $C0
seg002:8672 B7                                            .BYTE $B7
seg002:8673 84                                            .BYTE $84
seg002:8674 8C                            tab_18674_hi:   .BYTE $8C
seg002:8675 90                                            .BYTE $90
seg002:8676 90                                            .BYTE $90
seg002:8677 96                                            .BYTE $96
seg002:8678 96                                            .BYTE $96
seg002:8679 96                                            .BYTE $96
seg002:867A 97                                            .BYTE $97
seg002:867B 97                                            .BYTE $97
seg002:867C 97                                            .BYTE $97
seg002:867D 98                                            .BYTE $98
seg002:867E 98                                            .BYTE $98
seg002:867F 99                                            .BYTE $99
seg002:8680 99                                            .BYTE $99
seg002:8681 99                                            .BYTE $99
seg002:8682 9A                                            .BYTE $9A
seg002:8683 9E                                            .BYTE $9E
seg002:8684 9F                                            .BYTE $9F
seg002:8685 9F                                            .BYTE $9F
seg002:8686 94                                            .BYTE $94
seg002:8687 AC                                            .BYTE $AC
seg002:8688 AC                                            .BYTE $AC
seg002:8689 9F                                            .BYTE $9F
seg002:868A A0                                            .BYTE $A0
seg002:868B 9D                                            .BYTE $9D
seg002:868C 9E                                            .BYTE $9E
seg002:868D A0                                            .BYTE $A0
seg002:868E A1                                            .BYTE $A1
seg002:868F A1                                            .BYTE $A1
seg002:8690 A1                                            .BYTE $A1
seg002:8691 A2                                            .BYTE $A2
seg002:8692 A3                                            .BYTE $A3
seg002:8693 A2                                            .BYTE $A2
seg002:8694 A3                                            .BYTE $A3
seg002:8695 A3                                            .BYTE $A3
seg002:8696 A8                                            .BYTE $A8
seg002:8697 A6                                            .BYTE $A6
seg002:8698 AA                                            .BYTE $AA
seg002:8699 AA                                            .BYTE $AA
seg002:869A AA                                            .BYTE $AA
seg002:869B 82                                            .BYTE $82
seg002:869C AA                                            .BYTE $AA
seg002:869D AB                                            .BYTE $AB
seg002:869E AB                                            .BYTE $AB
seg002:869F A7                                            .BYTE $A7
seg002:86A0 AB                                            .BYTE $AB
seg002:86A1 A5                                            .BYTE $A5
seg002:86A2 A5                                            .BYTE $A5
seg002:86A3 9B                                            .BYTE $9B
seg002:86A4 9C                                            .BYTE $9C
seg002:86A5 9C                                            .BYTE $9C
seg002:86A6 9C                                            .BYTE $9C
seg002:86A7 9C                                            .BYTE $9C
seg002:86A8 AC                                            .BYTE $AC
seg002:86A9 AD                                            .BYTE $AD
seg002:86AA AD                                            .BYTE $AD
seg002:86AB A6                                            .BYTE $A6
seg002:86AC A6                                            .BYTE $A6
seg002:86AD A6                                            .BYTE $A6
seg002:86AE A6                                            .BYTE $A6
seg002:86AF 8F                                            .BYTE $8F
seg002:86B0 A7                                            .BYTE $A7
seg002:86B1 AE                                            .BYTE $AE
seg002:86B2 96                                            .BYTE $96
seg002:86B3 A6                                            .BYTE $A6
seg002:86B4 A3                                            .BYTE $A3
seg002:86B5 A3                                            .BYTE $A3
seg002:86B6 A8                                            .BYTE $A8
seg002:86B7 95                                            .BYTE $95
seg002:86B8 98                                            .BYTE $98
seg002:86B9 AE                                            .BYTE $AE
seg002:86BA AE                                            .BYTE $AE
seg002:86BB B2                                            .BYTE $B2
seg002:86BC B3                                            .BYTE $B3
seg002:86BD B3                                            .BYTE $B3
seg002:86BE B3                                            .BYTE $B3
seg002:86BF B3                                            .BYTE $B3
seg002:86C0 B3                                            .BYTE $B3
seg002:86C1 B3                                            .BYTE $B3
seg002:86C2 B3                                            .BYTE $B3
seg002:86C3 B6                                            .BYTE $B6
seg002:86C4 AE                                            .BYTE $AE
seg002:86C5 AE                                            .BYTE $AE
seg002:86C6 B6                                            .BYTE $B6
seg002:86C7 B7                                            .BYTE $B7
seg002:86C8 B7                                            .BYTE $B7
seg002:86C9 B7                                            .BYTE $B7
seg002:86CA B7                                            .BYTE $B7
seg002:86CB AE                                            .BYTE $AE
seg002:86CC AE                                            .BYTE $AE
seg002:86CD B7                                            .BYTE $B7
seg002:86CE A9                                            .BYTE $A9
```
