/**
   MyTft.cpp
   @autor    Bruno Merz

   @version  1.0
   @created  29.11.2021

*/


#define myDEBUG
#include "Debug.h"
#include "Configuration.h"
#include "MyTFT.h"


extern MYTM mytm;

#ifdef LILYGO_T_HMI

MyTFT* MyTFT::instance = 0;

MyTFT *MyTFT::getInstance() {
  if (!instance)
  {
      instance = new MyTFT();
  }
  return instance;
}


void MyTFT::init(void) {
  pinMode(PWR_EN_PIN, OUTPUT);
  digitalWrite(PWR_EN_PIN, HIGH);
  TFT_eSPI::init();
  setRotation(1);
  setSwapBytes(true);
  ir = TFTIconRenderer::getInstance();
  renderAndDisplayIcon(F("/TFTwordclock.ico"),0,1);
}


void MyTFT::clearMainCanvas(void) {
  DEBUG_PRINTF("clearMainCanvas: %d, %d\n",_mainCanvasWidth, _mainCanvasHeight);
  fillScreen(TFT_BLACK);
  //fillRect(0, 0, _mainCanvasWidth, _mainCanvasHeight, TFT_BLACK);
}

void MyTFT::clearStateCanvas(void) {
  fillRect(0, _mainCanvasHeight, _mainCanvasWidth, fontHeight(STD_FONT), TFT_BLACK);
}


void MyTFT::drawStateLine(String text) {
  clearStateCanvas();
  drawString(text,0,_stateLineYPos,STD_FONT);
}



void MyTFT::renderAndDisplayIcon(char *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos, int16_t y_pos) {
    ir->renderAndDisplay(iconFileName, delayAfter, cleanUp, x_pos, y_pos);
}

void MyTFT::renderAndDisplayIcon(const __FlashStringHelper *iconFileName, int delayAfter, byte cleanUp, int16_t x_pos, int16_t y_pos) {
  char tmp[40];
  strncpy_P(tmp, (const char*)iconFileName, sizeof(tmp));
  renderAndDisplayIcon(tmp, delayAfter, cleanUp, x_pos, y_pos);
}

#define XOFFSET 40
#define YOFFSET 0

void MyTFT::drawClock(void) {
  DEBUG_PRINTLN("drawClock start");
  fillScreen(TFT_LIGHTGREY);
  
  setTextColor(TFT_WHITE, TFT_DARKGREY);  // Adding a background colour erases previous text automatically
  
  // Draw clock face
  fillCircle(120+XOFFSET, 120, 118, TFT_GREEN);
  fillCircle(120+XOFFSET, 120, 110, TFT_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*114+120+XOFFSET;
    yy0 = sy*114+120;
    x1 = sx*100+120+XOFFSET;
    yy1 = sy*100+120;

    drawLine(x0, yy0, x1, yy1, TFT_GREEN);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*102+120+XOFFSET;
    yy0 = sy*102+120;
    // Draw minute markers
    drawPixel(x0, yy0, TFT_WHITE);
    
    // Draw main quadrant dots
    if(i==0 || i==180) fillCircle(x0, yy0, 2, TFT_WHITE);
    if(i==90 || i==270) fillCircle(x0, yy0, 2, TFT_WHITE);
  }

  fillCircle(120+XOFFSET, 121, 3, TFT_WHITE);

  // Draw text at position 120,260 using fonts 4
  // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . - a p m
  // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
  drawString("Uhrzeit",0,0,4);

  initial = true;
  DEBUG_PRINTLN("drawClock done");
}

void MyTFT::drawTime(void) {
    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = mytm.tm_sec*6;                  // 0-59 -> 0-354
    mdeg = mytm.tm_min*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = mytm.tm_hour*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    if (mytm.tm_sec==0 || initial) {
      initial = false;
      // Erase hour and minute hand positions every minute
      drawLine(ohx+XOFFSET, ohy, 120+XOFFSET, 121, TFT_BLACK);
      ohx = hx*62+121;    
      ohy = hy*62+121;
      drawLine(omx+XOFFSET, omy, 120+XOFFSET, 121, TFT_BLACK);
      omx = mx*84+120;    
      omy = my*84+121;
    }

      // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
      drawLine(osx+XOFFSET, osy, 120+XOFFSET, 121, TFT_BLACK);
      osx = sx*90+121;    
      osy = sy*90+121;
      drawLine(osx+XOFFSET, osy, 120+XOFFSET, 121, TFT_RED);
      drawLine(ohx+XOFFSET, ohy, 120+XOFFSET, 121, TFT_WHITE);
      drawLine(omx+XOFFSET, omy, 120+XOFFSET, 121, TFT_WHITE);
      drawLine(osx+XOFFSET, osy, 120+XOFFSET, 121, TFT_RED);

    fillCircle(120+XOFFSET, 121, 3, TFT_RED);
}
#endif