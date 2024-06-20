#include "SensorData.h"

void SensorData::InitSensors() {
  //Serial.println("Light init");
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
    while (!rainfallSensor.begin()) {
      Serial.println("Rainfall initialization failed.");
      delay(1000);
  }

  Serial.println("Start GPS...");
  gpsParser.setup();
  Serial.println("GPS Started...");

}

float SensorData::GetLightIntensity() {
  return 0;
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

float SensorData::GetRainLastHour() {
  float val = 0;
    try {
      val = rainfallSensor.getRainfall(1);
      printf("Rain Last Hour: %f\n", val);
    } catch (const std::exception &e) {
      printf("Exception occurred: %s\n", e.what());
    }
  return val;
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

bool SensorData::getLocation(float location[2]){
  unsigned long startTime = millis();
  bool done = false;
  while(!done){
    done = gpsParser.getCoordinates(location);
    if((millis() - startTime) > 10000 && !done){
      printf("Could not get location in 10 seconds!\n");
      return false;
    }
  }
  return true;
}

uint8_t SensorData::ValidateSensorData(float lightIntensity,
									   float temp,
									   float hum,
									   float rainLastHour,
									   int percentage) {
  //Check light intensity, cant be under 0 and above 200klx
  bool correctLightIntensity = (lightIntensity >= MIN_LIGHT_VALUE && lightIntensity <= MAX_LIGHT_VALUE);

  //Check temp, cant be under -40 or above 85 degrees celsius
  bool correctTemperature = (temp >= MIN_TEMP && temp <= MAX_TEMP);

  //Check humidity, cant be under 0% and 100%
  bool correctHumidity = (hum >= MIN_HUM && hum <= MAX_HUM);

  //Check rain coverage, cant be under 0 and above 256
  bool correctRain = (rainLastHour >= MIN_COVERAGE);

  //Check percentage, cant be below 0% or above 100%
  bool correctPercentage = (percentage >= MIN_BATTERY_PERCENTAGE && percentage <= MAX_BATTERY_PERCENTAGE);

  return (correctLightIntensity << 4)
	  | (correctTemperature << 3)
	  | (correctHumidity << 2)
	  | (correctRain << 1)
	  | (correctPercentage << 0);
}