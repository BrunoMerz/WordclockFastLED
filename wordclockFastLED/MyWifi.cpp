/**
   MyWifi.cpp
   @autor    Bruno Merz

   @version  2.0
   @created  30.12.2019
   @updated  31.03.2021 

*/

#include "MyWifi.h"

//#define myDEBUG
#include "Debug.h"
#include "Helper.h"


extern MyWifi mwifi;

extern MySpiffs myspiffs;
extern IconRenderer iconRenderer;

#define USE_LITTLEFS
#define USING_CORS_FEATURE         true

#define USE_ESP_WIFIMANAGER_NTP    false
#define USE_CLOUDFLARE_NTP         false

#define USE_STATIC_IP_CONFIG_IN_CP false

//#define _ESPASYNC_WIFIMGR_LOGLEVEL_ 4

#include <ESPAsync_WiFiManager.h>

extern AsyncWebServer server;
extern AsyncDNSServer dns1;
extern String ip;

ESPAsync_WiFiManager wifiManager(&server, &dns1);
static boolean  _wpsSuccess;
static boolean  _got_ip;
static int      _wifi_stat;



#if defined(ESP32)

#include "esp_wps.h"

static esp_wps_config_t config;

String wpspin2string(uint8_t a[]) {
  char wps_pin[9];
  for(int i=0;i<8;i++){
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
}


void wpsStop(void) {
    if(esp_wifi_wps_disable()){
      DEBUG_PRINTLN("WPS Disable Failed");
    }
}


boolean wpsStart(void) {
  _wpsSuccess=false;
  if(esp_wifi_wps_enable(&config)) {
    DEBUG_PRINTLN("WPS Enable Failed");
  } else if(esp_wifi_wps_start(0)) {
    DEBUG_PRINTLN("WPS Start Failed");
  } else
    DEBUG_PRINTLN("esp_wifi_wps_start OK");

  for (int i = 0; i < WPSTIMEOUT ; i++) {
    if (WiFi.status() == WL_CONNECTED)
    {
      DEBUG_PRINTLN("WPS WiFi connected");
      _wpsSuccess=true;
      break;
    }
    delay(1000);
    DEBUG_PRINTF("Waiting for IP, i=%d\n",i);
  }
  if(!_wpsSuccess) {
    DEBUG_PRINTLN("WPS WiFi failed");
    wpsStop();
  }
  return _wpsSuccess;
}


void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info){
  switch(event){
    case ARDUINO_EVENT_WIFI_STA_START:
      DEBUG_PRINTLN("Station Mode Started");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      ip = WiFi.localIP().toString();
      DEBUG_PRINTLN("Connected to :" + String(WiFi.SSID()));
      DEBUG_PRINT("GOT_IP: ");
      DEBUG_PRINTLN(ip);
      myspiffs.writeLog(F("WiFi GOT_IP: "));
      myspiffs.writeLog((char *)ip.c_str(), false);
      myspiffs.writeLog(F("\n"), false);
      _got_ip = true;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      DEBUG_PRINTLN("Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      DEBUG_PRINTLN("WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()));
      wpsStop();
      delay(10);
      WiFi.begin();
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
      DEBUG_PRINTLN("WPS Failed, retrying");
      wpsStop();
      wpsStart();
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      DEBUG_PRINTLN("WPS Timedout, retrying");
      wpsStop();
      wpsStart();
      break;
    case ARDUINO_EVENT_WPS_ER_PIN:
      DEBUG_PRINTLN("WPS_PIN = " + wpspin2string(info.wps_er_pin.pin_code));
      break;
    default:
      break;
  }
}


void wpsInitConfig(){
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}


#else // ESP8266

void WiFiEvent(WiFiEvent_t event) {
  _wifi_stat = event;
  DEBUG_PRINTF("[WiFi-event] event: %d\n", _wifi_stat);
  switch(event) {
    case WIFI_EVENT_STAMODE_CONNECTED:        // 0
      DEBUG_PRINTLN("WiFi connected");
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:     // 1
      DEBUG_PRINTLN("WiFi disconnected");
      myspiffs.writeLog(F("WiFi disconnected\n"));
      break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:  // 2
      DEBUG_PRINTLN("Author mode change");
      break;
    case WIFI_EVENT_STAMODE_GOT_IP:           // 3
      ip = WiFi.localIP().toString();
      DEBUG_PRINTLN("WiFi GOT_IP");
      DEBUG_PRINTLN("IP address: ");
      DEBUG_PRINTLN(ip);
      myspiffs.writeLog(F("WiFi GOT_IP: "));
      myspiffs.writeLog((char *)ip.c_str(), false);
      myspiffs.writeLog(F("\n"), false);
      _got_ip = true;
      break;
    case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:     // 4
      DEBUG_PRINTLN("WiFi DHCP Timeout");
      break;
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:  // 5
      DEBUG_PRINTLN("WiFi STA connected");
      myspiffs.writeLog(F("WiFi STA connected\n"));
      break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED: // 6
      DEBUG_PRINTLN("WiFi STA disconnected");
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:  // 7
      DEBUG_PRINTLN("WiFi probe request received");
      break;
    case WIFI_EVENT_MODE_CHANGE:                // 8
      DEBUG_PRINTLN("WiFi mode changed");
      break;
    case WIFI_EVENT_SOFTAPMODE_DISTRIBUTE_STA_IP: // 9
      DEBUG_PRINTLN("WiFi soft AP mode");
      break;
  }
}
#endif // ESP8266


MyWifi::MyWifi(void) {
  
}

/**
   Callback function that indicates that the ESP has switched to AP mode
*/
void MyWifi::configModeCallback (ESPAsync_WiFiManager *myWiFiManager) {
  DEBUG_PRINTLN("Entered configModeCallback");
  //display.flashPixel(2, 0, red, 3, 500);
  iconRenderer.renderAndDisplay(F("/accesspoint.ico"),0,1);
  DEBUG_PRINTLN(WiFi.softAPIP()); //imprime o IP do AP
  DEBUG_PRINTLN(myWiFiManager->getConfigPortalSSID());
}

/**
   Callback function that indicates that the ESP has saved the settings
*/
void MyWifi::saveConfigCallback (void) {
  DEBUG_PRINTLN("Enter saveConfigCallback");
  DEBUG_PRINTLN(WiFi.softAPIP()); //imprime o IP do AP
  myspiffs.setSetting(F("wifi_ssid"), wifiManager.getSSID());
  myspiffs.setSetting(F("wifi_pass"), wifiManager.getPW());
  myspiffs.writeSettings(true);
  DEBUG_PRINTLN("saveConfigCallback: "+myspiffs.getSettings());
}


//Startet die WPS Konfiguration
#if defined(ESP8266)
bool MyWifi::myStartWPS(void) {
  DEBUG_PRINTLN("WPS Konfiguration gestartet");
  _wpsSuccess = WiFi.beginWPSConfig();
  if(_wpsSuccess) {
      // Muss nicht immer erfolgreich heißen! Nach einem Timeout bist die SSID leer
      String newSSID = WiFi.SSID();
      if(newSSID.length() > 0) {
        // Nur wenn eine SSID gefunden wurde waren wir erfolgreich 
        DEBUG_PRINTF("WPS fertig. Erfolgreich angemeldet an SSID '%s', PWD '%s'\n", newSSID.c_str(), WiFi.psk().c_str());
      } else {
        _wpsSuccess = false;
      }
  } else
    DEBUG_PRINTLN("beginWPSConfig failed");
  return _wpsSuccess;
}

#else // ESP32

bool MyWifi::myStartWPS(void) {
  DEBUG_PRINTLN("WPS Konfiguration gestartet");
  myspiffs.writeLog(F("WPS Konfiguration gestartet\n"));
  _wpsSuccess = true;
  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_MODE_STA);
  DEBUG_PRINTLN("Starting WPS");
  wpsInitConfig();
  DEBUG_PRINTLN("Config done");
  wpsStart();

  return _wpsSuccess;
}

#endif


/**
   Initialize
*/
void MyWifi::init(void) {
#if defined(myDEBUG) && !defined(ESP_TX_RX_AS_GPIO_PINS)
  // Enable debug output 
  DEBUG_PRINTLN("setDebugOutput to true");
  wifiManager.setDebugOutput(true);
#else
  wifiManager.setDebugOutput(false);
#endif

  /**
     Set Callback Funktion for accesspoint konfiguration
  */
  wifiManager.setAPCallback(configModeCallback);

  /**
      Callback Funktion after connecting to a network
  */
  wifiManager.setSaveConfigCallback(saveConfigCallback);
}


/**
   resets network settings and restarts ESP
*/
void MyWifi::doReset(void) {
  DEBUG_PRINTLN("MyWifi::doReset called");
  myspiffs.removeSetting(F("wifi_ssid"));
  myspiffs.removeSetting(F("wifi_pass"));
  myspiffs.writeSettings(true);
  //system_restore();
  ESP.restart();
}

/**
   connects to a network
*/
void MyWifi::getWifiParams(String ssid, String pass) {
  int ret;
  int timeout;

#if defined(myDEBUG) && !defined(ESP_TX_RX_AS_GPIO_PINS)
  // Enable debug output
  DEBUG_PRINTLN("setDebugOutput to true");
  wifiManager.setDebugOutput(true);
#else
  wifiManager.setDebugOutput(false);
#endif

  DEBUG_PRINTF("getWifiParams: ssid=%s, pass=%s\n",ssid.c_str(),pass.c_str());
  /**
     indicates before connecting to a network
  */  
  WiFi.onEvent(WiFiEvent);
  _got_ip=false;

  if(ssid.length() && pass.length()) {
    iconRenderer.renderAndDisplay(F("/wifi.ico"),500,1);
    timeout = WIFITIMEOUT;
  } else {
    Serial.println(F("Starting WPS"));
    myspiffs.writeLog(F("Starting WPS\n"));
    iconRenderer.renderAndDisplay(F("/wps.ico"),500,1);
    timeout = WPSTIMEOUT;
    WiFi.mode(WIFI_STA);
    if(myStartWPS()) {
      myspiffs.setSetting(F("wifi_ssid"), WiFi.SSID());
      myspiffs.setSetting(F("wifi_pass"), WiFi.psk());
      myspiffs.writeSettings(true);
      DEBUG_PRINTF("myStartWPS successfull, ssid=%s, passwd=%s\n",WiFi.SSID().c_str(), WiFi.psk().c_str());
      ESP.restart();
    }
  }

  int16_t i=0;
  DEBUG_PRINTF("pass=%s\n",pass.c_str());
  if (pass.length() > 0) {
    int j=0;
    _wifi_stat = -1;
    WiFi.disconnect();
    i=0;
#if defined(ESP8266)
    while(i++ < 10 && _wifi_stat != WIFI_EVENT_STAMODE_DISCONNECTED) {
#else
    while(i++ < 10 && _wifi_stat != SYSTEM_EVENT_STA_DISCONNECTED) {
#endif
      //Serial.println("Warten auf disconnect");
      delay(100);
    }
    while(!_got_ip) {
#ifdef myDEBUG
      char logtxt[100];
      sprintf(logtxt,"WiFi.begin with, ssid=%s, timeout=%d i=%d\n",ssid.c_str(), timeout, i);
      DEBUG_PRINT(logtxt);
      myspiffs.writeLog(logtxt);
#endif
      _wifi_stat = -1;
      DEBUG_PRINTLN("vor WiFi.begin");
      WiFi.begin(ssid.c_str(), pass.c_str());
      i=0;
      while(i++ < timeout && !_got_ip) {
        // Warten auf connect oder timeout
#ifdef myDEBUG
        DEBUG_PRINTF("_wifi_stat=%d, i=%d\n", _wifi_stat, i);
        myspiffs.writeLog(F("Waiting for WiFi Connection\n"));
#endif
        delay(1000);
        if(digitalRead(WIFI_RESET)) {
          myspiffs.writeLog(F("Got Interrupt, resetting WiFi Settings\n"));
          doReset();
        }
      }
    }
  }
  else {
    timeout = WIFITIMEOUT;
    iconRenderer.renderAndDisplay(F("/accesspoint.ico"),500,1);
    DEBUG_PRINTF("setConfigPortalTimeout timeout=%i\n", timeout);
    Serial.println(F("Starting Accesspoint"));
    wifiManager.setConfigPortalTimeout(timeout);
    wifiManager.setConfigPortalChannel(0);
    wifiManager.setMinimumSignalQuality(30);
    //wifiManager.setConnectTimeout(timeout);
    ret = wifiManager.startConfigPortal(ssid.c_str());
    DEBUG_PRINTF("After setConfigPortalTimeout timeout=%i\n", timeout);
    i=0;
    while(i++ < timeout && !_got_ip) {
      //Serial.printf("While %i\n", i);
      delay(1000);
    }
  }

  if (!_got_ip) {
    DEBUG_PRINTLN("Failed to connect, remove WiFi Settings");
    DEBUG_FLUSH();
    myspiffs.removeSetting(F("wifi_ssid"));
    myspiffs.removeSetting(F("wifi_pass"));
    myspiffs.writeSettings(true);
    //system_restore();
    ESP.restart();
  } else {
    DEBUG_PRINTLN("Got connection");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    iconRenderer.renderAndDisplay(F("/check.ico"),2000,3);
    myspiffs.setSetting(F("wifi_ssid"), WiFi.SSID());
    myspiffs.setSetting(F("wifi_pass"), WiFi.psk());
    myspiffs.writeSettings(true);
    DEBUG_PRINTF("getWifiParams: ssid=%s, passwd=%s\n",WiFi.SSID().c_str(), WiFi.psk().c_str());
    DEBUG_PRINTLN("getWifiParams: "+myspiffs.getSettings());
  }
}



/**
   get State
*/
String MyWifi::getState(void) {
  const static char wifi[] PROGMEM = "\n\nWIFI:\nSSID=";
  const static char pwd[] PROGMEM = "\nPWD=";
  Helper::writeState(wifi, myspiffs.getSetting(F("wifi_ssid")));
  Helper::writeState(pwd, "");
  for(int  i=0;i<myspiffs.getSetting(F("wifi_pass")).length();i++)
    Helper::writeState("*");
  return "";
}
