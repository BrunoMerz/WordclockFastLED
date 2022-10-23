/**
   Configuration.h

   @autor    Bruno Merz
   @version  1.0
   @created  30.12.2019

   Version history:
   V 1.0:  - Created
   V 3.3:  - New Boards added
*/

#pragma once

#include <Arduino.h>

#define FIRMWARE  "V7.1"


/**
   different languages
*/
#define SPRACHE_DE
#define SPRACHE_CH
#define SPRACHE_EN
#define SPRACHE_FR
#define SPRACHE_IT
#define SPRACHE_NL
#define SPRACHE_ES
#define SPRACHE_PT
#define SPRACHE_GR
#define SPRACHE_AE


#define WITH_ALEXA
#define WITH_MQTT
//#define WITH_MAIL
//#define WITH_AUDIO
//#define WITH_RTC
#define WITH_LICENSE

#if defined(WITH_ALEXA)
#define with_alexa "Display:show"
#else
#define with_alexa "Display:none"
#endif

#if defined(WITH_MQTT)
#define with_mqtt "Display:show"
#else
#define with_mqtt "Display:none"
#endif

#define LED_LIB 1     // FASTLED
//#define LED_LIB 2   // CLED

#if (LED_LIB == 1)
enum MatrixType_t { HORIZONTAL_MATRIX,
                    VERTICAL_MATRIX,
                    HORIZONTAL_ZIGZAG_MATRIX,
                    VERTICAL_ZIGZAG_MATRIX };
#elif (LED_LIB == 2)
#include <LEDMatrix.h>
#include <LEDSprites.h>
#endif
                    

#define LED_COLS  11
#define LED_ROWS  10
#define LED_STRIPE_COUNT  LED_ROWS*LED_COLS+5
#define STRIP_X_DIRECTION   -1   // Richtung des Bandes von der Ecke: 1 - links->rechts, -1 rechts->links
#define STRIP_Y_DIRECTION   1    // Richtung des Bandes von der Ecke: 1 - unten->oben,   -1 oben->unten
#define MATRIX_TYPE         VERTICAL_MATRIX

#if defined(ESP32)
#define LICENSE_PIN 0
#else
#define LICENSE_PIN D0
#define RESET_PIN A0
#endif

#ifdef WITH_AUDIO
#define PinD3           // Data PIN of LED stripe D3  Wortuhr/do?datapin=0
#else
//#define PinD3           // Data PIN of LED stripe D3  Wortuhr/do?datapin=0
#define PinD4           // Data PIN of LED stripe D4  Wortuhr/do?datapin=2
#endif

#ifdef WITH_LICENSE
#define LIC_TAG "LIC, "
#else
#define LIC_TAG ""
#endif

//#define DCF77_SENSOR_EXISTS

#ifdef DCF77_SENSOR_EXISTS
#define PIN_DCF77_SIGNAL 0  // D3
#define PIN_DCF77_PON 1     // A0

#define MYDCF77_TELEGRAMMLAENGE 59
#define MYDCF77_SIGNAL_BINS 11
#define MYDCF77_DRIFT_CORRECTION_TIME 30

#define DCF77HELPER_MAX_SAMPLES 3

#endif

// ***************************************************************************
// Global variables / definitions
// ***************************************************************************
#define DBG_OUTPUT_PORT     Serial              // Set debug output port
#define CONFIG_PORTAL_SSID  F("Wortuhr")        // SSID if access point mode
#define JSON_CONFIG         F("/Wortuhr.json")  // Name of LittleFS JSON filename
#define JSON_RESOURCE       F("/resource.json") // Name of LittleFS JSON resource filename
#define JSON_UPDATE         F("/Update.json")   // Name of LittleFS JSON filename
#define INDEX_HTML          F("/index.html")    // index.html
#define INFO_FILE           F("/info.txt")      // Info Text für die Installation
#define LOG_FILE            F("/logfile.log")   // Name of logfile
#define LOG_FILE_OLD        F("/logfile1.log")   // Name of old logfile


#define SERIAL_SPEED        115200
//#define LED_STRIPE_COUNT    114         // 174 LEDs = 114 Uhrzeit, 1 Sekunden Blinker 59 - Sekundenanzeige


#if defined(ARDUINO_ESP8266_NODEMCU)
#define COMPILED_FOR                    "ESP-12E NodeMCU, D5"
#define LED_STRIPE_DATA_PIN 14          // Data PIN of LED stripe D5 - ESP8266
#define LED_PIN 2                       // buildin LED ESP8266
#define ESP_PIN_1 1                     // Tx
#define ESP_PIN_3 3                     // Rx
#endif                                  // ARDUINO_ESP8266_NODEMCU


#if defined(ESP32)
#define COMPILED_FOR                    "ESP-32, GPIO05"
#define LED_STRIPE_DATA_PIN 5           // Data PIN of LED stripe GPIO05
#define LED_PIN 2                       // buildin LED ESP32
#define ESP_PIN_1 1                     // Tx
#define ESP_PIN_3 3                     // Rx
#define WIFI_RESET 15
#endif                                  // ESP32

#if defined(ARDUINO_ESP8266_GENERIC)
#define COMPILED_FOR                    "ESP8266 GENERIC, GPIO2"
#define  LED_STRIPE_DATA_PIN 2          // Data PIN of LED stripe GPIO2
//#define LED_PIN 0                     // bei ESP-01S there is no usable buildin LED. We could connect one at GPIO0
#define ESP_PIN_1 1                     // Tx
#define ESP_PIN_3 3                     // Rx
//#define ESP_TX_RX_AS_GPIO_PINS          // ------------------ TX/RX as GIOP pins -------------------

#endif                                  // ARDUINO_ESP8266_GENERIC




#if defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO) 
#if defined(PinD3)
#define LED_STRIPE_DATA_PIN D3
#define COMPILED_FOR                    "WEMOS D1 Mini Pro, D3, " LIC_TAG
#else
#define COMPILED_FOR                    "WEMOS D1 Mini Pro, D4, " LIC_TAG
#define LED_STRIPE_DATA_PIN D4
#endif
#define LED_PIN D4                      // LED_BUILTIN D4
#define ESP_PIN_1 TX                    // Tx
#define ESP_PIN_3 RX                    // Rx
#endif                                  // ARDUINO_ESP8266_WEMOS_D1MINIPRO

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#if defined(PinD3)
#define LED_STRIPE_DATA_PIN D3
#define COMPILED_FOR                    "WEMOS D1 R2 & mini, D3, " LIC_TAG
#else
#define COMPILED_FOR                    "WEMOS D1 R2 & mini, D4, " LIC_TAG
#define LED_STRIPE_DATA_PIN D4 
#endif

#define LED_PIN D4                      // LED_BUILTIN D4
#define ESP_PIN_1 TX                    // Tx
#define ESP_PIN_3 RX                    // Rx
#define WIFI_RESET D8
#endif                                  // ARDUINO_ESP8266_WEMOS_D1MINI

typedef struct mytm
{
  uint8_t  tm_sec;
  uint8_t  tm_min;
  uint8_t  tm_hour;
  uint8_t  tm_mday;
  uint8_t  tm_mon;
  uint16_t tm_year;
  uint8_t  tm_wday;
  uint8_t  tm_yday;
  uint8_t  tm_isdst;
  int8_t   tm_tzoffset;
  double   tm_lon;
  double   tm_lat;
  String   tm_timezone;
  String   tm_ntpserver;
} MYTM;
