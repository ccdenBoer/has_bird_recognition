// global includes
#include <Arduino.h>
REDIRECT_STDOUT_TO(SerialUSB)

// m7 includes
//#include <LoRaConnection.h>
//#include <CayenneLPP.h>
#include <HASFSM.h>
#include <SDRAM.h>

// m7 defines
//LoRaConnection loraConnection;
//CayenneLPP cayenne(51);

void setup() {
  Serial.begin(115200);
  SDRAM.begin(SDRAM_START_ADDRESS);
//  while (!Serial) {
//	delay(10);
//  }
  delay(100);
  // setupM4Firmware();

//  loraConnection = LoRaConnection();
//
//  loraConnection.InitConnection();
  InitHASFSM();
  birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
					  FSM_Events::EVENTS_STATE_EXECUTED);
}

void loop() {
  birdSensorFSM.loop();

//  Serial.println("Start connection");
//  if (loraConnection.SetOTAAJoin(JOIN, 10)) {
//
//    Serial.println("Connection succeeded");
//
//    cayenne.reset();
//    cayenne.addAnalogInput(1, 1);
//    cayenne.addAnalogInput(2, 2);
//    cayenne.addAnalogInput(3, 3);
//    cayenne.addAnalogInput(4, 4);
//
//    Serial.println("Sending package");
//    Serial.println(loraConnection.SendPacketCayenne(cayenne.getBuffer(), cayenne.getSize(), 10));
//  }
}

