#include "HASFSM.h"
#include <SensorData.h>
#include <SdramAllocator.h>
#include <TTNFormatter.h>

int tensor_arena_size = 1024 * 1024 * 5;
float location[2];

HASFSM::HASFSM() : model(loadTfliteModel()) {
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
  //connection.InitialSetup();

  printf("Initializing MicroSD reder/writer");
  sd = SDCardReaderAndWriter();
  lastTimeSent = 0;
  sensorData.GetGPSLocation(location);
}

void HASFSM::Initializing() {
  mfcc.begin(SAMPLE_RATE, SAMPLE_TIME);
  sensorData.InitSensors();
  printf("Initializing\n");
  connection.InitConnection();

  sd.InitSDCardReaderAndWriter();

  // Raise new event after checking mic
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
  // printf("Listening\n");

  while (!mic.audioBufferReady()) {
	  yield();
	return;
  }

  auto audioBuffer = mic.audioBufferGet();

  // Start timer
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
  // also print in seconds
  printf("Prediction took %f s\n", (finish - start) / 1000.0);

  // Check for bird and update AI data
  bool birdFound = strcmp(prediction.class_name, "Geen Vogel") != 0;

  lastRecognizedBird = prediction.predicted_class;
  recognitionAccuracy = prediction.confidence;

  //TODO
  //Fix neural network file

  //bool birdFound = true;
  //lastRecognizedBird = 0;
  //recognitionAccuracy = 0;

  // Raise new event if a bird was found
  if (birdFound) {
	birdSensorFSM.raiseEvent(BIRD_FOUND);
  }
}

void HASFSM::GatheringData() {
  // Gather data
  auto lightIntensity = 0;

  auto temperature = sensorData.GetTemperature();
  auto humidity = sensorData.GetHumidity();

  auto raining = 0;

  // TODO:
  //  Fix rain coverage analog read
  //  auto rainCoverage = sensorData.GetRainSurface();
  auto rainCoverage = 0;

  // TODO:
  //  Fix battery percentage analog read
  //  batteryPercentage = sensorData.GetBatteryPercentage();


  // Check if day has passed to gather GPS data
  if ((millis() - lastTimeGSPGathered) < 86400000) {
	int gpsAttempts = 0;
	while (!sensorData.GetGPSLocation(location)) {
	  gpsAttempts++;
	  if (gpsAttempts > 10) {
		break;
	  }
	}
	lastTimeGSPGathered = millis();
  }



  // Validate
  correctMeasurements =
	  sensorData.ValidateSensorData(lightIntensity, temperature, humidity, rainCoverage, raining, batteryPercentage);

  static unsigned int fileIndex = sd.GetAmountOfFiles("/sd-card/measurements/");
  char fileName[60];
  sprintf(fileName, "/sd-card/measurements/MEASUREMENT%d.json\n", fileIndex);
  fileIndex++;

  // Sent measurements to SDCard
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
  if (((millis() - lastTimeSent) >=  (SEND_INTERVAL * 60 * 1000)) || (lastTimeSent == 0)) {
	lastTimeSent = millis();
	printf("Send interval reached\n");
	birdSensorFSM.raiseEvent(SEND_INTERVAL_REACHED);
  } else {
	//write the amount of time in seconds left to send
	auto timeLeft = (SEND_INTERVAL * 60 * 1000) - (millis() - lastTimeSent);
	auto timeLeftSeconds = timeLeft / 1000;
	printf("Time left to send: %ld seconds\n", timeLeftSeconds);
	birdSensorFSM.raiseEvent(SEND_INTERVAL_NOT_REACHED);
  }
}

#define TTN_MAX_BUFFER_SIZE 4096
void HASFSM::Sending() {
  // Check if TTN can be joined
  printf("Sending...\n");
  static bool joined = false;
  if (!joined && !connection.SetOTAAJoin(JOIN, 10)) {
	printf("failed to connect");
	birdSensorFSM.raiseEvent(JOIN_FAILED);
	return;
  }
  joined = true;

  // Read data from SD-Card
  printf("Opening SD-Card\n");
  DIR *dp = opendir("sd-card/.");
  struct dirent *entry;

  // Loop through every file in the directory
  if (dp == nullptr) {
	birdSensorFSM.raiseEvent(JOIN_FAILED); // TODO: Change to Send failed
	return;
  }

  std::vector<message_t, SdramAllocator<message_t>> messages;
  int index = 0;

  while ((entry = readdir(dp)) && index < 15) {
	// if not json file, skip
	if (!strstr(entry->d_name, ".json"))
	  continue;

	// Open and read file content
  printf("Read SD-Card File\n");
	char filePath[512];
	sprintf(filePath, "sd-card/%s", entry->d_name);

	char *bufferString = sd.ReadFileData(filePath);

	messages.push_back(TTNFormatter::convertStringToMessage(bufferString));
	index++;
	// Remove data
	remove(filePath);
  }

  printf("Create Payload\n");
  payload_t ttnPayload;
  ttnPayload.messageCount = messages.size();
  ttnPayload.messages = messages.data();
  closedir(dp);

  static uint8_t payloadBuffer[TTN_MAX_BUFFER_SIZE];
  auto size = TTNFormatter::convertPayloadToTTN(ttnPayload, payloadBuffer, TTN_MAX_BUFFER_SIZE);
  auto status = connection.CheckStatus();
  // TODO:
  //  Make work without checking status 2 times
  printf("status: %d\n", status);
  status = connection.CheckStatus();
  printf("status: %d\n", status);
  connection.SendPacketCayenne(payloadBuffer, size, 10);
  status = connection.CheckStatus();
  printf("status: %d\n", status);
  status = connection.CheckStatus();
  printf("status: %d\n", status);

  if (!status) {
	printf("Failed to send data\n");
	birdSensorFSM.raiseEvent(JOIN_FAILED);
	return;
  }

  birdSensorFSM.raiseEvent(SEND_SUCCEEDED);
}

void HASFSM::NotConnected() {
  int currentAttempst = 0;
  while (currentAttempst < MAX_RECONNECT_ATTEMPTS) {
	if (!connection.SetOTAAJoin(JOIN, 10)) {
	  printf("Failed to connect, retrying in 1 second\n");
	  currentAttempst++;
	  delay(1000);
	} else {
	  birdSensorFSM.raiseEvent(JOIN_SUCCESFULL);
	  return;
	}
  }

  birdSensorFSM.raiseEvent(CONNECTION_TIMEOUT);
}

void HASFSM::InitHASFSM() {
  // Initializing states
  birdSensorFSM.addState(FSM_States::STATE_INITIALIZING, [this] { Initializing(); });
  birdSensorFSM.addState(FSM_States::STATE_INITIALIZING_FAILED, [this] { InitializingFailed(); });
  birdSensorFSM.addState(FSM_States::STATE_LISTENING, [this] { Listening(); });
  birdSensorFSM.addState(FSM_States::STATE_GATHERING_DATA, [this] { GatheringData(); });
  birdSensorFSM.addState(FSM_States::STATE_SENDING, [this] { Sending(); });
  birdSensorFSM.addState(FSM_States::STATE_NOT_CONNECTED, [this] { NotConnected(); });

  // Initializing transistions
  birdSensorFSM.addTransition(FSM_States::STATE_INITIALIZING, FSM_Events::SENSORS_INITIALIZED, FSM_States::STATE_LISTENING);
  birdSensorFSM.addTransition(FSM_States::STATE_INITIALIZING, FSM_Events::INITIALIZING_FAILED, FSM_States::STATE_INITIALIZING_FAILED);
  birdSensorFSM.addTransition(FSM_States::STATE_LISTENING, FSM_Events::BIRD_FOUND, FSM_States::STATE_GATHERING_DATA);
  birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA, FSM_Events::SEND_INTERVAL_REACHED, FSM_States::STATE_SENDING);
  birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA,FSM_Events::SEND_INTERVAL_NOT_REACHED, FSM_States::STATE_LISTENING);
  birdSensorFSM.addTransition(FSM_States::STATE_SENDING, FSM_Events::SEND_SUCCEEDED, FSM_States::STATE_LISTENING);
  birdSensorFSM.addTransition(FSM_States::STATE_SENDING, FSM_Events::JOIN_FAILED, FSM_States::STATE_NOT_CONNECTED);
  birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED, FSM_Events::JOIN_SUCCESFULL, FSM_States::STATE_SENDING);
  birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED, FSM_Events::CONNECT_FAILED, FSM_States::STATE_NOT_CONNECTED);
  birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED, FSM_Events::CONNECTION_TIMEOUT, FSM_States::STATE_LISTENING);
}