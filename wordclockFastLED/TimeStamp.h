/**
   TimeStamp.h
   Class encapsulates a time stamp.

   @autor    Bruno Merz
   @version  1.0
   @created  01.01.2020
   @updated  01.01.2020

   Version history:
   V 1.0:  - Created

*/

#pragma once

#include "Arduino.h"
#include <stdio.h>

class TimeStamp {
  public:
    // Initialize with date = 1 and month = 1 is neccessary for getMinutesOfCentury()
    TimeStamp(byte minutes = 0, byte hours = 0, byte date = 1, byte dayOfWeek = 0, byte month = 1, byte year = 0);
    TimeStamp(TimeStamp* timestamp);

    byte getMinutes();
    unsigned int getMinutesOfDay();
    unsigned int getMinutesOfWeek1();
    unsigned int getMinutesOfWeek();
    unsigned long getMinutesOfCentury();
    byte getHours();
    byte getDate();
    byte getDayOfWeek();
    byte getMonth();
    byte getYear();

    void setMinutes(byte minutes);
    void setHours(byte hours);
    void setDate(byte date, bool overflow = true);
    void setDayOfWeek(byte dayOfWeek);
    void setMonth(byte month, bool overflow = true);
    void setYear(byte year, bool overflow = true);
    void set(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year, bool checkValidity = true);
    void set(TimeStamp* timeStamp, bool checkValidity = true);

    void incMinutes(byte addMinutes = 1);
    void incHours(byte addHours = 1);
    void addSubHoursOverflow(char addSubHours = 0);
    void incDate(byte addDate = 1, bool overflow = false);
    void incMonth(byte addMonth = 1, bool overflow = false);
    void incYear(byte addYear = 1, bool overflow = false);

    char* asString();

  protected:
    byte getDaysOfMonth(byte month, byte year);
    void CalculateAndSetDayOfWeek();
    void CheckDateValidity(bool overflow = true);

    byte _minutes;
    byte _hours;

    byte _date;
    byte _dayOfWeek;
    byte _month;
    byte _year;

    char _cDateTime[17];
};
