# `src/bank7.asm` — structural report for a 1:1 C translation

Zanac (USA), PRG bank 7 — the **fixed** 16 KiB bank, assembled with `org $c000,255`,
occupying `$C000`–`$FFFF` (16384 bytes, fully accounted for; see §0.3).

Everything below was derived from `src/bank7.asm` and `Build/bank7.lst` (read-only).
Line numbers are `bank7.asm` line numbers; they are **identical** to the DASM `.lst`
source-line column (the `.lst` is `bank7.asm` with the three `include` files expanded).
Addresses come from the `.lst` address column.
---

## Contents

* [0. Ground facts a code generator must assume](#0-ground-facts-a-code-generator-must-assume)
* [1. Complete label table (832 entries)](#1-complete-label-table-832-entries)
* [2. Tricky constructs](#2-tricky-constructs-exact-line-numbers)
  * 2.1 chopped procedures · 2.2 shared tails · 2.3 stack parameter passing / return-address reading
  * 2.4 computed control flow · 2.5 inline data · 2.6 hand-assembled bytes · 2.7 self-modifying code
  * 2.8 `TSX`/`TXS` · 2.9 other quirks
* [3. Call sites and calling conventions](#3-call-sites-and-calling-conventions)
* [4. Data tables](#4-data-tables)
* [5. Bank switching](#5-bank-switching)
* [6. Writes to memory outside `$0000`–`$07FF`](#6-writes-to-memory-outside-000007ff)
* [7. What a 1:1 C translation needs (checklist)](#7-what-a-11-c-translation-needs-checklist)
* [Appendix A — procedure table (256 procedures)](#appendix-a--procedure-table-256-procedures-address-order)
* [Appendix B — routines that end without a terminator](#appendix-b--routines-that-end-without-a-terminator-quick-index)

---

## 0. Ground facts a code generator must assume

### 0.1 ROM / mapper

* `Build/nes_header.bin` = `4E 45 53 1A 08 00 20 00 …` → 8 × 16 KiB PRG, CHR **RAM**, mapper **2 (UxROM)**.
* Mapper 2: `$8000`–`$BFFF` is the **switchable window**; `$C000`–`$FFFF` is hard-wired to the **last** bank = `bank7.bin`.
* Therefore *any* CPU write to `$8000`–`$FFFF` is a **bank-select write** — the address is irrelevant, the data byte is latched as the bank number.
  This is the entire bank-switching mechanism in this file (see §5).
* `bank4.asm`/`bank6.asm` assemble at `$8000`; `bank5.asm` is level data; `bank0..3` are zero-filled.

### 0.2 RAM map used by this file

* `$0000`–`$07FF` — 2 KiB RAM, incl. the zero page pointers listed in §5.3.
* `$0100`–`$01FF` — the **stack page**, but the game also uses a handful of page-1 arrays as data: `$0104`–`$010B`, `$0145`–`$014B` (`cram_new_palette`), `$016D`–`$0181`, `$0179`, `$017B`, `$0189`, `$0199`.
  Stack depth is kept small (`SP` is forced to `$FF` at reset/menu, §2.8), so these do not collide in practice.
* `$0200`–`$0217` — the OAM DMA page-2 buffer (`byte_200`…`byte_217`, dumped to `$4014` at line 2670).

### 0.3 Address coverage / order

* The file is **exactly** 16384 bytes: `$C000`–`$FFFF`. There are **no gaps and no overlaps**:
  every one of the 9513 listed source lines has a well-defined start address and the byte lengths tile the bank exactly.
  The two `$FF`-filled ranges are `$C093`–`$C0F7` (101 bytes, written as 101 explicit `.BYTE $FF` lines 467–567) and
  `$FF11`–`$FFF9` (233 bytes, produced by the fill value `255` of `org $fffa,255` at line 9513).
* **Source order == address order.** Every one of the 832 labels is at a strictly greater address than the previous one in source order (verified; 0 violations).
* **No duplicate labels / no aliased addresses** (832 labels, 832 distinct addresses).
* Only ordering *anomaly* worth noting: `stack_call_return` (`$C6D3`, line 1075) is emitted **before**
  `print_string` (`$C6DA`, line 1089) and `sub_C725` (`$C725`, line 1157), although it is their shared return thunk.
  Similarly `sub_C6EA` (`$C6EA`, line 1107) is emitted after its only callers.

---

## 1. Complete label table (832 entries)

`kind` values:

| kind | meaning |
|---|---|
| `code` | entry point of a real procedure (`; End of function` present, body is more than one `JMP`) |
| `thunk` | 3-byte `JMP xxxx` trampoline — the only instruction before `; End of function` |
| `data` | label on a `.BYTE`/`.WORD` line |
| `label_inside_code` | `loc_*` / `locret_*` branch target inside a procedure |

`size` = distance to the next label in address order (i.e. the byte span the label owns).

| # | name | address | kind | src line | size |
|---:|---|---|---|---:|---:|
| 1 | `j_reset` | `$C000` | thunk | 16 | 3 |
| 2 | `j_switch_bank` | `$C003` | thunk | 25 | 3 |
| 3 | `j_far_call` | `$C006` | thunk | 34 | 3 |
| 4 | `j_ppu_nmi_disable` | `$C009` | thunk | 43 | 3 |
| 5 | `j_ppu_nmi_enable` | `$C00C` | thunk | 52 | 3 |
| 6 | `j_ppu_disable_all` | `$C00F` | thunk | 61 | 3 |
| 7 | `j_ppu_enable_all` | `$C012` | thunk | 70 | 3 |
| 8 | `j_clear_oam_buffer` | `$C015` | thunk | 79 | 3 |
| 9 | `j_clear_screen` | `$C018` | thunk | 88 | 3 |
| 10 | `sub_C01B` | `$C01B` | thunk | 97 | 3 |
| 11 | `j_pad_read` | `$C01E` | thunk | 106 | 3 |
| 12 | `j_all_enemies_destroyed` | `$C021` | thunk | 115 | 3 |
| 13 | `sub_C024` | `$C024` | thunk | 125 | 3 |
| 14 | `sub_C027` | `$C027` | thunk | 135 | 3 |
| 15 | `sub_C02A` | `$C02A` | thunk | 144 | 3 |
| 16 | `j_handle_pad_buttons` | `$C02D` | thunk | 154 | 3 |
| 17 | `j_apu_play` | `$C030` | thunk | 163 | 3 |
| 18 | `sub_C033` | `$C033` | thunk | 173 | 3 |
| 19 | `j_ppu_update_background` | `$C036` | thunk | 182 | 3 |
| 20 | `sub_C039` | `$C039` | thunk | 191 | 3 |
| 21 | `sub_C03C` | `$C03C` | thunk | 200 | 3 |
| 22 | `sub_C03F` | `$C03F` | thunk | 210 | 3 |
| 23 | `sub_C042` | `$C042` | thunk | 220 | 3 |
| 24 | `sub_C045` | `$C045` | thunk | 229 | 3 |
| 25 | `sub_C048` | `$C048` | thunk | 238 | 3 |
| 26 | `sub_C04B` | `$C04B` | thunk | 248 | 3 |
| 27 | `sub_C04E` | `$C04E` | thunk | 258 | 3 |
| 28 | `sub_C051` | `$C051` | thunk | 267 | 3 |
| 29 | `sub_C054` | `$C054` | thunk | 276 | 3 |
| 30 | `sub_C057` | `$C057` | thunk | 285 | 3 |
| 31 | `j_ppu_cram_load` | `$C05A` | thunk | 295 | 3 |
| 32 | `sub_C05D` | `$C05D` | thunk | 304 | 3 |
| 33 | `j_apu_reset` | `$C060` | thunk | 313 | 3 |
| 34 | `j_apu_stop` | `$C063` | thunk | 322 | 3 |
| 35 | `j_apu_play_0` | `$C066` | thunk | 332 | 3 |
| 36 | `j_apu_update` | `$C069` | thunk | 341 | 3 |
| 37 | `sub_C06C` | `$C06C` | thunk | 350 | 3 |
| 38 | `sub_C06F` | `$C06F` | thunk | 359 | 3 |
| 39 | `sub_C072` | `$C072` | thunk | 368 | 3 |
| 40 | `j_switch_case` | `$C075` | thunk | 377 | 3 |
| 41 | `j_get_random_enemy` | `$C078` | thunk | 387 | 3 |
| 42 | `sub_C07B` | `$C07B` | thunk | 396 | 3 |
| 43 | `j_update_music_after_fortress_destroy` | `$C07E` | thunk | 405 | 3 |
| 44 | `sub_C081` | `$C081` | thunk | 415 | 3 |
| 45 | `sub_C084` | `$C084` | thunk | 424 | 3 |
| 46 | `sub_C087` | `$C087` | thunk | 433 | 3 |
| 47 | `sub_C08A` | `$C08A` | thunk | 442 | 3 |
| 48 | `sub_C08D` | `$C08D` | thunk | 452 | 3 |
| 49 | `sub_C090` | `$C090` | thunk | 461 | 104 |
| 50 | `bank_remap_table` | `$C0F8` | data | 568 | 8 |
| 51 | `randomize_tab` | `$C100` | data | 571 | 82 |
| 52 | `unk_C152` | `$C152` | data | 653 | 84 |
| 53 | `tab_C1A6` | `$C1A6` | data | 737 | 83 |
| 54 | `tab_C1F9` | `$C1F9` | data | 820 | 83 |
| 55 | `tab_C24C` | `$C24C` | data | 903 | 5 |
| 56 | `tab_C251` | `$C251` | data | 904 | 9 |
| 57 | `tab_C25A` | `$C25A` | data | 905 | 9 |
| 58 | `tab_C263` | `$C263` | data | 906 | 13 |
| 59 | `tab_C270` | `$C270` | data | 907 | 5 |
| 60 | `tab_C275` | `$C275` | data | 908 | 9 |
| 61 | `tab_C27E` | `$C27E` | data | 909 | 9 |
| 62 | `tab_C287` | `$C287` | data | 910 | 9 |
| 63 | `tab_C290` | `$C290` | data | 911 | 9 |
| 64 | `tab_C299` | `$C299` | data | 912 | 9 |
| 65 | `tab_C2A2` | `$C2A2` | data | 913 | 9 |
| 66 | `tab_C2AB` | `$C2AB` | data | 914 | 9 |
| 67 | `tab_C2B4` | `$C2B4` | data | 915 | 5 |
| 68 | `tab_C2B9` | `$C2B9` | data | 916 | 5 |
| 69 | `tab_C2BE` | `$C2BE` | data | 917 | 9 |
| 70 | `tab_C2C7` | `$C2C7` | data | 918 | 13 |
| 71 | `tab_C2D4` | `$C2D4` | data | 920 | 9 |
| 72 | `tab_C2DD` | `$C2DD` | data | 921 | 9 |
| 73 | `tab_C2E6` | `$C2E6` | data | 922 | 5 |
| 74 | `tab_C2EB` | `$C2EB` | data | 923 | 9 |
| 75 | `tab_C2F4` | `$C2F4` | data | 924 | 9 |
| 76 | `tab_C2FD` | `$C2FD` | data | 925 | 9 |
| 77 | `tab_C306` | `$C306` | data | 926 | 9 |
| 78 | `tab_C30F` | `$C30F` | data | 927 | 9 |
| 79 | `tab_C318` | `$C318` | data | 928 | 9 |
| 80 | `tab_C321` | `$C321` | data | 929 | 9 |
| 81 | `tab_C32A` | `$C32A` | data | 930 | 9 |
| 82 | `tab_C333` | `$C333` | data | 931 | 9 |
| 83 | `tab_C33C` | `$C33C` | data | 932 | 9 |
| 84 | `tab_C345` | `$C345` | data | 933 | 9 |
| 85 | `tab_C34E` | `$C34E` | data | 934 | 9 |
| 86 | `tab_C357` | `$C357` | data | 935 | 9 |
| 87 | `tab_C360` | `$C360` | data | 936 | 9 |
| 88 | `tab_C369` | `$C369` | data | 937 | 9 |
| 89 | `tab_C372` | `$C372` | data | 938 | 5 |
| 90 | `tab_C377` | `$C377` | data | 939 | 9 |
| 91 | `tab_C380` | `$C380` | data | 940 | 9 |
| 92 | `tab_C389` | `$C389` | data | 941 | 9 |
| 93 | `tab_C392` | `$C392` | data | 942 | 9 |
| 94 | `tab_C39B` | `$C39B` | data | 943 | 13 |
| 95 | `tab_C3A8` | `$C3A8` | data | 945 | 13 |
| 96 | `tab_C3B5` | `$C3B5` | data | 947 | 13 |
| 97 | `tab_C3C2` | `$C3C2` | data | 949 | 9 |
| 98 | `tab_C3CB` | `$C3CB` | data | 950 | 9 |
| 99 | `tab_C3D4` | `$C3D4` | data | 951 | 9 |
| 100 | `tab_C3DD` | `$C3DD` | data | 952 | 9 |
| 101 | `tab_C3E6` | `$C3E6` | data | 953 | 9 |
| 102 | `tab_C3EF` | `$C3EF` | data | 954 | 9 |
| 103 | `tab_C3F8` | `$C3F8` | data | 955 | 9 |
| 104 | `tab_C401` | `$C401` | data | 956 | 9 |
| 105 | `tab_C40A` | `$C40A` | data | 957 | 9 |
| 106 | `tab_C413` | `$C413` | data | 958 | 9 |
| 107 | `tab_C41C` | `$C41C` | data | 959 | 9 |
| 108 | `tab_C425` | `$C425` | data | 960 | 9 |
| 109 | `tab_C42E` | `$C42E` | data | 961 | 9 |
| 110 | `tab_C437` | `$C437` | data | 962 | 17 |
| 111 | `tab_C448` | `$C448` | data | 964 | 17 |
| 112 | `tab_C459` | `$C459` | data | 966 | 17 |
| 113 | `tab_C46A` | `$C46A` | data | 968 | 17 |
| 114 | `tab_C47B` | `$C47B` | data | 970 | 9 |
| 115 | `tab_C484` | `$C484` | data | 971 | 9 |
| 116 | `tab_C48D` | `$C48D` | data | 972 | 25 |
| 117 | `tab_C4A6` | `$C4A6` | data | 975 | 33 |
| 118 | `tab_C4C7` | `$C4C7` | data | 978 | 33 |
| 119 | `tab_C4E8` | `$C4E8` | data | 981 | 17 |
| 120 | `tab_C4F9` | `$C4F9` | data | 983 | 17 |
| 121 | `tab_C50A` | `$C50A` | data | 985 | 9 |
| 122 | `tab_C513` | `$C513` | data | 986 | 41 |
| 123 | `tab_C53C` | `$C53C` | data | 990 | 33 |
| 124 | `tab_C55D` | `$C55D` | data | 993 | 33 |
| 125 | `tab_C57E` | `$C57E` | data | 996 | 5 |
| 126 | `tab_C583` | `$C583` | data | 997 | 33 |
| 127 | `tab_C5A4` | `$C5A4` | data | 1000 | 37 |
| 128 | `tab_C5C9` | `$C5C9` | data | 1004 | 65 |
| 129 | `tab_C60A` | `$C60A` | data | 1010 | 9 |
| 130 | `tab_C613` | `$C613` | data | 1011 | 9 |
| 131 | `tab_C61C` | `$C61C` | data | 1012 | 9 |
| 132 | `tab_C625` | `$C625` | data | 1013 | 9 |
| 133 | `tab_C62E` | `$C62E` | data | 1014 | 9 |
| 134 | `tab_C637` | `$C637` | data | 1015 | 33 |
| 135 | `tab_C658` | `$C658` | data | 1018 | 33 |
| 136 | `tab_C679` | `$C679` | data | 1021 | 33 |
| 137 | `tab_C69A` | `$C69A` | data | 1024 | 17 |
| 138 | `sub_C6AB` | `$C6AB` | code | 1029 | 10 |
| 139 | `sub_C6B5` | `$C6B5` | code | 1042 | 21 |
| 140 | `sub_C6CA` | `$C6CA` | code | 1062 | 9 |
| 141 | `stack_call_return` | `$C6D3` | code | 1075 | 7 |
| 142 | `print_string` | `$C6DA` | code | 1089 | 6 |
| 143 | `loc_C6E0` | `$C6E0` | label_inside_code | 1096 | 10 |
| 144 | `sub_C6EA` | `$C6EA` | code | 1107 | 13 |
| 145 | `loc_C6F7` | `$C6F7` | label_inside_code | 1117 | 1 |
| 146 | `loc_C6F8` | `$C6F8` | label_inside_code | 1120 | 3 |
| 147 | `loc_C6FB` | `$C6FB` | label_inside_code | 1124 | 13 |
| 148 | `loc_C708` | `$C708` | label_inside_code | 1132 | 28 |
| 149 | `locret_C724` | `$C724` | label_inside_code | 1149 | 1 |
| 150 | `sub_C725` | `$C725` | code | 1157 | 49 |
| 151 | `loc_C756` | `$C756` | label_inside_code | 1184 | 4 |
| 152 | `loc_C75A` | `$C75A` | label_inside_code | 1188 | 14 |
| 153 | `clear_screen` | `$C768` | code | 1202 | 22 |
| 154 | `loc_C77E` | `$C77E` | label_inside_code | 1215 | 8 |
| 155 | `loc_C786` | `$C786` | label_inside_code | 1221 | 21 |
| 156 | `loc_C79B` | `$C79B` | label_inside_code | 1234 | 7 |
| 157 | `locret_C7A2` | `$C7A2` | label_inside_code | 1239 | 1 |
| 158 | `ppu_update_scroll_regs` | `$C7A3` | code | 1247 | 16 |
| 159 | `mem_page_fill` | `$C7B3` | code | 1263 | 6 |
| 160 | `loc_C7B9` | `$C7B9` | label_inside_code | 1269 | 13 |
| 161 | `switch_case` | `$C7C6` | code | 1285 | 18 |
| 162 | `sub_C7D8` | `$C7D8` | code | 1307 | 17 |
| 163 | `loc_C7E9` | `$C7E9` | label_inside_code | 1320 | 1 |
| 164 | `locret_C7EA` | `$C7EA` | label_inside_code | 1324 | 1 |
| 165 | `sub_C7EB` | `$C7EB` | code | 1332 | 13 |
| 166 | `sub_C7F8` | `$C7F8` | code | 1346 | 15 |
| 167 | `sub_C807` | `$C807` | code | 1363 | 14 |
| 168 | `switch_bank` | `$C815` | code | 1380 | 15 |
| 169 | `switch_bank_0_unused` | `$C824` | code | 1397 | 13 |
| 170 | `switch_bank_1_unused` | `$C831` | code | 1411 | 13 |
| 171 | `switch_bank_2_unused` | `$C83E` | code | 1425 | 13 |
| 172 | `switch_bank_3_unused` | `$C84B` | code | 1439 | 13 |
| 173 | `switch_bank_4` | `$C858` | code | 1453 | 13 |
| 174 | `switch_bank_5` | `$C865` | code | 1467 | 13 |
| 175 | `switch_bank_6` | `$C872` | code | 1482 | 13 |
| 176 | `far_call` | `$C87F` | code | 1498 | 33 |
| 177 | `far_call_prepare` | `$C8A0` | code | 1525 | 12 |
| 178 | `far_call_restore` | `$C8AC` | code | 1540 | 14 |
| 179 | `sub_C8BA` | `$C8BA` | code | 1554 | 2 |
| 180 | `loc_C8BC` | `$C8BC` | label_inside_code | 1559 | 24 |
| 181 | `loc_C8D4` | `$C8D4` | label_inside_code | 1571 | 11 |
| 182 | `sub_C8DF` | `$C8DF` | code | 1585 | 34 |
| 183 | `loc_C901` | `$C901` | label_inside_code | 1607 | 39 |
| 184 | `unk_C928` | `$C928` | data | 1632 | 8 |
| 185 | `unk_C930` | `$C930` | data | 1640 | 8 |
| 186 | `enemy_erase_2` | `$C938` | code | 1653 | 36 |
| 187 | `sub_C95C` | `$C95C` | code | 1673 | 41 |
| 188 | `sub_C985` | `$C985` | code | 1706 | 54 |
| 189 | `loc_C9BB` | `$C9BB` | label_inside_code | 1736 | 8 |
| 190 | `loc_C9C3` | `$C9C3` | label_inside_code | 1742 | 8 |
| 191 | `loc_C9CB` | `$C9CB` | label_inside_code | 1748 | 29 |
| 192 | `loc_C9E8` | `$C9E8` | label_inside_code | 1767 | 39 |
| 193 | `sub_CA0F` | `$CA0F` | code | 1791 | 3 |
| 194 | `sub_CA12` | `$CA12` | code | 1800 | 68 |
| 195 | `loc_CA56` | `$CA56` | label_inside_code | 1839 | 38 |
| 196 | `loc_CA7C` | `$CA7C` | label_inside_code | 1859 | 27 |
| 197 | `sub_CA97` | `$CA97` | code | 1877 | 6 |
| 198 | `ppu_cram_load` | `$CA9D` | code | 1888 | 2 |
| 199 | `loc_CA9F` | `$CA9F` | label_inside_code | 1892 | 19 |
| 200 | `randomize` | `$CAB2` | code | 1909 | 27 |
| 201 | `ppu_disable_all` | `$CACD` | code | 1932 | 10 |
| 202 | `ppu_set_control2` | `$CAD7` | code | 1945 | 6 |
| 203 | `ppu_enable_picture` | `$CADD` | code | 1957 | 10 |
| 204 | `ppu_nmi_disable` | `$CAE7` | code | 1969 | 4 |
| 205 | `ppu_set_control1` | `$CAEB` | code | 1979 | 6 |
| 206 | `ppu_enable_all` | `$CAF1` | code | 1989 | 6 |
| 207 | `ppu_nmi_enable` | `$CAF7` | code | 1999 | 10 |
| 208 | `clear_oam_buffer` | `$CB01` | code | 2012 | 9 |
| 209 | `pad_read` | `$CB0A` | code | 2025 | 12 |
| 210 | `loc_CB16` | `$CB16` | label_inside_code | 2033 | 23 |
| 211 | `loc_CB2D` | `$CB2D` | label_inside_code | 2048 | 19 |
| 212 | `sub_CB40` | `$CB40` | code | 2067 | 4 |
| 213 | `loc_CB44` | `$CB44` | label_inside_code | 2072 | 7 |
| 214 | `start_game_check` | `$CB4B` | code | 2083 | 28 |
| 215 | `loc_CB67` | `$CB67` | label_inside_code | 2098 | 15 |
| 216 | `loc_CB76` | `$CB76` | label_inside_code | 2107 | 148 |
| 217 | `loc_CC0A` | `$CC0A` | label_inside_code | 2178 | 11 |
| 218 | `locret_CC15` | `$CC15` | label_inside_code | 2185 | 1 |
| 219 | `unk_CC16` | `$CC16` | data | 2188 | 16 |
| 220 | `unk_CC26` | `$CC26` | data | 2204 | 16 |
| 221 | `continue_level_check` | `$CC36` | code | 2222 | 24 |
| 222 | `loc_CC4E` | `$CC4E` | label_inside_code | 2235 | 2 |
| 223 | `loc_CC50` | `$CC50` | label_inside_code | 2238 | 2 |
| 224 | `loc_CC52` | `$CC52` | label_inside_code | 2241 | 5 |
| 225 | `sub_CC57` | `$CC57` | code | 2252 | 86 |
| 226 | `unk_CCAD` | `$CCAD` | data | 2292 | 13 |
| 227 | `unk_CCBA` | `$CCBA` | data | 2305 | 13 |
| 228 | `unk_CCC7` | `$CCC7` | data | 2318 | 13 |
| 229 | `clear_oam_buffer_unused` | `$CCD4` | code | 2335 | 4 |
| 230 | `sub_CCD8` | `$CCD8` | code | 2344 | 2 |
| 231 | `loc_CCDA` | `$CCDA` | label_inside_code | 2347 | 9 |
| 232 | `vsync` | `$CCE3` | code | 2360 | 10 |
| 233 | `loc_CCED` | `$CCED` | label_inside_code | 2367 | 9 |
| 234 | `handle_pad_buttons` | `$CCF6` | code | 2381 | 17 |
| 235 | `loc_CD07` | `$CD07` | label_inside_code | 2392 | 27 |
| 236 | `loc_CD22` | `$CD22` | label_inside_code | 2408 | 10 |
| 237 | `loc_CD2C` | `$CD2C` | label_inside_code | 2415 | 15 |
| 238 | `pmove_direction_tab` | `$CD3B` | data | 2427 | 16 |
| 239 | `demo_buttons_tab` | `$CD4B` | data | 2428 | 32 |
| 240 | `sub_CD6B` | `$CD6B` | code | 2436 | 19 |
| 241 | `loc_CD7E` | `$CD7E` | label_inside_code | 2449 | 8 |
| 242 | `loc_CD86` | `$CD86` | label_inside_code | 2455 | 2 |
| 243 | `loc_CD88` | `$CD88` | label_inside_code | 2458 | 7 |
| 244 | `loc_CD8F` | `$CD8F` | label_inside_code | 2464 | 3 |
| 245 | `loc_CD92` | `$CD92` | label_inside_code | 2469 | 21 |
| 246 | `loc_CDA7` | `$CDA7` | label_inside_code | 2483 | 31 |
| 247 | `loc_CDC6` | `$CDC6` | label_inside_code | 2503 | 29 |
| 248 | `loc_CDE3` | `$CDE3` | label_inside_code | 2521 | 4 |
| 249 | `loc_CDE7` | `$CDE7` | label_inside_code | 2526 | 30 |
| 250 | `loc_CE05` | `$CE05` | label_inside_code | 2547 | 15 |
| 251 | `loc_CE14` | `$CE14` | label_inside_code | 2557 | 13 |
| 252 | `loc_CE21` | `$CE21` | label_inside_code | 2566 | 2 |
| 253 | `loc_CE23` | `$CE23` | label_inside_code | 2569 | 14 |
| 254 | `loc_CE31` | `$CE31` | label_inside_code | 2578 | 13 |
| 255 | `loc_CE3E` | `$CE3E` | label_inside_code | 2587 | 2 |
| 256 | `loc_CE40` | `$CE40` | label_inside_code | 2591 | 36 |
| 257 | `loc_CE64` | `$CE64` | label_inside_code | 2611 | 10 |
| 258 | `loc_CE6E` | `$CE6E` | label_inside_code | 2619 | 4 |
| 259 | `sub_CE72` | `$CE72` | code | 2629 | 26 |
| 260 | `locret_CE8C` | `$CE8C` | label_inside_code | 2645 | 1 |
| 261 | `nmi_handler` | `$CE8D` | code | 2653 | 55 |
| 262 | `dummy_irq_handler` | `$CEC4` | code | 2689 | 1 |
| 263 | `ppu_update_background` | `$CEC5` | code | 2698 | 7 |
| 264 | `loc_CECC` | `$CECC` | label_inside_code | 2704 | 7 |
| 265 | `loc_CED3` | `$CED3` | label_inside_code | 2710 | 84 |
| 266 | `loc_CF27` | `$CF27` | label_inside_code | 2744 | 16 |
| 267 | `loc_CF37` | `$CF37` | label_inside_code | 2753 | 1 |
| 268 | `loc_CF38` | `$CF38` | label_inside_code | 2756 | 56 |
| 269 | `loc_CF70` | `$CF70` | label_inside_code | 2779 | 5 |
| 270 | `loc_CF75` | `$CF75` | label_inside_code | 2785 | 4 |
| 271 | `loc_CF79` | `$CF79` | label_inside_code | 2789 | 19 |
| 272 | `loc_CF8C` | `$CF8C` | label_inside_code | 2803 | 22 |
| 273 | `loc_CFA2` | `$CFA2` | label_inside_code | 2815 | 48 |
| 274 | `loc_CFD2` | `$CFD2` | label_inside_code | 2835 | 5 |
| 275 | `loc_CFD7` | `$CFD7` | label_inside_code | 2841 | 7 |
| 276 | `loc_CFDE` | `$CFDE` | label_inside_code | 2846 | 14 |
| 277 | `loc_CFEC` | `$CFEC` | label_inside_code | 2854 | 18 |
| 278 | `loc_CFFE` | `$CFFE` | label_inside_code | 2865 | 51 |
| 279 | `ppu_cram_update` | `$D031` | code | 2897 | 27 |
| 280 | `loc_D04C` | `$D04C` | label_inside_code | 2911 | 12 |
| 281 | `loc_D058` | `$D058` | label_inside_code | 2918 | 2 |
| 282 | `loc_D05A` | `$D05A` | label_inside_code | 2921 | 1 |
| 283 | `loc_D05B` | `$D05B` | label_inside_code | 2924 | 72 |
| 284 | `locret_D0A3` | `$D0A3` | label_inside_code | 2953 | 1 |
| 285 | `sub_D0A4` | `$D0A4` | code | 2961 | 21 |
| 286 | `locret_D0B9` | `$D0B9` | label_inside_code | 2974 | 1 |
| 287 | `loc_D0BA` | `$D0BA` | label_inside_code | 2978 | 4 |
| 288 | `locret_D0BE` | `$D0BE` | label_inside_code | 2982 | 1 |
| 289 | `sub_D0BF` | `$D0BF` | code | 2991 | 26 |
| 290 | `loc_D0D9` | `$D0D9` | label_inside_code | 3005 | 15 |
| 291 | `loc_D0E8` | `$D0E8` | label_inside_code | 3015 | 23 |
| 292 | `loc_D0FF` | `$D0FF` | label_inside_code | 3026 | 9 |
| 293 | `check_high_score_reached` | `$D108` | code | 3037 | 9 |
| 294 | `loc_D111` | `$D111` | label_inside_code | 3044 | 26 |
| 295 | `locret_D12B` | `$D12B` | label_inside_code | 3058 | 1 |
| 296 | `sub_D12C` | `$D12C` | code | 3067 | 7 |
| 297 | `loc_D133` | `$D133` | label_inside_code | 3074 | 40 |
| 298 | `loc_D15B` | `$D15B` | label_inside_code | 3093 | 7 |
| 299 | `loc_D162` | `$D162` | label_inside_code | 3100 | 11 |
| 300 | `loc_D16D` | `$D16D` | label_inside_code | 3107 | 10 |
| 301 | `loc_D177` | `$D177` | label_inside_code | 3114 | 7 |
| 302 | `loc_D17E` | `$D17E` | label_inside_code | 3121 | 11 |
| 303 | `loc_D189` | `$D189` | label_inside_code | 3128 | 3 |
| 304 | `sub_D18C` | `$D18C` | code | 3136 | 7 |
| 305 | `sub_D193` | `$D193` | code | 3146 | 19 |
| 306 | `sub_D1A6` | `$D1A6` | code | 3161 | 15 |
| 307 | `loc_D1B5` | `$D1B5` | label_inside_code | 3174 | 12 |
| 308 | `loc_D1C1` | `$D1C1` | label_inside_code | 3181 | 23 |
| 309 | `loc_D1D8` | `$D1D8` | label_inside_code | 3194 | 6 |
| 310 | `loc_D1DE` | `$D1DE` | label_inside_code | 3199 | 2 |
| 311 | `locret_D1E0` | `$D1E0` | label_inside_code | 3202 | 1 |
| 312 | `unk_D1E1` | `$D1E1` | data | 3207 | 31 |
| 313 | `unk_D200` | `$D200` | data | 3238 | 59 |
| 314 | `unk_D23B` | `$D23B` | data | 3297 | 1 |
| 315 | `unk_D23C` | `$D23C` | data | 3298 | 132 |
| 316 | `sub_D2C0` | `$D2C0` | code | 3434 | 4 |
| 317 | `sub_D2C4` | `$D2C4` | code | 3443 | 4 |
| 318 | `clear_first_last_sprites` | `$D2C8` | code | 3453 | 14 |
| 319 | `loc_D2D6` | `$D2D6` | label_inside_code | 3463 | 11 |
| 320 | `loc_D2E1` | `$D2E1` | label_inside_code | 3472 | 2 |
| 321 | `locret_D2E3` | `$D2E3` | label_inside_code | 3475 | 1 |
| 322 | `set_new_high_score` | `$D2E4` | code | 3483 | 2 |
| 323 | `loc_D2E6` | `$D2E6` | label_inside_code | 3487 | 13 |
| 324 | `loc_D2F3` | `$D2F3` | label_inside_code | 3495 | 9 |
| 325 | `locret_D2FC` | `$D2FC` | label_inside_code | 3501 | 1 |
| 326 | `sub_D2FD` | `$D2FD` | code | 3509 | 33 |
| 327 | `loc_D31E` | `$D31E` | label_inside_code | 3526 | 16 |
| 328 | `loc_D32E` | `$D32E` | label_inside_code | 3536 | 2 |
| 329 | `loc_D330` | `$D330` | label_inside_code | 3539 | 2 |
| 330 | `loc_D332` | `$D332` | label_inside_code | 3542 | 20 |
| 331 | `locret_D346` | `$D346` | label_inside_code | 3554 | 1 |
| 332 | `sub_D347` | `$D347` | code | 3562 | 21 |
| 333 | `loc_D35C` | `$D35C` | label_inside_code | 3576 | 2 |
| 334 | `loc_D35E` | `$D35E` | label_inside_code | 3580 | 8 |
| 335 | `loc_D366` | `$D366` | label_inside_code | 3586 | 24 |
| 336 | `loc_D37E` | `$D37E` | label_inside_code | 3600 | 42 |
| 337 | `sub_D3A8` | `$D3A8` | code | 3625 | 7 |
| 338 | `loc_D3AF` | `$D3AF` | label_inside_code | 3632 | 10 |
| 339 | `loc_D3B9` | `$D3B9` | label_inside_code | 3639 | 25 |
| 340 | `loc_D3D2` | `$D3D2` | label_inside_code | 3652 | 3 |
| 341 | `loc_D3D5` | `$D3D5` | label_inside_code | 3655 | 11 |
| 342 | `loc_D3E0` | `$D3E0` | label_inside_code | 3662 | 19 |
| 343 | `locret_D3F3` | `$D3F3` | label_inside_code | 3676 | 1 |
| 344 | `sub_D3F4` | `$D3F4` | code | 3684 | 11 |
| 345 | `sub_D3FF` | `$D3FF` | code | 3696 | 15 |
| 346 | `loc_D40E` | `$D40E` | label_inside_code | 3705 | 17 |
| 347 | `loc_D41F` | `$D41F` | label_inside_code | 3716 | 8 |
| 348 | `loc_D427` | `$D427` | label_inside_code | 3722 | 15 |
| 349 | `loc_D436` | `$D436` | label_inside_code | 3732 | 8 |
| 350 | `loc_D43E` | `$D43E` | label_inside_code | 3738 | 22 |
| 351 | `loc_D454` | `$D454` | label_inside_code | 3751 | 14 |
| 352 | `locret_D462` | `$D462` | label_inside_code | 3761 | 1 |
| 353 | `sub_D463` | `$D463` | code | 3769 | 14 |
| 354 | `locret_D471` | `$D471` | label_inside_code | 3777 | 1 |
| 355 | `sub_D472` | `$D472` | code | 3785 | 11 |
| 356 | `loc_D47D` | `$D47D` | label_inside_code | 3793 | 3 |
| 357 | `sub_D480` | `$D480` | code | 3802 | 6 |
| 358 | `locret_D486` | `$D486` | label_inside_code | 3807 | 1 |
| 359 | `loc_D487` | `$D487` | label_inside_code | 3811 | 9 |
| 360 | `loc_D490` | `$D490` | label_inside_code | 3817 | 12 |
| 361 | `loc_D49C` | `$D49C` | label_inside_code | 3825 | 7 |
| 362 | `loc_D4A3` | `$D4A3` | label_inside_code | 3830 | 26 |
| 363 | `loc_D4BD` | `$D4BD` | label_inside_code | 3844 | 31 |
| 364 | `loc_D4DC` | `$D4DC` | label_inside_code | 3860 | 2 |
| 365 | `loc_D4DE` | `$D4DE` | label_inside_code | 3863 | 18 |
| 366 | `locret_D4F0` | `$D4F0` | label_inside_code | 3875 | 1 |
| 367 | `loc_D4F1` | `$D4F1` | label_inside_code | 3880 | 9 |
| 368 | `sub_D4FA` | `$D4FA` | code | 3891 | 22 |
| 369 | `sub_D510` | `$D510` | code | 3910 | 10 |
| 370 | `sub_D51A` | `$D51A` | code | 3922 | 14 |
| 371 | `sub_D528` | `$D528` | code | 3936 | 13 |
| 372 | `loc_D535` | `$D535` | label_inside_code | 3945 | 3 |
| 373 | `loc_D538` | `$D538` | label_inside_code | 3949 | 7 |
| 374 | `loc_D53F` | `$D53F` | label_inside_code | 3955 | 2 |
| 375 | `loc_D541` | `$D541` | label_inside_code | 3958 | 12 |
| 376 | `sub_D54D` | `$D54D` | code | 3971 | 12 |
| 377 | `sub_D559` | `$D559` | code | 3984 | 18 |
| 378 | `sub_D56B` | `$D56B` | code | 4001 | 8 |
| 379 | `loc_D573` | `$D573` | label_inside_code | 4007 | 9 |
| 380 | `loc_D57C` | `$D57C` | label_inside_code | 4015 | 25 |
| 381 | `loc_D595` | `$D595` | label_inside_code | 4028 | 22 |
| 382 | `sub_D5AB` | `$D5AB` | code | 4045 | 7 |
| 383 | `loc_D5B2` | `$D5B2` | label_inside_code | 4052 | 15 |
| 384 | `sub_D5C1` | `$D5C1` | code | 4067 | 13 |
| 385 | `loc_D5CE` | `$D5CE` | label_inside_code | 4077 | 8 |
| 386 | `locret_D5D6` | `$D5D6` | label_inside_code | 4084 | 1 |
| 387 | `sub_D5D7` | `$D5D7` | code | 4092 | 15 |
| 388 | `loc_D5E6` | `$D5E6` | label_inside_code | 4103 | 13 |
| 389 | `loc_D5F3` | `$D5F3` | label_inside_code | 4113 | 8 |
| 390 | `loc_D5FB` | `$D5FB` | label_inside_code | 4119 | 9 |
| 391 | `loc_D604` | `$D604` | label_inside_code | 4126 | 19 |
| 392 | `loc_D617` | `$D617` | label_inside_code | 4140 | 8 |
| 393 | `unk_D61F` | `$D61F` | data | 4148 | 28 |
| 394 | `unk_D63B` | `$D63B` | data | 4176 | 16 |
| 395 | `unk_D64B` | `$D64B` | data | 4192 | 16 |
| 396 | `random_enemy_tab` | `$D65B` | data | 4208 | 64 |
| 397 | `get_random_enemy` | `$D69B` | code | 4219 | 4 |
| 398 | `sub_D69F` | `$D69F` | code | 4228 | 16 |
| 399 | `sub_D6AF` | `$D6AF` | code | 4244 | 46 |
| 400 | `loc_D6DD` | `$D6DD` | label_inside_code | 4273 | 2 |
| 401 | `sub_D6DF` | `$D6DF` | code | 4282 | 39 |
| 402 | `loc_D706` | `$D706` | label_inside_code | 4303 | 14 |
| 403 | `loc_D714` | `$D714` | label_inside_code | 4311 | 55 |
| 404 | `tab_D74B` | `$D74B` | data | 4344 | 8 |
| 405 | `sub_D753` | `$D753` | code | 4356 | 39 |
| 406 | `locret_D77A` | `$D77A` | label_inside_code | 4379 | 1 |
| 407 | `sub_D77B` | `$D77B` | code | 4388 | 14 |
| 408 | `loc_D789` | `$D789` | label_inside_code | 4398 | 6 |
| 409 | `sub_D78F` | `$D78F` | code | 4407 | 21 |
| 410 | `locret_D7A4` | `$D7A4` | label_inside_code | 4418 | 1 |
| 411 | `set_nametable_addr_after_scroll` | `$D7A5` | code | 4427 | 24 |
| 412 | `nt_address_tab` | `$D7BD` | data | 4444 | 30 |
| 413 | `byte_D7DB` | `$D7DB` | data | 4459 | 4 |
| 414 | `byte_D7DF` | `$D7DF` | data | 4463 | 4 |
| 415 | `sub_D7E3` | `$D7E3` | code | 4471 | 19 |
| 416 | `loc_D7F6` | `$D7F6` | label_inside_code | 4483 | 10 |
| 417 | `loc_D800` | `$D800` | label_inside_code | 4490 | 10 |
| 418 | `loc_D80A` | `$D80A` | label_inside_code | 4497 | 14 |
| 419 | `loc_D818` | `$D818` | label_inside_code | 4507 | 12 |
| 420 | `sub_D824` | `$D824` | code | 4521 | 12 |
| 421 | `loc_D830` | `$D830` | label_inside_code | 4532 | 13 |
| 422 | `loc_D83D` | `$D83D` | label_inside_code | 4540 | 13 |
| 423 | `sub_D84A` | `$D84A` | code | 4553 | 5 |
| 424 | `sub_D84F` | `$D84F` | code | 4563 | 12 |
| 425 | `loc_D85B` | `$D85B` | label_inside_code | 4571 | 14 |
| 426 | `loc_D869` | `$D869` | label_inside_code | 4581 | 3 |
| 427 | `loc_D86C` | `$D86C` | label_inside_code | 4585 | 26 |
| 428 | `sub_D886` | `$D886` | code | 4606 | 14 |
| 429 | `sub_D894` | `$D894` | code | 4620 | 13 |
| 430 | `sub_D8A1` | `$D8A1` | code | 4633 | 30 |
| 431 | `sub_D8BF` | `$D8BF` | code | 4656 | 29 |
| 432 | `sub_D8DC` | `$D8DC` | code | 4677 | 30 |
| 433 | `loc_D8FA` | `$D8FA` | label_inside_code | 4696 | 5 |
| 434 | `loc_D8FF` | `$D8FF` | label_inside_code | 4701 | 10 |
| 435 | `loc_D909` | `$D909` | label_inside_code | 4709 | 23 |
| 436 | `sub_D920` | `$D920` | code | 4727 | 20 |
| 437 | `loc_D934` | `$D934` | label_inside_code | 4739 | 4 |
| 438 | `sub_D938` | `$D938` | code | 4748 | 2 |
| 439 | `sub_D93A` | `$D93A` | code | 4756 | 7 |
| 440 | `loc_D941` | `$D941` | label_inside_code | 4762 | 2 |
| 441 | `sub_D943` | `$D943` | code | 4770 | 20 |
| 442 | `loc_D957` | `$D957` | label_inside_code | 4783 | 15 |
| 443 | `loc_D966` | `$D966` | label_inside_code | 4793 | 11 |
| 444 | `loc_D971` | `$D971` | label_inside_code | 4801 | 3 |
| 445 | `loc_D974` | `$D974` | label_inside_code | 4805 | 13 |
| 446 | `loc_D981` | `$D981` | label_inside_code | 4813 | 3 |
| 447 | `loc_D984` | `$D984` | label_inside_code | 4817 | 41 |
| 448 | `loc_D9AD` | `$D9AD` | label_inside_code | 4839 | 41 |
| 449 | `loc_D9D6` | `$D9D6` | label_inside_code | 4861 | 10 |
| 450 | `sub_D9E0` | `$D9E0` | code | 4872 | 46 |
| 451 | `loc_DA0E` | `$DA0E` | label_inside_code | 4900 | 8 |
| 452 | `sub_DA16` | `$DA16` | code | 4911 | 2 |
| 453 | `loc_DA18` | `$DA18` | label_inside_code | 4914 | 16 |
| 454 | `sub_DA28` | `$DA28` | code | 4930 | 14 |
| 455 | `sub_DA36` | `$DA36` | code | 4946 | 34 |
| 456 | `loc_DA58` | `$DA58` | label_inside_code | 4967 | 9 |
| 457 | `loc_DA61` | `$DA61` | label_inside_code | 4976 | 15 |
| 458 | `loc_DA70` | `$DA70` | label_inside_code | 4986 | 10 |
| 459 | `loc_DA7A` | `$DA7A` | label_inside_code | 4993 | 11 |
| 460 | `loc_DA85` | `$DA85` | label_inside_code | 5000 | 21 |
| 461 | `loc_DA9A` | `$DA9A` | label_inside_code | 5011 | 14 |
| 462 | `loc_DAA8` | `$DAA8` | label_inside_code | 5024 | 19 |
| 463 | `loc_DABB` | `$DABB` | label_inside_code | 5035 | 5 |
| 464 | `loc_DAC0` | `$DAC0` | label_inside_code | 5041 | 31 |
| 465 | `loc_DADF` | `$DADF` | label_inside_code | 5057 | 7 |
| 466 | `loc_DAE6` | `$DAE6` | label_inside_code | 5062 | 18 |
| 467 | `loc_DAF8` | `$DAF8` | label_inside_code | 5073 | 3 |
| 468 | `loc_DAFB` | `$DAFB` | label_inside_code | 5076 | 11 |
| 469 | `loc_DB06` | `$DB06` | label_inside_code | 5085 | 4 |
| 470 | `loc_DB0A` | `$DB0A` | label_inside_code | 5089 | 21 |
| 471 | `loc_DB1F` | `$DB1F` | label_inside_code | 5102 | 57 |
| 472 | `loc_DB58` | `$DB58` | label_inside_code | 5136 | 45 |
| 473 | `loc_DB85` | `$DB85` | label_inside_code | 5161 | 55 |
| 474 | `loc_DBBC` | `$DBBC` | label_inside_code | 5191 | 9 |
| 475 | `unk_DBC5` | `$DBC5` | data | 5199 | 19 |
| 476 | `unk_DBD8` | `$DBD8` | data | 5218 | 237 |
| 477 | `sub_DCC5` | `$DCC5` | code | 5459 | 6 |
| 478 | `sub_DCCB` | `$DCCB` | code | 5468 | 91 |
| 479 | `sub_DD26` | `$DD26` | code | 5555 | 27 |
| 480 | `loc_DD41` | `$DD41` | label_inside_code | 5572 | 2 |
| 481 | `loc_DD43` | `$DD43` | label_inside_code | 5575 | 3 |
| 482 | `loc_DD46` | `$DD46` | label_inside_code | 5579 | 11 |
| 483 | `loc_DD51` | `$DD51` | label_inside_code | 5586 | 10 |
| 484 | `loc_DD5B` | `$DD5B` | label_inside_code | 5593 | 62 |
| 485 | `loc_DD99` | `$DD99` | label_inside_code | 5628 | 5 |
| 486 | `sub_DD9E` | `$DD9E` | code | 5638 | 2 |
| 487 | `loc_DDA0` | `$DDA0` | label_inside_code | 5642 | 10 |
| 488 | `loc_DDAA` | `$DDAA` | label_inside_code | 5649 | 34 |
| 489 | `loc_DDCC` | `$DDCC` | label_inside_code | 5668 | 21 |
| 490 | `loc_DDE1` | `$DDE1` | label_inside_code | 5680 | 5 |
| 491 | `loc_DDE6` | `$DDE6` | label_inside_code | 5685 | 2 |
| 492 | `loc_DDE8` | `$DDE8` | label_inside_code | 5690 | 9 |
| 493 | `loc_DDF1` | `$DDF1` | label_inside_code | 5700 | 2 |
| 494 | `sub_DDF3` | `$DDF3` | code | 5709 | 6 |
| 495 | `sub_DDF9` | `$DDF9` | code | 5719 | 21 |
| 496 | `loc_DE0E` | `$DE0E` | label_inside_code | 5731 | 6 |
| 497 | `sub_DE14` | `$DE14` | code | 5741 | 15 |
| 498 | `loc_DE23` | `$DE23` | label_inside_code | 5749 | 47 |
| 499 | `loc_DE52` | `$DE52` | label_inside_code | 5777 | 13 |
| 500 | `sub_DE5F` | `$DE5F` | code | 5790 | 15 |
| 501 | `loc_DE6E` | `$DE6E` | label_inside_code | 5798 | 35 |
| 502 | `sub_DE91` | `$DE91` | code | 5826 | 59 |
| 503 | `loc_DECC` | `$DECC` | label_inside_code | 5857 | 39 |
| 504 | `sub_DEF3` | `$DEF3` | code | 5884 | 43 |
| 505 | `loc_DF1E` | `$DF1E` | label_inside_code | 5905 | 42 |
| 506 | `sub_DF48` | `$DF48` | code | 5934 | 20 |
| 507 | `loc_DF5C` | `$DF5C` | label_inside_code | 5948 | 12 |
| 508 | `reset_score` | `$DF68` | code | 5963 | 18 |
| 509 | `loc_DF7A` | `$DF7A` | label_inside_code | 5974 | 2 |
| 510 | `loc_DF7C` | `$DF7C` | label_inside_code | 5978 | 35 |
| 511 | `unk_DF9F` | `$DF9F` | data | 5997 | 7 |
| 512 | `pal_DFA6` | `$DFA6` | data | 6004 | 25 |
| 513 | `show_main_menu` | `$DFBF` | code | 6034 | 97 |
| 514 | `loc_E020` | `$E020` | label_inside_code | 6080 | 96 |
| 515 | `loc_E080` | `$E080` | label_inside_code | 6123 | 41 |
| 516 | `loc_E0A9` | `$E0A9` | label_inside_code | 6144 | 31 |
| 517 | `loc_E0C8` | `$E0C8` | label_inside_code | 6159 | 18 |
| 518 | `loc_E0DA` | `$E0DA` | label_inside_code | 6170 | 26 |
| 519 | `loc_E0F4` | `$E0F4` | label_inside_code | 6183 | 12 |
| 520 | `loc_E100` | `$E100` | label_inside_code | 6191 | 10 |
| 521 | `loc_E10A` | `$E10A` | label_inside_code | 6198 | 3 |
| 522 | `loc_E10D` | `$E10D` | label_inside_code | 6201 | 28 |
| 523 | `loc_E129` | `$E129` | label_inside_code | 6218 | 28 |
| 524 | `loc_E145` | `$E145` | label_inside_code | 6232 | 24 |
| 525 | `aGameStart` | `$E15D` | data | 6247 | 16 |
| 526 | `aContinue` | `$E16D` | data | 6254 | 23 |
| 527 | `loc_E184` | `$E184` | label_inside_code | 6263 | 9 |
| 528 | `loc_E18D` | `$E18D` | label_inside_code | 6269 | 3 |
| 529 | `loc_E190` | `$E190` | label_inside_code | 6272 | 18 |
| 530 | `loc_E1A2` | `$E1A2` | label_inside_code | 6285 | 20 |
| 531 | `loc_E1B6` | `$E1B6` | label_inside_code | 6297 | 6 |
| 532 | `loc_E1BC` | `$E1BC` | label_inside_code | 6303 | 9 |
| 533 | `loc_E1C5` | `$E1C5` | label_inside_code | 6310 | 4 |
| 534 | `loc_E1C9` | `$E1C9` | label_inside_code | 6314 | 29 |
| 535 | `loc_E1E6` | `$E1E6` | label_inside_code | 6330 | 12 |
| 536 | `loc_E1F2` | `$E1F2` | label_inside_code | 6338 | 16 |
| 537 | `unk_E202` | `$E202` | data | 6350 | 13 |
| 538 | `unk_E20F` | `$E20F` | data | 6363 | 13 |
| 539 | `unk_E21C` | `$E21C` | data | 6376 | 13 |
| 540 | `sub_E229` | `$E229` | code | 6393 | 42 |
| 541 | `locret_E253` | `$E253` | label_inside_code | 6413 | 1 |
| 542 | `unk_E254` | `$E254` | data | 6419 | 9 |
| 543 | `unk_E25D` | `$E25D` | data | 6428 | 9 |
| 544 | `sub_E266` | `$E266` | code | 6441 | 8 |
| 545 | `loc_E26E` | `$E26E` | label_inside_code | 6448 | 15 |
| 546 | `loc_E27D` | `$E27D` | label_inside_code | 6457 | 5 |
| 547 | `loc_E282` | `$E282` | label_inside_code | 6462 | 19 |
| 548 | `aScoreTop` | `$E295` | data | 6478 | 20 |
| 549 | `aArea` | `$E2A9` | data | 6482 | 80 |
| 550 | `aAI` | `$E2F9` | data | 6559 | 9 |
| 551 | `aGameDesindedBy` | `$E302` | data | 6565 | 27 |
| 552 | `aProducedByAii` | `$E31D` | data | 6569 | 23 |
| 553 | `aPresentedByPon` | `$E334` | data | 6573 | 29 |
| 554 | `aCopyright1986P` | `$E351` | data | 6578 | 32 |
| 555 | `aLicensedBy` | `$E371` | data | 6586 | 14 |
| 556 | `aNintendoOfAmer` | `$E37F` | data | 6590 | 26 |
| 557 | `sub_E399` | `$E399` | code | 6597 | 17 |
| 558 | `loc_E3AA` | `$E3AA` | label_inside_code | 6607 | 13 |
| 559 | `locret_E3B7` | `$E3B7` | label_inside_code | 6615 | 1 |
| 560 | `byte_E3B8` | `$E3B8` | data | 6620 | 25 |
| 561 | `tab_E3D1` | `$E3D1` | data | 6623 | 35 |
| 562 | `sub_E3F4` | `$E3F4` | code | 6631 | 2 |
| 563 | `loc_E3F6` | `$E3F6` | label_inside_code | 6635 | 25 |
| 564 | `loc_E40F` | `$E40F` | label_inside_code | 6651 | 11 |
| 565 | `loc_E41A` | `$E41A` | label_inside_code | 6659 | 30 |
| 566 | `locret_E438` | `$E438` | label_inside_code | 6677 | 1 |
| 567 | `sound_test_menu` | `$E439` | code | 6685 | 17 |
| 568 | `aSoundTest` | `$E44A` | data | 6697 | 24 |
| 569 | `sub_E462` | `$E462` | code | 6711 | 13 |
| 570 | `loc_E46F` | `$E46F` | label_inside_code | 6720 | 24 |
| 571 | `loc_E487` | `$E487` | label_inside_code | 6734 | 4 |
| 572 | `loc_E48B` | `$E48B` | label_inside_code | 6738 | 23 |
| 573 | `loc_E4A2` | `$E4A2` | label_inside_code | 6751 | 25 |
| 574 | `loc_E4BB` | `$E4BB` | label_inside_code | 6765 | 20 |
| 575 | `show_ending_screen` | `$E4CF` | code | 6782 | 82 |
| 576 | `aScore` | `$E521` | data | 6820 | 53 |
| 577 | `loc_E556` | `$E556` | label_inside_code | 6843 | 8 |
| 578 | `loc_E55E` | `$E55E` | label_inside_code | 6850 | 9 |
| 579 | `loc_E567` | `$E567` | label_inside_code | 6859 | 54 |
| 580 | `loc_E59D` | `$E59D` | label_inside_code | 6892 | 40 |
| 581 | `loc_E5C5` | `$E5C5` | label_inside_code | 6914 | 10 |
| 582 | `loc_E5CF` | `$E5CF` | label_inside_code | 6922 | 27 |
| 583 | `loc_E5EA` | `$E5EA` | label_inside_code | 6938 | 28 |
| 584 | `loc_E606` | `$E606` | label_inside_code | 6955 | 9 |
| 585 | `sub_E60F` | `$E60F` | code | 6967 | 15 |
| 586 | `loc_E61E` | `$E61E` | label_inside_code | 6979 | 25 |
| 587 | `loc_E637` | `$E637` | label_inside_code | 6993 | 23 |
| 588 | `loc_E64E` | `$E64E` | label_inside_code | 7007 | 5 |
| 589 | `sub_E653` | `$E653` | code | 7017 | 6 |
| 590 | `loc_E659` | `$E659` | label_inside_code | 7023 | 23 |
| 591 | `locret_E670` | `$E670` | label_inside_code | 7035 | 1 |
| 592 | `congratulation_text` | `$E671` | data | 7040 | 1 |
| 593 | `aCongratulation` | `$E672` | data | 7041 | 19 |
| 594 | `aYouAreTheMessi` | `$E685` | data | 7044 | 32 |
| 595 | `aStaff` | `$E6A5` | data | 7047 | 8 |
| 596 | `aGameDesignJemi` | `$E6AD` | data | 7050 | 30 |
| 597 | `aProgram` | `$E6CB` | data | 7052 | 8 |
| 598 | `aJemini` | `$E6D3` | data | 7054 | 8 |
| 599 | `aMusicMiyamoto` | `$E6DB` | data | 7056 | 16 |
| 600 | `aGraphics` | `$E6EB` | data | 7058 | 9 |
| 601 | `aJanus` | `$E6F4` | data | 7060 | 7 |
| 602 | `aTestPlay` | `$E6FB` | data | 7062 | 10 |
| 603 | `aMncNui` | `$E705` | data | 7064 | 9 |
| 604 | `aWaoIsee` | `$E70E` | data | 7066 | 10 |
| 605 | `aYoriki` | `$E718` | data | 7068 | 9 |
| 606 | `aProdusedByAii` | `$E721` | data | 7071 | 18 |
| 607 | `aCreatedByCompi` | `$E733` | data | 7074 | 23 |
| 608 | `aSeeYouAgainInG` | `$E74A` | data | 7079 | 29 |
| 609 | `sub_E767` | `$E767` | code | 7090 | 5 |
| 610 | `sub_E76C` | `$E76C` | code | 7100 | 19 |
| 611 | `loc_E77F` | `$E77F` | label_inside_code | 7115 | 9 |
| 612 | `loc_E788` | `$E788` | label_inside_code | 7121 | 25 |
| 613 | `loc_E7A1` | `$E7A1` | label_inside_code | 7136 | 26 |
| 614 | `loc_E7BB` | `$E7BB` | label_inside_code | 7154 | 4 |
| 615 | `reset` | `$E7BF` | code | 7163 | 27 |
| 616 | `loc_E7DA` | `$E7DA` | label_inside_code | 7178 | 28 |
| 617 | `loc_E7F6` | `$E7F6` | label_inside_code | 7194 | 16 |
| 618 | `main_menu_loop` | `$E806` | code | 7209 | 24 |
| 619 | `sub_E81E` | `$E81E` | code | 7226 | 29 |
| 620 | `loc_E83B` | `$E83B` | label_inside_code | 7239 | 15 |
| 621 | `loc_E84A` | `$E84A` | label_inside_code | 7248 | 41 |
| 622 | `loc_E873` | `$E873` | label_inside_code | 7270 | 3 |
| 623 | `loc_E876` | `$E876` | label_inside_code | 7273 | 10 |
| 624 | `loc_E880` | `$E880` | label_inside_code | 7280 | 30 |
| 625 | `loc_E89E` | `$E89E` | label_inside_code | 7294 | 12 |
| 626 | `sub_E8AA` | `$E8AA` | code | 7308 | 4 |
| 627 | `loc_E8AE` | `$E8AE` | label_inside_code | 7312 | 12 |
| 628 | `loc_E8BA` | `$E8BA` | label_inside_code | 7319 | 15 |
| 629 | `sub_E8C9` | `$E8C9` | code | 7335 | 13 |
| 630 | `loc_E8D6` | `$E8D6` | label_inside_code | 7344 | 4 |
| 631 | `loc_E8DA` | `$E8DA` | label_inside_code | 7348 | 45 |
| 632 | `byte_E907` | `$E907` | data | 7370 | 16 |
| 633 | `loc_E917` | `$E917` | label_inside_code | 7374 | 23 |
| 634 | `loc_E92E` | `$E92E` | label_inside_code | 7387 | 9 |
| 635 | `loc_E937` | `$E937` | label_inside_code | 7393 | 2 |
| 636 | `level_completed_sequence` | `$E939` | code | 7402 | 23 |
| 637 | `loc_E950` | `$E950` | label_inside_code | 7417 | 3 |
| 638 | `loc_E953` | `$E953` | label_inside_code | 7421 | 17 |
| 639 | `loc_E964` | `$E964` | label_inside_code | 7432 | 9 |
| 640 | `loc_E96D` | `$E96D` | label_inside_code | 7439 | 31 |
| 641 | `loc_E98C` | `$E98C` | label_inside_code | 7455 | 9 |
| 642 | `sub_E995` | `$E995` | code | 7466 | 13 |
| 643 | `fade_palette_before_warp_another` | `$E9A2` | code | 7480 | 6 |
| 644 | `fade_palette_before_warp` | `$E9A8` | code | 7491 | 4 |
| 645 | `loc_E9AC` | `$E9AC` | label_inside_code | 7495 | 5 |
| 646 | `loc_E9B1` | `$E9B1` | label_inside_code | 7500 | 11 |
| 647 | `loc_E9BC` | `$E9BC` | label_inside_code | 7507 | 4 |
| 648 | `loc_E9C0` | `$E9C0` | label_inside_code | 7511 | 16 |
| 649 | `unk_E9D0` | `$E9D0` | data | 7523 | 16 |
| 650 | `update_music_after_fortress_destroy` | `$E9E0` | code | 7543 | 5 |
| 651 | `loc_E9E5` | `$E9E5` | label_inside_code | 7549 | 20 |
| 652 | `loc_E9F9` | `$E9F9` | label_inside_code | 7560 | 2 |
| 653 | `loc_E9FB` | `$E9FB` | label_inside_code | 7563 | 3 |
| 654 | `level_music_tab` | `$E9FE` | data | 7569 | 13 |
| 655 | `sub_EA0B` | `$EA0B` | code | 7586 | 2 |
| 656 | `loc_EA0D` | `$EA0D` | label_inside_code | 7591 | 21 |
| 657 | `loc_EA22` | `$EA22` | label_inside_code | 7604 | 3 |
| 658 | `loc_EA25` | `$EA25` | label_inside_code | 7607 | 17 |
| 659 | `all_enemies_destroyed` | `$EA36` | code | 7625 | 7 |
| 660 | `j_apu_reset_0` | `$EA3D` | thunk | 7638 | 3 |
| 661 | `j_apu_stop_0` | `$EA40` | thunk | 7647 | 3 |
| 662 | `j_apu_play_1` | `$EA43` | thunk | 7656 | 3 |
| 663 | `j_apu_update_0` | `$EA46` | thunk | 7665 | 3 |
| 664 | `sub_EA49` | `$EA49` | thunk | 7674 | 3 |
| 665 | `sub_EA4C` | `$EA4C` | thunk | 7683 | 3 |
| 666 | `sub_EA4F` | `$EA4F` | thunk | 7692 | 3 |
| 667 | `apu_update` | `$EA52` | code | 7701 | 27 |
| 668 | `loc_EA6D` | `$EA6D` | label_inside_code | 7715 | 13 |
| 669 | `loc_EA7A` | `$EA7A` | label_inside_code | 7723 | 15 |
| 670 | `loc_EA89` | `$EA89` | label_inside_code | 7731 | 7 |
| 671 | `locret_EA90` | `$EA90` | label_inside_code | 7736 | 1 |
| 672 | `loc_EA91` | `$EA91` | label_inside_code | 7740 | 11 |
| 673 | `loc_EA9C` | `$EA9C` | label_inside_code | 7748 | 3 |
| 674 | `loc_EA9F` | `$EA9F` | label_inside_code | 7751 | 11 |
| 675 | `loc_EAAA` | `$EAAA` | label_inside_code | 7758 | 28 |
| 676 | `loc_EAC6` | `$EAC6` | label_inside_code | 7772 | 25 |
| 677 | `loc_EADF` | `$EADF` | label_inside_code | 7783 | 45 |
| 678 | `loc_EB0C` | `$EB0C` | label_inside_code | 7804 | 11 |
| 679 | `loc_EB17` | `$EB17` | label_inside_code | 7810 | 14 |
| 680 | `loc_EB25` | `$EB25` | label_inside_code | 7820 | 8 |
| 681 | `loc_EB2D` | `$EB2D` | label_inside_code | 7826 | 10 |
| 682 | `loc_EB37` | `$EB37` | label_inside_code | 7832 | 9 |
| 683 | `loc_EB40` | `$EB40` | label_inside_code | 7838 | 10 |
| 684 | `loc_EB4A` | `$EB4A` | label_inside_code | 7845 | 3 |
| 685 | `loc_EB4D` | `$EB4D` | label_inside_code | 7848 | 11 |
| 686 | `sub_EB58` | `$EB58` | code | 7861 | 25 |
| 687 | `loc_EB71` | `$EB71` | label_inside_code | 7877 | 9 |
| 688 | `sub_EB7A` | `$EB7A` | code | 7889 | 23 |
| 689 | `loc_EB91` | `$EB91` | label_inside_code | 7902 | 7 |
| 690 | `loc_EB98` | `$EB98` | label_inside_code | 7908 | 23 |
| 691 | `word_EBAF` | `$EBAF` | data | 7926 | 60 |
| 692 | `apu_fx_0` | `$EBEB` | code | 7960 | 11 |
| 693 | `apu_fx_1` | `$EBF6` | code | 7975 | 15 |
| 694 | `apu_fx_6` | `$EC05` | code | 7991 | 6 |
| 695 | `apu_fx_2` | `$EC0B` | code | 8000 | 5 |
| 696 | `apu_fx_5` | `$EC10` | code | 8009 | 7 |
| 697 | `apu_fx_3` | `$EC17` | code | 8021 | 4 |
| 698 | `apu_fx_8` | `$EC1B` | code | 8030 | 4 |
| 699 | `apu_fx_4` | `$EC1F` | code | 8039 | 9 |
| 700 | `apu_fx_29` | `$EC28` | code | 8050 | 7 |
| 701 | `sub_EC2F` | `$EC2F` | code | 8060 | 6 |
| 702 | `apu_fx_9` | `$EC35` | code | 8070 | 6 |
| 703 | `loc_EC3B` | `$EC3B` | label_inside_code | 8075 | 4 |
| 704 | `apu_fx_13` | `$EC3F` | code | 8084 | 12 |
| 705 | `apu_fx_16` | `$EC4B` | code | 8098 | 4 |
| 706 | `apu_fx_10` | `$EC4F` | code | 8107 | 20 |
| 707 | `loc_EC63` | `$EC63` | label_inside_code | 8120 | 8 |
| 708 | `sub_EC6B` | `$EC6B` | code | 8131 | 9 |
| 709 | `apu_fx_7` | `$EC74` | code | 8143 | 4 |
| 710 | `apu_fx_17` | `$EC78` | code | 8152 | 7 |
| 711 | `apu_fx_19` | `$EC7F` | code | 8162 | 36 |
| 712 | `locret_ECA3` | `$ECA3` | label_inside_code | 8179 | 1 |
| 713 | `apu_fx_21` | `$ECA4` | code | 8187 | 12 |
| 714 | `sub_ECB0` | `$ECB0` | code | 8199 | 2 |
| 715 | `apu_fx_18` | `$ECB2` | code | 8208 | 37 |
| 716 | `apu_fx_22` | `$ECD7` | code | 8232 | 29 |
| 717 | `locret_ECF4` | `$ECF4` | label_inside_code | 8245 | 1 |
| 718 | `apu_fx_14_15` | `$ECF5` | code | 8253 | 19 |
| 719 | `sub_ED08` | `$ED08` | code | 8271 | 4 |
| 720 | `sub_ED0C` | `$ED0C` | code | 8280 | 4 |
| 721 | `sub_ED10` | `$ED10` | code | 8289 | 5 |
| 722 | `apu_fx_20` | `$ED15` | code | 8299 | 16 |
| 723 | `sub_ED25` | `$ED25` | code | 8313 | 10 |
| 724 | `sub_ED2F` | `$ED2F` | code | 8324 | 4 |
| 725 | `sub_ED33` | `$ED33` | code | 8333 | 9 |
| 726 | `apu_fx_23_24_25` | `$ED3C` | code | 8344 | 8 |
| 727 | `apu_fx_12` | `$ED44` | code | 8354 | 2 |
| 728 | `apu_fx_28` | `$ED46` | code | 8363 | 10 |
| 729 | `apu_fx_26_27` | `$ED50` | code | 8375 | 6 |
| 730 | `loc_ED56` | `$ED56` | label_inside_code | 8380 | 4 |
| 731 | `loc_ED5A` | `$ED5A` | label_inside_code | 8384 | 15 |
| 732 | `loc_ED69` | `$ED69` | label_inside_code | 8393 | 11 |
| 733 | `loc_ED74` | `$ED74` | label_inside_code | 8402 | 10 |
| 734 | `loc_ED7E` | `$ED7E` | label_inside_code | 8408 | 1 |
| 735 | `loc_ED7F` | `$ED7F` | label_inside_code | 8411 | 46 |
| 736 | `loc_EDAD` | `$EDAD` | label_inside_code | 8432 | 3 |
| 737 | `loc_EDB0` | `$EDB0` | label_inside_code | 8435 | 9 |
| 738 | `sub_EDB9` | `$EDB9` | code | 8446 | 11 |
| 739 | `loc_EDC4` | `$EDC4` | label_inside_code | 8454 | 27 |
| 740 | `locret_EDDF` | `$EDDF` | label_inside_code | 8467 | 1 |
| 741 | `loc_EDE0` | `$EDE0` | label_inside_code | 8471 | 28 |
| 742 | `sub_EDFC` | `$EDFC` | code | 8490 | 60 |
| 743 | `loc_EE38` | `$EE38` | label_inside_code | 8519 | 4 |
| 744 | `loc_EE3C` | `$EE3C` | label_inside_code | 8524 | 7 |
| 745 | `sub_EE43` | `$EE43` | code | 8535 | 38 |
| 746 | `loc_EE69` | `$EE69` | label_inside_code | 8555 | 8 |
| 747 | `sub_EE71` | `$EE71` | code | 8566 | 5 |
| 748 | `loc_EE76` | `$EE76` | label_inside_code | 8573 | 8 |
| 749 | `loc_EE7E` | `$EE7E` | label_inside_code | 8581 | 13 |
| 750 | `loc_EE8B` | `$EE8B` | label_inside_code | 8593 | 8 |
| 751 | `locret_EE93` | `$EE93` | label_inside_code | 8599 | 1 |
| 752 | `sub_EE94` | `$EE94` | code | 8607 | 21 |
| 753 | `locret_EEA9` | `$EEA9` | label_inside_code | 8617 | 1 |
| 754 | `sub_EEAA` | `$EEAA` | code | 8625 | 7 |
| 755 | `loc_EEB1` | `$EEB1` | label_inside_code | 8630 | 9 |
| 756 | `loc_EEBA` | `$EEBA` | label_inside_code | 8637 | 63 |
| 757 | `loc_EEF9` | `$EEF9` | label_inside_code | 8665 | 41 |
| 758 | `loc_EF22` | `$EF22` | label_inside_code | 8683 | 11 |
| 759 | `loc_EF2D` | `$EF2D` | label_inside_code | 8690 | 12 |
| 760 | `loc_EF39` | `$EF39` | label_inside_code | 8697 | 9 |
| 761 | `unk_EF42` | `$EF42` | data | 8706 | 4 |
| 762 | `unk_EF46` | `$EF46` | data | 8710 | 4 |
| 763 | `sub_EF4A` | `$EF4A` | code | 8718 | 38 |
| 764 | `locret_EF70` | `$EF70` | label_inside_code | 8734 | 1 |
| 765 | `apu_reset` | `$EF71` | code | 8743 | 8 |
| 766 | `apu_stop` | `$EF79` | code | 8754 | 4 |
| 767 | `loc_EF7D` | `$EF7D` | label_inside_code | 8760 | 19 |
| 768 | `apu_play` | `$EF90` | code | 8778 | 22 |
| 769 | `sub_EFA6` | `$EFA6` | code | 8800 | 28 |
| 770 | `loc_EFC2` | `$EFC2` | label_inside_code | 8819 | 4 |
| 771 | `loc_EFC6` | `$EFC6` | label_inside_code | 8823 | 28 |
| 772 | `loc_EFE2` | `$EFE2` | label_inside_code | 8839 | 48 |
| 773 | `loc_F012` | `$F012` | label_inside_code | 8863 | 4 |
| 774 | `loc_F016` | `$F016` | label_inside_code | 8867 | 5 |
| 775 | `locret_F01B` | `$F01B` | label_inside_code | 8871 | 1 |
| 776 | `apu_length_tab` | `$F01C` | data | 8876 | 216 |
| 777 | `sub_F0F4` | `$F0F4` | code | 8988 | 2 |
| 778 | `loc_F0F6` | `$F0F6` | label_inside_code | 8992 | 4 |
| 779 | `sub_F0FA` | `$F0FA` | code | 9001 | 4 |
| 780 | `apu_sound_tab` | `$F0FE` | data | 9008 | 72 |
| 781 | `sub_F146` | `$F146` | code | 9048 | 7 |
| 782 | `loc_F14D` | `$F14D` | label_inside_code | 9054 | 51 |
| 783 | `byte_F180` | `$F180` | data | 9092 | 16 |
| 784 | `word_F190` | `$F190` | data | 9095 | 18 |
| 785 | `byte_F1A2` | `$F1A2` | data | 9104 | 8 |
| 786 | `byte_F1AA` | `$F1AA` | data | 9106 | 8 |
| 787 | `byte_F1B2` | `$F1B2` | data | 9107 | 8 |
| 788 | `byte_F1BA` | `$F1BA` | data | 9109 | 13 |
| 789 | `byte_F1C7` | `$F1C7` | data | 9111 | 24 |
| 790 | `byte_F1DF` | `$F1DF` | data | 9114 | 12 |
| 791 | `byte_F1EB` | `$F1EB` | data | 9116 | 10 |
| 792 | `byte_F1F5` | `$F1F5` | data | 9118 | 20 |
| 793 | `byte_F209` | `$F209` | data | 9121 | 246 |
| 794 | `byte_F2FF` | `$F2FF` | data | 9146 | 115 |
| 795 | `byte_F372` | `$F372` | data | 9158 | 230 |
| 796 | `byte_F458` | `$F458` | data | 9181 | 128 |
| 797 | `byte_F4D8` | `$F4D8` | data | 9194 | 83 |
| 798 | `byte_F52B` | `$F52B` | data | 9203 | 25 |
| 799 | `byte_F544` | `$F544` | data | 9207 | 103 |
| 800 | `byte_F5AB` | `$F5AB` | data | 9218 | 45 |
| 801 | `byte_F5D8` | `$F5D8` | data | 9223 | 53 |
| 802 | `byte_F60D` | `$F60D` | data | 9229 | 226 |
| 803 | `byte_F6EF` | `$F6EF` | data | 9251 | 57 |
| 804 | `byte_F728` | `$F728` | data | 9257 | 61 |
| 805 | `byte_F765` | `$F765` | data | 9264 | 14 |
| 806 | `byte_F773` | `$F773` | data | 9267 | 16 |
| 807 | `byte_F783` | `$F783` | data | 9270 | 20 |
| 808 | `byte_F797` | `$F797` | data | 9273 | 19 |
| 809 | `byte_F7AA` | `$F7AA` | data | 9276 | 19 |
| 810 | `byte_F7BD` | `$F7BD` | data | 9279 | 27 |
| 811 | `byte_F7D8` | `$F7D8` | data | 9283 | 21 |
| 812 | `byte_F7ED` | `$F7ED` | data | 9286 | 16 |
| 813 | `byte_F7FD` | `$F7FD` | data | 9289 | 17 |
| 814 | `byte_F80E` | `$F80E` | data | 9292 | 19 |
| 815 | `byte_F821` | `$F821` | data | 9295 | 22 |
| 816 | `byte_F837` | `$F837` | data | 9298 | 21 |
| 817 | `byte_F84C` | `$F84C` | data | 9301 | 246 |
| 818 | `byte_F942` | `$F942` | data | 9326 | 143 |
| 819 | `byte_F9D1` | `$F9D1` | data | 9340 | 92 |
| 820 | `byte_FA2D` | `$FA2D` | data | 9350 | 35 |
| 821 | `byte_FA50` | `$FA50` | data | 9354 | 298 |
| 822 | `byte_FB7A` | `$FB7A` | data | 9383 | 97 |
| 823 | `byte_FBDB` | `$FBDB` | data | 9393 | 103 |
| 824 | `byte_FC42` | `$FC42` | data | 9404 | 307 |
| 825 | `byte_FD75` | `$FD75` | data | 9435 | 231 |
| 826 | `byte_FE5C` | `$FE5C` | data | 9458 | 21 |
| 827 | `byte_FE71` | `$FE71` | data | 9461 | 119 |
| 828 | `sub_FEE8` | `$FEE8` | code | 9478 | 16 |
| 829 | `loc_FEF8` | `$FEF8` | label_inside_code | 9488 | 5 |
| 830 | `locret_FEFD` | `$FEFD` | label_inside_code | 9492 | 1 |
| 831 | `loc_FEFE` | `$FEFE` | label_inside_code | 9496 | 11 |
| 832 | `loc_FF09` | `$FF09` | label_inside_code | 9503 | 247 |

Counts: `code` 201, `thunk` 56, `data` 203, `label_inside_code` 372.

### 1.1 Anomalies, notes and cross-checks

* **Order:** none. Source order == address order for all 832 labels.
* **Duplicates:** none (832 labels → 832 distinct addresses).
* **`continue_level_check` (`$CC36`, line 2222)** is the only `code` label in this file that is *not* covered by
  its own `; End of function` comment: IDA emitted one `; End of function start_game_check` at line 2246 that spans
  both. Factually it is a **second entry point** into the `start_game_check` region:
  * `start_game_check` (`$CB4B`, 2083) ends its own code at `locret_CC15` (`$CC15`, RTS, line 2185/2186),
  * the two data tables `unk_CC16` (`$CC16`) and `unk_CC26` (`$CC26`) then occupy `$CC16`–`$CC35`,
  * `continue_level_check` starts at `$CC36` and is reached **only** by the `BNE continue_level_check` at line 2151
    (there is no fall-through from the data tables).
  A code generator should therefore treat `$CC36` as a *separate* procedure (this matches the existing
  `c_src/analysis/procedures.json`, which lists 257 bank-7 procedures vs. the 256 IDA `End of function` blocks —
  the extra one is exactly `continue_level_check`).
* Kind naming: the existing `c_src/analysis/labels.json` marks the 56 trampolines as `kind: "code"` and records the
  trampoline-ness in `procedures.json` (`"thunk": true`). This report uses the requested kind name `thunk` for them.
* Unused procedures — see §2.9.
* `sub_C6CA`, `sub_C725` have no call sites — see §3.2.
---

## 2. Tricky constructs (exact line numbers)

### 2.1 Chopped procedures — procedures with no `RTS`/`JMP` that fall through into the next procedure

`A` = *pure* fall-through (the last instruction is not a control transfer at all — execution continues into the next procedure).
`B` = the last instruction is a conditional branch; the *not-taken* path falls into the next procedure.
`C` = the procedure ends with `JSR switch_case`; the inline jump table follows and control leaves through the dispatch (never falls through).

| proc | src lines | addr range | last instruction (line@addr) | falls into | class |
|---|---|---|---|---|---|
| `sub_C6AB` | 1029–1036 | $C6AB–$C6B4 | STA byte_9B (1035@$C6B3) | `sub_C6B5` | A |
| `sub_C6CA` | 1062–1068 | $C6CA–$C6D2 | JSR sub_C6EA (1067@$C6D0) | `stack_call_return` | A |
| `print_string` | 1089–1101 | $C6DA–$C6E9 | BCC stack_call_return (1100@$C6E8) | `sub_C6EA` | B |
| `far_call_prepare` | 1525–1534 | $C8A0–$C8AB | LDA ($12),Y (1533@$C8AA) | `far_call_restore` | A |
| `sub_CA0F` | 1791–1794 | $CA0F–$CA11 | LDA #8 (1793@$CA10) | `sub_CA12` | A |
| `ppu_disable_all` | 1932–1938 | $CACD–$CAD6 | AND #$E7 (1937@$CAD5) | `ppu_set_control2` | A |
| `ppu_nmi_disable` | 1969–1973 | $CAE7–$CAEA | AND #$7F (1972@$CAE9) | `ppu_set_control1` | A |
| `ppu_enable_all` | 1989–1993 | $CAF1–$CAF6 | JSR ppu_enable_picture (1992@$CAF4) | `ppu_nmi_enable` | A |
| `nmi_handler` | 2653–2683 | $CE8D–$CEC3 | PLA  (2682@$CEC3) | `dummy_irq_handler` | A |
| `sub_D18C` | 3136–3140 | $D18C–$D192 | LDA unk_D1E1,Y (3139@$D190) | `sub_D193` | A |
| `sub_D480` | 3802–3885 | $D480–$D4F9 | STA byte_14 (3884@$D4F8) | `sub_D4FA` | A |
| `sub_D4FA` | 3891–3898 | $D4FA–$D505 | JSR switch_case (3897@$D503) | `(unnamed data)` | C |
| `sub_D56B` | 4001–4039 | $D56B–$D5AA | STA $528,X (4038@$D5A8) | `sub_D5AB` | A |
| `sub_D7E3` | 4471–4515 | $D7E3–$D823 | STA byte_AB (4514@$D822) | `sub_D824` | A |
| `sub_D824` | 4521–4547 | $D824–$D849 | STA byte_A0 (4546@$D848) | `sub_D84A` | A |
| `sub_D84A` | 4553–4557 | $D84A–$D84E | LDY #0 (4556@$D84D) | `sub_D84F` | A |
| `sub_D84F` | 4563–4590 | $D84F–$D873 | JSR switch_case (4589@$D871) | `(unnamed data)` | C |
| `sub_D938` | 4748–4750 | $D938–$D939 | LDY byte_16 (4749@$D938) | `sub_D93A` | A |
| `sub_D93A` | 4756–4764 | $D93A–$D942 | STA byte_14 (4763@$D941) | `sub_D943` | A |
| `sub_D9E0` | 4872–4905 | $D9E0–$DA15 | BNE sub_DA28 (4904@$DA14) | `sub_DA16` | B |
| `sub_DA16` | 4911–4924 | $DA16–$DA27 | BNE loc_DA18 (4923@$DA26) | `sub_DA28` | B |
| `sub_DCC5` | 5459–5462 | $DCC5–$DCCA | STA $430,X (5461@$DCC8) | `sub_DCCB` | A |
| `sub_DE5F` | 5790–5820 | $DE5F–$DE90 | BNE loc_DE6E (5819@$DE8F) | `sub_DE91` | B |
| `sound_test_menu` | 6685–6705 | $E439–$E461 | JSR sub_DDF3 (6704@$E45F) | `sub_E462` | A |
| `sub_E462` | 6711–6776 | $E462–$E4CE | BNE sub_E462 (6775@$E4CD) | `show_ending_screen` | B |
| `sub_E60F` | 6967–7011 | $E60F–$E652 | BNE loc_E61E (7010@$E651) | `sub_E653` | B |
| `reset` | 7163–7203 | $E7BF–$E805 | JSR apu_reset (7202@$E803) | `main_menu_loop` | A |
| `main_menu_loop` | 7209–7220 | $E806–$E81D | JSR start_game_check (7219@$E81B) | `sub_E81E` | A |
| `sub_E81E` | 7226–7302 | $E81E–$E8A9 | BNE loc_E876 (7301@$E8A8) | `sub_E8AA` | B |
| `sub_E8C9` | 7335–7395 | $E8C9–$E938 | LDY #$FF (7394@$E937) | `level_completed_sequence` | A |
| `fade_palette_before_warp_another` | 7480–7484 | $E9A2–$E9A7 | BNE loc_E9AC (7483@$E9A6) | `fade_palette_before_warp` | B |
| `apu_fx_6` | 7991–7994 | $EC05–$EC0A | STA byte_300 (7993@$EC08) | `apu_fx_2` | A |
| `apu_fx_2` | 8000–8003 | $EC0B–$EC0F | STA $31A,X (8002@$EC0D) | `apu_fx_5` | A |
| `apu_fx_4` | 8039–8044 | $EC1F–$EC27 | BPL sub_EC2F (8043@$EC26) | `apu_fx_29` | B |
| `apu_fx_29` | 8050–8054 | $EC28–$EC2E | AND #$F0 (8053@$EC2D) | `sub_EC2F` | A |
| `apu_fx_10` | 8107–8125 | $EC4F–$EC6A | LDA #$F (8124@$EC69) | `sub_EC6B` | A |
| `sub_EC6B` | 8131–8137 | $EC6B–$EC73 | ORA byte_1E (8136@$EC72) | `apu_fx_7` | A |
| `apu_fx_14_15` | 8253–8265 | $ECF5–$ED07 | ORA #$80 (8264@$ED06) | `sub_ED08` | A |
| `sub_ED08` | 8271–8274 | $ED08–$ED0B | EOR #$80 (8273@$ED0A) | `sub_ED0C` | A |
| `apu_fx_20` | 8299–8307 | $ED15–$ED24 | BMI sub_ED2F (8306@$ED23) | `sub_ED25` | B |
| `sub_ED25` | 8313–8318 | $ED25–$ED2E | ORA #$C0 (8317@$ED2D) | `sub_ED2F` | A |
| `apu_fx_23_24_25` | 8344–8348 | $ED3C–$ED43 | STA unk_31A,X (8347@$ED41) | `apu_fx_12` | A |
| `apu_reset` | 8743–8748 | $EF71–$EF78 | STA byte_308 (8747@$EF76) | `apu_stop` | A |
| `sub_F0FA` | 9001–9005 | $F0FA–$F0FD | BNE loc_F0F6 (9004@$F0FC) | `apu_sound_tab` | B |

Notable ones:

* **`reset` → `main_menu_loop`** (lines 7163–7203 → 7209): after `JSR apu_reset` (line 7202) execution continues into `main_menu_loop`, so the cold-boot path *never returns*; `main_menu_loop` is the real top-level loop.
* **`main_menu_loop` → `sub_E81E`** (7209–7220 → 7226) and **`sub_E8C9` → `level_completed_sequence`** (7335–7395 → 7402).
* **`nmi_handler` → `dummy_irq_handler`** (2653–2683 → 2689): the NMI handler ends with three `PLA` (lines 2678–2682) and then executes the single `RTI` at line 2690.
* **`sub_C6CA` → `stack_call_return`** (1062–1068 → 1075): `PLA/STA $16/PLA/STA $17/JSR sub_C6EA` then falls into the shared return thunk.
* **`far_call_prepare` → `far_call_restore`** (1525–1534 → 1540): `far_call_prepare` pushes 3 bytes of the far address and falls into `far_call_restore`.
* **`sub_CA0F` → `sub_CA12`** (1791–1794 → 1800): `PHA; LDA #8` then falls into `sub_CA12`, which pops the saved A at line 1803.
* **`ppu_disable_all` → `ppu_set_control2`** (1932–1938 → 1945) and **`ppu_nmi_disable` → `ppu_set_control1`** (1969–1973 → 1979) and **`ppu_enable_all` → `ppu_nmi_enable`** (1989–1993 → 1999). All three are “compute A then fall into the shared `STA <reg>` tail”.
* `print_string` (1100): `BCC stack_call_return` is the last instruction; the *not-taken* successor at `$C6EA` is `sub_C6EA`. Since the branch is taken exactly when the string loop is finished, `print_string` always leaves through `stack_call_return` (line 1100) — see §2.2/§2.3.
* `sub_D4FA` (3898) and `sub_D84F` (4590) end with `JSR switch_case`; the words at `$D506`/`$D874` are the case table (class C, §2.4).
* `sub_F0FA` (line 9005, `BNE loc_F0F6`) falls through into `apu_sound_tab` — the *only* fall-through in the file that lands in a data table. Treat the `BNE` as unconditional (the branch is taken on every observed path); the following bytes are data, not code.

### 2.2 Common tails / one `RTS` shared by several entry points

| shared tail | addr | entered by |
|---|---|---|
| `stack_call_return` | `$C6D3` | `sub_C6CA` fall-through (1068); `print_string` `BCC` (1100); `sub_C725` `JMP` (1194) |
| `locret_C724` (RTS) | `$C724` | `sub_C6EA` fall-through (1148→1149); `sub_C6EA` `BCS` at 1126 |
| `locret_C7EA` (RTS) | `$C7EA` | `sub_C7D8` fall-through (1322→1324); `sub_C7EB` `BCC` at 1335 |
| `sub_C6EA` | `$C6EA` | `sub_C6CA` `JSR` (1067); `print_string` `JSR` (1098) |
| `dummy_irq_handler` (RTI) | `$CEC4` | `nmi_handler` fall-through (2683) |
| `ppu_set_control1` | `$CAEB` | `ppu_nmi_disable` fall-through (1973); `reset` `JSR` (7172) |
| `ppu_set_control2` | `$CAD7` | `ppu_disable_all` fall-through (1938); `JSR` from `reset` (7174) and `sub_E81E` (7245); `JSR ppu_enable_picture` is the separate `$CAE1` routine |
| `sub_C7F8` | `$C7F8` | shared cursor helper; 13 `JSR` sites (lines 1129, 1167, 1710, 1802, 2473, 3513, 3604, 5830, 5888, 6148, 6173, 6221, 6900) plus the `sub_C03C` thunk at 202 |

Multiple *entry points* into one body:

* `sub_C6EA` is entered at `$C6EA` (1107), and internally at `loc_C6F7` (`$C6F7`, 1117), `loc_C6F8` (`$C6F8`, 1120), `loc_C6FB` (`$C6FB`, 1124). The `A` register arriving at `loc_C6FB` is a length in characters and at `loc_C6F8` it is the raw control byte.
* `sub_C7D8` body is entered at `$C7D8` (1307) and its tail at `loc_C7E9` (`$C7E9`, 1320) / `locret_C7EA` (`$C7EA`, 1324).
* `sub_D5AB` is the shared tail of `sub_D510`, `sub_D51A`, `sub_D528`, `sub_D54D`, `sub_D559` (all `JMP sub_D5AB`, lines 3915/3929/3964/3977/3994) and itself ends with `JMP switch_bank_6` (4060).
* `sub_CA12` is the shared body of `sub_CA0F` (1791, fall-through, sets A=8) and `sub_CA97` (1877, `JMP`, sets A=9); both `PHA` the original `A` which `sub_CA12` pops at line 1803.
* `sub_D84F` is the shared tail of `sub_D84A` (fall-through 4557→4563) and of `sub_DA28` (`JMP sub_D84F`, line 4939).
* `sub_DA28` is the shared tail of `sub_D886`/`sub_D894`/`sub_D8A1`/`sub_D8DC`/`sub_D920` (`JMP`, lines 4613/4626/4649/4720/4741) and of `sub_D93A` (`JMP sub_DA28`, line 4759).
* `sub_D943` is entered at `$D943` and at `loc_D941` (`$D941`, 4762).

### 2.3 Stack manipulation for parameter passing / return-address reading

#### `print_string` — `$C6DA` (lines 1089–1101)

     1071: ; =============== S U B R O U T I N E =======================================
     1072: 
     1073: ; Return to the parent procedure that used parameter passing through the code stream
     1074: 
     1075: stack_call_return:                      ; CODE XREF: print_string+E↓j
     1076:                                         ; sub_C725+40↓j
     1077:                 LDA     byte_17
     1078:                 PHA
     1079:                 LDA     byte_16
     1080:                 PHA
     1081:                 RTS
     1082: ; End of function stack_call_return
     1083: 
     1084: 
     1085: ; =============== S U B R O U T I N E =======================================
     1086: 
     1087: ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
     1088: 
     1089: print_string:                           ; CODE XREF: show_main_menu+198↓p
     1090:                                         ; show_main_menu+1A8↓p ...
     1091:                 PLA
     1092:                 STA     byte_16
     1093:                 PLA
     1094:                 STA     byte_17
     1095: 
     1096: loc_C6E0:                               ; CODE XREF: print_string+C↓j
     1097:                 JSR     sub_C8BA
     1098:                 JSR     sub_C6EA
     1099:                 BCS     loc_C6E0
     1100:                 BCC     stack_call_return ; Return to the parent procedure that used parameter passing through the code stream
     1101: ; End of function print_string

* Calling convention: `JSR print_string` is immediately followed **in the instruction stream** by
  `[ctrl][vram_lo][vram_hi][text…]`. `sub_C6EA` decodes `ctrl` at lines 1109–1123:
  * `bit7 = 0` → block length = `ctrl + 3` (text length = `ctrl`),
  * `bit7 = 1, bit6 = 0` → block length = `(ctrl & $7F) + 3`,
  * `bit7 = 1, bit6 = 1` → block length = `8` (`LDA #8`, line 1114) — an escape for an 8-byte block (5 text bytes).
* 1091/1093 `PLA`/`PLA` pop the `JSR` return address into `$16`/`$17`; the `+1` effect of the eventual `RTS` in
  `stack_call_return` re-adds the “-1” that `JSR` pushed, so the final `RTS` returns to the byte **after** the inline parameter block.
* 1097 `JSR sub_C8BA` — wait for the PPU to be idle.
* 1098 `JSR sub_C6EA` — print one string, advancing `$16/$17` by the number of consumed bytes.
* 1099 `BCS loc_C6E0` — `sub_C6EA` returns carry **set** when more strings follow.
* 1100 `BCC stack_call_return` — chain to the shared `PLA/PLA/RTS` at `$C6D3`.
* `sub_C6EA` ($C6EA, 1107–1150) and `sub_C725` ($C725, 1157–1195) are the same idiom:
  * `sub_C6EA` reads byte `($16),1` (the `ctrl` byte), maps it through `sub_C7D8` (PPU-write-window test: carry **set** = “cannot write the name table right now”, lines 1307–1326) and `sub_C7F8`
  (VRAM-address → `$20`/`$21` cursor), copies `length` bytes from `($16),1` into the staging buffer at `$460,X`, then
  calls `sub_C807` to advance the VRAM cursor and adds `length` to `$16/$17`. It returns carry **set** at `locret_C724`
  when `sub_C7D8` says the window is closed (nothing consumed); `print_string` then waits (`JSR sub_C8BA`) and retries.
  Called **by `print_string` and `sub_C6CA` only**.
  * `sub_C725` reads a count at `($16),1`, computes `count+3`, positions the cursor, writes the fixed bytes `$19,$18` at `$461,X`/`$462,X` (the VRAM address), then copies `count` characters from `($16),0` to `$463,X`…; it ends with `JMP stack_call_return` (1194).
  * **`sub_C6CA` and `sub_C725` have no callers anywhere in bank7 (or in banks 4/5/6 / the API files).** They are dead entry points kept for completeness (reachable only via the `$C1xx` API table in another bank, if at all).

#### `stack_call_return` — `$C6D3` (lines 1075–1081)

     1075: stack_call_return:                      ; CODE XREF: print_string+E↓j
     1076:                                         ; sub_C725+40↓j
     1077:                 LDA     byte_17
     1078:                 PHA
     1079:                 LDA     byte_16
     1080:                 PHA
     1081:                 RTS
     1082: ; End of function stack_call_return

Pushes `$17` (high) then `$16` (low) and executes `RTS`, i.e. `goto ($17,$16)+1`. Used only to return to the caller that pushed an inline parameter block.

#### `switch_case` — `$C7C6` (lines 1285–1301)

     1283: ; Old-school Switch-Case. Right after the call there is a jump table of variable size.
     1284: 
     1285: switch_case:                            ; CODE XREF: j_switch_case↑j
     1286:                                         ; sub_D4FA+9↓p ...
     1287:                 ASL
     1288:                 TAY
     1289:                 PLA
     1290:                 STA     temp_0
     1291:                 PLA
     1292:                 STA     temp_1
     1293:                 INY
     1294:                 INY
     1295:                 LDA     (temp_0),Y
     1296:                 PHA
     1297:                 DEY
     1298:                 LDA     (temp_0),Y
     1299:                 PHA
     1300:                 RTS
     1301: ; End of function switch_case

`A` = case index. `ASL/TAY`; `PLA/PLA` recover the `JSR` return address (**which is `retaddr-1` because `JSR` pushes `PC-1`**) into `$00/$01`
(`temp_0`/`temp_1`); the 16-bit word at `(retaddr-1)+2*case+1` … i.e. the little-endian word stored at `retaddr+2*case` …
is pushed **low-then-high** and `RTS` transfers control to `word+1`. The table therefore stores `target-1`.
Net stack effect: exactly one `JSR` frame is consumed, so the dispatched handler returns straight to `switch_case`'s caller.

#### `far_call` — `$C87F` (lines 1498–1519), `j_far_call` = `$C006`, *documented as unused*

     1494: ; =============== S U B R O U T I N E =======================================
     1495: 
     1496: ; Not used. Variables 10,11,12,13 are used to store the context (registers A, Y, PC are saved). Current bank saved on stack.
     1497: 
     1498: far_call:                               ; CODE XREF: j_far_call↑j
     1499:                 STA     byte_10
     1500:                 STY     byte_11
     1501:                 PLA
     1502:                 STA     byte_12
     1503:                 CLC
     1504:                 ADC     #3
     1505:                 TAY
     1506:                 PLA
     1507:                 STA     byte_13
     1508:                 ADC     #0
     1509:                 PHA
     1510:                 TYA
     1511:                 PHA
     1512:                 LDA     last_bank_num
     1513:                 PHA
     1514:                 JSR     far_call_prepare
     1515:                 STA     byte_10
     1516:                 STY     byte_11
     1517:                 PLA
     1518:                 JMP     far_call_restore
     1519: ; End of function far_call
     1520: 
     1521: 
     1522: ; =============== S U B R O U T I N E =======================================
     1523: 
     1524: 
     1525: far_call_prepare:                       ; CODE XREF: far_call+16↑p
     1526:                 LDY     #3
     1527:                 LDA     ($12),Y
     1528:                 PHA
     1529:                 DEY
     1530:                 LDA     ($12),Y
     1531:                 PHA
     1532:                 DEY
     1533:                 LDA     ($12),Y
     1534: ; End of function far_call_prepare
     1535: 
     1536: 
     1537: ; =============== S U B R O U T I N E =======================================
     1538: 
     1539: 
     1540: far_call_restore:                       ; CODE XREF: far_call+1E↑j
     1541:                 STA     last_bank_num
     1542:                 TAY
     1543:                 LDA     bank_remap_table,Y
     1544:                 STA     bank_remap_table,Y
     1545:                 LDA     byte_10
     1546:                 LDY     byte_11
     1547:                 RTS
     1548: ; End of function far_call_restore

* `far_call` saves `A`/`Y`/`PC` in `$10`/`$11`/`$12`/`$13`, pushes the *current* `PC+3` and `last_bank_num`, then `JSR far_call_prepare`
  which pushes 3 more bytes from `($12),0..3`, and finally tail-jumps into `far_call_restore`, which reselects the bank and `RTS`es
  with a new `A`/`Y` from `$10`/`$11`.
* `j_far_call` has exactly one reference (the thunk `JMP` at line 35). No `JSR j_far_call` exists in any bank.

#### `sub_C8DF` — `$C8DF` (lines 1585–1629): far-pointer loader / bank-call stub

     1585: sub_C8DF:                               ; CODE XREF: sub_C042↑j
     1586:                                         ; start_game_check+A↓p ...
     1587:                 STA     byte_15
     1588:                 STY     byte_14
     1589:                 TYA
     1590:                 AND     #$F
     1591:                 TAY
     1592:                 LDA     last_bank_num
     1593:                 PHA
     1594:                 LDA     byte_15
     1595:                 ASL
     1596:                 ADC     unk_C928,Y
     1597:                 PHA
     1598:                 LDA     unk_C930,Y      ; 0x80 = demo mode
     1599:                 ADC     #0
     1600:                 PHA
     1601:                 LDA     byte_14
     1602:                 PHA
     1603:                 LDA     byte_2B
     1604:                 BEQ     loc_C901
     1605:                 JSR     sub_C8BA
     1606: 
     1607: loc_C901:                               ; CODE XREF: sub_C8DF+1D↑j
     1608:                 PLA
     1609:                 STA     byte_26
     1610:                 JSR     switch_bank     ; A: bank number
     1611:                 PLA
     1612:                 STA     byte_15
     1613:                 PLA
     1614:                 STA     byte_14
     1615:                 LDY     #0
     1616:                 LDA     ($14),Y
     1617:                 STA     byte_2C
     1618:                 INY
     1619:                 LDA     ($14),Y
     1620:                 STA     byte_2D
     1621:                 LDA     byte_18
     1622:                 STA     byte_2E
     1623:                 LDA     byte_19
     1624:                 STA     byte_2F
     1625:                 LDA     #2
     1626:                 STA     byte_2B
     1627:                 PLA
     1628:                 JMP     switch_bank     ; A: bank number
     1629: ; End of function sub_C8DF

* Convention (all 9 call sites): `A` = word index into the target bank's table at `$8030`, `Y` = bank number (always `4` in bank7).
* 1592 saves `last_bank_num`; 1593–1600 compute and push the *address* `$8030 + 2*A` (via `unk_C928`/`unk_C930`); 1601–1602 push `Y`.
* 1607–1610 pop the bank and `JSR switch_bank`; 1611–1614 pop the address into `$14`/`$15`; 1615–1620 `LDA ($14),Y` read the 16-bit
  pointer stored in the selected bank and put it in `$2C`/`$2D`; 1621–1624 publish `$18`/`$19` into `$2E`/`$2F`; 1625–1626 set `byte_2B = 2`;
* 1627–1628 pop the saved bank and `JMP switch_bank` — i.e. the routine **restores the previous bank before returning**.
* The `PLA`s here are **not** return-address reads: `sub_C8DF` only pops the values it pushed itself.

#### other `PHA`/`PLA` used purely for register save/restore across bank switches

* `switch_bank` (`$C815`, 1380–1391): `TYA/PHA … PLA/TAY` to preserve `Y`.
* `switch_bank_0_unused`…`switch_bank_6` (`$C824`–`$C872`, 1397–1491): `PHA … PLA` to preserve `A`.
* `sub_C985` 1743–1744 `LDA last_bank_num/PHA`, 1780–1781 `PLA/JSR switch_bank`; also 1707/1711 and 1714/1782 for `A` and `X`.
* `sub_C8BA` (1554–1579): `TYA/PHA` at the top, `PLA/TAY` at the bottom (level/PPU busy wait).
* `nmi_handler` 2654–2658/2678–2682: `PHA/TXA/PHA/TYA/PHA` … `PLA/TAY/PLA/TAX/PLA`.
* `sub_D753` 4356–4382 pushes/restores `byte_AA`/`byte_AB` (`PHA` 4359/4361, `PLA` 4367/4369) — these are *not* return-address reads.
* `far_call` 1501/1506 `PLA`/`PLA` **does** read the `JSR` return address (into `$12`/`$13`); it is the fifth return-address reader after `sub_C6CA`, `print_string`, `sub_C725` and `switch_case`.
* `apu_fx_5` (`$EC10`, 8009–8015) executes **four** `PLA` and then `JMP loc_EB17`: it discards the 2 bytes left over by the `RTS`
  dispatch in `sub_EB7A` (the `$EB85` return address) *plus* 2 more bytes, i.e. it unwinds one extra return frame before rejoining `apu_update`.

### 2.4 Computed / indirect control flow

* **`JMP (addr)` — none.** There is not a single `JMP (…)` instruction in the file (verified by exhaustive scan).
* **RTS-to-computed-address** — three places:
  1. `switch_case` `$C7C6` (line 1300) — inline jump table, used at lines 3897 and 4589 (§3).
  2. `sub_EB7A` `loc_EB98` `$EB98` (lines 7908–7922) — pushes return address `$EB85` and `word_EBAF[A*2]`, then `RTS`.
     `word_EBAF` = `$EBAF`–`$EBEA`, 30 words, entries are `handler-1` (`; fx N`); a dispatched handler `RTS`es back to `$EB86`.
  3. `sub_EFA6` `$EFA6` (lines 8812–8816) — pushes `byte_1E`/`byte_1F` (a pointer fetched from `apu_sound_tab`) and `RTS`es into the sound-data handler.
* **JMP to an address computed from a table** — `sub_D510`/`sub_D51A`/… via `JMP sub_D5AB`, and the `word_F190` indirection (§5.3). Also the dispatch through `$D506`/`$D874` above.
* **Branch into the middle of a routine** — many; the `loc_*` labels in §1 are exactly the legal entry points.

### 2.5 Inline data embedded in the instruction stream

Four kinds exist; a C translator must model them as *call-with-parameter-block*, not as fall-through code.

**(a) `print_string` parameter blocks** — 4 sites (also listed in §3.1):

| site (JSR line) | param bytes lines | count | VRAM addr | text |
|---|---|---|---|---|
| 6242 | 6244–6247 | `$0A` = 10 | `$2B22` | `GAME START` |
| 6249 | 6251–6254 | 8 | `$8B22` | `CONTINUE` |
| 6692 | 6694–6697 | `$0A` = 10 | `$8820` | `SOUND TEST` |
| 6815 | 6817–6820 | 5 | `$8920` | `SCORE` |

**(b) `switch_case` jump tables** — 2 sites, immediately after the `JSR switch_case`:

| site (JSR line) | table | addr | words | targets (`word+1`) |
|---|---|---|---|---|
| 3897 | `$D506` | `$D506`–`$D50F` | 5 | `$D510`, `$D51A`, `$D528`, `$D54D`, `$D559` |
| 4589 | `$D874` | `$D874`–`$D885` | 9 | `$D886`, `$D894`, `$D8A1`, `$D8BF`, `$D8DC`, `$D920`, `$D938`, `$D9E0`, `$DA16` (stored as `$D885`,`$D893`,`$D8A0`,`$D8BE`,`$D8DB`,`$D91F`,`$D937`,`$D9DF`,`$DA15`) |

(the last target of the second table is `$DA16`; the table lives at `$D874`–`$D885`.)

**(c) return-address readers** — five routines `PLA` the `JSR` return address: `sub_C6CA` (1063/1065), `print_string` (1091/1093), `sub_C725` (1158/1160), `switch_case` (1289/1291) and `far_call` (1501/1506). Only the first four treat the bytes after the `JSR` as data; `far_call` only needs the return address to compute the far target.

**(d) tables that physically live *inside* a procedure's address span** (the labels are real `data` labels, but they are
reached by fall-through from code that never executes them — the routine must `JMP`/`RTS` around them):

| label | addr | size | inside | read at |
|---|---|---|---|---|
| `unk_CC16` | `$CC16` | 16 | `start_game_check` 2083–2246 | line 2173 `LDA unk_CC16,Y` |
| `unk_CC26` | `$CC26` | 16 | `start_game_check` | line 2175 `LDA unk_CC26,Y` |
| `aGameStart` | `$E15D` | 16 | `show_main_menu` 6034–6347 | inline `print_string` text |
| `aContinue` | `$E16D` | 23 | `show_main_menu` | inline `print_string` text |
| `aSoundTest` | `$E44A` | 24 | `sound_test_menu` 6685–6705 | inline `print_string` text |
| `aScore` | `$E521` | 53 | `show_ending_screen` 6782–6961 | inline `print_string` text |
| `byte_E907` | `$E907` | 16 | `sub_E8C9` 7335–7395 | line 7352 `LDA byte_E907,Y` |

All 54 `.BYTE`/`.WORD` lines that sit inside a code span are enumerated in §4.4.

### 2.6 Hand-assembled bytes that force *absolute* addressing

DASM would assemble `LDA $44,X` as zero-page indexed; the author emitted the opcode bytes directly:

| src line | addr | bytes | meaning | inside |
|---|---|---|---|---|
| 1770 | `$C9EC` | `BD 44 00` | `LDA $0044,X` | `sub_C985` |
| 1777 | `$C9FE` | `9D 44 00` | `STA $0044,X` | `sub_C985` |
| 1845 | `$CA60` | `BD 44 00` | `LDA $0044,X` | `sub_CA12` |
| 1851 | `$CA6F` | `9D 44 00` | `STA $0044,X` | `sub_CA12` |

`$0044,X` is an absolute-indexed access to the byte array based at zero-page `$0044` (named `unk_44` in `src/vars.asm`).
The index is a tile number loaded just before (`LDA $462,X / TAX`), so the array is indexed 0–255 and therefore overlaps `$0100`–`$0143`.
*(Speculation: the actual extent of the array is 256 bytes; `vars.asm` only names a few bytes in that range.)*

### 2.7 Self-modifying code / writes into `$8000`–`$FFFF`

* **There is no self-modifying code in the usual sense**: no `STA`/`STX`/`STY` targeted at an instruction operand or
  at a `JMP`-table slot inside the fixed bank. The only absolute stores at `$8000`+ are the nine `bank_remap_table` writes
  (`$C0F8`+n), and those are **mapper bank-select writes**, not memory writes (§5).
* The four hand-assembled instructions in §2.6 are the only other “code bytes written by hand”, and they are static.
* No `STA $8000`/`STA $FFFF`-style literal store exists either (exhaustive operand scan, §6).

### 2.8 Use of the stack pointer value itself (`TSX`/`TXS`)

| src line | addr | instruction | purpose |
|---|---|---|---|
| 1918 | `$CAEE` | `TSX` (inside `randomize`, 1909–1926) | copies `SP` into `X`, then `TXA` at 1919 and adds it into the PRNG state (`STA last_randomize_result`), i.e. the stack pointer is used as an entropy source. |
| 7170 | `$E7C6` | `TXS` (inside `reset`, after `LDX #$FF`) | initialises `SP = $FF`. |
| 7212 | `$E7D4` | `TXS` (inside `main_menu_loop`, after `LDX #$FF`) | re-initialises `SP = $FF` every time the main menu is (re)entered. |

There are no `TSX`-then-`STA $0100,X` style stack-frame walks other than `randomize`.

### 2.9 Other quirks worth flagging

* `sub_D6DF` line 4306 uses `LDA $D74A,X` with `X` running 8…1 to read the 8 bytes of `tab_D74B` (`$D74B`–`$D752`) —
  the table's *effective base* is `$D74A`, one byte before the label (IDA annotated it `; tab_D74B - 1`).
* `sub_EB7A` `loc_EB91` (7902–7905) `JMP apu_fx_26_27` — a jump into the middle of another handler.
* `apu_fx_21` (`$ECA4`, 8187–8193) `JMP apu_fx_0` — tail-merge of the “advance the script pointer” tail.
* Self-looping procedures (infinite loops, exit only via NMI/branch): `sub_C7EB` (1339), `sub_D12C` (3129),
  `sub_D77B` (4400), `sub_D943` (4865), `sub_E462` (6775 `BNE self`), `sub_FEE8` (9507).
* `ppu_update_background` ends `JMP loc_CFA2` (2889), i.e. it re-enters its own inner loop.
* **Dead code** (verified: zero symbolic references anywhere in `src/bank4..7.asm` / `*_api.asm` / `bank7.lst`):
  `j_far_call` (`$C006`, line 34), `switch_bank_0_unused` (`$C824`), `switch_bank_1_unused` (`$C831`),
  `switch_bank_2_unused` (`$C83E`), `switch_bank_3_unused` (`$C84B`), **`switch_bank_4` (`$C858`)**,
  `clear_oam_buffer_unused` (`$CCD4`, line 2335), `sub_D2C4` (`$D2C4`, line 3443), `sub_C6CA` (`$C6CA`),
  `sub_C725` (`$C725`), and the whole duplicate API block `j_apu_reset_0` (`$EA3D`), `j_apu_stop_0` (`$EA40`),
  `j_apu_play_1` (`$EA43`), `j_apu_update_0` (`$EA46`), `sub_EA49`, `sub_EA4C`, `sub_EA4F` (lines 7638–7694).
* `apu_fx_0`…`apu_fx_29` and `sub_EC2F`/`sub_EC6B`/`sub_ED08`/`sub_ED0C`/`sub_ED25`/`sub_ED2F`/`apu_fx_7`/`apu_fx_12`
  have **no symbolic references**: they are reached only through the `word_EBAF` RTS dispatch table (§2.4) or by
  fall-through. A naive “unused procedure” detector will wrongly delete them.
* `reset` (7163) is the hardware reset vector target; `nmi_handler` (2653) the NMI vector; `dummy_irq_handler` (2689) the IRQ vector.
  Vectors are emitted at lines 9517–9519 (`nmi_handler`, `reset`, `dummy_irq_handler`) at `$FFFA`–`$FFFF`.
---

## 3. Call sites and calling conventions

### 3.1 `JSR print_string` — 4 sites

**line 6242** (`$E157`), inside `show_main_menu`

```asm
 6236:                 BNE     loc_E145
 6237:                 JSR     sub_C807
 6238:                 INC     byte_70
 6239:                 LDA     byte_70
 6240:                 CMP     #$F7
 6241:                 BNE     loc_E129
 6242:                 JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
 6243: ; ---------------------------------------------------------------------------
 6244:                 .BYTE  $A
 6245:                 .BYTE $22 ; "
 6246:                 .BYTE $2B ; +
 6247: aGameStart:     .BYTE "GAME START"
 6248: ; ---------------------------------------------------------------------------
 6249:                 JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
 6250: ; ---------------------------------------------------------------------------
 6251:                 .BYTE   8
 6252:                 .BYTE $22 ; "
 6253:                 .BYTE $8B
 6254: aContinue:      .BYTE "CONTINUE"
 6255: ; ---------------------------------------------------------------------------
```

**line 6249** (`$E167`), inside `show_main_menu`

```asm
 6243: ; ---------------------------------------------------------------------------
 6244:                 .BYTE  $A
 6245:                 .BYTE $22 ; "
 6246:                 .BYTE $2B ; +
 6247: aGameStart:     .BYTE "GAME START"
 6248: ; ---------------------------------------------------------------------------
 6249:                 JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
 6250: ; ---------------------------------------------------------------------------
 6251:                 .BYTE   8
 6252:                 .BYTE $22 ; "
 6253:                 .BYTE $8B
 6254: aContinue:      .BYTE "CONTINUE"
 6255: ; ---------------------------------------------------------------------------
 6256:                 LDA     #$C6
 6257:                 STA     byte_201
 6258:                 LDA     #0
 6259:                 STA     byte_202
 6260:                 LDA     #$4C ; 'L'
 6261:                 STA     byte_203
 6262: 
```

**line 6692** (`$E444`), inside `sound_test_menu`

```asm
 6686:                                         ; sound_test_menu+9↓j
 6687:                 JSR     pad_read        ; Poll the status of the controller (pad) and save the state of the buttons
 6688:                 LDA     pad_buttons_1
 6689:                 ORA     pad_buttons_2
 6690:                 AND     #$C0
 6691:                 BNE     sound_test_menu
 6692:                 JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
 6693: ; ---------------------------------------------------------------------------
 6694:                 .BYTE  $A
 6695:                 .BYTE $20
 6696:                 .BYTE $88
 6697: aSoundTest:     .BYTE "SOUND TEST"
 6698: ; ---------------------------------------------------------------------------
 6699:                 LDA     #1
 6700:                 STA     byte_6F
 6701:                 LDA     #1
 6702:                 STA     byte_70
 6703:                 JSR     sub_C6AB
 6704:                 JSR     sub_DDF3
 6705: ; End of function sound_test_menu
```

**line 6815** (`$E51B`), inside `show_ending_screen`

```asm
 6809:                 JSR     sub_C8DF
 6810:                 JSR     sub_C8BA
 6811:                 LDA     #$20 ; ' '
 6812:                 LDX     #$20 ; ' '
 6813:                 LDY     #$AA
 6814:                 JSR     clear_screen    ; Clear BG
 6815:                 JSR     print_string    ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
 6816: ; ---------------------------------------------------------------------------
 6817:                 .BYTE   5
 6818:                 .BYTE $20
 6819:                 .BYTE $89
 6820: aScore:         .BYTE "SCORE"
 6821: ; ---------------------------------------------------------------------------
 6822:                 LDA     #$20 ; ' '
 6823:                 STA     PPU_ADDRESS
 6824:                 LDA     #$8E
 6825:                 STA     PPU_ADDRESS
 6826:                 LDY     #$7A ; 'z'
 6827:                 JSR     sub_E266
 6828:                 LDY     #$19
```

Parameter block layout: `[count] [vram_low] [vram_high] [count characters]`.
The callee advances `$16/$17` past exactly `3+count` bytes and finally `RTS`es to `$16+1` (see §2.3).

### 3.2 `JSR sub_C6CA` and `JSR sub_C725` — **no call sites**

An exhaustive scan of `src/*.asm` (bank4, bank5, bank6, bank6_api, bank7_api) and of `Build/bank7.lst`
finds **zero** `JSR sub_C6CA` and **zero** `JSR sub_C725`. Both routines are dead code in this ROM revision.
Their internal calling convention is nevertheless complete and unambiguous (they are the same
“inline parameter block” convention as `print_string`) — full bodies:

```asm
 1059: ; =============== S U B R O U T I N E =======================================
 1060: 
 1061: 
 1062: sub_C6CA:
 1063:                 PLA
 1064:                 STA     byte_16
 1065:                 PLA
 1066:                 STA     byte_17
 1067:                 JSR     sub_C6EA
 1068: ; End of function sub_C6CA
 1069: 
 1070: 
 1071: ; =============== S U B R O U T I N E =======================================
 1072: 
 1073: ; Return to the parent procedure that used parameter passing through the code stream
 1074: 
 1075: stack_call_return:                      ; CODE XREF: print_string+E↓j
 1076:                                         ; sub_C725+40↓j
 1077:                 LDA     byte_17
 1078:                 PHA
 1079:                 LDA     byte_16
 1080:                 PHA
 1081:                 RTS
 1082: ; End of function stack_call_return
 1083: 
 1084: 
 1085: ; =============== S U B R O U T I N E =======================================
 1086: 
 1087: ; Uses old-school parameter passing, via stack and return address. After JSR is byte - number of Chars, then Word - VRAM address, then variable size string for output.
 1088: 
 1089: print_string:                           ; CODE XREF: show_main_menu+198↓p
 1090:                                         ; show_main_menu+1A8↓p ...
 1091:                 PLA
 1092:                 STA     byte_16
 1093:                 PLA
 1094:                 STA     byte_17
 1095: 
 1096: loc_C6E0:                               ; CODE XREF: print_string+C↓j
 1097:                 JSR     sub_C8BA
 1098:                 JSR     sub_C6EA
 1099:                 BCS     loc_C6E0
 1100:                 BCC     stack_call_return ; Return to the parent procedure that used parameter passing through the code stream
 1101: ; End of function print_string
 1102: 
 1103: 
 1104: ; =============== S U B R O U T I N E =======================================
 1105: 
 1106: 
 1107: sub_C6EA:                               ; CODE XREF: sub_C6CA+6↑p
 1108:                                         ; print_string+9↑p
 1109:                 LDY     #1
 1110:                 LDA     ($16),Y
 1111:                 BPL     loc_C6F8
 1112:                 ASL
 1113:                 BPL     loc_C6F7
 1114:                 LDA     #8
 1115:                 BNE     loc_C6FB
 1116: 
 1117: loc_C6F7:                               ; CODE XREF: sub_C6EA+7↑j
 1118:                 LSR
 1119: 
 1120: loc_C6F8:                               ; CODE XREF: sub_C6EA+4↑j
 1121:                 CLC
 1122:                 ADC     #3
 1123: 
 1124: loc_C6FB:                               ; CODE XREF: sub_C6EA+B↑j
 1125:                 JSR     sub_C7D8
 1126:                 BCS     locret_C724
 1127:                 STX     byte_10
 1128:                 STA     byte_11
 1129:                 JSR     sub_C7F8
 1130:                 TAX
 1131: 
 1132: loc_C708:                               ; CODE XREF: sub_C6EA+27↓j
 1133:                 LDA     ($16),Y
 1134:                 STA     $460,X
 1135:                 INY
 1136:                 INX
 1137:                 DEC     byte_11
 1138:                 BNE     loc_C708
 1139:                 JSR     sub_C807
 1140:                 LDX     byte_10
 1141:                 DEY
 1142:                 TYA
 1143:                 ADC     byte_16
 1144:                 STA     byte_16
 1145:                 LDA     byte_17
 1146:                 ADC     #0
 1147:                 STA     byte_17
 1148: 
 1149: locret_C724:                            ; CODE XREF: sub_C6EA+14↑j
 1150:                 RTS
 1151: ; End of function sub_C6EA
 1152: 
 1153: 
 1154: ; =============== S U B R O U T I N E =======================================
 1155: 
 1156: 
 1157: sub_C725:
 1158:                 PLA
 1159:                 STA     byte_16
 1160:                 PLA
 1161:                 STA     byte_17
 1162:                 LDY     #1
 1163:                 LDA     ($16),Y
 1164:                 CLC
 1165:                 ADC     #3
 1166:                 JSR     sub_C7EB
 1167:                 JSR     sub_C7F8
 1168:                 STX     byte_11
 1169:                 TAX
 1170:                 LDA     ($16),Y
 1171:                 STA     byte_10
 1172:                 STA     $460,X
 1173:                 LDA     byte_19
 1174:                 STA     $461,X
 1175:                 LDA     byte_18
 1176:                 STA     $462,X
 1177:                 TYA
 1178:                 SEC
 1179:                 ADC     byte_16
 1180:                 STA     byte_16
 1181:                 BCC     loc_C756
 1182:                 INC     byte_17
 1183: 
 1184: loc_C756:                               ; CODE XREF: sub_C725+2D↑j
 1185:                 LDY     #0
 1186:                 LDA     ($16),Y
 1187: 
 1188: loc_C75A:                               ; CODE XREF: sub_C725+3B↓j
 1189:                 STA     $463,X
 1190:                 INX
 1191:                 DEC     byte_10
 1192:                 BNE     loc_C75A
 1193:                 JSR     sub_C807
 1194:                 JMP     stack_call_return ; Return to the parent procedure that used parameter passing through the code stream
 1195: ; End of function sub_C725
```

### 3.3 `JMP stack_call_return` / `BCC stack_call_return` — 2 sites

**line 1100** (`$C6E8`, `BCC`)

```asm
 1093:                 PLA
 1094:                 STA     byte_17
 1095: 
 1096: loc_C6E0:                               ; CODE XREF: print_string+C↓j
 1097:                 JSR     sub_C8BA
 1098:                 JSR     sub_C6EA
 1099:                 BCS     loc_C6E0
 1100:                 BCC     stack_call_return ; Return to the parent procedure that used parameter passing through the code stream
 1101: ; End of function print_string
 1102: 
 1103: 
```

**line 1194** (`$C765`, `JMP`)

```asm
 1187: 
 1188: loc_C75A:                               ; CODE XREF: sub_C725+3B↓j
 1189:                 STA     $463,X
 1190:                 INX
 1191:                 DEC     byte_10
 1192:                 BNE     loc_C75A
 1193:                 JSR     sub_C807
 1194:                 JMP     stack_call_return ; Return to the parent procedure that used parameter passing through the code stream
 1195: ; End of function sub_C725
 1196: 
 1197: 
```

### 3.4 `JSR switch_bank` / `JMP switch_bank` (direct) — 7 sites

**line 26** (`$C003`, `JMP`) — A = bank number

```asm
   20: 
   21: ; =============== S U B R O U T I N E =======================================
   22: 
   23: ; Attributes: thunk
   24: 
   25: j_switch_bank:
   26:                 JMP     switch_bank     ; A: bank number
   27: ; End of function j_switch_bank
   28: 
   29: 
```

**line 1610** (`$C904`, `JSR`) — A = bank number

```asm
 1604:                 BEQ     loc_C901
 1605:                 JSR     sub_C8BA
 1606: 
 1607: loc_C901:                               ; CODE XREF: sub_C8DF+1D↑j
 1608:                 PLA
 1609:                 STA     byte_26
 1610:                 JSR     switch_bank     ; A: bank number
 1611:                 PLA
 1612:                 STA     byte_15
 1613:                 PLA
```

**line 1628** (`$C925`, `JMP`) — A = bank number

```asm
 1622:                 STA     byte_2E
 1623:                 LDA     byte_19
 1624:                 STA     byte_2F
 1625:                 LDA     #2
 1626:                 STA     byte_2B
 1627:                 PLA
 1628:                 JMP     switch_bank     ; A: bank number
 1629: ; End of function sub_C8DF
 1630: 
 1631: ; ---------------------------------------------------------------------------
```

**line 1781** (`$CA07`, `JSR`) — A = bank number

```asm
 1775:                 AND     $16E,Y
 1776:                 ORA     byte_11
 1777:                 .BYTE $9D, $44, $00  ; STA !$44,X  -- Force non Zero Page [!]
 1778:                 LDX     byte_21
 1779:                 STA     $467,X
 1780:                 PLA
 1781:                 JSR     switch_bank     ; A: bank number
 1782:                 PLA
 1783:                 TAX
 1784:                 JMP     sub_C807
```

**line 2441** (`$CD70`, `JSR`) — A = bank number

```asm
 2435: 
 2436: sub_CD6B:                               ; CODE XREF: sub_C033↑j
 2437:                                         ; sub_C8BA:loc_C8BC↑p
 2438:                 LDA     last_bank_num
 2439:                 PHA
 2440:                 LDA     byte_26
 2441:                 JSR     switch_bank     ; A: bank number
 2442:                 LDA     byte_2B
 2443:                 LSR
 2444:                 BCS     loc_CD7E
```

**line 2622** (`$CE6F`, `JMP`) — A = bank number

```asm
 2616:                 BCC     loc_CE6E
 2617:                 INC     byte_2D
 2618: 
 2619: loc_CE6E:                               ; CODE XREF: sub_CD6B:loc_CD8F↑j
 2620:                                         ; sub_CD6B+FF↑j
 2621:                 PLA
 2622:                 JMP     switch_bank     ; A: bank number
 2623: ; End of function sub_CD6B
 2624: 
 2625: 
```

**line 4340** (`$D748`, `JMP`) — A = bank number

```asm
 4334:                 STA     byte_A3
 4335:                 LDA     #0
 4336:                 STA     byte_AC
 4337:                 LDA     #2
 4338:                 STA     byte_AD
 4339:                 PLA
 4340:                 JMP     switch_bank     ; A: bank number
 4341: ; End of function sub_D6DF
 4342: 
 4343: ; ---------------------------------------------------------------------------
```

Also every `switch_bank_N` routine body (lines 1397–1491) writes `bank_remap_table+N`.

### 3.5 `JMP sub_C8DF` / `JSR sub_C8DF` — 9 sites

**line 221** (`$C042`, `JMP`) — A = table index, Y = bank number

```asm
  213: ; End of function sub_C03F
  214: 
  215: 
  216: ; =============== S U B R O U T I N E =======================================
  217: 
  218: ; Attributes: thunk
  219: 
  220: sub_C042:
  221:                 JMP     sub_C8DF
  222: ; End of function sub_C042
  223: 
  224: 
  225: ; =============== S U B R O U T I N E =======================================
```

**line 2089** (`$CB55`, `JSR`) — A = table index, Y = bank number

```asm
 2081: 
 2082: 
 2083: start_game_check:                       ; CODE XREF: main_menu_loop+15↓p
 2084:                 LDA     #0
 2085:                 STA     byte_19
 2086:                 STA     byte_18
 2087:                 LDA     #0
 2088:                 LDY     #4
 2089:                 JSR     sub_C8DF
 2090:                 LDA     pad_buttons_1
 2091:                 ORA     pad_buttons_2
 2092:                 AND     #$C0
 2093:                 CMP     #$C0
```

**line 2105** (`$CB73`, `JSR`) — A = table index, Y = bank number

```asm
 2097: 
 2098: loc_CB67:                               ; CODE XREF: start_game_check+15↑j
 2099:                 LDA     #1
 2100:                 STA     byte_19
 2101:                 LDA     #$40 ; '@'
 2102:                 STA     byte_18
 2103:                 LDA     #1
 2104:                 LDY     #4
 2105:                 JSR     sub_C8DF
 2106: 
 2107: loc_CB76:                               ; CODE XREF: start_game_check+1A↑j
 2108:                 LDA     #$17
 2109:                 STA     byte_19
```

**line 2114** (`$CB82`, `JSR`) — A = table index, Y = bank number

```asm
 2106: 
 2107: loc_CB76:                               ; CODE XREF: start_game_check+1A↑j
 2108:                 LDA     #$17
 2109:                 STA     byte_19
 2110:                 LDA     #$F0
 2111:                 STA     byte_18
 2112:                 LDA     #3
 2113:                 LDY     #4
 2114:                 JSR     sub_C8DF
 2115:                 JSR     sub_C8BA
 2116:                 LDA     #3
 2117:                 STA     byte_32
 2118:                 LDA     #0
```

**line 2265** (`$CC71`, `JSR`) — A = table index, Y = bank number

```asm
 2257:                 JSR     mem_page_fill   ; Fill memory $X.00 ... $Y.FF by value A
 2258:                 LDA     #$10
 2259:                 STA     byte_19
 2260:                 LDA     #0
 2261:                 STA     byte_18
 2262:                 LDY     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
 2263:                 LDA     unk_CCAD,Y
 2264:                 LDY     #4
 2265:                 JSR     sub_C8DF
 2266:                 JSR     sub_C8BA
 2267:                 JSR     switch_bank_5
 2268:                 LDA     current_level   ; Current game level (1 = first level). There are a total of 12 regular levels and 1 secret level in the game.
 2269:                 ASL
```

**line 6051** (`$DFE3`, `JSR`) — A = table index, Y = bank number

```asm
 6043:                 STA     byte_19
 6044:                 JSR     ppu_cram_load   ; Update Palette (CRAM). The Ptr variable ($18) contains the address of the palette. The palette is copied to $145. The “update palette” flag is set (checked in nmi_handler)
 6045:                 JSR     clear_oam_buffer
 6046:                 LDA     #0
 6047:                 STA     byte_19
 6048:                 STA     byte_18
 6049:                 LDA     #0
 6050:                 LDY     #4
 6051:                 JSR     sub_C8DF
 6052:                 LDA     #$19
 6053:                 STA     byte_19
 6054:                 LDA     #$F0
 6055:                 STA     byte_18
```

**line 6058** (`$DFF2`, `JSR`) — A = table index, Y = bank number

```asm
 6050:                 LDY     #4
 6051:                 JSR     sub_C8DF
 6052:                 LDA     #$19
 6053:                 STA     byte_19
 6054:                 LDA     #$F0
 6055:                 STA     byte_18
 6056:                 LDA     #3
 6057:                 LDY     #4
 6058:                 JSR     sub_C8DF
 6059:                 LDA     #$12
 6060:                 STA     byte_19
 6061:                 LDA     #0
 6062:                 STA     byte_18
```

**line 6065** (`$E001`, `JSR`) — A = table index, Y = bank number

```asm
 6057:                 LDY     #4
 6058:                 JSR     sub_C8DF
 6059:                 LDA     #$12
 6060:                 STA     byte_19
 6061:                 LDA     #0
 6062:                 STA     byte_18
 6063:                 LDA     #2
 6064:                 LDY     #4
 6065:                 JSR     sub_C8DF
 6066:                 JSR     sub_C8BA
 6067:                 LDA     #$20 ; ' '
 6068:                 LDX     #$20 ; ' '
 6069:                 LDY     #$AA
```

**line 6809** (`$E50C`, `JSR`) — A = table index, Y = bank number

```asm
 6801:                 JSR     sub_E60F
 6802:                 JSR     ppu_disable_all
 6803:                 LDA     #$12
 6804:                 STA     byte_19
 6805:                 LDA     #0
 6806:                 STA     byte_18
 6807:                 LDA     #2
 6808:                 LDY     #4
 6809:                 JSR     sub_C8DF
 6810:                 JSR     sub_C8BA
 6811:                 LDA     #$20 ; ' '
 6812:                 LDX     #$20 ; ' '
 6813:                 LDY     #$AA
```

### 3.6 `JSR switch_case` / `JMP switch_case` — 3 sites

**line 379** (`$C075`, `JMP`) — A = case index; the jump table follows immediately

```asm
  373: ; =============== S U B R O U T I N E =======================================
  374: 
  375: ; Attributes: thunk
  376: 
  377: j_switch_case:                          ; CODE XREF: enemy_type_3+4B↓P
  378:                                         ; special_weapon_process+2↓P ...
  379:                 JMP     switch_case     ; Old-school Switch-Case. Right after the call there is a jump table of variable size.
  380: ; End of function j_switch_case
  381: 
  382: 
  383: ; =============== S U B R O U T I N E =======================================
  384: 
  385: ; Attributes: thunk
  386: 
  387: j_get_random_enemy:                     ; CODE XREF: enemy_type_64_random+15↓P
  388:                 JMP     get_random_enemy ; Y: a random index in the table. Called from type 64
  389: ; End of function j_get_random_enemy
  390: 
  391: 
  392: ; =============== S U B R O U T I N E =======================================
  393: 
```

**line 3897** (`$D503`, `JSR`) — A = case index; the jump table follows immediately

```asm
 3891: sub_D4FA:                               ; CODE XREF: sub_D5AB+4↓j
 3892:                 LDA     ($6C),Y
 3893:                 INY
 3894:                 STY     byte_15
 3895:                 CMP     #5
 3896:                 BCS     sub_D56B
 3897:                 JSR     switch_case     ; Old-school Switch-Case. Right after the call there is a jump table of variable size.
 3898: ; End of function sub_D4FA
 3899: 
 3900: ; ---------------------------------------------------------------------------
 3901:                 .WORD $D50F             ; sub_D510 - 1
 3902:                 .WORD $D519             ; sub_D51A - 1
 3903:                 .WORD $D527             ; sub_D528 - 1
 3904:                 .WORD $D54C             ; sub_D54D - 1
 3905:                 .WORD $D558             ; sub_D559 - 1
 3906: 
 3907: ; =============== S U B R O U T I N E =======================================
 3908: 
 3909: 
 3910: sub_D510:
 3911:                 LDY     byte_15
```

**line 4589** (`$D871`, `JSR`) — A = case index; the jump table follows immediately

```asm
 4583: ; ---------------------------------------------------------------------------
 4584: 
 4585: loc_D86C:                               ; CODE XREF: sub_D84F+A↑j
 4586:                 LDA     ($B5),Y
 4587:                 INY
 4588:                 STY     byte_16
 4589:                 JSR     switch_case     ; Old-school Switch-Case. Right after the call there is a jump table of variable size.
 4590: ; End of function sub_D84F
 4591: 
 4592: ; ---------------------------------------------------------------------------
 4593:                 .WORD $D885             ; sub_D886 - 1
 4594:                 .WORD $D893             ; sub_D894 - 1
 4595:                 .WORD $D8A0             ; sub_D8A1 - 1
 4596:                 .WORD $D8BE             ; sub_D8BF - 1
 4597:                 .WORD $D8DB             ; sub_D8DC - 1
 4598:                 .WORD $D91F             ; sub_D920 - 1
 4599:                 .WORD $D937             ; sub_D938 - 1
 4600:                 .WORD $D9DF             ; sub_D9E0 - 1
 4601:                 .WORD $DA15             ; sub_DA16 - 1
 4602: 
 4603: ; =============== S U B R O U T I N E =======================================
```

### 3.7 `JMP far_call` — 1 site (`j_far_call`, declared unused)

**line 35** (`$C006`, `JMP`)

```asm
   31: 
   32: ; Attributes: thunk
   33: 
   34: j_far_call:
   35:                 JMP     far_call        ; Not used. Variables 10,11,12,13 are used to store the context (registers A, Y, PC are saved). Current bank saved on stack.
   36: ; End of function j_far_call
   37: 
   38: 
```

### 3.8 `JSR` into the `$8000`–`$BFFF` bank window (bank 6 API) — 20 sites

These call into whichever 16 KiB bank is currently selected at `$8000`. Every caller chain reaches them with
bank 6 mapped (`main_menu_loop`/`sub_E81E`/`sub_DE14`/`sub_DE91`/`sub_DEF3` all `JSR switch_bank_6` on entry, and the
`sub_D0BF` battle loop is entered from `sub_E81E`). Equate values are in `src/bank6_api.asm`.

| line | addr | target | abs |
|---|---|---|---|
| 2147 | `$CBCA` | `sub_18012` | `$8012` |
| 2148 | `$CBCD` | `j_set_default_weapon` | `$8015` |
| 2181 | `$CC0E` | `sub_1800F` | `$800F` |
| 3019 | `$D0F1` | `j_game_logic_processing` | `$8000` |
| 3687 | `$D3F7` | `j_update_score_onscreen` | `$801B` |
| 3770 | `$D463` | `j_check_enemy_overflow` | `$8021` |
| 3786 | `$D472` | `j_check_enemy_overflow` | `$8021` |
| 3849 | `$D4C5` | `j_check_enemy_overflow` | `$8021` |
| 6112 | `$E069` | `sub_18006` | `$8006` |
| 6128 | `$E088` | `sub_18009` | `$8009` |
| 6129 | `$E08B` | `sub_18003` | `$8003` |
| 6130 | `$E08E` | `sub_1800C` | `$800C` |
| 7230 | `$E827` | `sub_18006` | `$8006` |
| 7231 | `$E82A` | `j_clear_main_menu_sprites` | `$8018` |
| 7232 | `$E82D` | `j_update_score_onscreen` | `$801B` |
| 7253 | `$E851` | `sub_18009` | `$8009` |
| 7254 | `$E854` | `sub_18003` | `$8003` |
| 7255 | `$E857` | `sub_1800C` | `$800C` |
| 7282 | `$E883` | `sub_1801E` | `$801E` |
| 7608 | `$EA25` | `j_game_logic_processing` | `$8000` |

### 3.9 Other computed control flow (summary)

| construct | line(s) | addr | note |
|---|---|---|---|
| `switch_case` inline-table RTS dispatch | 3897, 4589 | `$D503`, `$D871` | tables at `$D506`, `$D874` |
| `sub_EB7A` `loc_EB98` PHA×4 + `RTS` | 7908–7922 | `$EB98`–`$EBA6` | dispatches through `word_EBAF` (30 entries) |
| `sub_EFA6` PHA/PHA + `RTS` | 8812–8816 | `$EFB2`–`$EFB6` | jumps to the pointer in `$1E/$1F` |
| `sub_EB7A` direct `JMP apu_fx_26_27` | 7905 | `$EB95` | jump into mid-handler |
| `apu_fx_21` `JMP apu_fx_0` | 8192 | `$ECAC` | tail merge |
| `JSR switch_case` inside own body | 3897, 4589 | | see above |
| `LDA ($B5),Y`-driven scripting in `sub_D84F`/`sub_DA36` | 4586, 4933–4936 … | | script interpreter, 35 `($B5),Y` dereferences |
---

## 4. Data tables

### 4.1 The `$C0F8`–`$C6AA` data block (the only contiguous data region in the bank)

| label | addr | size | content / format | indexed by |
|---|---|---|---|---|
| `bank_remap_table` | `$C0F8`–`$C0FF` | 8 | `0,1,2,3,4,5,6,7` | see §5 — the *address* is the mapper register, the *value* is the bank number. Read at 1386/1401/1415/1429/1443/1457/1472/1487 and 1543. |
| `randomize_tab` | `$C100`–`$C151` | 82 | PRNG table of small even numbers | `randomize` line 1915 `EOR randomize_tab,X`, `X` = `last_randomize_result` (`$7E`) |
| `unk_C152` | `$C152`–`$C1A5` | 84 | byte table, IDA xref `sub_181A7` (bank 6). No reference inside bank7. | external (`sub_181A7`, bank 6); `unk_C1A5` is its last byte (`bank7_api.asm`). |
| `tab_C1A6` | `$C1A6`–`$C1F8` | 83 | **low bytes** of 83 pointers | paired with `tab_C1F9` |
| `tab_C1F9` | `$C1F9`–`$C24B` | 83 | **high bytes** of the same 83 pointers | read as `(tab_C1A6[i], tab_C1F9[i])` by `sub_182D9` (bank 6). Example: `$4C`,`$C2` → `$C24C`. |
| `tab_C24C` … `tab_C69A` | `$C24C`–`$C6AA` | 1119 total (83 tables) | sprite meta records: `count` byte followed by `count` × 4 bytes | `sub_182D9` (bank 6) via the two pointer tables above |

The 83 pointer entries in `tab_C1A6`/`tab_C1F9` and the 83 sprite tables line up exactly:
`tab_C1A6[i] | tab_C1F9[i]<<8` reproduces the address of the i-th `tab_C24C…` label (verified for all 83 pairs).

Sprite record format (verified for all 83 tables: `size == 1 + 4*count`):

```
+0  count
+1  sprite 0: b0 b1 b2 b3
+5  sprite 1: b0 b1 b2 b3
...
```

| # | label | addr | size | count | next |
|---:|---|---|---|---:|---|
| 0 | `tab_C24C` | `$C24C` | 5 | 1 | `$C251` |
| 1 | `tab_C251` | `$C251` | 9 | 2 | `$C25A` |
| 2 | `tab_C25A` | `$C25A` | 9 | 2 | `$C263` |
| 3 | `tab_C263` | `$C263` | 13 | 3 | `$C270` |
| 4 | `tab_C270` | `$C270` | 5 | 1 | `$C275` |
| 5 | `tab_C275` | `$C275` | 9 | 2 | `$C27E` |
| 6 | `tab_C27E` | `$C27E` | 9 | 2 | `$C287` |
| 7 | `tab_C287` | `$C287` | 9 | 2 | `$C290` |
| 8 | `tab_C290` | `$C290` | 9 | 2 | `$C299` |
| 9 | `tab_C299` | `$C299` | 9 | 2 | `$C2A2` |
| 10 | `tab_C2A2` | `$C2A2` | 9 | 2 | `$C2AB` |
| 11 | `tab_C2AB` | `$C2AB` | 9 | 2 | `$C2B4` |
| 12 | `tab_C2B4` | `$C2B4` | 5 | 1 | `$C2B9` |
| 13 | `tab_C2B9` | `$C2B9` | 5 | 1 | `$C2BE` |
| 14 | `tab_C2BE` | `$C2BE` | 9 | 2 | `$C2C7` |
| 15 | `tab_C2C7` | `$C2C7` | 13 | 3 | `$C2D4` |
| 16 | `tab_C2D4` | `$C2D4` | 9 | 2 | `$C2DD` |
| 17 | `tab_C2DD` | `$C2DD` | 9 | 2 | `$C2E6` |
| 18 | `tab_C2E6` | `$C2E6` | 5 | 1 | `$C2EB` |
| 19 | `tab_C2EB` | `$C2EB` | 9 | 2 | `$C2F4` |
| 20 | `tab_C2F4` | `$C2F4` | 9 | 2 | `$C2FD` |
| 21 | `tab_C2FD` | `$C2FD` | 9 | 2 | `$C306` |
| 22 | `tab_C306` | `$C306` | 9 | 2 | `$C30F` |
| 23 | `tab_C30F` | `$C30F` | 9 | 2 | `$C318` |
| 24 | `tab_C318` | `$C318` | 9 | 2 | `$C321` |
| 25 | `tab_C321` | `$C321` | 9 | 2 | `$C32A` |
| 26 | `tab_C32A` | `$C32A` | 9 | 2 | `$C333` |
| 27 | `tab_C333` | `$C333` | 9 | 2 | `$C33C` |
| 28 | `tab_C33C` | `$C33C` | 9 | 2 | `$C345` |
| 29 | `tab_C345` | `$C345` | 9 | 2 | `$C34E` |
| 30 | `tab_C34E` | `$C34E` | 9 | 2 | `$C357` |
| 31 | `tab_C357` | `$C357` | 9 | 2 | `$C360` |
| 32 | `tab_C360` | `$C360` | 9 | 2 | `$C369` |
| 33 | `tab_C369` | `$C369` | 9 | 2 | `$C372` |
| 34 | `tab_C372` | `$C372` | 5 | 1 | `$C377` |
| 35 | `tab_C377` | `$C377` | 9 | 2 | `$C380` |
| 36 | `tab_C380` | `$C380` | 9 | 2 | `$C389` |
| 37 | `tab_C389` | `$C389` | 9 | 2 | `$C392` |
| 38 | `tab_C392` | `$C392` | 9 | 2 | `$C39B` |
| 39 | `tab_C39B` | `$C39B` | 13 | 3 | `$C3A8` |
| 40 | `tab_C3A8` | `$C3A8` | 13 | 3 | `$C3B5` |
| 41 | `tab_C3B5` | `$C3B5` | 13 | 3 | `$C3C2` |
| 42 | `tab_C3C2` | `$C3C2` | 9 | 2 | `$C3CB` |
| 43 | `tab_C3CB` | `$C3CB` | 9 | 2 | `$C3D4` |
| 44 | `tab_C3D4` | `$C3D4` | 9 | 2 | `$C3DD` |
| 45 | `tab_C3DD` | `$C3DD` | 9 | 2 | `$C3E6` |
| 46 | `tab_C3E6` | `$C3E6` | 9 | 2 | `$C3EF` |
| 47 | `tab_C3EF` | `$C3EF` | 9 | 2 | `$C3F8` |
| 48 | `tab_C3F8` | `$C3F8` | 9 | 2 | `$C401` |
| 49 | `tab_C401` | `$C401` | 9 | 2 | `$C40A` |
| 50 | `tab_C40A` | `$C40A` | 9 | 2 | `$C413` |
| 51 | `tab_C413` | `$C413` | 9 | 2 | `$C41C` |
| 52 | `tab_C41C` | `$C41C` | 9 | 2 | `$C425` |
| 53 | `tab_C425` | `$C425` | 9 | 2 | `$C42E` |
| 54 | `tab_C42E` | `$C42E` | 9 | 2 | `$C437` |
| 55 | `tab_C437` | `$C437` | 17 | 4 | `$C448` |
| 56 | `tab_C448` | `$C448` | 17 | 4 | `$C459` |
| 57 | `tab_C459` | `$C459` | 17 | 4 | `$C46A` |
| 58 | `tab_C46A` | `$C46A` | 17 | 4 | `$C47B` |
| 59 | `tab_C47B` | `$C47B` | 9 | 2 | `$C484` |
| 60 | `tab_C484` | `$C484` | 9 | 2 | `$C48D` |
| 61 | `tab_C48D` | `$C48D` | 25 | 6 | `$C4A6` |
| 62 | `tab_C4A6` | `$C4A6` | 33 | 8 | `$C4C7` |
| 63 | `tab_C4C7` | `$C4C7` | 33 | 8 | `$C4E8` |
| 64 | `tab_C4E8` | `$C4E8` | 17 | 4 | `$C4F9` |
| 65 | `tab_C4F9` | `$C4F9` | 17 | 4 | `$C50A` |
| 66 | `tab_C50A` | `$C50A` | 9 | 2 | `$C513` |
| 67 | `tab_C513` | `$C513` | 41 | $A | `$C53C` |
| 68 | `tab_C53C` | `$C53C` | 33 | 8 | `$C55D` |
| 69 | `tab_C55D` | `$C55D` | 33 | 8 | `$C57E` |
| 70 | `tab_C57E` | `$C57E` | 5 | 1 | `$C583` |
| 71 | `tab_C583` | `$C583` | 33 | 8 | `$C5A4` |
| 72 | `tab_C5A4` | `$C5A4` | 37 | 9 | `$C5C9` |
| 73 | `tab_C5C9` | `$C5C9` | 65 | $10 | `$C60A` |
| 74 | `tab_C60A` | `$C60A` | 9 | 2 | `$C613` |
| 75 | `tab_C613` | `$C613` | 9 | 2 | `$C61C` |
| 76 | `tab_C61C` | `$C61C` | 9 | 2 | `$C625` |
| 77 | `tab_C625` | `$C625` | 9 | 2 | `$C62E` |
| 78 | `tab_C62E` | `$C62E` | 9 | 2 | `$C637` |
| 79 | `tab_C637` | `$C637` | 33 | 8 | `$C658` |
| 80 | `tab_C658` | `$C658` | 33 | 8 | `$C679` |
| 81 | `tab_C679` | `$C679` | 33 | 8 | `$C69A` |
| 82 | `tab_C69A` | `$C69A` | 17 | 4 | `$C6AB` |

### 4.2 Every other data label (label, address, size, index register / reader)

| label | addr range | size | reader (line @ routine, index) |
|---|---|---:|---|
| `unk_C928` | `$C928`–`$C92F` | 8 | L1596 ADC `unk_C928,Y` (sub_C8DF) |
| `unk_C930` | `$C930`–`$C937` | 8 | L1598 LDA `unk_C930,Y` (sub_C8DF) |
| `unk_CC16` | `$CC16`–`$CC25` | 16 | L2173 LDA `unk_CC16,Y` (start_game_check) |
| `unk_CC26` | `$CC26`–`$CC35` | 16 | L2175 LDA `unk_CC26,Y` (start_game_check) |
| `unk_CCAD` | `$CCAD`–`$CCB9` | 13 | L2263 LDA `unk_CCAD,Y` (sub_CC57) |
| `unk_CCBA` | `$CCBA`–`$CCC6` | 13 | L2276 LDX `unk_CCBA,Y` (sub_CC57) |
| `unk_CCC7` | `$CCC7`–`$CCD3` | 13 | L2281 LDX `unk_CCC7,Y` (sub_CC57) |
| `pmove_direction_tab` | `$CD3B`–`$CD4A` | 16 | L2420 LDA `pmove_direction_tab,Y` (handle_pad_buttons) |
| `demo_buttons_tab` | `$CD4B`–`$CD6A` | 32 | L2405 LDA `demo_buttons_tab,Y` (handle_pad_buttons) |
| `unk_D1E1` | `$D1E1`–`$D1FF` | 31 | L3139 LDA `unk_D1E1,Y` (sub_D18C) |
| `unk_D200` | `$D200`–`$D23A` | 59 | *not referenced inside bank7* |
| `unk_D23B` | `$D23B`–`$D23B` | 1 | L3175 LDA `unk_D23B,Y` (sub_D1A6) |
| `unk_D23C` | `$D23C`–`$D2BF` | 132 | L3435 LDA `unk_D23C,X` (sub_D2C0); L3444 LDA `unk_D23C,Y` (sub_D2C4) |
| `unk_D61F` | `$D61F`–`$D63A` | 28 | L4111 LDA `unk_D61F,Y` (sub_D5D7) |
| `unk_D63B` | `$D63B`–`$D64A` | 16 | L4132 LDA `unk_D63B,Y` (sub_D5D7) |
| `unk_D64B` | `$D64B`–`$D65A` | 16 | L4141 LDA `unk_D64B,Y` (sub_D5D7) |
| `random_enemy_tab` | `$D65B`–`$D69A` | 64 | L3852 LDA `random_enemy_tab,X` (sub_D480); L4220 LDA `random_enemy_tab,Y` (get_random_enemy) |
| `tab_D74B` | `$D74B`–`$D752` | 8 | L4306 `LDA $D74A,X` in `sub_D6DF` — **off-by-one base**: `X` counts 8→1, so the 8 bytes read are `$D74B`…`$D752`. **Inference:** this is a deliberate one-byte-early base; the label `tab_D74B` is one byte *after* the effective base. |
| `nt_address_tab` | `$D7BD`–`$D7DA` | 30 | L4436 LDA `nt_address_tab,Y` (set_nametable_addr_after_scroll); L4438 LDA `nt_address_tab+1,Y` (set_nametable_addr_after_scroll) |
| `byte_D7DB` | `$D7DB`–`$D7DE` | 4 | L1771 AND `byte_D7DB,Y` (sub_C985); L1846 AND `byte_D7DB,Y` (sub_CA12); L1863 LDA `byte_D7DB,Y` (sub_CA12); L5143 AND `byte_D7DB,Y` (sub_DA36) |
| `byte_D7DF` | `$D7DF`–`$D7E2` | 4 | L1773 LDA `byte_D7DF,Y` (sub_C985); L1848 LDA `byte_D7DF,Y` (sub_CA12); L1865 LDA `byte_D7DF,Y` (sub_CA12); L5145 LDA `byte_D7DF,Y` (sub_DA36) |
| `unk_DBC5` | `$DBC5`–`$DBD7` | 19 | L5163 LDA `unk_DBC5,X` (sub_DA36); L5171 LDA `unk_DBC5,X` (sub_DA36); L5756 LDA `unk_DBC5,X` (sub_DE14); L5765 LDA `unk_DBC5,X` (sub_DE14) … (+2 more) |
| `unk_DBD8` | `$DBD8`–`$DCC4` | 237 | *not referenced inside bank7* |
| `unk_DF9F` | `$DF9F`–`$DFA5` | 7 | L5979 LDA `unk_DF9F,X` (reset_score) |
| `pal_DFA6` | `$DFA6`–`$DFBE` | 25 | *not referenced inside bank7* |
| `aGameStart` | `$E15D`–`$E16C` | 16 | *not referenced inside bank7* |
| `aContinue` | `$E16D`–`$E183` | 23 | *not referenced inside bank7* |
| `unk_E202` | `$E202`–`$E20E` | 13 | L6194 LDA `unk_E202,Y` (show_main_menu) |
| `unk_E20F` | `$E20F`–`$E21B` | 13 | L6199 LDA `unk_E20F,Y` (show_main_menu) |
| `unk_E21C` | `$E21C`–`$E228` | 13 | L6187 LDA `unk_E21C,Y` (show_main_menu) |
| `unk_E254` | `$E254`–`$E25C` | 9 | L6399 LDX `unk_E254,Y` (sub_E229) |
| `unk_E25D` | `$E25D`–`$E265` | 9 | L6401 AND `unk_E25D,Y` (sub_E229) |
| `aScoreTop` | `$E295`–`$E2A8` | 20 | *not referenced inside bank7* |
| `aArea` | `$E2A9`–`$E2F8` | 80 | *not referenced inside bank7* |
| `aAI` | `$E2F9`–`$E301` | 9 | *not referenced inside bank7* |
| `aGameDesindedBy` | `$E302`–`$E31C` | 27 | *not referenced inside bank7* |
| `aProducedByAii` | `$E31D`–`$E333` | 23 | *not referenced inside bank7* |
| `aPresentedByPon` | `$E334`–`$E350` | 29 | *not referenced inside bank7* |
| `aCopyright1986P` | `$E351`–`$E370` | 32 | *not referenced inside bank7* |
| `aLicensedBy` | `$E371`–`$E37E` | 14 | *not referenced inside bank7* |
| `aNintendoOfAmer` | `$E37F`–`$E398` | 26 | *not referenced inside bank7* |
| `byte_E3B8` | `$E3B8`–`$E3D0` | 25 | L6599 LDA `byte_E3B8,Y` (sub_E399); L6603 LDA `byte_E3B8,Y` (sub_E399); L6608 LDA `byte_E3B8,Y` (sub_E399) |
| `tab_E3D1` | `$E3D1`–`$E3F3` | 35 | *not referenced inside bank7* |
| `aSoundTest` | `$E44A`–`$E461` | 24 | *not referenced inside bank7* |
| `aScore` | `$E521`–`$E555` | 53 | *not referenced inside bank7* |
| `congratulation_text` | `$E671`–`$E671` | 1 | L6845 LDA `congratulation_text,X` (show_ending_screen) |
| `aCongratulation` | `$E672`–`$E684` | 19 | *not referenced inside bank7* |
| `aYouAreTheMessi` | `$E685`–`$E6A4` | 32 | *not referenced inside bank7* |
| `aStaff` | `$E6A5`–`$E6AC` | 8 | *not referenced inside bank7* |
| `aGameDesignJemi` | `$E6AD`–`$E6CA` | 30 | *not referenced inside bank7* |
| `aProgram` | `$E6CB`–`$E6D2` | 8 | *not referenced inside bank7* |
| `aJemini` | `$E6D3`–`$E6DA` | 8 | *not referenced inside bank7* |
| `aMusicMiyamoto` | `$E6DB`–`$E6EA` | 16 | *not referenced inside bank7* |
| `aGraphics` | `$E6EB`–`$E6F3` | 9 | *not referenced inside bank7* |
| `aJanus` | `$E6F4`–`$E6FA` | 7 | *not referenced inside bank7* |
| `aTestPlay` | `$E6FB`–`$E704` | 10 | *not referenced inside bank7* |
| `aMncNui` | `$E705`–`$E70D` | 9 | *not referenced inside bank7* |
| `aWaoIsee` | `$E70E`–`$E717` | 10 | *not referenced inside bank7* |
| `aYoriki` | `$E718`–`$E720` | 9 | *not referenced inside bank7* |
| `aProdusedByAii` | `$E721`–`$E732` | 18 | *not referenced inside bank7* |
| `aCreatedByCompi` | `$E733`–`$E749` | 23 | *not referenced inside bank7* |
| `aSeeYouAgainInG` | `$E74A`–`$E766` | 29 | *not referenced inside bank7* |
| `byte_E907` | `$E907`–`$E916` | 16 | L7352 LDA `byte_E907,Y` (sub_E8C9) |
| `unk_E9D0` | `$E9D0`–`$E9DF` | 16 | L7509 LDA `unk_E9D0,X` (fade_palette_before_warp) |
| `level_music_tab` | `$E9FE`–`$EA0A` | 13 | L7265 LDA `level_music_tab,Y` (sub_E81E); L7551 LDA `level_music_tab,Y` (update_music_after_fortress_destroy); L7557 LDA `level_music_tab,Y` (update_music_after_fortress_destroy) |
| `word_EBAF` | `$EBAF`–`$EBEA` | 60 | *not referenced inside bank7* |
| `unk_EF42` | `$EF42`–`$EF45` | 4 | L8645 LDA `unk_EF42,Y` (sub_EEAA); L8693 LDA `unk_EF42,Y` (sub_EEAA) |
| `unk_EF46` | `$EF46`–`$EF49` | 4 | L8648 LDA `unk_EF46,Y` (sub_EEAA) |
| `apu_length_tab` | `$F01C`–`$F0F3` | 216 | *not referenced inside bank7* |
| `apu_sound_tab` | `$F0FE`–`$F145` | 72 | L8805 LDA `apu_sound_tab,X` (sub_EFA6); L8807 LDA `apu_sound_tab+1,X` (sub_EFA6) |
| `byte_F180` | `$F180`–`$F18F` | 16 | L9008 .WORD `byte_F180` (None) |
| `word_F190` | `$F190`–`$F1A1` | 18 | L8498 LDA `word_F190,Y` (sub_EDFC); L8500 LDA `word_F190+1,Y` (sub_EDFC); L8540 LDA `word_F190,Y` (sub_EE43); L8542 LDA `word_F190+1,Y` (sub_EE43) |
| `byte_F1A2` | `$F1A2`–`$F1A9` | 8 | L9096 .WORD `byte_F1A2` (None) |
| `byte_F1AA` | `$F1AA`–`$F1B1` | 8 | L9097 .WORD `byte_F1AA` (None) |
| `byte_F1B2` | `$F1B2`–`$F1B9` | 8 | L9098 .WORD `byte_F1B2` (None) |
| `byte_F1BA` | `$F1BA`–`$F1C6` | 13 | L9099 .WORD `byte_F1BA` (None) |
| `byte_F1C7` | `$F1C7`–`$F1DE` | 24 | L9100 .WORD `byte_F1C7` (None) |
| `byte_F1DF` | `$F1DF`–`$F1EA` | 12 | L9101 .WORD `byte_F1DF` (None) |
| `byte_F1EB` | `$F1EB`–`$F1F4` | 10 | L9102 .WORD `byte_F1EB` (None) |
| `byte_F1F5` | `$F1F5`–`$F208` | 20 | L9103 .WORD `byte_F1F5` (None) |
| `byte_F209` | `$F209`–`$F2FE` | 246 | L9009 .WORD `byte_F209` (None) |
| `byte_F2FF` | `$F2FF`–`$F371` | 115 | L9010 .WORD `byte_F2FF` (None) |
| `byte_F372` | `$F372`–`$F457` | 230 | L9011 .WORD `byte_F372` (None) |
| `byte_F458` | `$F458`–`$F4D7` | 128 | L9012 .WORD `byte_F458` (None) |
| `byte_F4D8` | `$F4D8`–`$F52A` | 83 | L9013 .WORD `byte_F4D8` (None) |
| `byte_F52B` | `$F52B`–`$F543` | 25 | L9014 .WORD `byte_F52B` (None) |
| `byte_F544` | `$F544`–`$F5AA` | 103 | L9015 .WORD `byte_F544` (None) |
| `byte_F5AB` | `$F5AB`–`$F5D7` | 45 | L9016 .WORD `byte_F5AB` (None) |
| `byte_F5D8` | `$F5D8`–`$F60C` | 53 | L9017 .WORD `byte_F5D8` (None) |
| `byte_F60D` | `$F60D`–`$F6EE` | 226 | L9018 .WORD `byte_F60D` (None) |
| `byte_F6EF` | `$F6EF`–`$F727` | 57 | L9019 .WORD `byte_F6EF` (None) |
| `byte_F728` | `$F728`–`$F764` | 61 | L9020 .WORD `byte_F728` (None) |
| `byte_F765` | `$F765`–`$F772` | 14 | L9021 .WORD `byte_F765` (None) |
| `byte_F773` | `$F773`–`$F782` | 16 | L9022 .WORD `byte_F773` (None) |
| `byte_F783` | `$F783`–`$F796` | 20 | L9023 .WORD `byte_F783` (None) |
| `byte_F797` | `$F797`–`$F7A9` | 19 | L9024 .WORD `byte_F797` (None) |
| `byte_F7AA` | `$F7AA`–`$F7BC` | 19 | L9025 .WORD `byte_F7AA` (None) |
| `byte_F7BD` | `$F7BD`–`$F7D7` | 27 | L9026 .WORD `byte_F7BD` (None) |
| `byte_F7D8` | `$F7D8`–`$F7EC` | 21 | L9027 .WORD `byte_F7D8` (None) |
| `byte_F7ED` | `$F7ED`–`$F7FC` | 16 | L9028 .WORD `byte_F7ED` (None) |
| `byte_F7FD` | `$F7FD`–`$F80D` | 17 | L9029 .WORD `byte_F7FD` (None) |
| `byte_F80E` | `$F80E`–`$F820` | 19 | L9030 .WORD `byte_F80E` (None) |
| `byte_F821` | `$F821`–`$F836` | 22 | L9031 .WORD `byte_F821` (None) |
| `byte_F837` | `$F837`–`$F84B` | 21 | L9032 .WORD `byte_F837` (None) |
| `byte_F84C` | `$F84C`–`$F941` | 246 | L9033 .WORD `byte_F84C` (None) |
| `byte_F942` | `$F942`–`$F9D0` | 143 | L9034 .WORD `byte_F942` (None) |
| `byte_F9D1` | `$F9D1`–`$FA2C` | 92 | L9035 .WORD `byte_F9D1` (None) |
| `byte_FA2D` | `$FA2D`–`$FA4F` | 35 | L9036 .WORD `byte_FA2D` (None) |
| `byte_FA50` | `$FA50`–`$FB79` | 298 | L9037 .WORD `byte_FA50` (None) |
| `byte_FB7A` | `$FB7A`–`$FBDA` | 97 | L9038 .WORD `byte_FB7A` (None) |
| `byte_FBDB` | `$FBDB`–`$FC41` | 103 | L9039 .WORD `byte_FBDB` (None) |
| `byte_FC42` | `$FC42`–`$FD74` | 307 | L9040 .WORD `byte_FC42` (None) |
| `byte_FD75` | `$FD75`–`$FE5B` | 231 | L9041 .WORD `byte_FD75` (None) |
| `byte_FE5C` | `$FE5C`–`$FE70` | 21 | L9042 .WORD `byte_FE5C` (None) |
| `byte_FE71` | `$FE71`–`$FEE7` | 119 | L9043 .WORD `byte_FE71` (None) |

### 4.3 16-bit word tables

| label | addr range | bytes | words | entries point at | index register |
|---|---|---:|---:|---|---|
| *(anonymous, `sub_D4FA` case table)* | `$D506`–`$D50F` | 10 | 5 | `$D510`,`$D51A`,`$D528`,`$D54D`,`$D559` (each stored as `target-1`) | `A` (case index), consumed by `switch_case` |
| *(anonymous, `sub_D84F` case table)* | `$D874`–`$D885` | 18 | 9 | `$D886`,`$D894`,`$D8A1`,`$D8BF`,`$D8DC`,`$D920`,`$D938`,`$D9E0`,`$DA16` (`target-1`) | `A` |
| `nt_address_tab` | `$D7BD`–`$D7DA` | 30 | 15 | PPU name-table base addresses `$2000,$2040,…,$2380` | `Y` = `byte_10` (lines 4436/4438) |
| `word_EBAF` | `$EBAF`–`$EBEA` | 60 | 30 | `apu_fx_*` handlers, stored as `handler-1` | `X = A*2` (`sub_EB7A` line 7915/7917) |
| `apu_length_tab` | `$F01C`–`$F0F3` | 216 | 108 | period/length values `$7FE,$78E,…` | **no reference inside bank7** (read from another bank at fixed `$F01C`) |
| `apu_sound_tab` | `$F0FE`–`$F145` | 72 | 36 | pointers to sound scripts (`byte_F180`, `byte_F1A2`, `byte_F1AA`, … `byte_FE71`) | `X = A*2` (`sub_EFA6` lines 8805/8807) |
| `word_F190` | `$F190`–`$F1A1` | 18 | 9 | `$82E8` (**bank window!**), `byte_F1A2`, `byte_F1AA`, `byte_F1B2`, `byte_F1BA`, `byte_F1C7`, `byte_F1DF`, `byte_F1EB`, `byte_F1F5` | `Y = $326,X * 2` (`sub_EDFC` 8498/8500, `sub_EE43` 8540/8542) |
| *(anonymous, the 6502 vectors)* | `$FFFA`–`$FFFF` | 6 | 3 | `nmi_handler`, `reset`, `dummy_irq_handler` | hardware |

### 4.4 Inline data inside code spans

All 54 `.BYTE`/`.WORD` lines that fall inside a `; End of function` span, with the host routine:

| src line | addr | host routine | bytes |
|---|---|---|---|
| 1770 | `$C9EC` | `sub_C985` | `$BD, $44, $00` |
| 1777 | `$C9FE` | `sub_C985` | `$9D, $44, $00` |
| 1845 | `$CA60` | `sub_CA12` | `$BD, $44, $00` |
| 1851 | `$CA6F` | `sub_CA12` | `$9D, $44, $00` |
| 2188 | `$CC16` | `start_game_check` | `7` |
| 2189 | `$CC17` | `start_game_check` | `0` |
| 2190 | `$CC18` | `start_game_check` | `1` |
| 2191 | `$CC19` | `start_game_check` | `2` |
| 2192 | `$CC1A` | `start_game_check` | `3` |
| 2193 | `$CC1B` | `start_game_check` | `4` |
| 2194 | `$CC1C` | `start_game_check` | `5` |
| 2195 | `$CC1D` | `start_game_check` | `6` |
| 2196 | `$CC1E` | `start_game_check` | `7` |
| 2197 | `$CC1F` | `start_game_check` | `0` |
| 2198 | `$CC20` | `start_game_check` | `1` |
| 2199 | `$CC21` | `start_game_check` | `2` |
| 2200 | `$CC22` | `start_game_check` | `3` |
| 2201 | `$CC23` | `start_game_check` | `4` |
| 2202 | `$CC24` | `start_game_check` | `6` |
| 2203 | `$CC25` | `start_game_check` | `5` |
| 2204 | `$CC26` | `start_game_check` | `6` |
| 2205 | `$CC27` | `start_game_check` | `2` |
| 2206 | `$CC28` | `start_game_check` | `1` |
| 2207 | `$CC29` | `start_game_check` | `4` |
| 2208 | `$CC2A` | `start_game_check` | `1` |
| 2209 | `$CC2B` | `start_game_check` | `2` |
| 2210 | `$CC2C` | `start_game_check` | `3` |
| 2211 | `$CC2D` | `start_game_check` | `1` |
| 2212 | `$CC2E` | `start_game_check` | `3` |
| 2213 | `$CC2F` | `start_game_check` | `1` |
| 2214 | `$CC30` | `start_game_check` | `3` |
| 2215 | `$CC31` | `start_game_check` | `2` |
| 2216 | `$CC32` | `start_game_check` | `6` |
| 2217 | `$CC33` | `start_game_check` | `3` |
| 2218 | `$CC34` | `start_game_check` | `0` |
| 2219 | `$CC35` | `start_game_check` | `$C` |
| 6244 | `$E15A` | `show_main_menu` | `$A` |
| 6245 | `$E15B` | `show_main_menu` | `$22` |
| 6246 | `$E15C` | `show_main_menu` | `$2B` |
| 6247 | `$E15D` | `show_main_menu` | `"GAME START"` |
| 6251 | `$E16A` | `show_main_menu` | `8` |
| 6252 | `$E16B` | `show_main_menu` | `$22` |
| 6253 | `$E16C` | `show_main_menu` | `$8B` |
| 6254 | `$E16D` | `show_main_menu` | `"CONTINUE"` |
| 6694 | `$E447` | `sound_test_menu` | `$A` |
| 6695 | `$E448` | `sound_test_menu` | `$20` |
| 6696 | `$E449` | `sound_test_menu` | `$88` |
| 6697 | `$E44A` | `sound_test_menu` | `"SOUND TEST"` |
| 6817 | `$E51E` | `show_ending_screen` | `5` |
| 6818 | `$E51F` | `show_ending_screen` | `$20` |
| 6819 | `$E520` | `show_ending_screen` | `$89` |
| 6820 | `$E521` | `show_ending_screen` | `"SCORE"` |
| 7370 | `$E907` | `sub_E8C9` | `5, 6, 7, 8, 9, $A, $B, $C, $F, 1, 2, 3, 4, $F, $F` |
| 7371 | `$E916` | `sub_E8C9` | `0` |

### 4.5 Unlabelled data between procedures

| addr range | size | what |
|---|---:|---|
| `$C093`–`$C0F7` | 101 | `.BYTE $FF` filler (src lines 467–567) |
| `$D506`–`$D50F` | 10 | `sub_D4FA` case table |
| `$D874`–`$D885` | 18 | `sub_D84F` case table |
| `$E15A`–`$E15C` | 3 | `print_string` header (count, VRAM lo, VRAM hi) |
| `$E16A`–`$E16C` | 3 | `print_string` header |
| `$E447`–`$E449` | 3 | `print_string` header |
| `$E51E`–`$E520` | 3 | `print_string` header |
| `$FF11`–`$FFF9` | 233 | `org $fffa,255` fill (`$FF`) |
---

## 5. Bank switching

### 5.1 Mechanism

The cartridge is mapper **2 (UxROM)**: the 16 KiB bank latched by *any* CPU write to `$8000`–`$FFFF`
appears at `$8000`–`$BFFF`; `$C000`–`$FFFF` is permanently bank 7.

`switch_bank` (`$C815`, lines 1380–1391) is therefore not a memory store at all:

```asm
 1376: ; =============== S U B R O U T I N E =======================================
 1377: 
 1378: ; A: bank number
 1379: 
 1380: switch_bank:                            ; CODE XREF: j_switch_bank↑j
 1381:                                         ; sub_C8DF+25↓p ...
 1382:                 STA     last_bank_num
 1383:                 TYA
 1384:                 PHA
 1385:                 LDY     last_bank_num
 1386:                 LDA     bank_remap_table,Y
 1387:                 STA     bank_remap_table,Y
 1388:                 PLA
 1389:                 TAY
 1390:                 RTS
 1391: ; End of function switch_bank
```

* `A` = *logical* bank number. `STA last_bank_num` (zero page `$0F`) records it.
* `LDY last_bank_num; LDA bank_remap_table,Y` reads the remap byte from ROM `$C0F8+Y`.
* `STA bank_remap_table,Y` writes that byte to `$C0F8+Y`, which the mapper latches as the new bank number.
  Because `bank_remap_table` is `0,1,2,3,4,5,6,7`, the logical bank equals the physical bank in this build —
  the indirection exists only to make the bank number a *data* byte so the 6502 (which has no immediate-store)
  can select a run-time bank.
* `TYA/PHA … PLA/TAY` preserves `Y`; `RTS` returns.

The specialised copies (`switch_bank_0_unused` `$C824` … `switch_bank_6` `$C872`, lines 1397–1491) in-line a constant
index and are otherwise identical. `switch_bank_4` is at `$C858`; `switch_bank_5` `$C865`; `switch_bank_6` `$C872`.
`switch_bank_0_unused`–`switch_bank_3_unused` (`$C824`,`$C831`,`$C83E`,`$C84B`) are never called.

### 5.2 Every instruction that performs a bank switch

| src line | addr | instruction | value written (= new bank) | routine |
|---|---|---|---|---|
| 1387 | `$C81E` | `STA bank_remap_table,Y` | `bank_remap_table[0]` = `0` | `switch_bank` |
| 1402 | `$C82C` | `STA bank_remap_table` | `bank_remap_table[0]` = `0` | `switch_bank_0_unused` |
| 1416 | `$C839` | `STA bank_remap_table+1` | `bank_remap_table[1]` = `1` | `switch_bank_1_unused` |
| 1430 | `$C846` | `STA bank_remap_table+2` | `bank_remap_table[2]` = `2` | `switch_bank_2_unused` |
| 1444 | `$C853` | `STA bank_remap_table+3` | `bank_remap_table[3]` = `3` | `switch_bank_3_unused` |
| 1458 | `$C860` | `STA bank_remap_table+4` | `bank_remap_table[4]` = `4` | `switch_bank_4` |
| 1473 | `$C86D` | `STA bank_remap_table+5` | `bank_remap_table[5]` = `5` | `switch_bank_5` |
| 1488 | `$C87A` | `STA bank_remap_table+6` | `bank_remap_table[6]` = `6` | `switch_bank_6` |
| 1544 | `$C8B2` | `STA bank_remap_table,Y` | `bank_remap_table[0]` = `0` | `far_call_restore` |

Effective bank numbers actually selected by the code (values are the literal table bytes):

| routine | line | effective physical bank |
|---|---|---|
| `j_switch_bank` thunk / `JMP switch_bank` with `A=N` | 26 | N (dynamic) |
| `switch_bank_0_unused` | 1402 | 0 (unused) |
| `switch_bank_1_unused` | 1416 | 1 (unused) |
| `switch_bank_2_unused` | 1430 | 2 (unused) |
| `switch_bank_3_unused` | 1444 | 3 (unused) |
| `switch_bank_4` | 1458 | 4 |
| `switch_bank_5` | 1473 | 5 |
| `switch_bank_6` | 1488 | 6 |
| `far_call_restore` (`A` from stack) | 1544 | dynamic |
| `sub_CD6B` (`A = byte_26`) | 1610 | dynamic (`byte_26` set by `sub_C8DF`) |
| `sub_C8DF` final restore | 1628 | previous `last_bank_num` |
| `sub_C985` `JSR switch_bank` | 1781 | previous `last_bank_num` (restore) |
| `sub_CD6B` tail | 2622 | dynamic |
| `sub_D6DF` tail | 4340 | previous `last_bank_num` (restore) |

The complete set of bank selectors used at run time is therefore **0–7** (0–3 only via the unused copies),
with bank **6** as the workhorse (all of `sub_18003`…`j_check_enemy_overflow` live there), bank **5** for level data,
bank **4** for the alternate level/entity data loaded by `sub_C8DF`, and bank **7** (this file) permanently mapped.

### 5.3 Every place a pointer into the `$8000`–`$BFFF` window is formed or dereferenced

| src line | addr | instruction | comment |
|---|---|---|---|
| 2271 | `$CC7E` | `LDA $8030,Y` | level pointer table, `Y = current_level*2`, bank 5 selected at line 2267 |
| 2273 | `$CC83` | `LDA $8031,Y` | high byte of the same pointer → `byte_19` |
| 2277 | `$CC8D` | `LDA $8030,X` | `X = unk_CCBA[level]` |
| 2279 | `$CC92` | `LDA $8031,X` | → `byte_B9` |
| 2282 | `$CC9A` | `LDA $8030,X` | `X = unk_CCC7[level]` |
| 2284 | `$CC9F` | `LDA $8031,X` | → `byte_BB` |
| 5743 | `$DE17` | `LDA $8056` | `sub_DE14` after `JSR switch_bank_5` (5742) → `byte_10` |
| 5745 | `$DE1C` | `LDA $8057` | → `byte_11` |
| 5771 | `$DE46` | `LDA $8058` | bank 5 |
| 5773 | `$DE4B` | `LDA $8059` | bank 5 |
| 5792 | `$DE62` | `LDA $805A` | bank 5 |
| 5794 | `$DE67` | `LDA $805B` | bank 5 |
| 5848 | `$DEBB` | `LDA $805A` | bank 5 |
| 5850 | `$DEC0` | `LDA $805B` | bank 5 |
| 5900 | `$DF14` | `LDA $8056` | bank 5 |
| 5902 | `$DF19` | `LDA $8057` | bank 5 |
| 9095 | `$F190` | `word_F190: .WORD $82E8` | the only **stored** 16-bit literal that points into the window; loaded into `$1E/$1F` at 8498/8500 and 8540/8542 |

Plus the **indirect** dereferences whose pointer may be set to a `$8000`+ address:

| pointer | formed where | dereferenced at (line/addr) |
|---|---|---|
| `$18/$19` (`byte_18`) | callers store a 16-bit ROM address, e.g. 6040–6043 (`$DFA6`), 2260–2261, 6831–6834, 7500ff | 1893, 4316, 4319, 4705, 4710, 4847, 4850, 4977, 5013, 5018, 5113, 5116, 5119, 5122, 5125, 5858, 5906 (`LDA ($18),Y`) |
| `$B5/$B6` | set from `byte_18/19` in `sub_D6DF` (4320–4327) and from level data | 35 sites, `($B5),Y`, e.g. 4586, 4757, 4772, 4933 … |
| `$1E/$1F` | 8500 (`word_F190[0]` = `$82E8`), 8805/8807 (`apu_sound_tab`) | 8810, 8824, 8827, 8840, 8568, 8577, 8587, 8589, 8597 |
| `$10/$11` | set to `$8030+2n` implied and to `$8056`/`$8057` (`sub_DE14` 5743–5746), `$8058`/`$8059` (5771–5773), `$805A`/`$805B` (5792–5794, 5848–5850), `$8056`/`$8057` (5900–5902) | 1749, 1755, 4955, 4958, 4962, 5753, 5762, 5778, 5802, 5811 (`($10),Y`) |
| `$14/$15` from `sub_C8DF` (1616/1619) | `$8030 + 2*A` inside the freshly selected bank | 1616, 1619 (`LDA ($14),Y`); also 5864, 5868, 5911, 5916 |
| `$2C/$2D` (`byte_2C`=`$2C`, the 4-byte staging buffer) | written by `sub_C8DF` from the bank pointer | 2485, 2492, 2511, 2514, 2562, 2631, 2634 (`($2C),Y`) |

**Summary of how a `$8000`–`$BFFF` pointer is *formed*:**

1. `sub_C8DF` (lines 1594–1600) computes `$8030 + 2*A` at run time from `unk_C928`/`unk_C930` (base `$8030` for bank
   index 0–6, base `$C100` for index 7) and pushes it as the return-address; it is then popped into `$14/$15` and
   dereferenced by `LDA ($14),Y` at 1616/1619 **while the freshly selected bank is mapped**.
2. `sub_CC57` (2262–2288) hard-codes `$8030`/`$8031` after `JSR switch_bank_5`, then switches back with
   `JSR switch_bank_6` (2286).
3. `sub_DE14` (5741), `sub_DE91` (5826), `sub_DEF3` (5884) hard-code `$8056`–`$805B` after `JSR switch_bank_5` and
   end with `JMP switch_bank_6`.
4. `word_F190[0] = $82E8` (line 9095) is the only pointer *stored as data*; it is loaded into `$1E/$1F`
   at 8498/8500 and 8540/8542 and dereferenced with `LDA ($1E),Y` (8568, 8577, 8587, 8824, …).

`$8030` (and `$8056`–`$805B`) are the fixed entry points of the banked overlay format: `word at base+2i`.
*Speculation:* the overlay in bank 4 (loaded by `sub_C8DF`) and bank 5 (loaded by `sub_CC57`) use the same
`$8030`-relative layout seen in bank 6.

---

## 6. Writes to memory outside `$0000`–`$07FF`

### 6.1 PPU registers `$2000`–`$2007`

| addr | register | src lines |
|---|---|---|
| `$2000` | `PPU_CTRL_REG1` | 1250, 1566, 1569, 1980, 2004, 2844, 2861, 4413, 4416 |
| `$2001` | `PPU_CTRL_REG2` | 1947, 1961, 2663, 2676 |
| `$2003` | `PPU_SPR_ADDR` | 2668 |
| `$2005` | `PPU_SCROLL_REG` | 1252, 1254 |
| `$2006` | `PPU_ADDRESS` | 1204, 1207, 2716, 2718, 2737, 2740, 2748, 2750, 2809, 2811, 2816, 2818, 2823, 2826, 2848, 2851, 2872, 2875, 2881, 2883, 2904, 2905, 2913, 2915, 2947, 2948, 2949, 2950, 6093, 6095, 6099, 6101, 6602, 6604, 6645, 6646, 6823, 6825 |
| `$2007` | `PPU_DATA` | 1223, 1235, 2721, 2723, 2725, 2727, 2729, 2731, 2733, 2735, 2758, 2760, 2762, 2764, 2766, 2768, 2770, 2772, 2813, 2820, 2822, 2828, 2830, 2856, 2885, 2926, 2928, 2930, 2932, 2934, 2936, 2938, 2940, 6463, 6468, 6612, 6654 |

### 6.2 APU / I/O registers `$4000`–`$4017`

| `$4000` | `APU_REG_BASE (SQUARE1)` | 7830,X, 7836,X, 7846,X, 7851,X |
| `$4010` | `APU_DELTA_REG` | 7187, 8769 |
| `$4014` | `PPU_SPR_DMA` | 2670 |
| `$4015` | `APU_MASTERCTRL_REG` | 7823, 8768 |
| `$4016` | `JOYPAD_PORT1` | 2028, 2030, 7106, 7118, 7133, 7155 |
| `$4017` | `JOYPAD_PORT2` | 7185 |

(`$4000,X` at lines 7830/7836/7846/7851 writes the four APU channel registers through a `DEX` loop in `apu_update`;
`$4014` at line 2670 starts the OAM DMA; `$4016` writes at 2028/2030/7106/7118/7133/7155 drive controller strobe;
`$4017` at 7185 writes `$C0` during `reset`.)

### 6.3 `$8000`–`$FFFF` writes (all are mapper bank-select writes)

Exactly nine instructions — see §5.2: lines **1387, 1402, 1416, 1430, 1444, 1458, 1473, 1488, 1544**.
There are no other stores to `$8000`–`$FFFF` anywhere in the file (verified by resolving every `STA`/`STX`/`STY`
operand, including all bank7 labels, all equates, and all `$xxxx` literals).

### 6.4 Absolute stores into the stack page `$0100`–`$01FF` (data, not stack pushes)

| src line | addr | instruction | effective location |
|---|---|---|---|
| 2073 | `$CB44` | `STA $179,Y` | page-1 data array |
| 2348 | `$CCDA` | `STA $104,Y` | page-1 data array |
| 3108 | `$D16D` | `STA $175,Y` | page-1 data array |
| 3182 | `$D1C1` | `STA $17B,X` | page-1 data array |
| 3195 | `$D1D8` | `STA $179,Y` | page-1 data array |
| 3497 | `$D2F6` | `STA $181,Y` | page-1 data array |
| 4307 | `$D70E` | `STA $16D,X` | page-1 data array |
| 4711 | `$D90B` | `STA $146,X` | page-1 data array |
| 4921 | `$DA22` | `STA $16E,X` | page-1 data array |
| 4978 | `$DA63` | `STA $189,X` | page-1 data array |
| 5030 | `$DAB1` | `STA $189,X` | page-1 data array |
| 5149 | `$DB71` | `STA $104,X` | page-1 data array |
| 5779 | `$DE54` | `STA $104,Y` | page-1 data array |
| 5980 | `$DF7F` | `STA $182,X` | page-1 data array |
| 7358 | `$E8EE` | `STA $145,X` | page-1 data array |
| 7512 | `$E9C0` | `STA $145,Y` | page-1 data array |

These are ordinary data writes that happen to land in the stack page (`$0104`–`$010B`, `$0145`–`$014B`, `$016D`, `$016E`, `$0175`, `$0179`, `$017B`, `$0181`, `$0189`, `$0199`).
They are safe because `SP` never descends below `$01F0` (deepest call chain is ~6 frames).

### 6.5 Direct manipulation of the stack pointer

Only the three `TXS` sites of §2.8 (7170, 7212) and the `TSX` at 1918. All other stack traffic is `PHA`/`PLA`/`PHP`/`PLP`,
which write/read `$0100+SP` — always inside `$0100`–`$01FF`.
---

## 7. What a 1:1 C translation needs (checklist)

1. **Two label kinds become two C constructs.** `code`/`thunk` labels become functions; `label_inside_code`
   labels become `goto` targets (or `L_xxxx:` labels) inside the host function. Never emit a function for a `loc_*`.
2. **Emit procedures in address order, not source order** — here they coincide, but the fall-through table (§2.1)
   is the authority: a procedure whose last instruction is not `RTS`/`JMP`/`RTI` must *not* get a trailing `return;`.
3. **`print_string` / `sub_C6EA` / `sub_C725` / `sub_C6CA` / `stack_call_return` / `switch_case` need special support.**
   A faithful translation keeps the emulated stack (`sp`, `mem[$0100..$01FF]`) and implements
   `JSR` as `push16(pc-1)`, `RTS` as `pc = pop16()+1`. Then `PLA`/`PHA` in these routines work verbatim.
   Do **not** model them as C variadic calls.
4. **The four `print_string` sites must inline the parameter block** into the caller (rom image bytes), because
   `sub_C6EA` reads them via `($16),Y` from the program counter's address space.
5. **The two `switch_case` tables are code-relative data**: emit them as byte arrays immediately after the call
   and pass their *emulated address*.
6. **Bank-select writes are side effects on a `prg_bank` variable**, not memory writes. `STA abs` where
   `abs >= 0x8000` must be lowered to `prg_bank = <value read from ROM at abs>;` — and the ROM byte read is the
   real `bank_remap_table` value. In practice all nine sites are `bank_remap_table[+n]`.
7. **`$C000`-bank reads of `bank_remap_table`** (lines 1386, 1401, …) must read the *ROM image*, not a RAM mirror.
8. **PPU/APU writes** are the only memory-mapped I/O; §6.1/§6.2 enumerate every one.
9. **Indirect pointers** (`($xx),Y`) are the 2 KiB RAM reads/writes; the pointer variables are listed in §5.3.
10. `randomize` (1909) reads the emulated `SP`; keep `sp` a first-class variable.

---

## Appendix A — procedure table (256 procedures, address order)

`last` = the final instruction before `; End of function`. `->` = what the last instruction transfers to
(`fall` = the next procedure, `loop` = itself, otherwise the branch/jump target).

| # | procedure | addr | size | src lines | kind | last instruction | -> |
|---:|---|---:|---:|---|---|---|---|
| 1 | `j_reset` | `$C000` | 3 | 16–18 | thunk | JMP reset | reset |
| 2 | `j_switch_bank` | `$C003` | 3 | 25–27 | thunk | JMP switch_bank | switch_bank |
| 3 | `j_far_call` | `$C006` | 3 | 34–36 | thunk | JMP far_call | far_call |
| 4 | `j_ppu_nmi_disable` | `$C009` | 3 | 43–45 | thunk | JMP ppu_nmi_disable | ppu_nmi_disable |
| 5 | `j_ppu_nmi_enable` | `$C00C` | 3 | 52–54 | thunk | JMP ppu_nmi_enable | ppu_nmi_enable |
| 6 | `j_ppu_disable_all` | `$C00F` | 3 | 61–63 | thunk | JMP ppu_disable_all | ppu_disable_all |
| 7 | `j_ppu_enable_all` | `$C012` | 3 | 70–72 | thunk | JMP ppu_enable_all | ppu_enable_all |
| 8 | `j_clear_oam_buffer` | `$C015` | 3 | 79–81 | thunk | JMP clear_oam_buffer | clear_oam_buffer |
| 9 | `j_clear_screen` | `$C018` | 3 | 88–90 | thunk | JMP clear_screen | clear_screen |
| 10 | `sub_C01B` | `$C01B` | 3 | 97–99 | thunk | JMP sub_C6AB | sub_C6AB |
| 11 | `j_pad_read` | `$C01E` | 3 | 106–108 | thunk | JMP pad_read | pad_read |
| 12 | `j_all_enemies_destroyed` | `$C021` | 3 | 115–118 | thunk | JMP all_enemies_destroyed | all_enemies_destroyed |
| 13 | `sub_C024` | `$C024` | 3 | 125–128 | thunk | JMP sub_EA0B | sub_EA0B |
| 14 | `sub_C027` | `$C027` | 3 | 135–137 | thunk | JMP sub_DD9E | sub_DD9E |
| 15 | `sub_C02A` | `$C02A` | 3 | 144–147 | thunk | JMP sub_D18C | sub_D18C |
| 16 | `j_handle_pad_buttons` | `$C02D` | 3 | 154–156 | thunk | JMP handle_pad_buttons | handle_pad_buttons |
| 17 | `j_apu_play` | `$C030` | 3 | 163–166 | thunk | JMP apu_play | apu_play |
| 18 | `sub_C033` | `$C033` | 3 | 173–175 | thunk | JMP sub_CD6B | sub_CD6B |
| 19 | `j_ppu_update_background` | `$C036` | 3 | 182–184 | thunk | JMP ppu_update_background | ppu_update_background |
| 20 | `sub_C039` | `$C039` | 3 | 191–193 | thunk | JMP sub_C7D8 | sub_C7D8 |
| 21 | `sub_C03C` | `$C03C` | 3 | 200–203 | thunk | JMP sub_C7F8 | sub_C7F8 |
| 22 | `sub_C03F` | `$C03F` | 3 | 210–213 | thunk | JMP sub_C807 | sub_C807 |
| 23 | `sub_C042` | `$C042` | 3 | 220–222 | thunk | JMP sub_C8DF | sub_C8DF |
| 24 | `sub_C045` | `$C045` | 3 | 229–231 | thunk | JMP sub_C8BA | sub_C8BA |
| 25 | `sub_C048` | `$C048` | 3 | 238–241 | thunk | JMP sub_D2C0 | sub_D2C0 |
| 26 | `sub_C04B` | `$C04B` | 3 | 248–251 | thunk | JMP sub_C7EB | sub_C7EB |
| 27 | `sub_C04E` | `$C04E` | 3 | 258–260 | thunk | JMP sub_C95C | sub_C95C |
| 28 | `sub_C051` | `$C051` | 3 | 267–269 | thunk | JMP sub_C985 | sub_C985 |
| 29 | `sub_C054` | `$C054` | 3 | 276–278 | thunk | JMP sub_CA0F | sub_CA0F |
| 30 | `sub_C057` | `$C057` | 3 | 285–288 | thunk | JMP sub_D193 | sub_D193 |
| 31 | `j_ppu_cram_load` | `$C05A` | 3 | 295–297 | thunk | JMP ppu_cram_load | ppu_cram_load |
| 32 | `sub_C05D` | `$C05D` | 3 | 304–306 | thunk | JMP sub_DDF3 | sub_DDF3 |
| 33 | `j_apu_reset` | `$C060` | 3 | 313–315 | thunk | JMP apu_reset | apu_reset |
| 34 | `j_apu_stop` | `$C063` | 3 | 322–325 | thunk | JMP apu_stop | apu_stop |
| 35 | `j_apu_play_0` | `$C066` | 3 | 332–334 | thunk | JMP apu_play | apu_play |
| 36 | `j_apu_update` | `$C069` | 3 | 341–343 | thunk | JMP apu_update | apu_update |
| 37 | `sub_C06C` | `$C06C` | 3 | 350–352 | thunk | JMP sub_F0F4 | sub_F0F4 |
| 38 | `sub_C06F` | `$C06F` | 3 | 359–361 | thunk | JMP sub_F0FA | sub_F0FA |
| 39 | `sub_C072` | `$C072` | 3 | 368–370 | thunk | JMP sub_F146 | sub_F146 |
| 40 | `j_switch_case` | `$C075` | 3 | 377–380 | thunk | JMP switch_case | switch_case |
| 41 | `j_get_random_enemy` | `$C078` | 3 | 387–389 | thunk | JMP get_random_enemy | get_random_enemy |
| 42 | `sub_C07B` | `$C07B` | 3 | 396–398 | thunk | JMP sub_D5D7 | sub_D5D7 |
| 43 | `j_update_music_after_fortress_destroy` | `$C07E` | 3 | 405–408 | thunk | JMP update_music_after_fortress_destroy | update_music_after_fortress_destroy |
| 44 | `sub_C081` | `$C081` | 3 | 415–417 | thunk | JMP sub_D1A6 | sub_D1A6 |
| 45 | `sub_C084` | `$C084` | 3 | 424–426 | thunk | JMP sub_DE14 | sub_DE14 |
| 46 | `sub_C087` | `$C087` | 3 | 433–435 | thunk | JMP sub_DE5F | sub_DE5F |
| 47 | `sub_C08A` | `$C08A` | 3 | 442–445 | thunk | JMP sub_DEF3 | sub_DEF3 |
| 48 | `sub_C08D` | `$C08D` | 3 | 452–454 | thunk | JMP sub_DE91 | sub_DE91 |
| 49 | `sub_C090` | `$C090` | 3 | 461–464 | thunk | JMP sub_FEE8 | sub_FEE8 |
| 50 | `sub_C6AB` | `$C6AB` | 10 | 1029–1036 | code | STA byte_9B | fall to sub_C6B5 |
| 51 | `sub_C6B5` | `$C6B5` | 21 | 1042–1056 | code | RTS  | - |
| 52 | `sub_C6CA` | `$C6CA` | 9 | 1062–1068 | code | JSR sub_C6EA | fall to stack_call_return |
| 53 | `stack_call_return` | `$C6D3` | 7 | 1075–1082 | code | RTS  | - |
| 54 | `print_string` | `$C6DA` | 16 | 1089–1101 | code | BCC stack_call_return | stack_call_return |
| 55 | `sub_C6EA` | `$C6EA` | 59 | 1107–1151 | code | RTS  | - |
| 56 | `sub_C725` | `$C725` | 67 | 1157–1195 | code | JMP stack_call_return | stack_call_return |
| 57 | `clear_screen` | `$C768` | 59 | 1202–1241 | code | RTS  | - |
| 58 | `ppu_update_scroll_regs` | `$C7A3` | 16 | 1247–1256 | code | RTS  | - |
| 59 | `mem_page_fill` | `$C7B3` | 19 | 1263–1278 | code | RTS  | - |
| 60 | `switch_case` | `$C7C6` | 18 | 1285–1301 | code | RTS  | - |
| 61 | `sub_C7D8` | `$C7D8` | 19 | 1307–1326 | code | RTS  | - |
| 62 | `sub_C7EB` | `$C7EB` | 13 | 1332–1340 | code | JMP sub_C7EB | sub_C7EB |
| 63 | `sub_C7F8` | `$C7F8` | 15 | 1346–1357 | code | RTS  | - |
| 64 | `sub_C807` | `$C807` | 14 | 1363–1373 | code | RTS  | - |
| 65 | `switch_bank` | `$C815` | 15 | 1380–1391 | code | RTS  | - |
| 66 | `switch_bank_0_unused` | `$C824` | 13 | 1397–1405 | code | RTS  | - |
| 67 | `switch_bank_1_unused` | `$C831` | 13 | 1411–1419 | code | RTS  | - |
| 68 | `switch_bank_2_unused` | `$C83E` | 13 | 1425–1433 | code | RTS  | - |
| 69 | `switch_bank_3_unused` | `$C84B` | 13 | 1439–1447 | code | RTS  | - |
| 70 | `switch_bank_4` | `$C858` | 13 | 1453–1461 | code | RTS  | - |
| 71 | `switch_bank_5` | `$C865` | 13 | 1467–1476 | code | RTS  | - |
| 72 | `switch_bank_6` | `$C872` | 13 | 1482–1491 | code | RTS  | - |
| 73 | `far_call` | `$C87F` | 33 | 1498–1519 | code | JMP far_call_restore | far_call_restore |
| 74 | `far_call_prepare` | `$C8A0` | 12 | 1525–1534 | code | LDA ($12),Y | fall to far_call_restore |
| 75 | `far_call_restore` | `$C8AC` | 14 | 1540–1548 | code | RTS  | - |
| 76 | `sub_C8BA` | `$C8BA` | 37 | 1554–1579 | code | RTS  | - |
| 77 | `sub_C8DF` | `$C8DF` | 73 | 1585–1629 | code | JMP switch_bank | switch_bank |
| 78 | `enemy_erase_2` | `$C938` | 36 | 1653–1667 | code | RTS  | - |
| 79 | `sub_C95C` | `$C95C` | 41 | 1673–1700 | code | RTS  | - |
| 80 | `sub_C985` | `$C985` | 138 | 1706–1785 | code | JMP sub_C807 | sub_C807 |
| 81 | `sub_CA0F` | `$CA0F` | 3 | 1791–1794 | code | LDA #8 | fall to sub_CA12 |
| 82 | `sub_CA12` | `$CA12` | 133 | 1800–1871 | code | JMP sub_C807 | sub_C807 |
| 83 | `sub_CA97` | `$CA97` | 6 | 1877–1881 | code | JMP sub_CA12 | sub_CA12 |
| 84 | `ppu_cram_load` | `$CA9D` | 21 | 1888–1902 | code | RTS  | - |
| 85 | `randomize` | `$CAB2` | 27 | 1909–1926 | code | RTS  | - |
| 86 | `ppu_disable_all` | `$CACD` | 10 | 1932–1938 | code | AND #$E7 | fall to ppu_set_control2 |
| 87 | `ppu_set_control2` | `$CAD7` | 6 | 1945–1950 | code | RTS  | - |
| 88 | `ppu_enable_picture` | `$CADD` | 10 | 1957–1963 | code | RTS  | - |
| 89 | `ppu_nmi_disable` | `$CAE7` | 4 | 1969–1973 | code | AND #$7F | fall to ppu_set_control1 |
| 90 | `ppu_set_control1` | `$CAEB` | 6 | 1979–1983 | code | RTS  | - |
| 91 | `ppu_enable_all` | `$CAF1` | 6 | 1989–1993 | code | JSR ppu_enable_picture | fall to ppu_nmi_enable |
| 92 | `ppu_nmi_enable` | `$CAF7` | 10 | 1999–2006 | code | RTS  | - |
| 93 | `clear_oam_buffer` | `$CB01` | 9 | 2012–2018 | code | JMP mem_page_fill | mem_page_fill |
| 94 | `pad_read` | `$CB0A` | 54 | 2025–2060 | code | RTS  | - |
| 95 | `sub_CB40` | `$CB40` | 11 | 2067–2077 | code | RTS  | - |
| 96 | `start_game_check` | `$CB4B` | 268 | 2083–2246 | code | RTS  | - |
| 97 | `sub_CC57` | `$CC57` | 86 | 2252–2289 | code | JMP sub_D753 | sub_D753 |
| 98 | `clear_oam_buffer_unused` | `$CCD4` | 4 | 2335–2338 | code | RTS  | - |
| 99 | `sub_CCD8` | `$CCD8` | 11 | 2344–2353 | code | RTS  | - |
| 100 | `vsync` | `$CCE3` | 19 | 2360–2374 | code | RTS  | - |
| 101 | `handle_pad_buttons` | `$CCF6` | 69 | 2381–2424 | code | RTS  | - |
| 102 | `sub_CD6B` | `$CD6B` | 263 | 2436–2623 | code | JMP switch_bank | switch_bank |
| 103 | `sub_CE72` | `$CE72` | 27 | 2629–2647 | code | RTS  | - |
| 104 | `nmi_handler` | `$CE8D` | 55 | 2653–2683 | code | PLA  | fall to dummy_irq_handler |
| 105 | `dummy_irq_handler` | `$CEC4` | 1 | 2689–2691 | code | RTI  | - |
| 106 | `ppu_update_background` | `$CEC5` | 364 | 2698–2890 | code | JMP loc_CFA2 | loc_CFA2 |
| 107 | `ppu_cram_update` | `$D031` | 115 | 2897–2955 | code | RTS  | - |
| 108 | `sub_D0A4` | `$D0A4` | 27 | 2961–2985 | code | RTS  | - |
| 109 | `sub_D0BF` | `$D0BF` | 73 | 2991–3031 | code | JMP main_menu_loop | main_menu_loop |
| 110 | `check_high_score_reached` | `$D108` | 36 | 3037–3061 | code | RTS  | - |
| 111 | `sub_D12C` | `$D12C` | 96 | 3067–3130 | code | JMP sub_D12C | sub_D12C |
| 112 | `sub_D18C` | `$D18C` | 7 | 3136–3140 | code | LDA unk_D1E1,Y | fall to sub_D193 |
| 113 | `sub_D193` | `$D193` | 19 | 3146–3155 | code | JMP sub_D12C | sub_D12C |
| 114 | `sub_D1A6` | `$D1A6` | 59 | 3161–3204 | code | RTS  | - |
| 115 | `sub_D2C0` | `$D2C0` | 4 | 3434–3437 | code | RTS  | - |
| 116 | `sub_D2C4` | `$D2C4` | 4 | 3443–3446 | code | RTS  | - |
| 117 | `clear_first_last_sprites` | `$D2C8` | 28 | 3453–3477 | code | RTS  | - |
| 118 | `set_new_high_score` | `$D2E4` | 25 | 3483–3503 | code | RTS  | - |
| 119 | `sub_D2FD` | `$D2FD` | 74 | 3509–3556 | code | RTS  | - |
| 120 | `sub_D347` | `$D347` | 97 | 3562–3619 | code | JMP sub_C807 | sub_C807 |
| 121 | `sub_D3A8` | `$D3A8` | 76 | 3625–3678 | code | RTS  | - |
| 122 | `sub_D3F4` | `$D3F4` | 11 | 3684–3690 | code | JMP pad_read | pad_read |
| 123 | `sub_D3FF` | `$D3FF` | 100 | 3696–3763 | code | RTS  | - |
| 124 | `sub_D463` | `$D463` | 15 | 3769–3779 | code | RTS  | - |
| 125 | `sub_D472` | `$D472` | 14 | 3785–3796 | code | RTS  | - |
| 126 | `sub_D480` | `$D480` | 122 | 3802–3885 | code | STA byte_14 | fall to sub_D4FA |
| 127 | `sub_D4FA` | `$D4FA` | 12 | 3891–3898 | code | JSR switch_case | fall to unnamed |
| 128 | `sub_D510` | `$D510` | 10 | 3910–3916 | code | JMP sub_D5AB | sub_D5AB |
| 129 | `sub_D51A` | `$D51A` | 14 | 3922–3930 | code | JMP sub_D5AB | sub_D5AB |
| 130 | `sub_D528` | `$D528` | 37 | 3936–3965 | code | JMP sub_D5AB | sub_D5AB |
| 131 | `sub_D54D` | `$D54D` | 12 | 3971–3978 | code | JMP sub_D5AB | sub_D5AB |
| 132 | `sub_D559` | `$D559` | 18 | 3984–3995 | code | JMP sub_D5AB | sub_D5AB |
| 133 | `sub_D56B` | `$D56B` | 64 | 4001–4039 | code | STA $528,X | fall to sub_D5AB |
| 134 | `sub_D5AB` | `$D5AB` | 22 | 4045–4061 | code | JMP switch_bank_6 | switch_bank_6 |
| 135 | `sub_D5C1` | `$D5C1` | 22 | 4067–4086 | code | RTS  | - |
| 136 | `sub_D5D7` | `$D5D7` | 72 | 4092–4145 | code | RTS  | - |
| 137 | `get_random_enemy` | `$D69B` | 4 | 4219–4222 | code | RTS  | - |
| 138 | `sub_D69F` | `$D69F` | 16 | 4228–4238 | code | RTS  | - |
| 139 | `sub_D6AF` | `$D6AF` | 48 | 4244–4276 | code | RTS  | - |
| 140 | `sub_D6DF` | `$D6DF` | 108 | 4282–4341 | code | JMP switch_bank | switch_bank |
| 141 | `sub_D753` | `$D753` | 40 | 4356–4382 | code | RTS  | - |
| 142 | `sub_D77B` | `$D77B` | 20 | 4388–4401 | code | JMP sub_D77B | sub_D77B |
| 143 | `sub_D78F` | `$D78F` | 22 | 4407–4420 | code | RTS  | - |
| 144 | `set_nametable_addr_after_scroll` | `$D7A5` | 24 | 4427–4441 | code | RTS  | - |
| 145 | `sub_D7E3` | `$D7E3` | 65 | 4471–4515 | code | STA byte_AB | fall to sub_D824 |
| 146 | `sub_D824` | `$D824` | 38 | 4521–4547 | code | STA byte_A0 | fall to sub_D84A |
| 147 | `sub_D84A` | `$D84A` | 5 | 4553–4557 | code | LDY #0 | fall to sub_D84F |
| 148 | `sub_D84F` | `$D84F` | 37 | 4563–4590 | code | JSR switch_case | fall to unnamed |
| 149 | `sub_D886` | `$D886` | 14 | 4606–4614 | code | JMP sub_DA28 | sub_DA28 |
| 150 | `sub_D894` | `$D894` | 13 | 4620–4627 | code | JMP sub_DA28 | sub_DA28 |
| 151 | `sub_D8A1` | `$D8A1` | 30 | 4633–4650 | code | JMP sub_DA28 | sub_DA28 |
| 152 | `sub_D8BF` | `$D8BF` | 29 | 4656–4671 | code | JMP sub_D84A | sub_D84A |
| 153 | `sub_D8DC` | `$D8DC` | 68 | 4677–4721 | code | JMP sub_DA28 | sub_DA28 |
| 154 | `sub_D920` | `$D920` | 24 | 4727–4742 | code | JMP sub_DA28 | sub_DA28 |
| 155 | `sub_D938` | `$D938` | 2 | 4748–4750 | code | LDY byte_16 | fall to sub_D93A |
| 156 | `sub_D93A` | `$D93A` | 9 | 4756–4764 | code | STA byte_14 | fall to sub_D943 |
| 157 | `sub_D943` | `$D943` | 157 | 4770–4866 | code | JMP sub_D943 | sub_D943 |
| 158 | `sub_D9E0` | `$D9E0` | 54 | 4872–4905 | code | BNE sub_DA28 | sub_DA28 |
| 159 | `sub_DA16` | `$DA16` | 18 | 4911–4924 | code | BNE loc_DA18 | loc_DA18 |
| 160 | `sub_DA28` | `$DA28` | 14 | 4930–4940 | code | JMP sub_D84F | sub_D84F |
| 161 | `sub_DA36` | `$DA36` | 399 | 4946–5196 | code | JMP switch_bank_6 | switch_bank_6 |
| 162 | `sub_DCC5` | `$DCC5` | 6 | 5459–5462 | code | STA $430,X | fall to sub_DCCB |
| 163 | `sub_DCCB` | `$DCCB` | 19 | 5468–5476 | code | RTS  | - |
| 164 | `sub_DD26` | `$DD26` | 120 | 5555–5632 | code | RTS  | - |
| 165 | `sub_DD9E` | `$DD9E` | 85 | 5638–5703 | code | RTS  | - |
| 166 | `sub_DDF3` | `$DDF3` | 6 | 5709–5713 | code | JMP ppu_enable_all | ppu_enable_all |
| 167 | `sub_DDF9` | `$DDF9` | 27 | 5719–5735 | code | RTS  | - |
| 168 | `sub_DE14` | `$DE14` | 75 | 5741–5784 | code | JMP switch_bank_6 | switch_bank_6 |
| 169 | `sub_DE5F` | `$DE5F` | 50 | 5790–5820 | code | BNE loc_DE6E | loc_DE6E |
| 170 | `sub_DE91` | `$DE91` | 98 | 5826–5878 | code | JMP switch_bank_6 | switch_bank_6 |
| 171 | `sub_DEF3` | `$DEF3` | 85 | 5884–5928 | code | JMP switch_bank_6 | switch_bank_6 |
| 172 | `sub_DF48` | `$DF48` | 32 | 5934–5956 | code | RTS  | - |
| 173 | `reset_score` | `$DF68` | 55 | 5963–5994 | code | RTS  | - |
| 174 | `show_main_menu` | `$DFBF` | 579 | 6034–6347 | code | JMP loc_E190 | loc_E190 |
| 175 | `sub_E229` | `$E229` | 43 | 6393–6416 | code | RTS  | - |
| 176 | `sub_E266` | `$E266` | 42 | 6441–6470 | code | RTS  | - |
| 177 | `sub_E399` | `$E399` | 31 | 6597–6617 | code | RTS  | - |
| 178 | `sub_E3F4` | `$E3F4` | 69 | 6631–6679 | code | RTS  | - |
| 179 | `sound_test_menu` | `$E439` | 41 | 6685–6705 | code | JSR sub_DDF3 | fall to sub_E462 |
| 180 | `sub_E462` | `$E462` | 109 | 6711–6776 | code | BNE sub_E462 | sub_E462 |
| 181 | `show_ending_screen` | `$E4CF` | 320 | 6782–6961 | code | JMP main_menu_loop | main_menu_loop |
| 182 | `sub_E60F` | `$E60F` | 68 | 6967–7011 | code | BNE loc_E61E | loc_E61E |
| 183 | `sub_E653` | `$E653` | 30 | 7017–7037 | code | RTS  | - |
| 184 | `sub_E767` | `$E767` | 5 | 7090–7094 | code | RTS  | - |
| 185 | `sub_E76C` | `$E76C` | 83 | 7100–7157 | code | RTS  | - |
| 186 | `reset` | `$E7BF` | 71 | 7163–7203 | code | JSR apu_reset | fall to main_menu_loop |
| 187 | `main_menu_loop` | `$E806` | 24 | 7209–7220 | code | JSR start_game_check | fall to sub_E81E |
| 188 | `sub_E81E` | `$E81E` | 140 | 7226–7302 | code | BNE loc_E876 | loc_E876 |
| 189 | `sub_E8AA` | `$E8AA` | 31 | 7308–7329 | code | RTS  | - |
| 190 | `sub_E8C9` | `$E8C9` | 112 | 7335–7395 | code | LDY #$FF | fall to level_completed_sequence |
| 191 | `level_completed_sequence` | `$E939` | 92 | 7402–7460 | code | JMP sub_E81E | sub_E81E |
| 192 | `sub_E995` | `$E995` | 13 | 7466–7473 | code | JMP sub_EA0B | sub_EA0B |
| 193 | `fade_palette_before_warp_another` | `$E9A2` | 6 | 7480–7484 | code | BNE loc_E9AC | loc_E9AC |
| 194 | `fade_palette_before_warp` | `$E9A8` | 40 | 7491–7520 | code | RTS  | - |
| 195 | `update_music_after_fortress_destroy` | `$E9E0` | 30 | 7543–7566 | code | JMP apu_play | apu_play |
| 196 | `sub_EA0B` | `$EA0B` | 43 | 7586–7618 | code | RTS  | - |
| 197 | `all_enemies_destroyed` | `$EA36` | 7 | 7625–7631 | code | RTS  | - |
| 198 | `j_apu_reset_0` | `$EA3D` | 3 | 7638–7640 | thunk | JMP apu_reset | apu_reset |
| 199 | `j_apu_stop_0` | `$EA40` | 3 | 7647–7649 | thunk | JMP apu_stop | apu_stop |
| 200 | `j_apu_play_1` | `$EA43` | 3 | 7656–7658 | thunk | JMP apu_play | apu_play |
| 201 | `j_apu_update_0` | `$EA46` | 3 | 7665–7667 | thunk | JMP apu_update | apu_update |
| 202 | `sub_EA49` | `$EA49` | 3 | 7674–7676 | thunk | JMP sub_F0F4 | sub_F0F4 |
| 203 | `sub_EA4C` | `$EA4C` | 3 | 7683–7685 | thunk | JMP sub_F0FA | sub_F0FA |
| 204 | `sub_EA4F` | `$EA4F` | 3 | 7692–7694 | thunk | JMP sub_F146 | sub_F146 |
| 205 | `apu_update` | `$EA52` | 262 | 7701–7855 | code | RTS  | - |
| 206 | `sub_EB58` | `$EB58` | 34 | 7861–7883 | code | RTS  | - |
| 207 | `sub_EB7A` | `$EB7A` | 53 | 7889–7923 | code | RTS  | - |
| 208 | `apu_fx_0` | `$EBEB` | 11 | 7960–7969 | code | RTS  | - |
| 209 | `apu_fx_1` | `$EBF6` | 15 | 7975–7985 | code | RTS  | - |
| 210 | `apu_fx_6` | `$EC05` | 6 | 7991–7994 | code | STA byte_300 | fall to apu_fx_2 |
| 211 | `apu_fx_2` | `$EC0B` | 5 | 8000–8003 | code | STA $31A,X | fall to apu_fx_5 |
| 212 | `apu_fx_5` | `$EC10` | 7 | 8009–8015 | code | JMP loc_EB17 | loc_EB17 |
| 213 | `apu_fx_3` | `$EC17` | 4 | 8021–8024 | code | RTS  | - |
| 214 | `apu_fx_8` | `$EC1B` | 4 | 8030–8033 | code | RTS  | - |
| 215 | `apu_fx_4` | `$EC1F` | 9 | 8039–8044 | code | BPL sub_EC2F | sub_EC2F |
| 216 | `apu_fx_29` | `$EC28` | 7 | 8050–8054 | code | AND #$F0 | fall to sub_EC2F |
| 217 | `sub_EC2F` | `$EC2F` | 6 | 8060–8064 | code | RTS  | - |
| 218 | `apu_fx_9` | `$EC35` | 10 | 8070–8078 | code | RTS  | - |
| 219 | `apu_fx_13` | `$EC3F` | 12 | 8084–8092 | code | RTS  | - |
| 220 | `apu_fx_16` | `$EC4B` | 4 | 8098–8101 | code | RTS  | - |
| 221 | `apu_fx_10` | `$EC4F` | 28 | 8107–8125 | code | LDA #$F | fall to sub_EC6B |
| 222 | `sub_EC6B` | `$EC6B` | 9 | 8131–8137 | code | ORA byte_1E | fall to apu_fx_7 |
| 223 | `apu_fx_7` | `$EC74` | 4 | 8143–8146 | code | RTS  | - |
| 224 | `apu_fx_17` | `$EC78` | 7 | 8152–8156 | code | RTS  | - |
| 225 | `apu_fx_19` | `$EC7F` | 37 | 8162–8181 | code | RTS  | - |
| 226 | `apu_fx_21` | `$ECA4` | 12 | 8187–8193 | code | JMP apu_fx_0 | apu_fx_0 |
| 227 | `sub_ECB0` | `$ECB0` | 2 | 8199–8202 | code | RTS  | - |
| 228 | `apu_fx_18` | `$ECB2` | 37 | 8208–8226 | code | RTS  | - |
| 229 | `apu_fx_22` | `$ECD7` | 30 | 8232–8247 | code | RTS  | - |
| 230 | `apu_fx_14_15` | `$ECF5` | 19 | 8253–8265 | code | ORA #$80 | fall to sub_ED08 |
| 231 | `sub_ED08` | `$ED08` | 4 | 8271–8274 | code | EOR #$80 | fall to sub_ED0C |
| 232 | `sub_ED0C` | `$ED0C` | 4 | 8280–8283 | code | RTS  | - |
| 233 | `sub_ED10` | `$ED10` | 5 | 8289–8293 | code | RTS  | - |
| 234 | `apu_fx_20` | `$ED15` | 16 | 8299–8307 | code | BMI sub_ED2F | sub_ED2F |
| 235 | `sub_ED25` | `$ED25` | 10 | 8313–8318 | code | ORA #$C0 | fall to sub_ED2F |
| 236 | `sub_ED2F` | `$ED2F` | 4 | 8324–8327 | code | RTS  | - |
| 237 | `sub_ED33` | `$ED33` | 9 | 8333–8338 | code | RTS  | - |
| 238 | `apu_fx_23_24_25` | `$ED3C` | 8 | 8344–8348 | code | STA unk_31A,X | fall to apu_fx_12 |
| 239 | `apu_fx_12` | `$ED44` | 2 | 8354–8357 | code | RTS  | - |
| 240 | `apu_fx_28` | `$ED46` | 10 | 8363–8369 | code | RTS  | - |
| 241 | `apu_fx_26_27` | `$ED50` | 105 | 8375–8440 | code | RTS  | - |
| 242 | `sub_EDB9` | `$EDB9` | 67 | 8446–8484 | code | RTS  | - |
| 243 | `sub_EDFC` | `$EDFC` | 71 | 8490–8529 | code | RTS  | - |
| 244 | `sub_EE43` | `$EE43` | 46 | 8535–8560 | code | RTS  | - |
| 245 | `sub_EE71` | `$EE71` | 35 | 8566–8601 | code | RTS  | - |
| 246 | `sub_EE94` | `$EE94` | 22 | 8607–8619 | code | RTS  | - |
| 247 | `sub_EEAA` | `$EEAA` | 152 | 8625–8703 | code | RTS  | - |
| 248 | `sub_EF4A` | `$EF4A` | 39 | 8718–8737 | code | RTS  | - |
| 249 | `apu_reset` | `$EF71` | 8 | 8743–8748 | code | STA byte_308 | fall to apu_stop |
| 250 | `apu_stop` | `$EF79` | 23 | 8754–8771 | code | RTS  | - |
| 251 | `apu_play` | `$EF90` | 22 | 8778–8794 | code | RTS  | - |
| 252 | `sub_EFA6` | `$EFA6` | 118 | 8800–8873 | code | RTS  | - |
| 253 | `sub_F0F4` | `$F0F4` | 6 | 8988–8995 | code | RTS  | - |
| 254 | `sub_F0FA` | `$F0FA` | 4 | 9001–9005 | code | BNE loc_F0F6 | loc_F0F6 |
| 255 | `sub_F146` | `$F146` | 37 | 9048–9068 | code | RTS  | - |
| 256 | `sub_FEE8` | `$FEE8` | 41 | 9478–9508 | code | JMP sub_FEE8 | sub_FEE8 |

## Appendix B — routines that end without a terminator (quick index)

See §2.1 for the annotated table. Pure fall-through (class A) hosts, in address order:

`sub_C6AB`(L1029), `sub_C6CA`(L1062), `far_call_prepare`(L1525), `sub_CA0F`(L1791), `ppu_disable_all`(L1932), `ppu_nmi_disable`(L1969), `ppu_enable_all`(L1989), `nmi_handler`(L2653), `sub_D18C`(L3136), `sub_D480`(L3802), `sub_D56B`(L4001), `sub_D7E3`(L4471), `sub_D824`(L4521), `sub_D84A`(L4553), `sub_D938`(L4748), `sub_D93A`(L4756), `sub_DCC5`(L5459), `sound_test_menu`(L6685), `reset`(L7163), `main_menu_loop`(L7209), `sub_E8C9`(L7335), `apu_fx_6`(L7991), `apu_fx_2`(L8000), `apu_fx_29`(L8050), `apu_fx_10`(L8107), `sub_EC6B`(L8131), `apu_fx_14_15`(L8253), `sub_ED08`(L8271), `sub_ED25`(L8313), `apu_fx_23_24_25`(L8344), `apu_reset`(L8743)
