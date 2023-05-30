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
#include "FirmwareLoader.h"

// m7 defines
SDCardReaderAndWriter sdcard;
#endif

#ifdef TARGET_PORTENTA_H7_M4
// #include <NeuralNetwork.h>
// NeuralNetwork nn = NeuralNetwork();

int led = LEDB;
int delay_ms = 1000;
#endif


void setup() {
#ifdef TARGET_PORTENTA_H7_M7
  Serial.begin(115200);
  while (!Serial);
  setupM4Firmware();

  // InitHASFSM();
  // birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
  // FSM_Events::EVENTS_STATE_EXECUTED);
#endif
RPC.begin();

#ifdef TARGET_PORTENTA_H7_M4
  pinMode(led, OUTPUT);
#endif
}

void loop() {
  // nn.InputData(nullptr, nullptr, nullptr);
  // birdSensorFSM.loop();
  delay(1000);
#ifdef TARGET_PORTENTA_H7_M7
  Serial.println("Hello from m7");
#endif

#ifdef TARGET_PORTENTA_H7_M4
  Serial.println("Hello from m4");
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
#endif

#ifdef TARGET_PORTENTA_H7_M7
  while (RPC.available()) {
    Serial.write(RPC.read());
  }
#endif
}
