@echo off
title AvrDude Command Window
@echo.
@echo. trying to connect to device...
avrdude -c usbasp -p m328p -U lfuse:w:0xE2:m -U hfuse:w:0xDC:m -U efuse:w:0xFD:m
pause