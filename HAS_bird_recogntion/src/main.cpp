// global includes
#include "RPC.h"
#include <Arduino.h>

// m7 includes
#include <Arduino_PortentaBreakout.h>
#include <SDCardReaderAndWriter.h>

#include <SDRAM.h>
#include <NeuralNetwork.h>
#include "FirmwareLoader.h"
NeuralNetwork *nn = nullptr;

// m7 defines
SDCardReaderAndWriter sdcard;


int led = LEDB;
tfLiteModel_t model;

void setup()
{
  Serial.begin(115200);

  while (!Serial);
  Serial.println("M7 booted");

  pinMode(led, OUTPUT);
  // mockdata = new float*[128];
  // for(int i = 0; i<128; i++){
  //   mockdata[i] = new float[547];
  // }
  // for(int x; x < 128; x++){
  //   for(int y; y < 547; y++){
  //       mockdata[x][y] = 1;
  //     }
  //  }
  model = loadTfliteModel();

  char buffer[64];
  auto len = sprintf(buffer, "The address of ptr is %x on the M7", (unsigned int)model.data);
  Serial.write(buffer, len);
  Serial.println();
  for (int i = model.size - 50; i < model.size; i++)
  {
    Serial.print(model.data[i], HEX);
    Serial.print(",");
  }
  Serial.println();

  nn = new NeuralNetwork(model.data);
}

void loop()
{
  delay(1000);
  Serial.println("Hello from m7");

  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);

}
