//
// Created by lmw on 03/12/15.
//

#include "Arduino.h"
#include "state.h"

/************Command byte**************************/
#define CMD_NEXT_SONG 0X01
#define CMD_PREV_SONG 0X02
#define CMD_PLAY_W_INDEX 0X03
#define CMD_VOLUME_UP 0X04
#define CMD_VOLUME_DOWN 0X05
#define CMD_SET_VOLUME 0X06
#define CMD_SINGLE_CYCLE_PLAY 0X08
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_SLEEP_MODE 0X0A
#define CMD_WAKE_UP 0X0B
#define CMD_RESET 0X0C
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_PLAY_FOLDER_FILE 0X0F
#define CMD_STOP_PLAY 0X16
#define CMD_FOLDER_CYCLE 0X17
#define CMD_SHUFFLE_PLAY 0X18
#define CMD_SET_SINGLE_CYCLE 0X19
#define SINGLE_CYCLE_ON 0X00
#define SINGLE_CYCLE_OFF 0X01
#define CMD_SET_DAC 0X1A
#define DAC_ON  0X00
#define DAC_OFF 0X01
#define CMD_PLAY_W_VOL 0X22

namespace MP3Player {

    State *stateWaitForAlbumEnd;
    State *stateWaitPlay;
    StateMachine stateMachine;

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
//                sendCommand(CMD_PLAY_FOLDER_FILE, (byte)playTrack, 1);
                sendCommand(CMD_PLAY_W_INDEX, 0, 1);
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
        playTrack = id;
    }
}

