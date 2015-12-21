//
// Created by lmw on 03/12/15.
//

#include "Arduino.h"
#include "state.h"
#include "utils.h"

/************Command byte**************************/
#define CMD_SET_VOLUME 0x06
#define CMD_SEL_DEV 0x09
#define DEV_TF 0x02
#define CMD_PLAY_FOLDER_FILE 0x0F
#define CMD_RESET 0x0C
#define CMD_STOP_PLAY 0x16
#define CMD_QUERY_FLDR_TRACKS 0x4e

#define RESP_MEDIA_REMOVED      0x3b
#define RESP_MEDIA_INSERTED     0x3a
#define RESP_TF_TRACK_FINISHED  0x3d
#define RESP_ERROR              0x40
#define RESP_ACK                0x41
#define RESP_FLDR_TRACK_COUNT   0x4e


namespace MP3Player {

    static State *stateWaitPlay;
    static State *stateQueryTrackCount;
    static State *statePlay;
    StateMachine stateMachine;
    static byte response[16];
    static size_t respPos;

    int requestedAlbum = -1;
    int requestedAlbumTrackCount = 0;
    int requestedAlbumCurrTrack = 0;

    void sendCommand(byte command, byte data1, byte data2)
    {
        byte buffer[8];

        delay(20);
        buffer[0] = 0x7e; //starting byte
        buffer[1] = 0xff; //version
        buffer[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
        buffer[3] = command; //
        buffer[4] = 0x00;
        buffer[5] = data1;
        buffer[6] = data2;
        buffer[7] = 0xef; //ending byte
        for(uint8_t i=0; i<8; i++)//
        {
            Serial1.write(buffer[i]) ;
        }
    }

    void CMDSelectTF () {
        sendCommand(CMD_SEL_DEV, 0, DEV_TF);
    }

    void CMDSetVolume (byte volume) {
        sendCommand(CMD_SET_VOLUME, 0, volume);
    }

    void CMDTrackCount (byte album) {
        sendCommand(CMD_QUERY_FLDR_TRACKS, 0, album);
    }

    void CMDPlay (byte album, byte track) {
        sendCommand(CMD_PLAY_FOLDER_FILE, album, track);
    }

    void CMDReset () {
        sendCommand(CMD_RESET, 0, 0);
    }

    void CMDStop () {
        sendCommand(CMD_STOP_PLAY, 0, 0);
    }

    bool ProcessResponse (byte* code, uint16_t* value) {
        int c;

        while ((c = Serial1.read()) != -1) {
            if (response[0] == 0) {
                if (c == 0x7e){
                    response[0] = byte(c);
                    respPos = 1;
                }
            } else {
                response[respPos] = byte(c);
                if (response[respPos] == 0xef) {
                    utils::dump_byte_array(response, 16);
                    utils::Logln();
                    *code = response[3];
                    *value = (uint16_t(response[5])<<8) + uint16_t(response[6]);
                    response[0] = 0;
                    return true;
                }
                respPos++;
            }

        }
        return false;
    }

    struct Init : public State {
        void enter() {
            respPos = 0;
            response[0] = 0;
            Serial1.begin(9600);
            CMDSelectTF();
        }

        void action() {
            byte code;
            uint16_t value;

            if (ProcessResponse(&code, &value)){
                if (code == RESP_MEDIA_INSERTED && value == DEV_TF) {
                    stateGoto(stateWaitPlay);
                }
            }
        }
    } init;

    // ======================================================================================

    struct WaitPlay : public State {
        void enter() {
            requestedAlbum = -1;
        }

        void action() {
            if (requestedAlbum != -1) {
                stateGoto(stateQueryTrackCount);
            }
        }
    } waitPlay;

    // ======================================================================================

    struct QueryTrackCount : public State {
        void enter() {
            CMDTrackCount(requestedAlbum);
        }

        void action() {
            byte code;
            uint16_t value;

            if (ProcessResponse(&code, &value)){
                if (code == RESP_FLDR_TRACK_COUNT) {
                    requestedAlbumTrackCount = (int)value;
                    requestedAlbumCurrTrack = 1;
                    stateGoto(statePlay);
                    return;
                }

                if (code == RESP_ERROR) {
                    stateGoto(stateWaitPlay);
                    return;
                }
            }
        }
    } queryTrackCount;

    // ======================================================================================

    struct Play : public State {
        int endMessageCount;

        void enter() {
            endMessageCount = 0;
            CMDPlay((byte)requestedAlbum, (byte)requestedAlbumCurrTrack);
        }

        void action() {
            byte code;
            uint16_t value;

            if (ProcessResponse(&code, &value)) {
                if (code == RESP_TF_TRACK_FINISHED) {
                    endMessageCount++;
                    if (endMessageCount == 2) { // track finished is sent twice for some reason.
                        requestedAlbumCurrTrack++;
                        stateGoto(statePlay);
                        return;
                    }
                }

                if (code == RESP_ERROR) {
                    stateGoto(stateWaitPlay);
                    return;
                }
            }
        }

        void leave() {
        };
    } play;

    // ======================================================================================


    void Setup() {
        stateWaitPlay = &waitPlay;
        stateQueryTrackCount = &queryTrackCount;
        statePlay = &play;
        stateMachine.stateGoto(&init);
    }

    void PlayAlbum(int id) {
        utils::Log(F("Requesting Album "));
        utils::Logln(id);
        requestedAlbum = id;
    }
}

