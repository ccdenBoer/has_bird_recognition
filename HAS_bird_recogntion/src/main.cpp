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
// #include "FirmwareLoader.h"

// m7 defines
SDCardReaderAndWriter sdcard;
LoRaConnection loraConnection;
SensorData sensor;
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
    loraConnection.SetOTAAJoin(JOIN, 10);

    Serial.println("Connection succeeded");

    Serial.println("Sending package");
    Serial.println(loraConnection.SendPacket("Test", 10));
  }

  delay(10000);
}
