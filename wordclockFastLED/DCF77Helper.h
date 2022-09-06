/**
 * DCF77Helper

 */
#pragma once

#include "Configuration.h"

#ifdef DCF77_SENSOR_EXISTS

#include "Arduino.h"
#include "TimeStamp.h"

class DCF77Helper {
public:
    DCF77Helper();

    void addSample(TimeStamp* dcf77, TimeStamp* rtc);
    boolean samplesOk();

private:
    byte _cursor;
    TimeStamp *_zeitstempelDcf77[DCF77HELPER_MAX_SAMPLES];
    TimeStamp *_zeitstempelRtc[DCF77HELPER_MAX_SAMPLES];
};

#endif
