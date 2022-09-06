/**
   AdjustBrightness.cpp
   @autor    Bruno Merz

   @version  2.0
   @created  20.11.2020
   @updated  12.04.2021


*/

//#define myDEBUG
#include "Debug.h"
#include "Configuration.h"
#include "AdjustBrightness.h"
#include "Ticker.h"


extern MySpiffs myspiffs;
extern MyWifi mywifi;
extern Ticker ticker;
extern IconRenderer iconRenderer;
extern MYTM mytm;

#if defined(WITH_MQTT)
extern MyMqtt mymqtt;
#endif

String formatTime(int value);

float prozent(int min, int max, int cur, int negate, int invers) { // (a-x)/(y-x)
  double q = (double)(cur-min)/(double)(max-min);
  if(invers)
    q = 1-q;
  q *= negate;
  DEBUG_PRINTF("prozent: q=%f min=%d, max=%d, cur=%d\n", q, min, max, cur);
  return q;
}


AdjustBrightness::AdjustBrightness(void) {
  _enabled = false; 
  _lastDay = -1;
}


void AdjustBrightness::init(void) {
  double lat = myspiffs.getDoubleSetting(F("latitude"));
  double lon = myspiffs.getDoubleSetting(F("longitude"));

  DEBUG_PRINTF("adjb.init: lat=%f, lon=%f, tzoffset=%d, isdst=%d\n", lat, lon, mytm.tm_tzoffset, mytm.tm_isdst);
  if(lat && lon) {
    sun.setPosition(lat, lon, mytm.tm_tzoffset-mytm.tm_isdst);
    _enabled = true;
  } else
    _enabled = false;
}


int AdjustBrightness::getOffset(void) {
  float f=0;
  double _offset=0;
  //DEBUG_PRINTF("getOffset: _enabled=%d, _lastDay=%d, tm_day=%d\n", _enabled, _lastDay, mytm.tm_mday);
  if(_enabled) {
    if(_lastDay != mytm.tm_mday) {
      DEBUG_PRINTF("getOffset: tm_year=%d, tm_mon=%d, tm_mday=%d\n", mytm.tm_year, mytm.tm_mon, mytm.tm_mday);
      sun.setCurrentDate(mytm.tm_year, mytm.tm_mon, mytm.tm_mday);
      _sunrise[0]= sun.calcAstronomicalSunrise();
      _sunrise[1]= sun.calcNauticalSunrise();
      _sunrise[2]= sun.calcCivilSunrise();
      _sunrise[3]= sun.calcSunrise();
      
      _sunset[0]= sun.calcSunset();
      _sunset[1]= sun.calcCivilSunset();
      _sunset[2]= sun.calcNauticalSunset();
      _sunset[3]= sun.calcAstronomicalSunset();
      if(mytm.tm_isdst) {
        _sunrise[0]+=60;
        _sunrise[1]+=60;
        _sunrise[2]+=60;
        _sunrise[3]+=60;
        _sunset[0]+=60;
        _sunset[1]+=60;
        _sunset[2]+=60;
        _sunset[3]+=60;
      }

#ifdef myDEBUG
      for(int i=0;i<4;i++)
        Serial.printf("_sunrise[%d]=%s\n",i, formatTime(_sunrise[i]).c_str());
      for(int i=0;i<4;i++)
        Serial.printf("_sunset[%d]=%s\n",i, formatTime(_sunset[i]).c_str());
#endif
      _lastDay = mytm.tm_mday;
    }

    int a = mytm.tm_hour * 60 + mytm.tm_min;

    if(a > _sunset[3]) f=0.0;         // 22:07 Zeit sunset[3] und 24:00
    else if(a > _sunset[2]) f=0.25;   // 21:24
    else if(a > _sunset[1]) f=0.5;    // 20:44
    else if(a > _sunset[0]) f=0.75;   // 20:12
    
    else if(a > _sunrise[3]) f=1.0;   // 6:42
    else if(a > _sunrise[2]) f=0.75;  // 6:10
    else if(a > _sunrise[1]) f=0.5;   // 5:30
    else if(a > _sunrise[0]) f=0.25;  // 4:48
    else f=0.0;                       // Zeit zwischen 00:00 und sunrise[3]

    int from = myspiffs.getIntSetting(F("brightness_from"));
    if(from == -1) from=5;
    int to = myspiffs.getIntSetting(F("brightness_to"));
    if(to == -1) to=30;

    //double mid =(from+to)/2;
    double diff = to-from;
    
    _offset = diff * f + from;

    //_offset = diff / 2 * f + from;
    DEBUG_PRINTF("getOffset: a=%d, f=%f, _offset=%f, diff=%f, from=%d\n", a, f, _offset, diff, from);
  }
  return round(_offset);
}

String formatTime(int value) {
  String tmp;
  
  int h=value/60;
  int m=value%60;
  tmp = String(h)+":";
  if (m < 10)
    tmp += '0' + String(m);
  else
    tmp += String(m);

  return tmp;
}


String AdjustBrightness::getSunrise(void) {
  return formatTime(_sunrise[3]);
}


String AdjustBrightness::getSunset(void) {
  return formatTime(_sunset[0]);
}


void AdjustBrightness::sunriseSunset(void) {
  int curTime = mytm.tm_hour * 60 + mytm.tm_min;
#if defined(WITH_MQTT) && defined(myDEBUG)
  char buf[100];
  sprintf(buf,"curTime: %d, sunrise:%d, sunset:%d", curTime, (int)_sunrise[3], (int)_sunset[0]);
  mymqtt.publishLog(buf);
#endif
  if(curTime == (int)_sunrise[3]) {
    iconRenderer.renderAndDisplay(F("/sunrise.ico"),3000,1);
    ticker.render(2, 140, myspiffs.getResource("sunrise", "Sonnenaufgang"));
#if defined(WITH_MQTT)
    mymqtt.publishLog("Sunrise now");
#endif
  } else if(curTime == (int)_sunset[0]) {
    iconRenderer.renderAndDisplay(F("/sunset.ico"),3000,1);
    ticker.render(2, 140, myspiffs.getResource("sunset", "Sonnenuntergang"));
#if defined(WITH_MQTT)
    mymqtt.publishLog("Sunset now");
#endif
  }
}


void AdjustBrightness::forceUpdate(void) {
  _lastDay = -1;
  getOffset();
}
