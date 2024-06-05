#include "HASFSM.h"
#include <SensorData.h>
#include <SdramAllocator.h>
#include <TTNFormatter.h>

int tensor_arena_size = 1024 * 1024 * 4;
float location[2];
static int totalMeasurements;

HASFSM::HASFSM() : model(loadTfliteModel()) {
  printf("Initializing mic\n");
  mic = Mic();
  printf("Initializing mfcc\n");
  mfcc = MFCC();
  printf("Initializing neural network\n");
  neuralNetwork = new NeuralNetwork(model.data, tensor_arena_size);
  printf("Initializing sensors\n");
  sensorData = SensorData();
  printf("Initializing lora connection\n");
  connection = LoRaConnection();
  connection.InitialSetup();
  
  printf("Initializing MicroSD reader/writer\n");
  sd = SDCardReaderAndWriter();
  lastTimeSent = 0;
}

void HASFSM::Initializing() {
  mfcc.begin(SAMPLE_RATE, SAMPLE_TIME);
  sensorData.InitSensors();

  printf("Initializing\n");
  connection.InitConnection();

  sd.InitSDCardReaderAndWriter();

  //get the total files in the measurment folder so not all get sent every time
  totalMeasurements = sd.GetAmountOfFiles("/sd-card/measurements/");

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

  // Raise new event if a bird was found
  if (birdFound) {
	birdSensorFSM.raiseEvent(BIRD_FOUND);
  }
}

void HASFSM::GatheringData() {
  // Gather data
  float lightIntensity = 0;
  printf("Light Intensity: %f\n", lightIntensity);

  float rainLastHour = sensorData.GetRainLastHour();
  printf("Rain last hour: %f mm\n", rainLastHour);

  float temperature = sensorData.GetTemperature();
  printf("Temperature: %f C\n", temperature);
  float humidity = sensorData.GetHumidity();
  printf("Humidity: %f %\n", humidity);

  // TODO:
  //  Fix battery percentage analog read
  auto batteryPercentage = 0;

  // Get the current UTC time and location
  char dateTime[40];
  sensorData.getDateTime(dateTime);
  printf("Current date/time: %s\n", dateTime);

	//sensorData.getLocation(location);
  location[0] = 0;
  location[1] = 0;
  printf("Longitude: %f, Latitude: %f\n", location[0],location[1]);

  // Validate
  correctMeasurements =
	  sensorData.ValidateSensorData(lightIntensity, temperature, humidity, rainLastHour, batteryPercentage);

  static int fileIndex = sd.GetAmountOfFiles("/sd-card/measurements/");
  char fileName[60];
  sprintf(fileName, "/sd-card/measurements/%s_%d.json\n", dateTime, fileIndex);
  printf("Filename for new Measurement:%s\n", fileName);
  fileIndex++;

  // Sent measurements to SDCard
  sd.WriteToSDCard(fileName,
           dateTime,
				   lastRecognizedBird,
				   recognitionAccuracy,
				   lightIntensity,
				   temperature,
				   humidity,
				   rainLastHour,
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
  DIR *dp = opendir("/sd-card/measurements/");
  struct dirent *entry;

  // Loop through every file in the directory
  if (dp == nullptr) {
	birdSensorFSM.raiseEvent(JOIN_FAILED); // TODO: Change to Send failed
	return;
  }

  std::vector<message_t, SdramAllocator<message_t>> messages;
  int index = totalMeasurements;

  while ((entry = readdir(dp)) && index < (totalMeasurements + 15)) {
	// if not json file, skip
	if (!strstr(entry->d_name, ".json"))
	  continue;

	// Open and read file content
  printf("Read SD-Card File\n");
	char filePath[512];
	sprintf(filePath, "/sd-card/measurements/%s", entry->d_name);
  printf("File to payload: %s\n", filePath);

	char *bufferString = sd.ReadFileData(filePath);

	messages.push_back(TTNFormatter::convertStringToMessage(bufferString));
	index++;
	// Remove data
	//remove(filePath);
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
  totalMeasurements += index;
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