/**
   IconRenderer.h
   @autor    Bruno Merz

   @version  1.0
   @created  09.01.2020
   @updated  09.01.2020

   Version history:
   V 1.0:  - Created.

*/

#pragma once

class IconRenderer;

#include "Configuration.h"
#include "MyDisplay.h"

#if (USE_JPGDECODING == 1)
#include <JPEGDecoder.h>  // JPEG decoder library
#endif

#define BYTE  byte
#define WORD  short
#define DWORD int
#define LONG  int

struct ICOHEADER {
   WORD wReserved;  // Always 0
   WORD wResID;     // Always 1
   WORD wNumImages; // Number of icon images/directory entries
};


struct ICONDIRENTRY {
    BYTE  bWidth;    // Breite des Bildes
    BYTE  bHeight;    // Die doppelte Höhe des Bildes in Pixeln.
    BYTE  bCount;    // Anzahl der Farben im Bild (2 oder 16, bzw. 0 falls mehr als 
    BYTE  bReserved;    // 256 Farben). immer 0
    WORD  wPlanes;    // Anzahl der Farbebenen in der Regel 1 u. 0
    WORD  wBitCount;      // Anzahl der Bits pro Pixel( Anzahl der Farben) 1= Schwarz-
        // Weiß = 8 Byte, 4 = 16 Farben = 64 Byte, 
        // 8 = 256 Farben = 1024 Byte, 24 = 16 Milion
        // Farben (keine Farbtabelle) 
    DWORD dwBytesInRes;   // Bildgröße in Bytes ab diesem Record. Das bedeutet 
                          // BITMAPINFOHEADER, Farbtabelle, XOR- und 
                          // AND-Bild zusammen gerechnet.
    DWORD dwImageOffset;  // Offset ab wo der BITMAPINFOHEADER beginnt.
};


struct BITMAPINFOHEADER {
    DWORD   biSize;      // Länge des Info-Headers(dieser Record) = 40 Byte in Hex 28
    LONG    biWidth;      // Breite des Bildes 
    LONG    biHeight;     // Höhe des Bildes 
    WORD    biPlanes;     // Anzahl der Farbebenen in der Regel 1 u. 0
    WORD    biBitCount;   // Anzahl der Bits pro Pixel( Anzahl der Farben) 1= Schwartz-
        // Weiß = 8 Byte, 4 = 16 Farben = 64 Byte, 
        // 8 = 256 Farben = 1024 Byte, 24 = 16 Milion 
        // Farben (keine Farbtabelle)  
    DWORD   biCompression;  // Komprimierungstyp, 0 = Unkomprimiert, 1 = 8-Bit 
          // RLE =Run-Length-Encording-Verfahren, 2 = 4-Bit Run-Encording-
          // Verfahren
    DWORD   biSizeImage;    // Bildgröße ohne Farbtabelle. Aber XOR und AND Bild zusammen 
             // gerechnet bei Icon u. Cursor.
    DWORD   biXPelsPerMeter;   // Horizontale Auflösung
    DWORD   biYPelsPerMeter;   // Vertikale Auflösung
    DWORD   biClrUsed;         // Die Zahl der im Bild vorkommenden Farben zB. Bei 256 Farben
             //  müssen es nicht unbedingt 256 sein, es könne auch 
             // zB. 206 sein.
    DWORD   biClrImportant;    // die Anzahl der wichtigen Farben
} ;

struct ICON {
  BYTE  b;  // blue
  BYTE  g;  // green
  BYTE  r;  // red
  BYTE  a;  // alpha
};

class IconRenderer {
  public:
    IconRenderer(void);
    void renderAndDisplay(String iconFile, int delayAfter, byte cleanUp, int16_t x_pos=-1, int16_t y_pos=-1);
    // delayAfter (ms)
    // cleanUp  0 - don't clear LED Matrix neither before nor after
    //          1 - clear LED Matrix before displaying icon
    //          2 - clear LED Matrix after displaying icon and delay
    //          3 - 1 + 2

#if (USE_JPGDECODING == 1)
    void renderAndDisplayJPEG(String iconFileName, int delayAfter, byte cleanUp, int16_t x_pos, int16_t y_pos);
    void jpegInfo();
    void renderJPEG(int xpos, int ypos);
#endif

  private:
    ICOHEADER *_icoHeader;
    ICONDIRENTRY _iconDirentry, *_pIconDirentry;
    BITMAPINFOHEADER _bitmapInfoHeader, *_pBitmapInfoHeader;
    ICON *_icon;
    uint16_t  _x_pos;
    uint16_t  _y_pos;
};
