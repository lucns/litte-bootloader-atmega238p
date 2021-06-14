# litte-bootloader-atmega238p
Based on Optiboot.c v3. Tested and full run on atmega328p. 
Frequencies (F_CPU variable):

0.9984 Mhz (1Mhz Internal)
Ideal fuses (avrdude -c usbasp -p m328p -U lfuse:w:0x62:m -U hfuse:w:0xdc:m -U efuse:w:0xff:m)
IDE Arduino BAUD_RATE variable is 9600

8.2944 Mhz (8Mhz Internal)
Ideal fuses (avrdude -c usbasp -p m328p -U lfuse:w:0xE2:m -U hfuse:w:0xDC:m -U efuse:w:0xFD:m)
IDE Arduino BAUD_RATE variable is 57600
