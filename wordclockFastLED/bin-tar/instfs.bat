call espenv.bat



%ESPT% --port %PORT% write_flash --flash_size %FLASH_SIZE% %SPIFFS_START% "%BASEDIR%\bin-tar\wordclock%VERS%.%FS%.bin"

pause "Jetzt neu starten!!!!!!"
