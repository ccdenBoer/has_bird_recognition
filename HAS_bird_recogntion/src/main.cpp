// global includes
#include <Arduino.h>
REDIRECT_STDOUT_TO(SerialUSB)
  
#include <HASFSM.h>
#include <SDRAM.h>

HASFSM* hasFSM;
void setup() {
  Serial.begin(115200);
  SDRAM.begin(SDRAM_START_ADDRESS);
  while (!Serial)
  {
	delay(100);
  }

  delay(100);

  hasFSM = new HASFSM();
  hasFSM->InitHASFSM();
  printf("Init done\n");
  hasFSM->birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
					  FSM_Events::EVENTS_STATE_EXECUTED);
  printf("Setup done\n");
}

void loop() {
  printf("Looping\n");
  hasFSM->birdSensorFSM.loop();
}

