#include "SensorData.h"

void SensorData::InitSensors() {
    //Init light sensor
    lightSensor.begin();
}

float SensorData::GetLightIntensity() {
    return lightSensor.lightStrengthLux();
}