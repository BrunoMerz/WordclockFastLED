rem spiffs_start=0x200000
rem spiffs_end=0x3FA000
rem spiffs_blocksize=8192
rem spiffs_pagesize=256

rem size=2072576

rem D1 mini
rem generic.menu.eesz.4M2M=4MB (FS:2MB OTA:~1019KB)
rem generic.menu.eesz.4M2M.build.flash_size=4M
rem generic.menu.eesz.4M2M.build.flash_size_bytes=0x400000
rem generic.menu.eesz.4M2M.build.flash_ld=eagle.flash.4m2m.ld
rem generic.menu.eesz.4M2M.build.spiffs_pagesize=256
rem generic.menu.eesz.4M2M.upload.maximum_size=1044464
rem generic.menu.eesz.4M2M.build.rfcal_addr=0x3FC000
rem generic.menu.eesz.4M2M.build.spiffs_start=0x200000
rem generic.menu.eesz.4M2M.build.spiffs_end=0x3FA000
rem generic.menu.eesz.4M2M.build.spiffs_blocksize=8192


rem D1 mini pro
rem d1_mini_pro.menu.eesz.16M14M.build.spiffs_pagesize=256
rem d1_mini_pro.menu.eesz.16M14M.build.spiffs_start=0x200000
rem d1_mini_pro.menu.eesz.16M14M.build.spiffs_end=0xFFA000
rem d1_mini_pro.menu.eesz.16M14M.build.spiffs_blocksize=8192


call espenv.bat

@echo on

%MKSP% --create %BASEDIR%\data -b 8192 -p 256 -s %SPIFFS_SIZE% %BASEDIR%\bin-tar\wordclock%VERS%.%FS%.bin

pause
