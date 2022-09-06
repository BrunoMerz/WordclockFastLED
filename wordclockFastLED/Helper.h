/**
   Helper.h
   @autor    Bruno Merz

   @version  1.0
   @created  17.03.2021
   @updated  

  Helper class with static methods

*/

#pragma once

class Helper {
  public:
    static void writeState(const char* text);
    static void writeState(const char* text, String val);
    static void writeState(const char* text, char *val);
    static void writeState(const char* text, int val);
    static void LedBlink(int count, int ms);
};
