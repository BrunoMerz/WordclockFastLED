/**
   IconRenderer.cpp
   Class outputs icons stored in LittleFS

   @autor    B. Merz
   @version  1.1
   @created  09.01.2020
   @updated  19.03.2020

*/
#include "IconRenderer.h"
#include "MySpiffs.h"

//#define myDEBUG
#include "Debug.h"

extern MyDisplay display;
extern MySpiffs myspiffs;

int wp=0;

IconRenderer::IconRenderer() {

}

#if (USE_JPGDECODING == 1)
void IconRenderer::renderAndDisplayJPEG(String iconFileName, int delayAfter, byte cleanUp, int16_t x_pos, int16_t y_pos) {
  int ret=-1;
  File fh = LittleFS.open(iconFileName, "r");
  //Serial.printf("renderAndDisplayJPEG: vor decodeFsFile, fh=%d\n", fh);
  if(fh) {
    ret = JpegDec.decodeFsFile(fh);
    Serial.printf("renderAndDisplayJPEG: ret=%d\n",ret);
    fh.close();
  }
  Serial.printf("renderAndDisplayJPEG: nach decodeFsFile, ret=%d\n",ret);
  if(ret>0) {
    uint8_t row = 0; uint8_t col = 0;

    int width         = JpegDec.width;
    int height        = JpegDec.height;
    uint32_t mcuPixels= JpegDec.MCUWidth * JpegDec.height;

    uint16_t _width   = display.getWidth();
    uint16_t _height  = display.getHeight();

    // check if x_pos/y_pos are given, if no place ico midth of display
    if(x_pos >= 0) 
      _x_pos = x_pos;
    else
      if(_width>width)
        _x_pos=(_width-width)/2;
      else
        _x_pos = 0;

    if(y_pos >= 0) 
      _y_pos = y_pos;
    else
      if(_height>height)
        _y_pos=(_height-height)/2;
      else
        _y_pos = 0;

 Serial.printf("renderAndDisplayJPEG: width=%d, height=%d, mcuPixels=%d, _x_pos=%d, _y_pos=%d, cleanUp=%d, delayAfter=%d\n",width, height, mcuPixels,    _x_pos, _y_pos, cleanUp, delayAfter);
    // clear led matrix
    if(cleanUp & 1)
      display.clear();

    while(JpegDec.read()){
      uint16_t *pImg = JpegDec.pImage;
      for(uint8_t i=0; i < mcuPixels; i++){
        // Extract the red, green, blue values from each pixel
        uint8_t b = uint8_t((*pImg & 0x001F)<<3); // 5 LSB for blue
        uint8_t g = uint8_t((*pImg & 0x07C0)>>3); // 6 'middle' bits for green
        uint8_t r = uint8_t((*pImg++ & 0xF800)>>8); // 5 MSB for red
        // Calculate the matrix index (column and row)
        col = JpegDec.MCUx*8 + i%8;
        row = JpegDec.MCUy*8 + i/8;
        COLOR_T c = display.getColor(r,g,b);
        Serial.printf("x=%d, y=%d, c=%#.6x\n",col,row,c);
        // Set the matrix pixel to the RGB value
        display.drawPixel(_x_pos+col,_y_pos+(_height-row),c);
      }
    }
    display.show(delayAfter);

    // clear led matrix
    if(cleanUp & 2)
      display.clear();
  }
}
#endif

void IconRenderer::renderAndDisplay(String iconFileName, int delayAfter, byte cleanUp, int16_t x_pos, int16_t y_pos) {
  char *fb;
  String tmp=iconFileName;
  if(!tmp.startsWith("/"))
    tmp = "/"+tmp;
  DEBUG_PRINTF("Icon to render='%s', delayAfter=%d, cleanUp=%d\n",tmp.c_str(), delayAfter, cleanUp);
  delay(5);
  DEBUG_PRINTLN(tmp);

#if (USE_JPGDECODING == 1)
  if(iconFileName.endsWith(F(".jpg")) || iconFileName.endsWith(F(".jpeg"))) {
    renderAndDisplayJPEG(iconFileName, delayAfter, cleanUp, x_pos, y_pos);
    return;
  }
#endif

  int fs = myspiffs.fileSize(tmp);
  DEBUG_PRINTF("Icon size=%d\n",fs);
  if(fs <= 0)
    return;
  fb = (char *)malloc(fs+10);
  if(!fb) {
    DEBUG_PRINTF("Malloc %d bytes failed\n",fs);
    return;
  }
  int lng = myspiffs.readFile(tmp, fb);
  if(!lng){
    DEBUG_PRINTLN("renderAndDisplay: file not found -> "+iconFileName);
    return;
  }

  uint16_t _width = display.getWidth();
  uint16_t _height = display.getHeight();

  // set _icoHeader to beginning of icon file
  _icoHeader = (ICOHEADER*)fb;
  // set _pIconDirentry 
  _pIconDirentry = (ICONDIRENTRY*)(fb+sizeof(ICOHEADER));
  // _pIconDirentry must be copied due to alignment of integer values
  memcpy((void*)&_iconDirentry,(const void*) _pIconDirentry, sizeof(ICONDIRENTRY));
  // set Bitmap Infoheader 40 Byte
  _pBitmapInfoHeader = (BITMAPINFOHEADER*) (fb+sizeof(ICOHEADER)+(sizeof(ICONDIRENTRY)*_icoHeader->wNumImages));
  // _pBitmapInfoHeader must be copied due to alignment of integer values
  memcpy((void*)&_bitmapInfoHeader,(const void*) _pBitmapInfoHeader, sizeof(BITMAPINFOHEADER));

  // setup all necessary values
  byte r,g,b;
  int paddedW       = 0;
  int Mapsize  = 0;
  int wBitCount     = _bitmapInfoHeader.biBitCount;
  int width         = _bitmapInfoHeader.biWidth;
  int height        = _bitmapInfoHeader.biHeight/2;
  int wPlanes       = _bitmapInfoHeader.biPlanes;
  int Count    = 1 << (wBitCount * wPlanes);                 // Count = 2 ^ number of  bits, Bits=1->2 s, 4->16 s, 24->‭16.777.216‬ s
  byte bit_m        = Count - 1;

  // check if x_pos/y_pos are given, if no place ico midth of display
  if(x_pos >= 0) 
    _x_pos = x_pos;
  else
    if(_width>width)
      _x_pos=(_width-width)/2;
    else
      _x_pos = 0;

  if(y_pos >= 0) 
    _y_pos = y_pos;
  else
    if(_height>height)
      _y_pos=(_height-height)/2;
    else
      _y_pos = 0;

  DEBUG_PRINTF("_width=%d, width=%d, _x_pos=%d, _height=%d, height=%d, _y_pos=%d\n",_width, width, _x_pos, _height, height, _y_pos);
  // due to different bitmap width, i.e 11 bits width, we need some padding bytes
  if(wBitCount==1) {
    paddedW = (32 -_bitmapInfoHeader.biWidth)%32;
  } else if(wBitCount==4){
    paddedW = (16 -_bitmapInfoHeader.biWidth)%16;
  } else if(wBitCount==24){
    paddedW = 4 - ((_bitmapInfoHeader.biWidth*3)%4);
  }

  // calculate size of  table for icons with a bitCount less or equal than 8
  // for icons with bitCount=24 we d  on't have a  table
  if(wBitCount <= 8) Mapsize = (Count*4); // biBitCount=1->8, 4->64, 8->1024, 24->no  map

  char *XorMask = ((char *)_pBitmapInfoHeader + _bitmapInfoHeader.biSize + Mapsize);
  char *AndMask = XorMask + (((width+paddedW)*height*wBitCount)/8);
  
  DEBUG_PRINTF("Mapsize=%#x\nCount=%#x\npaddedW=%d\nXorMask=%#x\nAndMask=%#x\n",Mapsize, Count, paddedW, XorMask-fb, AndMask-fb);
  DEBUG_PRINTF("height=%d, width=%d, wBitCount=%d, paddedW=%d\n",height,width,wBitCount,paddedW);
  
  // icons with compressed bitmaps or icons > display size are not suppoerted
  if(_bitmapInfoHeader.biCompression || height > _height || width > _width) { 
    DEBUG_PRINTLN("Compressed icons or icons greater than LED Matrix not supported");
    if(fb) free(fb);
    display.show();
    return;
  }

  // clear led matrix
  if(cleanUp & 1)
    display.clear();

  if(wBitCount <= 8) {
    // Icon with  map    

    _icon = (ICON*) ((char *)_pBitmapInfoHeader + _bitmapInfoHeader.biSize);

    if(Count==2) {
      for(int y=height-1; y>=0; y--) {
        for(int x=0; x<width; x++) {
          int bit_n = x+(y*(width+paddedW));// bit_n: n-th bit within XorMask
          int byte_n = bit_n/8;             // byte_n: n-th byte within XorMask
          int bit_r = bit_n%8;              // bit_r: remainder 
          byte b = AndMask[byte_n];         // byte with necessary bit
          byte _i = b >> (7-bit_r);    // shift bit to right into rightmost position
          _i &= bit_m;                 // mask out all other bits
  
          if(!x) {
            DEBUG_PRINTLN();
          }
          if(x<_width && y>=(height-_height) && !_i) {
            DEBUG_PRINT("o ");
            display.drawPixel(x+_x_pos, _height-(y+_y_pos), display.getColor(0xff, 0xff, 0xff));
          } else {
            DEBUG_PRINT("  ");
          }
        }
        DEBUG_FLUSH();
      }
      DEBUG_PRINTLN();
    } else {  
      // 16 or 256 s
      DEBUG_PRINTLN("16 or 256 s");
      for(int y=height-1; y>=0; y--) {
        for(int x=0; x<width; x++) {
          int bit_n = (x*wBitCount)+(y*(width+paddedW)*wBitCount);// bit_n: n-th bit within XorMask
          int byte_n = bit_n/8;             // byte_n: n-th byte within XorMask
          int bit_r = bit_n%8;              // bit_r: remainder 
          byte b = XorMask[byte_n];         // byte with necessary bit
          byte _i = b >> (8-wBitCount-bit_r);    // shift bit to right into rightmost position
          _i &= bit_m;                 // mask out all other bits
  
          if(x<_width && y>=(height-_height)) {
            if(!x) {
              DEBUG_PRINTLN();
            }
            r = _icon[_i].r;
            g = _icon[_i].r;
            b = _icon[_i].r;
            
            if(r || g || b) {
              DEBUG_PRINT("o ");
              display.drawPixel(x+_x_pos, _height-1-(y+_y_pos), display.getColor(r, g, b));
            } else {
              DEBUG_PRINT("  ");
            }
          }
        }
      }
      DEBUG_PRINTLN();
    }
  } else {
    // Icon without  map, s defined directly in XorMask (each pixel in BGR)
    DEBUG_PRINTLN("Icon without  map");
    for(int y=height-1, offset=0; y>=0; y--) {
      for(int x=0; x<width; x++) {
        //offset = (x*3) + (y*3*(width+paddedW));
        offset = (x*3) + (y*3*width)+(paddedW*y);
        
        if(x<_width && y>=(height-_height)) {
          r = XorMask[offset+2];
          g = XorMask[offset+1];
          b = XorMask[offset];
          uint16_t xg = x+_x_pos;
          uint16_t yg = y+_y_pos;
          COLOR_T cg = display.getColor(r, g, b);
          DEBUG_PRINTF("x=%d, y=%d, c=%#.6x\n",xg,yg,cg);
          if(cg) {
            display.drawPixel(xg, _height-1-yg, cg);
          }
        }
      }
    }
  }

#ifdef myDEBUG
  display.DebugOutput(false);
  display.DebugOutput(true);
#endif

  display.show(delayAfter);

  // clear led matrix
  if(cleanUp & 2)
    display.clear();

  if(fb)
    free(fb);
}
