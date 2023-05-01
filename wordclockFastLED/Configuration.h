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

#define FIRMWARE  "V7.5"


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
#define SEC_STRIPE_COUNT  59
#define STRIP_X_DIRECTION   -1   // Richtung des Bandes von der Ecke: 1 - links->rechts, -1 rechts->links
#define STRIP_Y_DIRECTION   1    // Richtung des Bandes von der Ecke: 1 - unten->oben,   -1 oben->unten
#define MATRIX_TYPE         VERTICAL_MATRIX


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


#if defined(ARDUINO_ESP8266_NODEMCU)
#define COMPILED_FOR                    "ESP-12E NodeMCU, D5"
#define LED_STRIPE_DATA_PIN 14          // Data PIN of LED stripe D5 - ESP8266
#endif                                  // ARDUINO_ESP8266_NODEMCU


#if defined(WEMOS_D1_MINI32)
#define COMPILED_FOR                    "ESP-32, GPIO05"
#define LED_STRIPE_DATA_PIN 14          // Data PIN of LED stripe D5 - ESP8266
#define SEC_STRIPE_DATA_PIN 5           // Data PIN of LED stripe GPIO05
#define WIFI_RESET 15                   // Reset WiFi settings
#define LICENSE_PIN 0                   // Generate License Key
#define INPUT_PULLDOWN_16  INPUT_PULLDOWN
#endif                                  // ESP32


#if defined(ARDUINO_ESP8266_GENERIC)
#define COMPILED_FOR                    "ESP8266 GENERIC, GPIO2"
#define  LED_STRIPE_DATA_PIN 2          // Data PIN of LED stripe GPIO2
#endif                                  // ARDUINO_ESP8266_GENERIC


#if defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO) 
#define COMPILED_FOR                    "WEMOS D1 Mini Pro, D4, " LIC_TAG
#define LED_STRIPE_DATA_PIN D4 
#define SEC_STRIPE_DATA_PIN D3          // Data PIN of SEC stripe D3 - ESP8266
#define WIFI_RESET D8                   // Reset WiFi settings
#define LICENSE_PIN D0                  // Generate License Key
#endif                                  // ARDUINO_ESP8266_WEMOS_D1MINIPRO


#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#define COMPILED_FOR                    "WEMOS D1 R2 & mini, D4, " LIC_TAG
#define LED_STRIPE_DATA_PIN D4 
#define SEC_STRIPE_DATA_PIN D3          // Data PIN of SEC stripe D3 - ESP8266
#define WIFI_RESET D8                   // Reset WiFi settings
#define LICENSE_PIN D0                  // Generate License Key
#endif                                  // ARDUINO_ESP8266_WEMOS_D1MINI


#if defined(LILYGO_T_HMI)
#define COMPILED_FOR                    "LILYGO T-HMI"
#define LED_STRIPE_DATA_PIN 15          // Data PIN of LED stripe IO15
#define SEC_STRIPE_DATA_PIN 16          // Data PIN of SEC stripe Io16
#define WIFI_RESET 17
#define LICENSE_PIN 18
#define INPUT_PULLDOWN_16  INPUT_PULLDOWN
#define PWR_EN_PIN  (10)
#define PWR_ON_PIN  (14)
#define BAT_ADC_PIN (5)
#define BUTTON1_PIN (0)
#define BUTTON2_PIN (21)

// lcd
#define LCD_DATA0_PIN (48)
#define LCD_DATA1_PIN (47)
#define LCD_DATA2_PIN (39)
#define LCD_DATA3_PIN (40)
#define LCD_DATA4_PIN (41)
#define LCD_DATA5_PIN (42)
#define LCD_DATA6_PIN (45)
#define LCD_DATA7_PIN (46)
#define PCLK_PIN      (8)
#define CS_PIN        (6)
#define DC_PIN        (7)
#define RST_PIN       (-1)
#define BK_LIGHT_PIN  (38)

// touch screen
#define TOUCHSCREEN_SCLK_PIN (1)
#define TOUCHSCREEN_MISO_PIN (4)
#define TOUCHSCREEN_MOSI_PIN (3)
#define TOUCHSCREEN_CS_PIN   (2)
#define TOUCHSCREEN_IRQ_PIN  (9)

// sd card
#define SD_MISO_PIN (13)
#define SD_MOSI_PIN (11)
#define SD_SCLK_PIN (12)

#define SDIO_DATA0_PIN (13)
#define SDIO_CMD_PIN   (11)
#define SDIO_SCLK_PIN  (12)


#define STD_FONT 4

#endif


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

#define BYTE  uint8_t
#define WORD  uint16_t
#define DWORD uint32_t
#define LONG  uint32_t

struct ICOHEADER {
   WORD wReserved;  // Always 0
   WORD wResID;     // Always 1
   WORD wNumImages; // Number of icon images/directory entries
};


struct ICONDIRENTRY {
    BYTE  bWidth;    // Breite des Bildes
    BYTE  bHeight;    // Die doppelte Höhe des Bildes in Pixeln.
    BYTE  bCount;    // Anzahl der Farben im Bild (2 oder 16, bzw. 0 falls mehr als 
    BYTE  bReserved;    // 256 Farben). immer 0
    WORD  wPlanes;    // Anzahl der Farbebenen in der Regel 1 u. 0
    WORD  wBitCount;      // Anzahl der Bits pro Pixel( Anzahl der Farben) 1= Schwarz-
        // Weiß = 8 Byte, 4 = 16 Farben = 64 Byte, 
        // 8 = 256 Farben = 1024 Byte, 24 = 16 Milion
        // Farben (keine Farbtabelle) 
    DWORD dwBytesInRes;   // Bildgröße in Bytes ab diesem Record. Das bedeutet 
                          // BITMAPINFOHEADER, Farbtabelle, XOR- und 
                          // AND-Bild zusammen gerechnet.
    DWORD dwImageOffset;  // Offset ab wo der BITMAPINFOHEADER beginnt.
};


struct BITMAPINFOHEADER {
    DWORD   biSize;      // Länge des Info-Headers(dieser Record) = 40 Byte in Hex 28
    LONG    biWidth;      // Breite des Bildes 
    LONG    biHeight;     // Höhe des Bildes 
    WORD    biPlanes;     // Anzahl der Farbebenen in der Regel 1 u. 0
    WORD    biBitCount;   // Anzahl der Bits pro Pixel( Anzahl der Farben) 1= Schwartz-
        // Weiß = 8 Byte, 4 = 16 Farben = 64 Byte, 
        // 8 = 256 Farben = 1024 Byte, 24 = 16 Milion 
        // Farben (keine Farbtabelle)  
    DWORD   biCompression;  // Komprimierungstyp, 0 = Unkomprimiert, 1 = 8-Bit 
          // RLE =Run-Length-Encording-Verfahren, 2 = 4-Bit Run-Encording-
          // Verfahren
    DWORD   biSizeImage;    // Bildgröße ohne Farbtabelle. Aber XOR und AND Bild zusammen 
             // gerechnet bei Icon u. Cursor.
    DWORD   biXPelsPerMeter;   // Horizontale Auflösung
    DWORD   biYPelsPerMeter;   // Vertikale Auflösung
    DWORD   biClrUsed;         // Die Zahl der im Bild vorkommenden Farben zB. Bei 256 Farben
             //  müssen es nicht unbedingt 256 sein, es könne auch 
             // zB. 206 sein.
    DWORD   biClrImportant;    // die Anzahl der wichtigen Farben
} ;

struct ICON {
  BYTE  b;  // blue
  BYTE  g;  // green
  BYTE  r;  // red
  BYTE  a;  // alpha
};
