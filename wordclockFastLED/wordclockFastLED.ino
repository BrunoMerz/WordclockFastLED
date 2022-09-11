/**
   wordclockFastLED.ino

   @autor    Bruno Merz
   @version  7.0
   @created  30.12.2019
   @updated  18.03.2022
   
   Used Libraries:
   ===============
    
    ArduinoJson           6.19.4
    PubSubClient          2.8.0
    ESP8266WiFi           1.0 
    ESPAsyncTCP           1.2.2
    ESPAsyncUDP           1.1.0  
    ESPAsync_WiFiManager  1.13.0
    ESPAsyncDNSServer     1.0.0
    sunset                1.1.6
    ESP32-targz           1.1.2
    FastLED               3.5.0


   For ESP-12E:
   ============
   Board:         esp8266 by esp8266 comunity Version 2.7.4
                  NodeMCU 1.0 (ESP-12E Module)
   CPU Frequency: 80 Mhz
   Flash Size:    4MB (FS:2MB OTA:~1019KB))
   IwIP Variant:  v2 Higher Bandwidth

 
   For ESP32:
   ==========
   Board:         esp32 by Expressif Systems Version 1.0.6
                  ESP32 Dev Module
   CPU Frequency: 160/240 Mhz
   Partition Schema:  4 MB with spiffs (1.2MB APP/ 1.5MB LittleFS)

   Problem Multiple definitions in:

   .\Arduino-Projekte\libraries\ESPAsync_WiFiManager\src
   Delete the all the files in src directory.
   Copy all the files in src_cpp directory into src.


   For ESP-01S:
   ============
   Board:         esp8266 by esp8266 comunity Version 3.0.2
                  Generic ESP8266 Module
   CPU Frequency: 80 Mhz
   Flash Mode:    DOUT (compatible)
   Reset Method:  dtr (aka nodemcu)
   IwIP Variant:  v2 Low Memory
   Partition Schema:  1MB (FS:64KB OTA:~470KB)


   WEMOS D1 mini Pro:
   ==================
   Board:         esp8266 by esp8266 comunity Version 3.0.2
                  LOLIN(WEMOS) D1 mini Pro
   CPU Frequency: 80 Mhz
   IwIP Variant:  v2 Low Memory
   Partition Schema:  16MB (FS:14MB OTA:~1019KB)

*/

const char compile_date[] = __DATE__ " " __TIME__;

#include "Configuration.h"

//#define myDEBUG
#include "Debug.h"

#if defined(myDEBUG)
#define DEBUG_UPDATER Serial
#endif


#if defined(ESP8266)
#include <avr/pgmspace.h>
#include <ESP8266WiFi.h>
#include <cont.h>
#else
#include <WiFi.h>
#include "esp_wps.h"
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <Update.h>
#endif

#define DEST_FS_USES_LITTLEFS
#include <ESP32-targz.h>


#include <string.h>
#include <FS.h>


#include "MyWifi.h"
#include "MySpiffs.h"

#include "Renderer.h"

#include "MyDisplay.h"
#include "Ticker.h"
#include "Effekte.h"
#include "IconRenderer.h"
#include "TimeStamp.h"
#include "AdjustBrightness.h"
#include "Helper.h"
#include "MyTime.h"
#include "MyAlexa.h"

#ifdef DCF77_SENSOR_EXISTS
  #include "MyDCF77.h"
  #include "DCF77Helper.h"

  MyDCF77 dcf77(PIN_DCF77_SIGNAL, PIN_DCF77_PON);
  DCF77Helper dcf77Helper;
#endif

#if defined(WORDCLOCK_DEBUG)
aa bb
#endif

/*
   Handling webserver
*/
 
#include "ESPAsyncWebServer.h"
#include "ESPAsyncDNSServer.h"

AsyncWebServer server(80);
AsyncDNSServer dns1;

/*
 *  Handling Alexa
 */
#ifdef WITH_ALEXA
MyAlexa alexa;
#endif

/*
 *  Handling adjust brightness regarding time
 */
AdjustBrightness adjb;


/*
   Handling persistence of settings and html/other files
*/
MySpiffs  myspiffs;


/**
   MyMqtt; for homeautomation
*/
#if defined(WITH_MQTT)
#include "MyMqtt.h"
MyMqtt mymqtt;
#endif

/**
   Audio integration
*/
#if defined(WITH_AUDIO)
#include "MyAudio.h"
MyAudio myaudio;
#endif


/**
   MyMail; for receive/send emails
*/
#if defined(WITH_MAIL)
#include "MyMail.h"
MyMail mymail;
#endif


/*
   WiFi Handling
*/
MyWifi    mywifi;

/*
   Time Handling
*/
MyTime    mytime;

// processor declaration
String processor(const String& var);

// Die Matrix, eine Art Bildschirmspeicher
word matrix[16];
unsigned long lastBrightnessCheck;

MYTM  mytm;

TimeStamp night_on;
TimeStamp night_off;

#define bufsize 500

String  timeStamp;
String  ssid;
String  ip;
String  bodyData;
size_t uploadLength=0;
String uploadFilename="";
bool   uploadStarted=false;
int    displayEffekt=0;
int    displayTicker=0;
String iconFile="";
bool  reboot=false;
byte  reason=0;
bool  timezoneChanged=false;
char  textPlain[] = "text/plain";
int   brightness_night=0;
bool  shutdown;
bool  sec_flash_enabled=false;
int   seconds_stripe=0;
int   reset_pin=0;
byte progress_x;
int16_t x_pos_progress;
int16_t y_pos_progress;
bool nightOn=false;
bool  ap_mode = false;

uint8_t LedStripeDataPin = LED_STRIPE_DATA_PIN;

/*
   LED Driver for WS2812B stripes
*/
MyDisplay display;



/**
   Renderer, outputs Matrix to LEDs.
*/
Renderer renderer;


/**
   ticker, output text as a ticker to LEDs
*/
Ticker ticker;


/**
   effekte, outputs nice pattern on the hour
*/
Effekte effekte;


/**
   IconRenderer, renders icon files stored in LittleFS and displays these
*/
IconRenderer iconRenderer;

int lastMinutes = -1;
int lastSecond  = -1;

#ifdef WITH_LICENSE
/**
 * checkLicense
 */
void checkLicense(void) {
  byte mac[6];
  byte seq[6]={1,4,0,5,3,2};
  byte newMac[6];
  char license[20];

  WiFi.macAddress(mac);
  DEBUG_PRINTLN(WiFi.localIP().toString());
  DEBUG_PRINTLN(WiFi.macAddress());
  for(int i=0;i<6;i++) {
    int j = mac[i];
    int k = mac[i];
    j <<= 4;
    j &=  0xf0;
    k >>= 4;
    k &=  0x0f;
    j |= k;
    j += ((i+1)*17);
    newMac[seq[i]] = j;
  }

  sprintf(license,"%.2x%.2x%.2x-%.2x%.2x%.2x",newMac[0],newMac[1],newMac[2],newMac[3],newMac[4],newMac[5]);
  boolean lp = digitalRead(LICENSE_PIN);
  DEBUG_PRINTF("License pin=%d, value=%d\n",LICENSE_PIN, lp);
  if(lp || !myspiffs.exists("index.html") || myspiffs.getIntSetting(F("GL"))) {
    myspiffs.setSetting(F("license"),String(license));
    myspiffs.removeSetting(F("GL"));
    myspiffs.writeSettings(true);
   } else {
    String License = myspiffs.getSetting(F("license"));
    DEBUG_PRINTLN("License="+License+", GEN License="+license);
    if(License != String(license)) {
      Serial.println(F("License not found"));
      server.begin();
      ticker.render(1, 140, myspiffs.getResource(F("license"), F("Lizenz nicht gefunden")));
      iconRenderer.renderAndDisplay(F("/stopp.ico"),0,1);
      //LittleFS.end();
      while(true) {
        delay(1000);
        yield();
      }
    }
  }
}
#endif




void handle_notfound(AsyncWebServerRequest *request) {
  String msg = request->url();
  String mime_type;

#ifdef WITH_ALEXA
/*
  if (request->hasParam("body", true)) // This is necessary, otherwise ESP crashes if there is no body
  {
    String body = request->getParam("body", true)->value();
    Serial.println("body="+body);
  }
  Serial.println("url="+request->url());
*/
  if (uploadStarted || !alexa.alexaEnabled || !alexa.espalexa->handleAlexaApiCall(request)) //if you don't know the URI, ask espalexa whether it is an Alexa control request
  {
#endif
    DEBUG_PRINTLN("handle_notfound: "+msg+" not found");
    if(myspiffs.exists(request->url())) {
      if(msg.endsWith(F(".htm")) || msg.endsWith(F(".html")) || msg.endsWith(F(".log")))
        request->send(LittleFS, msg, String(), false, processor);
      else {
        if(msg.endsWith(F(".jpg")) || msg.endsWith(F(".jpeg")))
            mime_type = F("image/jpeg");
        else if(msg.endsWith(".png"))
          mime_type = F("image/png");
        else if(msg.endsWith(".ico"))
          mime_type = F("image/x-icon");
        else if(msg.endsWith(".bmp"))
          mime_type = F("image/bmp");
        else if(msg.endsWith(".gif"))
          mime_type = F("image/gif");
        else if(msg.endsWith(".css"))
          mime_type = F("text/css");
        else if(msg.endsWith(".pdf"))
          mime_type = F("application/pdf");
        else if(msg.endsWith(".txt"))
          mime_type = F("text/plain");
        else if(msg.endsWith(".json"))
          mime_type = F("application/json");
        else if(msg.endsWith(".mp3"))
          mime_type = F("audio/mpeg");
        else
          mime_type = F("application/octet-stream");
  
        DEBUG_PRINTLN("handle_notfound: mime_type="+mime_type+", file="+msg);
        request->send(LittleFS, msg, mime_type);

      }
    } else {
#ifdef WITH_MQTT
      mymqtt.publishLog((char *)msg.c_str());
#endif
      request->send(404, textPlain, msg.c_str()); 
    }
#ifdef WITH_ALEXA
  }
#endif
}


void handle_state(AsyncWebServerRequest *request) {
  const static char hn[] PROGMEM = "Hostname: ";
  const static char cmds[] PROGMEM = "\n\nCommands: state, format, reboot, show_icon, reset_wifi, mqttenable, mqttdisable";
  const static char ipt[] PROGMEM = "\nIP: ";
  const static char mac[] PROGMEM = "\nMAC: ";
  const static char dp[] PROGMEM = "\nLED-Pin: ";
  const static char brightness[] PROGMEM = "\nBrightness: ";
  const static char sunrise[] PROGMEM = "\nSunrise: ";
  const static char sunset[] PROGMEM = "\nSunset: ";
  const static char firmware[] PROGMEM = "\nFirmware: ";
  const static char boottime[] PROGMEM = "\nBoot Time: ";
  const static char currenttime[] PROGMEM = "\nCurrent Time: ";
  const static char compiledfor[] PROGMEM = "\nCompiled for: " COMPILED_FOR " ";
  const static char ss[] PROGMEM = "\n\nSketchSize: ";
  const static char fss[] PROGMEM = "\nFreeSketchSpace: ";
  const static char fh[] PROGMEM = "\nFree Heap: ";
  const static char hf[] PROGMEM = "\nHeapFragmentation: ";
  const static char mfbs[] PROGMEM = "\nMaxFreeBlockSize: ";
  const static char fs[] PROGMEM = "\nFree stack size: ";
  const static char fp[] PROGMEM = "\nFree Psram: ";
  const static char ma[] PROGMEM = "\nMaxAllocHeap: ";
  const static char rs[] PROGMEM = "\nReset Pin: ";

  myspiffs.openFile("/state.txt", "w");

  Helper::writeState(hn, ssid);
  Helper::writeState(ipt, ip);
  Helper::writeState(mac, WiFi.macAddress());
  Helper::writeState(dp, LedStripeDataPin);
  Helper::writeState(brightness, display.getBrightness());
  Helper::writeState(sunrise, adjb.getSunrise());
  Helper::writeState(sunset, adjb.getSunset());
  Helper::writeState(firmware, FIRMWARE);
  Helper::writeState(boottime, timeStamp);
  Helper::writeState(currenttime, mytime.getTime());
  Helper::writeState(compiledfor, compile_date);
  
  mywifi.getState();
  myspiffs.getState();
  ticker.getState();

#ifdef WITH_MQTT
  mymqtt.getState();
#endif

#ifdef WITH_ALEXA
  alexa.getState();
#endif

  Helper::writeState(cmds);

#if defined(ESP32)
  Helper::writeState(ss,  ESP.getSketchSize());
  Helper::writeState(fss, ESP.getFreeSketchSpace());
  Helper::writeState(fh,  ESP.getHeapSize());
  Helper::writeState(fp,  ESP.getFreePsram());
  Helper::writeState(ma,  ESP.getMaxAllocHeap());
#else
  Helper::writeState(ss,  ESP.getSketchSize());
  Helper::writeState(fss, ESP.getFreeSketchSpace());
  Helper::writeState(fh,  ESP.getFreeHeap());
  Helper::writeState(hf,  ESP.getHeapFragmentation());
  Helper::writeState(mfbs,ESP.getMaxFreeBlockSize());
  Helper::writeState(fs,  cont_get_free_stack(g_pcont));
  Helper::writeState(rs,  reset_pin);
#endif

  myspiffs.closeFile();
  request->send(LittleFS, "/state.txt", String(), false, NULL);
}


void handle_FileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    myspiffs.cleanup();
    uploadStarted = true;
    DEBUG_PRINTF("handle_FileUpload: start %s\n",filename.c_str());
    uploadLength=0;
    if (!filename.startsWith("/")) filename = "/" + filename;
    //DEBUG_PRINT(F("handle_FileUpload Name: "));
    //DEBUG_PRINTLN(filename);
    if(filename.length() > 1) {
      myspiffs.openFile(filename, "w");
      if(filename.equals("/resource.json"))
        myspiffs.reloadResource();
    }
  }
  uint32_t freeSpace = myspiffs.getFreeSpace();
  //DEBUG_PRINTF("freeSpace=%d, need=%d\n",freeSpace, len);
  if(freeSpace > len) {
    myspiffs.writeFile(data, len);
  }
  else {
    DEBUG_PRINTLN("Upload failed, not enough space");
    myspiffs.closeFile();
    myspiffs.remove(filename);
    String msg =F("Datei zu gross, nur noch ");
    msg += myspiffs.getFreeSpace();
    msg += F(" Bytes frei");
    request->send(404, textPlain, msg);
  }
  uploadLength+=len;
  if(final){
    DEBUG_PRINTF("handle_FileUpload: end %s, %d\n",filename.c_str(), uploadLength);
    myspiffs.closeFile();
    request->redirect("/expert.html");
    if(filename.endsWith(".bin") || filename.endsWith(".tar"))
      uploadFilename="/"+filename;
    uploadStarted = false;
  }
}


bool updateFW() {
  bool ret = false;
  uint8_t *data=(uint8_t *)malloc(bufsize + 12);
  String toDelete;
  size_t l;
  int sum = 0;
  unsigned long ltm1 = millis();
  progress_x = 0;
  x_pos_progress=(LED_COLS-11)/2; //(32-11)/2=10
  y_pos_progress=(LED_ROWS-10)/2; //(16-10)/2=3
  DEBUG_PRINTF("updateFW: fn=%s, x_pos=%d, y_pos=%d\n",uploadFilename.c_str(), x_pos_progress, y_pos_progress);
  iconRenderer.renderAndDisplay(F("/progress0.ico"),0,1,x_pos_progress,y_pos_progress);
  if (myspiffs.openFile(uploadFilename, "r")) {
    DEBUG_PRINTLN("updateFW Open ok:" + uploadFilename);
    toDelete = uploadFilename;
    if (!Update.begin(uploadLength)) {
      Update.printError(Serial);
    }
    do {
      l = myspiffs.readBytes(data, bufsize);
      sum += l;
      if (Update.write(data, l) != l) {
        Update.printError(Serial);
      } else {
        EVERY_N_MILLISECONDS(300){
          if (progress_x == 7) {
            for (progress_x = 0; progress_x < 7; progress_x++)
              display.clearPixel(x_pos_progress+2+progress_x,y_pos_progress+5);
            progress_x = 0;
          }
          display.drawPixel(x_pos_progress+2+progress_x++, y_pos_progress+5, 0x800080);
          display.show();
        }
      }
    } while (l == bufsize);

    myspiffs.closeFile();
    DEBUG_PRINTLN("updateFW remove:" + toDelete);
    myspiffs.remove(toDelete);

    if (sum == uploadLength)
      iconRenderer.renderAndDisplay(F("/hourglass.ico"),3000,3);
    else {
      iconRenderer.renderAndDisplay(F("/sum.ico"),3000,3);
      DEBUG_PRINTLN(F("updateFW sum != uploadLength"));
    }

    if (Update.end(true)) { //true to set the size to the current progress
      DEBUG_PRINTF("updateFW Update Success: %u\nRebooting...\n", sum);
      DEBUG_FLUSH();
      ret = true;
    } else {
      Update.printError(Serial);
    }
  } else {
    iconRenderer.renderAndDisplay(F("/uploadFailed.ico"),3000,3);
    DEBUG_PRINTLN("updateFW open file failed");
  }
  if(data)
    free(data);
  return ret;
}

#define SZ  80

static void myTarStatusProgressCallback( const char* name, size_t size, size_t total_unpacked ) {
  String filename(name);
  char buf[SZ];
  if (progress_x == 7) {
    for (progress_x = 0; progress_x < 7; progress_x++)
      display.clearPixel(x_pos_progress+2+progress_x,y_pos_progress+5);
    progress_x = 0;
  }
  display.drawPixel(x_pos_progress+2+progress_x++, y_pos_progress+5, 0x800080);
  display.show();
  int l = sprintf(buf, "<li>Datei: %s, Länge : %d Bytes</li>\n", name, size);
  myspiffs.writeFile((const uint8_t*)buf, l);

  if (filename.endsWith(".bin")) {
    uploadFilename = "/" + filename;
    uploadLength = size;
  }
}


bool expandTar() {
  bool ret = false;
  progress_x = 0;
  String tarFilename = uploadFilename;

  x_pos_progress=(LED_COLS-11)/2; //(32-11)/2=10
  y_pos_progress=(LED_ROWS-10)/2; //(16-10)/2=3
  DEBUG_PRINTF("updateFW: fn=%s, x_pos=%d, y_pos=%d\n",uploadFilename.c_str(), x_pos_progress, y_pos_progress);
  iconRenderer.renderAndDisplay(F("/progress0.ico"),0,1,x_pos_progress,y_pos_progress);

  uploadLength = 0;
  char buf[SZ];
  int l;

  myspiffs.openFile(String(F("/result.html")), "w");

  TarUnpacker *TARUnpacker = new TarUnpacker();
  TARUnpacker->setupFSCallbacks( targzTotalBytesFn, targzFreeBytesFn );
  TARUnpacker->setTarStatusProgressCallback( myTarStatusProgressCallback );
  if ( !TARUnpacker->tarExpander(tarGzFS, tarFilename.c_str(), tarGzFS, "/") ) {
    DEBUG_PRINTF("tarExpander failed with return code #%d\n", TARUnpacker->tarGzGetError() );
    String buf = F("<p style=\"color:red;font-size:24px;\">Zu wenig Speicherplatz, TAR Datei kann nicht entpackt werden!</p>");
    myspiffs.writeFile((char *)buf.c_str());
  }
  myspiffs.remove(tarFilename);
  File inf = LittleFS.open(INFO_FILE, "r");
  if (inf) {
    do {
      l = inf.readBytes(buf, SZ);
      myspiffs.writeFile((const uint8_t*)buf, l);
    } while (l == SZ);
    inf.flush();
    inf.close();
    myspiffs.remove(INFO_FILE);
  }

  myspiffs.closeFile();
  tarFilename = "";
  return ret;
}
void handle_getSettings(AsyncWebServerRequest *request) {
  DEBUG_PRINTLN(F("handle_getSettings"));
  String tmp=myspiffs.getSettings();
  request->send_P(200, textPlain, tmp.c_str()); // Send all setting values only to client ajax request
}


void handle_saveExpertSettings(AsyncWebServerRequest *request){
  DEBUG_PRINTLN("handle_saveExpertSettings: #params="+String(request->params()));
  for(int i=0;i<request->params();i++) {
    AsyncWebParameter* p = request->getParam(i);
    String value = p->value();
    String name = p->name();
    DEBUG_PRINTLN("handle_saveExpertSettings: name="+name+", value="+value);
    if(name == F("hostname") && value != myspiffs.getSetting(F("hostname"))) {
      reason=0;
      reboot = true;
    }
    if(name == F("time_zone") && value != mytm.tm_timezone) {
      mytm.tm_timezone = value;
      timezoneChanged = true;
    }
    if(name == F("ntp_server") && value != mytm.tm_ntpserver) {
      mytm.tm_ntpserver = value;
      timezoneChanged = true;
    }
     if(name == F("latitude") && value.toDouble() != mytm.tm_lat) {
      mytm.tm_lat = value.toDouble();
      timezoneChanged = true;
    }    
    if(name == F("longitude") && value.toDouble() != mytm.tm_lon) {
      mytm.tm_lon = value.toDouble();
      timezoneChanged = true;
    }
 
    myspiffs.setSetting(name, value);
    
  }
  myspiffs.writeSettings(true);
  lastMinutes = -1;
  request->redirect("/expert");
}


void handle_format(AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("handle_format called");
    myspiffs.format();
    DEBUG_PRINTLN("Dateisystem formattiert");
    myspiffs.writeSettings(true);
    request->send(200, textPlain, "Dateisystem formattiert");
}


void handle_resetWiFi(AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("reset_wifi called");
    DEBUG_FLUSH();
    myspiffs.removeSetting(F("wifi_ssid"));
    myspiffs.removeSetting(F("wifi_pass"));
    myspiffs.writeSettings(true);
    request->redirect("/");
    reason=1;
    reboot=true;
}


void handle_reboot(AsyncWebServerRequest *request) {
    request->redirect("/");
    reboot=true;
}


void handle_showIcon(AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("Got show_icon");
    if(request->params()>0) {
      iconFile = "/"+request->getParam(0)->name();
      DEBUG_PRINTLN(iconFile);
    }
    request->redirect("/");
}

void handle_seen(AsyncWebServerRequest *request) {
    DEBUG_PRINTLN("Got seen");
    myspiffs.remove("/result.html");
    request->redirect("/");
}

#ifdef WITH_MQTT
void handle_mqttEnable(AsyncWebServerRequest *request) {
    mymqtt.setEnabled(true);
    request->send(200, textPlain, "OK");
}

void handle_mqttDisable(AsyncWebServerRequest *request) {
    mymqtt.setEnabled(false);
    request->send(200, textPlain, "OK");
}
#endif

void handle_doArgs(AsyncWebServerRequest *request) {
  char    redir[20] ={"/"};
  String  message = F("Number of args received:");
          message += request->params();        //Get number of parameters
          message += F("\n");                     //Add a new line
  for (int i = 0; i < request->params(); i++) {
    AsyncWebParameter* p = request->getParam(i);
    String value = p->value();
    message += (String)"Arg nr=" + (String)i + (String)" -> ";  //Include the current iteration value
    message += p->name() + (String)": ";                //Get the name of the parameter
    message += value + "\n";                            //Get the value of the parameter
    if(p->name().equalsIgnoreCase(F("show"))) {
      if(!value.startsWith("/"))
        value="/"+value;
      DEBUG_PRINTLN("show="+value);
      iconFile=value;
    } else if(p->name().equalsIgnoreCase(F("remove"))) {
      if(!value.startsWith("/"))
        value="/"+value;
      DEBUG_PRINTLN("remove="+value);
      myspiffs.remove(value);
    } else if(p->name().equalsIgnoreCase(F("hostname"))) {
      DEBUG_PRINTLN("hostname="+value);
      if(value.length()) {
        if(value != ssid) {
          myspiffs.setSetting(F("hostname"), value);
          myspiffs.writeSettings(true);
        }
      }
    } else if(p->name().equalsIgnoreCase(F("effect"))) {
      DEBUG_PRINTLN("list="+value);
      displayEffekt=value.toInt();
      strcpy(redir, "effect.html");
      DEBUG_PRINTLN(message);
    } else if(p->name().equalsIgnoreCase(F("unset"))) {
      DEBUG_PRINTLN("unset="+value);
      if(value.length()) {
        myspiffs.removeSetting(value);
      }
    } else if(p->name().equalsIgnoreCase(F("ticker"))) {
      if(value.length()) {
        displayTicker=value.toInt();
        request->redirect("/ticker.html");
      }
    } else if(p->name().startsWith(F("set"))) {
      DEBUG_PRINTLN(p->name()+"="+value);
      if(value.length()) {
        myspiffs.setSetting(p->name().substring(3),value);
        myspiffs.writeSettings(true);
      }
    } else if(p->name().equalsIgnoreCase(F("reboot"))) {
      DEBUG_PRINTLN("reboot");
      ESP.restart();
    }
  }
  request->redirect(redir);
}

/*
void handle_onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  if(!index)
    bodyData="";

  for(int i=0; i<len; i++)
    bodyData += (char)*(data+i);

  if(index+len == total) {
    myspiffs.setSettings(bodyData);
    COLOR_T c = myspiffs.getRGBSetting(F("rgb"));
    display.setColor(c);
    display.setBrightness();

    String s = myspiffs.getSetting(F("nighton"));
    night_on.setHours(s.substring(0, 2).toInt());
    night_on.setMinutes(s.substring(3).toInt());

    s = myspiffs.getSetting(F("nightoff"));
    night_off.setHours(s.substring(0, 2).toInt());
    night_off.setMinutes(s.substring(3).toInt());

    sec_flash_enabled = myspiffs.getBoolSetting(F("sec_flash_enabled"));
    seconds_stripe = myspiffs.getIntSetting(F("seconds_stripe"));

    lastMinutes = -1;
    bodyData="";
  }
  request->send(200, textPlain, "OK");
}
*/

String getHostname(String *hn) {
  *hn=myspiffs.getSetting(F("hostname"));
  if(!hn->length())
    *hn = CONFIG_PORTAL_SSID;
  return *hn;
}

String processor(const String& var) {
  if(var == F("VERSION"))
    return "'"+ssid+"'"+" "+F(FIRMWARE);
  if(var == F("COMPILED"))
    return F(COMPILED_FOR);
  if(var == F("SUNRISE"))
    return adjb.getSunrise();
  if(var == F("SUNSET"))
    return adjb.getSunset();
  if(var == F("BRIGHTNESS"))
    return String(display.getBrightness());
  if(var == F("WITH_ALEXA"))
    return with_alexa;
  if(var == F("WITH_MQTT"))
    return with_mqtt;
  return myspiffs.getResource(var);

  return String();
}

void setup()
{
  char inser;

#ifdef ESP_TX_RX_AS_GPIO_PINS
//********** CHANGE PIN FUNCTION  TO GPIO **********
pinMode(ESP_PIN_1, FUNCTION_3); 
pinMode(ESP_PIN_3, FUNCTION_3); 
//**************************************************
#else
//********** CHANGE PIN FUNCTION  TO TX/RX **********
//pinMode(ESP_PIN_1, FUNCTION_0); 
//pinMode(ESP_PIN_3, FUNCTION_0); 
//***************************************************
#endif

#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
#endif

#ifndef ESP_TX_RX_AS_GPIO_PINS
  Serial.begin(SERIAL_SPEED);
  delay(100);
  Serial.println(F("\n\nWortuhr is initializing..."));
#endif

#ifdef WITH_LICENSE
  pinMode(LICENSE_PIN, INPUT_PULLDOWN_16);
#endif

  DEBUG_PRINTLN(F("... and starting in debug-mode..."));

#ifndef ESP_TX_RX_AS_GPIO_PINS
  Serial.flush();
#endif

  pinMode(D5,OUTPUT);        // D5
  digitalWrite(D5,0);
  pinMode(D6,INPUT_PULLUP);  // D6

  mywifi.init();
  
#ifdef DCF77_SENSOR_EXISTS
  pinMode(PIN_DCF77_PON, OUTPUT);
  // DCF77-Empfaenger ausschalten...
  dcf77.enable(false);
#endif

#if defined(ESP8266)
  if(!LittleFS.begin()) {
    DEBUG_PRINTLN(F("Failed to mount file system"));
  } else {
    FSInfo fs_info;
    LittleFS.info(fs_info);
    DEBUG_PRINTF("FSInfo: totalBytes=%d, usedBytes=%d, maxOpenFiles=%d, maxPathLength=%d\n", fs_info.totalBytes, fs_info.usedBytes, fs_info.maxOpenFiles, fs_info.maxPathLength);
  }
#else
  if(!LittleFS.begin(true)) {
    DEBUG_PRINTLN(F("Failed to mount file system"));
  } else {
    unsigned int totalBytes = LittleFS.totalBytes();
    unsigned int usedBytes = LittleFS.usedBytes();
    DEBUG_PRINTF("FSInfo: totalBytes=%d, usedBytes=%d\n", totalBytes, usedBytes);
  }
#endif

  // read all settings
  myspiffs.readSettings();

  ap_mode = myspiffs.getBoolSetting(F("ap_mode"));

  /*  seconds_stripe: Art der Sekundenanzeige 0-Keine Leds für Sekundenanzeige vorhanden
   *                                          1-Eine Led vorhanden
   *                                          2-59 Leds vorhanden, Anzeige immer nur eine Sekunde
   *                                          3-59 Leds vorhanden, Anzeige alle Leds nacheinander einschalten
   *                                          
   *  sec_flash_enabled: Sekundenanzeige ein/aus
   */
  seconds_stripe = myspiffs.getIntSetting(F("seconds_stripe"));
  sec_flash_enabled = myspiffs.getBoolSetting(F("sec_flash"));
  

#ifndef WITH_AUDIO
/*
  int tmp = myspiffs.getIntSetting(F("data_pin"));
  if(tmp != -1)
    LedStripeDataPin = tmp;
*/
#endif

  // initialize LED driver
  display.init(LedStripeDataPin, sec_flash_enabled?LED_STRIPE_COUNT+1:LED_STRIPE_COUNT);

  // clear renderer matrix
  renderer.clearScreenBuffer(matrix);

#if defined(myDEBUG)
  Helper::LedBlink(3, 500);
#endif

  iconRenderer.renderAndDisplay(F("/setup.ico"),1500,1);
  
  myspiffs.setSetting(F("power"),F("on"));

  int iRGB=myspiffs.getRGBSetting(F("rgb"));
  display.setColor(iRGB==-1?0xffffff:iRGB);

  getHostname(&ssid);
  DEBUG_PRINT(F("Start Hostname="));
  DEBUG_PRINTLN(ssid);

#if defined(ESP8266)
//  MDNS.begin(ssid);
  WiFi.hostname(ssid);
#else
  //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  //mMDNS.begin(ssid.c_str());
  if(!MDNS.begin(ssid.c_str())) {
     Serial.println("Error starting mDNS");
     return;
  }
  WiFi.setHostname(ssid.c_str());
#endif

  pinMode(RESET_PIN, INPUT);
  reset_pin = analogRead(RESET_PIN);
  Serial.print(F("reset_pin A0="));
  Serial.println(reset_pin);

  DEBUG_PRINT(F("\nreset_pin A0="));
  DEBUG_PRINTLN(reset_pin);
  if(reset_pin >= 1023) {
    Serial.println(F("\nResetting everything"));
    Serial.flush();
    delay(500);
    mywifi.doReset();
  }  if (ap_mode) {
#define DNS_PORT 53
#define DNS_NAME F("wortuhr.local")

    IPAddress local_IP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 0);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.persistent(false);
    WiFi.begin();
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(CONFIG_PORTAL_SSID, "wortuhr", 1, false, 2); // ssid, psk, channel, hidden, max_connection
    dns1.start(DNS_PORT, DNS_NAME, local_IP);
  } else {
    if (myspiffs.getSetting(F("wifi_ssid")).length() && myspiffs.getSetting(F("wifi_pass")).length()) {
      mywifi.getWifiParams(myspiffs.getSetting(F("wifi_ssid")), myspiffs.getSetting(F("wifi_pass")));
    } else {
      DEBUG_PRINTLN("Entering Access Point mode");
      mywifi.getWifiParams(ssid, "");
    }
  }
  
  DEBUG_PRINTLN("Setting all routes");
  
  server.onNotFound(handle_notfound);

  server.on("/do", HTTP_GET, handle_doArgs);                  // handle special args

  server.on("/state", HTTP_GET, handle_state);

  server.on("/reboot", HTTP_GET, handle_reboot);              // reboot clock

#ifdef WITH_LICENSE
  checkLicense();
#endif

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, myspiffs.getHtmlFilename("index", true), String(), false, processor);
  });
  
  server.on("/init", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", INI_Setup_html);
  });

  server.on("/expert", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, myspiffs.getHtmlFilename("expert", true), String(), false, processor); 
  });

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, textPlain, "OK");
  }, handle_FileUpload);

  server.on("/getSettings", HTTP_GET, handle_getSettings);    // get all setting values

  server.on("/saveExpertSettings", HTTP_POST, handle_saveExpertSettings);  // save expert setting values
  
  server.on("/saveSettings", HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {

    
    if(!index)
      bodyData="";
  
    for(int i=0; i<len; i++)
      bodyData += (char)*(data+i);

    DEBUG_PRINTF("saveSettings: index=%d, len=%d, total=%d, bodyData=\"%s\"\n", index, len, total, bodyData.c_str());

    if(index+len == total) {
      myspiffs.setSettings(bodyData);
      
      COLOR_T c = myspiffs.getRGBSetting(F("rgb"));
      display.setColor(c);
      display.setBrightness();

#ifdef WITH_ALEXA
      alexa.setColor(c);
      alexa.setBrightness();
#endif
      
      String s = myspiffs.getSetting(F("nighton"));
      night_on.setHours(s.substring(0, 2).toInt());
      night_on.setMinutes(s.substring(3).toInt());
  
      s = myspiffs.getSetting(F("nightoff"));
      night_off.setHours(s.substring(0, 2).toInt());
      night_off.setMinutes(s.substring(3).toInt());

      sec_flash_enabled = myspiffs.getBoolSetting(F("sec_flash_enabled"));
      seconds_stripe = myspiffs.getIntSetting(F("seconds_stripe"));

#ifdef WITH_ALEXA
      if(myspiffs.getBoolSetting(F("power")))
        alexa.setState(true);
      else
        alexa.setState(false);
#endif

      bodyData=myspiffs.getSetting(F("time_zone"));
      if(bodyData != mytm.tm_timezone) {
        mytm.tm_timezone = bodyData;
        timezoneChanged = true;
      }
      bodyData=myspiffs.getSetting(F("ntp_server"));
      if(bodyData != mytm.tm_ntpserver) {
        mytm.tm_ntpserver = bodyData;
        timezoneChanged = true;
      }
      bodyData=myspiffs.getSetting(F("latitude"));
      if(bodyData.toDouble() != mytm.tm_lat) {
        mytm.tm_lat = bodyData.toDouble();
        timezoneChanged = true;
      }
      bodyData=myspiffs.getSetting(F("longitude"));
      if(bodyData.toDouble() != mytm.tm_lon) {
        mytm.tm_lon = bodyData.toDouble();
        timezoneChanged = true;
      }
    
      lastMinutes = -1;
      bodyData="";
    }
    request->send(200, textPlain, "OK");
  });

  server.on("/format", HTTP_GET, handle_format);              // format LittleFS

  server.on("/reset_wifi", HTTP_GET, handle_resetWiFi);       // reset WiFi settings

  server.on("/show_icon", HTTP_GET, handle_showIcon);         // show an icon

  server.on("/seen", HTTP_GET, handle_seen);         // show an icon

//  server.onRequestBody(handle_onBody);
  
#ifdef WITH_MQTT
  server.on("/mqttenable",  HTTP_GET, handle_mqttEnable);     // enable MQTT
  server.on("/mqttdisable", HTTP_GET, handle_mqttDisable);    // disable MQTT
  mymqtt.init();
#endif

  // Display einschalten...
  // display.show();
  

  // Alexa start
#ifdef WITH_ALEXA
    if(myspiffs.getBoolSetting("alexaEnabled")) {
      alexa.initAlexa();
    }
    if(!alexa.alexaEnabled)
#endif
    server.begin();
    DEBUG_PRINTLN("HTTP server started");


  mytime.confTime();

  mytime.getTime(&mytm);

  timeStamp = mytime.getTime();

  if (ap_mode)
    ip = WiFi.softAPIP().toString();
  else
    ip = WiFi.localIP().toString();

  // Nachdem wir eine Uhrzeit haben kann jetzt Adjust Brightness initialisiert werden
  adjb.init();

  // und danach auch die Helligkeit eingestellt werden
  display.setBrightness();

  // in order to see associated IP addess and version of this sketch
  String startTicker = myspiffs.getResource("time", "Zeit") + " " + timeStamp + " IP: " + ip + " HN: " + ssid + "  " + FIRMWARE;
  timeStamp = mytime.getDate();

  // initialize night shutdown
  String s = myspiffs.getSetting(F("nighton"));
  night_on.setHours(s.substring(0, 2).toInt());
  night_on.setMinutes(s.substring(3).toInt());

  s = myspiffs.getSetting(F("nightoff"));
  night_off.setHours(s.substring(0, 2).toInt());
  night_off.setMinutes(s.substring(3).toInt());

  //ticker.setTickerText(myspiffs.getSetting(F("ticker_text")));

  // DCF77-Empfaenger einschalten...
  #ifdef DCF77_SENSOR_EXISTS
    dcf77.enable(true);
  #endif

  Serial.println(startTicker);
  Serial.println(F("setup done"));
  COLOR_T c = display.getColor();
  display.setColor(0xff6000);
  ticker.render(1, 180, startTicker);
  c = myspiffs.getRGBSetting(F("rgb"));
  display.setColor(c==-1?0xffffff:c);
  display.setBrightness();
}

/**
 * Korrekte Daten (auf Basis der Pruefbits) vom DCF-Empfaenger
 * bekommen. Sicherheitshalber gegen Zeitabstaende der RTC pruefen.
 */
 void manageNewDCF77Data() {
  
 }
#ifdef DCF77_SENSOR_EXISTS_XXXX
    void manageNewDCF77Data() {
        DEBUG_PRINT(F("Captured: "));
        DEBUG_PRINTLN(dcf77.asString());
        DEBUG_FLUSH();
    
        rtc.readTime();
        dcf77Helper.addSample(&dcf77, &rtc);
        // Stimmen die Abstaende im Array?
        // Pruefung mit Datum!
        if (dcf77Helper.samplesOk()) {
            helperSeconds = 59;
            rtc.setSeconds(0);
            rtc.set(&dcf77, false);
            rtc.addSubHoursOverflow(settings.getTimeShift());
            rtc.writeTime();
            DEBUG_PRINTLN(F("DCF77-Time (+/- Timeshift) written to RTC."));
            DEBUG_FLUSH();
            dcf77.setDcf77SuccessSync(&rtc);
            
        } else {
            DEBUG_PRINTLN(F("DCF77-Time trashed because wrong distances between timestamps."));
            DEBUG_FLUSH();
        }
    }
#endif


/*
   handle loop
*/
void loop()
{
  if(uploadStarted)
    return;
  if(uploadFilename.length()) {
    if(uploadFilename.endsWith(F(".tar"))) {
      delay(500);
      expandTar();
    }
    if(uploadFilename.endsWith(F(".bin"))) {
      delay(500);
      if(updateFW())
          ESP.restart();
    }
    uploadFilename = "";
  }
 
  if(reboot) {
    if(reason==1)
      system_restore();
     ESP.restart();
  }
    

  if(displayEffekt) {
    DEBUG_PRINTLN("call displayEffekt");
    effekte.displayEffekt(displayEffekt-1);
    displayEffekt=0;
  }

  if(displayTicker) {
    DEBUG_PRINTLN("call ticker.render");
    ticker.render(true);
    displayTicker=0;
    lastMinutes = -1;
  }

  if(iconFile.length()>0) {
    iconRenderer.renderAndDisplay(iconFile,5000,3);
    iconFile="";
    lastMinutes = -1;
  }

  if(timezoneChanged) {
    timezoneChanged = false;
    mytime.confTime();
    adjb.init();
    adjb.forceUpdate();
  }
  
  delay(300);

  mytime.getTime(&mytm);

/*  if(ap_mode)
    dns1.processNextRequest();
*/

  if (mytm.tm_min != lastMinutes) {
    DEBUG_PRINTF("Loop2 Zeit: %.2d:%.2d:%.2d\n", mytm.tm_hour, mytm.tm_min, mytm.tm_sec); // Zeit Datum Print Ausgabe formatieren
    DEBUG_FLUSH();
    if(WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      WiFi.begin(myspiffs.getSetting(F("wifi_ssid")), myspiffs.getSetting(F("wifi_pass")));
      myspiffs.writeLog(F("Lost Wifi connection, try to reconnect\n"));
    }

    /**
       check if night shutdown 21:00-05:00 cur: 22:00
        on < off ==> cur > on && akt <= off
        on > off ==> not(cur > off && cur < on) 
    */
    unsigned int rtc_minutes_of_day = (mytm.tm_hour==24?0:mytm.tm_hour) * 60 + mytm.tm_min;
    unsigned int on = night_on.getMinutesOfDay();
    unsigned int off = night_off.getMinutesOfDay();
    shutdown = false;
    if(on < off) {
      if(rtc_minutes_of_day >= on && rtc_minutes_of_day <= off)
        shutdown = true;
    } else {
      /**
              22:00 <= 05:00       ||      22:00 >= 21:00  ==> true
              20:00 <= 05:00       ||      20:00 >= 21:00  ==> false
              01:00 <= 05:00       ||      01:00 >= 21:00  ==> true
              06:00 <= 05:00       ||      06:00 >= 21:00  ==> false
      */
      if(rtc_minutes_of_day <= off || rtc_minutes_of_day >= on)
        shutdown = true;
    }

    if(shutdown)
      brightness_night = myspiffs.getIntSetting(F("brightness_night"));
    else
      brightness_night = 0;

    bool power = myspiffs.getBoolSetting(F("power"));
    if(!power || shutdown) {
      if(!power || brightness_night <= 0)
        display.clear();
      else {
        display.setBrightness(brightness_night);
        nightOn=true;
        renderer.showTime(matrix);
      }
      display.show();
      DEBUG_PRINTF("Night time, brightness_night=%d\n", brightness_night);
      if(mytm.tm_hour == 4 && mytm.tm_min == 0 && mytm.tm_sec < 5 && myspiffs.getIntSetting(F("nightly_restart")) == 1)
        ESP.restart();
    } else {
      // Falls die Helligkeit mal verändert wurde, wird jetzt der standard wiederhergestellt
      if(nightOn) {
        nightOn = false;
        display.setBrightness(0);
      }
      display.setBrightness();
      /**
       * On each hour display a nice random pattern
       */
      effekte.displayEffekt(-1);
      
      /**
       * if a ticker text exist display text
       */
      ticker.render();

      /**
       * if it's sunrise or sunset display tickertext
       */
      adjb.sunriseSunset();
      
      /**
         render time and minutes
      */
      DEBUG_PRINTLN("vor renderer.showTime");
      renderer.showTime(matrix);
      DEBUG_PRINTLN("nach renderer.showTime");
    }

    lastMinutes = mytm.tm_min;

    #if defined(WITH_MQTT)
      mymqtt.publish();
    #endif

    #if defined(WITH_MAIL)
      mymail.check();
    #endif
  }

  if(seconds_stripe>1) {
    DEBUG_PRINTLN("seconds_stripe>1");
    if(mytm.tm_sec != lastSecond && !shutdown) {
      if(mytm.tm_sec) {
        COLOR_T c = display.getColor();
        display.drawPixel(mytm.tm_sec+3, 10, c);
        if(seconds_stripe == 2 && mytm.tm_sec > 1)
          display.clearPixel(mytm.tm_sec+2, 10);
      } else {
        for(int i=1;i<=59;i++)
          display.clearPixel(i+3, 10);
      }
      if(!sec_flash_enabled && !shutdown) {
        lastSecond = mytm.tm_sec;
        display.show();
      }
    }
  }

  if(seconds_stripe==1 && sec_flash_enabled) {
    DEBUG_PRINTLN("seconds_stripe==1 && sec_flash_enabled");
    if(mytm.tm_sec != lastSecond && !shutdown) {
      if(mytm.tm_sec % 2) {
        COLOR_T c = display.getColor();
        display.drawPixel(4, 10, c);
      } else {
        display.clearPixel(4, 10);
      }
      lastSecond = mytm.tm_sec;
      display.show();
    }
  }

  #if defined(WITH_MQTT)
    mymqtt.check();
  #endif

#ifdef WITH_AUDIO
   myaudio.check();
#endif

/*
 * DCF77-Empfaenger anticken...
 */
#ifdef DCF77_SENSOR_EXISTS
  if (dcf77.poll(myspiffs.getBoolSetting(F("DcfSignalIsInverted"))))
    manageNewDCF77Data();
#endif

#ifdef WITH_ALEXA
  if(alexa.alexaEnabled)
    alexa.loopAlexa();
#endif

}
