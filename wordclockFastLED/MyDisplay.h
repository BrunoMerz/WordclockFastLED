#pragma once

#define COLOR_T uint32_t

#include "Configuration.h"
#include "TickerAlphabet.h"
#include "MySpiffs.h"

#include <FastLED.h>
/*
 Wiring Led stripe
 PixelMap is structured so that the letters can be addressed as usual
 i.e. E (0,0)   S(0,1)   I(0,3)  S(0,4)  T(0,5) ......

 113         114            11
 Ol   +-+ +-+ +-+ +-+ +--OR
    \ | | | | | | | | |  /
    E S K I S T A F Ü N F
    | | | | | | | | | | |
    Z E H N Z W A N Z I G
    | | | | | | | | | | |
    D R E I V I E R T E L
    | | | | | | | | | | |
    V O R F U N K N A C H
    | | | | | | | | | | |
    H A L B A E L F Ü N F
    | | | | | | | | | | |
    E I N S X A M Z W E I
    | | | | | | | | | | |
    D R E I P M J V I E R
    | | | | | | | | | | |
    S E C H S N L A C H T
    | | | | | | | | | | |
    S I E B E N Z W Ö L F
    | | | | | | | | | | |
    Z E H N E U N K U H R
   /  | | | | | | | | |  \
  UL--+ +-+ +-+ +-+ +-+   UR
  102                      0 (Start vom Datenpin aus gesehen)
*/
const byte pixelMap[] PROGMEM = {
  112,  92, 91, 72, 71, 52, 51, 32, 31, 12, 10,
  111,  93, 90, 73, 70, 53, 50, 33, 30, 13,  9,
  110,  94, 89, 74, 69, 54, 49, 34, 29, 14,  8,
  109,  95, 88, 75, 68, 55, 48, 35, 28, 15,  7,
  108,  96, 87, 76, 67, 56, 47, 36, 27, 16,  6,
  107,  97, 86, 77, 66, 57, 46, 37, 26, 17,  5,
  106,  98, 85, 78, 65, 58, 45, 38, 25, 18,  4,
  105,  99, 84, 79, 64, 59, 44, 39, 24, 19,  3,
  104, 100, 83, 80, 63, 60, 43, 40, 23, 20,  2,
  103, 101, 82, 81, 62, 61, 42, 41, 22, 21,  1,
  113,  11,  0, 102
};

  const byte coorMap[][2] PROGMEM = {
    {5,0},
    {0,0},  {0,0},  {0,0},  {1,0},  {2,0},  {3,0},  {4,0},  {5,0},  {6,0},  {7,0},  {8,0},  {9,0},  {10,0},  {10,0},  {10,0},
    {10,0}, {10,0}, {10,0}, {10,1}, {10,2}, {10,3}, {10,4}, {10,5}, {10,6}, {10,7}, {10,8}, {10,9}, {10,10}, {10,10}, {10,10},
    {10,9}, {10,9}, {10,9}, {9,9},  {8,9},  {7,9},  {6,9},  {4,9},  {3,9},  {2,9},  {1,9},  {0,9},  {0,9},   {0,9},
    {0,9},  {0,9},  {0,9},  {0,8},  {0,7},  {0,6},  {0,5},  {0,4},  {0,3},  {0,2},  {0,1},  {0,0},  {0,0},   {0,0},   {0,0}
  };
  
class MyDisplay {
  public:
    MyDisplay();
    void init(uint8_t dataPin, uint16_t ledStripeCount);
 
    COLOR_T getColor(uint8_t r, uint8_t g, uint8_t b);
    COLOR_T getColor(void);
    uint16_t getWidth(void);
    uint16_t getHeight(void);
    COLOR_T getRGBColor(uint16_t hsv);
    COLOR_T getColorHSV(uint8_t deg, uint8_t saturation, uint8_t brightness);
    CHSV    getHsvFromDegRnd(uint16_t rnd, uint16_t deg);
    uint16_t getDegree(uint16_t x, uint16_t y);
    int16_t getBrightness(void);
    COLOR_T getPixelColor(uint16_t x_pos, uint16_t y_pos);

    void clear(void);
    void clear(CRGB color);
    void clear(CHSV color);
    
    void setColor(COLOR_T color);
    void setTextColor(COLOR_T color);
    void setCursor(uint16_t x, uint16_t y);
    void setBrightness(uint16_t brightness);
    void setBrightness(void);
    
    void print(String letter);
    void DrawRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, COLOR_T c);
    void DrawRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, CHSV c);
    void drawPixel(uint16_t x_pos, uint16_t y_pos, COLOR_T color);
    void drawPixel(uint16_t x_pos, uint16_t y_pos, CHSV color);
    void drawPixel(uint16_t x_pos, uint16_t y_pos, CRGB color);
    void clearPixel(uint16_t x_pos, uint16_t y_pos);
    void DrawFilledRectangle(uint16_t x_pos, uint16_t y_pos, uint16_t x1_pos, uint16_t y1_pos, COLOR_T color);
    void DrawFilledRectangle(uint16_t x_pos, uint16_t y_pos, uint16_t x1_pos, uint16_t y1_pos, CHSV color);
    void show(uint16_t dly=0);
    
    void DebugOutput(boolean colorOutput);
    void setMaxPowerInVoltsAndMilliamps(uint8_t volts, uint32_t milliamps);
    void doBlur2d(fract8 blur_amount);
    void addColor(uint16_t x_pos, uint16_t y_pos, CHSV color);
    void fadePixel(uint16_t x_pos, uint16_t y_pos, byte step);
    void shiftMatrix(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
    void shiftMatrix(void);


  private:
    byte      _dataPin;
    uint16_t  _ledStripeCount;
    uint16_t  _x;
    uint16_t  _y;
    uint16_t  _width;
    uint16_t  _height;
    uint32_t  _color;
    uint16_t  _brightness;
    uint16_t  _angleOffset;
    byte      _brightnessInPercent;
#if (LED_LIB == 2)
    cLEDMatrixBase *m_Matrix;
#endif

};
