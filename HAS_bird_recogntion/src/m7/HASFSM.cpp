#include "HASFSM.h"
#include <SensorData.h>


int tensor_arena_size = 1024 * 1024 * 5;
float location[2];


HASFSM::HASFSM(): model(loadTfliteModel()) {
  printf("Initializing mic\n");
  mic = Mic();
  printf("Initializing mfcc\n");
  mfcc = MFCC();
  printf("Initializing neural network\n");
  neuralNetwork = new NeuralNetwork(model.data, tensor_arena_size, 11);
  printf("Initializing sensors\n");
  sensorData = SensorData();
  printf("Initializing lora connection\n");
  connection = LoRaConnection();
  sd = SDCardReaderAndWriter();
}

void HASFSM::Initializing() {
  mfcc.begin(SAMPLE_RATE, SAMPLE_TIME);
  sensorData.InitSensors();
  printf("Initializing\n");
  connection.InitConnection();
  sd.InitSDCardReaderAndWriter();


  //Gather initial GPS location
  printf("Gathering initial GPS location\n");
//    while(!sensorData.GetGPSLocation(location));
//    lastTimeGSPGathered = millis();


  //Raise new event after checking mic
  if (mic.begin()) {
	birdSensorFSM.raiseEvent(SENSORS_INITIALIZED);
  } else {
	birdSensorFSM.raiseEvent(INITIALIZING_FAILED);
  }
}

void HASFSM::InitializingFailed() {
  printf("Initializing failed\n");
}

void HASFSM::Listening() {
  //printf("Listening\n");

  while (!mic.audioBufferReady()) {
	yield();
	return;
  }

  auto audioBuffer = mic.audioBufferGet();

  //Start timer
  auto start = millis();
  auto mfcc_buffer = mfcc.process_audio(audioBuffer.data);
  auto finish = millis();
  printf("MFCC took %ld ms\n", finish - start);
  mic.audioBufferClear();

  neuralNetwork->InputData(mfcc_buffer);
  start = millis();
  NeuralNetwork::result_t prediction = neuralNetwork->Predict();
  finish = millis();
  printf("Prediction took %ld ms\n", finish - start);
  //also print in seconds
  printf("Prediction took %f s\n", (finish - start) / 1000.0);


  //Check for bird and update AI data
  bool birdFound = strcmp(prediction.class_name, "Geen Vogel") != 0;
  lastRecognizedBird = prediction.predicted_class;
  recognitionAccuracy = prediction.confidence;

  //Raise new event if a bird was found
  if (birdFound) {
	birdSensorFSM.raiseEvent(BIRD_FOUND);
  }
}

void HASFSM::GatheringData() {
  //Gather data
  auto lightIntensity = sensorData.GetLightIntensity();

  auto temperature = sensorData.GetTemperature();
  auto humidity = sensorData.GetHumidity();

  auto raining = sensorData.GetRainThreshold();
//  auto rainCoverage = sensorData.GetRainSurface();
  auto rainCoverage = 0;

  //Check if day has passed to gather GPS data
//  if ((millis() - lastTimeGSPGathered) > 86400000) {
//	int gpsAttempts = 0;
//	while (!sensorData.GetGPSLocation(location)) {
//	  gpsAttempts++;
//
//	  if (gpsAttempts > 50) {
//		break;
//	  }
//	}
//  }

  //TODO: Get battery percentage
  batteryPercentage = 20; //TEMP VALUE

  //Validate
  correctMeasurements = sensorData.ValidateSensorData(lightIntensity, temperature, humidity, rainCoverage, raining, batteryPercentage);

  static unsigned int fileIndex = 0;
  char fileName[22];
  sprintf(fileName, "/sd-card/test%d.json\n", fileIndex);
  fileIndex++;

  //Sent measurements to SDCard
  sd.WriteToSDCard(fileName,
				   lastRecognizedBird,
				   recognitionAccuracy,
				   lightIntensity,
				   temperature,
				   humidity,
				   rainCoverage,
				   raining,
				   batteryPercentage,
				   location[0],
				   location[1],
				   correctMeasurements);
  printf("Data written to SD card\n");

//  check written data

  sd.ReadFileData(fileName);

  // Check send interval
  if ((millis() - lastTimeSent) >= (SEND_INTERVAL * 60 * 1000)) {
	birdSensorFSM.raiseEvent(SEND_INTERVAL_REACHED);
	lastTimeSent = millis();
  } else {
	printf("Send interval not reached\n");
	birdSensorFSM.raiseEvent(SEND_INTERVAL_NOT_REACHED);
  }
}

void HASFSM::Sending() {
  //Check if TTN can be joined
  if (!connection.SetOTAAJoin(JOIN, 10)) {
	birdSensorFSM.raiseEvent(JOIN_FAILED);
  } else {
	connection.SetOTAAJoin(JOIN, 10);

	//Read data from SD-Card
	DIR *dp = nullptr;
	struct dirent *entry = nullptr;

	//Loop through every file in the directory
	dp = opendir("sd-card/.");
	if (dp != nullptr) {
	  while ((entry = readdir(dp))) {
		//Open and read file content
		char filePath[264];
		sprintf(filePath, "sd-card/%s", entry->d_name);

		char *bufferString = sd.ReadFileData(filePath);

		//Convert data
		DynamicJsonDocument doc(1024);
		deserializeJson(doc, bufferString);

		cayenne.reset();
		cayenne.addAnalogInput(1, doc["birdType"]);
		cayenne.addAnalogInput(2, doc["birdAccuracy"]);
		cayenne.addAnalogInput(3, doc["lightIntensity"]);
		cayenne.addAnalogInput(4, doc["temperature"]);
		cayenne.addAnalogInput(5, doc["humidity"]);
		cayenne.addAnalogInput(6, doc["rainCoverage"]);
		cayenne.addAnalogInput(7, doc["raining"]);
		cayenne.addAnalogInput(8, doc["batteryPercentage"]);
		cayenne.addAnalogInput(9, doc["lattitude"]);
		cayenne.addAnalogInput(10, doc["longtitude"]);
		cayenne.addAnalogInput(11, doc["validation"]);

		//Send data
		connection.SendPacketCayenne(cayenne.getBuffer(), cayenne.getSize(), 10);

		//Remove data
		remove(filePath);
	  }
	}

	closedir(dp);
  }
}

void HASFSM::NotConnected() {
  int currentAttempst = 0;
  while (currentAttempst < MAX_RECONNECT_ATTEMPTS) {
	if (!connection.SetOTAAJoin(JOIN, 10)) {
	  Serial.println("Trying to reconnect...");
	  currentAttempst++;
	  delay(1000);
	} else {
	  birdSensorFSM.raiseEvent(JOIN_SUCCESFULL);
	}
  }

  birdSensorFSM.raiseEvent(CONNECTION_TIMEOUT);
}

void HASFSM::InitHASFSM() {
  //Initializing states
  birdSensorFSM.addState(FSM_States::STATE_INITIALIZING,[this]{Initializing();});
  birdSensorFSM.addState(FSM_States::STATE_INITIALIZING_FAILED, [this]{InitializingFailed();} );
  birdSensorFSM.addState(FSM_States::STATE_LISTENING,   [this]{Listening();});
  birdSensorFSM.addState(FSM_States::STATE_GATHERING_DATA, [this]{GatheringData();} );
  birdSensorFSM.addState(FSM_States::STATE_SENDING, [this]{Sending();});
  birdSensorFSM.addState(FSM_States::STATE_NOT_CONNECTED,  [this]{NotConnected();});

  //Initializing transistions
  birdSensorFSM
	  .addTransition(FSM_States::STATE_INITIALIZING, FSM_Events::SENSORS_INITIALIZED, FSM_States::STATE_LISTENING);
  birdSensorFSM.addTransition(FSM_States::STATE_INITIALIZING,
							  FSM_Events::INITIALIZING_FAILED,
							  FSM_States::STATE_INITIALIZING_FAILED);

  birdSensorFSM.addTransition(FSM_States::STATE_LISTENING, FSM_Events::BIRD_FOUND, FSM_States::STATE_GATHERING_DATA);
  birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA, FSM_Events::SEND_INTERVAL_REACHED, FSM_States::STATE_SENDING);
  birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA, FSM_Events::SEND_INTERVAL_NOT_REACHED, FSM_States::STATE_LISTENING);
  birdSensorFSM.addTransition(FSM_States::STATE_SENDING, FSM_Events::SEND_SUCCEEDED, FSM_States::STATE_LISTENING);
  birdSensorFSM.addTransition(FSM_States::STATE_SENDING, FSM_Events::JOIN_FAILED, FSM_States::STATE_NOT_CONNECTED);
  birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED, FSM_Events::JOIN_SUCCESFULL, FSM_States::STATE_SENDING);
  birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED, FSM_Events::CONNECT_FAILED, FSM_States::STATE_NOT_CONNECTED);
  birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED, FSM_Events::CONNECTION_TIMEOUT, FSM_States::STATE_LISTENING);

  lastTimeSent = millis();
}