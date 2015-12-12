#include "Arduino.h"
#include "RFIDReader.h"
#include "SPI.h"
#include "MP3Player.h"
#include "utils.h"


void setup() {
    enableLogging(true);

    SPI.begin();

    RFIDReader::Setup();
    MP3Player::Setup();
}

void loop() {
    delay(10);
    RFIDReader::stateMachine.stateAction();
    MP3Player::stateMachine.stateAction();
}

