# Enemy types

```
seg002:8674 tab_18674_hi:   .BYTE $8C               ; $8C67 - type 1 (Zanac)
seg002:8675                 .BYTE $90               ; $9038 - type 2 (Zanac Standard Shot)
seg002:8676                 .BYTE $90               ; $907E - type 3 (Zanac Special Shot)
seg002:8677                 .BYTE $96               ; $961A - type 4 (Box Bullets)
seg002:8678                 .BYTE $96               ; $961A - type 5 (Box Empty)
seg002:8679                 .BYTE $96               ; $961A - type 6 (Box Power Chip)
seg002:867A                 .BYTE $97               ; $973E - type 7
seg002:867B                 .BYTE $97               ; $97C6 - type 8
seg002:867C                 .BYTE $97               ; $97FC - type 9
seg002:867D                 .BYTE $98               ; $9828 - type 10 (Blue Duster)
seg002:867E                 .BYTE $98               ; $98C5 - type 11
seg002:867F                 .BYTE $99               ; $993B - type 12
seg002:8680                 .BYTE $99               ; $993B - type 13
seg002:8681                 .BYTE $99               ; $99F5 - type 14
seg002:8682                 .BYTE $9A               ; $9AF6 - type 15
seg002:8683                 .BYTE $9E               ; $9E98 - type 16
seg002:8684                 .BYTE $9F               ; $9F48 - type 17
seg002:8685                 .BYTE $9F               ; $9F6B - type 18
seg002:8686                 .BYTE $94               ; $9434 - type 19
seg002:8687                 .BYTE $AC               ; $ACA2 - type 20
seg002:8688                 .BYTE $AC               ; $AC47 - type 21
seg002:8689                 .BYTE $9F               ; $9FD5 - type 22
seg002:868A                 .BYTE $A0               ; $A089 - type 23
seg002:868B                 .BYTE $9D               ; $9D79 - type 24
seg002:868C                 .BYTE $9E               ; $9E37 - type 25 (T-Cell Carla)
seg002:868D                 .BYTE $A0               ; $A0BA - type 26
seg002:868E                 .BYTE $A1               ; $A16A - type 27
seg002:868F                 .BYTE $A1               ; $A19F - type 28
seg002:8690                 .BYTE $A1               ; $A1C8 - type 29 (Effine Type4)
seg002:8691                 .BYTE $A2               ; $A2DA - type 30
seg002:8692                 .BYTE $A3               ; $A3CA - type 31
seg002:8693                 .BYTE $A2               ; $A2DA - type 32
seg002:8694                 .BYTE $A3               ; $A3CA - type 33
seg002:8695                 .BYTE $A3               ; $A3EB - type 34
seg002:8696                 .BYTE $A8               ; $A8E9 - type 35 (Small Explosion)
seg002:8697                 .BYTE $A6               ; $A6F0 - type 36 (Giza)
seg002:8698                 .BYTE $AA               ; $AA61 - type 37
seg002:8699                 .BYTE $AA               ; $AA8B - type 38 (Lead/Chip)
seg002:869A                 .BYTE $AA               ; $AA9E - type 39
seg002:869B                 .BYTE $82               ; $82FD - type 40
seg002:869C                 .BYTE $AA               ; $AAA7 - type 41 (Sig)
seg002:869D                 .BYTE $AB               ; $ABA1 - type 42
seg002:869E                 .BYTE $AB               ; $ABA9 - type 43
seg002:869F                 .BYTE $A7               ; $A74B - type 44 (Yellow Duster)
seg002:86A0                 .BYTE $AB               ; $ABE7 - type 45
seg002:86A1                 .BYTE $A5               ; $A508 - type 46
seg002:86A2                 .BYTE $A5               ; $A508 - type 47
seg002:86A3                 .BYTE $9B               ; $9B89 - type 48 (Capital Maseru)
seg002:86A4                 .BYTE $9C               ; $9C36 - type 49 (Capital Hume)
seg002:86A5                 .BYTE $9C               ; $9C36 - type 50 (Capital Soreido)
seg002:86A6                 .BYTE $9C               ; $9C2A - type 51 (Capital Girevu)
seg002:86A7                 .BYTE $9C               ; $9C36 - type 52 (Capital Somasu)
seg002:86A8                 .BYTE $AC               ; $ACDD - type 53
seg002:86A9                 .BYTE $AD               ; $AD24 - type 54
seg002:86AA                 .BYTE $AD               ; $AD35 - type 55 (Rio Fairy)
seg002:86AB                 .BYTE $A6               ; $A608 - type 56
seg002:86AC                 .BYTE $A6               ; $A644 - type 57
seg002:86AD                 .BYTE $A6               ; $A6A4 - type 58
seg002:86AE                 .BYTE $A6               ; $A6C0 - type 59
seg002:86AF                 .BYTE $8F               ; $8F77 - type 60
seg002:86B0                 .BYTE $A7               ; $A778 - type 61 (Sart)
seg002:86B1                 .BYTE $AE               ; $AE19 - type 62
seg002:86B2                 .BYTE $96               ; $96E7 - type 63
seg002:86B3                 .BYTE $A6               ; $A6CE - type 64 (Random from table)
seg002:86B4                 .BYTE $A3               ; $A3EB - type 65
seg002:86B5                 .BYTE $A3               ; $A3EB - type 66
seg002:86B6                 .BYTE $A8               ; $A835 - type 67 (Valkyrie)
seg002:86B7                 .BYTE $95               ; $958D - type 68
seg002:86B8                 .BYTE $98               ; $985F - type 69
seg002:86B9                 .BYTE $AE               ; $AEC0 - type 70
seg002:86BA                 .BYTE $AE               ; $AEC0 - type 71
seg002:86BB                 .BYTE $B2               ; $B267 - type 72
seg002:86BC                 .BYTE $B3               ; $B341 - type 73
seg002:86BD                 .BYTE $B3               ; $B341 - type 74
seg002:86BE                 .BYTE $B3               ; $B341 - type 75
seg002:86BF                 .BYTE $B3               ; $B341 - type 76
seg002:86C0                 .BYTE $B3               ; $B341 - type 77
seg002:86C1                 .BYTE $B3               ; $B341 - type 78
seg002:86C2                 .BYTE $B3               ; $B336 - type 79
seg002:86C3                 .BYTE $B6               ; $B69A - type 80
seg002:86C4                 .BYTE $AE               ; $AEC0 - type 81
seg002:86C5                 .BYTE $AE               ; $AE98 - type 82 (Weapon Distro)
seg002:86C6                 .BYTE $B6               ; $B6B5 - type 83
seg002:86C7                 .BYTE $B7               ; $B72B - type 84
seg002:86C8                 .BYTE $B7               ; $B72B - type 85
seg002:86C9                 .BYTE $B7               ; $B72B - type 86
seg002:86CA                 .BYTE $B7               ; $B72B - type 87
seg002:86CB                 .BYTE $AE               ; $AEC0 - type 88
seg002:86CC                 .BYTE $AE               ; $AEC0 - type 89
seg002:86CD                 .BYTE $B7               ; $B7B7 - type 90
seg002:86CE                 .BYTE $A9               ; $A984 - type 91 (Big Explosion)
```