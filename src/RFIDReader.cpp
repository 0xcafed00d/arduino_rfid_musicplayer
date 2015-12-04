//
// Created by lmw on 03/12/15.
//
#include "Arduino.h"
#include <MFRC522.h>
#include "state.h"

class TimeOut {
    uint32_t end;
public:
    TimeOut (uint32_t timeoutms = 0) {
        end = (uint32_t)millis() + timeoutms;
    }

    bool hasTimedOut () const {
        return millis() > end;
    }
};

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

namespace RFIDReader {

    State *stateWaitingForCard;
    State *stateReadCard;
    State *stateRestart;
    StateMachine stateMachine;

    const int RST_PIN = 9;
    const int SS_PIN = 10;

    MFRC522 mfrc522(SS_PIN, RST_PIN);
    MFRC522::MIFARE_Key key;

    template <typename T>
    void log (T v) {
        Serial.print(v);
    }

    template <typename T>
    void logln (T v) {
        Serial.println(v);
    }

    struct Init : public State {

        void enter() {
            logln(F("RFID Reader enter INIT"));
            mfrc522.PCD_Init();

            // Prepare the key (used both as key A and as key B)
            // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
            for (byte i = 0; i < 6; i++) {
                key.keyByte[i] = 0xFF;
            }
            stateGoto(stateWaitingForCard);
        }
    } init;

    struct WaitingForCard : public State {
        void enter() {
            Serial.println();
            Serial.println();
            logln(F("RFID Reader enter WaitingForCard"));
        }

        void action() {
            if (mfrc522.PICC_IsNewCardPresent()){
                logln(F("Got card"));
                stateGoto(stateReadCard);
            }
        }
    } waitingForCard;

    struct ReadCard : public State {
        bool gotCard;
        TimeOut timeout;

        void enter() {

            logln(F("RFID Reader enter ReadCard"));
            gotCard = false;
            timeout = TimeOut(1000);
        }

        void action() {
            if (timeout.hasTimedOut()) {
                logln("timedout");
                stateGoto(stateRestart);
            }

            if (mfrc522.PICC_ReadCardSerial()) {
                Serial.print(F("Card UID:"));
                dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
                Serial.println();

                logln("got");
                stateGoto(stateRestart);
            }
        }

        void leave() {
            logln(F("RFID Reader Leave ReadCard"));
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
        }

    } readCard;

    struct Restart : public State {
        TimeOut timeOut;

        void enter() {
            logln(F("RFID Reader enter Restart"));
            timeOut = TimeOut(5000);
        }

        void action() {
            if (timeOut.hasTimedOut()) {
                stateGoto(stateWaitingForCard);
            }
        }
    } restart;


    void Setup() {
        stateWaitingForCard = &waitingForCard;
        stateReadCard = &readCard;
        stateRestart = &restart;
        stateMachine.stateGoto(&init);
    }
}
