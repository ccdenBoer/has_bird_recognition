#include "SensorData.h"
#include <Adafruit_GPS.h>

//SensorData sensor;
#define GPSSerial Serial1

Adafruit_GPS GPS(&GPSSerial);
//float* location = nullptr;

#define GPSECHO false

void SensorData::InitSensors() {
  Serial.println("Light init");
  //Init light sensor
  lightSensor.begin();

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
  pinMode(RAIN_SENSOR_ANALOG_INPUT, INPUT);
  pinMode(RAIN_SENSOR_DIGITAL_INPUT, INPUT);

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
	int rawRead = analogRead(BATTERY_PERCENTAGE_INPUT);
	int percent = map(rawRead,0,1023,0,100);
	printf("Battery level: %d %d \n", rawRead, percent);
	return percent;

}

bool SensorData::GetGPSLocation(float buffer[2]) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
	if (c)
	  Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
	// a tricky thing here is if we print the NMEA sentence, or data
	// we end up not listening and catching other sentences!
	// so be very wary if using OUTPUT_ALLDATA and trying to print out data
	Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
	if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
	  return false; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
	timer = millis(); // reset the timer
	Serial.print("\nTime: ");
	if (GPS.hour < 10) {
	  Serial.print('0');
	}
	Serial.print(GPS.hour, DEC);
	Serial.print(':');
	if (GPS.minute < 10) {
	  Serial.print('0');
	}
	Serial.print(GPS.minute, DEC);
	Serial.print(':');
	if (GPS.seconds < 10) {
	  Serial.print('0');
	}
	Serial.print(GPS.seconds, DEC);
	Serial.print('.');
	if (GPS.milliseconds < 10) {
	  Serial.print("00");
	} else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
	  Serial.print("0");
	}
	Serial.println(GPS.milliseconds);
	Serial.print("Date: ");
	Serial.print(GPS.day, DEC);
	Serial.print('/');
	Serial.print(GPS.month, DEC);
	Serial.print("/20");
	Serial.println(GPS.year, DEC);
	Serial.print("Fix: ");
	Serial.print((int)GPS.fix);
	Serial.print(" quality: ");
	Serial.println((int)GPS.fixquality);
	if (GPS.fix) {
	  Serial.print("Location: ");
	  Serial.print(GPS.latitude, 4);
	  Serial.print(GPS.lat);
	  Serial.print(", ");
	  Serial.print(GPS.longitude, 4);
	  Serial.println(GPS.lon);
	  Serial.print("Speed (knots): ");
	  Serial.println(GPS.speed);
	  Serial.print("Angle: ");
	  Serial.println(GPS.angle);
	  Serial.print("Altitude: ");
	  Serial.println(GPS.altitude);
	  Serial.print("Satellites: ");
	  Serial.println((int)GPS.satellites);
	  Serial.print("Antenna status: ");
	  Serial.println((int)GPS.antenna);

	  //Lat
	  //Get first two numbers of float
	  int ddValue = std::floor(GPS.latitude / 100);
	  double mmValue = std::fmod(GPS.latitude, 100);
	  double newLat = ddValue + (mmValue / 60.0);

	  //Lon
	  //Get first two numbers of float
	  int ddValue2 = std::floor(GPS.longitude / 100);
	  double mmValue2 = std::fmod(GPS.longitude, 100);
	  double newLon = ddValue2 + (mmValue2 / 60.0);

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