/**

 */

#pragma once

#include "Configuration.h"

#ifdef DCF77_SENSOR_EXISTS

#include "Arduino.h"
#include "TimeStamp.h"

class MyDCF77 : public TimeStamp {

public:
    MyDCF77(byte signalPin, byte dcf77PonPin);

    void enable(boolean on);

    boolean poll(boolean signalIsInverted);

    unsigned long getDcf77LastSuccessSyncMinutes();
    void setDcf77SuccessSync(TimeStamp* _rtc);

    byte getDcf77ErrorCorner();

    boolean signal(boolean signalIsInverted);

private:
    static byte DCF77Factors[];

    byte _signalPin;
    byte _dcf77PonPin;
    boolean _enable = true;
    unsigned int _nPolls;    

    byte _bits[MYDCF77_TELEGRAMMLAENGE+1];
    byte _bitsPointer;
    
    unsigned int _bins[MYDCF77_SIGNAL_BINS];
    char _binsPointer;
    char _binsOffset;

    unsigned long _dcf77LastCycle;
    // _toggleSignal wird nur für EXT_MODE_DCF_DEBUG benötigt
    boolean _toggleSignal;
    byte _errorCorner;

    TimeStamp _dcf77LastSyncTime;

    boolean newCycle();
    void outputSignal(unsigned int average, unsigned int imax, unsigned int isum);

    byte decodeHelper(byte *checksum, byte startV, byte endV);
    boolean decode();

    void clearBits();
    void clearBins();
};

#endif
