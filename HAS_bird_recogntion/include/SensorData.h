#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <DFRobot_B_LUX_V30B.h>
#include <Adafruit_AHTX0.h>

#define LIGHT_SENSOR_INPUT_PIN  1
#define LIGHT_SENSOR_SCL_PIN    2
#define LIGHT_SENSOR_SDA_PIN    3

class SensorData {
    public:
        void InitSensors();         //Method to init all the sensors

        float GetLightIntensity();  //Method which returns the light intensity of the SEN0390 light sensor

        float GetTemperature();     //Method to get the temperature from the SEN0527 T/H sensor
        float GetHumidity();        //Method to get the humidity from the SEN0527 T/H sensor

        SensorData() {}
        ~SensorData(){}

    private:
        DFRobot_B_LUX_V30B lightSensor = DFRobot_B_LUX_V30B(LIGHT_SENSOR_INPUT_PIN, LIGHT_SENSOR_SCL_PIN, LIGHT_SENSOR_SDA_PIN);
        Adafruit_AHTX0 tempAndHumiditySensor;
};

#endif