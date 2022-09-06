/**
   TimeStamp.cpp
   Class encapsulates a time stamps.

   @autor    Bruno Merz
   @version  1.0
   @created  30.12.2019
   @updated  08.01.2020

   Version history:
   V 1.0:  - Created.
   V 1.1:  - DEBUG modified into myDEBUG

*/

//#define myDEBUG
#include "Debug.h"

#include "TimeStamp.h"


TimeStamp::TimeStamp(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year) {
  set(minutes, hours, date, dayOfWeek, month, year);
}

TimeStamp::TimeStamp(TimeStamp* timestamp) {
  set(timestamp);
}

byte TimeStamp::getMinutes() {
  return _minutes;
}

unsigned int TimeStamp::getMinutesOfDay() {
  return _minutes + 60 * _hours;
}

unsigned int TimeStamp::getMinutesOfWeek1() {
  return getMinutesOfDay() + _dayOfWeek * 24 * 60;
}

unsigned int TimeStamp::getMinutesOfWeek() {
  return getMinutesOfWeek1() - 24 * 60;
}

unsigned long TimeStamp::getMinutesOfCentury() {
  /*
      Exact calculation of the past minutes of the current century up to the TimeStamp.
      This function is also suitable for time comparisons of several TimeStamps over years.
  */
  unsigned int days = 0;
  if (_year)
    days += (_year + 3) / 4 + _year * 365;
  for (byte i = 1; i <= _month - 1; i++) {
    days += getDaysOfMonth(i, _year);
  }
  days += _date - 1;
  return ( (unsigned long) days * 24 * 60 + getMinutesOfDay() );
}

byte TimeStamp::getHours() {
  return _hours;
}

byte TimeStamp::getDate() {
  return _date;
}

byte TimeStamp::getDayOfWeek() {
  return _dayOfWeek;
}

byte TimeStamp::getMonth() {
  return _month;
}

byte TimeStamp::getYear() {
  return _year;
}

void TimeStamp::setMinutes(byte minutes) {
  _minutes = minutes % 60;
}


void TimeStamp::setHours(byte hours) {
  _hours = hours % 24;
}

void TimeStamp::setDayOfWeek(byte dayOfWeek) {
  _dayOfWeek = dayOfWeek;
}

void TimeStamp::setDate(byte date, bool overflow) {
  _date = date;
  CheckDateValidity(overflow);
}

void TimeStamp::setMonth(byte month, bool overflow) {
  _month = month;
  CheckDateValidity(overflow);
}

void TimeStamp::setYear(byte year, bool overflow) {
  _year = year;
  CheckDateValidity(overflow);
}

void TimeStamp::set(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year, bool checkValidity) {
  setMinutes(minutes);
  setHours(hours);
  _date = date;
  _dayOfWeek = dayOfWeek;
  _month = month;
  _year = year;
  if (checkValidity)
    CheckDateValidity(true);
}

void TimeStamp::set(TimeStamp* timeStamp, bool checkValidity) {
  set(timeStamp->_minutes, timeStamp->_hours, timeStamp->_date, timeStamp->_dayOfWeek, timeStamp->_month, timeStamp->_year, checkValidity);
}

/**
   increment minutes.
*/
void TimeStamp::incMinutes(byte addMinutes) {
  setMinutes(_minutes + addMinutes);
}

/**
   increment hours.
*/
void TimeStamp::incHours(byte addHours) {
  setHours(_hours + addHours);
}

/**
   Adjust the TimeStamp by the specified number of hours incl. day, month and yeas

*/
void TimeStamp::addSubHoursOverflow(char addSubHours) {
  char sumHours = _hours + addSubHours;
  // sumHours < 0
  while (sumHours < 0) {
    sumHours += 24;
    _date--;
    if (!_date) {
      _month--;
      if (!_month) {
        _year--;
        _month = 12;
      }
      _date = getDaysOfMonth(_month, _year);
    }
  }
  // sumHours >= 24
  _date += sumHours / 24;

  _hours = sumHours % 24;
  CheckDateValidity(true);

  //    if (addSubHours != 0)
  //        timeeDateFromMinutesOfCentury(getMinutesOfCentury() + (int) addSubHours * 60);
}

/**
   increment date, month yearDatum, Monat, Jahr erhöhren.
*/
void TimeStamp::incDate(byte addDate, bool overflow) {
  setDate(_date + addDate, overflow);
}

void TimeStamp::incMonth(byte addMonth, bool overflow) {
  setMonth(_month + addMonth, overflow);
}

void TimeStamp::incYear(byte addYear, bool overflow) {
  setYear(_year + addYear, overflow);
}

/**
   return time as string
*/
char* TimeStamp::asString() {
  _cDateTime[0] = 0;
  char temp[5];

  // build the string...
  if (_hours < 10) {
    sprintf(temp, "0%d:", _hours);
    strncat(_cDateTime, temp, strlen(temp));
  } else {
    sprintf(temp, "%d:", _hours);
    strncat(_cDateTime, temp, strlen(temp));
  }

  if (_minutes < 10) {
    sprintf(temp, "0%d ", _minutes);
    strncat(_cDateTime, temp, strlen(temp));
  } else {
    sprintf(temp, "%d ", _minutes);
    strncat(_cDateTime, temp, strlen(temp));
  }

  if (_date < 10) {
    sprintf(temp, "0%d.", _date);
    strncat(_cDateTime, temp, strlen(temp));
  } else {
    sprintf(temp, "%d.", _date);
    strncat(_cDateTime, temp, strlen(temp));
  }

  if (_month < 10) {
    sprintf(temp, "0%d.", _month);
    strncat(_cDateTime, temp, strlen(temp));
  } else {
    sprintf(temp, "%d.", _month);
    strncat(_cDateTime, temp, strlen(temp));
  }

  sprintf(temp, "%d", _year);
  strncat(_cDateTime, temp, strlen(temp));

  return _cDateTime;
}

byte TimeStamp::getDaysOfMonth(byte month, byte year) {
  // Only valid for years from 2000 to 2099
  switch (month) {
    default: return 0;
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      return 31;
      break;
    case 2:
      if (year % 4)
        return 28;
      else
        return 29;
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      return 30;
      break;
  }
}

void TimeStamp::CalculateAndSetDayOfWeek() {
  // (Adopted) alghorithm by Schwerdtfeger
  // This alghorithm is only valid from 1st March 2000 to 31st December 2099
  byte g = _year;
  if (_month < 3) g--;
  byte e;
  switch (_month) {
    default: e = 0; break;
    case 2:
    case 6:  e = 3; break;
    case 3:
    case 11: e = 2; break;
    case 4:
    case 7:  e = 5; break;
    case 8:  e = 1; break;
    case 9:
    case 12: e = 4; break;
    case 10: e = 6; break;
  }
  byte w = (_date + e + g + g / 4) % 7;
  if (!w) w = 7;
  setDayOfWeek(w);
}

void TimeStamp::CheckDateValidity(bool overflow) {
  if (_month < 1)
    _month = 1;
  while (_month > 12) {
    _month -= 12;
    if (overflow) _year++;
  }

  if (_date < 1)
    _date = 1;
  while (_date > getDaysOfMonth(_month, _year)) {
    _date -= getDaysOfMonth(_month, _year);
    if (overflow) {
      _month++;
      while (_month > 12) {
        _month -= 12;
        _year++;
      }
    }
  }

  _year %= 100;

  CalculateAndSetDayOfWeek();
}
