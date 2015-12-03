//
// Created by lmw on 03/12/15.
//
#include "Arduino.h"
#include <MFRC522.h>
#include "state.h"

namespace RFIDReader {

    State *stateWaitingForCard;
    State *stateReadCard;
    StateMachine stateMachine;

    const int RST_PIN = 9;
    const int SS_PIN = 10;

    MFRC522 mfrc522(SS_PIN, RST_PIN);
    MFRC522::MIFARE_Key key;

    struct Init : public State {
        void action() {
            stateGoto(stateWaitingForCard);
        }
    } init;

    struct WaitingForCard : public State {
        void action() {

        }
    } waitingForCard;

    struct ReadCard : public State {

        void action() {

        }
    } readCard;

    void Setup() {
        stateWaitingForCard = &waitingForCard;
        stateReadCard = &readCard;
        stateMachine.stateGoto(&init);
    }
}
