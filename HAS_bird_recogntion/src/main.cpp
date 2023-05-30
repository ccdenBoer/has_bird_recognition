#include <Arduino.h>
#include <SensorData.h>

uint32_t timer = millis();
SensorData sensor;

float buffer[2];

void setup() {
   Serial.begin(115200);

   while (!Serial) delay(10);
   
   sensor = SensorData();
   sensor.InitSensors();

   while (!sensor.GetGPSLocation(buffer)) {
      Serial.println("Fuck");
      delay(50);
   }

   Serial.println(buffer[0]);
   Serial.println(buffer[1]);
}

void loop() {
   // read data from the GPS in the 'main loop'
} 