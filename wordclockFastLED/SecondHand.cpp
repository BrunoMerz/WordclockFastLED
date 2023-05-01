
//#define myDEBUG

#include "Debug.h"
#include "SecondHand.h"

CRGB _sec[SEC_STRIPE_COUNT];

extern MyDisplay display;
extern MYTM mytm;
extern MySpiffs myspiffs;

SecondHand::SecondHand(void) {
  quarterWidth=5;
}


void SecondHand::init(void) {
  FastLED.addLeds<WS2812B, SEC_STRIPE_DATA_PIN, GRB>(_sec, SEC_STRIPE_COUNT);
}


int SecondHand::mapSecondNo(uint16_t sec) {
    // LED Nummer berechnen
    // 1-37   wird 23 - 59 (immer +22)
    // 38-59  wird 1 - 22  (immer -37)
    // oder LED-Nr + 22 Modulo 59
    sec = (sec+22) % 59;
    if(!sec) sec=59;
    return(sec);
}


// sec == 0   ==> Sekundenblinken
// sec 1-59   ==> Sekundenzeiger
void SecondHand::drawSecond(uint16_t sec, COLOR_T color) {
  CHSV c;

  DEBUG_PRINTF("drawSecond COLOR_T: sec=%d, c=%#.6x\n", sec, color);
  DEBUG_FLUSH();

  if(!color)
      c = display.getHsvFromDegRnd(display.getDegree(sec));

  if(sec) { // Sekundenzeiger
    if(color)
      _sec[mapSecondNo(sec)-1] = color;
    else
      _sec[mapSecondNo(sec)-1] = c;
     
  } else {  // Sekundenblinker  
    if(color)
      display.drawPixel(4, 10, color);
    else
      display.drawPixel(4, 10, c);
  }
}


void SecondHand::clearAllSeconds(void) {
  for(uint16_t i=1; i<59; i++)
    clearSecond(i);
}


void SecondHand::clearSecond(uint16_t sec) {
  if(sec)
    _sec[sec-1] = CRGB::Black;  // Sekundenzeiger
  else
    display.clearPixel(4, 10);  // Sekundenblink LED
}


void SecondHand::setQuarterWidth(uint16_t w) {
  if(w>0 && w<60)
    quarterWidth=w;
}


void SecondHand::drawQuarter(void) {
  uint16_t quarter;
  switch(mytm.tm_min) {
    case 0:
      quarter=4;
      break;
    case 15:
      quarter=1;
      break;
    case 30:
      quarter=2;
      break;
    case 45:
      quarter=3;
      break;
    default:
      quarter=0;
      break;
  }
  if(quarter)
    drawQuarter(quarter);
}


void SecondHand::drawQuarter(uint16_t quarter) {
  COLOR_T c = display.getColor();
  uint16_t dly = map(quarterWidth,1,59,30,1);
  clearAllSeconds();
  for(uint16_t q=0; q<quarter ;q++) {
    for(uint16_t i=1; i<=59+quarterWidth; i++) {
      if(i<60)
        drawSecond(i, c);
      if(i>quarterWidth)
        clearSecond(mapSecondNo(i-quarterWidth));
      show(dly);
    }
  }
}


void SecondHand::show(uint16_t dly) {
  display.show(dly);
}
