//
// Created by lmw on 03/12/15.
//

#include "Arduino.h"
#include "state.h"

namespace MP3Player {

    State *stateWaitForAlbumEnd;
    StateMachine stateMachine;

    struct Init : public State {
        void enter() {
            Serial1.begin(9600);
        }

        void action() {
        }
    } init;

    struct WaitForAlbumEnd : public State {
        void action() {

        }
    } waitForAlbumEnd;


    void Setup() {
        stateWaitForAlbumEnd = &waitForAlbumEnd;
        stateMachine.stateGoto(&init);
    }

    void PlayAlbum(int id) {

    }
}

