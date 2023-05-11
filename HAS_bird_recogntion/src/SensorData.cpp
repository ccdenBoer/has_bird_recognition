#include "SensorData.h"

void SensorData::InitSensors() {
    //Init light sensor
    lightSensor.begin();

    //Init temperature and humidity sensor
    if (! tempAndHumiditySensor.begin()) {
        Serial.println("Could not find AHT? Check wiring");
    }

    //Init of rain sensor
    pinMode(RAIN_SENSOR_ANALOG_INPUT,   INPUT);
    pinMode(RAIN_SENSOR_DIGITAL_INPUT,  INPUT);
}

float SensorData::GetLightIntensity() {
    return lightSensor.lightStrengthLux();
}

float SensorData::GetTemperature() {
    Adafruit_Sensor* tempSensor = tempAndHumiditySensor.getTemperatureSensor();
    sensors_event_t* tempEvent = nullptr;

    tempSensor->getEvent(tempEvent);

    return tempEvent->temperature;
}

float SensorData::GetHumidity() {
    Adafruit_Sensor* humiditySensor = tempAndHumiditySensor.getHumiditySensor();
    sensors_event_t* humidityEvent = nullptr;

    humiditySensor->getEvent(humidityEvent);
    
    return humidityEvent->relative_humidity;
}

bool SensorData::GetRainThreshold() {
    return digitalRead(RAIN_SENSOR_DIGITAL_INPUT);
}

int SensorData::GetRainSurface() {
    return analogRead(RAIN_SENSOR_ANALOG_INPUT);
}