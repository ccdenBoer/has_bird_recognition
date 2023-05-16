#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <DFRobot_B_LUX_V30B.h>
#include <DFRobot_AHT20.h>
#include <Arduino.h>
#include <Adafruit_GPS.h>

#define LIGHT_SENSOR_INPUT_PIN      0
#define LIGHT_SENSOR_SCL_PIN        0
#define LIGHT_SENSOR_SDA_PIN        0

#define RAIN_SENSOR_ANALOG_INPUT    0
#define RAIN_SENSOR_DIGITAL_INPUT   0

#define GPS_TX_PIN                  0
#define GPS_RX_PIN                  0

#define GPS_SERIAL                  Serial1 //RX1 and TX1
#define GPS_ECHO                    false

#define MIN_TEMP                    -45
#define MAX_TEMP                    85

#define MIN_LIGHT_VALUE             0
#define MAX_LIGHT_VALUE             200

#define MIN_HUM                     0
#define MAX_HUM                     100

#define MIN_COVERAGE                0
#define MAX_COVERAGE                256

class SensorData {
    public:
        //Initializing
        void    InitSensors();        //Method to init all the sensors

        //Light sensor
        float   GetLightIntensity();  //Method which returns the light intensity of the SEN0390 light sensor

        //Temp and humidity sensor
        float   GetTemperature();     //Method to get the temperature from the SEN0527 T/H sensor
        float   GetHumidity();        //Method to get the humidity from the SEN0527 T/H sensor

        //Rain sensor
        bool    GetRainThreshold();   //Method which returns if the sensor is wet or not
        int     GetRainSurface();     //Method which returns the coverage of the rain on the sensor

        //GPS sensor
        float*  GetGPSLocation();     //Method which returns the current location of the sensor

        //Validate measurements
        bool    ValidateSensorData(float lightIntensity, float temp, float hum, int rainSurface);

        SensorData() {}
        ~SensorData(){}

    private:
        DFRobot_B_LUX_V30B  lightSensor = DFRobot_B_LUX_V30B(LIGHT_SENSOR_INPUT_PIN, LIGHT_SENSOR_SCL_PIN, LIGHT_SENSOR_SDA_PIN);
        DFRobot_AHT20       tempAndHumiditySensor;

        Adafruit_GPS        GPS     = Adafruit_GPS(&GPS_SERIAL);
        uint32_t            timer   = millis();
};

#endif