call espenvPIO.bat



%ESPT% --port %PORT% write_flash --flash_size %FLASH_SIZE% 0x10000 "%BASEDIR%\firmware.bin"

pause
