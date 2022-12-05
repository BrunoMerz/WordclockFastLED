/**
   Effekte.cpp
   Class outputs nice pattern on the clock

   @autor    B. Merz
   @version  2.0
   @created  26.10.2018
   @updated  11.04.2021

*/

//#define myDEBUG
#include "Debug.h"

#include "Effekte.h"

extern MyDisplay display;
extern Ticker ticker;
extern MySpiffs myspiffs;
extern Renderer renderer;
extern MYTM mytm;
extern MyMqtt mymqtt;

extern word matrix[16];

bool endlessloop;

word MatrixArray4[6][10] PROGMEM = // 6 Zeilen, 10 Spalten
{
  { 0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000010000000000,
    0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000
  },

  { 0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000111000000000, 0b0000111000000000,
    0b0000111000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000
  },

  { 0b0000000000000000, 0b0000000000000000, 0b0001111100000000, 0b0001111100000000, 0b0001111100000000,
    0b0001111100000000, 0b0001111100000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000
  },

  { 0b0000000000000000, 0b0011111110000000, 0b0011111110000000, 0b0011111110000000, 0b0011111110000000,
    0b0011111110000000, 0b0011111110000000, 0b0011111110000000, 0b0000000000000000, 0b0000000000000000
  },

  { 0b0111111111000000, 0b0111111111000000, 0b0111111111000000, 0b0111111111000000, 0b0111111111000000,
    0b0111111111000000, 0b0111111111000000, 0b0111111111000000, 0b0111111111000000, 0b0000000000000000
  },

  { 0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000,
    0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000
  }
};

word MatrixArray5[9][10] PROGMEM = // 9 zeilen, 10 Spalten
{
  { 0b1111111111100000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
    0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b0111111111000000, 0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
    0b0000000000000000, 0b0000000000000000, 0b0000000000000000, 0b0111111111000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b0111111111000000, 0b0011111110000000, 0b0000000000000000, 0b0000000000000000,
    0b0000000000000000, 0b0000000000000000, 0b0011111110000000, 0b0111111111000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b0111111111000000, 0b0011111110000000, 0b0001111100000000, 0b0000000000000000,
    0b0000000000000000, 0b0001111100000000, 0b0011111110000000, 0b0111111111000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b0111111111000000, 0b0011111110000000, 0b0001111100000000, 0b0000111000000000,
    0b0000111000000000, 0b0001111100000000, 0b0011111110000000, 0b0111111111000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b0111111111000000, 0b0011111110000000, 0b0001111100000000, 0b0001111100000000,
    0b0001111100000000, 0b0001111100000000, 0b0011111110000000, 0b0111111111000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b0111111111000000, 0b0011111110000000, 0b0011111110000000, 0b0011111110000000,
    0b0011111110000000, 0b0011111110000000, 0b0011111110000000, 0b0111111111000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b0111111111000000, 0b0111111111000000, 0b0111111111000000, 0b0111111111000000,
    0b0111111111000000, 0b0111111111000000, 0b0111111111000000, 0b0111111111000000, 0b1111111111100000
  },

  { 0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000,
    0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000, 0b1111111111100000
  }
};


void randomizeArray(byte *array, int n) {
  // ramdomize array
  for(int i=0;i<n;i++) {
    int j = random(0,n);
    byte tmp = *(array+i);
    *(array+i) = *(array+j);
    *(array+j) = tmp;
  }
}


Effekte::Effekte(void) {
  _display_effect = true;
  for(int i=0;i<sizeof(_effectArray)/sizeof(byte);i++)
    _effectArray[i]=0;
}


/**
   Displays one of nice available patterns
*/
void Effekte::displayEffekt(int effectNo) {
  int _effectNo;
  _effectCount=0;
  _effect = myspiffs.getBoolSetting(F("effects"));
  char sequence[80];
  int lng = myspiffs.getCharSetting(F("effect_sequence"),sequence,sizeof(sequence));
  if(lng) {
    char *p = strtok(sequence,", ;\t");
    while(p && _effectCount<24) {
      _effectArray[_effectCount++] = atoi(p);
      p = strtok(NULL,", ;\t");
    }
  }

  if(!_effectCount)
    _effectCount = 1;

  DEBUG_PRINTF("displayEffekt: hours=%d, minutes=%d, _effectCount=%d, effectNo=%d\n", mytm.tm_hour, mytm.tm_min, _effectCount, effectNo);

  renderer.clearScreenBuffer(matrix);
  renderer.setMinutes(mytm.tm_hour, mytm.tm_min, matrix);
  renderer.setCorners(mytm.tm_min, matrix);
  if(effectNo >= 0) {
    _effectNo = effectNo;
  } else {
    _effectNo = _effectArray[mytm.tm_hour % _effectCount]-1;
    if(_effectNo == -1)
      _effectNo = random(0, 25);
  }

  DEBUG_PRINTF("displayEffekt: minutes=%d, _effect=%d, _display_effect=%d, _effectNo=%d\n", mytm.tm_min, _effect, _display_effect, _effectNo);

  if (!mytm.tm_min || effectNo >= 0)
  {
    if ((_effect && _display_effect) || effectNo >= 0)
    {
      myspiffs.writeLog(F("effectNo="));
      myspiffs.writeLog(_effectNo, false);
      myspiffs.writeLog(F("\n"), false);
      _display_effect = false;
      _showTime=false;
        switch (_effectNo) {
          case 0:
            displayEffect1();
            break;
          case 1:
            displayEffect2();
            break;
          case 2:
            displayEffect3();
            break;
          case 3:
            displayEffect4();
            break;
          case 4:
            displayEffect5();
            break;
          case 5:
            displayEffect6();
            break;
          case 6:
            displayEffect7();
            break;
          case 7:
            displayEffect8();
            break;
          case 8:
            displayEffect9();
            break;
          case 9:
            displayEffect10();
            break;
          case 10:
            displayEffect11();
            break;
          case 11:
            displayEffect12();
            break;
          case 12:
            displayEffect13();
            break;
          case 13:
            displayEffect14();
            break;
          case 14:
            displayEffect15();
            break;
          case 15:
            displayEffect16();
            break;
          case 16:
            displayEffect17();
            break;
          case 17:
            displayEffect18();
            break;
          case 18:
            displayEffect19();
            break;
          case 19:
            displayEffect20();
            break;
          case 20:
            displayEffect21();
            break;
          case 21:
            displayEffect22();
            break;
          case 22:
            displayEffect23();
            break;
          case 23:
            displayEffect24();
            break;
          case 24:
            displayEffect25();
            break;
          case 25:
            displayEffect26();
            break;
          case 26:
            displayEffect27();
            break;
           case 27:
            displayEffect28();
            break;
        }
    }
  } else
    _display_effect = true;
}

/**
   Display Effect 1 - Spirale
*/
void Effekte::displayEffect1(void) {
  long rnd = random(1,360);

  display.clear();
  for(int x,y,z=0; z<2; z++) {
    x=5;
    y=4;
    turnLedOnOff(x, y, z, DELAY30, true, rnd, 0);
    for (int i = 0, d = 1; i < 5; i++)
    {
      for (int j = 0; j < d; j++)
        turnLedOnOff(++x, y, z, DELAY30, true, rnd, 0);
        
      for (int j = 0; j < d; j++)
        turnLedOnOff(x, ++y, z, DELAY30, true, rnd, 0);
        
      d++;
      for (int j = 0; j < d; j++)
        turnLedOnOff(--x, y, z, DELAY30, true, rnd, 0);
        
      for (int j = 0; j < d && y > 0; j++)
        turnLedOnOff(x, --y, z, DELAY30, true, rnd, 0);
        
      d++;
    }
    _showTime=true;
  }
  delay(DELAY500);
}

/**
   Display Effect 2 - Aussen nach innen
*/
void Effekte::displayEffect2(void) {
  long rnd = random(0,360);

  display.clear();
  for(int z=0; z<2; z++) {
    for (int x,y,lc = 0; lc < 5; lc++)
    {
      y = lc;
      for (x = lc; x < (11 - lc); x++)
      {
        turnLedOnOff(x, y, z, DELAY30, true, rnd, 0);
        turnLedOnOff(x, 9-y, z, DELAY30, true, rnd, 0);
      }
      x = lc;
      for (y = lc; y < (10 - lc); y++)
      {
        turnLedOnOff(x, y, z, DELAY30, true, rnd, 0);
        turnLedOnOff(10-x, y, z, DELAY30, true, rnd, 0);
      }
    }
    _showTime=true;
  }
    delay(DELAY500);
}

/**
   Display Effect 3 - vertical line from left to right and vice versa
*/
void Effekte::displayEffect3(void) {
  long rnd = random(0,360);
  uint8_t cnt=6;
  
  display.clear();
  for(int z=0; z<cnt; z++) {
    for(int x=0; x<11; x++) {
      for(int y=0; y<10; y++) {
        if(x)
          turnLedOnOff(x-1, y, LEDOFF, 0, HIDELED, rnd, 0);
        turnLedOnOff(x, y, LEDON, 0, HIDELED, rnd, 0);
      }
      display.show(DELAY50);
    }
    
    if(z==cnt-1)
      _showTime=true;

    for(int x=10; x>=0; x--) {
      for(int y=0; y<10; y++) {
        turnLedOnOff(x, y, LEDOFF, 0, HIDELED, rnd, 0);
        if(x>0)
          turnLedOnOff(x-1, y, LEDON, 0, HIDELED, rnd, 0);
      }
      display.show(DELAY50);
    }
    
  }
  delay(DELAY500);
}

/**
   Display Effect 4 - Quadrate von innen nach außen / Matthias
*/
void Effekte::displayEffect4(void) {
  long rnd = random(0,360);
  uint8_t cnt=6;
  display.clear();

  for(int z=0; z<cnt; z++) {
    for (int i = 0; i < 6; i++)
    {
      for(int y=0; y<10; y++) {
        for(int x=0; x<11; x++) {
          word pattern = pgm_read_word(&(MatrixArray4[i][y]));
          if(pattern & (0b1000000000000000 >> x))
            turnLedOnOff(x, y, z%2, DELAY0, HIDELED, rnd, 0);
        }
      }
      display.show(DELAY200+(z*DELAY50));
    }
    if(z==cnt-2)
      _showTime=true;
  }
  delay(DELAY500);
}

/**
   Display Effect 5 - Dreiecke / Bruno
*/
void Effekte::displayEffect5(void) {
  long rnd = random(0,360);
  uint8_t cnt=6;

  display.clear();

  for(int z=0; z<cnt; z++) {
    for (int i = 0; i < 9; i++)
    {
      for(int y=0; y<10; y++) {
        for(int x=0; x<11; x++) {
          word pattern = pgm_read_word(&(MatrixArray5[i][y]));
          if(pattern & (0b1000000000000000 >> x))
            turnLedOnOff(x, y, z%2, DELAY0, HIDELED, rnd, 0);
        }
      }
      display.show(DELAY150);
    }
    if(z==cnt-2)
      _showTime=true;
  }
  delay(DELAY500);
}


/**
   Display Effect 6 - Lorenz
*/
void Effekte::displayEffect6(void) {
  long rnd = random(0,360);

  display.clear();
  for(int z=0; z<2; z++) {
    for(int i=0; i<11; i++)
    {
      for(int j=0; j<i; j++)
        turnLedOnOff(i, j, z, DELAY30, SHOWLED, rnd, 0);
      for(int j=0; j<=i; j++)
        turnLedOnOff(j, i==10?9:i, z, DELAY30, SHOWLED, rnd, 0);
    }
    _showTime=true;
  }
  delay(DELAY500);
}


/**
   Display Effect 7 - Matthias
*/
void Effekte::displayEffect7(void) {
  long rnd = random(0,360);

  display.clear();
  for (int z = 0; z < 2; z++) {
    for (int x = 0; x < 11; x++) {
      for (int y = 0; y < 9; y += 2)
        turnLedOnOff(x, y, z, DELAY0, HIDELED, rnd, 0);
      display.show();
      delay(DELAY100);
      for (int y = 1; y < 10; y += 2)
        turnLedOnOff(10-x, y, z, DELAY0, HIDELED, rnd, 0);
      display.show();
      delay(DELAY100);
    }
    _showTime=true;
  }
  delay(DELAY500);
}

/**
   Display Effect 8 - Bruno
*/
void Effekte::displayEffect8(void) {
  long rnd = random(0,360);

  display.clear();
  for (int x = 0; x < 11; x++)
  {
    for (int y = 0; y < 10; y++) {
      turnLedOnOff(x, y, LEDON, DELAY0, HIDELED, rnd, 0);
    }
    display.show();
    delay(DELAY100);
  }
  
  for (int x = 10; x >= 0; x--)
  {
    for (int y = 0; y < 10; y++) {
      turnLedOnOff(x, y, LEDOFF, DELAY0, HIDELED, rnd, 0);
    }
    display.show();
    delay(DELAY100);
  }

  for (int y = 0; y < 10; y++)
  {
    for (int x = 0; x < 11; x++) {
      turnLedOnOff(x, y, LEDON, DELAY0, HIDELED, rnd, 0);
    }
     display.show();
    delay(DELAY100);
  }

  _showTime=true;
  for (int y = 9; y >= 0; y--)
  {
    for (int x = 0; x < 11; x++) {
      turnLedOnOff(x, y, LEDOFF, DELAY0, HIDELED, rnd, 0);
    }
    display.show();
    delay(DELAY100);
  }
  delay(DELAY500);
}


/**
   Display Effect 9 - Random
*/
void Effekte::displayEffect9(void) {
  long rnd = random(0,360);

  display.clear();

  // set randomly some LED's
  for (int i = 0; i < 200; i++) {
    int x = random(0, 11);
    int y = random(0, 10);
    turnLedOnOff(x, y, LEDON, DELAY15, SHOWLED, rnd, 0);
  }
  // set LED's which are not turned on so far
  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 11; x++) {
      turnLedOnOff(x, y, LEDON, 0, SHOWLED, rnd, 0);
    }
  }

  // Clear all LEDs
  _showTime=true;
  for (int i = 0; i < 200; i++) {
    int x = random(0, 11);
    int y = random(0, 10);
    turnLedOnOff(x, y, LEDOFF, DELAY15, SHOWLED, rnd, 0);
  }

  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 11; x++) {
      turnLedOnOff(x, y, LEDOFF, 0, SHOWLED, rnd, 0);
    }
  }
  delay(DELAY500);
  //display.clear();
}


/**
   Display Effect 10, fill diagonal from top/left to bottom/right
*/
void Effekte::displayEffect10(void) {
  long rnd = random(0,360);

  display.clear();
  for(int z=0; z<2; z++) {
    for(int i=0;i<10;i++) {
      for(int x=0,y=i;x<11 && y>=0;x++,y--) {
        turnLedOnOff(x, y, z, DELAY30, true, rnd, 0);
      }
    }
    
    for(int i=1;i<11;i++) {
      for(int x=i,y=9;x<11 && y>=0;x++,y--) {
        turnLedOnOff(x, y, z, DELAY30, true, rnd, 0);
      }
    }
    _showTime=true;
  }
  delay(DELAY500);
}


/**
   Display Effect 11, horizontal line from top to bottom and vice versa
*/
void Effekte::displayEffect11(void) {
  long rnd = random(0,360);
  uint8_t cnt=6;

  display.clear();
  for(int z=0; z<cnt; z++) {
    for(int y=0; y<10; y++) {
      for(int x=0; x<11; x++) {
        if(y)
          turnLedOnOff(x, y-1, LEDOFF, 0, HIDELED, rnd, 0);
        turnLedOnOff(x, y, LEDON, 0, HIDELED, rnd, 0);
      }
      display.show();
      delay(DELAY50);
    }

    if(z==cnt-1)
      _showTime=true;

    for(int y=9; y>=0; y--) {
      for(int x=0; x<11; x++) {
        turnLedOnOff(x, y, LEDOFF, 0, HIDELED, rnd, 0);
        if(y>0)
          turnLedOnOff(x, y-1, LEDON, 0, HIDELED, rnd, 0);
      }
      display.show();
      delay(DELAY50);
    }
   
  }
  delay(DELAY500);
}


/**
   Display Effect 12, vertical/horizontal line from left/top to right/bottom and vice versa
*/
void Effekte::displayEffect12(void) {
  long rnd = random(0,360);
  uint8_t cnt=6;

  display.clear();
  for(int z=0; z<cnt; z++) {
    for(int y=0; y<10; y++) {
      for(int x=0; x<10; x++) {
        // horizontal line
        if(y)
          turnLedOnOff(x, y-1, LEDOFF, 0, HIDELED, rnd, 0);
        turnLedOnOff(x, y, LEDON, 0, HIDELED, rnd, 0);
        // vertical line
        if(y)
          turnLedOnOff(y-1, x, LEDOFF, 0, HIDELED, rnd, 0);
        turnLedOnOff(y, x, LEDON, 0, HIDELED, rnd, 0);
      }
      display.show();
      delay(DELAY50);
    }

    if(z==cnt-1)
      _showTime=true;

    for(int y=9; y>=0; y--) {
      for(int x=0; x<10; x++) {
        // horizontal line
        turnLedOnOff(x, y, LEDOFF, 0, HIDELED, rnd, 0);
        if(y>0)
          turnLedOnOff(x, y-1, LEDON, 0, HIDELED, rnd, 0);
        // vertical line
        turnLedOnOff(y+1, x, LEDOFF, 0, HIDELED, rnd, 0);
        turnLedOnOff(y, x, LEDON, 0, HIDELED, rnd, 0);
      }
      display.show();
      delay(DELAY50);
    }
    for(int y=0; y<10; y++) {
      turnLedOnOff(0, y, LEDOFF, 0, HIDELED, rnd, 0);
    }
    display.show();
  }
  delay(DELAY500);
}


/**
   Display Effect 13, Raindrops
*/
void Effekte::displayEffect13(void) {
  long rnd = random(0,360);

  _showTime=false;
  for(int y1=0;y1<10;y1++) {  // Füllen einer Spalte von oben nach unten, Spaltenreihenfolge zufällig
    for(int y2=0;y2<10-y1;y2++) {
      for(byte col=0;col<11;col++){
        if(y2)
          turnLedOnOff(col, y2-1, LEDOFF, 0, false, rnd, 0);
        turnLedOnOff(col, y2, LEDON, DELAY0, true, rnd, 0);
      }
      delay(18);
    }
  }
  delay(DELAY200);

  for(int y1=9;y1>=0;y1--) {
    for(int y2=y1;y2<10;y2++) {
      if(y1==0)
        _showTime=true;
      for(byte col=0;col<11;col++){
        if(y2<9)
          turnLedOnOff(col, y2+1, LEDON, DELAY0, false, rnd, 0);
        turnLedOnOff(col, y2, LEDOFF, 0, true, rnd, 0);
      }
      delay(18);
    }
  }

  delay(DELAY200);
}


/**
   Display Effect 14, Raindrops 2
*/
void Effekte::displayEffect14(void) {
  byte cols[11]={0,1,2,3,4,5,6,7,8,9,10};

  long rnd = random(0,360);
  
  display.clear();

  _showTime=false;
  randomizeArray(cols, 11);
  for(int x=0;x<11;x++) {
    byte col=cols[x];
    for(int y1=0;y1<10;y1++) {
      turnLedOnOff(col, y1, LEDON, DELAY30, true, rnd, 0);
    }
  }
  delay(DELAY500);
  _showTime=true;
  randomizeArray(cols, 11);
  for(int x=0;x<11;x++) {
    byte col=cols[x];
    for(int y1=9;y1>=0;y1--) {
      turnLedOnOff(col, y1, LEDOFF, DELAY30, true, rnd, 0);
    }
  } 
  delay(DELAY500);
/*
  long rnd = random(0,360);

  display.clear();

  _showTime=false;
  for(int y1=0;y1<10;y1++) {
    for(int y2=0;y2<10-y1;y2++) {
      for(int x=0;x<11;x++) {
        if(y2)
          turnLedOnOff(x, y2-1, LEDOFF, 0, false, rnd, 0);
        turnLedOnOff(x, y2, LEDON, 0, false, rnd, 0);
      }
      display.show();
      delay(DELAY100+(2*y1));
    }
  }
  delay(DELAY500);

  for(int y1=0;y1<10;y1++) {
    for(int y2=y1;y2>=0;y2--) {
      for(int x=0;x<11;x++) {
        if(y1==9)
          _showTime=true;
        turnLedOnOff(x, y2, LEDOFF, 0, false, rnd, 0);
        if(y2)
          turnLedOnOff(x, y2-1, LEDON, 0, false, rnd, 0);
      }
      display.show();
      delay(DELAY100+(2*y1));
    }
  } 

  delay(DELAY500);
*/
}


/**
   Display Effect 15, Raindrops (left/right)
*/
void Effekte::displayEffect15(void) {
  long rnd = random(0,360);

  _showTime=false;
  for(int x1=0;x1<11;x1++) {
    for(int x2=0;x2<11-x1;x2++) {
      for(byte row=0;row<10;row++) {
        if(x2)
          turnLedOnOff(x2-1, row, LEDOFF, 0, false, rnd, 0);
        turnLedOnOff(x2, row, LEDON, DELAY0, true, rnd, 0);
      }
      delay(18);
    }
  }

  delay(DELAY500);

  for(int x1=10;x1>=0;x1--) {
    for(int x2=x1;x2<11;x2++) {
      if(x1==0)
        _showTime=true;

      for(byte row=0;row<10;row++) {
       if(x2<10)
          turnLedOnOff(x2+1, row, LEDON, DELAY0, false, rnd, 0);
        turnLedOnOff(x2, row, LEDOFF, 0, true, rnd, 0);
      }
      delay(18);
    }
  }

  delay(DELAY500);
}


/**
   Display Effect 16, slider horizontal
*/
void Effekte::displayEffect16(void) {
  long rnd = random(0,360);

  _showTime=false;
  display.clear();
  for(int z=0; z<2; z++) {
    for(int x=0;x<16;x++){
      for(int y=0;y<10;y+=2) {
        if(x<11) {
          turnLedOnOff(x, y, LEDON, 0, true, rnd, 0);
          turnLedOnOff(10-x, y+1, LEDON, 0, true, rnd, 0);
        }
        if(x>4) {
          turnLedOnOff(x-5, y, LEDOFF, 0, true, rnd, 0);
          turnLedOnOff(15-x, y+1, LEDOFF, 0, true, rnd, 0);
        }
      }
      delay(DELAY50);
    }
    _showTime=true;
  }
  delay(DELAY500);
}


/**
   Display Effect 17, slider vertical
*/
void Effekte::displayEffect17(void) {
  long rnd = random(0,360);

  _showTime=false;
  display.clear();
  for(int z=0; z<2; z++) {
    for(int y=0;y<15;y++){
      for(int x=0;x<11;x+=2) {
        if(y<10) {
          turnLedOnOff(x, y, LEDON, 0, true, rnd, 0);
          if(x<10)
            turnLedOnOff(x+1, 9-y, LEDON, 0, true, rnd, 0);
        }
        if(y>4) {
          turnLedOnOff(x, y-5, LEDOFF, 0, true, rnd, 0);
          if(x<10)
            turnLedOnOff(x+1, 14-y, LEDOFF, 0, true, rnd, 0);
        }
      }
      delay(DELAY50);
    }
    _showTime=true;
  }
  delay(DELAY500);
}


/**
   Display Effect 18, color from left to right
*/
void Effekte::displayEffect18(void) {
  _showTime=false;
  display.clear();
  float c=0;
  for(int z=0; z<72; z++) {
    for(int x=0; x<11; x++) {
      CHSV hsv = display.getHsvFromDegRnd(0, c);
      for(int y=0;y<10;y++){
        turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
      }
    }
    c+=3.5;
    display.show();
    delay(DELAY80);
  }
  
   _showTime=true;
   for(int x=0;x<11;x++) {
     for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDOFF, 0, false, 0, 0);
    }
    display.show(DELAY30);
  }
  delay(DELAY500);
}


/**
   Display Effect 19, color rotate
*/
void Effekte::displayEffect19(void) {
  _showTime=false;
  float deg;
  CHSV hsv;

  // turn all LEDs on
  for(int x=0; x<11; x++) {
    for(int y=0; y<10; y++) {
      deg = display.getDegree(y, x);
      hsv = display.getHsvFromDegRnd(0, deg);
      turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
    }
  }
  display.show();
  // rotate color 144 times
  for(int z=0;z<144;z++) {
    for(int x=0; x<11; x++) {
      for(int y=0; y<10; y++) {
        deg = display.getDegree(y, x);
        hsv = display.getHsvFromDegRnd(z*10, deg);
        turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
       }
    }
    display.show(DELAY30);
  }

  // clear all LEDs
  _showTime=true;
  for(int x=0;x<11;x++) {
    for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDOFF, 0, false, 0, 0);
    }
    display.show(DELAY50);
  }
  delay(DELAY500);
}


/**
   Display Effect 20, sort color matrix
*/
void Effekte::displayEffect20(void) {
  float deg;
  CHSV hsv;

  long rnd = random(0,360);

  _showTime=false;
  
  for(int x=0; x<11; x++) {
    for(int y=0; y<10; y++) {
      deg = random(0,360);
      hsv = display.getHsvFromDegRnd(0, deg);
      turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
    }
  }
  display.show();
  
  // set randomly some LED's
  for (int i = 0; i < 600; i++) {
    int x = random(0, 11);
    int y = random(0, 10);
    turnLedOnOff(x, y, LEDON, DELAY10, SHOWLED, rnd, 0);
  }
  // set LED's which are not turned on so far
  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 11; x++) {
      turnLedOnOff(x, y, LEDON, 0, SHOWLED, rnd, 0);
    }
  }
  delay(DELAY500*3);
  // clear all LEDs
  _showTime=true;
  for(int x=0;x<11;x++) {
    for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDOFF, 0, false, 0, 0);
    }
    display.show();
    delay(DELAY30);
  }
  delay(DELAY500);
}


/**
   Display Effect 21, pump color
*/
void Effekte::displayEffect21(void) {
  CHSV hsv;

  _showTime=false;
  display.clear();
 
  uint8_t maxb = myspiffs.getIntSetting(F("brightness_to"));
  display.setBrightness(2);
  
  hsv = CHSV(random(0,256),255,255);
  for(int x=0; x<11; x++) {
    for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
    }
  }
  display.show(DELAY50);
 
  for(int br=1; br<maxb; br++) {
    display.setBrightness(br);
    display.show(200);
  }

  for(int br=maxb; br>0; br--) {
    display.setBrightness(br);
    display.show(200);
  }

  display.setBrightness(0);
  // clear all LEDs
  _showTime=true;
  for(int x=0;x<11;x++) {
    for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDOFF, 0, false, 0, 0);
    }
    display.show(DELAY50);
  }
  delay(DELAY500);
}


/**
   Display Effect 22, worm top/down
*/
void Effekte::displayEffect22(void) {
  long rnd = random(0,360);

  _showTime=false;
  display.clear();
  for(int z=0; z<2; z++) {
    int inc=1;
    int x=0;
    int x1=-1;
    int y=0;
    int i=0;
    while(i++ < 110) {
      turnLedOnOff(x, y, LEDON, 0, true, rnd, 0);
      if(inc == 1) {
        if(x>4)
          turnLedOnOff(x-5, y, LEDOFF, 0, true, rnd, 0);
        if(x1>=0)
          turnLedOnOff(x1--, y-1, LEDOFF, 0, true, rnd, 0);
      }

      if(inc == -1) {
        if(x<7)
          turnLedOnOff(x+4, y, LEDOFF, 0, true, rnd, 0);
        if(x1<11)
          turnLedOnOff(x1++, y-1, LEDOFF, 0, true, rnd, 0);
      }
      x += inc;
      if(x==11) {
        inc *= -1;
        y++;  
        x=10;
        x1=6;
      } else if(x==-1) {
        inc *= -1;
        x=0;
        y++;
        x1=4;
      }
      delay(DELAY30);
    }
    for(x=4; x>=0; x--) {
      turnLedOnOff(x, 9, LEDOFF, 0, true, rnd, 0);
      delay(DELAY50);
    }
      
    _showTime=true;
  }
  delay(DELAY500);  
}


/**
   Display Effect 23, worm left/right
*/
void Effekte::displayEffect23(void) {
  long rnd = random(0,360);

  _showTime=false;
  display.clear();
  for(int z=0; z<2; z++) {
    int inc=1;
    int x=0;
    int y1=-1;
    int y=0;
    int i=0;
    while(i++ < 110) {
      turnLedOnOff(x, y, LEDON, 0, true, rnd, 0);
      if(inc == 1) {
        if(y>4)
          turnLedOnOff(x, y-5, LEDOFF, 0, true, rnd, 0);
        if(y1>=0)
          turnLedOnOff(x-1, y1--, LEDOFF, 0, true, rnd, 0);
      }

      if(inc == -1) {
        if(y<6)
          turnLedOnOff(x, y+4, LEDOFF, 0, true, rnd, 0);
        if(y1<10)
          turnLedOnOff(x-1, y1++, LEDOFF, 0, true, rnd, 0);
      }
      y += inc;
      if(y==10) {
        inc *= -1;
        x++;  
        y=9;
        y1=5;
      } else if(y==-1) {
        inc *= -1;
        y=0;
        x++;
        y1=4;
      }
      delay(DELAY30);
    }
    for(y=5; y<10; y++) {
      turnLedOnOff(10, y, LEDOFF, 0, true, rnd, 0);
      delay(DELAY50);
    }
      
    _showTime=true;
  }
  delay(DELAY500);  
  
}


/**
   Display Effect 24
*/
void Effekte::displayEffect24(void) {
  //COLOR_T rgb;
  int arr[11]={10,30,80,40,20,5,20,40,80,30,5};
  
  _showTime=false;
  display.clear();
  
  long rnd=random(0,255);

  //rgb = display.getColorHSV(rnd*182.04, 255, 3);
  CHSV hsv(rnd,255,3);
  for(int x=0; x<11; x++) {
    for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
    }
  }
  display.show();
  //delay(DELAY100);

  for(int z=0; z<40; z++) {
    for(int x=0; x<11; x++) {
      //rgb = display.getColorHSV(rnd*182.04, 255, 3+arr[x]);
      hsv = CHSV(rnd, 255, 3+arr[x]);
      for(int y=0; y<10; y++) {
        turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
      }
      display.show();
    }
    // shift array
    int tmp=arr[0];
    for(int i=1; i<11; i++)
      arr[i-1]=arr[i];
    arr[10]=tmp;
  }

  delay(DELAY500*3);

  // clear all LEDs
  _showTime=true;
  for(int x=0;x<11;x++) {
    for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDOFF, 0, false, 0, 0);
    }
    display.show();
    delay(DELAY30);
  }
  delay(DELAY500);
}


/**
   Display Effect 25, Feuerwerk
*/
void Effekte::displayEffect25(void) {
  COLOR_T rgb=0xffffff;
  int br;
  
  int arr[68][2]={
    {5,9},
    {5,8},
    {5,7},
    {5,6},
    {5,5},
    {5,4},
    {5,3},
    {6,2},
    {6,1},
    {6,0},
  
    {5,9},
    {5,8},
    {4,7},
    {4,6},
    {4,5},
    {3,4},
    {2,3},
    {2,2},
    {1,1},
    {0,0},
  
    {5,9},
    {6,8},
    {7,7},
    {8,7},
    {9,7},
    {10,7},
  
    {5,9},
    {5,8},
    {5,7},
    {5,6},
    {5,5},
    {5,4},
    {4,3},
    {4,2},
    {3,1},
    {3,0},
  
    {5,9},
    {4,8},
    {3,7},
    {2,7},
    {1,7},
    {0,7},
  
    {5,9},
    {5,8},
    {5,7},
    {5,6},
    {6,5},
    {6,4},
    {7,3},
    {7,2},
    {8,1},
    {9,1},
    {10,0},
  
    {5,9},
    {5,8},
    {4,7},
    {3,6},
    {2,5},
    {1,5},
    {0,5},
  
    {5,9},
    {5,8},
    {5,7},
    {6,6},
    {7,5},
    {8,4},
    {9,4},
    {10,4},
  };

  _showTime=false;
  
  for(int z=0;z<3;z++) {
    br=99;
    display.clear();
    display.setBrightness(br);
    for(int i=0;i<68;i++) {
      int x=arr[i][0];
      int y=arr[i][1];
      long rnd=0;
      if(x==5 && y==9) {
        rnd=random(0,360);
        rgb=display.getRGBColor(rnd*182.04);
      }
      turnLedOnOff(x, y, LEDON, DELAY50, true, rnd, rgb);
    }
    // verschieben
    for(int z=0;z<9;z++) {
      for(int y=9;y>z;y--) {
        br -= 4;
        if(br<0) br=1;
        display.setBrightness(br);
        for(int x=0;x<11;x++) {
          COLOR_T c=display.getPixelColor(x,y-1);
          if(c)
            display.drawPixel(x,y,c);
          else
            display.clearPixel(x,y);
        }
      }
      // Oberste Zeile löschen
      for(int x=0;x<11;x++)
        display.clearPixel(x,z);
      display.show();
      delay(200);
    }
    // Letzte Zeile löschen
    for(int x=0;x<11;x++)
        display.clearPixel(x,9);
    display.show();
  }
  delay(2000);
  
  // clear all LEDs with standard brightness
  display.setBrightness(0);
  _showTime=true;
  for(int y=0; y<10; y++) {
    for(int x=0;x<11;x++) {
      turnLedOnOff(x, y, LEDOFF, DELAY50, true, 0, 0);
    }
  }
  
}


/**
   Display Effect 26
*/
void Effekte::displayEffect26(void) {
  COLOR_T rgb;
  byte color[3][3]={
    {255,255,255},
    {255,255,0},
    {255,0,0}
  };
  int helligkeit[3]={20,50,99};

  _showTime=false;
  display.clear();
  
  for(int h=0;h<3;h++) {
    display.setBrightness(helligkeit[h]);
    
    for(c=0; c<3; c++) {
      rgb = display.getColor(color[c][0], color[c][1], color[c][2]);
      for(int x=0; x<10; x++) {
        for(int y=0; y<2; y++) {
          display.drawPixel(x, y, rgb);
        }
      }
      display.show(10000);
    }
  }
  display.setBrightness(0);
}

void Effekte::displayEffect27(void) {
  endlessloop=true;
  _showTime=false;
  display.clear();
  float c=0;
  while(endlessloop) {
    for(int x=0; x<11; x++) {
      CHSV hsv = display.getHsvFromDegRnd(0, c);
      //COLOR_T rgb = display.getRGBColor(hsv);
      for(int y=0;y<10;y++){
        turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
      }
    }
    c+=3.5;
    display.show(DELAY80);
#if defined(WITH_MQTT)
    mymqtt.check();
#endif
  }
  
   _showTime=true;
   for(int x=0;x<11;x++) {
     for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDOFF, 0, false, 0, 0);
    }
    display.show(DELAY30);
   }
  delay(DELAY500);
}



void Effekte::displayEffect28(void) {
  _showTime=false;
  display.clear();
  float c;
  bool d;
  d=true;
  c=180;
  for(int z=0; z<72; z++){
    for(int x=0; x<11; x++) {
      CHSV hsv = display.getHsvFromDegRnd(0, c);
      //COLOR_T rgb = display.getRGBColor(hsv);
      for(int y=0;y<10;y++){
        turnLedOnOff(x, y, LEDON, 0, false, 0, hsv);
      }
    }
    if(c>300){
      d=false;
    }
    if(c<180){
      d=true;
    }
    if(d){
      c+=3.5;
    }
    else{
      c-=3.5;
    }
    display.show(DELAY80);
  }
  
   _showTime=true;
   for(int x=0;x<11;x++) {
     for(int y=0; y<10; y++) {
      turnLedOnOff(x, y, LEDOFF, 0, false, 0, 0);
    }
    display.show(DELAY30);
  }
  delay(DELAY500);
}
/**
   Turn Led on/off pos x,y 
*/
void Effekte::turnLedOnOff(byte x, byte y, byte z, int delayTime, boolean wakeup, long rnd, COLOR_T rgb) {
  DEBUG_PRINTF("turnLedOnOff: x=%d, y=%d, z=%d, delayTime=%d, wakeup=%d\n",x,y,z,delayTime,wakeup);

  if(!z) {
    if(rgb)
      display.drawPixel(x, y, rgb);
    else
      display.drawPixel(x, y, display.getHsvFromDegRnd(rnd, display.getDegree(y, x)));
  } else if(_showTime) {
      word tmp = (0b1000000000000000 >> x);
      if(!(matrix[y] & tmp)) {
        display.clearPixel(x, y);
      } else {
        COLOR_T c = display.getColor();
        display.drawPixel(x, y, c);
      }
  } else
    display.clearPixel(x, y);

  if(wakeup)
    display.show();

  if(delayTime)
    delay(delayTime);
}

void Effekte::turnLedOnOff(byte x, byte y, byte z, int delayTime, boolean wakeup, long rnd, CHSV hsv) {
  DEBUG_PRINTF("turnLedOnOff: x=%d, y=%d, z=%d, delayTime=%d, wakeup=%d\n",x,y,z,delayTime,wakeup);

  if(!z) {
      display.drawPixel(x, y, hsv);
  } else if(_showTime) {
      word tmp = (0b1000000000000000 >> x);
      if(!(matrix[y] & tmp)) {
        display.clearPixel(x, y);
      } else {
        COLOR_T c = display.getColor();
        display.drawPixel(x, y, c);
      }
  } else
    display.clearPixel(x, y);
    

  if(wakeup)
    display.show();

  if(delayTime)
    delay(delayTime);
}

void Effekte::test(void) {

}
