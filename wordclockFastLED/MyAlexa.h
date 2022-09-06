/**
 * MyAlexa.h
 * Class integration with Alexa
 *
 * @autor    B. Merz
 * @version  1.0
 * @created  07.07.2022
 *
 */

#pragma once

#ifdef WITH_ALEXA

#define ESPALEXA_ASYNC
#define ESPALEXA_NO_SUBPAGE
#define  ESPALEXA_MAXDEVICES  1

#ifdef myDEBUG
//#define ESPALEXA_DEBUG
#endif

#include "EspalexaDevice.h"
#include "Espalexa.h"
#include "MyDisplay.h"

class MyAlexa {
  public:
    MyAlexa(void);
    void    initAlexa(void);
    void    loopAlexa(void);
    String  getState(void);
    void    setState(boolean state=true);
    void    setBrightness(void);
    void    setBrightness(uint8_t bri);
    void    setColor(void);
    void    setColor(COLOR_T c);
    
    boolean alexaEnabled;
    Espalexa *espalexa;

  private:
    
};

#endif
