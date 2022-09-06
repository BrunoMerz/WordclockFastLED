/**
   MyMqtt.h
   @autor    Bruno Merz

   @version  1.0
   @created  20.11.2020
   @updated  

   Version history:
   V 1.0:  - Created.
   
   MQTT to publish and subscribe some messages

*/

#pragma once

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "MySpiffs.h"
#include "Ticker.h"
#include "MyDisplay.h"
#include "Effekte.h"
#include "AdjustBrightness.h"


#define JSON_MQTT_COUNT 50
//#define MQTT_MAX_PACKET_SIZE 512
#define MQTT_MAX_RECONNECT_TRIES 8

/**
 * Class definition
 */
class MyMqtt {
  public:
    MyMqtt(void);
    boolean init(void);
    boolean check(void);
    boolean reconnect(void);
    String getState(void);
    void publish(void);
    void publish(char *topic);
    void publishLog(char *msg);
    boolean getEnabled(void);
    void setEnabled(boolean enabled);
    static void mqtt_callback(char* topic, byte* payload, unsigned int length);
    static void allocString(char **dest, String src);
  
  private:
    boolean _enabled;
    char* _mqtt_server;
    char* _mqtt_user;
    char* _mqtt_pw;
    int    _mqtt_port;
    int    _mqtt_reconnect_retries;

    char* _mqtt_clientid;
    char* _mqtt_hostname;
    char* _mqtt_will_topic;
    char* _mqtt_will_payload;
    char* _mqtt_intopic;
    char* _mqtt_outtopic;
    char* _mqtt_log_topic;
 };
