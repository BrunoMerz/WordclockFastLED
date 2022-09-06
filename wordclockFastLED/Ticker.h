/**
   Ticker.h
   Klasse fuer die Laufschrift.

   @autor    Bruno Merz
   @version  1.0
   @created  03.03.2018
   @update   03.03.2018

   Version history:
   V 1.0:  - Created.

*/

#pragma once

#include "MyDisplay.h"
#include "MySpiffs.h"
#include "Effekte.h"
#include "MyAudio.h"

class Ticker {
  public:
    Ticker(void);

    void render(boolean test=false);
    void render(int repeat, int speed, String tt);
    String  getState(void);

  private:
    String _tickerText;
    int    _effect_before;
    int    _effect_after;
    String _icon_before;
    String _icon_after;
#ifdef WITH_AUDIO
    String _audio_before;
    String _audio_after;
#endif
    int    _frequency;
    int    _loopCount;
    int    _speed;
    COLOR_T _color;
};
