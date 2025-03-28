# Zanac Map

There are only 2 banks with code in the game (6 and 7). Bank 6 temporarily drops out of the address space during graphics loading, but in general we can say that the addresses are static (the caller usually knows that at the moment bank 6 contains the code).

Developers use all sorts of tricks like a shared RTS for multiple procedures, interleaving data and code (see for example switch_case, print_string), and "chopped" procedures that don't terminate but just "continue" into another procedure.
All this is quite usual for the NES, but because of this, the list of labeled procedures can change when it turns out that one procedure is actually 2, or a procedure was not immediately found in the mess.

Then just the contents of the left list from the IDA.

```
j_game_logic_processing	seg002	00008000	00000003			R	.	.	.	.	.	.
sub_18003	seg002	00008003	00000003			R	.	.	.	.	.	.
sub_18006	seg002	00008006	00000003			R	.	.	.	.	.	.
sub_18009	seg002	00008009	00000003			R	.	.	.	.	.	.
sub_1800C	seg002	0000800C	00000003			R	.	.	.	.	.	.
sub_1800F	seg002	0000800F	00000003			R	.	.	.	.	.	.
sub_18012	seg002	00008012	00000003			R	.	.	.	.	.	.
j_set_default_weapon	seg002	00008015	00000003			R	.	.	.	.	.	.
j_clear_main_menu_sprites	seg002	00008018	00000003			R	.	.	.	.	.	.
j_update_score_onscreen	seg002	0000801B	00000003			R	.	.	.	.	.	.
sub_1801E	seg002	0000801E	00000003			R	.	.	.	.	.	.
j_check_enemy_overflow	seg002	00008021	00000003			R	.	.	.	.	.	.
calculate_slope_and_speed	seg002	00008024	00000006			R	.	.	.	.	.	.
calculate_slope	seg002	0000802A	0000006D			R	.	.	.	.	.	.
set_speed_by_slope	seg002	00008097	0000008A	00000000	00000000	R	.	.	.	.	.	.
slope_div	seg002	0000818C	0000001B			R	.	.	.	.	.	.
sub_181A7	seg002	000081A7	0000003D			R	.	.	.	.	.	.
sub_181E4	seg002	000081E4	00000009			R	.	.	.	.	.	.
sub_181ED	seg002	000081ED	00000009			R	.	.	.	.	.	.
sub_181F6	seg002	000081F6	0000000E	00000000	00000000	R	.	.	.	.	.	.
sub_18204	seg002	00008204	0000002C			R	.	.	.	.	.	.
collision_detect_special_shot_and_zanac	seg002	00008230	00000020			R	.	.	.	.	.	.
collision_detect_all_shots	seg002	00008250	00000040			R	.	.	.	.	.	.
collision_detect_single	seg002	00008290	00000021	00000000	00000000	R	.	.	.	.	.	.
process_enemy_flags_sprites	seg002	000082B1	00000028			R	.	.	.	.	.	.
add_enemy_meta_sprite	seg002	000082D9	00000021			R	.	.	.	.	.	.
enemy_erase_and_continue	seg002	000082FA	00000004			R	.	.	.	.	.	.
enemy_erase	seg002	000082FE	00000024			R	.	.	.	.	.	.
process_y_speed	seg002	00008322	00000025			R	.	.	.	.	.	.
sub_18347	seg002	00008347	00000021			R	.	.	.	.	.	.
sub_18368	seg002	00008368	0000002A			R	.	.	.	.	.	.
sub_18392	seg002	00008392	0000002A			R	.	.	.	.	.	.
sub_183BC	seg002	000083BC	00000021			R	.	.	.	.	.	.
sub_183DD	seg002	000083DD	00000012			R	.	.	.	.	.	.
sub_183EF	seg002	000083EF	00000023			R	.	.	.	.	.	.
add_next_sprite	seg002	00008412	00000095			R	.	.	.	.	.	.
sub_184A7	seg002	000084A7	00000083			R	.	.	.	.	.	.
clear_main_menu_sprites	seg002	0000852A	00000011			R	.	.	.	.	.	.
update_score_onscreen	seg002	0000853B	00000050			R	.	.	.	.	.	.
update_player_lives	seg002	0000858B	00000043			R	.	.	.	.	.	.
add_sprite_from_zeropage	seg002	000085CE	0000000A			R	.	.	.	.	.	.
add_sprite	seg002	000085D8	00000027			R	.	.	.	.	.	.
add_more_sprites	seg002	000085FF	0000000D			R	.	.	.	.	.	.
add_next_sprite_from_zeropage	seg002	0000860C	0000000D			R	.	.	.	.	.	.
enemy_prng	seg002	0000872B	0000001B			R	.	.	.	.	.	.
spawn_top_random_x_position	seg002	00008746	00000024			R	.	.	.	.	.	.
sub_1876A	seg002	0000876A	00000014			R	.	.	.	.	.	.
sub_1877E	seg002	0000877E	0000000A			R	.	.	.	.	.	.
sub_18788	seg002	00008788	0000000E			R	.	.	.	.	.	.
sub_18796	seg002	00008796	00000009			R	.	.	.	.	.	.
sub_1879F	seg002	0000879F	00000007			R	.	.	.	.	.	.
sub_187A6	seg002	000087A6	00000007			R	.	.	.	.	.	.
game_logic_processing	seg002	000087C7	0000005E			R	.	.	.	.	.	.
check_enemy_overflow	seg002	00008825	00000010			R	.	.	.	.	.	.
sub_18835	seg002	00008835	0000012B	00000000	00000000	R	.	.	.	.	.	.
enemy_type_1_zanac	seg002	00008C68	0000003C	00000000	00000000	R	.	.	.	.	.	.
zanac_main_handler	seg002	00008CA4	00000185			R	.	.	.	.	.	.
sub_18E29	seg002	00008E29	0000004B			R	.	.	.	.	.	.
set_default_weapon	seg002	00008EA1	00000017			R	.	.	.	.	.	.
warp_sequence	seg002	00008ECA	000000AE			R	.	.	.	.	.	.
enemy_type_60	seg002	00008F78	000000A9			R	.	.	.	.	.	.
enemy_type_2_zanac_standard_shot	seg002	00009039	00000035			R	.	.	.	.	.	.
sub_1906E	seg002	0000906E	00000011			R	.	.	.	.	.	.
enemy_type_3	seg002	0000907F	0000005E			R	.	.	.	.	.	.
special_weapon_process	seg002	000090DD	00000005			R	.	.	.	.	.	.
sub_1913A	seg002	0000913A	00000013	00000000	00000000	R	.	.	.	.	.	.
sub_1914D	seg002	0000914D	0000001D			R	.	.	.	.	.	.
special_weapon_type_7	seg002	0000916A	00000008			R	.	.	.	.	.	.
special_weapon_type_1	seg002	00009172	00000029			R	.	.	.	.	.	.
special_weapon_type_0	seg002	0000919B	00000006			R	.	.	.	.	.	.
sub_191A1	seg002	000091A1	0000003C			R	.	.	.	.	.	.
sub_191E6	seg002	000091E6	00000005			R	.	.	.	.	.	.
special_weapon_type_2	seg002	000091EB	00000067			R	.	.	.	.	.	.
sub_1925C	seg002	0000925C	0000001C			R	.	.	.	.	.	.
special_weapon_type_3	seg002	00009278	00000079			R	.	.	.	.	.	.
sub_192F1	seg002	000092F1	0000003E			R	.	.	.	.	.	.
special_weapon_type_4	seg002	0000932F	0000002D			R	.	.	.	.	.	.
sub_1935C	seg002	0000935C	00000016	00000000	00000000	R	.	.	.	.	.	.
special_weapon_type_5	seg002	00009372	0000009E			R	.	.	.	.	.	.
sub_19410	seg002	00009410	0000001F	00000000	00000000	R	.	.	.	.	.	.
special_weapon_type_6	seg002	0000942F	00000006			R	.	.	.	.	.	.
enemy_type_19	seg002	00009435	0000000A			R	.	.	.	.	.	.
sub_1944F	seg002	0000944F	00000015			R	.	.	.	.	.	.
sub_19464	seg002	00009464	0000000B			R	.	.	.	.	.	.
sub_1946F	seg002	0000946F	0000001C			R	.	.	.	.	.	.
sub_1948B	seg002	0000948B	00000008			R	.	.	.	.	.	.
sub_19493	seg002	00009493	000000E5	00000000	00000000	R	.	.	.	.	.	.
enemy_type_68	seg002	0000958E	0000004B			R	.	.	.	.	.	.
sub_19615	seg002	00009615	00000006			R	.	.	.	.	.	.
enemy_type_4_5_6_boxes	seg002	0000961B	000000C1	00000000	00000000	R	.	.	.	.	.	.
enemy_type_63	seg002	000096E8	0000000B			R	.	.	.	.	.	.
sub_196F3	seg002	000096F3	0000004C			R	.	.	.	.	.	.
enemy_type_7	seg002	0000973F	00000075	00000000	00000000	R	.	.	.	.	.	.
sub_197B4	seg002	000097B4	00000006			R	.	.	.	.	.	.
enemy_type_8	seg002	000097C7	00000008			R	.	.	.	.	.	.
sub_197CF	seg002	000097CF	00000024			R	.	.	.	.	.	.
sub_197F3	seg002	000097F3	0000000A			R	.	.	.	.	.	.
enemy_type_9	seg002	000097FD	00000020			R	.	.	.	.	.	.
sub_1981D	seg002	0000981D	0000000C			R	.	.	.	.	.	.
enemy_type_10_blue_duster	seg002	00009829	00000037			R	.	.	.	.	.	.
enemy_type_69	seg002	00009860	00000066			R	.	.	.	.	.	.
enemy_type_11	seg002	000098C6	00000021			R	.	.	.	.	.	.
sub_19918	seg002	00009918	00000024			R	.	.	.	.	.	.
enemy_type_12_13	seg002	0000993C	00000094			R	.	.	.	.	.	.
enemy_type_14	seg002	000099F6	00000101			R	.	.	.	.	.	.
enemy_type_15	seg002	00009AF7	00000093			R	.	.	.	.	.	.
enemy_type_48_maseru	seg002	00009B8A	0000009D	00000000	00000000	R	.	.	.	.	.	.
enemy_type_51_girevu	seg002	00009C2B	0000000C	00000000	00000000	R	.	.	.	.	.	.
enemy_type_49_50_52	seg002	00009C37	00000060			R	.	.	.	.	.	.
capital_ships_health_check	seg002	00009C97	00000014			R	.	.	.	.	.	.
sub_19CAB	seg002	00009CAB	0000003B			R	.	.	.	.	.	.
set_capital_ship_parameters	seg002	00009CE6	00000029			R	.	.	.	.	.	.
sub_19D23	seg002	00009D23	00000057			R	.	.	.	.	.	.
enemy_type_24	seg002	00009D7A	00000035			R	.	.	.	.	.	.
sub_19DAF	seg002	00009DAF	00000077			R	.	.	.	.	.	.
sub_19E2A	seg002	00009E2A	0000000E			R	.	.	.	.	.	.
enemy_type_25_carla	seg002	00009E38	00000061			R	.	.	.	.	.	.
enemy_type_16	seg002	00009E99	0000001B	00000000	00000000	R	.	.	.	.	.	.
sub_19EB4	seg002	00009EB4	00000095			R	.	.	.	.	.	.
enemy_type_17	seg002	00009F49	00000023			R	.	.	.	.	.	.
enemy_type_18	seg002	00009F6C	00000050			R	.	.	.	.	.	.
enemy_type_22	seg002	00009FD6	0000007E			R	.	.	.	.	.	.
sub_1A05E	seg002	0000A05E	0000002C			R	.	.	.	.	.	.
enemy_type_23	seg002	0000A08A	00000031			R	.	.	.	.	.	.
enemy_type_26	seg002	0000A0BB	0000003E	00000000	00000000	R	.	.	.	.	.	.
sub_1A0F9	seg002	0000A0F9	00000017			R	.	.	.	.	.	.
sub_1A110	seg002	0000A110	00000055			R	.	.	.	.	.	.
enemy_type_27	seg002	0000A16B	00000035			R	.	.	.	.	.	.
enemy_type_28	seg002	0000A1A0	00000029			R	.	.	.	.	.	.
enemy_type_29_effine_T4	seg002	0000A1C9	00000095			R	.	.	.	.	.	.
sub_1A264	seg002	0000A264	0000004D			R	.	.	.	.	.	.
decrease_enemy_health	seg002	0000A2B1	0000001A			R	.	.	.	.	.	.
sub_1A2CB	seg002	0000A2CB	00000010			R	.	.	.	.	.	.
enemy_type_30_32	seg002	0000A2DB	000000F0	00000000	00000000	R	.	.	.	.	.	.
enemy_type_31_33	seg002	0000A3CB	0000000C			R	.	.	.	.	.	.
sub_1A3D7	seg002	0000A3D7	00000015			R	.	.	.	.	.	.
enemy_type_34_65_66	seg002	0000A3EC	000000F3			R	.	.	.	.	.	.
enemy_type_46_47	seg002	0000A509	000000F8			R	.	.	.	.	.	.
enemy_type_56	seg002	0000A609	0000003C			R	.	.	.	.	.	.
enemy_type_57	seg002	0000A645	00000060			R	.	.	.	.	.	.
enemy_type_58	seg002	0000A6A5	0000001C			R	.	.	.	.	.	.
enemy_type_59	seg002	0000A6C1	0000000E			R	.	.	.	.	.	.
enemy_type_64_random	seg002	0000A6CF	00000022			R	.	.	.	.	.	.
enemy_type_36_giza	seg002	0000A6F1	00000059			R	.	.	.	.	.	.
enemy_type_44_yellow_duster	seg002	0000A74C	0000002D			R	.	.	.	.	.	.
enemy_type_61_sart	seg002	0000A779	000000BD			R	.	.	.	.	.	.
enemy_type_67_valkyrie	seg002	0000A836	0000009B			R	.	.	.	.	.	.
update_chase_counters	seg002	0000A8D1	00000019			R	.	.	.	.	.	.
enemy_type_35_small_explosion	seg002	0000A8EA	00000094			R	.	.	.	.	.	.
enemy_type_91_big_explosion	seg002	0000A985	000000A1			R	.	.	.	.	.	.
enemy_type_37	seg002	0000AA62	00000005			R	.	.	.	.	.	.
sub_1AA67	seg002	0000AA67	00000008	00000000	00000000	R	.	.	.	.	.	.
sub_1AA6F	seg002	0000AA6F	00000017			R	.	.	.	.	.	.
sub_1AA86	seg002	0000AA86	00000006			R	.	.	.	.	.	.
enemy_type_38_lead	seg002	0000AA8C	00000005			R	.	.	.	.	.	.
sub_1AA91	seg002	0000AA91	0000000E			R	.	.	.	.	.	.
enemy_type_39	seg002	0000AA9F	00000009			R	.	.	.	.	.	.
enemy_type_41	seg002	0000AAA8	000000E8			R	.	.	.	.	.	.
enemy_type_42	seg002	0000ABA2	00000008			R	.	.	.	.	.	.
enemy_type_43	seg002	0000ABAA	00000005			R	.	.	.	.	.	.
sub_1ABAF	seg002	0000ABAF	00000039			R	.	.	.	.	.	.
enemy_type_45	seg002	0000ABE8	0000005C			R	.	.	.	.	.	.
enemy_type_21	seg002	0000AC48	0000003B			R	.	.	.	.	.	.
enemy_type_20	seg002	0000ACA3	0000003B			R	.	.	.	.	.	.
enemy_type_53	seg002	0000ACDE	00000047			R	.	.	.	.	.	.
enemy_type_54	seg002	0000AD25	00000011			R	.	.	.	.	.	.
enemy_type_55_fairy	seg002	0000AD36	000000E4			R	.	.	.	.	.	.
enemy_type_62	seg002	0000AE1A	0000007F			R	.	.	.	.	.	.
enemy_type_82_weapon_distro	seg002	0000AE99	00000028	00000000	00000000	R	.	.	.	.	.	.
enemy_type_70_71_81_88_89	seg002	0000AEC1	000000D9			R	.	.	.	.	.	.
sub_1B09A	seg002	0000B09A	00000010			R	.	.	.	.	.	.
sub_1B0AA	seg002	0000B0AA	000000BF			R	.	.	.	.	.	.
sub_1B169	seg002	0000B169	00000064			R	.	.	.	.	.	.
sub_1B1CD	seg002	0000B1CD	00000003			R	.	.	.	.	.	.
sub_1B1D0	seg002	0000B1D0	00000012			R	.	.	.	.	.	.
sub_1B1E2	seg002	0000B1E2	00000006			R	.	.	.	.	.	.
sub_1B1E8	seg002	0000B1E8	00000003			R	.	.	.	.	.	.
sub_1B1EB	seg002	0000B1EB	0000000F			R	.	.	.	.	.	.
sub_1B1FA	seg002	0000B1FA	00000014			R	.	.	.	.	.	.
sub_1B20E	seg002	0000B20E	00000052			R	.	.	.	.	.	.
enemy_type_72	seg002	0000B268	00000085			R	.	.	.	.	.	.
sub_1B2ED	seg002	0000B2ED	00000041			R	.	.	.	.	.	.
sub_1B32E	seg002	0000B32E	00000009			R	.	.	.	.	.	.
enemy_type_79	seg002	0000B337	0000000B			R	.	.	.	.	.	.
enemy_type_73_74_75_76_77_78	seg002	0000B342	00000013			R	.	.	.	.	.	.
fortress_core_appears	seg002	0000B355	00000049	00000000	00000000	R	.	.	.	.	.	.
sub_1B39E	seg002	0000B39E	000000AD			R	.	.	.	.	.	.
sub_1B44B	seg002	0000B44B	0000001C			R	.	.	.	.	.	.
sub_1B467	seg002	0000B467	0000001B			R	.	.	.	.	.	.
sub_1B482	seg002	0000B482	0000000F			R	.	.	.	.	.	.
sub_1B491	seg002	0000B491	0000000E			R	.	.	.	.	.	.
sub_1B49F	seg002	0000B49F	00000036			R	.	.	.	.	.	.
sub_1B4D5	seg002	0000B4D5	00000032			R	.	.	.	.	.	.
sub_1B507	seg002	0000B507	0000002C			R	.	.	.	.	.	.
sub_1B533	seg002	0000B533	00000009			R	.	.	.	.	.	.
sub_1B54A	seg002	0000B54A	0000002C	00000000	00000000	R	.	.	.	.	.	.
sub_1B576	seg002	0000B576	0000001C			R	.	.	.	.	.	.
sub_1B592	seg002	0000B592	00000008			R	.	.	.	.	.	.
sub_1B59A	seg002	0000B59A	00000016			R	.	.	.	.	.	.
sub_1B5B0	seg002	0000B5B0	0000000E	00000000	00000000	R	.	.	.	.	.	.
sub_1B5BE	seg002	0000B5BE	00000011			R	.	.	.	.	.	.
sub_1B5CF	seg002	0000B5CF	00000026			R	.	.	.	.	.	.
sub_1B611	seg002	0000B611	00000020			R	.	.	.	.	.	.
spawn_projectile	seg002	0000B631	00000015			R	.	.	.	.	.	.
sub_1B667	seg002	0000B667	00000034			R	.	.	.	.	.	.
enemy_type_80	seg002	0000B69B	0000001B			R	.	.	.	.	.	.
enemy_type_83	seg002	0000B6B6	00000076			R	.	.	.	.	.	.
enemy_type_84_85_86_87	seg002	0000B72C	00000088			R	.	.	.	.	.	.
enemy_type_90	seg002	0000B7B8	000000BB			R	.	.	.	.	.	.
sub_1B87F	seg002	0000B87F	00000024			R	.	.	.	.	.	.
sub_1B8A3	seg002	0000B8A3	0000000A			R	.	.	.	.	.	.
sub_1B8E5	seg002	0000B8E5	00000247	00000000	00000000	R	.	.	.	.	.	.
sub_1BB56	seg002	0000BB56	000000AE			R	.	.	.	.	.	.
sub_1BC04	seg002	0000BC04	0000004A			R	.	.	.	.	.	.
sub_1BC4E	seg002	0000BC4E	00000029			R	.	.	.	.	.	.
sub_1BCE0	seg002	0000BCE0	000000D9			R	.	.	.	.	.	.
sub_1BDC9	seg002	0000BDC9	0000002C			R	.	.	.	.	.	.
sub_1BE08	seg002	0000BE08	00000039			R	.	.	.	.	.	.
j_reset	ROM	0000C000	00000003			R	.	.	.	.	.	.
j_switch_bank	ROM	0000C003	00000003			R	.	.	.	.	.	.
j_far_call	ROM	0000C006	00000003			R	.	.	.	.	.	.
j_ppu_nmi_disable	ROM	0000C009	00000003			R	.	.	.	.	.	.
j_ppu_nmi_enable	ROM	0000C00C	00000003			R	.	.	.	.	.	.
j_ppu_disable_all	ROM	0000C00F	00000003			R	.	.	.	.	.	.
j_ppu_enable_all	ROM	0000C012	00000003			R	.	.	.	.	.	.
j_clear_oam_buffer	ROM	0000C015	00000003			R	.	.	.	.	.	.
j_clear_screen	ROM	0000C018	00000003			R	.	.	.	.	.	.
sub_C01B	ROM	0000C01B	00000003			R	.	.	.	.	.	.
j_pad_read	ROM	0000C01E	00000003			R	.	.	.	.	.	.
j_all_enemies_destroyed	ROM	0000C021	00000003			R	.	.	.	.	.	.
sub_C024	ROM	0000C024	00000003			R	.	.	.	.	.	.
sub_C027	ROM	0000C027	00000003			R	.	.	.	.	.	.
sub_C02A	ROM	0000C02A	00000003			R	.	.	.	.	.	.
j_handle_pad_buttons	ROM	0000C02D	00000003			R	.	.	.	.	.	.
j_apu_play	ROM	0000C030	00000003			R	.	.	.	.	.	.
sub_C033	ROM	0000C033	00000003			R	.	.	.	.	.	.
j_ppu_update_background	ROM	0000C036	00000003			R	.	.	.	.	.	.
sub_C039	ROM	0000C039	00000003			R	.	.	.	.	.	.
sub_C03C	ROM	0000C03C	00000003			R	.	.	.	.	.	.
sub_C03F	ROM	0000C03F	00000003			R	.	.	.	.	.	.
sub_C042	ROM	0000C042	00000003			R	.	.	.	.	.	.
sub_C045	ROM	0000C045	00000003			R	.	.	.	.	.	.
sub_C048	ROM	0000C048	00000003			R	.	.	.	.	.	.
sub_C04B	ROM	0000C04B	00000003			R	.	.	.	.	.	.
sub_C04E	ROM	0000C04E	00000003			R	.	.	.	.	.	.
sub_C051	ROM	0000C051	00000003			R	.	.	.	.	.	.
sub_C054	ROM	0000C054	00000003			R	.	.	.	.	.	.
sub_C057	ROM	0000C057	00000003			R	.	.	.	.	.	.
j_ppu_cram_load	ROM	0000C05A	00000003			R	.	.	.	.	.	.
sub_C05D	ROM	0000C05D	00000003			R	.	.	.	.	.	.
j_apu_reset	ROM	0000C060	00000003			R	.	.	.	.	.	.
j_apu_stop	ROM	0000C063	00000003			R	.	.	.	.	.	.
j_apu_play_0	ROM	0000C066	00000003			R	.	.	.	.	.	.
j_apu_update	ROM	0000C069	00000003			R	.	.	.	.	.	.
sub_C06C	ROM	0000C06C	00000003			R	.	.	.	.	.	.
sub_C06F	ROM	0000C06F	00000003			R	.	.	.	.	.	.
sub_C072	ROM	0000C072	00000003			R	.	.	.	.	.	.
j_switch_case	ROM	0000C075	00000003			R	.	.	.	.	.	.
j_get_random_enemy	ROM	0000C078	00000003			R	.	.	.	.	.	.
sub_C07B	ROM	0000C07B	00000003			R	.	.	.	.	.	.
j_update_music_after_fortress_destroy	ROM	0000C07E	00000003			R	.	.	.	.	.	.
sub_C081	ROM	0000C081	00000003			R	.	.	.	.	.	.
sub_C084	ROM	0000C084	00000003			R	.	.	.	.	.	.
sub_C087	ROM	0000C087	00000003			R	.	.	.	.	.	.
sub_C08A	ROM	0000C08A	00000003			R	.	.	.	.	.	.
sub_C08D	ROM	0000C08D	00000003			R	.	.	.	.	.	.
sub_C090	ROM	0000C090	00000003			R	.	.	.	.	.	.
sub_C6AB	ROM	0000C6AB	0000000A			R	.	.	.	.	.	.
sub_C6B5	ROM	0000C6B5	00000015			R	.	.	.	.	.	.
sub_C6CA	ROM	0000C6CA	00000009			R	.	.	.	.	.	.
stack_call_return	ROM	0000C6D3	00000007			R	.	.	.	.	.	.
print_string	ROM	0000C6DA	00000010			R	.	.	.	.	.	.
sub_C6EA	ROM	0000C6EA	0000003B			R	.	.	.	.	.	.
sub_C725	ROM	0000C725	00000043			R	.	.	.	.	.	.
clear_screen	ROM	0000C768	0000003B			R	.	.	.	.	.	.
ppu_update_scroll_regs	ROM	0000C7A3	00000010			R	.	.	.	.	.	.
mem_page_fill	ROM	0000C7B3	00000013			R	.	.	.	.	.	.
switch_case	ROM	0000C7C6	00000012			R	.	.	.	.	.	.
sub_C7D8	ROM	0000C7D8	00000013			R	.	.	.	.	.	.
sub_C7EB	ROM	0000C7EB	0000000D			R	.	.	.	.	.	.
sub_C7F8	ROM	0000C7F8	0000000F			R	.	.	.	.	.	.
sub_C807	ROM	0000C807	0000000E			R	.	.	.	.	.	.
switch_bank	ROM	0000C815	0000000F			R	.	.	.	.	.	.
switch_bank_0_unused	ROM	0000C824	0000000D			R	.	.	.	.	.	.
switch_bank_1_unused	ROM	0000C831	0000000D			R	.	.	.	.	.	.
switch_bank_2_unused	ROM	0000C83E	0000000D			R	.	.	.	.	.	.
switch_bank_3_unused	ROM	0000C84B	0000000D			R	.	.	.	.	.	.
switch_bank_4	ROM	0000C858	0000000D			R	.	.	.	.	.	.
switch_bank_5	ROM	0000C865	0000000D			R	.	.	.	.	.	.
switch_bank_6	ROM	0000C872	0000000D			R	.	.	.	.	.	.
far_call	ROM	0000C87F	00000021			R	.	.	.	.	.	.
far_call_prepare	ROM	0000C8A0	0000000C			R	.	.	.	.	.	.
far_call_restore	ROM	0000C8AC	0000000E			R	.	.	.	.	.	.
sub_C8BA	ROM	0000C8BA	00000025			R	.	.	.	.	.	.
sub_C8DF	ROM	0000C8DF	00000049			R	.	.	.	.	.	.
enemy_erase_2	ROM	0000C938	00000024			R	.	.	.	.	.	.
sub_C95C	ROM	0000C95C	00000029			R	.	.	.	.	.	.
sub_C985	ROM	0000C985	0000008A			R	.	.	.	.	.	.
sub_CA0F	ROM	0000CA0F	00000003	00000000	00000000	R	.	.	.	.	.	.
sub_CA12	ROM	0000CA12	00000085			R	.	.	.	.	.	.
sub_CA97	ROM	0000CA97	00000006			R	.	.	.	.	.	.
ppu_cram_load	ROM	0000CA9D	00000015			R	.	.	.	.	.	.
randomize	ROM	0000CAB2	0000001B			R	.	.	.	.	.	.
ppu_disable_all	ROM	0000CACD	0000000A			R	.	.	.	.	.	.
ppu_set_control2	ROM	0000CAD7	00000006			R	.	.	.	.	.	.
ppu_enable_picture	ROM	0000CADD	0000000A			R	.	.	.	.	.	.
ppu_nmi_disable	ROM	0000CAE7	00000004			R	.	.	.	.	.	.
ppu_set_control1	ROM	0000CAEB	00000006			R	.	.	.	.	.	.
ppu_enable_all	ROM	0000CAF1	00000006			R	.	.	.	.	.	.
ppu_nmi_enable	ROM	0000CAF7	0000000A			R	.	.	.	.	.	.
clear_oam_buffer	ROM	0000CB01	00000009			R	.	.	.	.	.	.
pad_read	ROM	0000CB0A	00000036			R	.	.	.	.	.	.
sub_CB40	ROM	0000CB40	0000000B			R	.	.	.	.	.	.
start_game_check	ROM	0000CB4B	0000010C			R	.	.	.	.	.	.
sub_CC57	ROM	0000CC57	00000056			R	.	.	.	.	.	.
clear_oam_buffer_unused	ROM	0000CCD4	00000004			R	.	.	.	.	.	.
sub_CCD8	ROM	0000CCD8	0000000B			R	.	.	.	.	.	.
vsync	ROM	0000CCE3	00000013			R	.	.	.	.	.	.
handle_pad_buttons	ROM	0000CCF6	00000045			R	.	.	.	.	.	.
sub_CD6B	ROM	0000CD6B	00000107			R	.	.	.	.	.	.
sub_CE72	ROM	0000CE72	0000001B			R	.	.	.	.	.	.
nmi_handler	ROM	0000CE8D	00000037			R	.	.	.	.	.	.
dummy_irq_handler	ROM	0000CEC4	00000001			R	.	.	.	.	.	.
ppu_update_background	ROM	0000CEC5	0000016C			R	.	.	.	.	.	.
ppu_cram_update	ROM	0000D031	00000073			R	.	.	.	.	.	.
sub_D0A4	ROM	0000D0A4	0000001B			R	.	.	.	.	.	.
sub_D0BF	ROM	0000D0BF	00000049			R	.	.	.	.	.	.
check_high_score_reached	ROM	0000D108	00000024			R	.	.	.	.	.	.
sub_D12C	ROM	0000D12C	00000060			R	.	.	.	.	.	.
sub_D18C	ROM	0000D18C	00000007	00000000	00000000	R	.	.	.	.	.	.
sub_D193	ROM	0000D193	00000013			R	.	.	.	.	.	.
sub_D1A6	ROM	0000D1A6	0000003B			R	.	.	.	.	.	.
sub_D2C0	ROM	0000D2C0	00000004			R	.	.	.	.	.	.
sub_D2C4	ROM	0000D2C4	00000004			R	.	.	.	.	.	.
clear_first_last_sprites	ROM	0000D2C8	0000001C			R	.	.	.	.	.	.
set_new_high_score	ROM	0000D2E4	00000019			R	.	.	.	.	.	.
sub_D2FD	ROM	0000D2FD	0000004A			R	.	.	.	.	.	.
sub_D347	ROM	0000D347	00000061			R	.	.	.	.	.	.
sub_D3A8	ROM	0000D3A8	0000004C			R	.	.	.	.	.	.
sub_D3F4	ROM	0000D3F4	0000000B			R	.	.	.	.	.	.
sub_D3FF	ROM	0000D3FF	00000064			R	.	.	.	.	.	.
sub_D463	ROM	0000D463	0000000F			R	.	.	.	.	.	.
sub_D472	ROM	0000D472	0000000E			R	.	.	.	.	.	.
sub_D480	ROM	0000D480	0000007A	00000000	00000000	R	.	.	.	.	.	.
sub_D4FA	ROM	0000D4FA	0000000C			R	.	.	.	.	.	.
sub_D510	ROM	0000D510	0000000A	00000000	00000000	R	.	.	.	.	.	.
sub_D51A	ROM	0000D51A	0000000E			R	.	.	.	.	.	.
sub_D528	ROM	0000D528	00000025			R	.	.	.	.	.	.
sub_D54D	ROM	0000D54D	0000000C	00000000	00000000	R	.	.	.	.	.	.
sub_D559	ROM	0000D559	00000012			R	.	.	.	.	.	.
sub_D56B	ROM	0000D56B	00000040	00000000	00000000	R	.	.	.	.	.	.
sub_D5AB	ROM	0000D5AB	00000016			R	.	.	.	.	.	.
sub_D5C1	ROM	0000D5C1	00000016			R	.	.	.	.	.	.
sub_D5D7	ROM	0000D5D7	00000048			R	.	.	.	.	.	.
get_random_enemy	ROM	0000D69B	00000004			R	.	.	.	.	.	.
sub_D69F	ROM	0000D69F	00000010			R	.	.	.	.	.	.
sub_D6AF	ROM	0000D6AF	00000030			R	.	.	.	.	.	.
sub_D6DF	ROM	0000D6DF	0000006C			R	.	.	.	.	.	.
sub_D753	ROM	0000D753	00000028			R	.	.	.	.	.	.
sub_D77B	ROM	0000D77B	00000014			R	.	.	.	.	.	.
sub_D78F	ROM	0000D78F	00000016			R	.	.	.	.	.	.
set_nametable_addr_after_scroll	ROM	0000D7A5	00000018			R	.	.	.	.	.	.
sub_D7E3	ROM	0000D7E3	00000041			R	.	.	.	.	.	.
sub_D824	ROM	0000D824	00000026	00000000	00000000	R	.	.	.	.	.	.
sub_D84A	ROM	0000D84A	00000005			R	.	.	.	.	.	.
sub_D84F	ROM	0000D84F	00000025			R	.	.	.	.	.	.
sub_D886	ROM	0000D886	0000000E			R	.	.	.	.	.	.
sub_D894	ROM	0000D894	0000000D			R	.	.	.	.	.	.
sub_D8A1	ROM	0000D8A1	0000001E			R	.	.	.	.	.	.
sub_D8BF	ROM	0000D8BF	0000001D			R	.	.	.	.	.	.
sub_D8DC	ROM	0000D8DC	00000044			R	.	.	.	.	.	.
sub_D920	ROM	0000D920	00000018			R	.	.	.	.	.	.
sub_D938	ROM	0000D938	00000002	00000000	00000000	R	.	.	.	.	.	.
sub_D93A	ROM	0000D93A	00000009	00000000	00000000	R	.	.	.	.	.	.
sub_D943	ROM	0000D943	0000009D			R	.	.	.	.	.	.
sub_D9E0	ROM	0000D9E0	00000036			R	.	.	.	.	.	.
sub_DA16	ROM	0000DA16	00000012	00000000	00000000	R	.	.	.	.	.	.
sub_DA28	ROM	0000DA28	0000000E			R	.	.	.	.	.	.
sub_DA36	ROM	0000DA36	0000018F			R	.	.	.	.	.	.
sub_DCC5	ROM	0000DCC5	00000006	00000000	00000000	R	.	.	.	.	.	.
sub_DCCB	ROM	0000DCCB	00000013			R	.	.	.	.	.	.
sub_DD26	ROM	0000DD26	00000078			R	.	.	.	.	.	.
sub_DD9E	ROM	0000DD9E	00000055			R	.	.	.	.	.	.
sub_DDF3	ROM	0000DDF3	00000006			R	.	.	.	.	.	.
sub_DDF9	ROM	0000DDF9	0000001B			R	.	.	.	.	.	.
sub_DE14	ROM	0000DE14	0000004B			R	.	.	.	.	.	.
sub_DE5F	ROM	0000DE5F	00000032	00000000	00000000	R	.	.	.	.	.	.
sub_DE91	ROM	0000DE91	00000062			R	.	.	.	.	.	.
sub_DEF3	ROM	0000DEF3	00000055			R	.	.	.	.	.	.
sub_DF48	ROM	0000DF48	00000020			R	.	.	.	.	.	.
reset_score	ROM	0000DF68	00000037			R	.	.	.	.	.	.
show_main_menu	ROM	0000DFBF	00000243			R	.	.	.	.	.	.
sub_E229	ROM	0000E229	0000002B			R	.	.	.	.	.	.
sub_E266	ROM	0000E266	0000002A			R	.	.	.	.	.	.
sub_E399	ROM	0000E399	0000001F			R	.	.	.	.	.	.
sub_E3F4	ROM	0000E3F4	00000045			R	.	.	.	.	.	.
sound_test_menu	ROM	0000E439	00000029			R	.	.	.	.	.	.
sub_E462	ROM	0000E462	0000006D			R	.	.	.	.	.	.
show_ending_screen	ROM	0000E4CF	00000140	00000000	00000000	R	.	.	.	.	.	.
sub_E60F	ROM	0000E60F	00000044			R	.	.	.	.	.	.
sub_E653	ROM	0000E653	0000001E			R	.	.	.	.	.	.
sub_E767	ROM	0000E767	00000005			R	.	.	.	.	.	.
sub_E76C	ROM	0000E76C	00000053			R	.	.	.	.	.	.
reset	ROM	0000E7BF	00000047	00000000	00000000	R	.	.	.	.	.	.
main_menu_loop	ROM	0000E806	00000018	00000000	00000000	R	.	.	.	.	.	.
sub_E81E	ROM	0000E81E	0000008C			R	.	.	.	.	.	.
sub_E8AA	ROM	0000E8AA	0000001F			R	.	.	.	.	.	.
sub_E8C9	ROM	0000E8C9	00000070	00000000	00000000	R	.	.	.	.	.	.
level_completed_sequence	ROM	0000E939	0000005C			R	.	.	.	.	.	.
sub_E995	ROM	0000E995	0000000D			R	.	.	.	.	.	.
fade_palette_before_warp_another	ROM	0000E9A2	00000006			R	.	.	.	.	.	.
fade_palette_before_warp	ROM	0000E9A8	00000028			R	.	.	.	.	.	.
update_music_after_fortress_destroy	ROM	0000E9E0	0000001E			R	.	.	.	.	.	.
sub_EA0B	ROM	0000EA0B	0000002B			R	.	.	.	.	.	.
all_enemies_destroyed	ROM	0000EA36	00000007			R	.	.	.	.	.	.
j_apu_reset_0	ROM	0000EA3D	00000003			R	.	.	.	.	.	.
j_apu_stop_0	ROM	0000EA40	00000003			R	.	.	.	.	.	.
j_apu_play_1	ROM	0000EA43	00000003			R	.	.	.	.	.	.
j_apu_update_0	ROM	0000EA46	00000003			R	.	.	.	.	.	.
sub_EA49	ROM	0000EA49	00000003			R	.	.	.	.	.	.
sub_EA4C	ROM	0000EA4C	00000003			R	.	.	.	.	.	.
sub_EA4F	ROM	0000EA4F	00000003			R	.	.	.	.	.	.
apu_update	ROM	0000EA52	00000106			R	.	.	.	.	.	.
sub_EB58	ROM	0000EB58	00000022			R	.	.	.	.	.	.
sub_EB7A	ROM	0000EB7A	00000035			R	.	.	.	.	.	.
apu_fx_0	ROM	0000EBEB	0000000B			R	.	.	.	.	.	.
apu_fx_1	ROM	0000EBF6	0000000F			R	.	.	.	.	.	.
apu_fx_6	ROM	0000EC05	00000006	00000000	00000000	R	.	.	.	.	.	.
apu_fx_2	ROM	0000EC0B	00000005	00000000	00000000	R	.	.	.	.	.	.
apu_fx_5	ROM	0000EC10	00000007			R	.	.	.	.	.	.
apu_fx_3	ROM	0000EC17	00000004			R	.	.	.	.	.	.
apu_fx_8	ROM	0000EC1B	00000004			R	.	.	.	.	.	.
apu_fx_4	ROM	0000EC1F	00000009	00000000	00000000	R	.	.	.	.	.	.
apu_fx_29	ROM	0000EC28	00000007	00000000	00000000	R	.	.	.	.	.	.
sub_EC2F	ROM	0000EC2F	00000006			R	.	.	.	.	.	.
apu_fx_9	ROM	0000EC35	0000000A			R	.	.	.	.	.	.
apu_fx_13	ROM	0000EC3F	0000000C			R	.	.	.	.	.	.
apu_fx_16	ROM	0000EC4B	00000004			R	.	.	.	.	.	.
apu_fx_10	ROM	0000EC4F	0000001C			R	.	.	.	.	.	.
sub_EC6B	ROM	0000EC6B	00000009	00000000	00000000	R	.	.	.	.	.	.
apu_fx_7	ROM	0000EC74	00000004			R	.	.	.	.	.	.
apu_fx_17	ROM	0000EC78	00000007			R	.	.	.	.	.	.
apu_fx_19	ROM	0000EC7F	00000025			R	.	.	.	.	.	.
apu_fx_21	ROM	0000ECA4	0000000C			R	.	.	.	.	.	.
sub_ECB0	ROM	0000ECB0	00000002			R	.	.	.	.	.	.
apu_fx_18	ROM	0000ECB2	00000025			R	.	.	.	.	.	.
apu_fx_22	ROM	0000ECD7	0000001E			R	.	.	.	.	.	.
apu_fx_14_15	ROM	0000ECF5	00000013			R	.	.	.	.	.	.
sub_ED08	ROM	0000ED08	00000004			R	.	.	.	.	.	.
sub_ED0C	ROM	0000ED0C	00000004			R	.	.	.	.	.	.
sub_ED10	ROM	0000ED10	00000005			R	.	.	.	.	.	.
apu_fx_20	ROM	0000ED15	00000010			R	.	.	.	.	.	.
sub_ED25	ROM	0000ED25	0000000A			R	.	.	.	.	.	.
sub_ED2F	ROM	0000ED2F	00000004			R	.	.	.	.	.	.
sub_ED33	ROM	0000ED33	00000009			R	.	.	.	.	.	.
apu_fx_23_24_25	ROM	0000ED3C	00000008	00000000	00000000	R	.	.	.	.	.	.
apu_fx_12	ROM	0000ED44	00000002			R	.	.	.	.	.	.
apu_fx_28	ROM	0000ED46	0000000A			R	.	.	.	.	.	.
apu_fx_26_27	ROM	0000ED50	00000069			R	.	.	.	.	.	.
sub_EDB9	ROM	0000EDB9	00000043			R	.	.	.	.	.	.
sub_EDFC	ROM	0000EDFC	00000047			R	.	.	.	.	.	.
sub_EE43	ROM	0000EE43	0000002E			R	.	.	.	.	.	.
sub_EE71	ROM	0000EE71	00000023			R	.	.	.	.	.	.
sub_EE94	ROM	0000EE94	00000016			R	.	.	.	.	.	.
sub_EEAA	ROM	0000EEAA	00000098			R	.	.	.	.	.	.
sub_EF4A	ROM	0000EF4A	00000027			R	.	.	.	.	.	.
apu_reset	ROM	0000EF71	00000008			R	.	.	.	.	.	.
apu_stop	ROM	0000EF79	00000017			R	.	.	.	.	.	.
apu_play	ROM	0000EF90	00000016			R	.	.	.	.	.	.
sub_EFA6	ROM	0000EFA6	00000076			R	.	.	.	.	.	.
sub_F0F4	ROM	0000F0F4	00000006			R	.	.	.	.	.	.
sub_F0FA	ROM	0000F0FA	00000004			R	.	.	.	.	.	.
sub_F146	ROM	0000F146	00000025			R	.	.	.	.	.	.
sub_FEE8	ROM	0000FEE8	00000029			R	.	.	.	.	.	.
```
