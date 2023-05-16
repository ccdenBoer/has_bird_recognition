#include <Arduino.h>
#include <HASFSM.h>

void setup() {
  InitHASFSM();
  birdSensorFSM.setup(FSM_States::STATE_INITIALIZING, FSM_Events::EVENTS_STATE_EXECUTED);
}

void loop() {
  birdSensorFSM.loop();
}