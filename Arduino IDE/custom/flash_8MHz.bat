@echo off
title AvrDude Command Window
@echo.
@echo. trying to connect to device...
avrdude -c usbasp -p m328p -U flash:w:optiboot_atmega328p_bootloader_8Mhz_38400bps.hex
pause