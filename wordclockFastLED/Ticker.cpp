/**
   Ticker.cpp
   Class creates a ticker text on the led matrix.

   @autor    Bruno Merz
   @version  2.0
   @created  03.03.2018
   @updated  23.03.2021


*/

//#define myDEBUG
#include "Debug.h"

#include "Ticker.h"
#include "TickerAlphabet.h"
#include "Helper.h"

extern MyDisplay display;
extern MySpiffs myspiffs;
extern Effekte effekte;
extern IconRenderer iconRenderer;
extern MYTM mytm;


#ifdef WITH_AUDIO
extern MyAudio  myaudio;
#endif

/**
   Constructer with text which should be displayed

   @param  text: text to be displayed

*/

Ticker::Ticker(void) {
  _frequency     = 60;
  _loopCount     = -1;
  _effect_before = -1;
  _effect_after  = -1;
  _icon_before   = "";
  _icon_after    = "";
#ifdef WITH_AUDIO
  _audio_before  = "";
  _audio_after   = "";
#endif
  _color         = -1;
  _speed         = -1;
}


void Ticker::render(boolean test) {
  COLOR_T save_rgb = myspiffs.getRGBSetting(F("rgb"));
  int i;

  _tickerText  = myspiffs.getSetting(F("ticker_text"));
  if(!_tickerText.length())
    return;

  _icon_before = myspiffs.getSetting(F("SV"));
  _icon_after  = myspiffs.getSetting(F("SN"));

  if((i = myspiffs.getIntSetting(F("H"))) != -1)
    _frequency = i;
   if((i = myspiffs.getIntSetting(F("W"))) != -1)
    _loopCount = i;
   if((i = myspiffs.getIntSetting(F("MV"))) != -1)
    _effect_before = i;
   if((i = myspiffs.getIntSetting(F("MN"))) != -1)
    _effect_after = i;

   if((i = myspiffs.getIntSetting(F("G"))) != -1)
    _speed = i;

  _color = myspiffs.getRGBSetting(F("F"));

  DEBUG_PRINTLN("render: "+_tickerText+ ", minutes="+String(mytm.tm_min)+", Seconds="+String(mytm.tm_sec));
  if(test || (_frequency != -1 && !(mytm.tm_min % _frequency) && mytm.tm_sec < 5)) {

    if(_effect_before != -1)
      effekte.displayEffekt(_effect_before-1);
  
    if(_icon_before.length())
      iconRenderer.renderAndDisplay(_icon_before, 3000, true);

#ifdef WITH_AUDIO
    if(_audio_before.length())
      myaudio.play(_audio_before);
#endif


    display.setColor(_color);

    if(_speed == -1) {
      _speed = myspiffs.getIntSetting(F("ticker_speed"));
      if(_speed == -1)
        _speed = 120;
    }

    render(_loopCount==-1?1:_loopCount, _speed, _tickerText);


    display.setColor(save_rgb);

    
    if(_effect_after != -1)
      effekte.displayEffekt(_effect_after-1);
  
    if(_icon_after.length())
      iconRenderer.renderAndDisplay(_icon_after, 3000, true);
  }
}


void Ticker::render(int repeat, int tickerSpeed, String tickerText) {
  String tt=tickerText;
  String sVal;
  int    iVal;
  
  DEBUG_PRINTLN("render: "+tickerText+", repeat="+String(repeat));

  display.clear();

  if (!tt.length())
    return;

  if(!tt.endsWith("    "))
    tt += "    ";

  while (repeat--)
  {
    for (int i = 0; i < tt.length(); i++)
    {
      int zeichen = tt[i];
      //DEBUG_PRINTF("Ticker::render: zeichen=%d\n", zeichen);
      if (zeichen == 0xc2) {
        zeichen = tt[++i];
        switch(zeichen) {
          case 0xb0:
            zeichen = 0x7f; // °
            break;
        }
      }
      if (zeichen == 0xc3) {
        // 0xc384 = Ä
        zeichen = tt[++i];
        switch(zeichen) {
          case 0x84:
            zeichen = 0x5b; // Ä
            break;
          case 0x96:
            zeichen = 0x5c; // Ö
            break; 
          case 0x9c:
            zeichen = 0x5d; // Ü
            break;           
          case 0xa4:
            zeichen = 0x7b; // ä
            break;
          case 0xb6:
            zeichen = 0x7c; // ö
            break; 
          case 0xbc:
            zeichen = 0x7d; // ü
            break;
          case 0x9f:
            zeichen = 0x7e; // ß
            break;           
        }
      }
      if (zeichen < 0x20 | zeichen > 0x7f)
        zeichen = 0x20;
      int breite = pgm_read_byte(&(TickerZeichen[(char)zeichen - ' '][10]));
      //DEBUG_PRINTF("Ticker::render: breite=%d\n", breite);
      
      for (int j = 0; j < breite; j++) {
        for (int k = 0; k < 10; k++) {
          word bitArray = pgm_read_byte(&(TickerZeichen[(char)zeichen - ' '][k])); // 0b10101000 -> j=0
          //DEBUG_PRINTF("Ticker::render: bitArray=%#x\n", bitArray);
          bitArray >>= (7 - j);
          bitArray &= 1;
          if(bitArray)
            display.drawPixel(LED_COLS-1,k,_color);
          else
            display.clearPixel(LED_COLS-1,k);
        }
        display.show(tickerSpeed/2);
        display.shiftMatrix();
        display.show(tickerSpeed/2);
      }
      display.shiftMatrix();
      display.show(tickerSpeed/2);
    }
    for(uint16_t x=0; x < LED_COLS; x++) {
      display.shiftMatrix(); 
      display.show(tickerSpeed/2);
    }
  }
}

String  Ticker::getState(void) {
  const static char tt[] PROGMEM = "\n\nTicker:\ntickerText=";
  const static char fr[] PROGMEM = "\nfrequency=";
  const static char lc[] PROGMEM = "\nloopCount=";
  const static char eb[] PROGMEM = "\neffect_before=";
  const static char ea[] PROGMEM = "\neffect_after=";
  const static char ib[] PROGMEM = "\nicon_before=";
  const static char ia[] PROGMEM = "\nicon_after=";
  const static char co[] PROGMEM = "\ncolor=";
  const static char sp[] PROGMEM = "\nspeed=";
  
  Helper::writeState(tt,_tickerText);
  Helper::writeState(fr,_frequency);
  Helper::writeState(lc,_loopCount);
  Helper::writeState(eb,_effect_before);
  Helper::writeState(ea,_effect_after);
  Helper::writeState(ib,_icon_before);
  Helper::writeState(ia,_icon_after);
  Helper::writeState(co,_color);
  Helper::writeState(sp,_speed);
  
  return "";
}
