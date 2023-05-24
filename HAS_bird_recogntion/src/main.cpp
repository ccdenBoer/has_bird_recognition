// #include "librosa.h"
// #include <HASFSM.h>
// #include <NeuralNetwork.h>
#include <Arduino.h>
#include <SDCardReaderAndWriter.h>

// NeuralNetwork nn = NeuralNetwork();

// void setup() {
//   // InitHASFSM();
//   // birdSensorFSM.setup(FSM_States::STATE_INITIALIZING, FSM_Events::EVENTS_STATE_EXECUTED);
//   // Serial.begin(115200);



// }

// void loop() {
//   // nn.InputData(nullptr, nullptr, nullptr);
//   // birdSensorFSM.loop();
//   delay(1000);
//   // Serial.println("Hello World");

// }
int myLED;
SDCardReaderAndWriter sdcard;

void setup() {

   randomSeed(analogRead(0));

   #ifdef CORE_CM7  
      bootM4();  
      myLED = LEDB; // built-in blue LED
   #endif

   #ifdef CORE_CM4  
      myLED = LEDG; // built-in greeen LED
   #endif   
   pinMode(myLED, OUTPUT);

   Serial.begin(9600);
   Serial.println("Hello world!");

  while (sdcard.InitSDCardReaderAndWriter()) {
    delay(1000);
  }
}

void loop() {
    Serial.println("Start write");

    sdcard.WriteToSDCard(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    Serial.println("Data written");

    delay(1000);
} 