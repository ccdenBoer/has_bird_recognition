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
  // nn.InputData(nullptr, nullptr, nullptr);
  // birdSensorFSM.loop();

#ifdef TARGET_PORTENTA_H7_M7
  /*
  delay(1000);
  Serial.println("Hello from m7");
  */
 float buffer[2];

  Serial.println("Setup start");
  sensor.GetGPSLocation(buffer);
  //loraConnection.InitialSetup();
  Serial.println("Setup done");
  delay(5000);
#endif

#ifdef TARGET_PORTENTA_H7_M4
/*
  delay(100);
  Serial.println("Hello from m4");
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  */
#endif

#ifdef TARGET_PORTENTA_H7_M7
/*
  String data = "";
  while (RPC.available())
  {
    data += (char)RPC.read();
  }
  if (data.length() > 0)
  {
    Serial.println(data);
  }
  */
#endif
}
