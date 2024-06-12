#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <DFRobot_B_LUX_V30B.h>
#include <DFRobot_AHT20.h>
#include <Arduino.h>
#include "GPSParser.h"
#include "DFRobot_RainfallSensor.h"
#include "Wire.h"

#define LIGHT_SENSOR_INPUT_PIN      26

#define I2C0_SCL_PIN         145
#define I2C0_SDA_PIN         146

#define I2C1_SCL_PIN         147
#define I2C1_SDA_PIN         148

#define I2C2_SCL_PIN        149
#define I2C2_SDA_PIN        150

#define RAIN_SENSOR_SCL_PIN I2C0_SCL_PIN
#define RAIN_SENSOR_SDA_PIN I2C0_SDA_PIN

#define BATTERY_PERCENTAGE_INPUT    PIN_A2 //A2

#define MIN_TEMP                    -45
#define MAX_TEMP                    85

#define MIN_LIGHT_VALUE             0
#define MAX_LIGHT_VALUE             200

#define MIN_HUM                     0
#define MAX_HUM                     100

#define MIN_COVERAGE                0
#define MAX_COVERAGE                256

#define MIN_BATTERY_PERCENTAGE      0
#define MAX_BATTERY_PERCENTAGE      100

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
        float    GetRainLastHour();   //Method which returns the mm of rain in the last hour

        //GPS sensor
        void getDateTime(char* dateTime);
        bool getLocation(float location[2]);

        //Battery Percentage
        int GetBatteryPercentage(); //Method which returns the current percentage of the battery capacity

        //Validate measurements
        uint8_t ValidateSensorData(float lightIntensity, float temp, float hum, float rainLastHour, int percentage);

        SensorData() {}
        ~SensorData(){}

    private:
        MbedI2C                     rainWire = MbedI2C(RAIN_SENSOR_SDA_PIN, RAIN_SENSOR_SCL_PIN);
        DFRobot_AHT20               tempAndHumiditySensor;
        GPSParser                   gpsParser;
        DFRobot_RainfallSensor_I2C  rainfallSensor = DFRobot_RainfallSensor_I2C(&rainWire);
        uint32_t                    timer = millis();
};

#endif