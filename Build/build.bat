del zanac.nes
dasm ../src/bank0.asm -f3 -obank0.bin -lbank0.lst
dasm ../src/bank1.asm -f3 -obank1.bin -lbank1.lst
dasm ../src/bank2.asm -f3 -obank2.bin -lbank2.lst
dasm ../src/bank3.asm -f3 -obank3.bin -lbank3.lst
dasm ../src/bank4.asm -f3 -obank4.bin -lbank4.lst
dasm ../src/bank5.asm -f3 -obank5.bin -lbank5.lst
dasm ../src/bank6.asm -f3 -obank6.bin -lbank6.lst
dasm ../src/bank7.asm -f3 -obank7.bin -lbank7.lst
COPY /B bank0.bin + bank1.bin + bank2.bin + bank3.bin + bank4.bin + bank5.bin + bank6.bin + bank7.bin /B prg.bin
COPY /B nes_header.bin + prg.bin /B zanac.nes
python crc32.py prg.bin