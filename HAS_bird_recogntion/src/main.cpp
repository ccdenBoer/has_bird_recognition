// global includes
#include "RPC.h"
#include <Arduino.h>

#ifdef TARGET_PORTENTA_H7_M4
#define Serial RPC
#endif

#ifdef TARGET_PORTENTA_H7_M7
// m7 includes
#include <Arduino_PortentaBreakout.h>
#include <SDCardReaderAndWriter.h>
#include <LoRaConnection.h>
#include <SensorData.h>
#include <CayenneLPP.h>
// #include "FirmwareLoader.h"

// m7 defines
SDCardReaderAndWriter sdcard;
LoRaConnection loraConnection;
SensorData sensor;
CayenneLPP cayenne(51);
#endif

#ifdef TARGET_PORTENTA_H7_M4
// #include <NeuralNetwork.h>
// NeuralNetwork nn = NeuralNetwork();

int led = LEDB;
#endif

void setup()
{
#ifdef TARGET_PORTENTA_H7_M7
  Serial.begin(115200);
  // setupM4Firmware();

  loraConnection = LoRaConnection();
  sensor = SensorData();

  loraConnection.InitConnection();

  // InitHASFSM();
  // birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
  // FSM_Events::EVENTS_STATE_EXECUTED);
  // bootM4();
#endif
  RPC.begin();

#ifdef TARGET_PORTENTA_H7_M4
  pinMode(led, OUTPUT);
#endif
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

  delay(10000);
}
