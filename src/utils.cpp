//
// Created by lmw on 10/12/15.
//

#include "utils.h"

void Logln() {
    if (loggingEnabled())
        Serial.println();
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    if (loggingEnabled()){
        for (byte i = 0; i < bufferSize; i++) {
            Log(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], HEX);
        }
    }
}

static bool loggingState = false;

bool enableLogging(bool enable) {
    TimeOut to (10000);

    Serial.begin(9600);
    while (!Serial) {
        if (to.hasTimedOut())
            return false;
    }
    Serial.println("asdfajlfj");

    loggingState = enable;
    return true;
}

bool loggingEnabled() {
    return loggingState;
}
