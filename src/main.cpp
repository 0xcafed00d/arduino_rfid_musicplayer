#include "Arduino.h"
#include "RFIDReader.h"
#include "SPI.h"
#include "MP3Player.h"
#include "utils.h"

utils::TimeOut inactiveTimer;
const uint32_t offTime = 30000;  // 30 seconds

void setup() {
    // while(!Serial);  // Remove before Flight
    SPI.begin();

    inactiveTimer = utils::TimeOut(offTime);

    RFIDReader::Setup();
    MP3Player::Setup();
}

void loop() {
    utils::UpdateLoggingState();
    delay(10);
    RFIDReader::stateMachine.stateAction();
    MP3Player::stateMachine.stateAction();

    if (MP3Player::IsPlaying()) {
        inactiveTimer = utils::TimeOut(offTime);
    }

    if (inactiveTimer.hasTimedOut()) {
        pinMode(2, OUTPUT);
        digitalWrite(2, HIGH);
    }
}
