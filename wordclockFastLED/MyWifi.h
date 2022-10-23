/**
   MyWifi.h
   @autor    Bruno Merz

   @version  2.0
   @created  30.12.2019
   @updated  31.03.2021

*/

#pragma once

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"
#endif




#include "MySpiffs.h"
#include "MyDisplay.h"
#include "IconRenderer.h"

#define WIFITIMEOUT 180
#define WPSTIMEOUT  30

class ESPAsync_WiFiManager;

class MyWifi {
  public:
    MyWifi(void);
    void init(void);
    static void saveConfigCallback (void);
    static void configModeCallback (ESPAsync_WiFiManager *myWiFiManager);
    bool myStartWPS(void);
    void getWifiParams(String ssid, String pass);
    String getState(void);
    bool isConnected(void);
    void doReset(void);

  private:
    bool   _do_reset;
};
