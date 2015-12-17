#include "Arduino.h"
#include "RFIDReader.h"
#include "SPI.h"
#include "MP3Player.h"
#include "utils.h"


void setup() {
    SPI.begin();

    RFIDReader::Setup();
    MP3Player::Setup();
}

void loop() {
    utils::UpdateLoggingState();
    delay(10);
    RFIDReader::stateMachine.stateAction();
    MP3Player::stateMachine.stateAction();
}

