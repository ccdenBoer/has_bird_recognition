#include "SensorData.h"

//float* location = nullptr;

void SensorData::InitSensors() {
  Serial.println("Light init");
  //Init light sensor
  //lightSensor.begin();

  Serial.println("Battery init");
  //pinMode(BATTERY_PERCENTAGE_INPUT, INPUT);

  Serial.println("AHT init");
  //Init AHT20
  uint8_t status;
  while ((status = tempAndHumiditySensor.begin()) != 0) {
	Serial.print("AHT20 sensor initialization failed. error status : ");
	Serial.println(status);
	delay(1000);
  }
  Serial.println("AHT inited");

  Serial.println("Rain init");
  //Init of rain sensor
  //pinMode(RAIN_SENSOR_ANALOG_INPUT, INPUT);
  //pinMode(RAIN_SENSOR_DIGITAL_INPUT, INPUT);

  Serial.println("Start GPS...");
  gpsParser.setup();
  Serial.println("GPS Started...");

}

float SensorData::GetLightIntensity() {
  return lightSensor.lightStrengthLux();
}

float SensorData::GetTemperature() {
  while (!tempAndHumiditySensor.startMeasurementReady(true))
	delay(10);
  return tempAndHumiditySensor.getTemperature_C();
}

float SensorData::GetHumidity() {
  while (!tempAndHumiditySensor.startMeasurementReady(true))
	delay(10);
  return tempAndHumiditySensor.getHumidity_RH();
}

bool SensorData::GetRainThreshold() {
  return digitalRead(RAIN_SENSOR_DIGITAL_INPUT);
}

int SensorData::GetRainSurface() {
  return map(analogRead(RAIN_SENSOR_ANALOG_INPUT), 1950, 0, 0, 256);
}

int SensorData::GetBatteryPercentage(){
	//max value 1023
	auto rawRead = analogRead(BATTERY_PERCENTAGE_INPUT);
	printf("Battery level raw: %d \n", rawRead);
	int percent = map(rawRead,0,1024,0,100);
	printf("Battery level: %d \n", percent);
	return percent;
}

void SensorData::getDateTime(char* dateTime){
  while(!gpsParser.getDateTime(dateTime)){
    //delay(1000);
  }
}

void SensorData::getLocation(float location[2]){
  while(!gpsParser.getCoordinates(location)){
    //delay(1000);
  }
}

uint8_t SensorData::ValidateSensorData(float lightIntensity,
									   float temp,
									   float hum,
									   int rainSurface,
									   bool raining,
									   int percentage) {
  //Check light intensity, cant be under 0 and above 200klx
  bool correctLightIntensity = (lightIntensity >= MIN_LIGHT_VALUE && lightIntensity <= MAX_LIGHT_VALUE);

  //Check temp, cant be under -40 or above 85 degrees celsius
  bool correctTemperature = (temp >= MIN_TEMP && temp <= MAX_TEMP);

  //Check humidity, cant be under 0% and 100%
  bool correctHumidity = (hum >= MIN_HUM && hum <= MAX_HUM);

  //Check rain coverage, cant be under 0 and above 256
  bool corretRainCoverage = (rainSurface >= MIN_COVERAGE && rainSurface < MAX_COVERAGE);

  //Check percentage, cant be below 0% or above 100%
  bool correctPercentage = (percentage >= MIN_BATTERY_PERCENTAGE && percentage <= MAX_BATTERY_PERCENTAGE);

  return (correctLightIntensity << 5)
	  | (correctTemperature << 4)
	  | (correctHumidity << 3)
	  | (raining << 2)
	  | (corretRainCoverage << 1)
	  | (correctPercentage << 0);
}