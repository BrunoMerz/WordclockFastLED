/**
   Helper.cpp
   @autor    Bruno Merz

   @version  1.0
   @created  17.03.2021
   @updated  

  Helper class with static methods

*/


//#define myDEBUG
#include "Debug.h"
#include "Helper.h"
#include "MySpiffs.h"

extern MySpiffs myspiffs;
//extern uint8_t LedStripeDataPin;

void Helper::writeState(const char* text) {
  char buffer[100];
  if(text) {
    strcpy_P(buffer, text);
    myspiffs.writeFile(buffer);
  }
}


void Helper::writeState(const char* text, char *val) {
  writeState(text);
  if(val)
    myspiffs.writeFile(val);
}


/**
   LedBlink: for debugging purposes, if buildin led exists and can be used
*/
void Helper::LedBlink(int count, int ms)
{

}


void Helper::writeState(const char* text, String val) {
  writeState(text, (char *)val.c_str());
}


void Helper::writeState(const char* text, int val) {
  char tmp[10];
  itoa(val, tmp, 10);
  writeState(text, tmp);
}
