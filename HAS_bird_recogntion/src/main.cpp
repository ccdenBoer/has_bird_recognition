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
#include <SensorData.h>

// m7 defines
SDCardReaderAndWriter sdcard;
SensorData sensors;
#endif

#ifdef TARGET_PORTENTA_H7_M4
// #include <NeuralNetwork.h>
// NeuralNetwork nn = NeuralNetwork();

int led = LEDB;
#endif


void setup() {
#ifdef TARGET_PORTENTA_H7_M7
  Serial.begin(115200);
  sensors = SensorData();
  delay(5000);
  sensors.InitSensors();

  // InitHASFSM();
  // birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
  // FSM_Events::EVENTS_STATE_EXECUTED);
  // bootM4();


#endif
}

void loop() {
  // nn.InputData(nullptr, nullptr, nullptr);
  // birdSensorFSM.loop();
  
#ifdef TARGET_PORTENTA_H7_M7
  Serial.println("Hello from m7");

  float buffer[2];

  Serial.println(sensors.GetLightIntensity());
  Serial.println(sensors.GetRainThreshold());
  Serial.println(sensors.GetRainSurface());
  Serial.println(sensors.GetTemperature());
  Serial.println(sensors.GetHumidity());

  while (!sensors.GetGPSLocation(buffer));

  Serial.print("Location: ");
  Serial.print(buffer[0]);
  Serial.print(", ");
  Serial.println(buffer[1]);

  delay(1000);
  
#endif
}
