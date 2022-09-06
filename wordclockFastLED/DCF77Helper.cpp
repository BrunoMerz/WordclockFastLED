/**

 */

#include "DCF77Helper.h"

#ifdef DCF77_SENSOR_EXISTS

//#define MyDEBUG
#include "Debug.h"

/**
 * Initialisierung und das Array 'falsch' vorbelegen, damit der Zeitabstand
 * der Samples zum Anfang nicht stimmt.
 */
DCF77Helper::DCF77Helper() {
    _cursor = 0;
    for (byte i = 0; i < DCF77HELPER_MAX_SAMPLES; i++) {
        _zeitstempelDcf77[i] = new TimeStamp(i, i, i, i, i, i);
        _zeitstempelRtc[i] = new TimeStamp(100, 0, 0, 0, 0, 0);
    }
}

/**
 * Einen neuen Sample hinzufuegen.
 */
void DCF77Helper::addSample(TimeStamp* dcf77, TimeStamp* rtc) {
    DEBUG_PRINT(F("Adding sample from dcf77: "));
    _zeitstempelDcf77[_cursor]->set(dcf77);
    DEBUG_PRINTLN(_zeitstempelDcf77[_cursor]->asString());

    DEBUG_PRINT(F("Adding sample from rtc: "));
    _zeitstempelRtc[_cursor]->set(rtc);
    DEBUG_PRINTLN(_zeitstempelRtc[_cursor]->asString());
    DEBUG_FLUSH();

    _cursor++;
    if (_cursor >= DCF77HELPER_MAX_SAMPLES) {
        _cursor = 0;
    }
}

/**
 * Die Samples vergleichen
 */
boolean DCF77Helper::samplesOk() {
    boolean retVal = true;
    for (byte i = 0; i < DCF77HELPER_MAX_SAMPLES - 1; i++) {
        unsigned long minuteDiffDcf77 = _zeitstempelDcf77[i]->getMinutesOfCentury() - _zeitstempelDcf77[i + 1]->getMinutesOfCentury();
        unsigned long minuteDiffRtc = _zeitstempelRtc[i]->getMinutesOfCentury() - _zeitstempelRtc[i + 1]->getMinutesOfCentury();
        long diff = minuteDiffDcf77 - minuteDiffRtc;
        // Teste den Minutenabstand (über den gesamten Datumsbereich) zwischen den Zeitstempeln...
        if ( abs(diff) <= 1 ) {
            DEBUG_PRINT(F("Diff #"));
            DEBUG_PRINT(i);
            DEBUG_PRINT(F(" distance is ok ("));
            DEBUG_PRINT((long) minuteDiffDcf77);
            DEBUG_PRINT(F("~="));
            DEBUG_PRINT((long) minuteDiffRtc);
            DEBUG_PRINTLN(F(")."));
            DEBUG_FLUSH();
        } else {
            DEBUG_PRINT(F("Diff #"));
            DEBUG_PRINT(i);
            DEBUG_PRINT(F(" distance is wrong ("));
            DEBUG_PRINT((long) minuteDiffDcf77);
            DEBUG_PRINT(F("!="));
            DEBUG_PRINT((long) minuteDiffRtc);
            DEBUG_PRINTLN(F(")."));
            DEBUG_FLUSH();
            retVal = false;
        }
    }

    return retVal;
}

#endif
