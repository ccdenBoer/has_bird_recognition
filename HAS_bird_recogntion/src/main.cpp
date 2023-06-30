// global includes
#include <Arduino.h>
REDIRECT_STDOUT_TO(SerialUSB)
  
#include <HASFSM.h>
#include <SDRAM.h>
#include <Scheduler.h>

HASFSM* hasFSM;


void toggleLed() {
  digitalWrite(LEDB, !digitalRead(LEDB));
  yield();
  delay(500);
}

#define USB_CONNECTED
void setup() {
  Serial.begin(115200);
  SDRAM.begin(SDRAM_START_ADDRESS);
  pinMode(LEDB, OUTPUT);
  Scheduler.startLoop(toggleLed, 1000);

#ifdef USB_CONNECTED
  while (!Serial) {
	delay(100);
  }
  delay(100);
#endif

  delay(100);

  hasFSM = new HASFSM();
  hasFSM->InitHASFSM();
  printf("Init done\n");
  hasFSM->birdSensorFSM.setup(FSM_States::STATE_INITIALIZING);
  printf("Setup done\n");
}

void loop() {
  hasFSM->birdSensorFSM.loop();
}



