@echo off
title AvrDude Command Window
@echo.
@echo. trying to connect to device...
avrdude -c usbasp -p m328p -U lfuse:w:0x62:m -U hfuse:w:0xdc:m -U efuse:w:0xff:m
pause