//
// Created by lmw on 10/12/15.
//

#ifndef ARDUINO_RFID_MUSICPLAYER_UTILS_H
#define ARDUINO_RFID_MUSICPLAYER_UTILS_H

#include "Arduino.h"


class TimeOut {
    uint32_t end;
public:
    TimeOut (uint32_t timeoutms = 0) {
        end = (uint32_t)millis() + timeoutms;
    }

    bool hasTimedOut () const {
        return millis() > end;
    }
};

template <typename T>
void log (T v) {
    Serial.print(v);
}

template <typename T>
void logln (T v) {
    Serial.println(v);
}

void logln ();
void dump_byte_array(byte *buffer, byte bufferSize);



#endif //ARDUINO_RFID_MUSICPLAYER_UTILS_H
