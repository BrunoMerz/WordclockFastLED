/**
 * MyAlexa.cpp
 * Class integration with Alexa
 *
 * @autor    B. Merz
 * @version  1.0
 * @created  07.07.2022
 *
 */

#include "Configuration.h"

#ifdef WITH_ALEXA

//#define myDEBUG
#include "Debug.h"

#include "MyAlexa.h"
#include "MyDisplay.h"
#include "MySpiffs.h"
#include "Helper.h"
#include "AdjustBrightness.h"

extern AdjustBrightness adjb;
extern MyAlexa alexa;
extern AsyncWebServer server;
extern String getHostname(String *hn);
extern MyDisplay display;
extern MySpiffs myspiffs;
extern int lastMinutes;

static char lastSetting[30];
static uint32_t lastValue;
EspalexaDevice *espalexaDevice;
void alexaCallback(EspalexaDevice* d);
COLOR_T colorCTtoRGB(uint16_t mired);
/*
 * Alexa Voice On/Off/Brightness/Color Control. Emulates a Philips Hue bridge to Alexa.
 * 
 * This was put together from these two excellent projects:
 * https://github.com/kakopappa/arduino-esp8266-alexa-wemo-switch
 * https://github.com/probonopd/ESP8266HueEmulator
 */

MyAlexa::MyAlexa() {
  alexaEnabled = false;
}


void MyAlexa::initAlexa()
{
  String hn;
  // create Espalexa object
  espalexa = new Espalexa();
  hn=myspiffs.getSetting(F("alexaName"));
  if(!hn.length())
    getHostname(&hn);
  // Define your devices here. 
  espalexaDevice = new EspalexaDevice(hn, alexaCallback, EspalexaDeviceType::extendedcolor); //color + color temperature
  espalexa->addDevice(espalexaDevice);
  //espalexaDevice->setName(hn);
  /*espalexaDevice->setValue(128); //creating the device yourself allows you to e.g. update their state value at any time!
  espalexaDevice->setColor(200); //color temperature in mireds
  espalexaDevice->setColor(255,160,0); //color in RGB
  espalexaDevice->setColor(14000,255); //color in Hue + Sat
  espalexaDevice->setColorXY(0.50,0.50); //color in XY
  */
  setBrightness();
  setColor();
  setState();

  alexaEnabled = espalexa->begin(&server);
  DEBUG_PRINTF("initAlexa: alexaEnabled=%d\n",alexaEnabled);
}

void MyAlexa::loopAlexa()
{
  if (!alexaEnabled) return;
  espalexa->loop();
  delay(1);
}

void MyAlexa::setState(boolean state) {
  if(alexaEnabled)
    espalexaDevice->setState(state);
}

void MyAlexa::setBrightness(void) {
  if(alexaEnabled)
    setBrightness(adjb.getOffset());
}

void MyAlexa::setBrightness(uint8_t bri) {
  if(alexaEnabled)
    espalexaDevice->setValue(map(bri,0,100,0,255));
}

void MyAlexa::setColor(void) {
  if(alexaEnabled)
    setColor(myspiffs.getRGBSetting(F("rgb")));
}

void MyAlexa::setColor(COLOR_T c) {
  if(alexaEnabled)
    espalexaDevice->setColor((c>>16)&0xff,(c>>8)&0xff,c&0xff);
}



//our callback functions
void alexaCallback(EspalexaDevice* dev)
{
  COLOR_T color;
  char cs[10];
  uint8_t bri;
  uint16_t ct;

  EspalexaDeviceProperty m = dev->getLastChangedProperty();
  lastValue=255;

  switch(m) {
    case EspalexaDeviceProperty::none:    // 0
      break;
    case EspalexaDeviceProperty::on:      // 1
      lastValue=1;
      myspiffs.setSetting(F("power"),"on");
      display.setBrightness(0);
      lastMinutes = -1;
      break;

    case EspalexaDeviceProperty::off:     // 2
      lastValue=0;
      myspiffs.setSetting(F("power"),"off");
      display.setBrightness(0);
      lastMinutes = -1;
      break;

    case EspalexaDeviceProperty::bri:     // 3
      bri = dev->getValue();
      lastValue=bri;
      if(bri <= 5) {
        display.setBrightness(0);
        bri=adjb.getOffset();
        Serial.printf("alexaCallback: bri1=%d\n",bri);
        bri=map(bri,0,100,0,255);
        Serial.printf("alexaCallback: bri2=%d\n",bri);
        //  alexa.setValue(bri);
      }
      else
        display.setBrightness(map(bri,0,255,0,100));
      lastMinutes = -1;
      break;
 
    case EspalexaDeviceProperty::ct:      // 5
      ct = dev->getCt();
      if (!ct) break;
      color = colorCTtoRGB(ct);
      lastValue=color;
      sprintf(cs,"#%.6x",color);
      //myspiffs.setSetting(F("rgb"),cs);
      display.setColor(color);
      lastMinutes = -1;
      break;
 
    case EspalexaDeviceProperty::hs:      // 4
    case EspalexaDeviceProperty::xy:      // 6
      color = espalexaDevice->getRGB();
      lastValue=color;
      if(color == 0xffffff)
      {
        myspiffs.writeLog(F("color == 0xffffff\n"));
        color=myspiffs.getRGBSetting(F("rgb"));
        myspiffs.writeLog(F("rgb="));
        myspiffs.writeLog(color, false);
        myspiffs.writeLog(F("\n"), false);
        alexa.setColor(color);
      } else {
        sprintf(cs,"#%.6x",color);
      }
      display.setColor(color);
      lastMinutes = -1;
      break;
    default:
      Serial.println(F("alexaCallback: unknown EspalexaDeviceProperty\n"));
      break;
  }

  sprintf(lastSetting,"alexaCallback %d: %s %#.6x\n", m, lastValue==1?"on":lastValue==0?"off":"",lastValue);
  myspiffs.writeLog(lastSetting);
  DEBUG_PRINTLN(lastSetting);
}


COLOR_T colorCTtoRGB(uint16_t mired)   //white spectrum to rgb, bins
{
  //this is only an approximation using WS2812B with gamma correction enabled
  byte rgb[3];
  
  if (mired > 475) {
    rgb[0]=255;rgb[1]=199;rgb[2]=92;//500
  } else if (mired > 425) {
    rgb[0]=255;rgb[1]=213;rgb[2]=118;//450
  } else if (mired > 375) {
    rgb[0]=255;rgb[1]=216;rgb[2]=118;//400
  } else if (mired > 325) {
    rgb[0]=255;rgb[1]=234;rgb[2]=140;//350
  } else if (mired > 275) {
    rgb[0]=255;rgb[1]=243;rgb[2]=160;//300
  } else if (mired > 225) {
    rgb[0]=250;rgb[1]=255;rgb[2]=188;//250
  } else if (mired > 175) {
    rgb[0]=247;rgb[1]=255;rgb[2]=215;//200
  } else {
    rgb[0]=237;rgb[1]=255;rgb[2]=239;//150
  }
  return (display.getColor(rgb[0],rgb[1],rgb[2]));
}


String MyAlexa::getState(void) {
  const static char ae[] PROGMEM = "\n\nalexaEnabled=";
  const static char lc[] PROGMEM = "\nLast changed property=";
  Helper::writeState(ae, alexaEnabled);
  Helper::writeState(lc, lastSetting);

  return "";
}

#endif
