PROGRAM    = optiboot
ISPTOOL	   = stk500v2
ISPPORT	   = usb
ISPSPEED   = -b 115200

MCU_TARGET = atmega168
LDSECTION  = --section-start=.text=0x3e00

ISPFUSES    = avrdude -c $(ISPTOOL) -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
-e -u -U lock:w:0x3f:m -U efuse:w:0x$(EFUSE):m -U hfuse:w:0x$(HFUSE):m -U lfuse:w:0x$(LFUSE):m
ISPFLASH    = avrdude -c $(ISPTOOL) -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
-U flash:w:$(PROGRAM)_$(TARGET).hex -U lock:w:0x0f:m

STK500 = "C:\Program Files\Atmel\AVR Tools\STK500\Stk500.exe"
STK500-1 = $(STK500) -e -d$(MCU_TARGET) -pf -vf -if$(PROGRAM)_$(TARGET).hex \
-lFF -LFF -f$(HFUSE)$(LFUSE) -EF8 -ms -q -cUSB -I200kHz -s -wt
STK500-2 = $(STK500) -d$(MCU_TARGET) -ms -q -lCF -LCF -cUSB -I200kHz -s -wt

OBJ        = $(PROGRAM).o
OPTIMIZE = -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls

DEFS       = 
LIBS       =

CC         = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,$(LDSECTION) -Wl,--relax -nostartfiles

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

atmega328_pro8: TARGET = atmega328p_bootloader
atmega328_pro8: MCU_TARGET = atmega328p
#atmega328_pro8: CFLAGS += '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'
#atmega328_pro8: AVR_FREQ = 1000000L
atmega328_pro8: LDSECTION  = --section-start=.text=0x7e00
atmega328_pro8: $(PROGRAM)_atmega328p_bootloader.hex

atmega328_pro8_isp: atmega328_pro8
atmega328_pro8_isp: TARGET = atmega328p_bootloader
atmega328_pro8_isp: MCU_TARGET = atmega328p
atmega328_pro8_isp: HFUSE = DE # 512 byte boot
atmega328_pro8_isp: LFUSE = FF # Low power xtal (16MHz) 16KCK/14CK+65ms
atmega328_pro8_isp: EFUSE = 05 # 2.7V brownout
atmega328_pro8_isp: isp

isp: $(TARGET)
	$(ISPFUSES)
	$(ISPFLASH)

isp-stk500: $(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

%.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
