#include <Arduino.h>
#include <HASFSM.h>
#include <NeuralNetwork.h>

NeuralNetwork nn = NeuralNetwork();

void setup() {
  InitHASFSM();
  birdSensorFSM.setup(FSM_States::STATE_INITIALIZING, FSM_Events::EVENTS_STATE_EXECUTED);
}

void loop() {
  nn.InputData(nullptr, nullptr, nullptr);
  birdSensorFSM.loop();
  delay(1000);
}