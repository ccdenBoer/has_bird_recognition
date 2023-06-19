#include <FSM.h>
#include <utility>
#include <Arduino.h>


FSM::FSM(int totalStates, int totalEvents) {
    this->totalStates = totalStates;
    this->totalEvents = totalEvents;
    this->states.resize(totalStates);
}

void FSM::addState(state_t state, std::function<void(void)> loopMethod) {
    if ( state < this->totalStates ) {
		printf("Added new state: %d\n", state);
	  	this->states[state] = std::move(loopMethod);
    }
}

void FSM::addTransition(state_t state, event_t event, state_t nextState) {
    if ( state < this->totalStates && event < this->totalEvents ) {
		printf("Added transition for state %d and event %d: %d\n", state, event, nextState);
        this->transitions[state][event] = nextState;
    }
}

void FSM::raiseEvent(event_t event) {
   	printf("Event raised: %d\n", event);

    if ( this->currentState != -1 &&  event < this->totalEvents ) {
	  // Check if event exists for the current state!
	  auto state = this->transitions[this->currentState].find(event);

	  if (state != this->transitions[this->currentState].end()) {
            this->currentState = state->second;
			printf("switched to new state, which is: %d\n", this->currentState);
        } else {
		// print error about current state and event that was raised
		printf("This event doesn't belong to this state. Current state: %d\t ,Event that was raised: %d", this->currentState,event);
        }
    }else{
	  printf("Current state is invalid, might be larger than the max state size or negative");
	}
}

void FSM::setup(state_t state) {
  	printf("Setup FSM\n");
    if ( this->currentState == -1 || state < this->totalStates ) { // Only set start state if not set yet!
        this->currentState  = state;
    }

	printf("Setup done\n");
    this->loopTiming = millis();
}

void FSM::loop() {
    this->totalLoopTiming = millis() - this->loopTiming;
	printf("Loop time: %ld ms\n", this->totalLoopTiming);
    this->loopTiming = millis(); // Restart timing calculation

    printf("Current state: %d\n", this->currentState);

    if ( this->currentState != -1 &&  this->currentState < this->totalStates ) {
        this->states[this->currentState]();
//        this->raiseEvent(this->lastEvent);
    } else {
        printf("Error in FSM loop, deadlock");
    }
}

unsigned long FSM::getLoopTime() {
    return this->totalLoopTiming;
}