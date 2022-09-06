call espenv.bat



%ESPT% --port %PORT% write_flash --flash_size %FLASH_SIZE% 0x0 "%BASEDIR%\bin-tar\wordclock%VERS%.%TYPE%%LIC%.%PIN%.bin"

pause
