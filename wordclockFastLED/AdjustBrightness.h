/**
   AdjustBrightness.h
   @autor    Bruno Merz

   @version  2.0
   @created  20.11.2020
   @updated  12.04.2021

   Regarding sunrise and sunset brightness will be ajusted slightly

*/

#pragma once

#include "MySpiffs.h"
#include "MyWifi.h"
#include "Ticker.h"
#include "IconRenderer.h"
#include "sunset.h"
#ifdef WITH_MQTT
#include "MyMqtt.h"
#endif

class AdjustBrightness {
  public:
    AdjustBrightness(void);
    void init(void);
    int getOffset(void);
    String getSunrise(void);
    String getSunset(void);
    void sunriseSunset(void);
    void forceUpdate(void);

  private:
    SunSet sun;
    boolean _enabled;
    byte    _lastDay;
    double  _sunrise[4];
    double  _sunset[4];
};
