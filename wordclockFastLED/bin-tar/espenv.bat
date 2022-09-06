@echo off

set ESPT=python C:\Users\mz\Documents\ESP-Tools\esptool-master\esptool.py
rem set ESPT=C:\Users\mz\Documents\ESP-Tools\esptool\esptool.exe

set BASEDIR=C:\Users\mz\Documents\GitHub\wordclockFastLED\wordclockFastLED

set PORT=COM7
rem set MKSP=C:\Users\mz\Documents\ESP-Tools\mkspiffs\mkspiffs.exe
set MKSP=C:\Users\mz\AppData\Local\Arduino15\packages\esp8266\tools\mklittlefs\2.5.0-4-fe5bb56\mklittlefs.exe
set VERS=70
set LIC=.lic
set PIN=D4
set FS=littlefs


rem D1 mini
set FLASH_SIZE=4MB
set SPIFFS_START=0x00200000 
set SPIFFS_SIZE=2072576
set TYPE=d1r2

rem D1 mini pro
rem set FLASH_SIZE=16MB
rem set SPIFFS_START=0x00200000
rem set SPIFFS_SIZE=14655488
rem set TYPE=pro

@echo on