temp_0 equ $0
temp_1 equ $1
byte_2 equ $2
byte_6 equ $6
unk_7 equ $7
byte_8 equ $8
last_bank_num equ $0f
byte_10 equ $10
byte_11 equ $11
byte_12 equ $12
byte_13 equ $13
byte_14 equ $14
byte_15 equ $15
byte_16 equ $16
byte_17 equ $17
byte_18 equ $18
byte_19 equ $19
byte_1A equ $1A
byte_1B equ $1B
byte_1C equ $1C
byte_1D equ $1D
byte_1E equ $1E
byte_1F equ $1F
byte_20 equ $20
byte_21 equ $21
byte_24 equ $24
byte_25 equ $25
byte_26 equ $26
byte_27 equ $27
byte_28 equ $28
byte_29 equ $29
byte_2A equ $2A
byte_2B equ $2B
byte_2C equ $2C
byte_2D equ $2D
byte_2E equ $2E
byte_2F equ $2F
game_mode equ $30                               ; 0x80 = demo mode
warp_sequence_counter equ $31
byte_32 equ $32
zanac_main_weapon_level equ $33                 ; 0...5, Super
pmove_direction equ $34                         ; On the Up/Down/Left/Right base, the direction of ship movement is selected (according to the table)
unk_36 equ $36
zanac_main_weapon_speed_y equ $37               ; Vertical speed of a regular Zanac shot
byte_39 equ $39
unk_3B equ $3B
unk_3C equ $3C
byte_40 equ $40
vblank_wait_flag equ $41                        ; Set (incremented) in the NMI handler. The application is waiting until this flag is 0 to know that the VBlank handler has not started its work yet
unk_42 equ $42
unk_44 equ $44
byte_45 equ $45
byte_46 equ $46
byte_47 equ $47
byte_48 equ $48
byte_49 equ $49
byte_4A equ $4A
byte_4B equ $4B
byte_4C equ $4C
byte_4D equ $4D
byte_4E equ $4E
byte_4F equ $4F
byte_50 equ $50
byte_51 equ $51
byte_52 equ $52
byte_53 equ $53
byte_54 equ $54
byte_55 equ $55
byte_56 equ $56
byte_5A equ $5A
unk_5B equ $5B
special_weapon_type equ $5C
byte_5D equ $5D
byte_5E equ $5E
unk_60 equ $60
byte_62 equ $62
byte_63 equ $63
unk_64 equ $64
byte_65 equ $65
byte_66 equ $66
byte_6C equ $6C
byte_6D equ $6D
byte_6E equ $6E
byte_6F equ $6F
byte_70 equ $70
byte_71 equ $71
byte_72 equ $72
byte_73 equ $73
byte_74 equ $74
byte_75 equ $75
byte_77 equ $77
byte_7A equ $7A
byte_7D equ $7D
last_randomize_result equ $7E               ; Used only in the `randomize` procedure
ppu_status_in_vblank equ $7F                ; The VBlank handler saves here the contents of the PPU_STATUS register
unk_80 equ $80
byte_83 equ $83
byte_84 equ $84
byte_85 equ $85
byte_86 equ $86
unk_86 equ $86
byte_87 equ $87
byte_90 equ $90
current_level equ $91                       ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
byte_92 equ $92
byte_98 equ $98
byte_99 equ $99
byte_9A equ $9A
byte_9B equ $9B
byte_9C equ $9C
byte_9D equ $9D
byte_9E equ $9E
byte_9F equ $9F
byte_A0 equ $A0
byte_A1 equ $A1
byte_A2 equ $A2
byte_A3 equ $A3
byte_A4 equ $A4
unk_A5 equ $A5
byte_A8 equ $A8
byte_A9 equ $A9
byte_AA equ $AA
byte_AB equ $AB
byte_AC equ $AC
byte_AD equ $AD
byte_AE equ $AE
byte_AF equ $AF
byte_B0 equ $B0
byte_B1 equ $B1
byte_B2 equ $B2
byte_B3 equ $B3
byte_B4 equ $B4
byte_B5 equ $B5
byte_B6 equ $B6
byte_B7 equ $B7
byte_B8 equ $B8
byte_B9 equ $B9
byte_BA equ $BA
byte_BB equ $BB
unk_BD equ $BD
unk_BE equ $BE
byte_C0 equ $C0
byte_C1 equ $C1
byte_C2 equ $C2
byte_C3 equ $C3
byte_C4 equ $C4
byte_C5 equ $C5
byte_C6 equ $C6
byte_C7 equ $C7
byte_C8 equ $C8
byte_CB equ $CB
unk_CB equ $CB
byte_CD equ $CD
demo_counter_2 equ $CE
demo_counter_1 equ $CF
demo_button_update_counter equ $D0
demo_last_buttons equ $D1
demo_button_tab_pointer equ $D2
unk_D9 equ $D9
unk_DB equ $DB
unk_DC equ $DC
unk_DD equ $DD
unk_DF equ $DF
unk_E1 equ $E1
unk_E7 equ $E7
unk_E8 equ $E8
unk_F2 equ $F2
unk_F3 equ $F3
unk_F4 equ $F4
pad_test1 equ $F5
pad_test2 equ $F6
pad_buttons_1 equ $F7
pad_buttons_2 equ $F8
unk_F9 equ $F9
byte_FB equ $FB
ppu_scroll_2nd_value equ $FC
ppu_scroll_1st_value equ $FD
last_ppu_ctrl2 equ $FE
last_ppu_ctrl1 equ $FF


hard_reset_marker1 equ $102
hard_reset_marker2 equ $103
cram_update_mode equ $144
cram_new_palette equ $145
byte_171 equ $171
byte_174 equ $174
unk_175 equ $175
byte_176 equ $176
byte_177 equ $177
byte_178 equ $178
byte_179 equ $179
byte_17A equ $17A
; $17D [6 bytes] : Score  (BCD encoded)
byte_181 equ $181
; $183 [6 bytes] : Top Score  (BCD encoded)
byte_199 equ $199
reset_counter equ $19A
byte_19B equ $19B


; Addresses $200...$2FF are used by the OAM buffer (for OAM DMA). This is a typical allocation for almost all NES games

byte_200 equ $200
byte_201 equ $201
byte_202 equ $202
byte_203 equ $203
byte_210 equ $210
byte_211 equ $211
byte_212 equ $212
byte_213 equ $213
byte_214 equ $214
byte_215 equ $215
byte_216 equ $216
byte_217 equ $217

; Addresses $300...$3FF are used by the APU engine

byte_300 equ $300
byte_302 equ $302
byte_303 equ $303
byte_304 equ $304
byte_305 equ $305
apu_sound_length equ $306
byte_307 equ $307
byte_308 equ $308
byte_30C equ $30C
byte_310 equ $310
byte_314 equ $314
byte_315 equ $315
byte_317 equ $317
byte_318 equ $318
byte_319 equ $319
unk_31A equ $31A
byte_31C equ $31C
byte_31D equ $31D
byte_31F equ $31F
byte_3C4 equ $3C4
byte_3C5 equ $3C5

; The $400 addresses are used (apparently) by the Background engine (the part of the code that is responsible for rendering and scrolling the background = level)

; Addresses $500...$7FF are used by the game logic ("AI") and contains a large number of arrays of 0x1a elements ("Enemy Data" aka game objects data)

byte_528 equ $528       ; Type  (The most significant bit is used to show that the object is spawned)
byte_52C equ $52C
byte_542 equ $542       ; Y Position
byte_546 equ $546
byte_55C equ $55C       ; X Position
byte_560 equ $560
byte_576 equ $576       ; Sprite ID (The identifier of the meta sprite, for displaying the object)
byte_590 equ $590
byte_5AA equ $5AA
byte_5C4 equ $5C4
byte_5DE equ $5DE
byte_5F8 equ $5F8
byte_612 equ $612       ; Y Speed
byte_62C equ $62C
byte_646 equ $646       ; X Speed
byte_660 equ $660       ; Flags
unk_67A equ $67A
byte_6E2 equ $6E2       ; Chase Timeout; Timer (down counter) used to update the pursuit coordinates. After reaching 0, it is refreshed from Chase Initial Timeout.
byte_6FC equ $6FC       ; Chase Counter; Number of Zanac pursuit attempts (down counter).
byte_716 equ $716       ; Chase Initial Timeout; Copied to Chase Timeout after the countdown ends and sub_18024 is called.
byte_764 equ $764       ; Enemy Health (For example, Giza has 16 HP. Each shot at such a tough enemy is accompanied by a ringing sound)
byte_77E equ $77E
byte_782 equ $782
byte_798 equ $798
byte_7B2 equ $7B2
byte_7CC equ $7CC
unk_7FE equ $7FE