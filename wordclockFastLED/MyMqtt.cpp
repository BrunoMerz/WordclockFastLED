  /**
   MyMqtt.cpp
   @autor    Bruno Merz

   @version  1.1
   @created  14.02.2020
   @updated  23.03.2021

*/

//#define myDEBUG
#include "Debug.h"

#include "Configuration.h"

#ifdef WITH_MQTT

#include "MyMqtt.h"
#include "Helper.h"


extern MyDisplay display;
extern Ticker ticker;
extern IconRenderer iconRenderer;
extern Effekte effekte;
extern MySpiffs myspiffs;
extern MyMqtt mymqtt;
extern int lastMinutes;
extern word matrix[16];
extern String ip;
extern MYTM mytm;
extern AdjustBrightness adjb;
extern bool endlessloop;
extern int brightness_night;

WiFiClient _client;
PubSubClient _mqtt_client(_client);

StaticJsonDocument<JSON_ARRAY_SIZE(JSON_MQTT_COUNT)+JSON_OBJECT_SIZE(JSON_MQTT_COUNT)> mqttDoc;


/*
 * Constructor
 */
MyMqtt::MyMqtt(void) {
  _enabled      = true;
  _mqtt_reconnect_retries = 0;
  _mqtt_server = NULL;
  _mqtt_port = 0;
/*
  _mqtt_last_in = NULL;
  _mqtt_last_out = NULL;
 _mqtt_last_color = NULL;
*/
}


boolean MyMqtt::init(void) {
  if(!reconnect())
    _enabled = false;
  return _enabled;
}

void MyMqtt::allocString(char **dest, String src) {
  int ls = src.length();
  int ld = *dest?strlen(*dest):0;
  int lm = ls+1;

  if(ls) {
    if(*dest) {
      if(ls != ld) {
        free(*dest);
      } else
        lm = 0;
    } 

    if(lm)
      *dest = (char *)malloc(lm);
 
    strcpy(*dest,src.c_str());
  }
}


boolean MyMqtt::reconnect(void) {
  // Loop until we're reconnected
  _mqtt_reconnect_retries = 0;
  if(!_mqtt_server && _enabled) {
    //_mqtt_server = myspiffs.getSetting(F("mqtt_server"));
    allocString(&_mqtt_server, myspiffs.getSetting(F("mqtt_server")));
    _mqtt_port   = myspiffs.getIntSetting(F("mqtt_port"));
    
    
    if(!_mqtt_server || !_mqtt_port) {
      _enabled = false;
      return false;
    }
 
    //_mqtt_user   = myspiffs.getSetting(F("mqtt_user"));
    //_mqtt_pw     = myspiffs.getSetting(F("mqtt_password"));
    allocString(&_mqtt_user, myspiffs.getSetting(F("mqtt_user")));
    allocString(&_mqtt_pw, myspiffs.getSetting(F("mqtt_password")));
  
    //_mqtt_hostname      = myspiffs.getSetting(F("hostname"));
    //_mqtt_clientid      = _mqtt_hostname;
    String tmp            = myspiffs.getSetting(F("hostname"));
    allocString(&_mqtt_hostname, tmp);
    allocString(&_mqtt_clientid, tmp);
    
    //_mqtt_will_topic    = _mqtt_hostname + F("/status");
    //_mqtt_log_topic    = _mqtt_hostname + F("/log");
    //_mqtt_intopic       = _mqtt_hostname + F("/in");
    //_mqtt_outtopic      = _mqtt_hostname + F("/out");
    //_mqtt_will_payload  = "OFFLINE";

    allocString(&_mqtt_will_topic, tmp + F("/status"));
    allocString(&_mqtt_log_topic, tmp + F("/log"));
    allocString(&_mqtt_intopic, tmp + F("/in"));
    allocString(&_mqtt_outtopic, tmp + F("/out"));
    allocString(&_mqtt_will_payload, F("OFFLINE"));
  }
  
  while (!_mqtt_client.connected() && _mqtt_reconnect_retries < MQTT_MAX_RECONNECT_TRIES) {
    _mqtt_reconnect_retries++;
    DEBUG_PRINTF("Attempting MQTT connection clientId=%s, server=%s, port=%d, %d / %d ...\n", _mqtt_clientid, _mqtt_server, _mqtt_port, _mqtt_reconnect_retries, MQTT_MAX_RECONNECT_TRIES);
    // Attempt to connect
    _mqtt_client.setServer(_mqtt_server, _mqtt_port);
    _mqtt_client.setCallback(mqtt_callback);
    if (_mqtt_client.connect(_mqtt_clientid, _mqtt_user, _mqtt_pw, _mqtt_will_topic, 2, true, _mqtt_will_payload, true)) {
      DEBUG_PRINTLN("MQTT connected!");
      DEBUG_PRINTF("MQTT topic in: %s\n", _mqtt_intopic);
      DEBUG_PRINTF("MQTT topic out: %s\n", _mqtt_outtopic);
      _mqtt_client.subscribe(_mqtt_intopic);
    } else {
      DEBUG_PRINT("failed, rc=");
      DEBUG_PRINT(_mqtt_client.state());
      DEBUG_PRINTLN(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  if (_mqtt_reconnect_retries >= MQTT_MAX_RECONNECT_TRIES) {
    DEBUG_PRINTF("MQTT connection failed, giving up after %d tries ...\n", _mqtt_reconnect_retries);
    return false;
  } else {
    _mqtt_client.publish(_mqtt_outtopic, new byte[0], 0, true);
    _mqtt_client.publish(_mqtt_will_topic, "ONLINE");
  }
  return true;
}


void MyMqtt::mqtt_callback(char* topic, byte* payload, unsigned int length) {
 
  DEBUG_PRINT("Message arrived: length=");
  DEBUG_PRINT(length);
  DEBUG_PRINT(", topic [");
  DEBUG_PRINT(topic);
  DEBUG_PRINT("] payload [");
  DEBUG_PRINTLN((char *)  payload);
  DEBUG_PRINTLN("]");
  DEBUG_FLUSH();

  char c[2]="X";
  for(int i=0;i<length;i++) {
    c[0] = *(payload+i);
    myspiffs.writeLog(c,i?false:true);
  }
  myspiffs.writeLog("\n",false);

  mqttDoc.clear();
  DeserializationError error = deserializeJson(mqttDoc, payload, length);
  if (!error) {
    String  s=""; 
    int     i;
    byte    b;
    bool    bl;
    bool    power = myspiffs.getBoolSetting(F("power"));

    int     _speed           = myspiffs.getIntSetting(F("G"));
    int     _repeat          = myspiffs.getIntSetting(F("W"));
    int     _frequency       = myspiffs.getIntSetting(F("H"));
    COLOR_T _mqtt_save_rgb   = myspiffs.getRGBSetting(F("rgb"));
    COLOR_T new_color=0x01000000;
    
    JsonVariant jv = mqttDoc[F("ticker")];
    if (!jv.isNull()) {
      jv = mqttDoc[F("ticker")][F("text")];
      if (!jv.isNull()) {
        s = jv.as<String>();
        myspiffs.setSetting(F("ticker_text"),s);
      }
      jv = mqttDoc[F("ticker")][F("color")];
      if (!jv.isNull()) {
        new_color = strtol(jv.as<String>().c_str(), NULL, 0);;
        myspiffs.setSetting(F("F"), new_color);
        display.setColor(new_color);
      }
      jv = mqttDoc[F("ticker")][F("speed")];
      if (!jv.isNull()) { 
        _speed  = jv.as<int>();
        myspiffs.setSetting(F("G"),jv.as<String>());
      }
      jv = mqttDoc[F("ticker")][F("repeat")];
      if (!jv.isNull()) { 
        _repeat  = jv.as<int>();
        myspiffs.setSetting(F("W"),jv.as<String>());
      }
      jv = mqttDoc[F("ticker")][F("frequency")];
      if (!jv.isNull()) { 
        _frequency  = jv.as<int>();
        myspiffs.setSetting(F("H"),jv.as<String>());
      }
      jv = mqttDoc[F("ticker")][F("reset")];
      if (!jv.isNull()) { 
        myspiffs.setSetting(F("ticker_text"),"");
      }
      DEBUG_PRINTLN("mqtt_callback: tickertext='"+s+"'");
      if(s.length())
        ticker.render(_repeat, _speed, s+"    ");
    }

    jv = mqttDoc[F("icon")][F("file")];
    if (!jv.isNull()) {
      s = jv.as<String>();
      if(!s.startsWith("/"))
        s = "/" + s;
      jv = mqttDoc[F("icon")][F("duration")];
      if (!jv.isNull()) {
        i = jv.as<int>();
      } else 
        i = 4000;
      jv = mqttDoc[F("icon")][F("action")];
      if (!jv.isNull()) {
        b  = jv.as<byte>();
       } else
        b = 3;
      iconRenderer.renderAndDisplay(s, i, b);
    }

    jv = mqttDoc[F("power")];
    if (!jv.isNull()) {
      power = jv.as<bool>();
      DEBUG_PRINT("mqtt_callback: power='");
      DEBUG_PRINT(power);
      DEBUG_PRINTLN("'");
      DEBUG_FLUSH();
      myspiffs.setSetting(F("power"),power?"on":"off");
    }
    
    jv = mqttDoc[F("color")][F("rgb")];
    if (!jv.isNull()) {
      COLOR_T new_color1 = strtol(jv.as<String>().c_str(), NULL, 0);;
      myspiffs.setSetting(F("rgb"), new_color1);
      display.setColor(new_color1);
    }
    
    jv = mqttDoc[F("color")][F("hsv")];
    if (!jv.isNull()) {
      uint8_t saturation, brightness;
      s = jv.as<String>();
      DEBUG_PRINT("mqtt_callback: color='"+s+"', ");
      DEBUG_PRINTLN(i);
      DEBUG_FLUSH();
      jv = mqttDoc[F("color")][F("saturation")];
      if (!jv.isNull()) {
        saturation = jv.as<int>();
      } else
         saturation= 255;
      jv = mqttDoc[F("color")][F("brightness")];
      if (!jv.isNull()) {
        brightness = jv.as<int>();
      } else
        brightness = myspiffs.getIntSetting(F("brightness"));
      uint8_t hsv = map(s.toInt(),0,359,0,255);
      COLOR_T rgb = display.getColorHSV(hsv, saturation, brightness);
      myspiffs.setSetting(F("rgb"), rgb);
    }

    jv = mqttDoc[F("night")][F("on")];
    if (!jv.isNull()) {
       myspiffs.setSetting(F("nighton"),jv.as<String>());
    }

    jv = mqttDoc[F("night")][F("off")];
    if (!jv.isNull()) {
      myspiffs.setSetting(F("nightoff"),jv.as<String>());
    }

    jv = mqttDoc[F("brightness")];
    if (!jv.isNull()) {
      jv = mqttDoc[F("brightness")][F("from")];
      if (!jv.isNull()) {
        b = jv.as<byte>();
  
        if(b<1) b=1;
        if(b>100) b=100;
          
        myspiffs.setSetting(F("brightness_from"),(String)b);
      }
  
      jv = mqttDoc[F("brightness")][F("to")];
      if (!jv.isNull()) {
        b = jv.as<byte>();
  
        if(b<1) b=1;
        if(b>100) b=100;
  
        myspiffs.setSetting(F("brightness_to"),(String)b);
      }

      jv = mqttDoc[F("brightness")][F("current")];
      if (!jv.isNull()) {
        b = jv.as<byte>();
  
        if(b<0) b=0;
        if(b>100) b=100;

        display.setBrightness(b);
      }
  
      jv = mqttDoc[F("brightness")][F("night")];
      if (!jv.isNull()) {
        b = jv.as<byte>();
  
        if(b<0) b=0;
        if(b>100) b=100;
  
        myspiffs.setSetting(F("brightness_night"),(String)b);
      }
      /*
      int bip=myspiffs.getIntSetting(F("brightness_to")) + myspiffs.getIntSetting(F("brightness_from"));
      bip /= 2;
      DEBUG_PRINTF("mqtt_callback: bip=%d\n",bip);
      display.setBrightness(bip);
      display.getBrightness();
      */
    }
    
    jv = mqttDoc[F("effect")];
    if (!jv.isNull()) {
      b = jv.as<byte>();
      DEBUG_PRINT("mqtt_callback: effekt=");
      DEBUG_PRINTLN(b);
      DEBUG_FLUSH();

      effekte.displayEffekt(b-1);
    }

    jv = mqttDoc[F("effects")];
    if (!jv.isNull()) {
      bl = jv.as<bool>();
      myspiffs.setSetting(F("effects"),bl?"on":"off");
    }

    jv = mqttDoc[F("itis")];
    if (!jv.isNull()) {
      bl = jv.as<bool>();
      myspiffs.setSetting(F("itis"),bl?"on":"off");
    }
    
    jv = mqttDoc[F("pulse")];
    if (!jv.isNull()) {
      bl = jv.as<bool>();
      myspiffs.setSetting(F("pulse"),bl?"on":"off");
    }
    
    jv = mqttDoc[F("reboot")];
    if (!jv.isNull()) {
      bl = jv.as<bool>();
      if(bl)
        ESP.restart();
    }

    jv = mqttDoc[F("mail")][F("icon")];
    if (!jv.isNull()) {
      s = jv.as<String>();
      if(!s.startsWith("/"))
        s = "/" + s;
      DEBUG_PRINT("mqtt_callback: mail='"+s+"', ");
      DEBUG_PRINTLN(i);
      DEBUG_FLUSH();
      iconRenderer.renderAndDisplay(s, 2500, 1);
      jv = mqttDoc["mail"]["text"];
      if (!jv.isNull()) {
        s = jv.as<String>();
      }
      jv = mqttDoc[F("mail")][F("color")];
      if (!jv.isNull()) {
        new_color  = strtol(jv.as<String>().c_str(), NULL, 0);
        display.setColor(new_color);
      }
      jv = mqttDoc[F("mail")][F("speed")];
      if (!jv.isNull()) {
        _speed  = jv.as<int>();
      }
      ticker.render(_repeat, _speed, s+"    ");
    }

    jv = mqttDoc[F("effect_sequence")];
    if (!jv.isNull()) {
      DEBUG_PRINTLN("mqtt_callback: effect_sequence="+jv.as<String>());
      myspiffs.setSetting(F("effect_sequence"),jv.as<String>());
    }

    jv = mqttDoc[F("loop")];
    if (!jv.isNull()) {
      DEBUG_PRINTLN("mqtt_callback: loop="+jv.as<bool>());
      endlessloop=false;
    }

    if(new_color != 0x01000000) {
      display.setColor(_mqtt_save_rgb);
    }

    lastMinutes=-1;
  }

  DEBUG_PRINTLN();
  DEBUG_FLUSH();
}


void MyMqtt::publish(void) {

    char  _mqtt_topic[12];
    if(!_enabled)
      return;

    mqttDoc.clear();
    sprintf(_mqtt_topic, "%.2d:%.2d:%.2d", mytm.tm_hour, mytm.tm_min, mytm.tm_sec);
    mqttDoc[F("time")] = _mqtt_topic;

    mqttDoc[F("brightness")][F("from")] = myspiffs.getIntSetting(F("brightness_from"));
    mqttDoc[F("brightness")][F("to")] = myspiffs.getIntSetting(F("brightness_to"));
    mqttDoc[F("brightness")][F("night")] = myspiffs.getIntSetting(F("brightness_night"));
    mqttDoc[F("brightness")][F("current")] = display.getBrightness();

    mqttDoc[F("night")][F("on")] = myspiffs.getSetting(F("nighton"));
    mqttDoc[F("night")][F("off")] = myspiffs.getSetting(F("nightoff"));
    mqttDoc[F("power")] = myspiffs.getBoolSetting(F("power"));
    mqttDoc[F("IP")] = ip;
    mqttDoc[F("color")][F("time")] = myspiffs.getSetting(F("rgb"));
    mqttDoc[F("color")][F("led1")] = myspiffs.getSetting(F("led1"));
    mqttDoc[F("color")][F("led2")] = myspiffs.getSetting(F("led2"));
    mqttDoc[F("color")][F("led3")] = myspiffs.getSetting(F("led3"));
    mqttDoc[F("color")][F("led4")] = myspiffs.getSetting(F("led3"));

    mqttDoc[F("itis")] = myspiffs.getBoolSetting(F("itis"));
    mqttDoc[F("pulse")] = myspiffs.getBoolSetting(F("pulse"));

    sprintf(_mqtt_topic, "%.2d.%.2d.%.4d", mytm.tm_mday, mytm.tm_mon, mytm.tm_year);
    mqttDoc[F("date")] = _mqtt_topic;
    mqttDoc[F("language")] = myspiffs.getIntSetting(F("language"));
    mqttDoc[F("geo")][F("lon")] = myspiffs.getSetting(F("longitude"));
    mqttDoc[F("geo")][F("lat")] = myspiffs.getSetting(F("latitude"));

    mqttDoc[F("ticker")][F("text")] = myspiffs.getSetting(F("ticker_text"));
    mqttDoc[F("ticker")][F("sv")] = myspiffs.getSetting(F("SV"));
    mqttDoc[F("ticker")][F("sn")] = myspiffs.getSetting(F("SN"));
    mqttDoc[F("ticker")][F("mv")] = myspiffs.getIntSetting(F("MV"));
    mqttDoc[F("ticker")][F("mn")] = myspiffs.getIntSetting(F("MN"));
    mqttDoc[F("ticker")][F("speed")] = myspiffs.getIntSetting(F("G"));
    mqttDoc[F("ticker")][F("frequency")] = myspiffs.getIntSetting(F("H"));
    mqttDoc[F("ticker")][F("repeat")] = myspiffs.getIntSetting(F("W"));
    mqttDoc[F("ticker")][F("color")] = myspiffs.getSetting(F("F"));

    mqttDoc[F("effects")][F("sequence")] = myspiffs.getSetting(F("effect_sequence"));
    mqttDoc[F("effects")][F("on")] = myspiffs.getBoolSetting(F("effects"));
    mqttDoc[F("expert")][F("time_zone")] = myspiffs.getSetting(F("time_zone"));
    mqttDoc[F("expert")][F("ntp_server")] = myspiffs.getSetting(F("ntp_server"));
    mqttDoc[F("expert")][F("ntp_port")] = myspiffs.getSetting(F("ntp_port"));
    mqttDoc[F("expert")][F("sunrise")] = adjb.getSunrise();
    mqttDoc[F("expert")][F("sunset")] = adjb.getSunset();
    

    _mqtt_client.beginPublish(_mqtt_outtopic, measureJson(mqttDoc), true);
    serializeJson(mqttDoc, _mqtt_client);
    _mqtt_client.endPublish();
}


void MyMqtt::publish(char *topic) {
  if(_enabled) {
    _mqtt_client.publish(_mqtt_outtopic, (byte*)topic, strlen(topic), true);
    //_mqtt_last_out = topic;
    //allocString(&_mqtt_last_out, topic);
  }
}

void MyMqtt::publishLog(char *msg) {
  if(_enabled) {
    char msg1[512];
    sprintf(msg1,"%.2d.%.2d.%.4d %.2d:%.2d:%.2d  %s", mytm.tm_mday, mytm.tm_mon, mytm.tm_year, mytm.tm_hour, mytm.tm_min, mytm.tm_sec, msg);
    _mqtt_client.publish(_mqtt_log_topic, msg1);
  }
}

boolean MyMqtt::check(void) {
    boolean ret=false;
    if(_enabled) {
      if (!_mqtt_client.connected()) {
        DEBUG_PRINTLN("MQTT disconnected, reconnecting!");
        publishLog("MQTT disconnected, reconnecting!");
        init();
      }
      ret = _mqtt_client.loop();
    }
    return ret;
}


void MyMqtt::setEnabled(boolean enabled) {
  _enabled = enabled;
}


boolean MyMqtt::getEnabled(void) {
  return _enabled;
}

/**
   get state
*/
String MyMqtt::getState(void) {
  const static char server[] PROGMEM = "\n\nMQTT:\nServer=";
  const static char port[] PROGMEM = "\nPort=";
  const static char username[] PROGMEM = "\nUsername=";
  const static char password[] PROGMEM = "\nPassword=";
  const static char enabledyes[] PROGMEM = "\nEnabled=yes";
  const static char enabledno[] PROGMEM = "\nEnabled=no";
  const static char lastin[] PROGMEM = "\nlast_mqtt_in=";
  const static char lastout[] PROGMEM = "\nlast_mqtt_out=";
  const static char lastcolor[] PROGMEM = "\nlast_mqtt_color=";

  Helper::writeState(server,_mqtt_server);
  Helper::writeState(port,_mqtt_port);
  Helper::writeState(username,_mqtt_user);
  Helper::writeState(password,_mqtt_pw);
  if(_enabled)
    Helper::writeState(enabledyes);
  else
    Helper::writeState(enabledno);
/*
  Helper::writeState(lastin,_mqtt_last_in);
  Helper::writeState(lastout,_mqtt_last_out);
  Helper::writeState(lastcolor,_mqtt_last_color);
*/
  return "";
}

#endif    //WITH_MQTT
