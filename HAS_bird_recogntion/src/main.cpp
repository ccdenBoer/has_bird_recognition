#include <Arduino.h>
#include <SensorData.h>

SensorData sensor;

void setup() {
   Serial.begin(9600);

   sensor = SensorData();
   sensor.InitSensors();
}

void loop() {
   float light = sensor.GetLightIntensity();
   Serial.print("Light: ");
   Serial.println(light);

    delay(1000);
} 