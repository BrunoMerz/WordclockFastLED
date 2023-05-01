@echo off

set ESPT=python C:\Users\mz\Documents\ESP-Tools\esptool-master\esptool.py
rem set ESPT=C:\Users\mz\Documents\ESP-Tools\esptool\esptool.exe

set BASEDIR=C:\Users\mz\Documents\GitHub\wordclockFastLED\.pio\build\lilygo-t-hmi

set PORT=COM5

rem set MKSP=C:\Users\mz\Documents\ESP-Tools\mkspiffs\mkspiffs.exe
set MKSP=C:\Users\mz\AppData\Local\Arduino15\packages\esp8266\tools\mklittlefs\2.5.0-4-fe5bb56\mklittlefs.exe
set VERS=74
set LIC=.lic
set PIN=D4
set FS=littlefs


rem LILYGO T-HMI
set FLASH_SIZE=16MB
set SPIFFS_START=0xc90000 
set SPIFFS_SIZE=0x360000
set TYPE=d1r2

rem D1 mini pro
rem set FLASH_SIZE=16MB
rem set SPIFFS_START=0x00200000
rem set SPIFFS_SIZE=14655488
rem set TYPE=pro

@echo on