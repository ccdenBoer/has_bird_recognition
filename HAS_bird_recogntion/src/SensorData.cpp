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