#include "Arduino.h"
#include "RFIDReader.h"
#include "SPI.h"
#include "MP3Player.h"

void setup() {
    Serial.begin(9600);
    while (!Serial);
    SPI.begin();

    RFIDReader::Setup();
    MP3Player::Setup();
}

void loop() {
    Serial.print(".");
    delay(10);
    RFIDReader::stateMachine.stateAction();
    MP3Player::stateMachine.stateAction();
}

