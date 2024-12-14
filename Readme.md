# ZANAC Decompile Project

Zanac USA NES ROM was used for decompiling (NES-ZA-USA): https://nescartdb.com/profile/view/258/zanac  (PRG CRC32 = E292AA10)

The cartridge uses the UNROM mapper: 8KByte CHR RAM + 128KByte PRG ROM.
PRG ROM composition:
- Bank 0-3: empty (filled by 0)
- Bank 4: graphics data  (unused space filled by 0xff)
- Bank 5: graphics data  (unused space filled by 0xff)
- Bank 6: enemy & game logic   (unused space filled by 0xff)
- Bank 7: main engine (ppu, apu, reset etc.)  (unused space filled by 0xff)

Developers (COMPILE):
- JEMINI,JANUS,MOO: game design
- JEMINI: program
- MIYAMOTO: music
- JANUS: graphics
- MNC NUI,WAO ISEE,YORIKI: test play

## References

- https://strategywiki.org/wiki/Zanac
