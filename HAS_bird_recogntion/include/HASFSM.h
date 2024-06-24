#ifndef HAS_FSM_H
#define HAS_FSM_H

#include <FSM.h>
#include <Mic.h>
#include <SensorData.h>
#include <LoRaConnection.h>
#include <SDCardReaderAndWriter.h>
#include <NeuralNetwork.h>
#include "FirmwareLoader.h"
#include <MFCC.h>

#define SEND_INTERVAL 15 //Minutes
//#define SAVE_AUDIO //save latest recording as audio.wav to the sd card
#define MAX_NUMBER_OF_MEASUREMENTS_TO_SEND 8//lora can only handle packets to 256 bytes, 256/28 = 9.5, including some other data only 8 measurements can be send at a time

enum FSM_States {
  STATE_INITIALIZING,
  STATE_INITIALIZING_FAILED,
  STATE_LISTENING,
  STATE_GATHERING_DATA,
  STATE_SENDING,
  STATE_NOT_CONNECTED,
  STATE_TOTAL
};

enum FSM_Events {
  INITIALIZING_FAILED,
  SENSORS_INITIALIZED,
  BIRD_FOUND,
  SEND_INTERVAL_NOT_REACHED,
  SEND_INTERVAL_REACHED,
  JOIN_FAILED,
  SEND_SUCCEEDED,
  JOIN_SUCCESFULL,
  CONNECT_FAILED,
  CONNECTION_TIMEOUT,
  EVENTS_STATE_EXECUTED,
  EVENTS_TOTAL
};

typedef struct {
  uint8_t birdList;
  uint8_t birdType;
  uint8_t birdAccuracy;
  uint8_t temperature;
  uint8_t humidity;
  uint8_t rainLastHour;
  uint8_t batteryPercentage;
  uint8_t validation;
  uint32_t latitude;
  uint32_t longitude;
  uint32_t date;
  uint32_t time;
  uint16_t lightIntensity;
} message_t;

typedef struct payload{
  uint16_t messageCount = 0;
  uint16_t messageSize = sizeof(message_t);
  message_t* messages = nullptr;
} payload_t;

class HASFSM {
public:
  explicit HASFSM();
  void InitHASFSM();
  FSM birdSensorFSM = FSM(STATE_TOTAL, EVENTS_TOTAL);

private:
  void Initializing();
  void Listening();
  void GatheringData();
  void Sending();
  void NotConnected();
  static void InitializingFailed();

  Mic						mic;
  SensorData              	sensorData;
  LoRaConnection          	connection;
  SDCardReaderAndWriter   	sd;
  NeuralNetwork 			*neuralNetwork;
  tfLiteModel_t           	model;
  MFCC 						mfcc;


  int 						lastRecognizedBird;
  float 					recognitionAccuracy;
  uint8_t 					correctMeasurements;
  int 						batteryPercentage;
  uint32_t 					lastTimeSent;
  uint32_t 					lastTimeGSPGathered;
};

#endif