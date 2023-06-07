// global includes
#include "RPC.h"
#include <Arduino.h>

// m7 includes
#include <Arduino_PortentaBreakout.h>
#include <SDCardReaderAndWriter.h>

#include <SDRAM.h>
#include <NeuralNetwork.h>
#include "FirmwareLoader.h"
//euralNetwork *nn = nullptr;
float mockdata [128][547][1];

// m7 defines
SDCardReaderAndWriter sdcard;


int led = LEDB;
//tfLiteModel_t model;

void setup()
{
  Serial.begin(115200);

  while (!Serial);
  Serial.println("M7 booted");

  pinMode(led, OUTPUT);

  for(int x; x < 128; x++){
    for(int y; y < 547; y++){
        mockdata[x][y][0] = 1;
      }
   }
  //model = loadTfliteModel();

  char buffer[64];
  //auto len = sprintf(buffer, "The address of ptr is %x on the M7", (unsigned int)model.data);
  //Serial.write(buffer, len);
  //Serial.println();
  //for (int i = model.size - 50; i < model.size; i++)
  //{
   // Serial.print(model.data[i], HEX);
    //Serial.print(",");
  //}
  //Serial.println();
  //int input_shape[3] = {128,547,1};
  //int tensor_arena_size = 1024*1024*5;
  //nn = new NeuralNetwork(model.data, tensor_arena_size, 11, input_shape);
}

void loop()
{
  delay(1000);
  Serial.println("Hello from m7");
  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  //nn->InputData(mockdata);
  //NeuralNetwork::result_t prediction = nn->Predict();
  //Serial.print("Got predicted class: ");
  //Serial.print(prediction.class_name);
  //Serial.print(", with a confidence of : ");
  //Serial.println(prediction.confidence);


}
