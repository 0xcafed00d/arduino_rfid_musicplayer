//
// Created by lmw on 10/12/15.
//

#include "utils.h"

void logln () {
    Serial.println();
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        log(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}