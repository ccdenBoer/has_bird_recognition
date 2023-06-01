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
#include <NeuralNetwork.h>

// m7 defines
SDCardReaderAndWriter sdcard;
NeuralNetwork nn;
float mockdata[128][547][1];
#endif

#ifdef TARGET_PORTENTA_H7_M4

int led = LEDB;
#endif


void setup() {
#ifdef TARGET_PORTENTA_H7_M7
  Serial.begin(115200);
  setupM4Firmware();
  // InitHASFSM();
  // birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
  // FSM_Events::EVENTS_STATE_EXECUTED);
   nn = NeuralNetwork();
#endif
RPC.begin();

#ifdef TARGET_PORTENTA_H7_M4
  pinMode(led, OUTPUT);
   // for(int x; x < 128; x++){
   //    for(int y; y < 547; y++){
   //       mockdata[x][y][0] = 1;
   //    }
   // }
#endif
}

void loop() {
  // nn.InputData(nullptr, nullptr, nullptr);
  // birdSensorFSM.loop();
  
#ifdef TARGET_PORTENTA_H7_M7
  delay(1000);
  Serial.println("Hello from m7");
    nn.InputData(mockdata);
    int predict = nn.ScanData();
  char buffer[100];
  sprintf(buffer, "Predicted %d \n", predict);
  Serial.println(buffer);
#endif

#ifdef TARGET_PORTENTA_H7_M4
  delay(100);
  Serial.println("Hello from m4");
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);

#endif

#ifdef TARGET_PORTENTA_H7_M7
  String data = "";
  while (RPC.available()) {
    data += (char)RPC.read();
  }
  if (data.length() > 0) {
    Serial.println(data);
  }
#endif
}
