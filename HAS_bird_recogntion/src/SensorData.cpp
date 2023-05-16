#include "SensorData.h"

void SensorData::InitSensors() {
    //Init light sensor
    lightSensor.begin();

    //Init AHT20
    tempAndHumiditySensor.begin();

    uint8_t status;
    while((status = tempAndHumiditySensor.begin()) != 0){
        Serial.print("AHT20 sensor initialization failed. error status : ");
        Serial.println(status);
        delay(1000);
    }

    //Init of rain sensor
    pinMode(RAIN_SENSOR_ANALOG_INPUT,   INPUT);
    pinMode(RAIN_SENSOR_DIGITAL_INPUT,  INPUT);

    //Init GPS sensor
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
    GPS.sendCommand(PGCMD_ANTENNA);

    delay(1000);

    GPS_SERIAL.println(PMTK_Q_RELEASE);
}

float SensorData::GetLightIntensity() {
    return lightSensor.lightStrengthLux();
}

float SensorData::GetTemperature() {
    return tempAndHumiditySensor.getTemperature_C();
}

float SensorData::GetHumidity() {
    return tempAndHumiditySensor.getHumidity_RH();
}

bool SensorData::GetRainThreshold() {
    return digitalRead(RAIN_SENSOR_DIGITAL_INPUT);
}

int SensorData::GetRainSurface() {
    return map(analogRead(RAIN_SENSOR_ANALOG_INPUT), 1950, 0, 0, 256);
}

float* SensorData::GetGPSLocation() {
  char c = GPS.read();
  float location[3];

  if (GPS_ECHO) {
    if (c) {
        Serial.print(c);
    }
  }

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) { // this also sets the newNMEAreceived() flag to false
      return location; // we can fail to parse a sentence in which case we should just wait for another
    }
  }

    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);

    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);

    if (GPS.fix) {
        Serial.print("Location: ");
        Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
        location[0] = GPS.latitude;
        Serial.print(", ");
        Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
        location[1] = GPS.longitude;
        Serial.print("Altitude: "); Serial.println(GPS.altitude);
        location[2] = GPS.altitude;
    }

    return location;
}

uint8_t SensorData::ValidateSensorData(float lightIntensity, float temp, float hum, int rainSurface, bool raining, int percentage) {
    //Check light intensity, cant be under 0 and above 200klx
    bool correctLightIntensity = (lightIntensity >= MIN_LIGHT_VALUE && lightIntensity <= MAX_LIGHT_VALUE);

    //Check temp, cant be under -40 or above 85 degrees celsius
    bool correctTemperature = (temp >= MIN_TEMP && temp <= MAX_TEMP);

    //Check humidity, cant be under 0% and 100%
    bool correctHumidity = (hum >= MIN_HUM && hum <= MAX_HUM);

    //Check rain coverage, cant be under 0 and above 256
    bool corretRainCoverage = (rainSurface >= MIN_COVERAGE && rainSurface < MAX_COVERAGE);

    //Check percentage, cant be below 0% or above 100%
    bool correctPercentage = (percentage >= 0 && percentage <= 100);

    return (correctLightIntensity << 5)
        | (correctTemperature << 4)
        | (correctHumidity << 3)
        | (raining << 2)
        | (corretRainCoverage << 1)
        | (correctPercentage << 0);
}