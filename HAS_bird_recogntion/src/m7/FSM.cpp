#include <FSM.h>
#include <iostream>

FSM::FSM(int totalStates, int totalEvents) {
    this->totalStates = totalStates;
    this->totalEvents = totalEvents;

    this->states.resize(totalStates);
}

void FSM::addState(int state, std::function<void(void)> loopMethod) {
    if ( state < this->totalStates ) {
        std::cout << "Added new state: " + String(state);
        this->states[state] = loopMethod;
    }
}

void FSM::addTransition(int state, int event, int nextState) {
    if ( state < this->totalStates && event < this->totalEvents ) {
        std::cout << "Added transition for state " + String(state) + " and event " + String(event) + ": " + String(nextState);
        this->transitions[state][event] = nextState;
    }
}

void FSM::raiseEvent(int event) {
    std::cout << "Event raised: " + String(event);

    if ( this->currentState != -1 &&  event < this->totalEvents ) {
        std::cout << "Current state is invalid, might be larger than the max state size or negative";

        if ( this->transitions[currentState].find(event) != this->transitions[currentState].end() ) { // Check if event exists for the current state!
            int newState = this->transitions[this->currentState][event]; // get the new state from the transition map
            std::cout << "Found a new state for the current event, which is: " + String(newState);

            this->states[newState]();// Entering new state

            this->currentState = newState;
            std::cout << "switched to new state, which is: " + String(this->currentState);
        } else {
            std::cout << "This event doesn't belong to this state. Currently in the previous event, which is: " + String(this->currentState);
        }
    }
}

void FSM::setup(int state, int eventStateExecuted) {
    if ( this->currentState == -1 || state < this->totalStates ) { // Only set start state if not set yet!
        this->currentState  = state;
        this->lastEvent     = eventStateExecuted;
        this->states[state](); // Call the pre function!
    }

    this->loopTiming = millis();
}

void FSM::loop() {
    this->totalLoopTiming = millis() - this->loopTiming;
    std::cout << "Loop time: " + String(this->totalLoopTiming) + "ms";
    this->loopTiming = millis(); // Restart timing calculation

    std::cout << "Current state: " + String(this->currentState);

    if ( this->currentState != -1 &&  this->currentState < this->totalStates ) {
        this->states[this->currentState]();
        this->raiseEvent(this->lastEvent);

    } else {
        std::cout << "Error in FSM loop, deadlock";
    }
}

unsigned long FSM::getLoopTime() {
    return this->totalLoopTiming;
}