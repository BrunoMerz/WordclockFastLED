/**
   IconRenderer.h
   @autor    Bruno Merz

   @version  1.0
   @created  09.01.2020
   @updated  09.01.2020

   Version history:
   V 1.0:  - Created.

*/

#pragma once

class IconRenderer;

#include "Configuration.h"
#include "MyDisplay.h"

#if (USE_JPGDECODING == 1)
#include <JPEGDecoder.h>  // JPEG decoder library
#endif




class IconRenderer {
  public:
    IconRenderer(void);
    void renderAndDisplay(String iconFile, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1);
    // delayAfter (ms)
    // cleanUp  0 - don't clear LED Matrix neither before nor after
    //          1 - clear LED Matrix before displaying icon
    //          2 - clear LED Matrix after displaying icon and delay
    //          3 - 1 + 2

#if (USE_JPGDECODING == 1)
    void renderAndDisplayJPEG(String iconFileName, int delayAfter, byte cleanUp, int16_t x_pos, int16_t y_pos);
    void jpegInfo();
    void renderJPEG(int xpos, int ypos);
#endif

  private:
    ICOHEADER *_icoHeader;
    ICONDIRENTRY _iconDirentry, *_pIconDirentry;
    BITMAPINFOHEADER _bitmapInfoHeader, *_pBitmapInfoHeader;
    ICON *_icon;
    uint16_t  _x_pos;
    uint16_t  _y_pos;
};
