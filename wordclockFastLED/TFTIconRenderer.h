/**
   TFTIconRenderer.h
   @autor    Bruno Merz

   @version  1.0
   @created  09.01.2020
   @updated  09.01.2020

   Version history:
   V 1.0:  - Created.

*/

#pragma once

#include "MySpiffs.h"

class MyTFT;
class MySpiffs;

#ifdef LILYGO_T_HMI

class TFTIconRenderer {
  public:
    static TFTIconRenderer* getInstance();
    void renderAndDisplay(char *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1);
    void renderAndDisplay(const __FlashStringHelper *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1);
    // delayAfter (ms)
    // cleanUp  0 - don't clear display neither before nor after
    //          1 - clear display before displaying icon
    //          2 - clear display after displaying icon and delay
    //          3 - 1 + 2


  private:
    TFTIconRenderer(void);
    static TFTIconRenderer *instance;
    MyTFT *tft;
    MySpiffs *myfs;
    ICOHEADER *_icoHeader;
    ICONDIRENTRY _iconDirentry, *_pIconDirentry;
    BITMAPINFOHEADER _bitmapInfoHeader, *_pBitmapInfoHeader;
    ICON *_icon;
    uint16_t  _x_pos;
    uint16_t  _y_pos;
};

#else
class TFTIconRenderer {
  public:
    static TFTIconRenderer* getInstance();
    void renderAndDisplay(char *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1) {};
    void renderAndDisplay(const __FlashStringHelper *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1) {};
private:
    TFTIconRenderer(void);
    static TFTIconRenderer *instance;
};

#endif