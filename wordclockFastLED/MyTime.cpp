/**
   MyTime.cpp
   @autor    Bruno Merz

   @version  1.0
   @created  11.10.2021

*/

//#define myDEBUG
#include "Debug.h"

#include "Configuration.h"

#include "MyTime.h"

extern MySpiffs myspiffs;
extern MYTM     mytm;
extern bool     ap_mode;

void time_is_set_scheduled() {    // This function is set as the callback when time data is retrieved
  // In this case we will print the new time to serial port, so the user can see it change (from 1970)
  
  time_t now1 = time(nullptr);
  time_t now2 = now1;
  DEBUG_PRINT("In time_is_set_scheduled: Current Time=");
  DEBUG_PRINTLN(ctime(&now1));
  tm *gm = gmtime(&now1);
  int8_t gm_hour = gm?gm->tm_hour:0;
  tm *lt = localtime(&now2);
  int8_t lt_hour = lt?lt->tm_hour:0;

  mytm.tm_tzoffset = lt_hour-gm_hour;
  DEBUG_PRINTF("lt_hour=%d, gm_hour=%d, tm_tzoffset=%d\n",lt_hour, gm_hour, mytm.tm_tzoffset);
}

MyTime::MyTime(void) {
  settimeofday_cb(time_is_set_scheduled);
  mytm.tm_lat = 0;
  mytm.tm_lat = 0;
  mytm.tm_ntpserver = "";
  mytm.tm_timezone = "";
}

void MyTime::confTime(void) {
  // This is where your time zone is set
  char *echr=NULL;
  if(!mytm.tm_lat) {
    mytm.tm_lat = myspiffs.getDoubleSetting(F("latitude"));
    mytm.tm_lon = myspiffs.getDoubleSetting(F("longitude"));
    mytm.tm_ntpserver = myspiffs.getSetting(F("ntp_server"));
    mytm.tm_timezone = myspiffs.getSetting(F("time_zone"));
  }
  // Check if old style timezone like "3600" is set
  int lng = strtol(mytm.tm_timezone.c_str(), &echr, 10);
  if(!*echr) {
    mytm.tm_timezone = "CET-1CEST,M3.5.0,M10.5.0/3";
    myspiffs.setSetting(F("time_zone"),mytm.tm_timezone);
    myspiffs.writeSettings(true);
  }
  DEBUG_PRINTF("confTime: tz=%s\n", mytm.tm_timezone.c_str());
  DEBUG_PRINTF("confTime: lng=%d\n",lng);
  DEBUG_PRINTF("confTime: echr=%c\n",*echr);
  DEBUG_PRINTF("confTime: TZ=%s, NTP=%s\n",mytm.tm_timezone.c_str(), mytm.tm_ntpserver.c_str());
  configTime(mytm.tm_timezone.c_str(), mytm.tm_ntpserver.c_str());
  myspiffs.writeLog(F("confTime: TZ="));
  myspiffs.writeLog((char *)mytm.tm_timezone.c_str(), false);
  myspiffs.writeLog(F(", NTP="), false);
  myspiffs.writeLog((char *)mytm.tm_ntpserver.c_str(), false);
  myspiffs.writeLog(F("\n"), false);
  delay(250);
}

String MyTime::getTime(void) {
  char  buffer[12];
  now = time(nullptr);
  lt = localtime(&now);
  
  strftime(buffer, sizeof(buffer), "%H:%M:%S", lt);
  String timeStamp(buffer);
  return timeStamp;
}

String MyTime::getDate(void) {
  char *buffer;
  now = time(nullptr);
  buffer = ctime(&now);
  String timeStamp(buffer);
  
  return timeStamp;
}


/**
   returns current time hours, minutes, seconds and day
*/
void MyTime::getTime(MYTM *mt) {
  now = time(nullptr);
  lt = localtime(&now);
  
  mt->tm_hour = lt->tm_hour;
  mt->tm_min  = lt->tm_min;
  mt->tm_sec  = lt->tm_sec;
  mt->tm_mday = lt->tm_mday;
  mt->tm_mon  = lt->tm_mon+1;
  mt->tm_year = lt->tm_year+1900;
  mt->tm_isdst= lt->tm_isdst;

  //DEBUG_PRINTF("MyTime.getTime: isdst=%d\n",mt->tm_isdst);
}


/**
   sets current time hours, minutes, seconds and day
*/
void MyTime::setTime(char *tms) {
  struct timeval tv;
  struct tm tm_act;
  if(ap_mode) {
    strptime(tms, "%Y-%m-%dT%H:%M:%S", &tm_act);
    time_t ts = mktime(&tm_act);
    tv.tv_sec = ts;
    configTime(0, 0, NULL, NULL, NULL);
    settimeofday(&tv,NULL);
    
    //DEBUG_PRINTF("setTime: %s\n",tms);
    //DEBUG_PRINTLN(getDate());
  }
}
