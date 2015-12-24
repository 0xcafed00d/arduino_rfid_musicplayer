#include "utils.h"

namespace utils {

    void Logln() {
        if (loggingEnabled())
            Serial.println();
    }

    void dump_byte_array(byte* buffer, byte bufferSize) {
        if (loggingEnabled()) {
            for (byte i = 0; i < bufferSize; i++) {
                Log(buffer[i] < 0x10 ? " 0" : " ");
                Serial.print(buffer[i], HEX);
            }
        }
    }

    static bool loggingState = false;

    bool loggingEnabled() {
        return loggingState;
    }

    void UpdateLoggingState() {
        loggingState = bool(Serial);
    }
}
