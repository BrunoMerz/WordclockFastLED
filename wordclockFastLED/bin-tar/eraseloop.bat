call espenv.bat


:start
%ESPT% --port %PORT% erase_flash 
goto start
pause
