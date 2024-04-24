#include "SensorData.h"
#include <Adafruit_GPS.h>

//SensorData sensor;
#define GPSSerial Serial3

Adafruit_GPS GPS(&GPSSerial);
//float* location = nullptr;

#define GPSECHO false

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
  //Init GPS sensor
  GPSSerial.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);

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

bool SensorData::GetGPSLocation(float buffer[2]) {
	printf("Getting GPS Location\n");
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (c & GPSECHO) printf("%c\n",c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
	// a tricky thing here is if we print the NMEA sentence, or data
	// we end up not listening and catching other sentences!
	// so be very wary if using OUTPUT_ALLDATA and trying to print out data
 	//this also sets the newNMEAreceived() flag to false
	printf("gps last nmea: %s\n", GPS.lastNMEA());
	if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
	  return false; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
	timer = millis(); // reset the timer
	printf("Time: %02d:%02d:%02d.%03d\n", GPS.hour, GPS.minute, GPS.seconds, GPS.milliseconds);

	printf("Date: %02d/%02d/20%02d\n", GPS.day, GPS.month, GPS.year);
	printf("Fix: %d quality: %d\n", (int)GPS.fix, (int)GPS.fixquality);

	if (GPS.fix) {
	  //Replacement using printf
	  printf("Location: %04f%c, %04f%c\n", GPS.latitude, GPS.lat, GPS.longitude, GPS.lon);
	  printf("Speed (knots): %f\n", GPS.speed);
	  printf("Angle: %f\n", GPS.angle);
	  printf("Altitude: %f\n", GPS.altitude);
	  printf("Satellites: %d\n", (int)GPS.satellites);
	  printf("Antenna status: %d\n", (int)GPS.antenna);

	  //Lat
	  //Get first two numbers of float
	  auto ddValue = std::floor(GPS.latitude / 100.0f);
	  auto mmValue = std::fmod(GPS.latitude, 100.0f);
	  auto newLat = ddValue + (mmValue / 60.0f);

	  //Lon
	  //Get first two numbers of float
	  auto ddValue2 = std::floor(GPS.longitude / 100.0f);
	  auto mmValue2 = std::fmod(GPS.longitude, 100.0f);
	  auto newLon = ddValue2 + (mmValue2 / 60.0f);

	  buffer[0] = newLat;
	  buffer[1] = newLon;

	  return true;
	}
  }

  return false;
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