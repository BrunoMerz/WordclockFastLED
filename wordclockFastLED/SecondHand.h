#pragma once


#include "Configuration.h"
#include "MyDisplay.h"
#include "MySpiffs.h"


class SecondHand {
  public:
    SecondHand();
    void init();
    void drawSecond(uint16_t sec, COLOR_T color);
    void clearSecond(uint16_t sec);
    void clearAllSeconds(void);
    void drawQuarter(void);
    void drawQuarter(uint16_t quarter);
    void setQuarterWidth(uint16_t w);
    void show(uint16_t dly=0);
  private:
    int mapSecondNo(uint16_t sec);
    uint16_t  quarterWidth;
};
