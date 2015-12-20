//
// Created by lmw on 03/12/15.
//

#include "Arduino.h"
#include "state.h"
#include "utils.h"

/************Command byte**************************/
#define CMD_SET_VOLUME 0X06
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY_FOLDER_FILE 0X0F
#define CMD_STOP_PLAY 0X16
#define CMD_QUERY_FLDR_TRACKS 0x4e

#define RESP_MEDIA_REMOVED      0x3b
#define RESP_MEDIA_INSERTED     0x3a
#define RESP_TF_TRACK_FINISHED  0x3d
#define RESP_ERROR              0x40
#define RESP_ACK                0x41
#define RESP_FLDR_TRACK_COUNT   0x4e


namespace MP3Player {

    State *stateWaitForAlbumEnd;
    State *stateWaitPlay;
    StateMachine stateMachine;

    static byte response[16];

    int playTrack = -1;

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

    void CMDStop () {
        sendCommand(CMD_STOP_PLAY, 0, 0);
    }

    bool ProcessResponse (byte* code, int* value) {
        return false;
    }

    struct Init : public State {
        void enter() {
            Serial1.begin(9600);
            sendCommand(CMD_SEL_DEV, 0, DEV_TF);
            stateGoto(stateWaitPlay);
        }
    } init;

    struct WaitPlay : public State {
        void action() {
            if (playTrack != -1) {
                CMDPlay(1,1);
                stateGoto(stateWaitForAlbumEnd);
            }
        }

        void leave() {
            playTrack = -1;
        };
    } waitPlay;


    struct WaitForAlbumEnd : public State {
        void action() {

        }
    } waitForAlbumEnd;


    void Setup() {
        stateWaitPlay = &waitPlay;
        stateWaitForAlbumEnd = &waitForAlbumEnd;
        stateMachine.stateGoto(&init);
    }

    void PlayAlbum(int id) {
        utils::Log(F("Requesting Album "));
        utils::Logln(id);
        playTrack = id;
    }
}

