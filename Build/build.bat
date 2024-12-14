del zanac.nes
dasm ../src/zero_filled_bank.asm -f3 -obank0.bin
dasm ../src/zero_filled_bank.asm -f3 -obank1.bin
dasm ../src/zero_filled_bank.asm -f3 -obank2.bin
dasm ../src/zero_filled_bank.asm -f3 -obank3.bin
dasm ../src/bank4.asm -f3 -obank4.bin
dasm ../src/bank5.asm -f3 -obank5.bin
dasm ../src/bank6.asm -f3 -obank6.bin -lbank6.lst
dasm ../src/bank7.asm -f3 -obank7.bin -lbank7.lst
COPY /B bank0.bin + bank1.bin + bank2.bin + bank3.bin + bank4.bin + bank5.bin + bank6.bin + bank7.bin /B prg.bin
COPY /B nes_header.bin + prg.bin /B zanac.nes
python crc32.py prg.bin