#include <Arduino.h>
#include <HASFSM.h>

HASFiniteStateMachine hasFSM;

void setup() {
  hasFSM = HASFiniteStateMachine();

  hasFSM.InitHASFSM();
  hasFSM.birdSensorFSM.setup(FSM_States::STATE_INITIALIZING, FSM_Events::EVENTS_STATE_EXECUTED);
}

void loop() {
  hasFSM.birdSensorFSM.loop();
}