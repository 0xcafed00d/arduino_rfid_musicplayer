#include "Arduino.h"
#include <MFRC522.h>  // https://github.com/miguelbalboa/rfid
#include "state.h"
#include "MP3Player.h"
#include "utils.h"

namespace RFIDReader {

    State* stateWaitingForCard;
    State* stateReadCard;
    State* stateRestart;
    StateMachine stateMachine;

    const int RST_PIN = 9;
    const int SS_PIN = 10;

    MFRC522 mfrc522(SS_PIN, RST_PIN);
    MFRC522::MIFARE_Key key;

    byte calcTrailerBlock(byte blockAddress) {
        return ((blockAddress + 4) & ~0x3) - 1;
    }

    struct Init : public State {
        void enter() {
            utils::Logln(F("RFID Reader enter INIT"));
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
            utils::Logln();
            utils::Logln();
            utils::Logln(F("RFID Reader enter WaitingForCard"));
        }

        void action() {
            if (mfrc522.PICC_IsNewCardPresent()) {
                stateGoto(stateReadCard);
            }
        }
    } waitingForCard;

    struct ReadCard : public State {
        utils::TimeOut timeout;

        void enter() {
            utils::Logln(F("RFID Reader enter ReadCard"));

            timeout = utils::TimeOut(500);
        }

        bool readFromCard(byte* buffer, byte size, byte blockAddr) {
            byte trailerBlock = calcTrailerBlock(blockAddr);

            utils::Logln(F("Authenticating using key A..."));
            MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
                MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
                utils::Log(F("PCD_Authenticate() failed: "));
                utils::Logln(mfrc522.GetStatusCodeName(status));
                return false;
            }

            // Read data from the block
            utils::Log(F("Reading data from block "));
            utils::Log(blockAddr);
            utils::Logln(F(" ..."));
            status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                utils::Log(F("MIFARE_Read() failed: "));
                utils::Logln(mfrc522.GetStatusCodeName(status));
                return false;
            }

            return true;
        }

        bool writeToCard(byte* buffer, byte size, byte blockAddr) {
            byte trailerBlock = calcTrailerBlock(blockAddr);

            // Authenticate using key B
            Serial.println(F("Authenticating again using key B..."));
            MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
                MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("PCD_Authenticate() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
                return false;
            }

            // Write data to the block
            Serial.print(F("Writing data into block "));
            Serial.print(blockAddr);
            Serial.println(F(" ..."));
            status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, buffer, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Write() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
                return false;
            }
            return true;
        }

        void action() {
            if (timeout.hasTimedOut()) {
                utils::Logln("timedout");
                stateGoto(stateRestart);
            }

            byte buffer[18];
            byte size = sizeof(buffer);

            // strcpy ((char*)buffer, "music:03");

            if (mfrc522.PICC_ReadCardSerial()) {
                utils::Log(F("Card UID:"));
                utils::dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
                utils::Logln("");

                // writeToCard(buffer, size, 1);

                bool ok = readFromCard(buffer, size, 1);

                if (ok) {
                    utils::Log(F("Data in block "));
                    utils::Log(1);
                    utils::Logln(F(":"));
                    utils::dump_byte_array(buffer, 16);
                    utils::Logln("");
                    utils::Logln("");
                    utils::Logln((char*)buffer);

                    if (memcmp((const char*)buffer, "music:", 6) == 0) {
                        int id = (int)strtol((const char*)buffer + 6, NULL, 10);
                        MP3Player::PlayAlbum(id);
                    }
                }
                stateGoto(stateRestart);
            }
        }

        void leave() {
            utils::Logln(F("RFID Reader Leave ReadCard"));
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
        }

    } readCard;

    struct Restart : public State {
        utils::TimeOut timeOut;

        void enter() {
            utils::Logln(F("RFID Reader enter Restart"));
            timeOut = utils::TimeOut(500);
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
