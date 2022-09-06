/**

 */
 
#include "MyDCF77.h"

#ifdef DCF77_SENSOR_EXISTS

//#define MyDEBUG
#include "Debug.h"

// Anzeige des Signalgraphen, nur wenn auch DEBUG gesetzt
#define DEBUG_SIGNAL
// Höhe des Signalgraphen, wenn DEBUG_SIGNAL gesetzt (Default: 40)
#define DEBUG_SIGNAL_VIS_HEIGHT 40

byte MyDCF77::DCF77Factors[] = {1, 2, 4, 8, 10, 20, 40, 80};

/**
 * Initialisierung mit dem Pin, an dem das Signal des Empfaengers anliegt
 */
MyDCF77::MyDCF77(byte signalPin, byte dcf77PonPin) : TimeStamp() {
    _signalPin = signalPin;
#ifndef MYDCF77_SIGNAL_IS_ANALOG
    pinMode(_signalPin, INPUT);
#endif

    _dcf77PonPin = dcf77PonPin;

    _nPolls = 0;
    _bitsPointer = 0;
    _binsPointer = 0;
    _binsOffset = 0;
    _dcf77LastCycle = 0;
}


void MyDCF77::enable(boolean on) {
    if (on && !_enable) {
        _enable = on;
        DEBUG_PRINTLN(F("DCF77-Empfaenger aufgeweckt."));
        DEBUG_FLUSH();
        digitalWrite(_dcf77PonPin, LOW);
    }
    if (!on && _enable) {
        _enable = on;
        DEBUG_PRINTLN(F("DCF77-Empfaenger schlafen gelegt."));
        DEBUG_FLUSH();
        digitalWrite(_dcf77PonPin, HIGH);
    }
}

/**
 * Liegt ein Signal vom Empfaenger an?
 */
boolean MyDCF77::signal(boolean signalIsInverted) {
    boolean val;
#ifdef MYDCF77_SIGNAL_IS_ANALOG
    if (signalIsInverted) {
        val = analogRead(_signalPin) < MYDCF77_ANALOG_SIGNAL_TRESHOLD;
    } else {
        val = analogRead(_signalPin) > MYDCF77_ANALOG_SIGNAL_TRESHOLD;
    }
#else
    if (signalIsInverted) {
        val = (digitalRead(_signalPin) == LOW);
    } else {
        val = (digitalRead(_signalPin) == HIGH);
    }
#endif
    return val;
}

/**
 * Aufsammeln der Zustaende des DCF77-Signals.
 */
boolean MyDCF77::poll(boolean signalIsInverted) {
    boolean retVal = false;
   
    if (_binsPointer >= 0) {
        _nPolls++;            
        if (signal(signalIsInverted)) {
            // Array an _binsPointer-Position wird mit Messwerten gefüllt
            _bins[_binsPointer]++;
            if (!_toggleSignal) {
                _toggleSignal = true;
                _errorCorner++;
                // Überlauf von _errorCorner ist unproblematisch
            }
        } else {
            _toggleSignal = false;   
        }     
    }

    if (micros() - _dcf77LastCycle >= 1000000/MYDCF77_SIGNAL_BINS) {
        _dcf77LastCycle += 1000000/MYDCF77_SIGNAL_BINS;
        retVal = newCycle();
    }
       
    return retVal;
}

/**
 * Der ( 1 / MYDCF77_SIGNAL_BINS )-ste Teil einer Sekunde startet.
 * Muss von einem externen Zeitgeber, z. B. einer RTC, aufgerufen werden.
 *
 * Diese Funktion setzt updateFromDCF77 auf eine Zeitdauer, die abgewartet
 * wird, bevor die Funktion poll() (siehe oben) WAHR zurückgibt.
 * TRUE bedeutet, das Zeittelegramm wurde korrekt ausgewertet, die Zeitdaten
 * koennen mit den Gettern abgerufen werden.
 * FALSE bedeutet, die Auswertung laeuft oder war falsch, die Getter liefern
 * alte Informationen.
 */
boolean MyDCF77::newCycle() {
    static char _updateFromDCF77 = -1;
    static byte _driftTimer = 0;
    boolean retVal = false;
    
    // (1s / MYDCF77_SIGNAL_BINS) sind vorbei
    _binsPointer++;

    if (_updateFromDCF77 > 0)
        _updateFromDCF77--;
    
    // Springe zurück zu Bin 0 und analysiere die letzte Sekunde
    if (_binsPointer >= MYDCF77_SIGNAL_BINS) {
        _binsPointer = 0;

        unsigned int average = (unsigned long) _nPolls * 17 / 100;
        _nPolls = 0;
      
        // Maximumsuche und Summenbildung
        unsigned int imax = 0;
        unsigned int isum = 0;
        byte pos = 0;
        for (byte i = 0; i < MYDCF77_SIGNAL_BINS; i++) {
           if (_bins[i] > imax) {
              imax = _bins[i];
              pos = i;
           }
           isum += _bins[i];
        }
      
        if (isum > average) {
            _bits[_bitsPointer] = 1;
        } else {
            _bits[_bitsPointer] = 0;
        }

        #ifdef DEBUG_SIGNAL
            outputSignal(average, imax, isum);
        #endif

        _bitsPointer++;
        if (_bitsPointer > MYDCF77_TELEGRAMMLAENGE) {
            _bitsPointer = 0;
        }

        if (!isum) {
            if (decode()) {
              /*
               * Signal befindet sich zentriert im Sekundenintervall, 
               * daher 500ms mit dem Einstellen der Uhr warten.
               */
                _updateFromDCF77 = MYDCF77_SIGNAL_BINS / 2;
            }
            clearBits();
        }
        _binsOffset += ((MYDCF77_SIGNAL_BINS-1)/2) - pos;
        _driftTimer++;
    }
    
    // MYDCF77_DRIFT_CORRECTION_TIME Sekunden sind vorbei
    if (_driftTimer >= MYDCF77_DRIFT_CORRECTION_TIME) {
        _driftTimer = 0;
      /* 
       * Wenn Offset < 0, muss gewartet werden
       * Wenn Offset > 0, muss früher begonnen werden
       * Ergänzung: Eigentlich müsste nPolls ebenfalls angepasst werden,
       * da die Berechnung von "average" im Moment der Driftkorrektur 
       * falsch ist.
       * Diese kleine Abweichung wird aber bewusst in Kauf genommen.
       */  
        _binsOffset /= MYDCF77_DRIFT_CORRECTION_TIME;
        _binsPointer = _binsOffset;
        
        if (_binsPointer) {
            DEBUG_PRINT(F("Driftkorrektur erforderlich! Offset: "));
            DEBUG_PRINTLN((int) _binsPointer);
            DEBUG_FLUSH();
        }            
        _binsOffset = 0;
    }

    // Lösche den nächsten Datenpunkt
    if (_binsPointer >= 0) 
        _bins[_binsPointer] = 0;

    if (_updateFromDCF77 == 0) {
        _updateFromDCF77 = -1;
        retVal = true;
    }
    
    return retVal;
}

void MyDCF77::outputSignal(unsigned int average, unsigned int imax, unsigned int isum) {
    unsigned long t1 = (unsigned long) imax * 1000 / DEBUG_SIGNAL_VIS_HEIGHT;
    for (byte i = 0; i < MYDCF77_SIGNAL_BINS; i++ ) {
        if (i < 10) {
            DEBUG_PRINT(F("0"));
        }
        DEBUG_PRINT(i);
        DEBUG_PRINT(F(" "));
        byte signal_height = 0;
        if (_bins[i])
            signal_height = (unsigned long) _bins[i] * 1000 / t1;
        for (byte j = 0; j < signal_height; j++) {
            DEBUG_PRINT("-");
        }
        DEBUG_PRINTLN();
    }
    DEBUG_PRINT(F("Drift: "));
    DEBUG_PRINT((int) _binsOffset);
    DEBUG_PRINT(F(" Average: "));
    DEBUG_PRINT(average);
    DEBUG_PRINT(F(" Highcount: "));
    DEBUG_PRINTLN(isum);
    for (byte i = 0; i < MYDCF77_TELEGRAMMLAENGE; i++) {      
        DEBUG_PRINT(F(" "));
        DEBUG_PRINT(_bits[i]);
        if (i == _bitsPointer) {
          DEBUG_PRINT(".");
        }
    }
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN(_bitsPointer);
    DEBUG_PRINTLN();
    DEBUG_FLUSH();
}

#ifdef DCF77_SENSOR_EXISTS
    /*
     * Vergangene Minuten seit der letzten erfolgreichen DCF-Auswertung bekommen.
     */
    unsigned long MyDCF77::getDcf77LastSuccessSyncMinutes() {
        return _dcf77LastSyncTime.getMinutesOfCentury();
    //    return ( ( (millis() - _dcf77LastSyncTime) / 60000) % 5760 );
    }
    
    /*
     * Zeitpunkt der letzten erfolgreich DCF-Auswertung zurücksetzen.
     */
    void MyDCF77::setDcf77SuccessSync(TimeStamp* _rtc) {
        _dcf77LastSyncTime.set(_rtc);
    }
#endif

/**
 * Die passende Eckled zum Debuggen bekommen.
 */
byte MyDCF77::getDcf77ErrorCorner() { 
    return _errorCorner;
}

/**
 * Decodierung des Telegramms...
 */
byte MyDCF77::decodeHelper(byte *checksum, byte startV, byte endV) {
    byte value = 0;
    for (byte i = startV; i <= endV; i++) {
        if (_bits[i]) {
            value += _bits[i] * DCF77Factors[i-startV];
            (*checksum)++;
        } 
    }
    return value;
}
 
boolean MyDCF77::decode() {
    byte c = 0; // bitcount for checkbit
    boolean ok = true;

    DEBUG_PRINTLN(F("Decoding telegram..."));
    DEBUG_FLUSH();

    if (_bits[0]) {
        ok = false;
        DEBUG_PRINTLN(F("Check-bit M failed."));
        DEBUG_FLUSH();
    }

    if (!_bits[20]) {
        ok = false;
        DEBUG_PRINTLN(F("Check-bit S failed."));
        DEBUG_FLUSH();
    }

    if (_bits[17] == _bits[18]) {
        ok = false;
        DEBUG_PRINTLN(F("Check Z1 != Z2 failed."));
        DEBUG_FLUSH();
    }

    //
    // minutes
    //
    c = 0;
    _minutes = decodeHelper(&c, 21, 27);
    DEBUG_PRINT(F("Minutes: "));
    DEBUG_PRINTLN(_minutes);
    DEBUG_FLUSH();
    if ((c + _bits[28]) % 2) {
        ok = false;
        DEBUG_PRINTLN(F("Check-bit P1: minutes failed."));
        DEBUG_FLUSH();
    }

    //
    // hours
    //
    c = 0;
    _hours = decodeHelper(&c, 29, 34);
    DEBUG_PRINT(F("Hours: "));
    DEBUG_PRINTLN(_hours);
    DEBUG_FLUSH();
    if ((c + _bits[35]) % 2) {
        ok = false;
        DEBUG_PRINTLN(F("Check-bit P2: hours failed."));
        DEBUG_FLUSH();
    }

    //
    // date
    //
    c = 0;
    _date = decodeHelper(&c, 36, 41);
    DEBUG_PRINT(F("Date: "));
    DEBUG_PRINTLN(_date);
    DEBUG_FLUSH();

    //
    // day of week
    //
    _dayOfWeek = decodeHelper(&c, 42, 44);
    DEBUG_PRINT(F("Day of week: "));
    DEBUG_PRINTLN(_dayOfWeek);
    DEBUG_FLUSH();

    //
    // month
    //
    _month = decodeHelper(&c, 45, 49);
    DEBUG_PRINT(F("Month: "));
    DEBUG_PRINTLN(_month);
    DEBUG_FLUSH();

    //
    // year
    //
    _year = decodeHelper(&c, 50, 57);
    DEBUG_PRINT(F("Year: "));
    DEBUG_PRINTLN(_year);
    DEBUG_FLUSH();
    if ((c + _bits[58]) % 2) {
        ok = false;
        DEBUG_PRINTLN(F("Check-bit P3: date failed."));
        DEBUG_FLUSH();
    }

    if (_minutes > 59) {
        DEBUG_PRINTLN(F("Minutes out of range."));
        ok = false;
    }
    if (_hours > 23) {
        DEBUG_PRINTLN(F("Hours out of range."));
        ok = false;
    }
    if (_date > 31) {
        DEBUG_PRINTLN(F("Date out of range."));
        ok = false;
    }
    if (_month > 12) {
        DEBUG_PRINTLN(F("Month out of range."));
        ok = false;
    }

    if (!ok) {
        // discard date...
        _minutes = 0;
        _hours = 0;
        _date = 0;
        _dayOfWeek = 0;
        _month = 0;
        _year = 0;
    }

    return ok;
}

/*
 * Das Bits-Array loeschen.
 */
void MyDCF77::clearBits() {
    for (byte i = 0; i < MYDCF77_TELEGRAMMLAENGE; i++) {
        _bits[i] = 0;
    }
    _bitsPointer = 0;
}

/*
 * Das Bins-Array loeschen.
 */
void MyDCF77::clearBins() {
    for (byte i = 0; i < MYDCF77_SIGNAL_BINS; i++) {
        _bins[i] = 0;
    }   
}

#endif
