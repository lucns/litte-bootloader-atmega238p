# litte-bootloader-atmega238p
Based on Optiboot.c v3. Tested and full run on atmega328p. 
Frequencies:

1 Mhz (1Mhz Internal)
Ideal fuses (avrdude -c usbasp -p m328p -U lfuse:w:0x62:m -U hfuse:w:0xdc:m -U efuse:w:0xff:m)
IDE Arduino "upload" variable is 4800

8 Mhz (8Mhz Internal)
Ideal fuses (avrdude -c usbasp -p m328p -U lfuse:w:0xE2:m -U hfuse:w:0xDC:m -U efuse:w:0xFD:m)
IDE Arduino "upload" variable is 38400
