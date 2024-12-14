# ZANAC Decompile Project

Zanac USA NES ROM was used for decompiling (NES-ZA-USA): https://nescartdb.com/profile/view/258/zanac  (PRG CRC32 = E292AA10)

Very primitive level of decompilation - I just marked up all procedures in IDA with minimal comments (spent 1 evening) and exported what I got.
After that I did a little tweaking by hand to make it buildable. Mathew Dillon DASM is used to assemble it.

But now you can at least start somewhere and do your experiments with this brilliant game.

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

Thank you guys!

## Progress

Project development happens sporadically when I am bored. In the evening I sit down, launch IDA and in parallel the Mesen debugger and play the game trying to understand the purpose of all procedures.
At the moment there are 422 procedures marked, but about 20 procedures are identified (more or less clear what they do).

More details here: [map.md](map.md)

## References

- https://strategywiki.org/wiki/Zanac
