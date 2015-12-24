#include "state.h"

void State::stateGoto(State* state) {
	sm->stateGoto(state);
}

void StateMachine::stateGoto(State* state) {
	state->sm = this;

	if (currentState)
		currentState->leave();

	currentState = state;

	if (currentState)
		currentState->enter();
}

void StateMachine::stateAction() {
	if (currentState)
		currentState->action();
}
