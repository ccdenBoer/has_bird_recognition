// global includes
#include "RPC.h"
#include <Arduino.h>

#ifdef TARGET_PORTENTA_H7_M4
#define Serial RPC
#include <NeuralNetwork.h>

NeuralNetwork nn;
float **mockdata;
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

int led = LEDB;
#endif


void setup() {
#ifdef TARGET_PORTENTA_H7_M7
  Serial.begin(115200);
  setupM4Firmware();
  // InitHASFSM();
  // birdSensorFSM.setup(FSM_States::STATE_INITIALIZING,
  // FSM_Events::EVENTS_STATE_EXECUTED);
#endif
RPC.begin();

#ifdef TARGET_PORTENTA_H7_M4
  nn = NeuralNetwork();
  pinMode(led, OUTPUT);
  mockdata = new float*[128];
  for(int i = 0; i<128; i++){
    mockdata[i] = new float[547];
  }
  for(int x; x < 128; x++){
    for(int y; y < 547; y++){
        mockdata[x][y] = 1;
      }
   }
#endif
}

void loop() {
  // nn.InputData(nullptr, nullptr, nullptr);
  // birdSensorFSM.loop();
  
#ifdef TARGET_PORTENTA_H7_M7
  delay(1000);
  Serial.println("Hello from m7");
#endif

#ifdef TARGET_PORTENTA_H7_M4
  delay(100);
  Serial.println("Hello from m4");
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  nn.InputData(mockdata);
  int predict = nn.ScanData();
  char buffer[100];
  sprintf(buffer, "Predicted %d \n", predict);
  Serial.println(buffer);

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
