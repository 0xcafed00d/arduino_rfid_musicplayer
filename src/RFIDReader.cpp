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

    void dump_byte_array(byte *buffer, byte bufferSize) {
        for (byte i = 0; i < bufferSize; i++) {
            log(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], HEX);
        }
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
            logln(' ');
            logln(' ');
            logln(F("RFID Reader enter WaitingForCard"));
        }

        void action() {
            if (mfrc522.PICC_IsNewCardPresent()){
                stateGoto(stateReadCard);
            }
        }
    } waitingForCard;

    struct ReadCard : public State {
        TimeOut timeout;

        void enter() {
            logln(F("RFID Reader enter ReadCard"));
            timeout = TimeOut(500);
        }

        bool readFromCard (byte* buffer, byte size) {
            logln(F("Authenticating using key A..."));
            MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                                                                        1, &key, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
                log(F("PCD_Authenticate() failed: "));
                logln(mfrc522.GetStatusCodeName(status));
                return false;
            }

            // Read data from the block
            log(F("Reading data from block ")); log(1);
            logln(F(" ..."));
            status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(1, buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                log(F("MIFARE_Read() failed: "));
                logln(mfrc522.GetStatusCodeName(status));
                return false;
            }

            return true;
        }

        bool writeToCard (byte* buffer, byte size) {

            return false;
        }

        void action() {
            if (timeout.hasTimedOut()) {
                logln("timedout");
                stateGoto(stateRestart);
            }

            byte buffer[18];
            byte size = sizeof(buffer);

            if (mfrc522.PICC_ReadCardSerial()) {
                log(F("Card UID:"));
                dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
                logln("");

                bool ok = readFromCard(buffer, size);

                if (ok){
                    log(F("Data in block ")); log(1); logln(F(":"));
                    dump_byte_array(buffer, 16); logln("");
                    logln("");
                }
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
            timeOut = TimeOut(500);
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
