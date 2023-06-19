#include <FSM.h>
#include <utility>
#include <Arduino.h>


FSM::FSM(int totalStates, int totalEvents) {
    this->totalStates = totalStates;
    this->totalEvents = totalEvents;

    this->states.resize(totalStates);
}

void FSM::addState(int state, std::function<void(void)> loopMethod) {
    if ( state < this->totalStates ) {
		printf("Added new state: %d\n", state);
	  	this->states[state] = std::move(loopMethod);
    }
}

void FSM::addTransition(int state, int event, int nextState) {
    if ( state < this->totalStates && event < this->totalEvents ) {
		printf("Added transition for state %d and event %d: %d\n", state, event, nextState);
        this->transitions[state][event] = nextState;
    }
}

void FSM::raiseEvent(int event) {
   	printf("Event raised: %d\n", event);

    if ( this->currentState != -1 &&  event < this->totalEvents ) {
        if ( this->transitions[currentState].find(event) != this->transitions[currentState].end() ) { // Check if event exists for the current state!
            int newState = this->transitions[this->currentState][event]; // get the new state from the transition map
			printf("Found a new state for the current event, which is: %d\n", newState);

            this->states[newState]();// Entering new state

            this->currentState = newState;
			printf("switched to new state, which is: %d\n", this->currentState);
        } else {
		  printf("This event doesn't belong to this state. Currently in the previous event, which is: %d\n", this->currentState);
        }
    }else{
	  printf("Current state is invalid, might be larger than the max state size or negative");
	}
}

void FSM::setup(int state, int eventStateExecuted) {
  	printf("Setup FSM\n");
    if ( this->currentState == -1 || state < this->totalStates ) { // Only set start state if not set yet!
        this->currentState  = state;
        this->lastEvent     = eventStateExecuted;
        this->states[state](); // Call the pre function!
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
        this->raiseEvent(this->lastEvent);

    } else {
        printf("Error in FSM loop, deadlock");
    }
}

unsigned long FSM::getLoopTime() {
    return this->totalLoopTiming;
}