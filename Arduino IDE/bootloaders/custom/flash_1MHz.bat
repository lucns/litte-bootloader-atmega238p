@echo off
title AvrDude Command Window
@echo.
@echo. trying to connect to device...
avrdude -c usbasp -p m328p -U flash:w:optiboot_atmega328p_bootloader_1MHz_4800bps.hex
pause