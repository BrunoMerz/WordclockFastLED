
#include "MyDisplay.h"
#include "AdjustBrightness.h"

extern MySpiffs myspiffs;
extern MYTM     mytm;
extern AdjustBrightness adjb;

//#define myDEBUG
#include "Debug.h"

#if (LED_LIB == 1)

#define TWIDTH      (STRIP_X_DIRECTION*LED_COLS)
#define THEIGHT     (STRIP_Y_DIRECTION*LED_ROWS)
#define TMTYPE      MATRIX_TYPE
#define MABSHEIGHT  (THEIGHT * ((THEIGHT < 0) * -1 + (THEIGHT > 0)))
#define MABSWIDTH   (TWIDTH * ((TWIDTH < 0) * -1 + (TWIDTH > 0)))

CRGB _out[LED_STRIPE_COUNT];

uint16_t mXY(uint16_t x, uint16_t y)
{
  uint16_t num;
  if(y==10 && x > 3)
    num = 110 + x;
  else
    num = pgm_read_byte(&(pixelMap[x + (y * LED_COLS)]));
  return num;
}
    
#elif (LED_LIB == 2)
cLEDMatrix<STRIP_X_DIRECTION*LED_COLS, STRIP_Y_DIRECTION*LED_ROWS, MATRIX_TYPE> _out;
#endif

#if (LED_LIB == 2)
// notwendig wegen blur2d
uint16_t XY(uint8_t x, uint8_t y) { 
  return _out.mXY(x, y); 
}
#endif

MyDisplay::MyDisplay() {
  _width = LED_COLS;
  _height = LED_ROWS;
  _color = 0xffffff;
  _brightness = 0;
}


void MyDisplay::DebugOutput(boolean colorOutput) {
  Serial.println();
  for(int y=_height-1;y>=0;y--) {
    for(int x=0;x<_width;x++) {
      COLOR_T c=getPixelColor(x,y);
      if(colorOutput) {
        if(c)
          Serial.printf("%#.6x ",c);
        else
          Serial.printf("         ",c);
      } else
        if(c)
          Serial.print("o");
        else
          Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}


void MyDisplay::init(uint8_t dataPin, uint16_t ledStripeCount) {
  DEBUG_PRINTF("MyDisplay::init dataPin=%d, ledStripeCount=%d\n", dataPin, ledStripeCount);
  _ledStripeCount = ledStripeCount;
#if (LED_LIB == 2)
  m_Matrix = &_out;
  FastLED.addLeds<WS2812B, dataPin, GRB>(_out[0], _ledStripeCount);
#else
  FastLED.addLeds<WS2812B, LED_STRIPE_DATA_PIN, GRB>(_out, _ledStripeCount);
#endif
  FastLED.setBrightness(50);
  FastLED.clear(true);
  int32_t max_current = myspiffs.getIntSetting("max_current");
  FastLED.setMaxPowerInVoltsAndMilliamps(5, max_current==-1?2000:max_current);
}


uint16_t MyDisplay::getWidth(void) {
  return _width;
}


uint16_t MyDisplay::getHeight(void) {
  return _height;
}



uint32_t MyDisplay::getColor(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8 ) | (uint32_t)b;
}


uint32_t MyDisplay::getColor(void) {
  return _color;
}


void MyDisplay::setColor(COLOR_T color) {
  _color = color;
}


void MyDisplay::setTextColor(COLOR_T color) {
  _color = color;
}


COLOR_T MyDisplay::getColorHSV(uint8_t hsv, uint8_t saturation, uint8_t brightness) {
  CHSV x(hsv, saturation, brightness);
  CRGB rgb;
  hsv2rgb_rainbow(x, rgb);
  COLOR_T curColor = ((long)rgb.r << 16L) | ((long)rgb.g << 8L) | (long)rgb.b;
  return curColor;
}


void MyDisplay::setCursor(uint16_t x_pos, uint16_t y_pos) {
  if (x_pos > LED_COLS - 1 || y_pos > LED_ROWS - 1) return;
  _x = x_pos;
  _y = y_pos;
}

void MyDisplay::print(String letter) {
  DEBUG_PRINTF("print '%s', x=%d, y=%d\n",letter.c_str(),_x, _y);
  for(int n=0;n<letter.length();n++) {
    char zeichen = letter[n];
    if (zeichen < 0x20 || zeichen > 0x7f)
        zeichen = 0x20;
    int breite = pgm_read_byte(&(TickerZeichen[(char)zeichen - ' '][5]));
    DEBUG_PRINTF("print zeichen=%c, breite=%d _x=%d\n",zeichen, breite, _x);
    for (int j = 0; j < breite; j++) {
      for (int k = 0; k < CHAR_ROWS; k++) {
        word bitArray = pgm_read_byte(&(TickerZeichen[(char)zeichen - ' '][k]));
        DEBUG_PRINTF("print zeichen[%d]=%x\n",k,bitArray);
        bitArray >>= (7 - j);
        bitArray &= 1;
        if((_x+j) < _width && (_y+k) < _height) {
          if(bitArray)
            drawPixel(_x+j, _y+(CHAR_ROWS-k-1), _color);
          else
            drawPixel(_x+j, _y+(CHAR_ROWS-k-1), 0);
        }
      }
    }
    _x += breite;
    if(_x < _width) {
      for (int k = 0; k < CHAR_ROWS; k++)
        drawPixel(_x, _y+k, 0);
      _x ++;
    } else 
      _x = _width-1;
  }
  show();
}





void MyDisplay::drawPixel(uint16_t x_pos, uint16_t y_pos, COLOR_T color) {
  DEBUG_PRINTF("drawPixel COLOR_T: x=%d, y=%d, c=%#.6x, _width=%d, _height=%d\n", x_pos, y_pos, color, _width, _height);
  DEBUG_FLUSH();
#if (LED_LIB == 2)
  (*m_Matrix)(x_pos, y_pos) = color;
#else
  int16_t n = mXY(x_pos, y_pos);
  if(n<LED_STRIPE_COUNT) {
    if(color)
      _out[n] = color;
    else
      _out[n] = getHsvFromDegRnd(_angleOffset, getDegree(y_pos, x_pos));
  }
  else {
    //Serial.printf("drawPixel COLOR_T: x=%d, y=%d, n=%d\n",x_pos, y_pos, n);
  }
#endif
}


void MyDisplay::drawPixel(uint16_t x_pos, uint16_t y_pos, CHSV color) {
  DEBUG_PRINTF("drawPixel: x=%d, y=%d, c=%#.6x, _width=%d, _height=%d\n", x_pos, y_pos, color, _width, _height);
  DEBUG_FLUSH();
#if (LED_LIB == 2)
  (*m_Matrix)(x_pos, y_pos) = color;
#else
  int16_t n = mXY(x_pos, y_pos);
  if(n<LED_STRIPE_COUNT)
    _out[n] = color;
  else {
    //Serial.printf("drawPixel CHSV: x=%d, y=%d, n=%d\n",x_pos, y_pos, n);
  }
#endif
}


void MyDisplay::drawPixel(uint16_t x_pos, uint16_t y_pos, CRGB color) {
  DEBUG_PRINTF("drawPixel CRGB: x=%d, y=%d,c=%#.6x, _width=%d, _height=%d\n", x_pos, y_pos, color, _width, _height);
  DEBUG_FLUSH();
#if (LED_LIB == 2)
  (*m_Matrix)(x_pos, y_pos) = color;
#else
  int16_t n = mXY(x_pos, y_pos);
  if(n<LED_STRIPE_COUNT)
    _out[n] = color;
  else {
    //Serial.printf("drawPixel CRGB: x=%d, y=%d, n=%d\n",x_pos, y_pos, n);
  }
#endif
}


void MyDisplay::clearPixel(uint16_t x_pos, uint16_t y_pos) {
  int16_t n = mXY(x_pos, y_pos);
  if(n<LED_STRIPE_COUNT)
    _out[n] = CRGB::Black;
}


void MyDisplay::DrawFilledRectangle(uint16_t x_pos, uint16_t y_pos, uint16_t x1_pos, uint16_t y1_pos, COLOR_T color) {
  DEBUG_PRINTF("DrawFilledRectangle x_pos=%d, y_pos=%d, x1_pos=%d, y1_pos=%d, color=%x\n",x_pos,y_pos,x1_pos,y1_pos,color);
//  _out.DrawFilledRectangle(x_pos, y_pos, x1_pos, y1_pos, color);

  for(uint16_t y=y_pos; y<y1_pos; y++)
    for(uint16_t x=x_pos; x<x1_pos; x++) {
      DEBUG_PRINTF("x=%d, y=%d, color=%#.6x\n",x,y,color);
      drawPixel(x,y,color);
    }
}

void MyDisplay::DrawFilledRectangle(uint16_t x_pos, uint16_t y_pos, uint16_t x1_pos, uint16_t y1_pos, CHSV color) {
  DEBUG_PRINTF("DrawFilledRectangle x_pos=%d, y_pos=%d, x1_pos=%d, y1_pos=%d, color=%x\n",x_pos,y_pos,x1_pos,y1_pos,color);
//  _out.DrawFilledRectangle(x_pos, y_pos, x1_pos, y1_pos, color);

  for(uint16_t y=y_pos; y<y1_pos; y++)
    for(uint16_t x=x_pos; x<x1_pos; x++) {
      DEBUG_PRINTF("x=%d, y=%d, color=%#.6x\n",x,y,color);
      drawPixel(x,y,color);
    }
}


void MyDisplay::clear(void) {
  DEBUG_PRINTLN("clear");
  _angleOffset = (mytm.tm_hour*15+mytm.tm_min*6)%360;
  FastLED.clear();
}


void MyDisplay::clear(CRGB color) {
  DEBUG_PRINTF("clear CRGB, c=%#.6x\n", getColor(color.r, color.g, color.b));
#if (LED_LIB == 2)
  m_Matrix->DrawFilledRectangle(0, 0, LED_COLS-1, LED_ROWS-1, color);
#else
  for(uint16_t n=0;n<LED_STRIPE_COUNT;n++)
    _out[n] = color;
#endif
}


void MyDisplay::clear(CHSV color) {
  DEBUG_PRINTLN("clear CHSV");
  DrawFilledRectangle(0, 0, LED_COLS-1, LED_ROWS-1, color);
}


void MyDisplay::show(uint16_t dly) {
  DEBUG_PRINTF("show, delay=%d\n",dly);
  FastLED.show();
  if(dly)
    delay(dly);
  else
    delay(5);
}


// Convert angle + randomvalue to a hsv value
CHSV MyDisplay::getHsvFromDegRnd(uint16_t rnd, uint16_t dg) {
  uint16_t deg = ((uint16_t)(dg+0.5)+rnd)%360;
  uint8_t h = map(deg,0,359,0,255);
  CHSV hsv(h,255,255);
  DEBUG_PRINTF("rnd=%d, deg=%d, h=%d, hsv=%#.6x\n", rnd, deg, h, hsv);
  return hsv;
}


// Convert angle + randomvalue to a hsv value
CHSV MyDisplay::getHsvFromDegRnd(uint16_t dg) {
  return getHsvFromDegRnd(_angleOffset, dg);
}


uint16_t MyDisplay::getDegree(uint16_t sec) {
  // sec == 0  ==> Sekunden blinken unter der Glocke
  // sec == 1-60  ==> Sekundenzeiger rund um die Uhr
  uint16_t x = pgm_read_byte(&(coorMap[sec][0]));
  uint16_t y = pgm_read_byte(&(coorMap[sec][1]));
  return getDegree(y, x);
}


uint16_t MyDisplay::getDegree(uint16_t y, uint16_t x) {
  float degree;
  const double dx = 29.9;
  const double dy = 33.22;

  
  // Ankathete und Gegenkathete berechnen
  double ak = 149.5 - (x * dx);
  double gk = 149.5 - (y * dy);

  if(ak==0)
    if(gk==0)
      degree=0;
    else if(gk<0)
          degree=270;
         else
          degree=90;
  else {
    // atan() liefert radian und wird in Grad umgerechnet
    degree = atan(gk/ak) * 57.2957795131;
    if(ak<0 && gk<0)
      degree = 360-degree;
    if(ak>=0 && gk<0)
      degree = 180-degree;
    if(ak<0 && gk>=0)
      degree *= -1;
    if(ak>=0 && gk>=0)
      degree = 180-degree;
  }
  DEBUG_PRINTF("x=%d, y=%d, degree=%f\n", x, y, degree);
  return degree;
}


COLOR_T MyDisplay::getRGBColor(uint16_t hsv) {
  byte brightness = map(getBrightness(), 0, 99, 0, 255);
  CHSV x(hsv, 255, brightness);
  CRGB rgb;
  hsv2rgb_rainbow(x, rgb);
  COLOR_T curColor = ((long)rgb.r << 16L) | ((long)rgb.g << 8L) | (long)rgb.b;
  return curColor;
}


int16_t MyDisplay::getBrightness(void) {
  if(_brightness)
    return _brightness;
  else
    return adjb.getOffset();
}


void MyDisplay::setBrightness(void) {
  int bn;
  if(_brightness)
    bn = _brightness;
  else
    bn = adjb.getOffset();

  DEBUG_PRINTF("setBrightness void: bn=%d\n", bn);
  bn = map(bn,0,100,0,255);
  FastLED.setBrightness(bn);
}


void MyDisplay::setBrightness(uint16_t brightness) {
  DEBUG_PRINTF("setBrightness: brightness=%d\n", brightness);
  _brightness = brightness;
  setBrightness();
}


COLOR_T MyDisplay::getPixelColor(uint16_t x_pos, uint16_t y_pos) {
  CRGB led;
#if (LED_LIB == 2)
  led = (*m_Matrix)(x_pos, y_pos);
#else
  uint16_t n = mXY(x_pos, y_pos);
  if(n<LED_STRIPE_COUNT)
    led = _out[n];
  else {
    //Serial.printf("getPixelColor: x=%d, y=%d, n=%d\n",x_pos, y_pos, n);
    return 0;
  }
#endif
  return (((uint32_t)led.r << 16) | ((long)led.g << 8 ) | (long)led.b);
}


void MyDisplay::shiftMatrix(void) {
  for(uint16_t y_pos=0; y_pos < LED_ROWS; y_pos++) {
    uint16_t nd;
    uint16_t ns;
    for(uint16_t x_pos=0; x_pos < LED_COLS; x_pos++) {
      ns=mXY(x_pos+1,y_pos);
      nd=mXY(x_pos,y_pos);
      memmove8(&_out[nd], &_out[ns], sizeof(CRGB));
    }
    nd=mXY(LED_COLS-1,y_pos);
    _out[nd] = CRGB::Black;
  }
}

void MyDisplay::shiftMatrix(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  for(uint16_t y_pos=0; y_pos < height; y_pos++) {
    uint16_t ns;
    uint16_t nd;
    for(uint16_t x_pos=0; x_pos < width; x_pos++) {
      ns=mXY(x+x_pos+1,y+y_pos);
      nd=mXY(x+x_pos,y+y_pos);
      memmove8(&_out[nd], &_out[ns],sizeof(CRGB));
    }
    nd=mXY(x+width-1,y+y_pos);
    _out[nd] = CRGB::Black;
  }
}


void MyDisplay::DrawRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, COLOR_T c) {
//mz  _out.DrawRectangle(x_pos, y_pos, x_size, y_size, c);
  // 2 Horizontale Linien
  for(uint16_t x=x0; x<x1; x++) {
    drawPixel(x, y0, c);
    drawPixel(x, y1, c);
  }
  // 2 Vertikale Linien
  for(uint16_t y=y0; y<y1; y++) {
    drawPixel(x0, y, c);
    drawPixel(x1, y, c);
  }
}


void MyDisplay::DrawRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, CHSV c) {
//mz  _out.DrawRectangle(x_pos, y_pos, x_size, y_size, c);
  // 2 Horizontale Linien
  for(uint16_t x=x0; x<x1; x++) {
    drawPixel(x, y0, c);
    drawPixel(x, y1, c);
  }
  // 2 Vertikale Linien
  for(uint16_t y=y0; y<y1; y++) {
    drawPixel(x0, y, c);
    drawPixel(x1, y, c);
  }
}


void MyDisplay::setMaxPowerInVoltsAndMilliamps(uint8_t volts, uint32_t milliamps) {
  FastLED.setMaxPowerInVoltsAndMilliamps(volts, milliamps);
  DEBUG_PRINTF("setMaxPowerInVoltsAndMilliamps: %d, %d\n",volts, milliamps);
}


void MyDisplay::addColor(uint16_t x_pos, uint16_t y_pos, CHSV color) {
#if (LED_LIB == 2)
  (*m_Matrix)(x_pos, y_pos) += color;
#else
  uint16_t n = mXY(x_pos, y_pos);
  if(n<LED_STRIPE_COUNT)
    _out[n] += color;
  else {
    //Serial.printf("addColor: x=%d, y=%d, n=%d\n",x_pos, y_pos, n);
  }
#endif
}


void MyDisplay::doBlur2d(fract8 blur_amount) {
#if (LED_LIB == 2)
  blur2d(_out[0], LED_COLS, LED_ROWS, blur_amount);
#endif
}


void MyDisplay::fadePixel(uint16_t x_pos, uint16_t y_pos, byte step) {     // neuer Fader
#if (LED_LIB == 2)
  CRGB led = (*m_Matrix)(x_pos, y_pos);
  
  if (led.r >= 30 ||
      led.g >= 30 ||
      led.b >= 30) {
    (*m_Matrix)(x_pos, y_pos).fadeToBlackBy(step);
  } else {
    (*m_Matrix)(x_pos,y_pos) = 0;
  }
#else
  uint16_t n = mXY(x_pos, y_pos);
  if(n<LED_STRIPE_COUNT) {
    CRGB led = _out[n];
    if (led.r >= 30 ||
        led.g >= 30 ||
        led.b >= 30) {
          _out[n].fadeToBlackBy(step);
        } else {
          _out[n] = 0;
        }
  } else {
    //Serial.printf("addColor: x=%d, y=%d, n=%d\n",x_pos, y_pos, n);
  }
#endif
}
