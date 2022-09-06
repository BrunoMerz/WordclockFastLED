/**
 * Effekte.h
 * Class outputs nice pattern on the hour
 *
 * @autor    B. Merz
 * @version  2.0
 * @created  26.10.2018
 * @updated  11.04.2021
 *
 */

#pragma once

#include "Arduino.h"
#include "MyDisplay.h"
#include "Ticker.h"
#include "MySpiffs.h"
#include "Renderer.h"
#include "MyMqtt.h"

#define LOOP1  15
#define LOOP2  80
#define LOOP3  3
#define LOOP8  8
#define LOOP30 30
#define DELAY0    0
#define DELAY5    5
#define DELAY10   10
#define DELAY15   15
#define DELAY20   20
#define DELAY25   25
#define DELAY30   30
#define DELAY50   50
#define DELAY80   80
#define DELAY100  100
#define DELAY150  150
#define DELAY200  200
#define DELAY500  500


#define LEDON   0
#define LEDOFF  1
#define SHOWLED true
#define HIDELED false

class Effekte {
public:
    Effekte(void);
    void displayEffekt(int effectNo);
    void test(void);
    void displayEffect1(void);
    void displayEffect2(void);
    void displayEffect3(void);
    void displayEffect4(void);
    void displayEffect5(void);
    void displayEffect6(void);
    void displayEffect7(void);
    void displayEffect8(void);
    void displayEffect9(void);
    void displayEffect10(void);
    void displayEffect11(void);
    void displayEffect12(void);
    void displayEffect13(void);
    void displayEffect14(void);
    void displayEffect15(void);
    void displayEffect16(void);
    void displayEffect17(void);
    void displayEffect18(void);
    void displayEffect19(void);
    void displayEffect20(void);
    void displayEffect21(void);
    void displayEffect22(void);
    void displayEffect23(void);
    void displayEffect24(void);
    void displayEffect25(void);
    void displayEffect26(void);
    void displayEffect27(void);
    void displayEffect28(void);

private:
    void turnLedOnOff(byte x, byte y, byte z, int delayT, boolean wakeup, long rnd, COLOR_T c);
    void turnLedOnOff(byte x, byte y, byte z, int delayTime, boolean wakeup, long rnd, CHSV rgb);
    
    boolean _display_effect;
    boolean _effect;
    boolean _showTime;
    byte _effectArray[24];
    byte _effectCount;
    
    COLOR_T c;
};
