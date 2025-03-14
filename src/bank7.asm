    processor 6502
    org $c000,255

    include "../src/nesregs.asm"
    include "../src/vars.asm"
    include "../src/bank6_api.asm"

; Segment type: Pure code
                ;.segment ROM
                ;* =  $C000

; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_reset:
                JMP     reset
; End of function j_reset


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_switch_bank:
                JMP     switch_bank     ; A: bank number
; End of function j_switch_bank


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_far_call:
                JMP     far_call        ; Not used. Variables 10,11,12,13 are used to store the context (registers A, Y, PC are saved). Current bank saved on stack.
; End of function j_far_call


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_ppu_nmi_disable:
                JMP     ppu_nmi_disable
; End of function j_ppu_nmi_disable


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_ppu_nmi_enable:
                JMP     ppu_nmi_enable
; End of function j_ppu_nmi_enable


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_ppu_disable_all:                      ; CODE XREF: sub_1B8E5+18A↓P
                JMP     ppu_disable_all
; End of function j_ppu_disable_all


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_ppu_enable_all:                       ; CODE XREF: sub_1B8E5+1B9↓P
                JMP     ppu_enable_all
; End of function j_ppu_enable_all


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_clear_oam_buffer:                     ; CODE XREF: sub_183DD↓P
                JMP     clear_oam_buffer
; End of function j_clear_oam_buffer


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_clear_screen:
                JMP     clear_screen    ; Clear BG
; End of function j_clear_screen


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C01B:
                JMP     sub_C6AB
; End of function sub_C01B


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_pad_read:
                JMP     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
; End of function j_pad_read


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_all_enemies_destroyed:                ; CODE XREF: sub_1B2ED+E↓P
                                        ; sub_1B8E5+181↓P
                JMP     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
; End of function j_all_enemies_destroyed


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C024:                               ; CODE XREF: sub_1B8E5+217↓P
                                        ; sub_1B8E5+21F↓P ...
                JMP     sub_EA0B
; End of function sub_C024


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C027:
                JMP     sub_DD9E
; End of function sub_C027


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C02A:                               ; CODE XREF: sub_196F3↓P
                                        ; enemy_type_61_sart+9F↓P ...
                JMP     sub_D18C
; End of function sub_C02A


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_handle_pad_buttons:
                JMP     handle_pad_buttons ; Process button presses and if DEMO mode is active, simulate their pressing according to the table
; End of function j_handle_pad_buttons


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_play:                             ; CODE XREF: enemy_type_60+2A↓P
                                        ; enemy_type_2_zanac_standard_shot+15↓P ...
                JMP     apu_play        ; A = sound num (1...35)
; End of function j_apu_play


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C033:                               ; CODE XREF: sub_18835↓P
                JMP     sub_CD6B
; End of function sub_C033


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_ppu_update_background:
                JMP     ppu_update_background ; Large procedure dealing with updating the Name Table/Attr Table to display a portion of the current game level
; End of function j_ppu_update_background


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C039:                               ; CODE XREF: enemy_type_90+65↓P
                JMP     sub_C7D8
; End of function sub_C039


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C03C:                               ; CODE XREF: sub_1B20E+9↓P
                                        ; enemy_type_90+6A↓P ...
                JMP     sub_C7F8
; End of function sub_C03C


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C03F:                               ; CODE XREF: sub_1B20E+4A↓P
                                        ; enemy_type_90+92↓P ...
                JMP     sub_C807
; End of function sub_C03F


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C042:
                JMP     sub_C8DF
; End of function sub_C042


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C045:
                JMP     sub_C8BA
; End of function sub_C045


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C048:                               ; CODE XREF: sub_18835:loc_18936↓P
                                        ; sub_18835:loc_18941↓P
                JMP     sub_D2C0
; End of function sub_C048


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C04B:                               ; CODE XREF: sub_1B20E+6↓P
                                        ; sub_1BC4E+2↓P ...
                JMP     sub_C7EB
; End of function sub_C04B


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C04E:
                JMP     sub_C95C
; End of function sub_C04E


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C051:
                JMP     sub_C985
; End of function sub_C051


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C054:
                JMP     sub_CA0F
; End of function sub_C054


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C057:                               ; CODE XREF: enemy_type_14+FD↓J
                                        ; sub_1B8E5+208↓P
                JMP     sub_D193
; End of function sub_C057


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_ppu_cram_load:
                JMP     ppu_cram_load   ; Update Palette (CRAM). The Ptr variable ($18) contains the address of the palette. The palette is copied to $145. The “update palette” flag is set (checked in nmi_handler)
; End of function j_ppu_cram_load


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C05D:
                JMP     sub_DDF3
; End of function sub_C05D


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_reset:
                JMP     apu_reset
; End of function j_apu_reset


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_stop:                             ; CODE XREF: enemy_type_60+1B↓P
                                        ; sub_1B8E5+1D8↓P ...
                JMP     apu_stop
; End of function j_apu_stop


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_play_0:
                JMP     apu_play        ; A = sound num (1...35)
; End of function j_apu_play_0


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_update:
                JMP     apu_update      ; A typical approach for updating the APU state. Called from VBlank handler (every frame)
; End of function j_apu_update


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C06C:
                JMP     sub_F0F4
; End of function sub_C06C


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C06F:
                JMP     sub_F0FA
; End of function sub_C06F


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C072:                               ; CODE XREF: sub_1B8E5+86↓P
                JMP     sub_F146
; End of function sub_C072


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_switch_case:                          ; CODE XREF: enemy_type_3+4B↓P
                                        ; special_weapon_process+2↓P ...
                JMP     switch_case     ; Old-school Switch-Case. Right after the call there is a jump table of variable size.
; End of function j_switch_case


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_get_random_enemy:                     ; CODE XREF: enemy_type_64_random+15↓P
                JMP     get_random_enemy ; Y: a random index in the table. Called from type 64
; End of function j_get_random_enemy


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C07B:                               ; CODE XREF: enemy_type_60+25↓P
                JMP     sub_D5D7
; End of function sub_C07B


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_update_music_after_fortress_destroy:  ; CODE XREF: sub_1B8E5+241↓J
                                        ; sub_1BE08+21↓P
                JMP     update_music_after_fortress_destroy
; End of function j_update_music_after_fortress_destroy


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C081:                               ; CODE XREF: sub_1BDC9+1A↓P
                JMP     sub_D1A6
; End of function sub_C081


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C084:                               ; CODE XREF: sub_1BB56+28↓P
                JMP     sub_DE14
; End of function sub_C084


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C087:
                JMP     sub_DE5F
; End of function sub_C087


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C08A:                               ; CODE XREF: sub_1BB56+39↓P
                                        ; sub_1BB56+45↓P
                JMP     sub_DEF3
; End of function sub_C08A


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C08D:                               ; CODE XREF: sub_1BCE0:loc_1BCEB↓P
                JMP     sub_DE91
; End of function sub_C08D


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_C090:                               ; CODE XREF: sub_1B8E5+1E4↓P
                                        ; sub_1B8E5:loc_1BAFF↓P
                JMP     sub_FEE8        ; Activated after defeating each boss and level start
; End of function sub_C090

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
bank_remap_table:.BYTE 0, 1, 2, 3, 4, 5, 6, 7
                                        ; DATA XREF: switch_bank_0_unused+5↓r
                                        ; switch_bank_0_unused+8↓w ...
randomize_tab:  .BYTE   8
                .BYTE   8
                .BYTE  $A
                .BYTE  $C
                .BYTE   3
                .BYTE   4
                .BYTE   6
                .BYTE   8
                .BYTE   2
                .BYTE   3
                .BYTE   4
                .BYTE   5
                .BYTE   6
                .BYTE   8
                .BYTE   8
                .BYTE  $A
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   6
                .BYTE   6
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   6
                .BYTE   3
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   6
                .BYTE   8
                .BYTE   4
                .BYTE   4
                .BYTE   4
                .BYTE   4
                .BYTE   4
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   6
                .BYTE   8
                .BYTE  $A
                .BYTE $10
                .BYTE $18
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE  $C
                .BYTE  $C
                .BYTE  $C
                .BYTE   2
                .BYTE  $C
                .BYTE  $C
                .BYTE $18
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE $10
                .BYTE  $E
                .BYTE  $B
unk_C152:       .BYTE   8               ; xref sub_181A7
                .BYTE   2
                .BYTE   6
                .BYTE  $A
                .BYTE $14
                .BYTE   3
                .BYTE   4
                .BYTE   6
                .BYTE   8
                .BYTE   6
                .BYTE   5
                .BYTE   4
                .BYTE   3
                .BYTE   2
                .BYTE   3
                .BYTE   6
                .BYTE  $A
                .BYTE   8
                .BYTE   8
                .BYTE   5
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   6
                .BYTE   6
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   4
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE  $C
                .BYTE  $C
                .BYTE  $C
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   6
                .BYTE   8
                .BYTE   3
                .BYTE   3
                .BYTE   4
                .BYTE   3
                .BYTE   3
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE $10
                .BYTE $10
                .BYTE $18
                .BYTE $18
                .BYTE   6
                .BYTE   8
                .BYTE  $A
                .BYTE $10
                .BYTE $18
                .BYTE $10
                .BYTE $18
                .BYTE   8
                .BYTE $18
                .BYTE  $C
                .BYTE  $C
                .BYTE   2
                .BYTE  $C
                .BYTE $14
                .BYTE $13
                .BYTE   8
                .BYTE   6
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE $10
                .BYTE  $E
                .BYTE  $B
                .BYTE   8
tab_C1A6:       .BYTE $4C ; L           ; xref sub_182D9
                .BYTE $51 ; Q
                .BYTE $5A ; Z
                .BYTE $63 ; c
                .BYTE $70 ; p
                .BYTE $75 ; u
                .BYTE $7E ; ~
                .BYTE $87
                .BYTE $90
                .BYTE $99
                .BYTE $A2
                .BYTE $AB
                .BYTE $B4
                .BYTE $B9
                .BYTE $BE
                .BYTE $C7
                .BYTE $D4
                .BYTE $DD
                .BYTE $E6
                .BYTE $EB
                .BYTE $F4
                .BYTE $FD
                .BYTE   6
                .BYTE  $F
                .BYTE $18
                .BYTE $21 ; !
                .BYTE $2A ; *
                .BYTE $33 ; 3
                .BYTE $3C ; <
                .BYTE $45 ; E
                .BYTE $4E ; N
                .BYTE $57 ; W
                .BYTE $60 ; `
                .BYTE $69 ; i
                .BYTE $72 ; r
                .BYTE $77 ; w
                .BYTE $80
                .BYTE $89
                .BYTE $92
                .BYTE $9B
                .BYTE $A8
                .BYTE $B5
                .BYTE $C2
                .BYTE $CB
                .BYTE $D4
                .BYTE $DD
                .BYTE $E6
                .BYTE $EF
                .BYTE $F8
                .BYTE   1
                .BYTE  $A
                .BYTE $13
                .BYTE $1C
                .BYTE $25 ; %
                .BYTE $2E ; .
                .BYTE $37 ; 7
                .BYTE $48 ; H
                .BYTE $59 ; Y
                .BYTE $6A ; j
                .BYTE $7B ; {
                .BYTE $84
                .BYTE $8D
                .BYTE $A6
                .BYTE $C7
                .BYTE $E8
                .BYTE $F9
                .BYTE  $A
                .BYTE $13
                .BYTE $3C ; <
                .BYTE $5D ; ]
                .BYTE $7E ; ~
                .BYTE $83
                .BYTE $A4
                .BYTE $C9
                .BYTE  $A
                .BYTE $13
                .BYTE $1C
                .BYTE $25 ; %
                .BYTE $2E ; .
                .BYTE $37 ; 7
                .BYTE $58 ; X
                .BYTE $79 ; y
                .BYTE $9A
tab_C1F9:       .BYTE $C2               ; $C24C  (xref sub_182D9)
                .BYTE $C2               ; $C251
                .BYTE $C2               ; $C25A
                .BYTE $C2               ; $C263
                .BYTE $C2               ; $C270
                .BYTE $C2               ; $C275
                .BYTE $C2               ; $C27E
                .BYTE $C2               ; $C287
                .BYTE $C2               ; $C290
                .BYTE $C2               ; $C299
                .BYTE $C2               ; $C2A2
                .BYTE $C2               ; $C2AB
                .BYTE $C2               ; $C2B4
                .BYTE $C2               ; $C2B9
                .BYTE $C2               ; $C2BE
                .BYTE $C2               ; $C2C7
                .BYTE $C2               ; $C2D4
                .BYTE $C2               ; $C2DD
                .BYTE $C2               ; $C2E6
                .BYTE $C2               ; $C2EB
                .BYTE $C2               ; $C2F4
                .BYTE $C2               ; $C2FD
                .BYTE $C3               ; $C306
                .BYTE $C3               ; $C30F
                .BYTE $C3               ; $C318
                .BYTE $C3               ; $C321
                .BYTE $C3               ; $C32A
                .BYTE $C3               ; $C333
                .BYTE $C3               ; $C33C
                .BYTE $C3               ; $C345
                .BYTE $C3               ; $C34E
                .BYTE $C3               ; $C357
                .BYTE $C3               ; $C360
                .BYTE $C3               ; $C369
                .BYTE $C3               ; $C372
                .BYTE $C3               ; $C377
                .BYTE $C3               ; $C380
                .BYTE $C3               ; $C389
                .BYTE $C3               ; $C392
                .BYTE $C3               ; $C39B
                .BYTE $C3               ; $C3A8
                .BYTE $C3               ; $C3B5
                .BYTE $C3               ; $C3C2
                .BYTE $C3               ; $C3CB
                .BYTE $C3               ; $C3D4
                .BYTE $C3               ; $C3DD
                .BYTE $C3               ; $C3E6
                .BYTE $C3               ; $C3EF
                .BYTE $C3               ; $C3F8
                .BYTE $C4               ; $C401
                .BYTE $C4               ; $C40A
                .BYTE $C4               ; $C413
                .BYTE $C4               ; $C41C
                .BYTE $C4               ; $C425
                .BYTE $C4               ; $C42E
                .BYTE $C4               ; $C437
                .BYTE $C4               ; $C448
                .BYTE $C4               ; $C459
                .BYTE $C4               ; $C46A
                .BYTE $C4               ; $C47B
                .BYTE $C4               ; $C484
                .BYTE $C4               ; $C48D
                .BYTE $C4               ; $C4A6
                .BYTE $C4               ; $C4C7
                .BYTE $C4               ; $C4E8
                .BYTE $C4               ; $C4F9
                .BYTE $C5               ; $C50A
                .BYTE $C5               ; $C513
                .BYTE $C5               ; $C53C
                .BYTE $C5               ; $C55D
                .BYTE $C5               ; $C57E
                .BYTE $C5               ; $C583
                .BYTE $C5               ; $C5A4
                .BYTE $C5               ; $C5C9
                .BYTE $C6               ; $C60A
                .BYTE $C6               ; $C613
                .BYTE $C6               ; $C61C
                .BYTE $C6               ; $C625
                .BYTE $C6               ; $C62E
                .BYTE $C6               ; $C637
                .BYTE $C6               ; $C658
                .BYTE $C6               ; $C679
                .BYTE $C6               ; $C69A
tab_C24C:       .BYTE 1, $F8, $7C, 0, $FC
tab_C251:       .BYTE 2, $F8, $7C, 0, $F8, $F8, $7C, $40, 0
tab_C25A:       .BYTE 2, $F8, $80, 0, $F8, $F8, $80, $40, 0
tab_C263:       .BYTE 3, 4, $7C, 1, $E8, $F4, $7C, 1, $FC, 4, $7C, 1, $10
tab_C270:       .BYTE 1, $F8, $20, 0, $FC
tab_C275:       .BYTE 2, $F8, $22, 0, $F8, $F8, $22, $40, 0
tab_C27E:       .BYTE 2, $F8, $24, 0, $F8, $F8, $24, $40, 0
tab_C287:       .BYTE 2, $F8, $26, 0, $F8, $F8, $26, $40, 0
tab_C290:       .BYTE 2, $F8, $28, 0, $F8, $F8, $28, $40, 0
tab_C299:       .BYTE 2, $F8, $2A, $80, $F8, $F8, $2A, $40, 0
tab_C2A2:       .BYTE 2, $F8, $2C, $80, $F8, $F8, $2C, $40, 0
tab_C2AB:       .BYTE 2, $F8, $2E, $80, $F8, $F8, $2E, $40, 0
tab_C2B4:       .BYTE 1, $F8, $3A, 0, $FC
tab_C2B9:       .BYTE 1, $F8, $A8, 0, $FC
tab_C2BE:       .BYTE 2, $F8, $A8, 0, $F8, $F8, $A8, 0, 0
tab_C2C7:       .BYTE 3, $F8, $A8, 0, $F6, $FC, $A8, 0, $FC, $F8, $A8
                .BYTE 0, 2
tab_C2D4:       .BYTE 2, $F8, $30, 0, $F8, $F8, $32, 0, 0
tab_C2DD:       .BYTE 2, $F8, $34, 0, $F8, $F8, $36, 0, 0
tab_C2E6:       .BYTE 1, $F8, $38, 0, $FC
tab_C2EB:       .BYTE 2, $F8, $36, $40, $F8, $F8, $34, $40, 0
tab_C2F4:       .BYTE 2, $F8, $3C, 0, $F8, $F8, $3C, $40, 0
tab_C2FD:       .BYTE 2, $F8, $3E, 0, $F8, $F8, $3E, $40, 0
tab_C306:       .BYTE 2, $F8, $40, 0, $F8, $F8, $40, $40, 0
tab_C30F:       .BYTE 2, $F8, $42, 0, $F8, $F8, $42, $40, 0
tab_C318:       .BYTE 2, $F8, $44, 0, $F8, $F8, $44, $40, 0
tab_C321:       .BYTE 2, $F8, $46, 0, $F8, $F8, $46, $40, 0
tab_C32A:       .BYTE 2, $F8, $48, 0, $F8, $F8, $48, $40, 0
tab_C333:       .BYTE 2, $F8, $4A, 0, $F8, $F8, $4A, $40, 0
tab_C33C:       .BYTE 2, $F8, $4C, 0, $F8, $F8, $4C, $40, 0
tab_C345:       .BYTE 2, $F8, $4E, 0, $F8, $F8, $4E, $40, 0
tab_C34E:       .BYTE 2, $F8, $50, 0, $F8, $F8, $50, $40, 0
tab_C357:       .BYTE 2, $F8, $52, 0, $F8, $F8, $52, $40, 0
tab_C360:       .BYTE 2, $F8, $54, 0, $F8, $F8, $54, $40, 0
tab_C369:       .BYTE 2, $F8, $56, 0, $F8, $F8, $56, $40, 0
tab_C372:       .BYTE 1, $F8, $56, 0, $FC
tab_C377:       .BYTE 2, $F8, $58, 0, $F8, $F8, $5A, 0, 0
tab_C380:       .BYTE 2, $F8, $5C, 0, $F8, $F8, $5E, 0, 0
tab_C389:       .BYTE 2, $F8, $60, 0, $F8, $F8, $60, $40, 0
tab_C392:       .BYTE 2, $F8, $62, 0, $F8, $F8, $64, 0, 0
tab_C39B:       .BYTE 3, $F8, $6A, 0, $F4, $F8, $8A, 1, $FC, $F8, $6A
                .BYTE $40, 4
tab_C3A8:       .BYTE 3, $F8, $66, 0, $F4, $F8, $86, 1, $FC, $F8, $66
                .BYTE $40, 4
tab_C3B5:       .BYTE 3, $F8, $88, 0, $F4, $F8, $86, 1, $FC, $F8, $8C
                .BYTE 0, 4
tab_C3C2:       .BYTE 2, $F8, $6C, 0, $F8, $F8, $6C, $40, 0
tab_C3CB:       .BYTE 2, $F8, $6E, 0, $F8, $F8, $70, 0, 0
tab_C3D4:       .BYTE 2, $F8, $72, 0, $F8, $F8, $72, $40, 0
tab_C3DD:       .BYTE 2, $F8, $74, 0, $F8, $F8, $76, 0, 0
tab_C3E6:       .BYTE 2, $F8, $78, 0, $F8, $F8, $7A, 0, 0
tab_C3EF:       .BYTE 2, $F8, $1C, 0, $F8, $F8, $1E, 0, 0
tab_C3F8:       .BYTE 2, $F8, $18, 0, $F8, $F8, $1A, 0, 0
tab_C401:       .BYTE 2, $F8, $14, 0, $F8, $F8, $16, 0, 0
tab_C40A:       .BYTE 2, $F8, $1A, $40, $F8, $F8, $18, $40, 0
tab_C413:       .BYTE 2, $F8, $1E, $40, $F8, $F8, $1C, $40, 0
tab_C41C:       .BYTE 2, $F8, $AE, 0, $F8, $F8, $B0, 0, 0
tab_C425:       .BYTE 2, $F8, $C4, 0, $F8, $F8, $C4, $40, 0
tab_C42E:       .BYTE 2, $F8, $C2, 0, $F8, $F8, $C2, $40, 0
tab_C437:       .BYTE 4, $F8, $C4, 0, $F0, $F8, $C4, $40, $F8, $F8, $C4
                .BYTE 0, 0, $F8, $C4, $40, 8
tab_C448:       .BYTE 4, $F8, $C2, 0, $F0, $F8, $C2, $40, $F8, $F8, $C2
                .BYTE 0, 0, $F8, $C2, $40, 8
tab_C459:       .BYTE 4, $F8, $C4, 0, $E8, $F8, $C4, $40, $F0, $F8, $C4
                .BYTE 0, 8, $F8, $C4, $40, $10
tab_C46A:       .BYTE 4, $F8, $C2, 0, $E8, $F8, $C2, $40, $F0, $F8, $C2
                .BYTE 0, 8, $F8, $C2, $40, $10
tab_C47B:       .BYTE 2, $F8, $C0, 0, $F8, $F8, $C0, $40, 0
tab_C484:       .BYTE 2, $F8, $BE, 0, $F8, $F8, $BE, $40, 0
tab_C48D:       .BYTE 6, $F0, $BA, 0, $F4, $F0, $BC, 0, $FC, $F0, $BA
                .BYTE $40, 4, 0, $BA, $80, $F4, 0, $BC, $80, $FC, 0, $BA
                .BYTE $C0, 4
tab_C4A6:       .BYTE 8, $F0, $B6, 0, $F0, $E9, $B8, 0, $F8, $E9, $B8
                .BYTE $40, 0, $F0, $B6, $40, 8, 0, $B6, $80, $F0, 7, $B8
                .BYTE $80, $F8, 7, $B8, $C0, 0, 0, $B6, $C0, 8
tab_C4C7:       .BYTE 8, $F0, $B2, 0, $F0, $E6, $B4, 0, $F8, $E6, $B4
                .BYTE $40, 0, $F0, $B2, $40, 8, 0, $B2, $80, $F0, $A, $B4
                .BYTE $80, $F8, $A, $B4, $C0, 0, 0, $B2, $C0, 8
tab_C4E8:       .BYTE 4, $F4, $B6, 0, $F0, $ED, $B8, 0, $F8, $ED, $B8
                .BYTE $40, 0, $F4, $B6, $40, 8
tab_C4F9:       .BYTE 4, $F4, $B2, 0, $F0, $EA, $B4, 0, $F8, $EA, $B4
                .BYTE $40, 0, $F4, $B2, $40, 8
tab_C50A:       .BYTE 2, $F8, $68, 0, $F8, $F8, $68, $40, 0
tab_C513:       .BYTE $A, $E8, $84, 0, $E8, $E8, $84, $40, $F0, $F8, $84
                .BYTE 0, $F8, $F8, $84, $40, 0, $E8, $84, 0, 8, $E8, $84
                .BYTE $40, $10, $10, $82, $80, $E8, 8, $82, $80, $F0, 8
                .BYTE $82, $C0, 8, $10, $82, $C0, $10
tab_C53C:       .BYTE 8, $F0, $98, 0, $F0, $F0, $9A, 0, $F8, $F0, $9C
                .BYTE 0, 0, $F0, $9E, 0, 8, 0, $98, $80, $F0, 0, $9A, $80
                .BYTE $F8, 0, $9C, $80, 0, 0, $9E, $80, 8
tab_C55D:       .BYTE 8, $F0, $A0, 0, $F0, $F0, $A2, 0, $F8, $F0, $A4
                .BYTE 0, 0, $F0, $A6, 0, 8, 0, $A0, $80, $F0, 0, $A2, $80
                .BYTE $F8, 0, $A4, $80, 0, 0, $A6, $80, 8
tab_C57E:       .BYTE 1, $F8, $D8, 0, $FC
tab_C583:       .BYTE 8, $F0, $92, 0, $E7, $F0, $94, 0, $EF, $F0, $8E
                .BYTE 0, 9, $F0, $90, 0, $11, 0, $96, $40, $F4, $F8, $AA
                .BYTE $80, $FC, 0, $96, 0, 4, 8, $AC, 0, $FC
tab_C5A4:       .BYTE 9, $F0, $92, 0, $EC, $F0, $94, 0, $F4, $F0, $8E
                .BYTE 0, 4, $F0, $90, 0, $C, 0, $92, $80, $EC, 0, $94
                .BYTE $80, $F4, 0, $8E, $80, 4, 0, $90, $80, $C, $F8, $86
                .BYTE 1, $FC
tab_C5C9:       .BYTE $10, $D8, $92, 0, $EB, $D8, $94, 0, $F3, $D8, $8E
                .BYTE 0, 5, $D8, $90, 0, $D, $E8, $D0, $80, $ED, $E8, $D2
                .BYTE $80, $F5, $E8, $D4, $80, 3, $E8, $D6, $80, $B, $F8
                .BYTE $CC, 0, $ED, $F8, $C8, 1, $F8, $F8, $CA, 1, 0, $F8
                .BYTE $CE, 0, $B, 8, $D0, 0, $ED, 8, $D2, 0, $F5, 8, $D4
                .BYTE 0, 3, 8, $D6, 0, $B
tab_C60A:       .BYTE 2, $F8, $84, 0, $F8, $F8, $84, $40, 0
tab_C613:       .BYTE 2, $F8, $F4, $40, $F8, $F8, $F4, 0, 0
tab_C61C:       .BYTE 2, $F8, $F6, $40, $F8, $F8, $F6, 0, 0
tab_C625:       .BYTE 2, $F8, $EC, 0, $F8, $F8, $EE, 0, 0
tab_C62E:       .BYTE 2, $F8, $F0, 0, $F8, $F8, $F2, 0, 0
tab_C637:       .BYTE 8, $F0, $DC, $40, $F0, $F0, $DA, $40, $F8, $F0, $DA
                .BYTE 0, 0, $F0, $DC, 0, 8, 0, $E0, $40, $F0, 0, $DE, $40
                .BYTE $F8, 0, $DE, 0, 0, 0, $E0, 0, 8
tab_C658:       .BYTE 8, $F0, $E4, $40, $F0, $F0, $E2, $40, $F8, $F0, $E2
                .BYTE 0, 0, $F0, $E4, 0, 8, 0, $E4, $C0, $F0, 0, $E2, $80
                .BYTE $F8, 0, $E2, $C0, 0, 0, $E4, $80, 8
tab_C679:       .BYTE 8, $F0, $E8, $40, $F0, $F0, $E6, 0, $F8, $F0, $E6
                .BYTE $40, 0, $F0, $E8, 0, 8, 0, $E8, $C0, $F0, 0, $E6
                .BYTE $80, $F8, 0, $E6, $C0, 0, 0, $E8, $80, 8
tab_C69A:       .BYTE 4, $F0, $EA, $40, $F8, $F0, $EA, 0, 0, 0, $EA, $C0, $F8, 0, $EA, $80, 0

; =============== S U B R O U T I N E =======================================


sub_C6AB:                               ; CODE XREF: sub_C01B↑j
                                        ; sub_D6DF+C↓p ...
                LDA     #0
                STA     byte_98
                STA     byte_99
                STA     byte_9A
                STA     byte_9B
; End of function sub_C6AB


; =============== S U B R O U T I N E =======================================


sub_C6B5:                               ; CODE XREF: ppu_update_background+4↓p
                                        ; sub_D69F+6↓p
                LDA     byte_9A
                STA     ppu_scroll_1st_value
                LDA     byte_98
                STA     ppu_scroll_2nd_value
                LDA     last_ppu_ctrl1
                AND     #$FC
                LSR
                ORA     byte_99
                ASL
                ORA     byte_9B
                STA     last_ppu_ctrl1
                RTS
; End of function sub_C6B5


; =============== S U B R O U T I N E =======================================


sub_C6CA:
                PLA
                STA     byte_16
                PLA
                STA     byte_17
                JSR     sub_C6EA
; End of function sub_C6CA


; =============== S U B R O U T I N E =======================================

; Return to the parent procedure that used parameter passing through the code stream

stack_call_return:                      ; CODE XREF: print_string+E↓j
                                        ; sub_C725+40↓j
                LDA     byte_17
                PHA
                LDA     byte_16
                PHA
                RTS
; End of function stack_call_return


; =============== S U B R O U T I N E =======================================

; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.

print_string:                           ; CODE XREF: show_main_menu+198↓p
                                        ; show_main_menu+1A8↓p ...
                PLA
                STA     byte_16
                PLA
                STA     byte_17

loc_C6E0:                               ; CODE XREF: print_string+C↓j
                JSR     sub_C8BA
                JSR     sub_C6EA
                BCS     loc_C6E0
                BCC     stack_call_return ; Return to the parent procedure that used parameter passing through the code stream
; End of function print_string


; =============== S U B R O U T I N E =======================================


sub_C6EA:                               ; CODE XREF: sub_C6CA+6↑p
                                        ; print_string+9↑p
                LDY     #1
                LDA     ($16),Y
                BPL     loc_C6F8
                ASL
                BPL     loc_C6F7
                LDA     #8
                BNE     loc_C6FB

loc_C6F7:                               ; CODE XREF: sub_C6EA+7↑j
                LSR

loc_C6F8:                               ; CODE XREF: sub_C6EA+4↑j
                CLC
                ADC     #3

loc_C6FB:                               ; CODE XREF: sub_C6EA+B↑j
                JSR     sub_C7D8
                BCS     locret_C724
                STX     byte_10
                STA     byte_11
                JSR     sub_C7F8
                TAX

loc_C708:                               ; CODE XREF: sub_C6EA+27↓j
                LDA     ($16),Y
                STA     $460,X
                INY
                INX
                DEC     byte_11
                BNE     loc_C708
                JSR     sub_C807
                LDX     byte_10
                DEY
                TYA
                ADC     byte_16
                STA     byte_16
                LDA     byte_17
                ADC     #0
                STA     byte_17

locret_C724:                            ; CODE XREF: sub_C6EA+14↑j
                RTS
; End of function sub_C6EA


; =============== S U B R O U T I N E =======================================


sub_C725:
                PLA
                STA     byte_16
                PLA
                STA     byte_17
                LDY     #1
                LDA     ($16),Y
                CLC
                ADC     #3
                JSR     sub_C7EB
                JSR     sub_C7F8
                STX     byte_11
                TAX
                LDA     ($16),Y
                STA     byte_10
                STA     $460,X
                LDA     byte_19
                STA     $461,X
                LDA     byte_18
                STA     $462,X
                TYA
                SEC
                ADC     byte_16
                STA     byte_16
                BCC     loc_C756
                INC     byte_17

loc_C756:                               ; CODE XREF: sub_C725+2D↑j
                LDY     #0
                LDA     ($16),Y

loc_C75A:                               ; CODE XREF: sub_C725+3B↓j
                STA     $463,X
                INX
                DEC     byte_10
                BNE     loc_C75A
                JSR     sub_C807
                JMP     stack_call_return ; Return to the parent procedure that used parameter passing through the code stream
; End of function sub_C725


; =============== S U B R O U T I N E =======================================

; Clear BG

clear_screen:                           ; CODE XREF: j_clear_screen↑j
                                        ; show_main_menu+4E↓p ...
                STA     PPU_ADDRESS
                STA     byte_2
                LDA     #0
                STA     PPU_ADDRESS
                STA     temp_0
                LDA     byte_2
                CMP     #$20 ; ' '
                BCS     loc_C77E
                STY     temp_1
                BCC     loc_C786

loc_C77E:                               ; CODE XREF: clear_screen+10↑j
                LDA     #$C0
                STA     temp_0
                LDA     #4
                STA     temp_1

loc_C786:                               ; CODE XREF: clear_screen+14↑j
                                        ; clear_screen+23↓j ...
                STX     PPU_DATA
                DEC     temp_0
                BNE     loc_C786
                DEC     temp_1
                BNE     loc_C786
                LDA     byte_2
                CMP     #$20 ; ' '
                BCC     locret_C7A2
                LDA     #$40 ; '@'
                STA     temp_0

loc_C79B:                               ; CODE XREF: clear_screen+38↓j
                STY     PPU_DATA
                DEC     temp_0
                BNE     loc_C79B

locret_C7A2:                            ; CODE XREF: clear_screen+2D↑j
                RTS
; End of function clear_screen


; =============== S U B R O U T I N E =======================================


ppu_update_scroll_regs:                 ; CODE XREF: ppu_enable_all↓p
                                        ; nmi_handler+26↓p
                LDA     last_ppu_ctrl1
                STA     PPU_CTRL_REG1
                LDA     ppu_scroll_1st_value
                STA     PPU_SCROLL_REG
                LDA     ppu_scroll_2nd_value
                STA     PPU_SCROLL_REG
                RTS
; End of function ppu_update_scroll_regs


; =============== S U B R O U T I N E =======================================

; Fill memory $X.00 ... $Y.FF by value A

mem_page_fill:                          ; CODE XREF: clear_oam_buffer+6↓j
                                        ; sub_CC57+8↓p
                STX     temp_1
                LDX     #0
                STX     temp_0

loc_C7B9:                               ; CODE XREF: mem_page_fill+A↓j
                                        ; mem_page_fill+10↓j
                STA     (temp_0,X)
                INC     temp_0
                BNE     loc_C7B9
                INC     temp_1
                CPY     temp_1
                BCS     loc_C7B9
                RTS
; End of function mem_page_fill


; =============== S U B R O U T I N E =======================================

; Old-school Switch-Case. Right after the call there is a jump table of variable size.

switch_case:                            ; CODE XREF: j_switch_case↑j
                                        ; sub_D4FA+9↓p ...
                ASL
                TAY
                PLA
                STA     temp_0
                PLA
                STA     temp_1
                INY
                INY
                LDA     (temp_0),Y
                PHA
                DEY
                LDA     (temp_0),Y
                PHA
                RTS
; End of function switch_case


; =============== S U B R O U T I N E =======================================


sub_C7D8:                               ; CODE XREF: sub_C039↑j
                                        ; sub_C6EA:loc_C6FB↑p ...
                PHA
                CLC
                ADC     byte_21
                CMP     #$51 ; 'Q'
                BCS     loc_C7E9
                CMP     #$11
                BCC     loc_C7E9
                LDA     byte_9C
                BNE     loc_C7E9
                CLC

loc_C7E9:                               ; CODE XREF: sub_C7D8+6↑j
                                        ; sub_C7D8+A↑j ...
                PLA

locret_C7EA:                            ; CODE XREF: sub_C7EB+3↓j
                RTS
; End of function sub_C7D8


; =============== S U B R O U T I N E =======================================


sub_C7EB:                               ; CODE XREF: sub_C04B↑j
                                        ; sub_C725+D↑p ...
                JSR     sub_C7D8
                BCC     locret_C7EA
                PHA
                JSR     sub_C8BA
                PLA
                JMP     sub_C7EB
; End of function sub_C7EB


; =============== S U B R O U T I N E =======================================


sub_C7F8:                               ; CODE XREF: sub_C03C↑j
                                        ; sub_C6EA+1A↑p ...
                STA     byte_25
                LDA     byte_21
                PHA
                LDA     byte_20
                STA     byte_24
                LDA     #0
                STA     byte_20
                PLA
                RTS
; End of function sub_C7F8


; =============== S U B R O U T I N E =======================================


sub_C807:                               ; CODE XREF: sub_C03F↑j
                                        ; sub_C6EA+29↑p ...
                LDA     byte_25
                CLC
                ADC     byte_21
                STA     byte_21
                LDA     byte_24
                ADC     #1
                STA     byte_20
                RTS
; End of function sub_C807


; =============== S U B R O U T I N E =======================================

; A: bank number

switch_bank:                            ; CODE XREF: j_switch_bank↑j
                                        ; sub_C8DF+25↓p ...
                STA     last_bank_num
                TYA
                PHA
                LDY     last_bank_num
                LDA     bank_remap_table,Y
                STA     bank_remap_table,Y
                PLA
                TAY
                RTS
; End of function switch_bank


; =============== S U B R O U T I N E =======================================


switch_bank_0_unused:
                PHA
                LDA     #0
                STA     last_bank_num
                LDA     bank_remap_table
                STA     bank_remap_table
                PLA
                RTS
; End of function switch_bank_0_unused


; =============== S U B R O U T I N E =======================================


switch_bank_1_unused:
                PHA
                LDA     #1
                STA     last_bank_num
                LDA     bank_remap_table+1
                STA     bank_remap_table+1
                PLA
                RTS
; End of function switch_bank_1_unused


; =============== S U B R O U T I N E =======================================


switch_bank_2_unused:
                PHA
                LDA     #2
                STA     last_bank_num
                LDA     bank_remap_table+2
                STA     bank_remap_table+2
                PLA
                RTS
; End of function switch_bank_2_unused


; =============== S U B R O U T I N E =======================================


switch_bank_3_unused:
                PHA
                LDA     #3
                STA     last_bank_num
                LDA     bank_remap_table+3
                STA     bank_remap_table+3
                PLA
                RTS
; End of function switch_bank_3_unused


; =============== S U B R O U T I N E =======================================


switch_bank_4:
                PHA
                LDA     #4
                STA     last_bank_num
                LDA     bank_remap_table+4
                STA     bank_remap_table+4
                PLA
                RTS
; End of function switch_bank_4


; =============== S U B R O U T I N E =======================================


switch_bank_5:                          ; CODE XREF: sub_C985+41↓p
                                        ; sub_CC57+20↓p ...
                PHA
                LDA     #5
                STA     last_bank_num
                LDA     bank_remap_table+5
                STA     bank_remap_table+5
                PLA
                RTS
; End of function switch_bank_5


; =============== S U B R O U T I N E =======================================


switch_bank_6:                          ; CODE XREF: sub_CC57+4D↓p
                                        ; sub_D5AB+13↓j ...
                PHA
                LDA     #6
                STA     last_bank_num
                LDA     bank_remap_table+6
                STA     bank_remap_table+6
                PLA
                RTS
; End of function switch_bank_6


; =============== S U B R O U T I N E =======================================

; Not used. Variables 10,11,12,13 are used to store the context (registers A, Y, PC are saved). Current bank saved on stack.

far_call:                               ; CODE XREF: j_far_call↑j
                STA     byte_10
                STY     byte_11
                PLA
                STA     byte_12
                CLC
                ADC     #3
                TAY
                PLA
                STA     byte_13
                ADC     #0
                PHA
                TYA
                PHA
                LDA     last_bank_num
                PHA
                JSR     far_call_prepare
                STA     byte_10
                STY     byte_11
                PLA
                JMP     far_call_restore
; End of function far_call


; =============== S U B R O U T I N E =======================================


far_call_prepare:                       ; CODE XREF: far_call+16↑p
                LDY     #3
                LDA     ($12),Y
                PHA
                DEY
                LDA     ($12),Y
                PHA
                DEY
                LDA     ($12),Y
; End of function far_call_prepare


; =============== S U B R O U T I N E =======================================


far_call_restore:                       ; CODE XREF: far_call+1E↑j
                STA     last_bank_num
                TAY
                LDA     bank_remap_table,Y
                STA     bank_remap_table,Y
                LDA     byte_10
                LDY     byte_11
                RTS
; End of function far_call_restore


; =============== S U B R O U T I N E =======================================


sub_C8BA:                               ; CODE XREF: sub_C045↑j
                                        ; print_string:loc_C6E0↑p ...
                TYA
                PHA

loc_C8BC:                               ; CODE XREF: sub_C8BA+20↓j
                JSR     sub_CD6B
                LDA     last_ppu_ctrl2
                AND     #$18            ; Check BG or OBJ enabled
                BNE     loc_C8D4
                LDA     last_ppu_ctrl1  ; Disable VBlank
                AND     #$7F
                STA     PPU_CTRL_REG1
                JSR     ppu_update_background ; Large procedure dealing with updating the Name Table/Attr Table to display a portion of the current game level
                LDA     last_ppu_ctrl1  ; Restore VBlank
                STA     PPU_CTRL_REG1

loc_C8D4:                               ; CODE XREF: sub_C8BA+9↑j
                LDA     byte_2B
                ORA     byte_9C
                ORA     byte_20
                BNE     loc_C8BC
                PLA
                TAY
                RTS
; End of function sub_C8BA


; =============== S U B R O U T I N E =======================================


sub_C8DF:                               ; CODE XREF: sub_C042↑j
                                        ; start_game_check+A↓p ...
                STA     byte_15
                STY     byte_14
                TYA
                AND     #$F
                TAY
                LDA     last_bank_num
                PHA
                LDA     byte_15
                ASL
                ADC     unk_C928,Y
                PHA
                LDA     unk_C930,Y      ; 0x80 = demo mode
                ADC     #0
                PHA
                LDA     byte_14
                PHA
                LDA     byte_2B
                BEQ     loc_C901
                JSR     sub_C8BA

loc_C901:                               ; CODE XREF: sub_C8DF+1D↑j
                PLA
                STA     byte_26
                JSR     switch_bank     ; A: bank number
                PLA
                STA     byte_15
                PLA
                STA     byte_14
                LDY     #0
                LDA     ($14),Y
                STA     byte_2C
                INY
                LDA     ($14),Y
                STA     byte_2D
                LDA     byte_18
                STA     byte_2E
                LDA     byte_19
                STA     byte_2F
                LDA     #2
                STA     byte_2B
                PLA
                JMP     switch_bank     ; A: bank number
; End of function sub_C8DF

; ---------------------------------------------------------------------------
unk_C928:       .BYTE $30 ; 0
                .BYTE $30 ; 0
                .BYTE $30 ; 0
                .BYTE $30 ; 0
                .BYTE $30 ; 0
                .BYTE $30 ; 0
                .BYTE $30 ; 0
                .BYTE   0
unk_C930:       .BYTE $80
                .BYTE $80
                .BYTE $80
                .BYTE $80
                .BYTE $80
                .BYTE $80
                .BYTE $80
                .BYTE $C1               ; $C100

; =============== S U B R O U T I N E =======================================

; Clearing a game object (X). So far, the array assignments are unknown. The same procedure is found in bank 6.

enemy_erase_2:                          ; CODE XREF: sub_DD9E+4E↓p
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
; End of function enemy_erase_2


; =============== S U B R O U T I N E =======================================


sub_C95C:                               ; CODE XREF: sub_C04E↑j
                LDA     byte_19
                LSR
                LSR
                LSR
                STA     byte_10
                LSR
                LSR
                STA     byte_11
                LDA     #8
                STA     byte_19
                LDA     byte_18
                STA     byte_12
                AND     #$F8
                ASL
                ROL     byte_19
                ASL
                ROL     byte_19
                ORA     byte_10
                STA     byte_18
                LDA     byte_12
                AND     #$E0
                LSR
                LSR
                ORA     byte_11
                TAY
                RTS
; End of function sub_C95C


; =============== S U B R O U T I N E =======================================


sub_C985:                               ; CODE XREF: sub_C051↑j
                PHA
                LDA     #8
                JSR     sub_C7EB
                JSR     sub_C7F8
                PLA
                STA     byte_10
                TXA
                PHA
                LDX     byte_21
                LDA     byte_19
                AND     #$F
                ORA     #$C0
                STA     $460,X
                LDA     byte_18
                STA     $461,X
                TYA
                ORA     #$C0
                STA     $466,X
                LDA     #0
                STA     byte_11
                LDA     byte_10
                ASL
                ROL     byte_11
                ASL
                ROL     byte_11
                ADC     byte_10
                BCC     loc_C9BB
                INC     byte_11

loc_C9BB:                               ; CODE XREF: sub_C985+32↑j
                ADC     byte_BA
                STA     byte_10
                BCC     loc_C9C3
                INC     byte_11

loc_C9C3:                               ; CODE XREF: sub_C985+3A↑j
                LDA     last_bank_num
                PHA
                JSR     switch_bank_5
                LDY     #0

loc_C9CB:                               ; CODE XREF: sub_C985+4F↓j
                LDA     ($10),Y
                STA     $462,X
                INX
                INY
                CPY     #4
                BNE     loc_C9CB
                LDA     ($10),Y
                STA     byte_10
                LDA     byte_18
                AND     #2
                LSR
                TAY
                LDA     byte_18
                AND     #$40 ; '@'
                BEQ     loc_C9E8
                INY
                INY

loc_C9E8:                               ; CODE XREF: sub_C985+5F↑j
                LDA     $462,X
                TAX
                .BYTE $BD, $44, $00   ; LDA     !$44,X  -- Force non Zero Page [!]
                AND     byte_D7DB,Y
                STA     byte_11
                LDA     byte_D7DF,Y
                LDY     byte_10
                AND     $16E,Y
                ORA     byte_11
                .BYTE $9D, $44, $00  ; STA !$44,X  -- Force non Zero Page [!]
                LDX     byte_21
                STA     $467,X
                PLA
                JSR     switch_bank     ; A: bank number
                PLA
                TAX
                JMP     sub_C807
; End of function sub_C985


; =============== S U B R O U T I N E =======================================


sub_CA0F:                               ; CODE XREF: sub_C054↑j
                PHA
                LDA     #8
; End of function sub_CA0F


; =============== S U B R O U T I N E =======================================


sub_CA12:                               ; CODE XREF: sub_CA97+3↓j
                JSR     sub_C7EB
                JSR     sub_C7F8
                PLA
                STA     byte_10
                TXA
                PHA
                LDX     byte_21
                LDA     byte_19
                AND     #$F
                ORA     #$C0
                STA     $460,X
                LDA     byte_18
                STA     $461,X
                TYA
                ORA     #$C0
                STA     $466,X
                LDY     byte_10
                TYA
                STA     $462,X
                INY
                TYA
                STA     $464,X
                INY
                TYA
                STA     $463,X
                INY
                TYA
                STA     $465,X
                LDA     byte_18
                AND     #2
                LSR
                TAY
                LDA     byte_18
                AND     #$40 ; '@'
                BEQ     loc_CA56
                INY
                INY

loc_CA56:                               ; CODE XREF: sub_CA12+40↑j
                LDA     byte_25
                CMP     #8
                BNE     loc_CA7C
                LDA     $466,X
                TAX
                .BYTE $BD, $44, $00    ; LDA     $44,X    -- Force non Zero Page [!]
                AND     byte_D7DB,Y
                STA     byte_11
                LDA     byte_D7DF,Y
                AND     byte_63
                ORA     byte_11
                .BYTE $9D, $44, $00     ; STA     $44,X   -- Force non Zero Page [!]
                LDX     byte_21
                STA     $467,X
                PLA
                TAX
                JMP     sub_C807
; ---------------------------------------------------------------------------

loc_CA7C:                               ; CODE XREF: sub_CA12+48↑j
                LDA     $460,X
                ORA     #$E0
                STA     $460,X
                LDA     byte_D7DB,Y
                STA     $468,X
                LDA     byte_D7DF,Y
                AND     byte_63
                STA     $467,X
                PLA
                TAX
                JMP     sub_C807
; End of function sub_CA12


; =============== S U B R O U T I N E =======================================


sub_CA97:
                PHA
                LDA     #9
                JMP     sub_CA12
; End of function sub_CA97


; =============== S U B R O U T I N E =======================================

; Update Palette (CRAM). The Ptr variable ($18) contains the address of the palette. The palette is copied to $145. The “update palette” flag is set (checked in nmi_handler)

ppu_cram_load:                          ; CODE XREF: j_ppu_cram_load↑j
                                        ; show_main_menu+14↓p ...
                LDY     #0

loc_CA9F:                               ; CODE XREF: ppu_cram_load+A↓j
                LDA     ($18),Y
                STA     cram_new_palette,Y
                INY
                CPY     #$19
                BNE     loc_CA9F
                LDA     #3
                ORA     cram_update_mode ; cram_update_mode |= 3
                STA     cram_update_mode
                RTS
; End of function ppu_cram_load


; =============== S U B R O U T I N E =======================================

; Another PRNG, based on the $C100 table

randomize:                              ; CODE XREF: start_game_check+8F↓p
                                        ; start_game_check+A5↓p ...
                STX     byte_10
                LDX     last_randomize_result ; Used only in the `randomize` procedure
                SEC
                ROL
                EOR     randomize_tab,X
                ADC     $10,X
                STA     last_randomize_result ; Used only in the `randomize` procedure
                TSX
                TXA
                LDX     byte_10
                ADC     last_randomize_result ; Used only in the `randomize` procedure
                ADC     $470,X
                ADC     byte_A2
                STA     last_randomize_result ; Used only in the `randomize` procedure
                RTS
; End of function randomize


; =============== S U B R O U T I N E =======================================


ppu_disable_all:                        ; CODE XREF: j_ppu_disable_all↑j
                                        ; show_main_menu+3↓p ...
                JSR     ppu_nmi_disable
                LDA     PPU_STATUS
                LDA     last_ppu_ctrl2
                AND     #$E7
; End of function ppu_disable_all


; =============== S U B R O U T I N E =======================================

; A = new PPU CTRL2 value

ppu_set_control2:                       ; CODE XREF: reset+13↓p
                                        ; sub_E81E+27↓p
                STA     PPU_CTRL_REG2
                STA     last_ppu_ctrl2
                RTS
; End of function ppu_set_control2


; =============== S U B R O U T I N E =======================================

; Enable BG & Sprites

ppu_enable_picture:                     ; CODE XREF: ppu_enable_all+3↓p
                LDA     last_ppu_ctrl2
                ORA     #$18
                STA     last_ppu_ctrl2
                STA     PPU_CTRL_REG2
                RTS
; End of function ppu_enable_picture


; =============== S U B R O U T I N E =======================================


ppu_nmi_disable:                        ; CODE XREF: j_ppu_nmi_disable↑j
                                        ; ppu_disable_all↑p
                LDA     last_ppu_ctrl1
                AND     #$7F
; End of function ppu_nmi_disable


; =============== S U B R O U T I N E =======================================


ppu_set_control1:                       ; CODE XREF: reset+E↓p
                STA     PPU_CTRL_REG1
                STA     last_ppu_ctrl1
                RTS
; End of function ppu_set_control1


; =============== S U B R O U T I N E =======================================


ppu_enable_all:                         ; CODE XREF: j_ppu_enable_all↑j
                                        ; sub_DDF3+3↓j
                JSR     ppu_update_scroll_regs
                JSR     ppu_enable_picture ; Enable BG & Sprites
; End of function ppu_enable_all


; =============== S U B R O U T I N E =======================================


ppu_nmi_enable:                         ; CODE XREF: j_ppu_nmi_enable↑j
                                        ; sub_DDF9+9↓p ...
                LDA     last_ppu_ctrl1
                ORA     #$80
                STA     last_ppu_ctrl1
                STA     PPU_CTRL_REG1
                RTS
; End of function ppu_nmi_enable


; =============== S U B R O U T I N E =======================================


clear_oam_buffer:                       ; CODE XREF: j_clear_oam_buffer↑j
                                        ; clear_oam_buffer_unused↓p ...
                LDA     #$F8
                LDX     #2
                LDY     #2
                JMP     mem_page_fill   ; Fill memory $X.00 ... $Y.FF by value A
; End of function clear_oam_buffer


; =============== S U B R O U T I N E =======================================

; Poll the status of the controller (pad) and save the state of the buttons

pad_read:                               ; CODE XREF: j_pad_read↑j
                                        ; sub_D0BF+2F↓p ...
                LDA     #1
                STA     JOYPAD_PORT1
                LDA     #0
                STA     JOYPAD_PORT1
                LDX     #8

loc_CB16:                               ; CODE XREF: pad_read+1F↓j
                LDA     JOYPAD_PORT1
                LSR
                ROL     pad_test1
                LSR
                ROL     temp_0
                LDA     JOYPAD_PORT2
                LSR
                ROL     pad_test2
                LSR
                ROL     temp_1
                DEX
                BNE     loc_CB16
                LDX     #1

loc_CB2D:                               ; CODE XREF: pad_read+33↓j
                LDY     pad_buttons_1,X
                LDA     pad_test1,X
                ORA     0,X
                STA     pad_buttons_1,X
                TYA
                EOR     pad_buttons_1,X
                AND     pad_buttons_1,X
                STA     pad_test1,X
                DEX
                BPL     loc_CB2D
                RTS
; End of function pad_read


; =============== S U B R O U T I N E =======================================

; Clear array $172...$179

sub_CB40:                               ; CODE XREF: start_game_check:continue_level_check↓p
                                        ; reset+3E↓p
                LDA     #0
                LDY     #8

loc_CB44:                               ; CODE XREF: sub_CB40+8↓j
                STA     $179,Y
                DEY
                BNE     loc_CB44
                RTS
; End of function sub_CB40


; =============== S U B R O U T I N E =======================================


start_game_check:                       ; CODE XREF: main_menu_loop+15↓p
                LDA     #0
                STA     byte_19
                STA     byte_18
                LDA     #0
                LDY     #4
                JSR     sub_C8DF
                LDA     pad_buttons_1
                ORA     pad_buttons_2
                AND     #$C0
                CMP     #$C0
                BEQ     loc_CB67
                LDA     byte_19B
                BEQ     loc_CB76

loc_CB67:                               ; CODE XREF: start_game_check+15↑j
                LDA     #1
                STA     byte_19
                LDA     #$40 ; '@'
                STA     byte_18
                LDA     #1
                LDY     #4
                JSR     sub_C8DF

loc_CB76:                               ; CODE XREF: start_game_check+1A↑j
                LDA     #$17
                STA     byte_19
                LDA     #$F0
                STA     byte_18
                LDA     #3
                LDY     #4
                JSR     sub_C8DF
                JSR     sub_C8BA
                LDA     #3
                STA     byte_32
                LDA     #0
                STA     game_mode       ; 0x80 = demo mode
                STA     zanac_main_weapon_level ; 0...5, Super
                STA     byte_49
                STA     byte_46
                STA     byte_47
                STA     byte_48
                STA     byte_4B
                STA     byte_5A
                STA     byte_4D
                STA     byte_4A
                STA     byte_56
                STA     byte_54
                STA     byte_176
                STA     byte_177
                STA     byte_178
                STA     byte_39
                STA     byte_55
                STA     byte_C0
                STA     byte_5D
                LDA     #$10
                STA     byte_77
                LDA     #2
                STA     byte_45
                STA     byte_179
                LDA     #7
                STA     byte_62
                LDA     #$F
                STA     byte_51
                JSR     sub_18012
                JSR     j_set_default_weapon
                LDA     pad_buttons_1
                AND     #$10            ; Check START pressed (by player)
                BNE     continue_level_check
                LDA     #$80
                STA     game_mode       ; 0x80 = demo mode
                JSR     randomize       ; Another PRNG, based on the $C100 table
                AND     #7
                CLC
                ADC     #1
                STA     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                LDA     #$14
                STA     demo_counter_2
                LDA     #1
                STA     demo_button_update_counter
                LDA     #8
                STA     byte_51
                JSR     randomize       ; Another PRNG, based on the $C100 table
                AND     #$1F
                STA     demo_button_tab_pointer
                INC     byte_CD
                LDA     byte_CD
                AND     #$F
                STA     byte_CD
                TAY
                LDA     unk_CC16,Y
                STA     byte_18
                LDA     unk_CC26,Y
                STA     byte_19

loc_CC0A:                               ; CODE XREF: start_game_check+C8↓j
                BEQ     locret_CC15
                LDA     byte_18
                JSR     sub_1800F
                DEC     byte_19
                BNE     loc_CC0A

locret_CC15:                            ; CODE XREF: start_game_check:loc_CC0A↑j
                RTS
; ---------------------------------------------------------------------------
unk_CC16:       .BYTE   7
                .BYTE   0
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   4
                .BYTE   5
                .BYTE   6
                .BYTE   7
                .BYTE   0
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   4
                .BYTE   6
                .BYTE   5
unk_CC26:       .BYTE   6
                .BYTE   2
                .BYTE   1
                .BYTE   4
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   1
                .BYTE   3
                .BYTE   1
                .BYTE   3
                .BYTE   2
                .BYTE   6
                .BYTE   3
                .BYTE   0
                .BYTE  $C
; ---------------------------------------------------------------------------

continue_level_check:                   ; CODE XREF: start_game_check+89↑j
                JSR     sub_CB40        ; Clear array $172...$179
                LDY     byte_7D
                LDA     byte_199
                BEQ     loc_CC50
                LDA     pad_buttons_1
                ORA     pad_buttons_2
                AND     #8
                BNE     loc_CC52
                CPY     #11
                BCC     loc_CC4E
                LDY     #10

loc_CC4E:                               ; CODE XREF: start_game_check+FF↑j
                BNE     loc_CC52

loc_CC50:                               ; CODE XREF: start_game_check+F3↑j
                LDY     #1

loc_CC52:                               ; CODE XREF: start_game_check+FB↑j
                                        ; start_game_check:loc_CC4E↑j
                STY     byte_7D
                STY     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                RTS
; End of function start_game_check


; =============== S U B R O U T I N E =======================================


sub_CC57:                               ; CODE XREF: sub_E81E+12↓p
                LDA     #0
                STA     warp_sequence_counter
                LDX     #5
                LDY     #7
                JSR     mem_page_fill   ; Fill memory $X.00 ... $Y.FF by value A
                LDA     #$10
                STA     byte_19
                LDA     #0
                STA     byte_18
                LDY     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                LDA     unk_CCAD,Y
                LDY     #4
                JSR     sub_C8DF
                JSR     sub_C8BA
                JSR     switch_bank_5
                LDA     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                ASL
                TAY
                LDA     $8030,Y
                STA     byte_18
                LDA     $8031,Y
                STA     byte_19
                LDY     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                LDX     unk_CCBA,Y
                LDA     $8030,X
                STA     byte_B8
                LDA     $8031,X
                STA     byte_B9
                LDX     unk_CCC7,Y
                LDA     $8030,X
                STA     byte_BA
                LDA     $8031,X
                STA     byte_BB
                JSR     switch_bank_6
                JSR     sub_D6DF
                JMP     sub_D753
; End of function sub_CC57

; ---------------------------------------------------------------------------
unk_CCAD:       .BYTE   6
                .BYTE   4
                .BYTE   4
                .BYTE   4
                .BYTE   4
                .BYTE   5
                .BYTE   5
                .BYTE   5
                .BYTE   5
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6
unk_CCBA:       .BYTE $1E
                .BYTE $1A
                .BYTE $1A
                .BYTE $1A
                .BYTE $1A
                .BYTE $1C
                .BYTE $1C
                .BYTE $1C
                .BYTE $1C
                .BYTE $1E
                .BYTE $1E
                .BYTE $1E
                .BYTE $1E
unk_CCC7:       .BYTE $24 ; $
                .BYTE $20
                .BYTE $20
                .BYTE $20
                .BYTE $20
                .BYTE $22 ; "
                .BYTE $22 ; "
                .BYTE $22 ; "
                .BYTE $22 ; "
                .BYTE $24 ; $
                .BYTE $24 ; $
                .BYTE $24 ; $
                .BYTE $24 ; $

; =============== S U B R O U T I N E =======================================


clear_oam_buffer_unused:
                JSR     clear_oam_buffer
                RTS
; End of function clear_oam_buffer_unused


; =============== S U B R O U T I N E =======================================


sub_CCD8:
                LDY     #0

loc_CCDA:                               ; CODE XREF: sub_CCD8+8↓j
                STA     $104,Y
                INY
                CPY     #$80
                BNE     loc_CCDA
                RTS
; End of function sub_CCD8


; =============== S U B R O U T I N E =======================================

; Wait for the next VBlank to start. Limits the game speed to the TV frame rate

vsync:                                  ; CODE XREF: sub_D0BF+2C↓p
                                        ; sub_D3F4↓p ...
                LDA     byte_19B
                BEQ     loc_CCED
                LDA     vblank_wait_flag ; Set (incremented) in the NMI handler. The application is waiting until this flag is 0 to know that the VBlank handler has not started its work yet
                STA     byte_181

loc_CCED:                               ; CODE XREF: vsync+3↑j
                                        ; vsync+C↓j
                LDA     vblank_wait_flag ; Set (incremented) in the NMI handler. The application is waiting until this flag is 0 to know that the VBlank handler has not started its work yet
                BEQ     loc_CCED
                LDA     #0
                STA     vblank_wait_flag ; Set (incremented) in the NMI handler. The application is waiting until this flag is 0 to know that the VBlank handler has not started its work yet
                RTS
; End of function vsync


; =============== S U B R O U T I N E =======================================

; Process button presses and if DEMO mode is active, simulate their pressing according to the table

handle_pad_buttons:                     ; CODE XREF: j_handle_pad_buttons↑j
                STX     byte_10         ; Save X in temp
                LDA     game_mode       ; 0x80 = demo mode
                BPL     loc_CD2C        ; Check demo play
                DEC     demo_counter_1
                BNE     loc_CD07
                DEC     demo_counter_2
                BNE     loc_CD07
                JMP     main_menu_loop
; ---------------------------------------------------------------------------

loc_CD07:                               ; CODE XREF: handle_pad_buttons+8↑j
                                        ; handle_pad_buttons+C↑j
                DEC     demo_button_update_counter
                BNE     loc_CD22
                JSR     randomize       ; Another PRNG, based on the $C100 table
                AND     #$F
                ADC     #$18
                STA     demo_button_update_counter
                LDA     demo_button_tab_pointer
                AND     #$1F
                STA     demo_button_tab_pointer
                INC     demo_button_tab_pointer
                TAY
                LDA     demo_buttons_tab,Y
                STA     demo_last_buttons

loc_CD22:                               ; CODE XREF: handle_pad_buttons+13↑j
                LDA     demo_last_buttons
                STA     pad_buttons_1
                STA     pad_buttons_2
                STA     pad_test1
                STA     pad_test2

loc_CD2C:                               ; CODE XREF: handle_pad_buttons+4↑j
                LDA     pad_buttons_1
                ORA     pad_buttons_2
                AND     #$F
                TAY
                LDA     pmove_direction_tab,Y
                STA     pmove_direction ; On the Up/Down/Left/Right base, the direction of ship movement is selected (according to the table)
                LDX     byte_10         ; Restore X from temp
                RTS
; End of function handle_pad_buttons

; ---------------------------------------------------------------------------
pmove_direction_tab:.BYTE 4, 7, 1, 4, 3, 6, 0, 3, 5, 8, 2, 5, 4, 7, 1, 4
demo_buttons_tab:.BYTE $C2, $C6, $C0, $CA, $C6, $C1, $C2, $C1, $C4, $C5
                .BYTE $C8, $C6, $C0, $C9, $C5, $C2, $C4, $C0, $C5, $C2
                .BYTE $CA, $C4, $C0, $C1, $C0, $C5, $C1, $CA, $C9, $C0
                .BYTE $C5, $C9

; =============== S U B R O U T I N E =======================================


sub_CD6B:                               ; CODE XREF: sub_C033↑j
                                        ; sub_C8BA:loc_C8BC↑p
                LDA     last_bank_num
                PHA
                LDA     byte_26
                JSR     switch_bank     ; A: bank number
                LDA     byte_2B
                LSR
                BCS     loc_CD7E
                LSR
                BCC     loc_CD8F
                JSR     sub_CE72

loc_CD7E:                               ; CODE XREF: sub_CD6B+B↑j
                LDA     byte_9C
                BEQ     loc_CD86
                LDA     #$D
                BNE     loc_CD88

loc_CD86:                               ; CODE XREF: sub_CD6B+15↑j
                LDA     #$4D ; 'M'

loc_CD88:                               ; CODE XREF: sub_CD6B+19↑j
                SEC
                SBC     byte_21
                BEQ     loc_CD8F
                BCS     loc_CD92

loc_CD8F:                               ; CODE XREF: sub_CD6B+E↑j
                                        ; sub_CD6B+20↑j
                JMP     loc_CE6E
; ---------------------------------------------------------------------------

loc_CD92:                               ; CODE XREF: sub_CD6B+22↑j
                PHA
                CLC
                ADC     #3
                JSR     sub_C7F8
                TAX
                PLA
                STA     byte_12
                STA     byte_13
                LDY     #0
                LDA     byte_2B
                AND     #$20 ; ' '
                BNE     loc_CE21

loc_CDA7:                               ; CODE XREF: sub_CD6B+4D↓j
                                        ; sub_CD6B+76↓j ...
                LDA     ($2C),Y
                INY
                CMP     byte_29
                BNE     loc_CDC6
                LDA     byte_2B
                EOR     #$80
                STA     byte_2B
                LDA     ($2C),Y
                CMP     byte_29
                BNE     loc_CDA7
                LDA     #0
                STA     byte_2B
                LDA     byte_13
                SEC
                SBC     byte_12
                JMP     loc_CE40
; ---------------------------------------------------------------------------

loc_CDC6:                               ; CODE XREF: sub_CD6B+41↑j
                CMP     byte_2A
                BNE     loc_CDE7
                LDA     byte_2B
                EOR     #$40 ; '@'
                STA     byte_2B
                AND     #$40 ; '@'
                BEQ     loc_CDE3
                LDA     ($2C),Y
                PHA
                INY
                LDA     ($2C),Y
                STA     byte_2D
                PLA
                STA     byte_2C
                LDY     #0
                BEQ     loc_CDA7

loc_CDE3:                               ; CODE XREF: sub_CD6B+67↑j
                INY
                INY
                BNE     loc_CDA7

loc_CDE7:                               ; CODE XREF: sub_CD6B+5D↑j
                BIT     byte_2B
                BVC     loc_CE05
                ASL
                ROR     byte_10
                ASL
                ROR     byte_10
                ASL
                ROR     byte_10
                ASL
                ROR     byte_10
                ASL
                ROR     byte_10
                ASL
                ROR     byte_10
                ASL
                ROR     byte_10
                ASL
                ROR     byte_10
                LDA     byte_10

loc_CE05:                               ; CODE XREF: sub_CD6B+7E↑j
                BIT     byte_2B
                BMI     loc_CE14
                STA     $463,X
                INX
                DEC     byte_12
                BEQ     loc_CE3E
                JMP     loc_CDA7
; ---------------------------------------------------------------------------

loc_CE14:                               ; CODE XREF: sub_CD6B+9C↑j
                STA     byte_27
                LDA     byte_2B
                EOR     #$20 ; ' '
                STA     byte_2B
                LDA     ($2C),Y
                INY
                STA     byte_28

loc_CE21:                               ; CODE XREF: sub_CD6B+3A↑j
                LDA     byte_27

loc_CE23:                               ; CODE XREF: sub_CD6B+C2↓j
                STA     $463,X
                INX
                DEC     byte_28
                BEQ     loc_CE31
                DEC     byte_12
                BNE     loc_CE23
                BEQ     loc_CE3E

loc_CE31:                               ; CODE XREF: sub_CD6B+BE↑j
                LDA     byte_2B
                EOR     #$20 ; ' '
                STA     byte_2B
                DEC     byte_12
                BEQ     loc_CE3E
                JMP     loc_CDA7
; ---------------------------------------------------------------------------

loc_CE3E:                               ; CODE XREF: sub_CD6B+A4↑j
                                        ; sub_CD6B+C4↑j ...
                LDA     byte_13

loc_CE40:                               ; CODE XREF: sub_CD6B+58↑j
                LDX     byte_21
                STA     $460,X
                CLC
                ADC     #3
                STA     byte_25
                LDA     byte_2F
                STA     $461,X
                LDA     byte_2E
                STA     $462,X
                LDA     byte_13
                PHA
                JSR     sub_C807
                PLA
                CLC
                ADC     byte_2E
                STA     byte_2E
                BCC     loc_CE64
                INC     byte_2F

loc_CE64:                               ; CODE XREF: sub_CD6B+F5↑j
                TYA
                CLC
                ADC     byte_2C
                STA     byte_2C
                BCC     loc_CE6E
                INC     byte_2D

loc_CE6E:                               ; CODE XREF: sub_CD6B:loc_CD8F↑j
                                        ; sub_CD6B+FF↑j
                PLA
                JMP     switch_bank     ; A: bank number
; End of function sub_CD6B


; =============== S U B R O U T I N E =======================================


sub_CE72:                               ; CODE XREF: sub_CD6B+10↑p
                LDY     #0
                LDA     ($2C),Y
                STA     byte_29
                INY
                LDA     ($2C),Y
                STA     byte_2A
                LDA     #1
                STA     byte_2B
                LDA     byte_2C
                CLC
                ADC     #2
                STA     byte_2C
                BCC     locret_CE8C
                INC     byte_2D

locret_CE8C:                            ; CODE XREF: sub_CE72+16↑j
                RTS
; End of function sub_CE72


; =============== S U B R O U T I N E =======================================


nmi_handler:                            ; DATA XREF: ROM:FFFA↓o
                PHA
                TXA
                PHA
                TYA
                PHA
                LDA     last_ppu_ctrl2
                PHA
                AND     #$E7
                STA     last_ppu_ctrl2
                STA     PPU_CTRL_REG2
                INC     vblank_wait_flag ; Set (incremented) in the NMI handler. The application is waiting until this flag is 0 to know that the VBlank handler has not started its work yet
                LDA     PPU_STATUS
                STA     ppu_status_in_vblank ; The VBlank handler saves here the contents of the PPU_STATUS register
                LDA     #0
                STA     PPU_SPR_ADDR
                LDA     #2              ; Start OAM DMA. Page 2 ($200) is traditionally used for the transfer
                STA     PPU_SPR_DMA
                JSR     ppu_update_background ; Large procedure dealing with updating the Name Table/Attr Table to display a portion of the current game level
                JSR     ppu_cram_update ; Update CRAM (palette). $145 contains a fresh palette
                JSR     ppu_update_scroll_regs
                PLA
                STA     last_ppu_ctrl2
                STA     PPU_CTRL_REG2
                JSR     apu_update      ; A typical approach for updating the APU state. Called from VBlank handler (every frame)
                PLA
                TAY
                PLA
                TAX
                PLA
; End of function nmi_handler


; =============== S U B R O U T I N E =======================================


dummy_irq_handler:                      ; DATA XREF: ROM:FFFE↓o
                RTI
; End of function dummy_irq_handler


; =============== S U B R O U T I N E =======================================

; Large procedure dealing with updating the Name Table/Attr Table to display a portion of the current game level

ppu_update_background:                  ; CODE XREF: j_ppu_update_background↑j
                                        ; sub_C8BA+12↑p ...
                LDA     byte_90
                BPL     loc_CECC
                JSR     sub_C6B5

loc_CECC:                               ; CODE XREF: ppu_update_background+2↑j
                LDA     byte_9C
                BNE     loc_CED3
                JMP     loc_CF70
; ---------------------------------------------------------------------------

loc_CED3:                               ; CODE XREF: ppu_update_background+9↑j
                CMP     #1
                BNE     loc_CF27
                INC     byte_9C
                LDA     byte_A0
                ORA     #3
                STA     PPU_ADDRESS
                LDA     byte_9E
                STA     PPU_ADDRESS
                LDY     byte_9D
                LDA     $104,Y
                STA     PPU_DATA
                LDA     $105,Y
                STA     PPU_DATA
                LDA     $106,Y
                STA     PPU_DATA
                LDA     $107,Y
                STA     PPU_DATA
                LDA     $108,Y
                STA     PPU_DATA
                LDA     $109,Y
                STA     PPU_DATA
                LDA     $10A,Y
                STA     PPU_DATA
                LDA     $10B,Y
                STA     PPU_DATA
                LDA     byte_A0
                STA     PPU_ADDRESS
                LDA     byte_9F
                ORA     #$20 ; ' '
                STA     PPU_ADDRESS
                LDY     #$20 ; ' '
                BNE     loc_CF37

loc_CF27:                               ; CODE XREF: ppu_update_background+10↑j
                LDA     #0
                STA     byte_9C
                LDA     byte_A0
                STA     PPU_ADDRESS
                LDA     byte_9F
                STA     PPU_ADDRESS
                LDY     #0

loc_CF37:                               ; CODE XREF: ppu_update_background+60↑j
                CLC

loc_CF38:                               ; CODE XREF: ppu_update_background+A9↓j
                LDA     $470,Y
                STA     PPU_DATA
                LDA     $471,Y
                STA     PPU_DATA
                LDA     $472,Y
                STA     PPU_DATA
                LDA     $473,Y
                STA     PPU_DATA
                LDA     $474,Y
                STA     PPU_DATA
                LDA     $475,Y
                STA     PPU_DATA
                LDA     $476,Y
                STA     PPU_DATA
                LDA     $477,Y
                STA     PPU_DATA
                TYA
                ADC     #8
                TAY
                AND     #$1F
                BNE     loc_CF38

loc_CF70:                               ; CODE XREF: ppu_update_background+B↑j
                LDA     byte_20
                BNE     loc_CF75
                RTS
; ---------------------------------------------------------------------------

loc_CF75:                               ; CODE XREF: ppu_update_background+AD↑j
                LDX     #0
                STX     byte_21

loc_CF79:                               ; CODE XREF: ppu_update_background+10F↓j
                LDA     $460,X
                INX
                TAY
                BPL     loc_CFDE
                AND     #$7F
                TAY
                ASL
                BPL     loc_CFD7
                ASL
                BPL     loc_CF8C
                JMP     loc_CFFE
; ---------------------------------------------------------------------------

loc_CF8C:                               ; CODE XREF: ppu_update_background+C2↑j
                LSR
                LSR
                ADC     #$20 ; ' '
                TAY
                ORA     #3
                STA     PPU_ADDRESS
                LDA     $465,X
                STA     PPU_ADDRESS
                LDA     $466,X
                STA     PPU_DATA

loc_CFA2:                               ; CODE XREF: ppu_update_background+169↓j
                STY     PPU_ADDRESS
                LDA     $460,X
                STA     PPU_ADDRESS
                LDA     $461,X
                STA     PPU_DATA
                LDA     $462,X
                STA     PPU_DATA
                STY     PPU_ADDRESS
                LDA     $460,X
                ORA     #$20 ; ' '
                STA     PPU_ADDRESS
                LDA     $463,X
                STA     PPU_DATA
                LDA     $464,X
                STA     PPU_DATA
                TXA
                ADC     #7
                TAX

loc_CFD2:                               ; CODE XREF: ppu_update_background+136↓j
                DEC     byte_20
                BNE     loc_CF79
                RTS
; ---------------------------------------------------------------------------

loc_CFD7:                               ; CODE XREF: ppu_update_background+BF↑j
                LDA     last_ppu_ctrl1
                ORA     #4
                STA     PPU_CTRL_REG1

loc_CFDE:                               ; CODE XREF: ppu_update_background+B9↑j
                LDA     $460,X
                STA     PPU_ADDRESS
                INX
                LDA     $460,X
                STA     PPU_ADDRESS
                INX

loc_CFEC:                               ; CODE XREF: ppu_update_background+12F↓j
                LDA     $460,X
                STA     PPU_DATA
                INX
                DEY
                BNE     loc_CFEC
                LDA     last_ppu_ctrl1
                STA     PPU_CTRL_REG1
                JMP     loc_CFD2
; ---------------------------------------------------------------------------

loc_CFFE:                               ; CODE XREF: ppu_update_background+C4↑j
                LSR
                LSR
                AND     #$F
                ADC     #$20 ; ' '
                PHA
                ORA     #3
                STA     PPU_ADDRESS
                TAY
                LDA     $465,X
                STA     PPU_ADDRESS
                LDA     PPU_DATA
                LDA     PPU_DATA
                AND     $467,X
                ORA     $466,X
                PHA
                STY     PPU_ADDRESS
                LDA     $465,X
                STA     PPU_ADDRESS
                PLA
                STA     PPU_DATA
                PLA
                TAY
                SEC
                JMP     loc_CFA2
; End of function ppu_update_background


; =============== S U B R O U T I N E =======================================

; Update CRAM (palette). $145 contains a fresh palette

ppu_cram_update:                        ; CODE XREF: nmi_handler+23↑p
                LDA     cram_update_mode
                BEQ     locret_D0A3
                CMP     #2
                BEQ     loc_D04C
                LDY     #0
                LDX     #$3F ; '?'
                STX     PPU_ADDRESS
                STY     PPU_ADDRESS
                LDX     #4
                CMP     #3
                BEQ     loc_D05A
                BNE     loc_D058

loc_D04C:                               ; CODE XREF: ppu_cram_update+7↑j
                LDA     #$3F ; '?'
                STA     PPU_ADDRESS
                LDA     #$10
                STA     PPU_ADDRESS
                LDY     #$C

loc_D058:                               ; CODE XREF: ppu_cram_update+19↑j
                LDX     #2

loc_D05A:                               ; CODE XREF: ppu_cram_update+17↑j
                CLC

loc_D05B:                               ; CODE XREF: ppu_cram_update+5F↓j
                LDA     cram_new_palette ; Reserved 0x19 bytes for new palette (CRAM)
                STA     PPU_DATA
                LDA     $146,Y
                STA     PPU_DATA
                LDA     $147,Y
                STA     PPU_DATA
                LDA     $148,Y
                STA     PPU_DATA
                LDA     cram_new_palette ; Reserved 0x19 bytes for new palette (CRAM)
                STA     PPU_DATA
                LDA     $149,Y
                STA     PPU_DATA
                LDA     $14A,Y
                STA     PPU_DATA
                LDA     $14B,Y
                STA     PPU_DATA
                TYA
                ADC     #6
                TAY
                DEX
                BNE     loc_D05B
                LDA     #$3F ; '?'
                STA     PPU_ADDRESS
                STX     PPU_ADDRESS
                STX     PPU_ADDRESS
                STX     PPU_ADDRESS
                STX     cram_update_mode

locret_D0A3:                            ; CODE XREF: ppu_cram_update+3↑j
                RTS
; End of function ppu_cram_update


; =============== S U B R O U T I N E =======================================


sub_D0A4:                               ; CODE XREF: sub_EA0B+20↓p
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$FE
                CMP     game_mode       ; 0x80 = demo mode
                BEQ     locret_D0BE
                LDY     byte_32
                BNE     loc_D0BA
                ORA     #2
                STA     game_mode       ; 0x80 = demo mode
                BPL     locret_D0B9
                JMP     main_menu_loop
; ---------------------------------------------------------------------------

locret_D0B9:                            ; CODE XREF: sub_D0A4+10↑j
                RTS
; ---------------------------------------------------------------------------

loc_D0BA:                               ; CODE XREF: sub_D0A4+A↑j
                ORA     #$40 ; '@'
                STA     game_mode       ; 0x80 = demo mode

locret_D0BE:                            ; CODE XREF: sub_D0A4+6↑j
                                        ; sub_D0BF+4↓j
                RTS
; End of function sub_D0A4


; =============== S U B R O U T I N E =======================================


sub_D0BF:                               ; CODE XREF: sub_E81E+6D↓p
                LDA     game_mode       ; 0x80 = demo mode
                AND     #2
                BEQ     locret_D0BE
                LDA     #9
                STA     byte_66
                JSR     set_new_high_score
                JSR     apu_stop
                LDY     #4
                LDA     pad_buttons_1
                AND     #$20 ; ' '
                BEQ     loc_D0D9
                LDY     #$23 ; '#'

loc_D0D9:                               ; CODE XREF: sub_D0BF+16↑j
                TYA
                JSR     apu_play        ; A = sound num (1...35)
                PLA
                PLA
                LDA     #3
                STA     byte_6F
                LDA     #0
                STA     byte_300

loc_D0E8:                               ; CODE XREF: sub_D0BF+44↓j
                JSR     sub_D7E3
                JSR     vsync           ; Wait for the next VBlank to start. Limits the game speed to the TV frame rate
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                JSR     j_game_logic_processing
                JSR     clear_first_last_sprites ; Clear the last 4 and the first 4 sprites according to the byte_39 variable condition. The game uses these sprites in a special way (how?)
                LDA     byte_300
                BEQ     loc_D0FF
                JMP     main_menu_loop
; ---------------------------------------------------------------------------

loc_D0FF:                               ; CODE XREF: sub_D0BF+3B↑j
                LDA     pad_test1
                AND     #$10
                BEQ     loc_D0E8
                JMP     main_menu_loop
; End of function sub_D0BF


; =============== S U B R O U T I N E =======================================


check_high_score_reached:               ; CODE XREF: sub_D193+D↓p
                BIT     byte_39
                BMI     locret_D12B
                BVS     locret_D12B
                LDY     #7
                SEC

loc_D111:                               ; CODE XREF: check_high_score_reached+10↓j
                LDA     $179,Y          ; Score
                SBC     $181,Y          ; High Score
                DEY
                BNE     loc_D111
                BCC     locret_D12B
                LDA     #$40 ; '@'
                STA     byte_39
                LDA     game_mode       ; 0x80 = demo mode
                AND     #4
                BNE     locret_D12B
                LDA     #9              ; Play High-Score reached sound
                JSR     apu_play        ; A = sound num (1...35)

locret_D12B:                            ; CODE XREF: check_high_score_reached+2↑j
                                        ; check_high_score_reached+4↑j ...
                RTS
; End of function check_high_score_reached


; =============== S U B R O U T I N E =======================================


sub_D12C:                               ; CODE XREF: sub_D12C:loc_D189↓j
                                        ; sub_D193+10↓j
                LDA     byte_32
                BEQ     locret_D12B
                LDY     #4
                SEC

loc_D133:                               ; CODE XREF: sub_D12C+E↓j
                LDA     $179,Y
                SBC     $175,Y
                DEY
                BNE     loc_D133
                BCC     locret_D12B
                LDA     byte_176
                BNE     loc_D15B
                LDA     byte_177
                BNE     loc_D15B
                LDA     byte_178
                BNE     loc_D15B
                LDA     byte_179
                CMP     #2
                BNE     loc_D15B
                LDA     #8
                STA     byte_179
                BNE     loc_D177

loc_D15B:                               ; CODE XREF: sub_D12C+15↑j
                                        ; sub_D12C+1A↑j ...
                CLC
                LDA     #8
                STA     byte_10
                LDY     #4

loc_D162:                               ; CODE XREF: sub_D12C+49↓j
                LDA     $175,Y
                ADC     byte_10
                CMP     #$A
                BCC     loc_D16D
                SBC     #$A

loc_D16D:                               ; CODE XREF: sub_D12C+3D↑j
                STA     $175,Y
                LDA     #0
                STA     byte_10
                DEY
                BNE     loc_D162

loc_D177:                               ; CODE XREF: sub_D12C+2D↑j
                INC     byte_32
                BNE     loc_D17E
                DEC     byte_32
                RTS
; ---------------------------------------------------------------------------

loc_D17E:                               ; CODE XREF: sub_D12C+4D↑j
                LDA     game_mode       ; 0x80 = demo mode
                AND     #4
                BNE     loc_D189
                LDA     #8              ; Play +1 Live sound (Compile Fanfare)
                JSR     apu_play        ; A = sound num (1...35)

loc_D189:                               ; CODE XREF: sub_D12C+56↑j
                JMP     sub_D12C
; End of function sub_D12C


; =============== S U B R O U T I N E =======================================


sub_D18C:                               ; CODE XREF: sub_C02A↑j
                LDA     $7E6,X
                TAY
                LDA     unk_D1E1,Y
; End of function sub_D18C


; =============== S U B R O U T I N E =======================================


sub_D193:                               ; CODE XREF: sub_C057↑j
                STA     byte_10
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$80            ; Check DEMO mode
                BNE     locret_D1E0
                LDA     byte_10
                JSR     sub_D1A6
                JSR     check_high_score_reached
                JMP     sub_D12C
; End of function sub_D193


; =============== S U B R O U T I N E =======================================


sub_D1A6:                               ; CODE XREF: sub_C081↑j
                                        ; sub_D193+A↑p
                STA     byte_10
                ASL
                CLC
                ADC     byte_10
                ASL
                ADC     #6
                TAY
                STX     byte_10
                LDX     #5
                CLC

loc_D1B5:                               ; CODE XREF: sub_D1A6+20↓j
                LDA     unk_D23B,Y
                ADC     $17B,X
                CMP     #$A
                BCC     loc_D1C1
                SBC     #$A

loc_D1C1:                               ; CODE XREF: sub_D1A6+17↑j
                STA     $17B,X
                DEY
                DEX
                BPL     loc_D1B5
                LDA     #0
                ADC     byte_17A
                STA     byte_17A
                CMP     #$A
                BCC     loc_D1DE
                LDA     #9
                LDY     #7

loc_D1D8:                               ; CODE XREF: sub_D1A6+36↓j
                STA     $179,Y
                DEY
                BNE     loc_D1D8

loc_D1DE:                               ; CODE XREF: sub_D1A6+2C↑j
                LDX     byte_10

locret_D1E0:                            ; CODE XREF: sub_D193+6↑j
                RTS
; End of function sub_D1A6

; ---------------------------------------------------------------------------
unk_D1E1:       .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE   7
                .BYTE   4
                .BYTE   4
                .BYTE   6
                .BYTE   6
                .BYTE   6
                .BYTE  $A
                .BYTE   7
                .BYTE   7
                .BYTE   8
                .BYTE   0
                .BYTE   2
                .BYTE   1
                .BYTE   7
                .BYTE   7
                .BYTE   0
                .BYTE   9
                .BYTE   8
                .BYTE   7
                .BYTE   9
                .BYTE   8
                .BYTE   6
unk_D200:       .BYTE   6
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   7
                .BYTE   9
                .BYTE   2
                .BYTE   2
                .BYTE   2
                .BYTE   0
                .BYTE   7
                .BYTE   3
                .BYTE   3
                .BYTE   2
                .BYTE   3
                .BYTE   6
                .BYTE   8
                .BYTE  $A
                .BYTE  $A
                .BYTE  $B
                .BYTE  $B
                .BYTE  $C
                .BYTE $14
                .BYTE   0
                .BYTE   0
                .BYTE   2
                .BYTE   3
                .BYTE   4
                .BYTE   2
                .BYTE   0
                .BYTE   7
                .BYTE $10
                .BYTE   4
                .BYTE   9
                .BYTE  $A
                .BYTE  $B
                .BYTE   7
                .BYTE  $B
                .BYTE  $A
                .BYTE   6
                .BYTE   6
                .BYTE   0
                .BYTE  $B
                .BYTE  $A
                .BYTE   9
                .BYTE  $A
                .BYTE   9
                .BYTE  $A
                .BYTE  $B
                .BYTE   0
                .BYTE $14
                .BYTE   6
                .BYTE   0
                .BYTE   8
                .BYTE   9
                .BYTE   9
                .BYTE   7
                .BYTE   7
                .BYTE   8
unk_D23B:       .BYTE  $D
unk_D23C:       .BYTE   0
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
                .BYTE   1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   6
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   7
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   2
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   5
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   2
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   4
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   8
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   5
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   2
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   3
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   4
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   5
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   7
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   2
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0

; =============== S U B R O U T I N E =======================================


sub_D2C0:                               ; CODE XREF: sub_C048↑j
                LDA     unk_D23C,X
                RTS
; End of function sub_D2C0


; =============== S U B R O U T I N E =======================================


sub_D2C4:
                LDA     unk_D23C,Y
                RTS
; End of function sub_D2C4


; =============== S U B R O U T I N E =======================================

; Clear the last 4 and the first 4 sprites according to the byte_39 variable condition. The game uses these sprites in a special way (how?)

clear_first_last_sprites:               ; CODE XREF: sub_D0BF+35↑p
                                        ; sub_EA0B+1D↓p
                BIT     byte_39
                BVC     locret_D2E3
                LDA     byte_39
                AND     #4
                BNE     loc_D2E1
                LDA     #$F8
                LDY     #$F0

loc_D2D6:                               ; CODE XREF: clear_first_last_sprites+17↓j
                STA     $200,Y
                INY
                INY
                INY
                INY
                CPY     #$10
                BNE     loc_D2D6

loc_D2E1:                               ; CODE XREF: clear_first_last_sprites+8↑j
                INC     byte_39

locret_D2E3:                            ; CODE XREF: clear_first_last_sprites+2↑j
                RTS
; End of function clear_first_last_sprites


; =============== S U B R O U T I N E =======================================


set_new_high_score:                     ; CODE XREF: sub_D0BF+A↑p
                                        ; show_ending_screen+C↓p
                LDY     #7

loc_D2E6:                               ; CODE XREF: set_new_high_score+9↓j
                LDA     $179,Y
                SBC     $181,Y
                DEY
                BNE     loc_D2E6
                BCC     locret_D2FC
                LDY     #7

loc_D2F3:                               ; CODE XREF: set_new_high_score+16↓j
                LDA     $179,Y
                STA     $181,Y
                DEY
                BNE     loc_D2F3

locret_D2FC:                            ; CODE XREF: set_new_high_score+B↑j
                RTS
; End of function set_new_high_score


; =============== S U B R O U T I N E =======================================


sub_D2FD:
                LDA     #$A
                JSR     sub_C7D8
                BCS     locret_D346
                JSR     sub_C7F8
                STX     byte_10
                TAX
                LDA     #0
                STA     byte_11
                TAY
                LDA     #7
                STA     $460,X
                LDA     #$19
                STA     $461,X
                LDA     #$18
                STA     $462,X

loc_D31E:                               ; CODE XREF: sub_D2FD+3C↓j
                LDA     byte_11
                BNE     loc_D32E
                LDA     ($16),Y
                ORA     byte_11
                STA     byte_11
                BNE     loc_D330
                LDA     #$20 ; ' '
                BNE     loc_D332

loc_D32E:                               ; CODE XREF: sub_D2FD+23↑j
                LDA     ($16),Y

loc_D330:                               ; CODE XREF: sub_D2FD+2B↑j
                ADC     #$30 ; '0'

loc_D332:                               ; CODE XREF: sub_D2FD+2F↑j
                STA     $463,X
                INY
                INX
                CPY     #6
                BNE     loc_D31E
                LDA     #$30 ; '0'
                STA     $463,X
                LDX     byte_65
                JSR     sub_C807
                CLC

locret_D346:                            ; CODE XREF: sub_D2FD+5↑j
                RTS
; End of function sub_D2FD


; =============== S U B R O U T I N E =======================================


sub_D347:                               ; CODE XREF: show_main_menu+A1↓p
                                        ; show_main_menu+23D↓p ...
                LDY     #$30 ; '0'
                STY     byte_11
                STY     byte_12
                SEC
                SBC     #$64 ; 'd'
                BCC     loc_D35C
                INC     byte_11
                SBC     #$64 ; 'd'
                BCC     loc_D35C
                INC     byte_11
                SBC     #$64 ; 'd'

loc_D35C:                               ; CODE XREF: sub_D347+9↑j
                                        ; sub_D347+F↑j
                ADC     #$64 ; 'd'

loc_D35E:                               ; CODE XREF: sub_D347+1D↓j
                SBC     #$A
                BCC     loc_D366
                INC     byte_12
                BNE     loc_D35E

loc_D366:                               ; CODE XREF: sub_D347+19↑j
                ADC     #$3A ; ':'
                STA     byte_13
                LDA     byte_11
                CMP     #$30 ; '0'
                BNE     loc_D37E
                LDA     #$20 ; ' '
                STA     byte_11
                LDA     byte_12
                CMP     #$30 ; '0'
                BNE     loc_D37E
                LDA     #$20 ; ' '
                STA     byte_12

loc_D37E:                               ; CODE XREF: sub_D347+27↑j
                                        ; sub_D347+31↑j
                LDA     #6
                JSR     sub_C7EB
                JSR     sub_C7F8
                TAY
                LDA     #3
                STA     $460,Y
                LDA     byte_19
                STA     $461,Y
                LDA     byte_18
                STA     $462,Y
                LDA     byte_11
                STA     $463,Y
                LDA     byte_12
                STA     $464,Y
                LDA     byte_13
                STA     $465,Y
                JMP     sub_C807
; End of function sub_D347


; =============== S U B R O U T I N E =======================================


sub_D3A8:                               ; CODE XREF: sub_EA0B:loc_EA22↓p
                LDA     pad_test1
                AND     #$10
                BNE     loc_D3AF
                RTS
; ---------------------------------------------------------------------------

loc_D3AF:                               ; CODE XREF: sub_D3A8+4↑j
                JSR     sub_F0F4
                LDA     byte_83
                PHA
                LDA     byte_17A
                PHA

loc_D3B9:                               ; CODE XREF: sub_D3A8+31↓j
                JSR     sub_D3F4
                LDA     byte_19B
                BEQ     loc_D3D5
                LDA     pad_test1
                AND     #$20 ; ' '
                BEQ     loc_D3D2
                INC     byte_7A
                LDA     byte_7A
                AND     #7
                STA     byte_7A
                STA     byte_17A

loc_D3D2:                               ; CODE XREF: sub_D3A8+1D↑j
                JSR     sub_D3FF

loc_D3D5:                               ; CODE XREF: sub_D3A8+17↑j
                LDA     pad_test1
                AND     #$10
                BEQ     loc_D3B9
                JSR     sub_F0FA
                LDY     #$1E

loc_D3E0:                               ; CODE XREF: sub_D3A8+40↓j
                TYA
                PHA
                JSR     sub_D3F4
                PLA
                TAY
                DEY
                BNE     loc_D3E0
                PLA
                STA     byte_17A
                PLA
                EOR     #4
                STA     byte_83

locret_D3F3:                            ; CODE XREF: sub_D3FF+3↓j
                RTS
; End of function sub_D3A8


; =============== S U B R O U T I N E =======================================


sub_D3F4:                               ; CODE XREF: sub_D3A8:loc_D3B9↑p
                                        ; sub_D3A8+3A↑p
                JSR     vsync           ; Wait for the next VBlank to start. Limits the game speed to the TV frame rate
                JSR     j_update_score_onscreen
                INC     byte_83
                JMP     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
; End of function sub_D3F4


; =============== S U B R O U T I N E =======================================


sub_D3FF:                               ; CODE XREF: sub_D3A8:loc_D3D2↑p
                JSR     sub_E76C
                BEQ     locret_D3F3
                LDA     temp_0
                AND     #1
                BEQ     loc_D40E
                LDA     #$FF
                STA     byte_32

loc_D40E:                               ; CODE XREF: sub_D3FF+9↑j
                LDA     temp_0
                AND     #$80
                BEQ     loc_D427
                LDA     byte_7D
                CLC
                ADC     #1
                CMP     #$D
                BNE     loc_D41F
                LDA     #1

loc_D41F:                               ; CODE XREF: sub_D3FF+1C↑j
                STA     byte_92
                LDA     game_mode       ; 0x80 = demo mode
                ORA     #$30 ; '0'
                STA     game_mode       ; 0x80 = demo mode

loc_D427:                               ; CODE XREF: sub_D3FF+13↑j
                LDA     temp_0
                AND     #$40 ; '@'
                BEQ     loc_D43E
                LDA     byte_7D
                SEC
                SBC     #1
                BNE     loc_D436
                LDA     #$C

loc_D436:                               ; CODE XREF: sub_D3FF+33↑j
                STA     byte_92
                LDA     game_mode       ; 0x80 = demo mode
                ORA     #$30 ; '0'
                STA     game_mode       ; 0x80 = demo mode

loc_D43E:                               ; CODE XREF: sub_D3FF+2C↑j
                LDA     temp_1
                AND     #2
                BEQ     loc_D454
                LDA     #1
                STA     byte_51
                LDA     byte_C0
                AND     #$22 ; '"'
                CMP     #2
                BNE     loc_D454
                LDA     #0
                STA     byte_C2

loc_D454:                               ; CODE XREF: sub_D3FF+43↑j
                                        ; sub_D3FF+4F↑j
                LDA     byte_8
                AND     #2
                BEQ     locret_D462
                LDA     #0
                STA     byte_AC
                LDA     #8
                STA     byte_AD

locret_D462:                            ; CODE XREF: sub_D3FF+59↑j
                RTS
; End of function sub_D3FF


; =============== S U B R O U T I N E =======================================


sub_D463:                               ; CODE XREF: sub_D480+20↓p
                JSR     j_check_enemy_overflow
                BCS     locret_D471
                LDA     #0
                STA     byte_46
                LDA     #$44 ; 'D'
                STA     $528,Y

locret_D471:                            ; CODE XREF: sub_D463+3↑j
                RTS
; End of function sub_D463


; =============== S U B R O U T I N E =======================================


sub_D472:                               ; CODE XREF: sub_D480+3A↓p
                JSR     j_check_enemy_overflow
                BCS     loc_D47D
                LDA     #$3D ; '='
                STA     $528,Y
                RTS
; ---------------------------------------------------------------------------

loc_D47D:                               ; CODE XREF: sub_D472+3↑j
                DEC     byte_4D
                RTS
; End of function sub_D472


; =============== S U B R O U T I N E =======================================


sub_D480:                               ; CODE XREF: sub_E81E+70↓p
                LDA     game_mode       ; 0x80 = demo mode
                AND     #8
                BEQ     loc_D487

locret_D486:                            ; CODE XREF: sub_D480+1A↓j
                RTS
; ---------------------------------------------------------------------------

loc_D487:                               ; CODE XREF: sub_D480+4↑j
                LDA     byte_62
                AND     #1
                BEQ     loc_D490
                JSR     sub_D5D7

loc_D490:                               ; CODE XREF: sub_D480+B↑j
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$10
                BEQ     loc_D49C
                LDA     byte_C0
                AND     #2
                BEQ     locret_D486

loc_D49C:                               ; CODE XREF: sub_D480+14↑j
                LDA     byte_46
                BEQ     loc_D4A3
                JSR     sub_D463

loc_D4A3:                               ; CODE XREF: sub_D480+1E↑j
                LDA     byte_528
                CMP     #$81
                BNE     loc_D4DE
                DEC     byte_52
                BNE     loc_D4DE
                LDA     byte_53
                STA     byte_52
                LDA     byte_4D
                INC     byte_4D
                AND     #$F
                BNE     loc_D4BD
                JSR     sub_D472

loc_D4BD:                               ; CODE XREF: sub_D480+38↑j
                LDA     byte_62
                AND     #$A
                CMP     #2
                BNE     loc_D4DE
                JSR     j_check_enemy_overflow
                BCS     loc_D4DE
                LDX     byte_50
                LDA     random_enemy_tab,X
                STA     $528,Y
                JSR     sub_D5C1
                INX
                CPX     byte_4F
                BNE     loc_D4DC
                LDX     byte_4E

loc_D4DC:                               ; CODE XREF: sub_D480+58↑j
                STX     byte_50

loc_D4DE:                               ; CODE XREF: sub_D480+28↑j
                                        ; sub_D480+2C↑j ...
                LDA     byte_90
                AND     #1
                BEQ     locret_D4F0
                LDA     byte_A2
                AND     #$1F
                BNE     locret_D4F0
                LDA     byte_6E
                BEQ     loc_D4F1
                DEC     byte_6E

locret_D4F0:                            ; CODE XREF: sub_D480+62↑j
                                        ; sub_D480+68↑j
                RTS
; ---------------------------------------------------------------------------

loc_D4F1:                               ; CODE XREF: sub_D480+6C↑j
                JSR     switch_bank_5
                LDY     #0
                LDA     #1
                STA     byte_14
; End of function sub_D480


; =============== S U B R O U T I N E =======================================


sub_D4FA:                               ; CODE XREF: sub_D5AB+4↓j
                LDA     ($6C),Y
                INY
                STY     byte_15
                CMP     #5
                BCS     sub_D56B
                JSR     switch_case     ; Old-school Switch-Case. Right after the call there is a jump table of variable size.
; End of function sub_D4FA

; ---------------------------------------------------------------------------
                .WORD $D50F             ; sub_D510 - 1
                .WORD $D519             ; sub_D51A - 1
                .WORD $D527             ; sub_D528 - 1
                .WORD $D54C             ; sub_D54D - 1
                .WORD $D558             ; sub_D559 - 1

; =============== S U B R O U T I N E =======================================


sub_D510:
                LDY     byte_15
                LDA     ($6C),Y
                STA     byte_6E
                INY
                JMP     sub_D5AB
; End of function sub_D510


; =============== S U B R O U T I N E =======================================


sub_D51A:
                LDY     byte_15
                LDA     byte_62
                AND     #$F9
                ORA     ($6C),Y
                STA     byte_62
                INY
                JMP     sub_D5AB
; End of function sub_D51A


; =============== S U B R O U T I N E =======================================


sub_D528:
                LDY     byte_15
                LDA     ($6C),Y
                BMI     loc_D538
                CLC
                ADC     byte_4B
                BCC     loc_D535
                LDA     #$FF

loc_D535:                               ; CODE XREF: sub_D528+9↑j
                JMP     loc_D541
; ---------------------------------------------------------------------------

loc_D538:                               ; CODE XREF: sub_D528+4↑j
                CLC
                ADC     byte_4B
                BCS     loc_D53F
                LDA     #0

loc_D53F:                               ; CODE XREF: sub_D528+13↑j
                STA     byte_47

loc_D541:                               ; CODE XREF: sub_D528:loc_D535↑j
                STA     byte_4B
                LDA     byte_62
                ORA     #1
                STA     byte_62
                INY
                JMP     sub_D5AB
; End of function sub_D528


; =============== S U B R O U T I N E =======================================


sub_D54D:
                LDY     byte_15
                LDA     ($6C),Y
                STA     byte_14
                INC     byte_14
                INY
                JMP     sub_D5AB
; End of function sub_D54D


; =============== S U B R O U T I N E =======================================


sub_D559:
                LDY     byte_15
                LDA     ($6C),Y
                PHA
                INY
                LDA     ($6C),Y
                STA     byte_6D
                PLA
                STA     byte_6C
                LDY     #0
                JMP     sub_D5AB
; End of function sub_D559


; =============== S U B R O U T I N E =======================================


sub_D56B:                               ; CODE XREF: sub_D4FA+7↑j
                STA     byte_16
                LDA     byte_62
                AND     #8
                BEQ     loc_D57C

loc_D573:                               ; CODE XREF: sub_D56B+1A↓j
                LDA     byte_16
                BPL     sub_D5AB
                INY
                INY
                JMP     sub_D5AB
; ---------------------------------------------------------------------------

loc_D57C:                               ; CODE XREF: sub_D56B+6↑j
                STY     byte_15
                JSR     sub_DD9E
                STY     byte_17
                LDY     byte_15
                BCS     loc_D573
                JSR     sub_D5C1
                LDX     byte_17
                LDA     byte_16
                BMI     loc_D595
                STA     $528,X
                BPL     sub_D5AB

loc_D595:                               ; CODE XREF: sub_D56B+23↑j
                AND     #$7F
                STA     $576,X
                LDA     ($6C),Y
                INY
                STA     $590,X
                LDA     ($6C),Y
                INY
                STA     $660,X
                LDA     #$45 ; 'E'
                STA     $528,X
; End of function sub_D56B


; =============== S U B R O U T I N E =======================================


sub_D5AB:                               ; CODE XREF: sub_D510+7↑j
                                        ; sub_D51A+B↑j ...
                DEC     byte_14
                BEQ     loc_D5B2
                JMP     sub_D4FA
; ---------------------------------------------------------------------------

loc_D5B2:                               ; CODE XREF: sub_D5AB+2↑j
                TYA
                CLC
                ADC     byte_6C
                STA     byte_6C
                LDA     byte_6D
                ADC     #0
                STA     byte_6D
                JMP     switch_bank_6
; End of function sub_D5AB


; =============== S U B R O U T I N E =======================================


sub_D5C1:                               ; CODE XREF: sub_D480+52↑p
                                        ; sub_D56B+1C↑p
                LDA     byte_48
                CLC
                ADC     #4
                BCC     loc_D5CE
                INC     byte_47
                BNE     loc_D5CE
                DEC     byte_47

loc_D5CE:                               ; CODE XREF: sub_D5C1+5↑j
                                        ; sub_D5C1+9↑j
                STA     byte_48
                INC     byte_4C
                BNE     locret_D5D6
                DEC     byte_4C

locret_D5D6:                            ; CODE XREF: sub_D5C1+11↑j
                RTS
; End of function sub_D5C1


; =============== S U B R O U T I N E =======================================


sub_D5D7:                               ; CODE XREF: sub_C07B↑j
                                        ; sub_D480+D↑p
                LDA     byte_62
                AND     #$FE
                STA     byte_62
                LDA     byte_4B
                CLC
                ADC     byte_47
                BCC     loc_D5E6
                LDA     #$FF

loc_D5E6:                               ; CODE XREF: sub_D5D7+B↑j
                LSR
                CMP     #$10
                BCC     loc_D5F3
                SBC     #$10
                LSR
                LSR
                TAY
                LDA     unk_D61F,Y

loc_D5F3:                               ; CODE XREF: sub_D5D7+12↑j
                STA     byte_4E
                CMP     byte_50
                BCC     loc_D5FB
                STA     byte_50

loc_D5FB:                               ; CODE XREF: sub_D5D7+20↑j
                LDA     byte_47
                CLC
                ADC     byte_49
                BCC     loc_D604
                LDA     #$FF

loc_D604:                               ; CODE XREF: sub_D5D7+29↑j
                LSR
                LSR
                LSR
                LSR
                TAY
                LDA     unk_D63B,Y
                CLC
                ADC     byte_4E
                STA     byte_4F
                CMP     byte_50
                BCS     loc_D617
                STA     byte_50

loc_D617:                               ; CODE XREF: sub_D5D7+3C↑j
                LDA     unk_D64B,Y
                STA     byte_53
                STA     byte_52
                RTS
; End of function sub_D5D7

; ---------------------------------------------------------------------------
unk_D61F:       .BYTE $10
                .BYTE $14
                .BYTE $20
                .BYTE $12
                .BYTE $28 ; (
                .BYTE $10
                .BYTE   8
                .BYTE $18
                .BYTE $30 ; 0
                .BYTE $28 ; (
                .BYTE $24 ; $
                .BYTE   4
                .BYTE   8
                .BYTE $20
                .BYTE $10
                .BYTE $28 ; (
                .BYTE $2C ; ,
                .BYTE $34 ; 4
                .BYTE $25 ; %
                .BYTE $15
                .BYTE $33 ; 3
                .BYTE $12
                .BYTE $18
                .BYTE $2E ; .
                .BYTE $35 ; 5
                .BYTE   6
                .BYTE $20
                .BYTE $37 ; 7
unk_D63B:       .BYTE   1
                .BYTE   2
                .BYTE   1
                .BYTE   1
                .BYTE   2
                .BYTE   2
                .BYTE   1
                .BYTE   2
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   2
                .BYTE   3
                .BYTE   2
                .BYTE   4
                .BYTE   5
unk_D64B:       .BYTE $30 ; 0
                .BYTE $38 ; 8
                .BYTE $3C ; <
                .BYTE $34 ; 4
                .BYTE $38 ; 8
                .BYTE $3C ; <
                .BYTE $30 ; 0
                .BYTE $34 ; 4
                .BYTE $40 ; @
                .BYTE $3C ; <
                .BYTE $30 ; 0
                .BYTE $38 ; 8
                .BYTE $33 ; 3
                .BYTE $2E ; .
                .BYTE $2C ; ,
                .BYTE $1C
random_enemy_tab:.BYTE $2C, $A, $C, $38, $D, $16, 7, $10, $A, $39, $1E
                .BYTE $2C, $24, $40, $2E, $E, 8, $1B, $12, $22, $1E, $E
                .BYTE $F, $2E, $17, 8, $1B, 9, $3A, $11, $D, $C, $1E, $12
                .BYTE $24, $2F, $F, $1C, 9, $41, $2E, $1D, $E, 8, $3A
                .BYTE $40, $1E, $12, $2C, $3A, $16, $2F, $11, $1D, $42
                .BYTE $D, $3A, $1E, $1B, $2F, $43, $22, $24, $1E

; =============== S U B R O U T I N E =======================================

; Y: a random index in the table. Called from type 64

get_random_enemy:                       ; CODE XREF: j_get_random_enemy↑j
                LDA     random_enemy_tab,Y ; 0...63
                RTS
; End of function get_random_enemy


; =============== S U B R O U T I N E =======================================


sub_D69F:                               ; CODE XREF: sub_D6AF+1E↓j
                                        ; sub_D6AF+2C↓j
                LDA     byte_90
                ORA     #$80
                STA     byte_90
                JSR     sub_C6B5
                LDA     byte_90
                AND     #$7F
                STA     byte_90
                RTS
; End of function sub_D69F


; =============== S U B R O U T I N E =======================================


sub_D6AF:                               ; CODE XREF: sub_D753+E↓p
                                        ; sub_D7E3+4↓p
                LDA     byte_A9
                SEC
                SBC     byte_AA
                STA     byte_A9
                ROL
                EOR     #1
                ROR
                LDA     #0
                ADC     byte_AB
                STA     byte_AE
                LDA     byte_98
                SEC
                SBC     byte_AE
                PHP
                CMP     byte_98
                BEQ     loc_D6DD
                PLP
                STA     byte_98
                BCS     sub_D69F
                LDA     byte_99
                EOR     #1
                STA     byte_99
                LDA     byte_98
                SBC     #$F
                STA     byte_98
                BCS     sub_D69F

loc_D6DD:                               ; CODE XREF: sub_D6AF+19↑j
                PLP
                RTS
; End of function sub_D6AF


; =============== S U B R O U T I N E =======================================


sub_D6DF:                               ; CODE XREF: sub_CC57+50↑p
                LDA     #0
                STA     byte_AA
                STA     byte_AB
                STA     byte_9C
                STA     byte_90
                STA     byte_B2
                JSR     sub_C6AB
                LDA     #$FF
                STA     byte_A2
                STA     byte_A3
                STA     byte_A8
                STA     byte_B2
                LDA     #$F
                STA     byte_A4
                LDA     #$D
                STA     byte_A1
                LDA     #1
                STA     byte_B7
                LDX     #8

loc_D706:                               ; CODE XREF: sub_D6DF+33↓j
                LDA     #$80
                STA     $3FF,X
                LDA     $D74A,X         ; tab_D74B - 1
                STA     $16D,X
                DEX
                BNE     loc_D706

loc_D714:                               ; CODE XREF: sub_D8BF+B↓p
                LDA     last_bank_num
                PHA
                JSR     switch_bank_5
                LDY     #0
                LDA     ($18),Y
                STA     byte_B3
                INY
                LDA     ($18),Y
                STA     byte_B4
                LDA     byte_18
                CLC
                ADC     #2
                STA     byte_B5
                LDA     byte_19
                ADC     #0
                STA     byte_B6
                LDA     byte_B3
                SEC
                SBC     #1
                STA     byte_A2
                LDA     byte_B4
                SBC     #0
                STA     byte_A3
                LDA     #0
                STA     byte_AC
                LDA     #2
                STA     byte_AD
                PLA
                JMP     switch_bank     ; A: bank number
; End of function sub_D6DF

; ---------------------------------------------------------------------------
tab_D74B:       .BYTE   0
                .BYTE $55 ; U
                .BYTE $AA
                .BYTE $FF
                .BYTE $AA
                .BYTE $FF
                .BYTE $AA
                .BYTE $FF

; =============== S U B R O U T I N E =======================================


sub_D753:                               ; CODE XREF: sub_CC57+53↑j
                                        ; sub_D753+21↓j ...
                LDA     byte_AA
                PHA
                LDA     byte_AB
                PHA
                LDA     #0
                STA     byte_AA
                LDA     #8
                STA     byte_AB
                JSR     sub_D6AF
                PLA
                STA     byte_AB
                PLA
                STA     byte_AA
                JSR     sub_D824
                JSR     sub_D77B
                LDA     byte_A1
                CMP     #$B
                BNE     sub_D753
                LDA     byte_B7
                BNE     sub_D753

locret_D77A:                            ; CODE XREF: sub_D77B+6↓j
                                        ; sub_D77B+C↓j
                RTS
; End of function sub_D753


; =============== S U B R O U T I N E =======================================


sub_D77B:                               ; CODE XREF: sub_D753+1A↑p
                                        ; sub_D77B+11↓j ...
                LDA     byte_9C
                BNE     loc_D789
                LDA     byte_20
                BEQ     locret_D77A
                LDA     byte_21
                CMP     #$10
                BCC     locret_D77A

loc_D789:                               ; CODE XREF: sub_D77B+2↑j
                JSR     sub_D78F
                JMP     sub_D77B
; End of function sub_D77B


; =============== S U B R O U T I N E =======================================


sub_D78F:                               ; CODE XREF: sub_D77B:loc_D789↑p
                LDA     last_ppu_ctrl2
                AND     #$18
                BNE     locret_D7A4
                LDA     last_ppu_ctrl1
                AND     #$7F
                STA     PPU_CTRL_REG1
                JSR     ppu_update_background ; Large procedure dealing with updating the Name Table/Attr Table to display a portion of the current game level
                LDA     last_ppu_ctrl1
                STA     PPU_CTRL_REG1

locret_D7A4:                            ; CODE XREF: sub_D78F+4↑j
                RTS
; End of function sub_D78F


; =============== S U B R O U T I N E =======================================

; Called after changing the BG scroll. Apparently it sets the Nametable address to update the new row of meta-tiles (16px)

set_nametable_addr_after_scroll:        ; CODE XREF: sub_D824:loc_D83D↓p
                LDA     byte_A1
                ASL
                TAY
                AND     #$1C
                ASL
                STA     byte_9D
                ADC     #$C0
                STA     byte_9E
                LDA     nt_address_tab,Y
                STA     byte_9F
                LDA     nt_address_tab+1,Y
                STA     byte_A0
                RTS
; End of function set_nametable_addr_after_scroll

; ---------------------------------------------------------------------------
nt_address_tab: .WORD $2000
                .WORD $2040
                .WORD $2080
                .WORD $20C0
                .WORD $2100
                .WORD $2140
                .WORD $2180
                .WORD $21C0
                .WORD $2200
                .WORD $2240
                .WORD $2280
                .WORD $22C0
                .WORD $2300
                .WORD $2340
                .WORD $2380
byte_D7DB:      .BYTE $FC
                .BYTE $F3
                .BYTE $CF
                .BYTE $3F ; ?
byte_D7DF:      .BYTE 3
                .BYTE  $C
                .BYTE $30
                .BYTE $C0

; =============== S U B R O U T I N E =======================================


sub_D7E3:                               ; CODE XREF: sub_D0BF:loc_D0E8↑p
                                        ; sub_E81E:loc_E880↓p ...
                LDA     #0
                STA     byte_90
                JSR     sub_D6AF
                LDA     byte_C0
                AND     #$20 ; ' '
                BNE     loc_D7F6
                LDA     byte_C0
                AND     #3
                BNE     sub_D824

loc_D7F6:                               ; CODE XREF: sub_D7E3+B↑j
                LDA     byte_AB
                CMP     byte_AD
                BCC     loc_D80A
                BEQ     loc_D800
                BCS     loc_D818

loc_D800:                               ; CODE XREF: sub_D7E3+19↑j
                LDA     byte_AA
                CMP     byte_AC
                BCC     loc_D80A
                BEQ     sub_D824
                BCS     loc_D818

loc_D80A:                               ; CODE XREF: sub_D7E3+17↑j
                                        ; sub_D7E3+21↑j
                LDA     byte_AA
                ADC     #2
                STA     byte_AA
                LDA     byte_AB
                ADC     #0
                STA     byte_AB
                BCC     sub_D824

loc_D818:                               ; CODE XREF: sub_D7E3+1B↑j
                                        ; sub_D7E3+25↑j
                LDA     byte_AA
                SBC     #2
                STA     byte_AA
                LDA     byte_AB
                SBC     #0
                STA     byte_AB
; End of function sub_D7E3


; =============== S U B R O U T I N E =======================================


sub_D824:                               ; CODE XREF: sub_D753+17↑p
                                        ; sub_D7E3+11↑j ...
                LDA     byte_A4
                CLC
                ADC     byte_AE
                STA     byte_A4
                CMP     #$10
                BCS     loc_D830
                RTS
; ---------------------------------------------------------------------------

loc_D830:                               ; CODE XREF: sub_D824+9↑j
                SBC     #$10
                STA     byte_A4
                JSR     sub_D77B
                INC     byte_A2
                BNE     loc_D83D
                INC     byte_A3

loc_D83D:                               ; CODE XREF: sub_D824+15↑j
                JSR     set_nametable_addr_after_scroll ; Called after changing the BG scroll. Apparently it sets the Nametable address to update the new row of meta-tiles (16px)
                LDA     byte_B7
                BEQ     sub_D84A
                LDA     byte_A0
                ORA     #8
                STA     byte_A0
; End of function sub_D824


; =============== S U B R O U T I N E =======================================


sub_D84A:                               ; CODE XREF: sub_D824+1E↑j
                                        ; sub_D8BF+1A↓j
                JSR     switch_bank_5
                LDY     #0
; End of function sub_D84A


; =============== S U B R O U T I N E =======================================


sub_D84F:                               ; CODE XREF: sub_DA28+B↓j
                LDA     byte_A2
                CMP     byte_B3
                BNE     loc_D85B
                LDA     byte_A3
                CMP     byte_B4
                BEQ     loc_D86C

loc_D85B:                               ; CODE XREF: sub_D84F+4↑j
                TYA
                BEQ     loc_D869
                CLC
                ADC     byte_B5
                STA     byte_B5
                LDA     byte_B6
                ADC     #0
                STA     byte_B6

loc_D869:                               ; CODE XREF: sub_D84F+D↑j
                JMP     sub_DA36
; ---------------------------------------------------------------------------

loc_D86C:                               ; CODE XREF: sub_D84F+A↑j
                LDA     ($B5),Y
                INY
                STY     byte_16
                JSR     switch_case     ; Old-school Switch-Case. Right after the call there is a jump table of variable size.
; End of function sub_D84F

; ---------------------------------------------------------------------------
                .WORD $D885             ; sub_D886 - 1
                .WORD $D893             ; sub_D894 - 1
                .WORD $D8A0             ; sub_D8A1 - 1
                .WORD $D8BE             ; sub_D8BF - 1
                .WORD $D8DB             ; sub_D8DC - 1
                .WORD $D91F             ; sub_D920 - 1
                .WORD $D937             ; sub_D938 - 1
                .WORD $D9DF             ; sub_D9E0 - 1
                .WORD $DA15             ; sub_DA16 - 1

; =============== S U B R O U T I N E =======================================


sub_D886:
                LDY     byte_16
                LDA     ($B5),Y
                STA     byte_AC
                INY
                LDA     ($B5),Y
                STA     byte_AD
                JMP     sub_DA28
; End of function sub_D886


; =============== S U B R O U T I N E =======================================


sub_D894:
                LDY     byte_16
                LDA     ($B5),Y
                STA     byte_A8
                LDA     #0
                STA     byte_B2
                JMP     sub_DA28
; End of function sub_D894


; =============== S U B R O U T I N E =======================================


sub_D8A1:
                LDY     byte_16
                LDA     ($B5),Y
                STA     byte_B0
                INY
                LDA     ($B5),Y
                STA     byte_B1
                INY
                LDA     ($B5),Y
                STA     byte_6C
                INY
                LDA     ($B5),Y
                STA     byte_6D
                LDA     #0
                STA     byte_AF
                STA     byte_6E
                JMP     sub_DA28
; End of function sub_D8A1


; =============== S U B R O U T I N E =======================================


sub_D8BF:
                LDY     byte_16
                LDA     ($B5),Y
                STA     byte_18
                INY
                LDA     ($B5),Y
                STA     byte_19
                JSR     loc_D714
                LDA     byte_B3
                STA     byte_A2
                LDA     byte_B4
                STA     byte_A3
                LDA     #7
                STA     byte_66
                JMP     sub_D84A
; End of function sub_D8BF


; =============== S U B R O U T I N E =======================================


sub_D8DC:
                LDY     byte_16
                LDA     ($B5),Y
                STA     byte_18
                INY
                LDA     ($B5),Y
                STA     byte_19
                INY
                LDA     ($B5),Y
                STY     byte_16
                PHA
                LDX     #0
                LDY     #$18
                LSR
                BCS     loc_D8FA
                LDX     #$C
                LDY     #$C
                BNE     loc_D8FF

loc_D8FA:                               ; CODE XREF: sub_D8DC+16↑j
                LSR
                BCS     loc_D8FF
                LDY     #$C

loc_D8FF:                               ; CODE XREF: sub_D8DC+1C↑j
                                        ; sub_D8DC+1F↑j
                STY     byte_15
                LDY     #0
                LDA     ($18),Y
                STA     cram_new_palette ; Reserved 0x19 bytes for new palette (CRAM)
                INY

loc_D909:                               ; CODE XREF: sub_D8DC+36↓j
                LDA     ($18),Y
                STA     $146,X
                INY
                INX
                DEC     byte_15
                BNE     loc_D909
                PLA
                ORA     cram_update_mode
                STA     cram_update_mode
                LDY     byte_16
                JMP     sub_DA28
; End of function sub_D8DC


; =============== S U B R O U T I N E =======================================


sub_D920:
                LDY     byte_16
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$20 ; ' '
                BNE     loc_D934
                LDA     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                STA     byte_92
                INC     byte_92
                LDA     game_mode       ; 0x80 = demo mode
                ORA     #$10
                STA     game_mode       ; 0x80 = demo mode

loc_D934:                               ; CODE XREF: sub_D920+6↑j
                DEY
                JMP     sub_DA28
; End of function sub_D920


; =============== S U B R O U T I N E =======================================


sub_D938:
                LDY     byte_16
; End of function sub_D938


; =============== S U B R O U T I N E =======================================


sub_D93A:                               ; CODE XREF: sub_D9E0+2B↓j
                LDA     ($B5),Y
                BNE     loc_D941
                JMP     sub_DA28
; ---------------------------------------------------------------------------

loc_D941:                               ; CODE XREF: sub_D93A+2↑j
                STA     byte_14
; End of function sub_D93A


; =============== S U B R O U T I N E =======================================


sub_D943:                               ; CODE XREF: sub_D943+9A↓j
                INY
                LDA     ($B5),Y
                STA     byte_15
                INY
                AND     #7
                TAX
                BIT     byte_15
                BPL     loc_D957
                LDA     #0
                STA     $430,X
                BEQ     loc_D966

loc_D957:                               ; CODE XREF: sub_D943+B↑j
                LDA     ($B5),Y
                INY
                STA     $430,X
                LDA     byte_15
                AND     #$20 ; ' '
                BNE     loc_D966
                LDA     ($B5),Y
                INY

loc_D966:                               ; CODE XREF: sub_D943+12↑j
                                        ; sub_D943+1E↑j
                STA     $408,X
                BIT     byte_15
                BVC     loc_D971
                LDA     #0
                BEQ     loc_D974

loc_D971:                               ; CODE XREF: sub_D943+28↑j
                LDA     ($B5),Y
                INY

loc_D974:                               ; CODE XREF: sub_D943+2C↑j
                STA     $438,X
                LDA     byte_15
                AND     #$20 ; ' '
                BEQ     loc_D981
                LDA     #1
                BNE     loc_D984

loc_D981:                               ; CODE XREF: sub_D943+38↑j
                LDA     ($B5),Y
                INY

loc_D984:                               ; CODE XREF: sub_D943+3C↑j
                STA     $410,X
                LDA     ($B5),Y
                STA     $400,X
                INY
                LDA     byte_15
                AND     #$10
                BEQ     loc_D9AD
                LDA     ($B5),Y
                STA     $440,X
                INY
                LDA     ($B5),Y
                STA     $448,X
                INY
                LDA     ($B5),Y
                STA     $418,X
                INY
                LDA     ($B5),Y
                STA     $420,X
                JMP     loc_D9D6
; ---------------------------------------------------------------------------

loc_D9AD:                               ; CODE XREF: sub_D943+4E↑j
                LDA     ($B5),Y
                STA     byte_18
                INY
                LDA     ($B5),Y
                STA     byte_19
                STY     byte_16
                LDY     #0
                LDA     ($18),Y
                STA     $418,X
                INY
                LDA     ($18),Y
                STA     $420,X
                LDA     byte_18
                CLC
                ADC     #2
                STA     $440,X
                LDA     byte_19
                ADC     #0
                STA     $448,X
                LDY     byte_16

loc_D9D6:                               ; CODE XREF: sub_D943+67↑j
                JSR     sub_DCCB
                DEC     byte_14
                BEQ     sub_DA28
                JMP     sub_D943
; End of function sub_D943


; =============== S U B R O U T I N E =======================================


sub_D9E0:
                LDY     byte_16
                LDA     #$3C ; '<'
                STA     byte_C4
                LDA     ($B5),Y
                STA     byte_C5
                BEQ     loc_DA0E
                INY
                LDA     ($B5),Y
                STA     byte_C6
                INY
                LDA     ($B5),Y
                STA     byte_C8
                INY
                LDA     ($B5),Y
                STA     byte_C7
                INY
                LDA     ($B5),Y
                STA     byte_C3
                LDA     #1
                STA     byte_C0
                LDA     #0
                STA     byte_C1
                STA     byte_C2
                INY
                JMP     sub_D93A
; ---------------------------------------------------------------------------

loc_DA0E:                               ; CODE XREF: sub_D9E0+A↑j
                LDA     byte_C0
                EOR     #$22 ; '"'
                STA     byte_C0
                BNE     sub_DA28
; End of function sub_D9E0


; =============== S U B R O U T I N E =======================================


sub_DA16:
                LDY     byte_16

loc_DA18:                               ; CODE XREF: sub_DA16+10↓j
                LDA     ($B5),Y
                TAX
                CMP     #8
                BCS     sub_DA28
                INY
                LDA     ($B5),Y
                STA     $16E,X
                INY
                BNE     loc_DA18
; End of function sub_DA16


; =============== S U B R O U T I N E =======================================


sub_DA28:                               ; CODE XREF: sub_D886+B↑j
                                        ; sub_D894+A↑j ...
                INY
                LDA     ($B5),Y
                STA     byte_B3
                INY
                LDA     ($B5),Y
                STA     byte_B4
                INY
                JMP     sub_D84F
; End of function sub_DA28


; =============== S U B R O U T I N E =======================================


sub_DA36:                               ; CODE XREF: sub_D84F:loc_D869↑j
                LDA     byte_A8
                ASL
                ADC     byte_A8
                TAY
                LDA     byte_B8
                STA     byte_10
                LDA     byte_B9
                STA     byte_11
                LDA     ($10),Y
                STA     byte_18
                INY
                LDA     ($10),Y
                STA     byte_19
                INY
                LDA     byte_B2
                CMP     ($10),Y
                BCC     loc_DA58
                LDA     #0
                STA     byte_B2

loc_DA58:                               ; CODE XREF: sub_DA36+1C↑j
                INC     byte_B2
                ASL
                ASL
                ASL
                ASL
                TAY
                LDX     #0

loc_DA61:                               ; CODE XREF: sub_DA36+34↓j
                LDA     ($18),Y
                STA     $189,X
                INY
                INX
                CPX     #$10
                BNE     loc_DA61
                LDX     #0
                STX     byte_15

loc_DA70:                               ; CODE XREF: sub_DA36+CD↓j
                LDA     $400,X
                CMP     #$80
                BNE     loc_DA7A
                JMP     loc_DAFB
; ---------------------------------------------------------------------------

loc_DA7A:                               ; CODE XREF: sub_DA36+3F↑j
                LDA     $430,X
                BEQ     loc_DA85
                DEC     $430,X
                JMP     loc_DAFB
; ---------------------------------------------------------------------------

loc_DA85:                               ; CODE XREF: sub_DA36+47↑j
                LDA     $450,X
                STA     byte_18
                LDA     $458,X
                STA     byte_19
                LDY     #0
                LDA     $420,X
                STA     byte_14
                LDA     $400,X
                TAX

loc_DA9A:                               ; CODE XREF: sub_DA36+6F↓j
                                        ; sub_DA36+88↓j
                LDA     ($18),Y
                BNE     loc_DAA8
                INY
                TXA
                CLC
                ADC     ($18),Y
                TAX
                INY
                JMP     loc_DA9A
; ---------------------------------------------------------------------------

loc_DAA8:                               ; CODE XREF: sub_DA36+66↑j
                INY
                CMP     #1
                BEQ     loc_DAC0
                CPX     #$10
                BCS     loc_DABB
                STA     $189,X
                CMP     #$1B
                BCS     loc_DABB
                JSR     sub_DD26

loc_DABB:                               ; CODE XREF: sub_DA36+79↑j
                                        ; sub_DA36+80↑j
                INX
                DEC     byte_14
                BNE     loc_DA9A

loc_DAC0:                               ; CODE XREF: sub_DA36+75↑j
                LDX     byte_15
                TYA
                CLC
                ADC     byte_18
                STA     $450,X
                LDA     byte_19
                ADC     #0
                STA     $458,X
                DEC     $428,X
                BNE     loc_DAFB
                LDA     $410,X
                BEQ     loc_DAE6
                DEC     $410,X
                BNE     loc_DAE6

loc_DADF:                               ; CODE XREF: sub_DA36+C0↓j
                LDA     #$80
                STA     $400,X
                BNE     loc_DAFB

loc_DAE6:                               ; CODE XREF: sub_DA36+A2↑j
                                        ; sub_DA36+A7↑j
                LDA     $400,X
                CLC
                ADC     $438,X
                STA     $400,X
                CMP     #$20 ; ' '
                BCC     loc_DAF8
                CMP     #$F0
                BCC     loc_DADF

loc_DAF8:                               ; CODE XREF: sub_DA36+BC↑j
                JSR     sub_DCC5

loc_DAFB:                               ; CODE XREF: sub_DA36+41↑j
                                        ; sub_DA36+4C↑j ...
                INC     byte_15
                LDX     byte_15
                CPX     #8
                BEQ     loc_DB06
                JMP     loc_DA70
; ---------------------------------------------------------------------------

loc_DB06:                               ; CODE XREF: sub_DA36+CB↑j
                LDX     #0
                STX     byte_14

loc_DB0A:                               ; CODE XREF: sub_DA36+144↓j
                LDA     #0
                STA     byte_19
                LDA     $189,X
                STA     byte_10
                ASL
                ROL     byte_19
                ASL
                ROL     byte_19
                ADC     byte_10
                BCC     loc_DB1F
                INC     byte_19

loc_DB1F:                               ; CODE XREF: sub_DA36+E5↑j
                CLC
                ADC     byte_BA
                STA     byte_18
                LDA     byte_BB
                ADC     byte_19
                STA     byte_19
                LDY     #0
                TXA
                ASL
                TAX
                LDA     ($18),Y
                STA     $470,X
                INY
                LDA     ($18),Y
                STA     $471,X
                INY
                LDA     ($18),Y
                STA     $490,X
                INY
                LDA     ($18),Y
                STA     $491,X
                INY
                LDA     ($18),Y
                STA     byte_10
                LDA     byte_14
                AND     #1
                TAY
                LDA     byte_A1
                AND     #1
                BEQ     loc_DB58
                INY
                INY

loc_DB58:                               ; CODE XREF: sub_DA36+11E↑j
                LDA     byte_14
                LSR
                CLC
                ADC     byte_9D
                TAX
                LDA     $104,X
                AND     byte_D7DB,Y
                STA     byte_11
                LDA     byte_D7DF,Y
                LDY     byte_10
                AND     $16E,Y
                ORA     byte_11
                STA     $104,X
                INC     byte_14
                LDX     byte_14
                CPX     #$10
                BNE     loc_DB0A
                LDA     byte_A1
                ASL
                ASL
                ASL
                STA     byte_18
                LDY     #0

loc_DB85:                               ; CODE XREF: sub_DA36+172↓j
                LDX     $189,Y
                LDA     unk_DBC5,X
                ASL
                ASL
                ASL
                ASL
                STA     byte_10
                INY
                LDX     $189,Y
                LDA     unk_DBC5,X
                ORA     byte_10
                STY     byte_19
                LDY     byte_18
                STA     $4B0,Y
                LDY     byte_19
                INC     byte_18
                INY
                CPY     #$10
                BNE     loc_DB85
                LDA     #1
                STA     byte_9C
                DEC     byte_A1
                BPL     loc_DBBC
                LDA     #$E
                STA     byte_A1
                LDA     byte_B7
                EOR     #1
                STA     byte_B7

loc_DBBC:                               ; CODE XREF: sub_DA36+17A↑j
                LDA     byte_90
                ORA     #1
                STA     byte_90
                JMP     switch_bank_6
; End of function sub_DA36

; ---------------------------------------------------------------------------
unk_DBC5:       .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   1
                .BYTE   2
                .BYTE   2
                .BYTE   2
                .BYTE   2
                .BYTE   3
                .BYTE   3
                .BYTE   0
                .BYTE   0
                .BYTE  $F
                .BYTE   4
                .BYTE   5
                .BYTE   5
                .BYTE   4
                .BYTE   3
                .BYTE   8
unk_DBD8:       .BYTE  $B
                .BYTE  $E
                .BYTE   2
                .BYTE   2
                .BYTE   3
                .BYTE   3
                .BYTE   3
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
                .BYTE  $C
                .BYTE  $D
                .BYTE  $D
                .BYTE  $C
                .BYTE  $C
                .BYTE  $D
                .BYTE  $D
                .BYTE  $C
                .BYTE  $C
                .BYTE  $D
                .BYTE  $D
                .BYTE  $C
                .BYTE  $C
                .BYTE  $D
                .BYTE  $D
                .BYTE  $C
                .BYTE   0
                .BYTE   0
                .BYTE  $B
                .BYTE   0
                .BYTE  $B
                .BYTE   9
                .BYTE  $B
                .BYTE   9
                .BYTE   9
                .BYTE  $F
                .BYTE  $A
                .BYTE   8
                .BYTE  $A
                .BYTE   8
                .BYTE  $F
                .BYTE  $F
                .BYTE   4
                .BYTE   5
                .BYTE   6
                .BYTE   7
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $A
                .BYTE   9
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE   6
                .BYTE   7
                .BYTE   7
                .BYTE   6
                .BYTE   0
                .BYTE  $A
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
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
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   0

; =============== S U B R O U T I N E =======================================


sub_DCC5:                               ; CODE XREF: sub_DA36:loc_DAF8↑p
                LDA     $408,X
                STA     $430,X
; End of function sub_DCC5


; =============== S U B R O U T I N E =======================================


sub_DCCB:                               ; CODE XREF: sub_D943:loc_D9D6↑p
                LDA     $440,X
                STA     $450,X
                LDA     $448,X
                STA     $458,X
                LDA     $418,X
                STA     $428,X
                RTS
; End of function sub_DCCB

; ---------------------------------------------------------------------------
                .BYTE $49 ; I
                .BYTE $4A ; J
                .BYTE $4B ; K
                .BYTE $4C ; L
                .BYTE $4D ; M
                .BYTE $4E ; N
                .BYTE $54 ; T
                .BYTE $54 ; T
                .BYTE $47 ; G
                .BYTE $46 ; F
                .BYTE $36 ; 6
                .BYTE $57 ; W
                .BYTE $57 ; W
                .BYTE $57 ; W
                .BYTE $57 ; W
                .BYTE $54 ; T
                .BYTE $56 ; V
                .BYTE $55 ; U
                .BYTE $4B ; K
                .BYTE $58 ; X
                .BYTE $35 ; 5
                .BYTE $59 ; Y
                .BYTE $54 ; T
                .BYTE $52 ; R
                .BYTE   8
                .BYTE   8
                .BYTE   4
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE  $C
                .BYTE  $C
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   4
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   4
                .BYTE   8
                .BYTE   4
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8
                .BYTE   8

; =============== S U B R O U T I N E =======================================


sub_DD26:                               ; CODE XREF: sub_DA36+82↑p
                STA     byte_1A
                STY     byte_17
                TAY
                LDA     $DCDC,Y
                PHA
                STX     byte_16
                JSR     sub_DD9E
                TYA
                TAX
                PLA
                BCC     loc_DD46
                CMP     #$47 ; 'G'
                BEQ     loc_DD41
                CMP     #$52 ; 'R'
                BNE     loc_DD43

loc_DD41:                               ; CODE XREF: sub_DD26+15↑j
                INC     byte_AF

loc_DD43:                               ; CODE XREF: sub_DD26+19↑j
                JMP     loc_DD99
; ---------------------------------------------------------------------------

loc_DD46:                               ; CODE XREF: sub_DD26+11↑j
                STA     $528,X
                CMP     #$47 ; 'G'
                BEQ     loc_DD51
                CMP     #$52 ; 'R'
                BNE     loc_DD5B

loc_DD51:                               ; CODE XREF: sub_DD26+25↑j
                LDY     byte_AF
                LDA     ($B0),Y
                STA     $716,X
                INY
                STY     byte_AF

loc_DD5B:                               ; CODE XREF: sub_DD26+29↑j
                LDA     byte_A1
                STA     $62C,X
                LDY     byte_1A
                LDA     byte_16
                ASL
                PHA
                CLC
                ADC     byte_9F
                STA     $5F8,X
                LDA     byte_A0
                STA     $612,X
                LDA     #$E0
                CLC
                ADC     $DCF4,Y
                CLC
                ADC     byte_A4
                SEC
                SBC     byte_AE
                STA     $542,X
                PLA
                ASL
                ASL
                ASL
                CLC
                ADC     $DD0C,Y
                STA     $55C,X
                LDA     byte_16
                LSR
                CLC
                ADC     byte_9D
                STA     $646,X
                LDA     byte_1A
                STA     $74A,X

loc_DD99:                               ; CODE XREF: sub_DD26:loc_DD43↑j
                LDY     byte_17
                LDX     byte_16
                RTS
; End of function sub_DD26


; =============== S U B R O U T I N E =======================================


sub_DD9E:                               ; CODE XREF: sub_C027↑j
                                        ; sub_D56B+13↑p ...
                LDY     #$19

loc_DDA0:                               ; CODE XREF: sub_DD9E+A↓j
                LDA     $528,Y
                BEQ     loc_DDF1
                DEY
                CPY     #4
                BNE     loc_DDA0

loc_DDAA:                               ; CODE XREF: sub_DD9E+2C↓j
                INY
                LDA     $528,Y
                AND     #$7F
                CMP     #$14
                BEQ     loc_DDE8
                CMP     #$15
                BEQ     loc_DDE8
                CMP     #$25 ; '%'
                BEQ     loc_DDE8
                CMP     #$26 ; '&'
                BEQ     loc_DDE8
                CMP     #$3B ; ';'
                BEQ     loc_DDE8
                CMP     #$29 ; ')'
                BEQ     loc_DDE8
                CPY     #$19
                BNE     loc_DDAA

loc_DDCC:                               ; CODE XREF: sub_DD9E+46↓j
                LDA     $528,Y
                AND     #$7F
                CMP     #$46 ; 'F'
                BCS     loc_DDE1
                CMP     #$38 ; '8'
                BCS     loc_DDE8
                CMP     #$35 ; '5'
                BCS     loc_DDE6
                CMP     #$27 ; '''
                BNE     loc_DDE8

loc_DDE1:                               ; CODE XREF: sub_DD9E+35↑j
                DEY
                CPY     #4
                BNE     loc_DDCC

loc_DDE6:                               ; CODE XREF: sub_DD9E+3D↑j
                SEC
                RTS
; ---------------------------------------------------------------------------

loc_DDE8:                               ; CODE XREF: sub_DD9E+14↑j
                                        ; sub_DD9E+18↑j ...
                TXA
                PHA
                TYA
                TAX
                JSR     enemy_erase_2   ; Clearing a game object (X). So far, the array assignments are unknown. The same procedure is found in bank 6.
                PLA
                TAX

loc_DDF1:                               ; CODE XREF: sub_DD9E+5↑j
                CLC
                RTS
; End of function sub_DD9E


; =============== S U B R O U T I N E =======================================


sub_DDF3:                               ; CODE XREF: sub_C05D↑j
                                        ; show_main_menu+A7↓p ...
                JSR     sub_DDF9
                JMP     ppu_enable_all
; End of function sub_DDF3


; =============== S U B R O U T I N E =======================================


sub_DDF9:                               ; CODE XREF: sub_DDF3↑p
                                        ; sub_E81E+20↓p
                LDA     cram_update_mode
                PHA
                LDA     #0
                STA     cram_update_mode
                JSR     ppu_nmi_enable
                LDY     #1
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                PLA
                STA     cram_update_mode

loc_DE0E:                               ; CODE XREF: sub_DDF9+18↓j
                LDA     cram_update_mode
                BNE     loc_DE0E
                RTS
; End of function sub_DDF9


; =============== S U B R O U T I N E =======================================


sub_DE14:                               ; CODE XREF: sub_C084↑j
                JSR     switch_bank_5
                LDA     $8056
                STA     byte_10
                LDA     $8057
                STA     byte_11
                LDY     #0

loc_DE23:                               ; CODE XREF: sub_DE14+30↓j
                TYA
                LSR
                STA     byte_13
                LDA     ($10),Y
                INY
                TAX
                LDA     unk_DBC5,X
                ASL
                ASL
                ASL
                ASL
                STA     byte_12
                LDA     ($10),Y
                INY
                TAX
                LDA     unk_DBC5,X
                ORA     byte_12
                LDX     byte_13
                STA     $4B0,X
                CPY     #240
                BNE     loc_DE23
                LDA     $8058
                STA     byte_10
                LDA     $8059
                STA     byte_11
                LDY     #0

loc_DE52:                               ; CODE XREF: sub_DE14+46↓j
                LDA     ($10),Y
                STA     $104,Y
                INY
                CPY     #64
                BNE     loc_DE52
                JMP     switch_bank_6
; End of function sub_DE14


; =============== S U B R O U T I N E =======================================


sub_DE5F:                               ; CODE XREF: sub_C087↑j
                JSR     switch_bank_5
                LDA     $805A
                STA     byte_10
                LDA     $805B
                STA     byte_11
                LDY     #0

loc_DE6E:                               ; CODE XREF: sub_DE5F+30↓j
                TYA
                LSR
                STA     byte_13
                LDA     ($10),Y
                INY
                TAX
                LDA     unk_DBC5,X
                ASL
                ASL
                ASL
                ASL
                STA     byte_12
                LDA     ($10),Y
                INY
                TAX
                LDA     unk_DBC5,X
                ORA     byte_12
                LDX     byte_13
                STA     $4B0,X
                CPY     #$70 ; 'p'
                BNE     loc_DE6E
; End of function sub_DE5F


; =============== S U B R O U T I N E =======================================


sub_DE91:                               ; CODE XREF: sub_C08D↑j
                JSR     switch_bank_5
                LDA     #$23 ; '#'
                JSR     sub_C7EB
                JSR     sub_C7F8
                TAX
                STX     byte_15
                LDA     #$20 ; ' '
                STA     $460,X
                LDA     #$10
                STA     byte_19
                LDA     byte_6F
                ASL
                ASL
                ASL
                STA     byte_70
                ASL
                ASL
                ROL     byte_19
                STA     $462,X
                LDA     byte_19
                STA     $461,X
                LDA     $805A
                STA     byte_18
                LDA     $805B
                STA     byte_19
                LDA     byte_70
                AND     #$F0
                STA     byte_70
                TAY

loc_DECC:                               ; CODE XREF: sub_DE91+5A↓j
                LDA     ($18),Y
                JSR     sub_DF48
                LDA     byte_6F
                AND     #1
                ASL
                TAY
                LDA     ($14),Y
                STA     $463,X
                INY
                INX
                LDA     ($14),Y
                STA     $463,X
                INX
                INC     byte_70
                LDA     byte_70
                TAY
                AND     #$F
                BNE     loc_DECC
                JSR     sub_C807
                JMP     switch_bank_6
; End of function sub_DE91


; =============== S U B R O U T I N E =======================================


sub_DEF3:                               ; CODE XREF: sub_C08A↑j
                JSR     switch_bank_5
                LDA     #$21 ; '!'
                JSR     sub_C7EB
                JSR     sub_C7F8
                TAX
                LDA     #$9E
                STA     $460,X
                LDA     byte_6F
                STA     $462,X
                LDA     #$20 ; ' '
                STA     $461,X
                LDA     byte_6F
                LSR
                STA     byte_70
                TAY
                LDA     $8056
                STA     byte_18
                LDA     $8057
                STA     byte_19

loc_DF1E:                               ; CODE XREF: sub_DEF3+4D↓j
                LDA     ($18),Y
                JSR     sub_DF48
                LDA     byte_6F
                AND     #1
                TAY
                LDA     ($14),Y
                STA     $463,X
                INX
                INY
                INY
                LDA     ($14),Y
                STA     $463,X
                INX
                LDA     byte_70
                CLC
                ADC     #$10
                STA     byte_70
                TAY
                CMP     #$F0
                BCC     loc_DF1E
                JSR     sub_C807
                JMP     switch_bank_6
; End of function sub_DEF3


; =============== S U B R O U T I N E =======================================


sub_DF48:                               ; CODE XREF: sub_DE91+3D↑p
                                        ; sub_DEF3+2D↑p
                STA     byte_10
                LDA     #0
                STA     byte_11
                LDA     byte_10
                ASL
                ROL     byte_11
                ASL
                ROL     byte_11
                ADC     byte_10
                BCC     loc_DF5C
                INC     byte_11

loc_DF5C:                               ; CODE XREF: sub_DF48+10↑j
                CLC
                ADC     byte_BA
                STA     byte_14
                LDA     byte_11
                ADC     byte_BB
                STA     byte_15
                RTS
; End of function sub_DF48


; =============== S U B R O U T I N E =======================================

; During Hard Reset, reset current Score. Also increase Reset Counter

reset_score:                            ; CODE XREF: reset+2D↓p
                LDA     hard_reset_marker1
                CMP     #$35 ; '5'
                BNE     loc_DF7A
                LDA     hard_reset_marker2
                CMP     #$53 ; 'S'
                BNE     loc_DF7A
                INC     reset_counter
                RTS
; ---------------------------------------------------------------------------

loc_DF7A:                               ; CODE XREF: reset_score+5↑j
                                        ; reset_score+C↑j
                LDX     #6

loc_DF7C:                               ; CODE XREF: reset_score+1B↓j
                LDA     unk_DF9F,X
                STA     $182,X
                DEX
                BPL     loc_DF7C
                LDA     #1
                STA     byte_7D
                LDA     #0
                STA     reset_counter
                STA     byte_199
                STA     byte_19B
                LDA     #$35 ; '5'
                STA     hard_reset_marker1
                LDA     #$53 ; 'S'
                STA     hard_reset_marker2
                RTS
; End of function reset_score

; ---------------------------------------------------------------------------
unk_DF9F:       .BYTE   0
                .BYTE   0
                .BYTE   0
                .BYTE   1
                .BYTE   0
                .BYTE   0
                .BYTE   0
pal_DFA6:       .BYTE  $F               ; Palette for the main menu
                .BYTE $37 ; 7
                .BYTE $27 ; '
                .BYTE $18
                .BYTE $35 ; 5
                .BYTE $25 ; %
                .BYTE $15
                .BYTE $30 ; 0
                .BYTE $10
                .BYTE   0
                .BYTE $31 ; 1
                .BYTE $21 ; !
                .BYTE $11
                .BYTE $16
                .BYTE $30 ; 0
                .BYTE $12
                .BYTE $30 ; 0
                .BYTE $37 ; 7
                .BYTE $16
                .BYTE $30 ; 0
                .BYTE $22 ; "
                .BYTE  $F
                .BYTE $30 ; 0
                .BYTE $28 ; (
                .BYTE  $F

; =============== S U B R O U T I N E =======================================

; Display the title (main menu), no game mode selection yet

show_main_menu:                         ; CODE XREF: main_menu_loop+9↓p
                JSR     apu_stop
                JSR     ppu_disable_all
                LDA     #0
                STA     byte_9C
                STA     byte_20
                LDA     #$A6
                STA     byte_18
                LDA     #$DF            ; $DFA6
                STA     byte_19
                JSR     ppu_cram_load   ; Update Palette (CRAM). The Ptr variable ($18) contains the address of the palette. The palette is copied to $145. The “update palette” flag is set (checked in nmi_handler)
                JSR     clear_oam_buffer
                LDA     #0
                STA     byte_19
                STA     byte_18
                LDA     #0
                LDY     #4
                JSR     sub_C8DF
                LDA     #$19
                STA     byte_19
                LDA     #$F0
                STA     byte_18
                LDA     #3
                LDY     #4
                JSR     sub_C8DF
                LDA     #$12
                STA     byte_19
                LDA     #0
                STA     byte_18
                LDA     #2
                LDY     #4
                JSR     sub_C8DF
                JSR     sub_C8BA
                LDA     #$20 ; ' '
                LDX     #$20 ; ' '
                LDY     #$AA
                JSR     clear_screen    ; Clear BG
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                LDA     pad_buttons_1
                ORA     pad_buttons_2
                AND     #$C0
                CMP     #$C0
                BNE     loc_E020
                JMP     sound_test_menu
; ---------------------------------------------------------------------------

loc_E020:                               ; CODE XREF: show_main_menu+5C↑j
                LDA     #$20 ; ' '
                STA     byte_19
                LDA     #0
                STA     byte_18
                LDA     #$90
                STA     byte_16
                LDA     #$E2
                STA     byte_17
                JSR     sub_E3F4
                LDY     #0
                JSR     sub_E399
                LDA     #$20 ; ' '
                STA     PPU_ADDRESS
                LDA     #$68 ; 'h'
                STA     PPU_ADDRESS
                LDY     #$7A ; 'z'
                JSR     sub_E266
                LDA     #$20 ; ' '
                STA     PPU_ADDRESS
                LDA     #$74 ; 't'
                STA     PPU_ADDRESS
                LDY     #$82
                JSR     sub_E266
                LDA     #$20 ; ' '
                STA     byte_19
                LDA     #$88
                STA     byte_18
                LDA     byte_7D
                JSR     sub_D347
                JSR     sub_C6AB
                JSR     sub_DDF3
                JSR     sub_18006
                LDA     #3
                JSR     apu_play        ; A = sound num (1...35)
                LDA     #$B
                STA     byte_66
                LDA     #4
                STA     demo_counter_2
                LDA     #0
                STA     byte_300
                STA     byte_6F

loc_E080:                               ; CODE XREF: show_main_menu+DE↓j
                                        ; show_main_menu+E7↓j
                LDY     #1
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                JSR     sub_E229
                JSR     sub_18009
                JSR     sub_18003
                JSR     sub_1800C
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                LDA     pad_test1
                AND     #$10
                BNE     loc_E0A9
                LDA     byte_300
                BEQ     loc_E080
                LDA     #0
                STA     byte_300
                DEC     demo_counter_2
                BNE     loc_E080
                RTS
; ---------------------------------------------------------------------------

loc_E0A9:                               ; CODE XREF: show_main_menu+D9↑j
                JSR     clear_oam_buffer
                LDA     #$13
                JSR     sub_C7EB
                JSR     sub_C7F8
                TAX
                LDA     #$10
                STA     $460,X
                LDA     #$23 ; '#'
                STA     $461,X
                LDA     #$E0
                STA     $462,X
                LDY     #$10
                LDA     #$AA

loc_E0C8:                               ; CODE XREF: show_main_menu+10E↓j
                STA     $463,X
                INX
                DEY
                BNE     loc_E0C8
                JSR     sub_C807
                LDA     #$21 ; '!'
                STA     byte_6F
                LDA     #$A2
                STA     byte_70

loc_E0DA:                               ; CODE XREF: show_main_menu+160↓j
                LDA     #$10
                JSR     sub_C7EB
                JSR     sub_C7F8
                TAX
                LDA     #$8D
                STA     $460,X
                LDA     byte_6F
                STA     $461,X
                LDA     byte_70
                STA     $462,X
                LDY     #0

loc_E0F4:                               ; CODE XREF: show_main_menu+155↓j
                LDA     byte_70
                CMP     #$BD
                BCC     loc_E100
                LDA     unk_E21C,Y
                JMP     loc_E10D
; ---------------------------------------------------------------------------

loc_E100:                               ; CODE XREF: show_main_menu+139↑j
                CMP     #$A2
                BNE     loc_E10A
                LDA     unk_E202,Y
                JMP     loc_E10D
; ---------------------------------------------------------------------------

loc_E10A:                               ; CODE XREF: show_main_menu+143↑j
                LDA     unk_E20F,Y

loc_E10D:                               ; CODE XREF: show_main_menu+13E↑j
                                        ; show_main_menu+148↑j
                STA     $463,X
                INX
                INY
                CPY     #$D
                BNE     loc_E0F4
                JSR     sub_C807
                INC     byte_70
                LDA     byte_70
                CMP     #$BE
                BNE     loc_E0DA
                LDA     #$21 ; '!'
                STA     byte_6F
                LDA     #$E8
                STA     byte_70

loc_E129:                               ; CODE XREF: show_main_menu+196↓j
                LDA     #$B
                JSR     sub_C7EB
                JSR     sub_C7F8
                TAX
                LDA     #$88
                STA     $460,X
                LDA     byte_6F
                STA     $461,X
                LDA     byte_70
                STA     $462,X
                LDY     #8
                LDA     #$20 ; ' '

loc_E145:                               ; CODE XREF: show_main_menu+18B↓j
                STA     $463,X
                INX
                DEY
                BNE     loc_E145
                JSR     sub_C807
                INC     byte_70
                LDA     byte_70
                CMP     #$F7
                BNE     loc_E129
                JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
; ---------------------------------------------------------------------------
                .BYTE  $A
                .BYTE $22 ; "
                .BYTE $2B ; +
aGameStart:     .BYTE "GAME START"
; ---------------------------------------------------------------------------
                JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
; ---------------------------------------------------------------------------
                .BYTE   8
                .BYTE $22 ; "
                .BYTE $8B
aContinue:      .BYTE "CONTINUE"
; ---------------------------------------------------------------------------
                LDA     #$C6
                STA     byte_201
                LDA     #0
                STA     byte_202
                LDA     #$4C ; 'L'
                STA     byte_203

loc_E184:                               ; CODE XREF: show_main_menu+1FA↓j
                LDA     #$82
                LDY     byte_199
                BEQ     loc_E18D
                LDA     #$9A

loc_E18D:                               ; CODE XREF: show_main_menu+1CA↑j
                STA     byte_200

loc_E190:                               ; CODE XREF: show_main_menu+20F↓j
                                        ; show_main_menu+217↓j ...
                LDY     #1
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                LDA     pad_test1
                ORA     pad_test2
                TAY
                AND     #$10
                BEQ     loc_E1A2
                RTS
; ---------------------------------------------------------------------------

loc_E1A2:                               ; CODE XREF: show_main_menu+1E0↑j
                TYA
                AND     #$2C ; ','
                BEQ     loc_E1C9
                LDA     #$15
                JSR     apu_play        ; A = sound num (1...35)
                TYA
                AND     #$20 ; ' '
                BEQ     loc_E1BC
                LDA     byte_199
                EOR     #1

loc_E1B6:                               ; CODE XREF: show_main_menu+204↓j
                                        ; show_main_menu+208↓j
                STA     byte_199
                JMP     loc_E184
; ---------------------------------------------------------------------------

loc_E1BC:                               ; CODE XREF: show_main_menu+1F0↑j
                TYA
                AND     #8
                BEQ     loc_E1C5
                LDA     #0
                BEQ     loc_E1B6

loc_E1C5:                               ; CODE XREF: show_main_menu+200↑j
                LDA     #1
                BNE     loc_E1B6

loc_E1C9:                               ; CODE XREF: show_main_menu+1E6↑j
                LDA     reset_counter
                CMP     #$D
                BNE     loc_E190
                LDA     pad_test1
                ORA     pad_test2
                AND     #3
                BEQ     loc_E190
                AND     #2
                BNE     loc_E1E6
                DEC     byte_7D
                BNE     loc_E1F2
                LDA     #$C
                STA     byte_7D
                BNE     loc_E1F2

loc_E1E6:                               ; CODE XREF: show_main_menu+21B↑j
                INC     byte_7D
                LDA     byte_7D
                CMP     #$D
                BNE     loc_E1F2
                LDA     #1
                STA     byte_7D

loc_E1F2:                               ; CODE XREF: show_main_menu+21F↑j
                                        ; show_main_menu+225↑j ...
                LDA     #$20 ; ' '
                STA     byte_19
                LDA     #$88
                STA     byte_18
                LDA     byte_7D
                JSR     sub_D347
                JMP     loc_E190
; End of function show_main_menu

; ---------------------------------------------------------------------------
unk_E202:       .BYTE $B0
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B6
                .BYTE $B3
unk_E20F:       .BYTE $B1
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $DF
                .BYTE $B4
unk_E21C:       .BYTE $B2
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B7
                .BYTE $B5

; =============== S U B R O U T I N E =======================================


sub_E229:                               ; CODE XREF: show_main_menu+C6↑p
                LDA     reset_counter
                CMP     #9
                BNE     locret_E253
                JSR     sub_E76C
                LDY     byte_6F
                LDX     unk_E254,Y
                LDA     0,X
                AND     unk_E25D,Y
                BEQ     locret_E253
                INC     byte_6F
                CPY     #8
                BNE     locret_E253
                LDA     #$FF
                STA     byte_19B
                LDA     #0
                STA     byte_6F
                LDA     #9
                JSR     apu_play        ; A = sound num (1...35)

locret_E253:                            ; CODE XREF: sub_E229+5↑j
                                        ; sub_E229+14↑j ...
                RTS
; End of function sub_E229

; ---------------------------------------------------------------------------
unk_E254:       .BYTE   7
                .BYTE   3
                .BYTE   6
                .BYTE   5
                .BYTE   6
                .BYTE   5
                .BYTE   6
                .BYTE   2
                .BYTE   8
unk_E25D:       .BYTE   2
                .BYTE $20
                .BYTE   2
                .BYTE $10
                .BYTE $40 ; @
                .BYTE   2
                .BYTE   4
                .BYTE $40 ; @
                .BYTE   2

; =============== S U B R O U T I N E =======================================


sub_E266:                               ; CODE XREF: show_main_menu+85↑p
                                        ; show_main_menu+94↑p ...
                LDA     #7
                STA     byte_10
                LDA     #0
                STA     byte_11

loc_E26E:                               ; CODE XREF: sub_E266+22↓j
                LDA     byte_11
                BNE     loc_E27D
                LDA     $100,Y
                STA     byte_11
                BNE     loc_E27D
                LDA     #$20 ; ' '
                BNE     loc_E282

loc_E27D:                               ; CODE XREF: sub_E266+A↑j
                                        ; sub_E266+11↑j
                LDA     $100,Y
                ORA     #$30 ; '0'

loc_E282:                               ; CODE XREF: sub_E266+15↑j
                STA     PPU_DATA
                INY
                DEC     byte_10
                BNE     loc_E26E
                LDA     #$30 ; '0'
                STA     PPU_DATA
                RTS
; End of function sub_E266

; ---------------------------------------------------------------------------
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE   3
                .BYTE $11
aScoreTop:      .BYTE "score         top"
                .BYTE $FF
                .BYTE   3
                .BYTE   4
aArea:          .BYTE "area"
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE   8
                .BYTE  $F
                .BYTE $90
                .BYTE $91
                .BYTE $92
                .BYTE $93
                .BYTE $96
                .BYTE $A0
                .BYTE $82
                .BYTE $83
                .BYTE $85
                .BYTE $8F
                .BYTE $A0
                .BYTE $87
                .BYTE $9A
                .BYTE $8A
                .BYTE $8B
                .BYTE $FF
                .BYTE   9
                .BYTE  $B
                .BYTE $94
                .BYTE $95
                .BYTE $96
                .BYTE $9B
                .BYTE $9C
                .BYTE $82
                .BYTE $84
                .BYTE $86
                .BYTE $9B
                .BYTE $9C
                .BYTE $82
                .BYTE $FF
                .BYTE   8
                .BYTE  $F
                .BYTE $94
                .BYTE $95
                .BYTE $97
                .BYTE $98
                .BYTE $9D
                .BYTE $9E
                .BYTE $81
                .BYTE $80
                .BYTE $98
                .BYTE $9D
                .BYTE $9E
                .BYTE $88
                .BYTE $89
                .BYTE $8C
                .BYTE $8D
                .BYTE $FF
                .BYTE   8
                .BYTE  $F
                .BYTE $99
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $9A
                .BYTE $8E
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE  $D
                .BYTE   4
aAI:            .BYTE "a.i."
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE   3
                .BYTE $18
aGameDesindedBy:.BYTE "game desinded by COMPILE"
                .BYTE $FF
                .BYTE   3
                .BYTE $14
aProducedByAii: .BYTE "produced      by aii"
                .BYTE $FF
                .BYTE   3
                .BYTE $1A
aPresentedByPon:.BYTE "presented     by pony inc"
                .BYTE $9F
                .BYTE $FF
                .BYTE   3
                .BYTE $1A
aCopyright1986P:.BYTE "copyright @1986  pony inc"
                .BYTE $9F
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE $FF
                .BYTE   9
                .BYTE  $B
aLicensedBy:    .BYTE "licensed by"
                .BYTE $FF
                .BYTE   4
                .BYTE $18
aNintendoOfAmer:.BYTE "nintendo OF america inc."
                .BYTE $FF
                .BYTE $FE

; =============== S U B R O U T I N E =======================================


sub_E399:                               ; CODE XREF: show_main_menu+76↑p
                                        ; sub_E399+17↓j ...
                LDA     byte_E3B8,Y
                BEQ     locret_E3B7
                LDA     #$23 ; '#'
                STA     PPU_ADDRESS
                LDA     byte_E3B8,Y
                STA     PPU_ADDRESS
                INY

loc_E3AA:                               ; CODE XREF: sub_E399+1C↓j
                LDA     byte_E3B8,Y
                INY
                CMP     #$80
                BEQ     sub_E399
                STA     PPU_DATA
                BNE     loc_E3AA

locret_E3B7:                            ; CODE XREF: sub_E399+3↑j
                RTS
; End of function sub_E399

; ---------------------------------------------------------------------------
byte_E3B8:      .BYTE $C2, 0, 0, $AA, 0, 0, $80, $CA, 0, $80, $D2, $FF
                .BYTE $FF, $FF, $FF, $80, $E5, $A5, $A5, $80, $EB, $A0
                .BYTE $A0, $80, 0
tab_E3D1:       .BYTE $CA, $5A, $52, $50, $50, $80, $D3, $A, $A, $80, $D8
                .BYTE 0, 0, 0, $CC, $FF, $FF, $FF, $FF, 0, 0, 0, $FF, $FF
                .BYTE $FF, $80, $E8, $FF, $FF, $FF, $FF, $50, $50, $80
                .BYTE 0

; =============== S U B R O U T I N E =======================================


sub_E3F4:                               ; CODE XREF: show_main_menu+71↑p
                                        ; sub_E3F4+42↓j
                LDY     #0

loc_E3F6:                               ; CODE XREF: sub_E3F4+24↓j
                LDA     ($16),Y
                INY
                CMP     #$FE
                BCS     loc_E41A
                CLC
                ADC     byte_18
                TAX
                LDA     byte_19
                ADC     #0
                STA     PPU_ADDRESS
                STX     PPU_ADDRESS
                LDA     ($16),Y
                INY
                TAX

loc_E40F:                               ; CODE XREF: sub_E3F4+22↓j
                LDA     ($16),Y
                INY
                STA     PPU_DATA
                DEX
                BNE     loc_E40F
                BEQ     loc_E3F6

loc_E41A:                               ; CODE XREF: sub_E3F4+7↑j
                CMP     #$FE
                BEQ     locret_E438
                TYA
                CLC
                ADC     byte_16
                STA     byte_16
                LDA     byte_17
                ADC     #0
                STA     byte_17
                LDA     byte_18
                ADC     #$20 ; ' '
                STA     byte_18
                LDA     byte_19
                ADC     #0
                STA     byte_19
                BCC     sub_E3F4

locret_E438:                            ; CODE XREF: sub_E3F4+28↑j
                RTS
; End of function sub_E3F4


; =============== S U B R O U T I N E =======================================


sound_test_menu:                        ; CODE XREF: show_main_menu+5E↑j
                                        ; sound_test_menu+9↓j
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                LDA     pad_buttons_1
                ORA     pad_buttons_2
                AND     #$C0
                BNE     sound_test_menu
                JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
; ---------------------------------------------------------------------------
                .BYTE  $A
                .BYTE $20
                .BYTE $88
aSoundTest:     .BYTE "SOUND TEST"
; ---------------------------------------------------------------------------
                LDA     #1
                STA     byte_6F
                LDA     #1
                STA     byte_70
                JSR     sub_C6AB
                JSR     sub_DDF3
; End of function sound_test_menu


; =============== S U B R O U T I N E =======================================


sub_E462:                               ; CODE XREF: sub_E462+51↓j
                                        ; sub_E462+57↓j ...
                LDA     #$20 ; ' '
                STA     byte_19
                LDA     #$CD
                STA     byte_18
                LDA     byte_6F
                JSR     sub_D347

loc_E46F:                               ; CODE XREF: sub_E462+1D↓j
                                        ; sub_E462+2F↓j ...
                LDY     #1
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                LDA     pad_test1
                ORA     pad_test2
                BNE     loc_E487
                DEC     byte_70
                BNE     loc_E46F
                LDA     #2
                STA     byte_70
                BNE     loc_E48B

loc_E487:                               ; CODE XREF: sub_E462+19↑j
                LDA     #$F
                STA     byte_70

loc_E48B:                               ; CODE XREF: sub_E462+23↑j
                LDA     pad_buttons_1
                ORA     pad_buttons_2
                STA     pad_buttons_1
                BEQ     loc_E46F
                AND     #$C0
                BEQ     loc_E4A2
                JSR     apu_stop
                LDA     byte_6F
                JSR     apu_play        ; A = sound num (1...35)
                JMP     loc_E46F
; ---------------------------------------------------------------------------

loc_E4A2:                               ; CODE XREF: sub_E462+33↑j
                LDA     pad_buttons_1
                AND     #$F
                BEQ     loc_E46F
                JSR     apu_stop
                LDA     pad_buttons_1
                AND     #2
                BEQ     loc_E4BB
                DEC     byte_6F
                BNE     sub_E462
                LDA     #$23 ; '#'
                STA     byte_6F
                BNE     sub_E462

loc_E4BB:                               ; CODE XREF: sub_E462+4D↑j
                LDA     pad_buttons_1
                AND     #1
                BEQ     loc_E46F
                INC     byte_6F
                LDA     byte_6F
                CMP     #$24 ; '$'
                BCC     sub_E462
                LDA     #1
                STA     byte_6F
                BNE     sub_E462
; End of function sub_E462


; =============== S U B R O U T I N E =======================================


show_ending_screen:                     ; CODE XREF: level_completed_sequence+50↓j
                LDY     #$78 ; 'x'
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                JSR     ppu_disable_all
                LDA     #$C
                STA     byte_7D
                JSR     set_new_high_score
                JSR     clear_oam_buffer
                LDA     #$80
                STA     byte_210
                STA     byte_213
                STA     byte_72
                STA     byte_73
                LDA     #$C
                STA     byte_74
                LDA     #$FF
                STA     byte_75
                JSR     ppu_nmi_enable
                LDY     #1
                JSR     sub_E60F
                JSR     ppu_disable_all
                LDA     #$12
                STA     byte_19
                LDA     #0
                STA     byte_18
                LDA     #2
                LDY     #4
                JSR     sub_C8DF
                JSR     sub_C8BA
                LDA     #$20 ; ' '
                LDX     #$20 ; ' '
                LDY     #$AA
                JSR     clear_screen    ; Clear BG
                JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
; ---------------------------------------------------------------------------
                .BYTE   5
                .BYTE $20
                .BYTE $89
aScore:         .BYTE "SCORE"
; ---------------------------------------------------------------------------
                LDA     #$20 ; ' '
                STA     PPU_ADDRESS
                LDA     #$8E
                STA     PPU_ADDRESS
                LDY     #$7A ; 'z'
                JSR     sub_E266
                LDY     #$19
                JSR     sub_E399
                LDA     #$A6
                STA     byte_18
                LDA     #$DF
                STA     byte_19
                JSR     ppu_cram_load   ; Update Palette (CRAM). The Ptr variable ($18) contains the address of the palette. The palette is copied to $145. The “update palette” flag is set (checked in nmi_handler)
                LDA     #10             ; Play Ending music
                JSR     apu_play        ; A = sound num (1...35)
                JSR     sub_C6AB
                JSR     sub_DDF3
                LDX     #0
                LDA     #6
                STA     byte_6F

loc_E556:                               ; CODE XREF: show_ending_screen+FE↓j
                                        ; show_ending_screen+134↓j
                LDA     congratulation_text,X
                BEQ     loc_E5C5
                INX
                STA     byte_70

loc_E55E:                               ; CODE XREF: show_ending_screen+CB↓j
                                        ; show_ending_screen+F4↓j
                LDA     byte_70
                ASL
                ASL
                ASL
                SBC     #3
                STA     byte_73

loc_E567:                               ; CODE XREF: show_ending_screen+9A↓j
                LDA     byte_20
                BNE     loc_E567
                LDA     byte_6F
                ASL
                ASL
                ASL
                PHA
                SEC
                SBC     #4
                STA     byte_72
                JSR     loc_E61E
                LDA     #8
                STA     byte_11
                PLA
                ASL
                ROL     byte_11
                ASL
                ROL     byte_11
                ADC     byte_70
                STA     byte_10
                LDA     $E671,X
                BEQ     loc_E5C5
                CMP     #$FF
                BEQ     loc_E5CF
                INX
                CMP     #$A
                BCS     loc_E59D
                ADC     byte_70
                STA     byte_70
                JMP     loc_E55E
; ---------------------------------------------------------------------------

loc_E59D:                               ; CODE XREF: show_ending_screen+C5↑j
                PHA
                LDA     byte_10
                PHA
                LDA     byte_11
                PHA
                LDA     #4
                JSR     sub_C7EB
                JSR     sub_C7F8
                TAY
                LDA     #1
                STA     $460,Y
                PLA
                STA     $461,Y
                PLA
                STA     $462,Y
                PLA
                STA     $463,Y
                JSR     sub_C807
                INC     byte_70
                BNE     loc_E55E

loc_E5C5:                               ; CODE XREF: show_ending_screen+8A↑j
                                        ; show_ending_screen+BC↑j
                INX
                LDY     #$14
                JSR     sub_E60F
                INC     byte_6F
                BNE     loc_E556

loc_E5CF:                               ; CODE XREF: show_ending_screen+C0↑j
                                        ; show_ending_screen+13B↓j
                LDY     #1
                JSR     sub_E60F
                LDA     byte_74
                BNE     loc_E5EA
                LDA     byte_75
                CMP     #$FE
                BCC     loc_E5EA
                LDA     #$FE
                STA     byte_75
                LDA     #$AC
                STA     byte_72
                LDA     #$C8
                STA     byte_73

loc_E5EA:                               ; CODE XREF: show_ending_screen+107↑j
                                        ; show_ending_screen+10D↑j
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                LDA     byte_75
                CMP     #$FE
                BNE     loc_E606
                LDA     pad_buttons_1
                AND     pad_buttons_2
                AND     #$C0
                CMP     #$C0
                BNE     loc_E606
                LDA     #$FD
                STA     byte_75
                LDX     #$D8
                JMP     loc_E556
; ---------------------------------------------------------------------------

loc_E606:                               ; CODE XREF: show_ending_screen+122↑j
                                        ; show_ending_screen+12C↑j
                LDA     pad_test1
                AND     #$10
                BEQ     loc_E5CF
                JMP     main_menu_loop
; End of function show_ending_screen


; =============== S U B R O U T I N E =======================================


sub_E60F:                               ; CODE XREF: show_ending_screen+2B↑p
                                        ; show_ending_screen+F9↑p ...
                TYA
                PHA
                JSR     sub_E653
                LDY     #1
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                PLA
                TAY
                DEY
                BNE     sub_E60F

loc_E61E:                               ; CODE XREF: show_ending_screen+A7↑p
                                        ; sub_E60F+3C↓j ...
                JSR     sub_E653
                LDY     #1
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                LDA     byte_210
                CMP     byte_72
                PHP
                BEQ     loc_E637
                CLC
                ADC     #1
                STA     byte_210
                STA     byte_214

loc_E637:                               ; CODE XREF: sub_E60F+1D↑j
                LDA     byte_213
                CMP     byte_73
                BEQ     loc_E64E
                CLC
                ADC     #2
                STA     byte_213
                CLC
                ADC     #8
                STA     byte_217
                PLP
                JMP     loc_E61E
; ---------------------------------------------------------------------------

loc_E64E:                               ; CODE XREF: sub_E60F+2D↑j
                PLP
                BEQ     locret_E670
                BNE     loc_E61E
; End of function sub_E60F


; =============== S U B R O U T I N E =======================================


sub_E653:                               ; CODE XREF: sub_E60F+2↑p
                                        ; sub_E60F:loc_E61E↑p
                INC     byte_71
                BNE     loc_E659
                DEC     byte_74

loc_E659:                               ; CODE XREF: sub_E653+2↑j
                LDA     byte_71
                AND     #2
                ASL
                ADC     #$EC
                STA     byte_211
                ADC     #2
                STA     byte_215
                LDA     #1
                STA     byte_212
                STA     byte_216

locret_E670:                            ; CODE XREF: sub_E60F+40↑j
                RTS
; End of function sub_E653

; ---------------------------------------------------------------------------
congratulation_text:.BYTE   8
aCongratulation:.BYTE "CONGRATULATIONS!",0
                .BYTE   0
                .BYTE   2
aYouAreTheMessi:.BYTE "YOU ARE THE MESSIAH OF EARTH.",0
                .BYTE   0
                .BYTE  $D
aStaff:         .BYTE "STAFF",0
                .BYTE   0
                .BYTE   2
aGameDesignJemi:.BYTE "game design JEMINI,JANUS,MOO",0
                .BYTE   2
aProgram:       .BYTE "program"
                .BYTE   5
aJemini:        .BYTE "JEMINI",0
                .BYTE   2
aMusicMiyamoto: .BYTE "music",7,"MIYAMOTO",0
                .BYTE   2
aGraphics:      .BYTE "graphics"
                .BYTE   4
aJanus:         .BYTE "JANUS",0
                .BYTE   2
aTestPlay:      .BYTE "test play"
                .BYTE   3
aMncNui:        .BYTE "MNC NUI",0
                .BYTE  $E
aWaoIsee:       .BYTE "WAO ISEE",0
                .BYTE  $E
aYoriki:        .BYTE "YORIKI",0
                .BYTE   0
                .BYTE   5
aProdusedByAii: .BYTE "prodused by aii",0
                .BYTE   0
                .BYTE   6
aCreatedByCompi:.BYTE "created by COMPILE",0
                .BYTE   0
                .BYTE  $F
                .BYTE $FF
                .BYTE   3
aSeeYouAgainInG:.BYTE "see you again",$5C,"in guardic"
                .BYTE $9F
                .BYTE   0
                .BYTE   0
                .BYTE  $F
                .BYTE $FF

; =============== S U B R O U T I N E =======================================

; Clear byte_FB

sub_E767:                               ; CODE XREF: reset+16↓p
                LDA     #0
                STA     byte_FB
                RTS
; End of function sub_E767


; =============== S U B R O U T I N E =======================================


sub_E76C:                               ; CODE XREF: sub_D3FF↑p
                                        ; sub_E229+7↑p
                LDA     byte_FB
                AND     #$F8
                STA     byte_FB
                ORA     #5
                STA     JOYPAD_PORT1
                NOP
                NOP
                NOP
                NOP
                NOP
                NOP
                LDX     #8

loc_E77F:                               ; CODE XREF: sub_E76C+49↓j
                LDA     byte_FB
                ORA     #4
                STA     JOYPAD_PORT1
                LDY     #$A

loc_E788:                               ; CODE XREF: sub_E76C+1D↓j
                DEY
                BNE     loc_E788
                NOP
                LDY     byte_FB
                LDA     JOYPAD_PORT2
                LSR
                AND     #$F
                BEQ     loc_E7BB
                STA     0,X
                LDA     byte_FB
                ORA     #6
                STA     JOYPAD_PORT1
                LDY     #$A

loc_E7A1:                               ; CODE XREF: sub_E76C+36↓j
                DEY
                BNE     loc_E7A1
                NOP
                NOP
                LDA     JOYPAD_PORT2
                ROL
                ROL
                ROL
                AND     #$F0
                ORA     0,X
                EOR     #$FF
                STA     0,X
                DEX
                BPL     loc_E77F
                LDY     byte_FB
                ORA     #$FF

loc_E7BB:                               ; CODE XREF: sub_E76C+28↑j
                STY     JOYPAD_PORT1
                RTS
; End of function sub_E76C


; =============== S U B R O U T I N E =======================================


reset:                                  ; CODE XREF: j_reset↑j
                                        ; DATA XREF: ROM:FFFC↓o
                LDA     #0
                STA     byte_20
                STA     cram_update_mode
                STA     byte_9C
                LDX     #$FF
                TXS
                LDA     #$30 ; '0'
                JSR     ppu_set_control1
                LDA     #6
                JSR     ppu_set_control2 ; A = new PPU CTRL2 value
                JSR     sub_E767        ; Clear byte_FB
                LDX     #3

loc_E7DA:                               ; CODE XREF: reset+1E↓j
                                        ; reset+21↓j
                LDA     PPU_STATUS
                BPL     loc_E7DA
                DEX
                BNE     loc_E7DA
                LDA     #$C0
                STA     JOYPAD_PORT2
                LDA     #0
                STA     APU_DELTA_REG
                JSR     reset_score     ; During Hard Reset, reset current Score. Also increase Reset Counter
                LDA     byte_7D
                PHA
                LDX     #$A
                LDA     #0

loc_E7F6:                               ; CODE XREF: reset+3C↓j
                STA     0,X
                INX
                CPX     #$F9
                BNE     loc_E7F6
                JSR     sub_CB40        ; Clear array $172...$179
                PLA
                STA     byte_7D
                JSR     apu_reset
; End of function reset


; =============== S U B R O U T I N E =======================================


main_menu_loop:                         ; CODE XREF: handle_pad_buttons+E↑j
                                        ; sub_D0A4+12↑j ...
                LDX     #$FF
                TXS
                JSR     switch_bank_6
                JSR     ppu_disable_all
                JSR     show_main_menu  ; Display the title (main menu), no game mode selection yet
                JSR     switch_bank_6
                JSR     apu_stop
                JSR     ppu_disable_all
                JSR     start_game_check
; End of function main_menu_loop


; =============== S U B R O U T I N E =======================================


sub_E81E:                               ; CODE XREF: level_completed_sequence+59↓j
                JSR     switch_bank_6
                JSR     ppu_disable_all
                JSR     clear_oam_buffer
                JSR     sub_18006
                JSR     j_clear_main_menu_sprites
                JSR     j_update_score_onscreen
                JSR     sub_CC57
                LDY     #7
                LDA     game_mode       ; 0x80 = demo mode
                BPL     loc_E83B
                LDY     #9

loc_E83B:                               ; CODE XREF: sub_E81E+19↑j
                TYA
                STA     byte_66
                JSR     sub_DDF9
                LDA     last_ppu_ctrl2
                ORA     #$10
                JSR     ppu_set_control2 ; A = new PPU CTRL2 value
                LDY     #$28 ; '('

loc_E84A:                               ; CODE XREF: sub_E81E+3F↓j
                TYA
                PHA
                LDY     #1
                JSR     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                JSR     sub_18009
                JSR     sub_18003
                JSR     sub_1800C
                PLA
                TAY
                DEY
                BNE     loc_E84A
                JSR     sub_DDF3
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$80
                BNE     loc_E876
                LDY     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                LDA     level_music_tab,Y
                CPY     #1
                BNE     loc_E873
                LDA     #7

loc_E873:                               ; CODE XREF: sub_E81E+51↑j
                JSR     apu_play        ; A = sound num (1...35)

loc_E876:                               ; CODE XREF: sub_E81E+48↑j
                                        ; sub_E81E+8A↓j
                LDA     #1
                STA     byte_528
                LDA     #0
                STA     byte_5AA

loc_E880:                               ; CODE XREF: sub_E81E+86↓j
                JSR     sub_D7E3
                JSR     sub_1801E
                LDY     #1
                JSR     sub_EA0B
                JSR     sub_D0BF
                JSR     sub_D480
                LDA     byte_528
                CMP     #$81
                BNE     loc_E89E
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$30 ; '0'
                BNE     sub_E8C9

loc_E89E:                               ; CODE XREF: sub_E81E+78↑j
                                        ; sub_E8C9+52↓j
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$BF
                CMP     game_mode       ; 0x80 = demo mode
                BEQ     loc_E880
                STA     game_mode       ; 0x80 = demo mode
                BNE     loc_E876
; End of function sub_E81E


; =============== S U B R O U T I N E =======================================


sub_E8AA:                               ; CODE XREF: level_completed_sequence+43↓p
                LDX     #5
                LDY     #$15

loc_E8AE:                               ; CODE XREF: sub_E8AA+12↓j
                LDA     $528,X
                AND     #$7F
                BEQ     loc_E8BA
                LDA     #$28 ; '('
                STA     $528,X

loc_E8BA:                               ; CODE XREF: sub_E8AA+9↑j
                INX
                DEY
                BNE     loc_E8AE
                LDA     #0
                STA     byte_C0
                LDA     byte_62
                AND     #$F7
                STA     byte_62
                RTS
; End of function sub_E8AA


; =============== S U B R O U T I N E =======================================


sub_E8C9:                               ; CODE XREF: sub_E81E+7E↑j
                CMP     #$10
                BEQ     loc_E917
                LDA     byte_4B
                CLC
                ADC     #$40 ; '@'
                BCC     loc_E8D6
                LDA     #$FF

loc_E8D6:                               ; CODE XREF: sub_E8C9+9↑j
                STA     byte_4B
                LDX     #1

loc_E8DA:                               ; CODE XREF: sub_E8C9+2B↓j
                LDA     $145,X
                AND     #$F
                TAY
                LDA     byte_E907,Y
                STA     byte_10
                LDA     $145,X
                AND     #$30 ; '0'
                EOR     #$30 ; '0'
                ORA     byte_10
                STA     $145,X
                INX
                CPX     #$D
                BNE     loc_E8DA
                LDA     #3
                STA     cram_update_mode
                JSR     apu_stop
                LDA     #11
                JSR     apu_play        ; A = sound num (1...35)
                LDY     #$B3
                BNE     level_completed_sequence ; Perform actions after completing a level (start music, activate Warp, etc.).
; ---------------------------------------------------------------------------
byte_E907:      .BYTE 5, 6, 7, 8, 9, $A, $B, $C, $F, 1, 2, 3, 4, $F, $F
                .BYTE 0
; ---------------------------------------------------------------------------

loc_E917:                               ; CODE XREF: sub_E8C9+2↑j
                LDA     byte_C0
                AND     #3
                BNE     loc_E89E
                LDA     game_mode       ; 0x80 = demo mode
                AND     #8
                BNE     loc_E937
                LDA     #8
                STA     byte_AD
                LDA     #0
                STA     byte_AC
                JSR     sub_F146

loc_E92E:                               ; CODE XREF: sub_E8C9+6C↓j
                JSR     sub_E995
                LDA     byte_AB
                CMP     #4
                BCC     loc_E92E

loc_E937:                               ; CODE XREF: sub_E8C9+58↑j
                LDY     #$FF
; End of function sub_E8C9


; =============== S U B R O U T I N E =======================================

; Perform actions after completing a level (start music, activate Warp, etc.).

level_completed_sequence:               ; CODE XREF: sub_E8C9+3C↑j
                                        ; level_completed_sequence+2C↓j
                TYA
                PHA
                AND     #$1F
                BNE     loc_E953
                CPY     #$C0
                BCS     loc_E953
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$10
                BNE     loc_E950
                CPY     #$61 ; 'a'
                BCS     loc_E950
                JSR     fade_palette_before_warp ; Shade the palette before warp after completing a level

loc_E950:                               ; CODE XREF: level_completed_sequence+E↑j
                                        ; level_completed_sequence+12↑j
                JSR     fade_palette_before_warp_another ; Another option to shade the palette before warp after completing a level

loc_E953:                               ; CODE XREF: level_completed_sequence+4↑j
                                        ; level_completed_sequence+8↑j
                JSR     sub_E995
                PLA
                TAY
                CPY     #$B4
                BNE     loc_E964
                JSR     apu_stop
                LDA     #30             ; Level Complete music
                JSR     apu_play        ; A = sound num (1...35)

loc_E964:                               ; CODE XREF: level_completed_sequence+21↑j
                DEY
                BNE     level_completed_sequence ; Perform actions after completing a level (start music, activate Warp, etc.).
                LDA     warp_sequence_counter
                ORA     #1
                STA     warp_sequence_counter

loc_E96D:                               ; CODE XREF: level_completed_sequence+41↓j
                LDA     #$81
                STA     byte_528
                LDY     #1
                JSR     sub_EA0B
                LDA     byte_528
                BNE     loc_E96D
                JSR     sub_E8AA
                LDA     game_mode       ; 0x80 = demo mode
                AND     #$CF
                STA     game_mode       ; 0x80 = demo mode
                AND     #8
                BEQ     loc_E98C
                JMP     show_ending_screen
; ---------------------------------------------------------------------------

loc_E98C:                               ; CODE XREF: level_completed_sequence+4E↑j
                LDA     byte_92
                STA     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                STA     byte_7D
                JMP     sub_E81E
; End of function level_completed_sequence


; =============== S U B R O U T I N E =======================================


sub_E995:                               ; CODE XREF: sub_E8C9:loc_E92E↑p
                                        ; level_completed_sequence:loc_E953↑p
                LDA     #$81
                STA     byte_528
                JSR     sub_D7E3
                LDY     #1
                JMP     sub_EA0B
; End of function sub_E995


; =============== S U B R O U T I N E =======================================

; Another option to shade the palette before warp after completing a level

fade_palette_before_warp_another:       ; CODE XREF: level_completed_sequence:loc_E950↑p
                LDY     #1
                LDA     #1
                BNE     loc_E9AC
; End of function fade_palette_before_warp_another


; =============== S U B R O U T I N E =======================================

; Shade the palette before warp after completing a level

fade_palette_before_warp:               ; CODE XREF: level_completed_sequence+14↑p
                LDY     #$D
                LDA     #2

loc_E9AC:                               ; CODE XREF: fade_palette_before_warp_another+4↑j
                PHA
                LDA     #$C
                STA     byte_10

loc_E9B1:                               ; CODE XREF: fade_palette_before_warp+1E↓j
                LDA     $145,Y
                CMP     #$10
                BCC     loc_E9BC
                SBC     #$10
                BCS     loc_E9C0

loc_E9BC:                               ; CODE XREF: fade_palette_before_warp+E↑j
                TAX
                LDA     unk_E9D0,X

loc_E9C0:                               ; CODE XREF: fade_palette_before_warp+12↑j
                STA     $145,Y
                INY
                DEC     byte_10
                BNE     loc_E9B1
                PLA
                ORA     cram_update_mode
                STA     cram_update_mode
                RTS
; End of function fade_palette_before_warp

; ---------------------------------------------------------------------------
unk_E9D0:       .BYTE  $C
                .BYTE  $C
                .BYTE   1
                .BYTE   2
                .BYTE  $C
                .BYTE   4
                .BYTE   7
                .BYTE   8
                .BYTE   9
                .BYTE  $C
                .BYTE   9
                .BYTE  $A
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F
                .BYTE  $F

; =============== S U B R O U T I N E =======================================


update_music_after_fortress_destroy:    ; CODE XREF: j_update_music_after_fortress_destroy↑j
                LDA     game_mode       ; 0x80 = demo mode
                BPL     loc_E9E5
                RTS
; ---------------------------------------------------------------------------

loc_E9E5:                               ; CODE XREF: update_music_after_fortress_destroy+2↑j
                LDY     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                LDA     level_music_tab,Y
                CPY     #10             ; Level 10
                BNE     loc_E9FB
                LDA     pad_buttons_1
                AND     #$20 ; ' '      ; Hold SELECT to play secret music (33)
                BNE     loc_E9F9
                LDA     level_music_tab,Y
                BNE     loc_E9FB

loc_E9F9:                               ; CODE XREF: update_music_after_fortress_destroy+12↑j
                LDA     #33

loc_E9FB:                               ; CODE XREF: update_music_after_fortress_destroy+C↑j
                                        ; update_music_after_fortress_destroy+17↑j
                JMP     apu_play        ; A = sound num (1...35)
; End of function update_music_after_fortress_destroy

; ---------------------------------------------------------------------------
level_music_tab:.BYTE   2
                .BYTE   1
                .BYTE $1D
                .BYTE   1
                .BYTE $1D
                .BYTE $20
                .BYTE $1F
                .BYTE $20
                .BYTE $19
                .BYTE $1F
                .BYTE $19
                .BYTE $20
                .BYTE   2

; =============== S U B R O U T I N E =======================================


sub_EA0B:                               ; CODE XREF: sub_C024↑j
                                        ; sub_E81E+6A↑p ...
                TXA
                PHA

loc_EA0D:                               ; CODE XREF: sub_EA0B+26↓j
                TYA
                PHA
                JSR     vsync           ; Wait for the next VBlank to start. Limits the game speed to the TV frame rate
                JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
                LDA     game_mode       ; 0x80 = demo mode
                BPL     loc_EA22
                LDA     pad_test1
                AND     #$10            ; START Button pressed?
                BEQ     loc_EA25
                JMP     main_menu_loop
; ---------------------------------------------------------------------------

loc_EA22:                               ; CODE XREF: sub_EA0B+C↑j
                JSR     sub_D3A8

loc_EA25:                               ; CODE XREF: sub_EA0B+12↑j
                JSR     j_game_logic_processing
                JSR     clear_first_last_sprites ; Clear the last 4 and the first 4 sprites according to the byte_39 variable condition. The game uses these sprites in a special way (how?)
                JSR     sub_D0A4
                PLA
                TAY
                DEY
                BNE     loc_EA0D
                PLA
                TAX
                RTS
; End of function sub_EA0B


; =============== S U B R O U T I N E =======================================

; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)

all_enemies_destroyed:                  ; CODE XREF: j_all_enemies_destroyed↑j
                                        ; sub_DDF9+E↑p ...
                JSR     vsync           ; Wait for the next VBlank to start. Limits the game speed to the TV frame rate
                DEY
                BNE     all_enemies_destroyed ; Called when all enemies are destroyed (with weapon 6 or via Enemy Eraser)
                RTS
; End of function all_enemies_destroyed


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_reset_0:
                JMP     apu_reset
; End of function j_apu_reset_0


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_stop_0:
                JMP     apu_stop
; End of function j_apu_stop_0


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_play_1:
                JMP     apu_play        ; A = sound num (1...35)
; End of function j_apu_play_1


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

j_apu_update_0:
                JMP     apu_update      ; A typical approach for updating the APU state. Called from VBlank handler (every frame)
; End of function j_apu_update_0


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_EA49:
                JMP     sub_F0F4
; End of function sub_EA49


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_EA4C:
                JMP     sub_F0FA
; End of function sub_EA4C


; =============== S U B R O U T I N E =======================================

; Attributes: thunk

sub_EA4F:
                JMP     sub_F146
; End of function sub_EA4F


; =============== S U B R O U T I N E =======================================

; A typical approach for updating the APU state. Called from VBlank handler (every frame)

apu_update:                             ; CODE XREF: j_apu_update↑j
                                        ; nmi_handler+2F↑p ...
                LDA     byte_308
                BNE     locret_EA90
                LDA     byte_302
                AND     #$FE
                CMP     byte_302
                BEQ     loc_EA7A
                ORA     #$80
                STA     byte_302
                LDA     #0
                JSR     sub_EFA6
                LDX     #0

loc_EA6D:                               ; CODE XREF: apu_update+26↓j
                LDA     $3EC,X
                ORA     #$40 ; '@'
                STA     $3EC,X
                INX
                CPX     #6
                BNE     loc_EA6D

loc_EA7A:                               ; CODE XREF: apu_update+D↑j
                LDA     byte_302
                BPL     loc_EA89
                LDX     byte_31F
                BNE     loc_EA91
                AND     #$7F
                STA     byte_302

loc_EA89:                               ; CODE XREF: apu_update+2B↑j
                LDA     byte_302
                AND     #2
                BEQ     loc_EA91

locret_EA90:                            ; CODE XREF: apu_update+3↑j
                RTS
; ---------------------------------------------------------------------------

loc_EA91:                               ; CODE XREF: apu_update+30↑j
                                        ; apu_update+3C↑j
                JSR     sub_EE94
                LDY     #4
                LDA     byte_302
                BPL     loc_EA9C
                INY

loc_EA9C:                               ; CODE XREF: apu_update+47↑j
                STY     byte_303

loc_EA9F:                               ; CODE XREF: apu_update+D0↓j
                LDX     byte_303
                LDA     $31A,X
                BNE     loc_EAAA
                JMP     loc_EB17
; ---------------------------------------------------------------------------

loc_EAAA:                               ; CODE XREF: apu_update+53↑j
                TAY
                LDA     $3EC,X
                AND     #$7F
                STA     $3EC,X
                LDA     $350,X
                AND     #1
                BEQ     loc_EAC6
                LDA     $386,X
                CLC
                ADC     $38C,X
                STA     $38C,X
                BCC     loc_EADF

loc_EAC6:                               ; CODE XREF: apu_update+66↑j
                DEC     $368,X
                BNE     loc_EADF
                LDA     $338,X
                STA     $368,X
                INC     $356,X
                LDA     $356,X
                CMP     $35C,X
                BNE     loc_EADF
                JSR     sub_EB7A

loc_EADF:                               ; CODE XREF: apu_update+72↑j
                                        ; apu_update+77↑j ...
                JSR     sub_EDFC
                JSR     sub_EEAA
                LDA     $350,X
                AND     #$20 ; ' '
                BEQ     loc_EB17
                LDA     $3AA,X
                CLC
                ADC     $3B0,X
                STA     $3B0,X
                BCC     loc_EB17
                LDA     $320,X
                AND     #$F
                CMP     $3B6,X
                BNE     loc_EB0C
                LDA     $350,X
                ORA     #2
                STA     $350,X
                BNE     loc_EB17

loc_EB0C:                               ; CODE XREF: apu_update+AE↑j
                LDA     $350,X
                AND     #$FD
                STA     $350,X
                DEC     $320,X

loc_EB17:                               ; CODE XREF: apu_update+55↑j
                                        ; apu_update+98↑j ...
                LDA     byte_302
                BMI     loc_EB25
                DEX
                STX     byte_303
                BMI     loc_EB25
                JMP     loc_EA9F
; ---------------------------------------------------------------------------

loc_EB25:                               ; CODE XREF: apu_update+C8↑j
                                        ; apu_update+CE↑j
                LDA     byte_307
                STA     APU_MASTERCTRL_REG
                LDX     #$F

loc_EB2D:                               ; CODE XREF: apu_update+103↓j
                LDA     $309,X
                CMP     #$FF
                BCS     loc_EB37
                STA     APU_REG_BASE,X

loc_EB37:                               ; CODE XREF: apu_update+E0↑j
                DEX
                BCS     loc_EB40
                LDA     $309,X
                STA     APU_REG_BASE,X

loc_EB40:                               ; CODE XREF: apu_update+E6↑j
                DEX
                BCS     loc_EB4D
                LDA     $309,X
                BMI     loc_EB4A
                LDA     #8

loc_EB4A:                               ; CODE XREF: apu_update+F4↑j
                STA     APU_REG_BASE,X

loc_EB4D:                               ; CODE XREF: apu_update+EF↑j
                DEX
                LDA     $309,X
                STA     APU_REG_BASE,X
                DEX
                BPL     loc_EB2D
                RTS
; End of function apu_update


; =============== S U B R O U T I N E =======================================


sub_EB58:
                CMP     #8
                BCS     loc_EB71
                RTS
; ---------------------------------------------------------------------------
                LDA     $39E,X
                SEC
                SBC     byte_1E
                STA     $39E,X
                LDA     $3A4,X
                SBC     byte_1F
                STA     $3A4,X
                BCC     loc_EB71
                RTS
; ---------------------------------------------------------------------------

loc_EB71:                               ; CODE XREF: sub_EB58+2↑j
                                        ; sub_EB58+16↑j
                LDA     #0
                STA     $39E,X
                STA     $3A4,X
                RTS
; End of function sub_EB58


; =============== S U B R O U T I N E =======================================


sub_EB7A:                               ; CODE XREF: apu_update+8A↑p
                LDA     $344,X
                STA     byte_1C
                LDA     $34A,X
                STA     byte_1D
                LDY     #$FF
                INY
                LDX     byte_303
                LDA     ($1C),Y
                BMI     loc_EB91
                JMP     loc_ED56
; ---------------------------------------------------------------------------

loc_EB91:                               ; CODE XREF: sub_EB7A+12↑j
                CMP     #$B0
                BCC     loc_EB98
                JMP     apu_fx_26_27
; ---------------------------------------------------------------------------

loc_EB98:                               ; CODE XREF: sub_EB7A+19↑j
                ASL
                TAX
                LDA     #$EB
                PHA
                LDA     #$85
                PHA
                LDA     $EBB0,X
                PHA
                LDA     $EBAF,X
                PHA
                LDX     byte_303
                INY
                LDA     ($1C),Y
                RTS
; End of function sub_EB7A

; ---------------------------------------------------------------------------
word_EBAF:      .WORD $EBEA             ; fx 0
                .WORD $EBF5             ; fx 1
                .WORD $EC0A             ; fx 2
                .WORD $EC16             ; fx 3
                .WORD $EC1E             ; fx 4
                .WORD $EC0F             ; fx 5
                .WORD $EC04             ; fx 6
                .WORD $EC73             ; fx 7
                .WORD $EC1A             ; fx 8
                .WORD $EC34             ; fx 9
                .WORD $EC4E             ; fx 10
                .WORD $EF8F             ; fx 11 = apu_play
                .WORD $ED43             ; fx 12
                .WORD $EC3E             ; fx 13
                .WORD $ECF4             ; fx 14
                .WORD $ECF4             ; fx 15
                .WORD $EC4A             ; fx 16
                .WORD $EC77             ; fx 17
                .WORD $ECB1             ; fx 18
                .WORD $EC7E             ; fx 19
                .WORD $ED14             ; fx 20
                .WORD $ECA3             ; fx 21
                .WORD $ECD6             ; fx 22
                .WORD $ED3B             ; fx 23
                .WORD $ED3B             ; fx 24
                .WORD $ED3B             ; fx 25
                .WORD $ED4F             ; fx 26
                .WORD $ED4F             ; fx 27
                .WORD $ED45             ; fx 28
                .WORD $EC27             ; fx 29

; =============== S U B R O U T I N E =======================================


apu_fx_0:                               ; CODE XREF: apu_fx_1+B↓j
                                        ; apu_fx_21+9↓j
                TAX
                INY
                LDA     ($1C),Y
                STA     byte_1D
                STX     byte_1C
                LDY     #$FF
                RTS
; End of function apu_fx_0


; =============== S U B R O U T I N E =======================================


apu_fx_1:
                CLC
                ADC     byte_303
                TAX
                INY
                LDA     ($1C),Y
                DEC     $31A,X
                BNE     apu_fx_0
                INY
                RTS
; End of function apu_fx_1


; =============== S U B R O U T I N E =======================================


apu_fx_6:
                LDA     $3C2,X
                STA     byte_300
; End of function apu_fx_6


; =============== S U B R O U T I N E =======================================


apu_fx_2:
                LDA     #0
                STA     $31A,X
; End of function apu_fx_2


; =============== S U B R O U T I N E =======================================


apu_fx_5:
                PLA
                PLA
                PLA
                PLA
                JMP     loc_EB17
; End of function apu_fx_5


; =============== S U B R O U T I N E =======================================


apu_fx_3:
                STA     $32C,X
                RTS
; End of function apu_fx_3


; =============== S U B R O U T I N E =======================================


apu_fx_8:
                STA     $326,X
                RTS
; End of function apu_fx_8


; =============== S U B R O U T I N E =======================================


apu_fx_4:
                STA     byte_1E
                LDA     $320,X
                AND     #$F
                BPL     sub_EC2F
; End of function apu_fx_4


; =============== S U B R O U T I N E =======================================


apu_fx_29:
                STA     byte_1E
                LDA     $320,X
                AND     #$F0
; End of function apu_fx_29


; =============== S U B R O U T I N E =======================================


sub_EC2F:                               ; CODE XREF: apu_fx_4+7↑j
                ORA     byte_1E
                STA     $320,X
                RTS
; End of function sub_EC2F


; =============== S U B R O U T I N E =======================================


apu_fx_9:
                BEQ     loc_EC3B
                CLC
                ADC     $332,X

loc_EC3B:                               ; CODE XREF: apu_fx_9↑j
                STA     $332,X
                RTS
; End of function apu_fx_9


; =============== S U B R O U T I N E =======================================


apu_fx_13:
                CLC
                ADC     byte_303
                TAX
                INY
                LDA     ($1C),Y
                STA     $31A,X
                RTS
; End of function apu_fx_13


; =============== S U B R O U T I N E =======================================


apu_fx_16:
                STA     $31A,X
                RTS
; End of function apu_fx_16


; =============== S U B R O U T I N E =======================================


apu_fx_10:
                PHA
                LDA     $320,X
                AND     #$F
                STA     byte_1E
                PLA
                CLC
                BPL     loc_EC63
                ADC     byte_1E
                BCS     sub_EC6B
                LDA     #0
                BEQ     sub_EC6B

loc_EC63:                               ; CODE XREF: apu_fx_10+A↑j
                ADC     byte_1E
                CMP     #$10
                BCC     sub_EC6B
                LDA     #$F
; End of function apu_fx_10


; =============== S U B R O U T I N E =======================================


sub_EC6B:                               ; CODE XREF: apu_fx_10+E↑j
                                        ; apu_fx_10+12↑j ...
                STA     byte_1E
                LDA     $320,X
                AND     #$F0
                ORA     byte_1E
; End of function sub_EC6B


; =============== S U B R O U T I N E =======================================


apu_fx_7:
                STA     $320,X
                RTS
; End of function apu_fx_7


; =============== S U B R O U T I N E =======================================


apu_fx_17:
                ORA     byte_300
                STA     byte_300
                RTS
; End of function apu_fx_17


; =============== S U B R O U T I N E =======================================


apu_fx_19:
                STA     byte_1E
                AND     #$3F ; '?'
                STA     $3B6,X
                LDA     $350,X
                AND     #$CD
                STA     $350,X
                LDA     #0
                STA     $3B0,X
                INY
                LDA     ($1C),Y
                STA     $3AA,X
                BEQ     locret_ECA3
                LDA     $350,X
                ORA     #$20 ; ' '
                STA     $350,X

locret_ECA3:                            ; CODE XREF: apu_fx_19+1A↑j
                RTS
; End of function apu_fx_19


; =============== S U B R O U T I N E =======================================


apu_fx_21:
                LDA     $350,X
                AND     #2
                BNE     sub_ECB0
                LDA     ($1C),Y
                JMP     apu_fx_0
; End of function apu_fx_21


; =============== S U B R O U T I N E =======================================


sub_ECB0:                               ; CODE XREF: apu_fx_21+5↑j
                INY
                RTS
; End of function sub_ECB0


; =============== S U B R O U T I N E =======================================


apu_fx_18:
                CLC
                DEC     $37A,X
                ADC     $37A,X
                INC     $37A,X
                STA     byte_1E
                INY
                LDA     ($1C),Y
                ADC     #0
                STA     byte_1F
                STY     byte_304
                LDY     #0
                LDA     ($1E),Y
                LDY     byte_304
                CLC
                ADC     $332,X
                STA     $332,X
                RTS
; End of function apu_fx_18


; =============== S U B R O U T I N E =======================================


apu_fx_22:
                STA     $386,X
                LDA     #$80
                STA     $38C,X
                LDA     $350,X
                AND     #$FE
                STA     $350,X
                LDA     $386,X
                BEQ     locret_ECF4
                LDA     $350,X
                ORA     #1
                STA     $350,X

locret_ECF4:                            ; CODE XREF: apu_fx_22+13↑j
                RTS
; End of function apu_fx_22


; =============== S U B R O U T I N E =======================================


apu_fx_14_15:
                STA     byte_1E
                ASL
                BEQ     sub_ED10
                LDA     $3CE,X
                PHP
                CLC
                ADC     byte_1E
                AND     #$F
                PLP
                BPL     sub_ED08
                ORA     #$80
; End of function apu_fx_14_15


; =============== S U B R O U T I N E =======================================


sub_ED08:                               ; CODE XREF: apu_fx_14_15+F↑j
                BCC     sub_ED0C
                EOR     #$80
; End of function sub_ED08


; =============== S U B R O U T I N E =======================================


sub_ED0C:                               ; CODE XREF: sub_ED08↑j
                STA     $3CE,X
                RTS
; End of function sub_ED0C


; =============== S U B R O U T I N E =======================================


sub_ED10:                               ; CODE XREF: apu_fx_14_15+3↑j
                LSR
                STA     $3CE,X
                RTS
; End of function sub_ED10


; =============== S U B R O U T I N E =======================================


apu_fx_20:
                BMI     sub_ED25
                BEQ     sub_ED33
                STA     $3BC,X
                LDA     $350,X
                AND     #$BF
                ORA     #$80
                BMI     sub_ED2F
; End of function apu_fx_20


; =============== S U B R O U T I N E =======================================


sub_ED25:                               ; CODE XREF: apu_fx_20↑j
                AND     #$7F
                STA     $3BC,X
                LDA     $350,X
                ORA     #$C0
; End of function sub_ED25


; =============== S U B R O U T I N E =======================================


sub_ED2F:                               ; CODE XREF: apu_fx_20+E↑j
                STA     $350,X
                RTS
; End of function sub_ED2F


; =============== S U B R O U T I N E =======================================


sub_ED33:                               ; CODE XREF: apu_fx_20+2↑j
                LDA     $350,X
                AND     #$7F
                STA     $350,X
                RTS
; End of function sub_ED33


; =============== S U B R O U T I N E =======================================


apu_fx_23_24_25:
                LDA     unk_31A,X
                EOR     #$10
                STA     unk_31A,X
; End of function apu_fx_23_24_25


; =============== S U B R O U T I N E =======================================


apu_fx_12:
                DEY
                RTS
; End of function apu_fx_12


; =============== S U B R O U T I N E =======================================


apu_fx_28:
                LDA     unk_31A,X
                EOR     #4
                STA     unk_31A,X
                DEY
                RTS
; End of function apu_fx_28


; =============== S U B R O U T I N E =======================================


apu_fx_26_27:                           ; CODE XREF: sub_EB7A+1B↑j
                LDA     $362,X
                JMP     loc_ED5A
; ---------------------------------------------------------------------------

loc_ED56:                               ; CODE XREF: sub_EB7A+14↑j
                STA     $362,X
                INY

loc_ED5A:                               ; CODE XREF: apu_fx_26_27+3↑j
                JSR     sub_EDB9
                LDA     ($1C),Y
                CMP     #$E0
                BCS     loc_ED69
                LDA     $36E,X
                JMP     loc_ED7F
; ---------------------------------------------------------------------------

loc_ED69:                               ; CODE XREF: apu_fx_26_27+11↑j
                SEC
                SBC     #$E0
                BNE     loc_ED74
                INY
                LDA     ($1C),Y
                JMP     loc_ED7E
; ---------------------------------------------------------------------------

loc_ED74:                               ; CODE XREF: apu_fx_26_27+1C↑j
                STY     byte_304
                TAY
                LDA     $F16A,Y
                LDY     byte_304

loc_ED7E:                               ; CODE XREF: apu_fx_26_27+21↑j
                INY

loc_ED7F:                               ; CODE XREF: apu_fx_26_27+16↑j
                STA     $35C,X
                STA     $36E,X
                TYA
                CLC
                ADC     byte_1C
                STA     $344,X
                LDA     byte_1D
                ADC     #0
                STA     $34A,X
                LDA     $338,X
                STA     $368,X
                LDA     #0
                STA     $356,X
                STA     $398,X
                LDA     $31A,X
                AND     #$10
                BEQ     loc_EDAD
                LDA     $374,X
                BNE     loc_EDB0

loc_EDAD:                               ; CODE XREF: apu_fx_26_27+56↑j
                STA     $392,X

loc_EDB0:                               ; CODE XREF: apu_fx_26_27+5B↑j
                LDA     $3EC,X
                ORA     #$80
                STA     $3EC,X
                RTS
; End of function apu_fx_26_27


; =============== S U B R O U T I N E =======================================


sub_EDB9:                               ; CODE XREF: apu_fx_26_27:loc_ED5A↑p
                STA     $39E,X
                AND     #$FF
                BNE     loc_EDC4
                STA     $3A4,X
                RTS
; ---------------------------------------------------------------------------

loc_EDC4:                               ; CODE XREF: sub_EDB9+5↑j
                LDA     $33E,X
                CMP     #3
                BNE     loc_EDE0
                LDA     #1
                STA     $3A4,X
                DEC     $39E,X
                LDA     $39E,X
                CMP     #$10
                BCC     locret_EDDF
                ADC     #$6F ; 'o'
                STA     $39E,X

locret_EDDF:                            ; CODE XREF: sub_EDB9+1F↑j
                RTS
; ---------------------------------------------------------------------------

loc_EDE0:                               ; CODE XREF: sub_EDB9+10↑j
                LDA     $39E,X
                CLC
                ADC     $332,X
                STY     byte_304
                ASL
                TAY
                LDA     $F002,Y
                STA     $39E,X
                LDA     $F003,Y
                STA     $3A4,X
                LDY     byte_304
                RTS
; End of function sub_EDB9


; =============== S U B R O U T I N E =======================================


sub_EDFC:                               ; CODE XREF: apu_update:loc_EADF↑p
                LDA     $320,X
                AND     #$10
                BEQ     loc_EE3C
                LDA     $326,X
                BEQ     loc_EE3C
                ASL
                TAY
                LDA     word_F190,Y
                STA     byte_1E
                LDA     word_F190+1,Y   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                STA     byte_1F
                LDY     $392,X
                JSR     sub_EE71
                INY
                STA     byte_1E
                LDA     $320,X
                AND     #$F
                CLC
                ADC     byte_1E
                SBC     #$E
                STA     apu_sound_length
                TYA
                STA     $392,X
                LDA     $320,X
                AND     #$F0
                BCC     loc_EE38
                ORA     apu_sound_length

loc_EE38:                               ; CODE XREF: sub_EDFC+37↑j
                STA     $374,X
                RTS
; ---------------------------------------------------------------------------

loc_EE3C:                               ; CODE XREF: sub_EDFC+5↑j
                                        ; sub_EDFC+A↑j ...
                LDA     $320,X
                STA     $374,X
                RTS
; End of function sub_EDFC


; =============== S U B R O U T I N E =======================================


sub_EE43:
                LDA     $326,X
                BEQ     loc_EE3C
                ASL
                TAY
                LDA     word_F190,Y
                STA     byte_1E
                LDA     word_F190+1,Y   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
                STA     byte_1F
                LDY     $392,X
                JSR     sub_EE71
                INY
                STA     byte_1E
                LDA     $320,X
                CLC
                ADC     byte_1E
                SBC     #$1F
                BCS     loc_EE69
                LDA     #0

loc_EE69:                               ; CODE XREF: sub_EE43+22↑j
                STA     $374,X
                TYA
                STA     $392,X
                RTS
; End of function sub_EE43


; =============== S U B R O U T I N E =======================================


sub_EE71:                               ; CODE XREF: sub_EDFC+1B↑p
                                        ; sub_EE43+14↑p
                LDA     ($1E),Y
                BMI     loc_EE76
                RTS
; ---------------------------------------------------------------------------

loc_EE76:                               ; CODE XREF: sub_EE71+2↑j
                CMP     #$80
                BNE     loc_EE7E
                DEY
                LDA     ($1E),Y
                RTS
; ---------------------------------------------------------------------------

loc_EE7E:                               ; CODE XREF: sub_EE71+7↑j
                CMP     #$81
                BNE     loc_EE8B
                TYA
                INY
                SEC
                SBC     ($1E),Y
                TAY
                LDA     ($1E),Y
                RTS
; ---------------------------------------------------------------------------

loc_EE8B:                               ; CODE XREF: sub_EE71+F↑j
                CMP     #$82
                BNE     locret_EE93
                LDY     #0
                LDA     ($1E),Y

locret_EE93:                            ; CODE XREF: sub_EE71+1C↑j
                RTS
; End of function sub_EE71


; =============== S U B R O U T I N E =======================================


sub_EE94:                               ; CODE XREF: apu_update:loc_EA91↑p
                LDY     #$FF
                STY     byte_30C
                STY     byte_310
                STY     byte_314
                STY     byte_318
                INY
                STY     byte_307
                STY     byte_319

locret_EEA9:                            ; CODE XREF: sub_EEAA+1E↓j
                RTS
; End of function sub_EE94


; =============== S U B R O U T I N E =======================================


sub_EEAA:                               ; CODE XREF: apu_update+90↑p
                LDA     byte_319
                AND     #$20 ; ' '
                BEQ     loc_EEBA

loc_EEB1:                               ; CODE XREF: sub_EEAA+29↓j
                LDA     $3EC,X
                ORA     #$40 ; '@'
                STA     $3EC,X
                RTS
; ---------------------------------------------------------------------------

loc_EEBA:                               ; CODE XREF: sub_EEAA+5↑j
                LDA     $31A,X
                ORA     byte_319
                STA     byte_319
                LDA     $31A,X
                AND     #3
                BEQ     locret_EEA9
                LDY     $33E,X
                LDA     unk_EF42,Y
                AND     byte_307
                BNE     loc_EEB1
                LDA     unk_EF46,Y
                AND     byte_307
                STA     byte_307
                TYA
                ASL
                ASL
                TAY
                LDA     #$FF
                STA     $30C,Y
                LDA     $39E,X
                ORA     $3A4,X
                BEQ     loc_EF39
                LDA     $31A,X
                AND     #2
                BEQ     loc_EEF9
                JSR     sub_EF4A

loc_EEF9:                               ; CODE XREF: sub_EEAA+4A↑j
                LDA     $31A,X
                AND     #1
                BEQ     loc_EF39
                LDA     $374,X
                STA     $309,Y
                LDA     $32C,X
                STA     $30A,Y
                LDA     $39E,X
                STA     $30B,Y
                LDA     $3EC,X
                BMI     loc_EF22
                AND     #$40 ; '@'
                BNE     loc_EF22
                LDA     $32C,X
                BEQ     loc_EF2D
                BMI     loc_EF2D

loc_EF22:                               ; CODE XREF: sub_EEAA+6B↑j
                                        ; sub_EEAA+6F↑j
                LDA     $326,X
                AND     #$F8
                ORA     $3A4,X
                STA     $30C,Y

loc_EF2D:                               ; CODE XREF: sub_EEAA+74↑j
                                        ; sub_EEAA+76↑j
                LDY     $33E,X
                LDA     unk_EF42,Y
                ORA     byte_307
                STA     byte_307

loc_EF39:                               ; CODE XREF: sub_EEAA+43↑j
                                        ; sub_EEAA+54↑j
                LDA     $3EC,X
                AND     #$BF
                STA     $3EC,X
                RTS
; End of function sub_EEAA

; ---------------------------------------------------------------------------
unk_EF42:       .BYTE   1
                .BYTE   2
                .BYTE   4
                .BYTE   8
unk_EF46:       .BYTE $FE
                .BYTE $FD
                .BYTE $FB
                .BYTE $F7

; =============== S U B R O U T I N E =======================================


sub_EF4A:                               ; CODE XREF: sub_EEAA+4C↑p
                LDA     byte_307
                AND     #8
                BNE     locret_EF70
                LDA     byte_307
                ORA     #8
                STA     byte_307
                LDA     $374,X
                STA     byte_315
                LDA     $3CE,X
                STA     byte_317
                LDA     $3EC,X
                BPL     locret_EF70
                LDA     $326,X
                STA     byte_318

locret_EF70:                            ; CODE XREF: sub_EF4A+5↑j
                                        ; sub_EF4A+1E↑j
                RTS
; End of function sub_EF4A


; =============== S U B R O U T I N E =======================================


apu_reset:                              ; CODE XREF: j_apu_reset↑j
                                        ; reset+44↑p ...
                LDA     #0
                STA     byte_302
                STA     byte_308
; End of function apu_reset


; =============== S U B R O U T I N E =======================================


apu_stop:                               ; CODE XREF: j_apu_stop↑j
                                        ; sub_D0BF+D↑p ...
                TXA
                PHA
                LDX     #6

loc_EF7D:                               ; CODE XREF: apu_stop+A↓j
                LDA     #0
                STA     $319,X
                DEX
                BNE     loc_EF7D
                PLA
                TAX
                LDA     #0
                STA     APU_MASTERCTRL_REG
                STA     APU_DELTA_REG
                RTS
; End of function apu_stop


; =============== S U B R O U T I N E =======================================

; A = sound num (1...35)

apu_play:                               ; CODE XREF: j_apu_play↑j
                                        ; j_apu_play_0↑j ...
                INC     byte_308
                STA     byte_1E
                TXA
                PHA
                TYA
                PHA
                LDA     byte_1E
                JSR     sub_EFA6
                DEC     byte_308
                PLA
                TAY
                PLA
                TAX
                RTS
; End of function apu_play


; =============== S U B R O U T I N E =======================================


sub_EFA6:                               ; CODE XREF: apu_update+16↑p
                                        ; apu_play+B↑p
                STA     byte_305
                ASL
                TAX
                LDA     apu_sound_tab,X
                STA     byte_1E
                LDA     apu_sound_tab+1,X
                STA     byte_1F
                LDY     #0
                LDA     ($1E),Y
                BNE     loc_EFC2
                LDA     byte_1F
                PHA
                LDA     byte_1E
                PHA
                RTS
; ---------------------------------------------------------------------------

loc_EFC2:                               ; CODE XREF: sub_EFA6+13↑j
                INY
                STA     apu_sound_length

loc_EFC6:                               ; CODE XREF: sub_EFA6+73↓j
                LDA     ($1E),Y
                INY
                TAX
                LDA     ($1E),Y
                BEQ     loc_F012
                LDA     $31A,X
                AND     #$40 ; '@'
                BNE     locret_F01B
                LDA     byte_305
                STA     $3C2,X
                TXA
                PHA
                LDA     #9
                STA     byte_305

loc_EFE2:                               ; CODE XREF: sub_EFA6+4A↓j
                LDA     ($1E),Y
                INY
                STA     $31A,X
                TXA
                CLC
                ADC     #6
                TAX
                DEC     byte_305
                BNE     loc_EFE2
                PLA
                TAX
                LDA     #0
                STA     $350,X
                STA     $3EC,X
                STA     $356,X
                STA     $3CE,X
                LDA     $3C2,X
                STA     byte_305
                LDA     #1
                STA     $35C,X
                STA     $368,X
                BNE     loc_F016

loc_F012:                               ; CODE XREF: sub_EFA6+26↑j
                STA     $31A,X
                INY

loc_F016:                               ; CODE XREF: sub_EFA6+6A↑j
                DEC     apu_sound_length
                BNE     loc_EFC6

locret_F01B:                            ; CODE XREF: sub_EFA6+2D↑j
                RTS
; End of function sub_EFA6

; ---------------------------------------------------------------------------
apu_length_tab: .WORD $7FE
                .WORD $78E
                .WORD $720
                .WORD $6BA
                .WORD $65A
                .WORD $5FE
                .WORD $5A8
                .WORD $556
                .WORD $50A
                .WORD $4C2
                .WORD $47E
                .WORD $43C
                .WORD $3FF
                .WORD $3C7
                .WORD $390
                .WORD $35D
                .WORD $32D
                .WORD $2FF
                .WORD $2D4
                .WORD $2AB
                .WORD $285
                .WORD $261
                .WORD $23F
                .WORD $21E
                .WORD $1FF
                .WORD $1E3
                .WORD $1C8
                .WORD $1AE
                .WORD $196
                .WORD $17F
                .WORD $16A
                .WORD $155
                .WORD $142
                .WORD $130
                .WORD $11F
                .WORD $10F
                .WORD $FF
                .WORD $F1
                .WORD $E4
                .WORD $D7
                .WORD $CB
                .WORD $BF
                .WORD $B5
                .WORD $AA
                .WORD $A1
                .WORD $98
                .WORD $8F
                .WORD $87
                .WORD $7F
                .WORD $78
                .WORD $72
                .WORD $6B
                .WORD $65
                .WORD $5F
                .WORD $5A
                .WORD $55
                .WORD $50
                .WORD $4C
                .WORD $47
                .WORD $43
                .WORD $3F
                .WORD $3C
                .WORD $39
                .WORD $35
                .WORD $32
                .WORD $2F
                .WORD $2D
                .WORD $2A
                .WORD $28
                .WORD $26
                .WORD $23
                .WORD $21
                .WORD $1F
                .WORD $1E
                .WORD $1C
                .WORD $1A
                .WORD $19
                .WORD $17
                .WORD $16
                .WORD $15
                .WORD $14
                .WORD $13
                .WORD $11
                .WORD $10
                .WORD $F
                .WORD $F
                .WORD $E
                .WORD $D
                .WORD $C
                .WORD $B
                .WORD $B
                .WORD $A
                .WORD $A
                .WORD 9
                .WORD 8
                .WORD 8
                .WORD 7
                .WORD 7
                .WORD 7
                .WORD 6
                .WORD 6
                .WORD 5
                .WORD 5
                .WORD 5
                .WORD 5
                .WORD 4
                .WORD 4
                .WORD 4

; =============== S U B R O U T I N E =======================================


sub_F0F4:                               ; CODE XREF: sub_C06C↑j
                                        ; sub_D3A8:loc_D3AF↑p ...
                LDA     #3

loc_F0F6:                               ; CODE XREF: sub_F0FA+2↓j
                STA     byte_302
                RTS
; End of function sub_F0F4


; =============== S U B R O U T I N E =======================================


sub_F0FA:                               ; CODE XREF: sub_C06F↑j
                                        ; sub_D3A8+33↑p ...
                LDA     #1
                BNE     loc_F0F6
; End of function sub_F0FA

; ---------------------------------------------------------------------------
apu_sound_tab:  .WORD byte_F180
                .WORD byte_F209
                .WORD byte_F2FF
                .WORD byte_F372
                .WORD byte_F458
                .WORD byte_F4D8
                .WORD byte_F52B
                .WORD byte_F544
                .WORD byte_F5AB
                .WORD byte_F5D8
                .WORD byte_F60D
                .WORD byte_F6EF
                .WORD byte_F728
                .WORD byte_F765
                .WORD byte_F773
                .WORD byte_F783
                .WORD byte_F797
                .WORD byte_F7AA
                .WORD byte_F7BD
                .WORD byte_F7D8
                .WORD byte_F7ED
                .WORD byte_F7FD
                .WORD byte_F80E
                .WORD byte_F821
                .WORD byte_F837
                .WORD byte_F84C
                .WORD byte_F942
                .WORD byte_F9D1
                .WORD byte_FA2D
                .WORD byte_FA50
                .WORD byte_FB7A
                .WORD byte_FBDB
                .WORD byte_FC42
                .WORD byte_FD75
                .WORD byte_FE5C
                .WORD byte_FE71

; =============== S U B R O U T I N E =======================================


sub_F146:                               ; CODE XREF: sub_C072↑j
                                        ; sub_E8C9+62↑p ...
                LDA     #1
                STA     byte_308
                LDX     #2

loc_F14D:                               ; CODE XREF: sub_F146+1F↓j
                LDA     $350,X
                ORA     #$30 ; '0'
                STA     $350,X
                LDA     #$10
                STA     $3AA,X
                LDA     #8
                STA     $3B6,X
                LDA     #0
                STA     $3B0,X
                DEX
                BPL     loc_F14D
                STA     byte_308
                RTS
; End of function sub_F146

; ---------------------------------------------------------------------------
                .BYTE   1
                .BYTE   2
                .BYTE   3
                .BYTE   4
                .BYTE   6
                .BYTE   8
                .BYTE  $C
                .BYTE $10
                .BYTE $18
                .BYTE $20
                .BYTE $30 ; 0
                .BYTE $40 ; @
                .BYTE $60 ; `
                .BYTE $80
                .BYTE $C0
                .BYTE   0
                .BYTE   9
                .BYTE $12
                .BYTE $1C
                .BYTE $24 ; $
                .BYTE $2A ; *
byte_F180:      .BYTE 1, 5, 1, $BF, 7, $9B, 0, 1, 0, $8B, $F1, $38, $E5
                                        ; DATA XREF: ROM:apu_sound_tab↑o
                .BYTE $30, $38, $30
word_F190:      .WORD $82E8
                .WORD byte_F1A2
                .WORD byte_F1AA
                .WORD byte_F1B2
                .WORD byte_F1BA
                .WORD byte_F1C7
                .WORD byte_F1DF
                .WORD byte_F1EB
                .WORD byte_F1F5
byte_F1A2:      .BYTE $F, $D, $C, $B, $A, 6, 8, $80
                                        ; DATA XREF: ROM:F192↑o
byte_F1AA:      .BYTE $F, $C, 9, 7, 4, 2, 0, $80 ; DATA XREF: ROM:F194↑o
byte_F1B2:      .BYTE $C, $E, $F, $D, $B, 9, 7, $80
                                        ; DATA XREF: ROM:F196↑o
byte_F1BA:      .BYTE $F, $F, $F, $F, $F, $F, $F, $E, $C, $A, 8, 0, $80
                                        ; DATA XREF: ROM:F198↑o
byte_F1C7:      .BYTE 7, 9, $C, $F, $F, $F, $F, $E, $D, $D, $C, $C, $C
                                        ; DATA XREF: ROM:F19A↑o
                .BYTE $B, $B, $B, $A, $A, $A, 9, 9, 9, 8, $80
byte_F1DF:      .BYTE 6, $A, $C, $F, $F, $F, $F, $F, $F, $E, $D, $80
                                        ; DATA XREF: ROM:F19C↑o
byte_F1EB:      .BYTE $C, $F, $F, $D, $C, $B, $A, 9, 8, $80
                                        ; DATA XREF: ROM:F19E↑o
byte_F1F5:      .BYTE $C, $D, $E, $F, $F, $E, $D, $D, $C, $C, $C, $B, $B
                                        ; DATA XREF: ROM:F1A0↑o
                .BYTE $B, $A, $A, $A, $B, $81, 6
byte_F209:      .BYTE 3, 0, 1, $7F, 7, 0, $FE, 2, 0, $28, $F2, 1, 1, $BB
                                        ; DATA XREF: ROM:F100↑o
                .BYTE 3, 0, $FE, 2, 1, $B9, $F2, 2, 3, $BD, 2, 0, $A, 2
                .BYTE 2, $E1, $F2, $25, $E5, $27, $28, $25, $E3, $27, $E5
                .BYTE $28, $E3, $27, $E5, $25, $27, $25, $27, $28, $25
                .BYTE $E3, $27, $E5, $28, $E3, $27, $E5, $25, $27, $28
                .BYTE $2A, $2C, $28, $E3, $2A, $E5, $2C, $E3, $2A, $E5
                .BYTE $28, $2A, $28, $2A, $2C, $28, $E3, $2A, $E5, $2C
                .BYTE $E3, $2A, $E5, $28, $2A, $2D, $2F, $31, $2D, $E3
                .BYTE $2F, $E5, $31, $E3, $2F, $E5, $2D, $2F, $2D, $2F
                .BYTE $31, $2D, $E3, $2F, $E5, $31, $E3, $2F, $E5, $2D
                .BYTE $2F, $2C, $2E, $30, $2C, $E3, $2E, $E5, $30, $E3
                .BYTE $2E, $E5, $2C, $2E, $2C, $2E, $30, $31, $33, $34
                .BYTE $36, $38, $3D, $E9, $3B, $39, $E3, $38, $E5, $36
                .BYTE $E3, $38, $F4, $3B, $E9, $39, $38, $36, $E7, $38
                .BYTE $39, $E9, $38, $36, $34, $33, $E3, $34, $33, $34
                .BYTE $33, $34, $33, $34, $33, $34, $33, $34, $33, $34
                .BYTE $33, $34, $33, $EB, $80, $28, $F2, $8D, $60, 8, $92
                .BYTE $D9, $F2, $19, $E3, $31, $25, $2C, $38, $31, $25
                .BYTE $25, $19, $31, $25, $2C, $38, $31, $25, $25, $81
                .BYTE $60, $BC, $F2, $89, $F9, $80, $B9, $F2, 0, $FF, 0
                .BYTE 5, 0, 3, 0, 0, $8F, 6, $8D, $60, 8, $92, $D9, $F2
                .BYTE $19, $E5, $E5, $E3, $E3, $E3, $E3, $E5, $E5, $E3
                .BYTE $E3, $E3, $E3, $81, $60, $E6, $F2, $89, $F9, $80
                .BYTE $E3, $F2
byte_F2FF:      .BYTE 2, 0, 1, $7F, 1, 0, $F8, 2, 0, $14, $F3, 1, 1, $BF
                                        ; DATA XREF: ROM:F102↑o
                .BYTE 1, 0, 4, 2, 1, $41, $F3, $8D, $60, $10, $92, $62
                .BYTE $F3, $31, $E3, $25, $2C, $25, $28, $25, $2A, $25
                .BYTE $31, $25, $2C, $25, $28, $25, $2A, $25, $81, $60
                .BYTE $17, $F3, $8D, $60, 4, $36, $31, $35, $31, $38, $31
                .BYTE $3D, $31, $81, $60, $32, $F3, $80, $14, $F3, $8D
                .BYTE $60, $12, $92, $60, $F3, $19, $E3, $E3, $E3, $E3
                .BYTE $25, $E5, $19, $E5, $E3, $E3, $E3, $E3, $25, $E5
                .BYTE $19, $81, $60, $44, $F3, $89, $E8, $80, $41, $F3
                .BYTE $C, $C, 0, 1, 1, 3, 0, $FB, 0, 7, 0, $FB, 0, 3, 0
                .BYTE $FB, 0, 0
byte_F372:      .BYTE 3, 0, $41, $7F, 7, 0, 0, 2, 0, $91, $F3, 1, $43
                                        ; DATA XREF: ROM:F104↑o
                .BYTE $BF, 2, 0, $C, 2, 2, $17, $F4, 2, $41, $BF, 1, $82
                .BYTE 0, 2, 1, $47, $F4, $31, $E3, $25, $2C, $25, $29
                .BYTE $25, $2A, $25, $31, $25, $2C, $25, $29, $2A, $2C
                .BYTE $31, $33, $25, $35, $25, $36, $25, $33, $25, $35
                .BYTE $25, $31, $25, $2C, $25, $29, $25, $2C, $20, $27
                .BYTE $20, $24, $20, $25, $20, $2C, $20, $27, $20, $24
                .BYTE $20, $25, $20, $2C, $20, $2E, $20, $30, $20, $2C
                .BYTE $20, $2E, $20, $30, $20, $31, $20, $33, $20, $36
                .BYTE $23, $33, $23, $2F, $23, $33, $23, $36, $23, $33
                .BYTE $23, $2F, $23, $33, $23, $36, $23, $38, $23, $3A
                .BYTE $23, $36, $23, $38, $23, $3A, $23, $3B, $23, $3D
                .BYTE $23, $3A, $1E, $38, $1E, $36, $1E, $35, $1E, $36
                .BYTE $1E, $35, $1E, $33, $1E, $31, $1E, $2F, $2E, $2F
                .BYTE $1E, $2E, $1E, $2F, $1E, $31, $2F, $2E, $2C, $2F
                .BYTE $2E, $2C, $2A, $91, 1, $80, $91, $F3, $8F, 9, $8D
                .BYTE $60, $F, $92, $38, $F4, $19, $E3, $E3, $E3, $E3
                .BYTE $25, $E5, $19, $81, $60, $1C, $F4, $19, $E3, $E5
                .BYTE $E3, $25, $E3, $E5, $E3, $89, 7, $80, $19, $F4, 0
                .BYTE 0, $FB, 0, 0, 0, 3, 0, 0, 0, $FB, 0, 0, 0, 0, $8D
                .BYTE $60, $10, $8A, $F, $49, $E3, $49, $8A, $FF, $81
                .BYTE $60, $4C, $F4, $80, $47, $F4
byte_F458:      .BYTE 3, 2, $41, $7F, 6, 0, 0, 3, 0, $77, $F4, 3, $41
                                        ; DATA XREF: ROM:F106↑o
                .BYTE $7F, 6, 0, 0, 3, 1, $A2, $F4, 4, $41, $8E, 6, 0
                .BYTE $C, 3, 2, $BE, $F4, $31, $E2, $2F, $31, $E9, 0, $E4
                .BYTE $34, $E7, $36, $33, $E9, 0, $E6, $31, $E4, 0, $31
                .BYTE $EA, $8A, $FE, $88, 5, $28, $E6, $27, $2A, $28, $27
                .BYTE $25, $28, $E4, $27, $25, $23, $25, $E6, $24, $25
                .BYTE $27, $25, $EA, $86, $25, $F3, 0, $E4, $21, $E7, $23
                .BYTE $20, $E9, 0, $E6, $2C, $E4, 0, $2C, $EA, $8A, $FD
                .BYTE 0, $E8, $23, $21, $20, $1E, $20, $1C, $EA, $82, $19
                .BYTE $F3, 0, $E4, $15, $E7, $17, $14, $E9, 0, $E6, $19
                .BYTE $E4, 0, $19, $EA, $8A, $FD, $19, $E0, $7C, $88, 1
                .BYTE $19, $E4, $82
byte_F4D8:      .BYTE 2, 0, 3, $30, 0, 0, 0, 2, 0, $ED, $F4, 1, 1, $B6
                                        ; DATA XREF: ROM:F108↑o
                .BYTE 1, 0, 1, 2, 1, $A, $F5, $8F, $F, $8A, 3, $8D, $60
                .BYTE 4, $19, $E3, $8A, 2, $81, $60, $F4, $F4, $8D, $60
                .BYTE $10, $19, $E3, $8A, $FF, $81, $60, $FF, $F4, $80
                .BYTE $EF, $F4, $2E, $E3, $2A, $2E, $30, $34, $38, $36
                .BYTE $31, $2E, $2A, $2E, $30, $34, $33, $31, $2F, $30
                .BYTE $25, $2A, $2E, $31, $30, $2A, $27, $29, $2C, $31
                .BYTE $36, $31, $80, $A, $F5
byte_F52B:      .BYTE 1, 3, 1, $8F, 0, 0, 4, 1, 2, $36, $F5, $8D, $60
                                        ; DATA XREF: ROM:F10A↑o
                .BYTE 8, $31, $E1, $37, $E1, $89, 2, $81, $60, $39, $F5
                .BYTE $82
byte_F544:      .BYTE 3, 2, 1, $3F, 7, 0, $FE, 2, 0, $63, $F5, 3, $41
                                        ; DATA XREF: ROM:F10C↑o
                .BYTE $3E, 3, 0, $FE, 2, 1, $73, $F5, 4, $43, $BE, 2, 0
                .BYTE $FE, 2, 2, $84, $F5, $2C, $E5, $31, $38, $36, $E3
                .BYTE $38, $E5, $E3, $3B, $E5, $3A, $36, $38, $EB, $82
                .BYTE $8D, $60, 4, $20, $E3, $2C, $25, $19, $19, $19, $25
                .BYTE $19, $81, $60, $76, $F5, $82, $8F, 6, $31, $E3, $E5
                .BYTE $E3, $E5, $2F, $E3, $E5, $E3, $E5, $2A, $E3, $E5
                .BYTE $E3, $2C, $E5, $2F, $E3, $31, $E3, $2C, $E5, $2F
                .BYTE $E3, $31, $2C, $E5, $2F, $E3, $31, $2F, $2E, $2C
                .BYTE $2A, $8B, 1, $82
byte_F5AB:      .BYTE 2, 3, $61, $BF, 7, 0, $F8, 2, 0, $C0, $F5, 4, $41
                                        ; DATA XREF: ROM:F10E↑o
                .BYTE $7F, 7, 0, $F8, 2, 1, $CC, $F5, $38, $E6, $38, $E4
                .BYTE $3C, $3A, $38, $3C, $41, $3F, $E8, $82, $35, $E6
                .BYTE $35, $E4, $35, $36, $36, $38, $38, $3C, $E8, $82
byte_F5D8:      .BYTE 2, 3, $61, $7F, 7, 0, $F4, 2, 0, $ED, $F5, 4, $41
                                        ; DATA XREF: ROM:F110↑o
                .BYTE $BF, 3, 0, 0, 2, 1, $FD, $F5, $31, $E3, $2C, $31
                .BYTE $2C, $31, $2C, $31, $2C, $31, $E5, $35, $E3, $3D
                .BYTE $E9, $82, $25, $E3, $29, $2C, $31, $35, $38, $3D
                .BYTE $38, $2C, $E5, $31, $E3, $35, $E9, $82
byte_F60D:      .BYTE 3, 2, $41, $BF, 7, 0, 4, 2, 0, $2C, $F6, 3, $41
                                        ; DATA XREF: ROM:F112↑o
                .BYTE $7C, 3, 0, 4, 2, 1, $A2, $F6, 4, $43, $BA, 2, 0
                .BYTE 4, 2, 2, $D9, $F6, 0, $E3, $25, $23, $E5, $25, $27
                .BYTE $28, $27, $E3, $28, $E5, $2A, $E3, $28, $E5, $27
                .BYTE $E9, $23, 0, $E3, $28, $27, $E5, $28, $2A, $2C, $2A
                .BYTE $E3, $2C, $E5, $2D, $E3, $2C, $E5, $2A, $E9, $2E
                .BYTE 0, $E3, $31, $2F, $E5, $31, $33, $34, $33, $E3, $34
                .BYTE $E5, $36, $E3, $34, $E5, $33, $E9, $2F, $2D, $2C
                .BYTE 0, $E5, $2C, $E3, $2D, $2F, $E5, $2C, $2E, $E9, $31
                .BYTE $34, $E3, $E5, $E5, $E5, $E3, $E9, $33, $E7, $36
                .BYTE $31, $E3, $E5, $E5, $E5, $E3, $E9, $2F, $E7, $33
                .BYTE $34, $E3, $E5, $E5, $E5, $E3, $E9, $33, $E7, $36
                .BYTE $31, $E3, $E5, $E5, $E5, $E3, $E9, $2F, $E5, $31
                .BYTE $33, $E7, $34, $EB, 0, $80, $2C, $F6, $8D, $60, $21
                .BYTE $92, $B8, $F6, $19, $E3, $31, $25, $2C, $38, $31
                .BYTE $25, $19, $81, $60, $A5, $F6, $80, $A2, $F6, 0, 0
                .BYTE 0, 2, 2, 0, $FE, $FE, 0, 2, 2, 0, $FE, $FE, 0, $FB
                .BYTE 0, 2, 0, 0, 5, 0, $FE, 0, $FB, 0, 2, 0, 5, 0, $FE
                .BYTE 0, 0, $8F, 9, $8D, $60, $21, $92, $B8, $F6, $25
                .BYTE $E5, $E5, $E3, $E3, $E3, $E3, $81, $60, $DE, $F6
                .BYTE $80, $DB, $F6
byte_F6EF:      .BYTE 3, 0, 1, $BF, 0, 0, 0, 1, 0, $E, $F7, 1, 2, $3F
                                        ; DATA XREF: ROM:F114↑o
                .BYTE 0, 0, $C, 1, 3, $E, $F7, 2, 1, $BF, 1, 0, $C, 1
                .BYTE 1, $16, $F7, $8F, 4, $93, $80, $18, $25, $F0, $82
                .BYTE $93, $80, $18, $30, $E5, $34, $38, $36, $31, $2E
                .BYTE $2A, $22, $3A, $34, $95, $19, $F7, $82
byte_F728:      .BYTE 3, 0, 1, $3F, 0, 0, $C, 2, 0, $47, $F7, 1, 1, $3F
                                        ; DATA XREF: ROM:F116↑o
                .BYTE 0, 0, $10, 2, 1, $47, $F7, 2, 1, $8F, 1, 0, $C, 1
                .BYTE 2, $55, $F7, $D, $E5, $8D, $60, $10, $D, $E3, $8A
                .BYTE $FF, $81, $60, $4C, $F7, $82, $31, $E3, $3D, $31
                .BYTE $3E, $31, $3F, $31, $40, $31, $41, 0, $EC, $8B, 5
                .BYTE $82
byte_F765:      .BYTE 1, 3, 1, $3F, 0, $8A, 0, 1, 1, $70, $F7, $42, $E4
                                        ; DATA XREF: ROM:F118↑o
                .BYTE $82
byte_F773:      .BYTE 1, 3, 1, $3F, 0, $89, 0, 1, 1, $7E, $F7, $2A, $E2
                                        ; DATA XREF: ROM:F11A↑o
                .BYTE $2A, $E4, $82
byte_F783:      .BYTE 1, 3, 1, $7F, 0, $89, 0, 1, 1, $8E, $F7, $31, $E2
                                        ; DATA XREF: ROM:F11C↑o
                .BYTE $33, $36, $38, $3A, $3C, $38, $82
byte_F797:      .BYTE 1, 4, 3, $3F, 0, $9F, 0, 1, 0, $A2, $F7, $8F, $C
                                        ; DATA XREF: ROM:F11E↑o
                .BYTE $93, $80, $20, $19, $ED, $82
byte_F7AA:      .BYTE 1, 3, 3, $BF, 0, $83, 0, 1, 1, $B5, $F7, $93, $84
                                        ; DATA XREF: ROM:F120↑o
                .BYTE $80, $8F, $E, $24, $E8, $82
byte_F7BD:      .BYTE 1, 4, 1, $BF, 0, 0, 0, 1, 3, $C8, $F7, $E, $E6, $8D
                                        ; DATA XREF: ROM:F122↑o
                .BYTE $60, 8, $88, 1, $F, $E4, $8A, $FE, $81, $60, $CF
                .BYTE $F7, $82
byte_F7D8:      .BYTE 1, 4, 1, $BF, 0, $89, 0, 1, 0, $E3, $F7, $93, $83
                                        ; DATA XREF: ROM:F124↑o
                .BYTE $30, $2C, $E3, $2E, $95, $E6, $F7, $82
byte_F7ED:      .BYTE 1, 3, 1, $FF, 0, 0, 0, 1, 2, $F8, $F7, $52, $E1
                                        ; DATA XREF: ROM:F126↑o
                .BYTE $6A, $E2, $82
byte_F7FD:      .BYTE 1, 4, 1, $7F, 1, $81, 0, 1, 0, 8, $F8, $38, $E6
                                        ; DATA XREF: ROM:F128↑o
                .BYTE $83, $8A, $20, $82
byte_F80E:      .BYTE 1, 4, 3, $BF, 0, $F7, 0, 1, 1, $19, $F8, $8F, $A
                                        ; DATA XREF: ROM:F12A↑o
                .BYTE $93, $84, $60, $19, $EB, $82
byte_F821:      .BYTE 1, 4, 1, $BF, 0, $8A, $F4, 1, 1, $2C, $F8, $93, $87
                                        ; DATA XREF: ROM:F12C↑o
                .BYTE $20, $2E, $E4, $89, 2, $95, $2F, $F8, $82
byte_F837:      .BYTE 1, 3, 1, $3F, 0, 0, 8, 1, 1, $42, $F8, $93, $84
                                        ; DATA XREF: ROM:F12E↑o
                .BYTE $40, $49, $E1, $30, $95, $45, $F8, $82
byte_F84C:      .BYTE 3, 0, 1, $7F, 7, 0, $FE, 2, 0, $6B, $F8, 1, 1, $BB
                                        ; DATA XREF: ROM:F130↑o
                .BYTE 0, 0, $A, 2, 2, $FE, $F8, 2, 3, $BD, 1, 0, $A, 2
                .BYTE 1, $28, $F9, $25, $E3, $E5, $E3, $E5, $E5, $E5, $E5
                .BYTE $28, $E5, $27, $E5, $25, $E3, $E5, $E3, $E5, $E5
                .BYTE $E5, $E5, $2C, $E5, $28, $E5, $25, $E3, $E5, $E3
                .BYTE $E5, $E5, $E5, $E5, $28, $E5, $27, $E5, $25, $E3
                .BYTE $E5, $E3, $E5, $E5, $E5, $24, $E5, $22, $E5, $24
                .BYTE $E5, $25, $E3, $E5, $E3, $E5, $E5, $E5, $E5, $28
                .BYTE $E5, $27, $E5, $2C, $E3, $E5, $E3, $E5, $E5, $E7
                .BYTE $2A, $E5, $2C, $E5, $28, $E3, $E5, $E3, $E5, $E5
                .BYTE $E7, $2A, $E5, $28, $E5, $27, $E3, $E5, $E3, $E5
                .BYTE $E5, $E5, $E5, $28, $E5, $2A, $E5, $31, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $2F, $E5, $2C, $E5, $E5, $2F
                .BYTE $E3, $E5, $E3, $E5, $E5, $E7, $2A, $E5, $2C, $E5
                .BYTE $2D, $E3, $E5, $E3, $E5, $E5, $E5, $2C, $E5, $2A
                .BYTE $E5, $28, $E5, $2C, $E3, $E5, $E3, $E5, $E5, $E5
                .BYTE $2A, $E5, $28, $E5, $27, $E5, $80, $6B, $F8, $8D
                .BYTE $60, $C, $19, $E3, $31, $25, $2C, $38, $31, $25
                .BYTE $25, $19, $31, $25, $2C, $38, $31, $25, $25, $92
                .BYTE $1C, $F9, $81, $60, 1, $F9, $80, $FE, $F8, $F9, $FF
                .BYTE 5, 3, 5, $FF, $FE, $FE, 5, $FF, $FE, $FE, $8D, $60
                .BYTE $C, $19, $E5, $E5, $E3, $E3, $E3, $E3, $E5, $E5
                .BYTE $E3, $E3, $E3, $E3, $92, $1C, $F9, $81, $60, $2B
                .BYTE $F9, $80, $28, $F9
byte_F942:      .BYTE 3, 0, 1, $3F, 7, 0, 0, 2, 0, $61, $F9, 1, 1, $7E
                                        ; DATA XREF: ROM:F132↑o
                .BYTE 3, 0, 0, 2, 1, $93, $F9, 2, 1, $8E, 3, 0, $18, 2
                .BYTE 2, $B3, $F9, $8D, $60, 4, $19, $E5, $1B, $1C, $1B
                .BYTE $1E, $1B, $1C, $1B, $81, $60, $64, $F9, $19, $1B
                .BYTE $1C, $1B, $1E, $1B, $1C, $1E, $8D, $60, 5, $92, $8E
                .BYTE $F9, $19, $1B, $1C, $1B, $1E, $1B, $1C, $1B, $81
                .BYTE $60, $7C, $F9, $80, $61, $F9, $FB, 0, $FE, 0, 7
                .BYTE $8D, $60, $A, $92, $A9, $F9, 0, $E5, 0, $17, 0, $19
                .BYTE 0, $17, 0, $81, $60, $96, $F9, $80, $93, $F9, $FB
                .BYTE 0, $FE, 0, 7, 0, 0, 0, 0, 0, $8D, $60, $A, $92, $A9
                .BYTE $F9, $D, $E3, $14, $D, $14, $D, $14, $D, $14, $D
                .BYTE $14, $D, $14, $D, $14, $D, $14, $81, $60, $B6, $F9
                .BYTE $80, $B3, $F9
byte_F9D1:      .BYTE 3, 2, $41, $3F, 7, 0, 0, 2, 0, $F0, $F9, 3, $41
                                        ; DATA XREF: ROM:F134↑o
                .BYTE $3F, 7, 0, 0, 2, 1, $FD, $F9, 4, $41, $8D, 7, 0
                .BYTE $C, 2, 2, 8, $FA, $2C, $E5, $29, $E3, $2A, $2C, $E9
                .BYTE $2E, $E7, $E7, $30, $EB, $82, $25, $E5, $E3, $E3
                .BYTE $E9, $27, $E7, $E7, $29, $EB, $82, $19, $E5, $25
                .BYTE $E3, $E3, $19, $E5, $25, $E3, $E3, $19, $E5, $25
                .BYTE $E3, $E3, $13, $E5, $1F, $E3, $E3, $13, $E5, $1F
                .BYTE $E3, $E3, $1D, $E3, $1D, $24, $1D, $24, $1D, $24
                .BYTE $1D, $24, $E9, $82
byte_FA2D:      .BYTE 2, 2, $41, $3F, 7, 0, 0, 2, 0, $44, $FA, 3, $41
                                        ; DATA XREF: ROM:F136↑o
                .BYTE $3F, 7, 0, 0, 2, 1, $42, $FA, 0, $E5, $93, $85, $18
                .BYTE $2C, $E3, $31, $33, $38, $95, $47, $FA, $82
byte_FA50:      .BYTE 3, 0, 1, $3F, 7, 0, 0, 2, 0, $6F, $FA, 1, 1, $8F
                                        ; DATA XREF: ROM:F138↑o
                .BYTE 3, 0, $C, 2, 2, $18, $FB, 2, 1, $3F, 3, 0, 0, 2
                .BYTE 1, $5B, $FB, $25, $E5, $27, $E7, 0, $E3, $28, $E3
                .BYTE $27, $E5, $25, $27, $28, $31, $E3, $E5, $E3, $E5
                .BYTE $E5, $E5, $2F, $2D, $2C, $2A, $E5, $2C, $E7, $2D
                .BYTE $E5, $2C, $2A, $2C, $2D, $2F, $E3, $E5, $E3, $E5
                .BYTE $E5, $E5, $2D, $2C, $2A, $25, $E5, $27, $E7, 0, $E3
                .BYTE $28, $E3, $27, $E5, $25, $27, $28, $31, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $33, $34, $E7, $31, $E5, $33
                .BYTE $E7, $34, $E5, $33, $31, $33, $34, $33, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $31, $2F, $E7, $31, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $34, $33, $31, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $2F, $2F, $31, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $34, $33, $31, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $2F, $2F, $2D, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $31, $2F, $2D, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $2C, $2C, $2D, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $31, $2F, $2D, $E3, $E5
                .BYTE $E3, $E5, $E5, $E5, $E5, $2D, $2D, $80, $6F, $FA
                .BYTE $8D, $60, $10, $92, $3B, $FB, $19, $E5, $31, $25
                .BYTE $2C, $38, $31, $25, $25, $19, $31, $25, $2C, $92
                .BYTE $4B, $FB, $38, $31, $25, $25, $81, $60, $1B, $FB
                .BYTE $89, 5, $80, $18, $FB, 1, 1, 1, $FE, 2, 2, 2, 2
                .BYTE 5, $FD, $FC, 2, 5, $FD, $FC, 0, $FF, $FF, $FF, $FF
                .BYTE $FE, $FE, $FE, $FE, 0, 0, 0, 0, 0, 0, 0, 0, $8D
                .BYTE $60, $10, $92, $3B, $FB, $19, $E5, $E5, $E3, $E3
                .BYTE $E3, $E3, $E5, $E5, $92, $4B, $FB, $E3, $E3, $E3
                .BYTE $E3, $81, $60, $5E, $FB, $89, 5, $80, $5B, $FB
byte_FB7A:      .BYTE 3, 2, $41, $7F, 7, 0, 4, 2, 0, $99, $FB, 3, $41
                                        ; DATA XREF: ROM:F13A↑o
                .BYTE $BF, 7, 0, $10, 2, 2, $B0, $FB, 4, $41, $BF, 7, 0
                .BYTE 4, 2, 1, $C6, $FB, $31, $E4, $31, $31, $2C, $E7
                .BYTE $38, $38, $36, $E4, $36, $36, $33, $E7, $3B, $36
                .BYTE $3D, $E3, $3B, $44, $E0, $2A, $82, 0, $EB, $23, $E3
                .BYTE $27, $2A, $2F, $2A, $2F, $33, $27, $2F, $33, $36
                .BYTE $3B, $33, $36, $3B, $3F, $38, $EB, $82, $19, $F1
                .BYTE $E3, $F1, $E3, $F1, $E3, $F1, $E3, $17, $F1, $E3
                .BYTE $F1, $E3, $F1, $E3, $F1, $E3, $19, $EB, $82
byte_FBDB:      .BYTE 2, 0, 1, $3F, 7, 0, $F8, 2, 0, $F0, $FB, 1, 1, $BE
                                        ; DATA XREF: ROM:F13C↑o
                .BYTE 7, 0, $1C, 2, 2, $24, $FC, $8D, $60, 1, $25, $E5
                .BYTE $27, $28, $25, $27, $28, $2A, $2C, $25, $25, $2C
                .BYTE $25, $20, $19, $25, $19, $28, $2A, $2C, $28, $2A
                .BYTE $2C, $2E, $2F, $2C, $2C, $34, $2C, $20, $19, $81
                .BYTE $60, $1D, $FC, $25, $19, $89, $FE, $80, $F3, $FB
                .BYTE $20, $25, $89, 2, $80, $F0, $FB, $8D, $60, 8, $D
                .BYTE $E5, $D, $19, $D, $14, $D, $17, $D, $92, $3A, $FC
                .BYTE $81, $60, $27, $FC, $80, $24, $FC, 2, 0, 0, 0, $FE
                .BYTE 0, 0, 0
byte_FC42:      .BYTE 3, 0, 1, $BF, 8, 0, 0, 2, 0, $61, $FC, 1, 1, $BE
                                        ; DATA XREF: ROM:F13E↑o
                .BYTE 2, 0, 0, 2, 1, $BF, $FC, 2, 1, $BF, 1, 0, $C, 2
                .BYTE 2, $32, $FD, $2F, $E3, $30, $E3, $31, $F5, $2C, $EB
                .BYTE $28, $E3, $29, $E3, $2A, $F5, $25, $EB, $2A, $E3
                .BYTE $2C, $E3, $2D, $F5, $2F, $E5, $31, $E7, 0, $E5, $33
                .BYTE $E5, 0, $E5, $34, 0, $31, $EB, $20, $E3, 0, $20
                .BYTE 0, $20, 0, $20, 0, $20, 0, $38, 0, $31, 0, $38, 0
                .BYTE $3D, $E9, $38, $E9, $3B, $E9, $36, $E9, $39, $E7
                .BYTE $38, $36, $34, $38, $E5, $36, $34, $33, $36, $34
                .BYTE $33, $31, $36, $E9, $31, $34, $E5, $33, $F2, $36
                .BYTE $E5, 0, $33, 0, $31, $EB, 0, $EB, $80, $61, $FC
                .BYTE $25, $E5, $25, $2C, $25, $E3, $2A, $E5, $28, $E3
                .BYTE $27, $E5, $25, $27, $25, $E5, $25, $2C, $25, $E3
                .BYTE $2A, $E5, $28, $E3, $27, $E5, $25, $27, $21, $E5
                .BYTE $21, $28, $21, $E3, $27, $E5, $25, $E3, $23, $E5
                .BYTE $21, $23, $21, $E5, $21, $28, $21, $E3, $27, $E5
                .BYTE $25, $E3, $23, $E5, $21, $23, $1E, $E5, $1E, $25
                .BYTE $1E, $E3, $23, $E5, $21, $E3, $20, $E5, $1E, $20
                .BYTE $1E, $E5, $1E, $25, $1E, $E3, $23, $E5, $21, $E3
                .BYTE $20, $E5, $1E, $20, $25, $E5, $25, $2C, $25, $E3
                .BYTE $2A, $E5, $28, $E3, $27, $E5, $25, $27, $25, $E5
                .BYTE $25, $2C, $25, $E3, $2A, $E5, $28, $E3, $27, $E5
                .BYTE $25, $27, $80, $BF, $FC, $19, $E7, $25, $20, $E5
                .BYTE $25, $27, $2C, $19, $E7, $25, $2C, $E5, $28, $25
                .BYTE $20, $15, $E7, $21, $1C, $E5, $21, $25, $28, $15
                .BYTE $E7, $21, $25, $E5, $23, $21, $20, $12, $E7, $1E
                .BYTE $15, $E5, $19, $21, $25, $12, $E7, $1E, $25, $E5
                .BYTE $23, $21, $20, $19, $E7, $25, $20, $E5, $25, $28
                .BYTE $2C, $19, $E7, $25, $31, $E5, $2C, $28, $25, $80
                .BYTE $32, $FD
byte_FD75:      .BYTE 3, 0, 1, $3E, 7, 0, $F8, 2, 0, $94, $FD, 1, 2, $BE
                                        ; DATA XREF: ROM:F140↑o
                .BYTE 2, 0, 4, 2, 1, $1D, $FE, 2, 1, $BF, 2, 0, $10, 2
                .BYTE 2, $36, $FE, $31, $E5, $2C, $2A, $29, $2A, $2C, $2A
                .BYTE $29, $2F, $E5, $2F, $E3, $2F, $2E, $E5, $2E, $E3
                .BYTE $2E, $2C, $E5, $2C, $E3, $E3, $2A, $E5, $2A, $E3
                .BYTE $2A, $31, $E5, $2C, $2A, $29, $2A, $2C, $2A, $29
                .BYTE $2F, $E5, $2F, $30, $30, $31, $31, $E3, $31, $35
                .BYTE $E5, $35, $E3, $35, $33, $E5, $33, $33, $33, $33
                .BYTE $35, $36, $35, $38, $E5, $38, $E3, $38, $36, $E5
                .BYTE $36, $E3, $36, $35, $E5, $35, $E3, $35, $33, $E5
                .BYTE $33, $E3, $33, $31, $E5, $31, $F2, 0, $E9, $2C, $E5
                .BYTE $2C, $F2, 0, $E9, $2F, $E5, $2F, $F2, 0, $E9, $2A
                .BYTE $E5, $2A, $F2, 0, $E9, $33, $E5, $31, $33, $35, $36
                .BYTE $35, $36, $38, $3B, $3A, $38, $36, $3A, $38, $36
                .BYTE $35, $33, $E5, $33, $F2, 0, $E9, $3F, $E5, $3F, $F2
                .BYTE 0, $E9, $80, $94, $FD, $8F, 2, $25, $E5, $25, $E3
                .BYTE $25, $25, $E5, $25, $E3, $25, $25, $E5, $25, $E3
                .BYTE $25, $25, $E5, $25, $E3, $25, $80, $1F, $FE, $8D
                .BYTE $60, 7, $19, $E5, $25, $19, $19, $20, $20, $23, $23
                .BYTE $19, $E5, $25, $19, $19, $20, $20, $1E, $1E, $92
                .BYTE $55, $FE, $81, $60, $39, $FE, $80, $36, $FE, 2, 0
                .BYTE $FE, 0, 2, $FE, 0
byte_FE5C:      .BYTE 1, 3, 1, $BF, 0, 0, $F8, 1, 1, $67, $FE, $93, 4
                                        ; DATA XREF: ROM:F142↑o
                .BYTE $20, $49, $E1, $30, $95, $6A, $FE, $82
byte_FE71:      .BYTE 3, 2, $41, $BF, 1, 0, 0, 1, 0, $90, $FE, 3, $41
                                        ; DATA XREF: ROM:F144↑o
                .BYTE $BF, 1, 0, $C, 1, 1, $AF, $FE, 4, $41, $BF, 0, 0
                .BYTE $24, 1, 2, $D6, $FE, $8D, $60, 3, $2C, $E5, $2E
                .BYTE $2F, $31, $33, $34, $33, $31, $81, $60, $93, $FE
                .BYTE $33, $31, $2F, $31, $2F, $2E, $2F, $2E, $2C, $2E
                .BYTE $2C, $2B, $2C, $E9, $86, $2C, $E5, $E5, $E5, $E5
                .BYTE $E5, $2E, $2F, $31, $8D, $60, 2, $33, $34, $33, $31
                .BYTE $2C, $2E, $2F, $31, $81, $60, $BB, $FE, $33, $34
                .BYTE $33, $31, $33, $31, $2F, $31, $2F, $2E, $2F, $2E
                .BYTE $2C, $E9, $82, 0, $EB, $E9, $8D, $60, $30, $38, $E3
                .BYTE $89, $FF, $81, $60, $DC, $FE, $89, 0, $14, $82

; =============== S U B R O U T I N E =======================================

; Activated after defeating each boss and level start

sub_FEE8:                               ; CODE XREF: sub_C090↑j
                                        ; sub_FEE8+26↓j
                LDA     byte_31C
                BEQ     loc_FEF8
                LDA     byte_3C4
                CMP     #$1B
                BEQ     loc_FF09
                CMP     #$1C
                BEQ     loc_FF09

loc_FEF8:                               ; CODE XREF: sub_FEE8+3↑j
                LDA     byte_31D
                BNE     loc_FEFE

locret_FEFD:                            ; CODE XREF: sub_FEE8+1F↓j
                RTS
; ---------------------------------------------------------------------------

loc_FEFE:                               ; CODE XREF: sub_FEE8+13↑j
                LDA     byte_3C5
                CMP     #8
                BEQ     loc_FF09
                CMP     #9
                BNE     locret_FEFD

loc_FF09:                               ; CODE XREF: sub_FEE8+A↑j
                                        ; sub_FEE8+E↑j ...
                LDY     #1
                JSR     sub_EA0B
                JMP     sub_FEE8        ; Activated after defeating each boss and level start
; End of function sub_FEE8



; Padding to the end of the bank
    org $fffa,255



    .WORD nmi_handler
    .WORD reset
    .WORD dummy_irq_handler
