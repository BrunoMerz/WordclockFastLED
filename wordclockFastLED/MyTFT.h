/**
   MyTft.h
   @autor    Bruno Merz

   @version  1.0
   @created  29.11.2021

*/

#pragma once

#ifdef LILYGO_T_HMI

#include <TFT_eSPI.h>
#include <SPI.h>
#include "TFTIconRenderer.h"

class TFTIconRenderer;


class MyTFT: public TFT_eSPI {
  public:
    static MyTFT* getInstance();
    void init(void);
    void drawStateLine(String text);
    void clearMainCanvas(void);
    void clearStateCanvas(void);
    void renderAndDisplayIcon(char *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1);
    void renderAndDisplayIcon(const __FlashStringHelper *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1);
    void drawClock(void);
    void drawTime(void);
  private:
    static MyTFT *instance;
    MyTFT(void): TFT_eSPI(TFT_WIDTH,TFT_HEIGHT) {
      _mainCanvasWidth = width();
      _mainCanvasHeight = height(); // -fontHeight(STD_FONT);
      _stateLineYPos = _mainCanvasHeight;
      
    };
    TFTIconRenderer *ir;
    uint16_t _stateLineYPos;
    uint16_t _mainCanvasWidth;
    uint16_t _mainCanvasHeight;float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
    float sdeg=0, mdeg=0, hdeg=0;
    uint16_t osx=120, osy=120, omx=120, omy=120, ohx=120, ohy=120;  // Saved H, M, S x & y coords
    uint16_t x0=0, x1=0, yy0=0, yy1=0;
    boolean initial=true;
    
};

#else

class MyTFT {
  public:
    MyTFT(void) {};
    static MyTFT *getInstance() {};
    void init(void) {};
    void drawPixel(uint16_t x, uint16_t y, uint32_t c) {};
    uint32_t color565(uint8_t r, uint8_t g, uint8_t b) {};
    uint16_t height() {};
    uint16_t width() {};
    void drawStateLine(String text) {};
    void clearMainCanvas(void) {};
    void clearStateCanvas(void) {};
    void renderAndDisplayIcon(char *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos, int16_t y_pos) {};
    void renderAndDisplayIcon(const __FlashStringHelper *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1) {};
    void drawClock(void) {};
    void drawTime(void) {};
    void drawString(String&, int, int, int) {};
};

#endif