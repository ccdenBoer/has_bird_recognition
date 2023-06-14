// global includes
#include "RPC.h"
#include <Arduino.h>

// m7 includes
#include <Arduino_PortentaBreakout.h>
#include <LoRaConnection.h>
#include <CayenneLPP.h>
#include <SDRAM.h>
#include <NeuralNetwork.h>

// m7 defines
LoRaConnection loraConnection;
CayenneLPP cayenne(51);

void setup()
{
  Serial.begin(115200);
  // setupM4Firmware();

  loraConnection = LoRaConnection();

  loraConnection.InitConnection();
  // InitHASFSM();
  // birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
  // FSM_Events::EVENTS_STATE_EXECUTED);
  // bootM4();
}

void loop()
{
  Serial.println("Start connection");
  if (loraConnection.SetOTAAJoin(JOIN, 10)) {

    Serial.println("Connection succeeded");

    cayenne.reset();
    cayenne.addAnalogInput(1, 1);
    cayenne.addAnalogInput(2, 2);
    cayenne.addAnalogInput(3, 3);
    cayenne.addAnalogInput(4, 4);

    Serial.println("Sending package");
    Serial.println(loraConnection.SendPacketCayenne(cayenne.getBuffer(), cayenne.getSize(), 10));
  }
}
