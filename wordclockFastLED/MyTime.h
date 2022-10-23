/**
   MyTime.h
   @autor    Bruno Merz

   @version  1.0
   @created  11.10.2021

   Handle time

*/

#pragma once

#include "MySpiffs.h"

#if defined(ESP8266)
#include <coredecls.h>                  // settimeofday_cb()
#include <PolledTimeout.h>
#endif

#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval

class MyTime {
  public:
    MyTime(void);
    String getTime(void);
    void   getTime(MYTM *mt);
    String getDate(void);
    void   confTime(void);
    void   setTime(char *tms);
    
  private:
    time_t now;
    struct tm *lt;
    struct tm *gm;
};
