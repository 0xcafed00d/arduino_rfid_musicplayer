//
// Created by lmw on 10/12/15.
//

#include "utils.h"

void logln () {
    if (loggingEnabled())
        Serial.println();
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    if (loggingEnabled()){
        for (byte i = 0; i < bufferSize; i++) {
            log(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], HEX);
        }
    }
}

static bool loggingState = false;

bool enableLogging(bool enable) {
    loggingState = enable;
    return true;
}

bool loggingEnabled() {
    return loggingState;
}
