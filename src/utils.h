#ifndef ARDUINO_RFID_MUSICPLAYER_UTILS_H
#define ARDUINO_RFID_MUSICPLAYER_UTILS_H

#include "Arduino.h"

namespace utils {

    class TimeOut {
        uint32_t end;

       public:
        TimeOut(uint32_t timeoutms = 0) {
            end = (uint32_t)millis() + timeoutms;
        }

        bool hasTimedOut() const {
            return millis() > end;
        }
    };

    void UpdateLoggingState();
    bool loggingEnabled();

    template <typename T>
    void Log(T v) {
        if (loggingEnabled())
            Serial.print(v);
    }

    template <typename T>
    void Logln(T v) {
        if (loggingEnabled())
            Serial.println(v);
    }

    void Logln();
    void dump_byte_array(byte* buffer, byte bufferSize);
}

#endif  // ARDUINO_RFID_MUSICPLAYER_UTILS_H
