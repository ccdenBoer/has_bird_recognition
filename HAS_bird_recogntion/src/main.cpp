// global includes
#include <Arduino.h>
REDIRECT_STDOUT_TO(SerialUSB)
  
#include <HASFSM.h>
#include <SDRAM.h>
  
void setup() {
  Serial.begin(115200);
  SDRAM.begin(SDRAM_START_ADDRESS);
  delay(100);

  InitHASFSM();
  birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
					  FSM_Events::EVENTS_STATE_EXECUTED);
}

void loop() {
  birdSensorFSM.loop();
}

