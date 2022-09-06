/**
   Renderer.h
   class renders words to the matrix

   @autor    Bruno Merz
   @version  2.0
   @created  30.12.2019
   @updated  09.04.2021


*/

#pragma once

#include "Configuration.h"
#include "MySpiffs.h"

#define LANGUAGE_DE_DE 0
#define LANGUAGE_DE_SW 1
#define LANGUAGE_DE_BA 2
#define LANGUAGE_DE_SA 3
#define LANGUAGE_CH    4
#define LANGUAGE_EN    5
#define LANGUAGE_EN_AM 6
#define LANGUAGE_FR    7
#define LANGUAGE_IT    8
#define LANGUAGE_NL    9
#define LANGUAGE_ES    10
#define LANGUAGE_PT    11
#define LANGUAGE_GR    12
#define LANGUAGE_COUNT 12

class Renderer {
  public:
    Renderer(void);

    void showTime(word matrix[16]);
    bool pollDisplayBlinkAfternoon();
    void clearScreenIfNeeded_DisplayOnBlinking(word matrix[16]);
    void setAfternoon(byte hours);

    void setMinutes(byte hours, byte minutes, word matrix[16]);
    void setCorners(byte minutes, word matrix[16]);

    void scrambleScreenBuffer(word matrix[16]);
    void clearScreenBuffer(word matrix[16]);
    void setAllScreenBuffer(word matrix[16]);

    void writeScreenBufferToMatrix(word matrix[16], bool singleLedDisplay);

  private:
    void setHours(byte hours, byte glatt_vor_nach, byte language, word matrix[16]);

    int       _last_minute;
    bool      _pulse;
    bool      _esist;
};
