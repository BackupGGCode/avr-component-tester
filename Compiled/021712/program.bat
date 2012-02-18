@echo off
echo C:\WinAVR-20100110\Datasheets\sources\mine\ACT-SRC\avr-component-tester\Compiled\021712
cls
avrdude -p atmega328p -B 6 -c usbtiny -P USB -U flash:w:TransistorTestNew.hex:a -U eeprom:w:TransistorTestNew.eep:a
echo.
echo.
echo Complete.
pause
exit