#include <HASFSM.h>
#include <SensorData.h>
#include <NeuralNetwork.h>
#include <FirmwareLoader.h>
#include <SDCardReaderAndWriter.h>

FSM birdSensorFSM = FSM(STATE_TOTAL, EVENTS_TOTAL);

SensorData              sensorData;
LoRaConnection          connection;
SDCardReaderAndWriter   sd;
NeuralNetwork           *nn = nullptr;
tfLiteModel_t           model;

uint32_t            lastTimeSent;
uint32_t            lastTimeGSPGathered;
int                 input_shape[3]      = {128,547,1};
int                 tensor_arena_size   = 1024*1024*5;

int               lastRecognizedBird;
float             recognitionAccuracy;
float             lightIntensity;
float             temperature;
float             humidity;
bool              raining;
float             rainCoverage;
float             batteryPercentage;
float             location[2];
uint8_t           correctMeasurements;

void Start() {

}

void Initializing() {
    //Init sensors and lora connection
    sensorData = SensorData();
    connection = LoRaConnection();

    sensorData.InitSensors();
    connection.InitConnection();

    //Init neural network
    nn = new NeuralNetwork(model.data, tensor_arena_size, 11, input_shape);
    model = loadTfliteModel();

    //Gather initial GPS location
    while(!sensorData.GetGPSLocation(location));
    lastTimeGSPGathered = millis();

    //TODO: Check if mic is working
    bool micIsOkey = true;
    
    //Raise new event after checking mic
    if(micIsOkey) {
        birdSensorFSM.raiseEvent(SENSORS_INITIALIZED);
    } else {
        birdSensorFSM.raiseEvent(INITIALIZING_FAILED);
    }
}

void InitializingFailed() {

}

void Listening() {
    int startListeningTime = millis();

    //Gather audio
    while ((millis() - startListeningTime) < (LISTEN_TIME * 1000)) {
        //TODO: Take audio fragment
    }

    //TODO: Convert data and input to NN
    //nn->InputData(/*data*/);
    NeuralNetwork::result_t prediction = nn->Predict();

    //Check for bird and update AI data
    bool birdFound          = (prediction.class_name != "No bird");
    lastRecognizedBird      = prediction.predicted_class;
    recognitionAccuracy     = prediction.confidence;

    //Raise new event if a bird was found
    if (birdFound) {
        birdSensorFSM.raiseEvent(BIRD_FOUND);
    }
}

void GatheringData() {
    //Gather data
    lightIntensity  = sensorData.GetLightIntensity();
    temperature     = sensorData.GetTemperature();
    humidity        = sensorData.GetHumidity();
    raining         = sensorData.GetRainThreshold();
    rainCoverage    = sensorData.GetRainSurface();

    //Check if day has passed to gather GPS data
    if ((millis() - lastTimeGSPGathered) > 86400000) {
        int gpsAttempts = 0;
        while (!sensorData.GetGPSLocation(location)) {
            gpsAttempts++;

            if (gpsAttempts > 50) {
                break;
            }
        }
    }

    //TODO: Get battery percentage
    batteryPercentage = 20; //TEMP VALUE

    //Validate
    correctMeasurements = sensorData.ValidateSensorData(lightIntensity, temperature, humidity, rainCoverage, raining, batteryPercentage);

    //Sent measurements to SDCard
    sd.WriteToSDCard(lastRecognizedBird, recognitionAccuracy, lightIntensity, temperature, humidity, rainCoverage, raining, batteryPercentage, location[0], location[1], correctMeasurements);

    //Check send interval
    if ((millis() - lastTimeSent) >= (SEND_INTERVAL * 60 * 1000)) {
        birdSensorFSM.raiseEvent(SEND_INTERVAL_REACHED);
        lastTimeSent = millis();
    } else {
        birdSensorFSM.raiseEvent(SEND_INTERVAL_NOT_REACHED);
    }
}

void Sending() {
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
                char* filePath = strcat("sd-card/", entry->d_name);
                char* bufferString = sd.ReadFileData(filePath);

                //Convert data
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, bufferString);

                lastRecognizedBird      = doc["birdType"];
                recognitionAccuracy     = doc["birdAccuracy"];
                lightIntensity          = doc["lightIntensity"];
                temperature             = doc["temperature"];
                humidity                = doc["humidity"];
                rainCoverage            = doc["rainCoverage"];
                raining                 = doc["raining"];
                batteryPercentage       = doc["batteryPercentage"];
                location[0]             = doc["lattitude"];
                location[1]             = doc["longtitude"];
                correctMeasurements     = doc["validation"];

                //TODO: Send data
                char buffer[11] = {lastRecognizedBird, recognitionAccuracy, lightIntensity, temperature, humidity, rainCoverage, raining, batteryPercentage, location[0], location[1], correctMeasurements};
                connection.SendPacket(buffer, 1);

                //Remove data
                remove(filePath);
            }
        }

        closedir(dp);
    }
}

void NotConnected() {
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

void InitHASFSM() {
    //Initializing states
    birdSensorFSM.addState(FSM_States::STATE_START,                 Start);
    birdSensorFSM.addState(FSM_States::STATE_INITIALIZING,          Initializing);
    birdSensorFSM.addState(FSM_States::STATE_INITIALIZING_FAILED,   InitializingFailed);
    birdSensorFSM.addState(FSM_States::STATE_LISTENING,             Listening);
    birdSensorFSM.addState(FSM_States::STATE_GATHERING_DATA,        GatheringData);
    birdSensorFSM.addState(FSM_States::STATE_SENDING,               Sending);
    birdSensorFSM.addState(FSM_States::STATE_NOT_CONNECTED,         NotConnected);

    //Initializing transistions
    birdSensorFSM.addTransition(FSM_States::STATE_START,          FSM_Events::EVENT_START,                FSM_States::STATE_INITIALIZING);
    birdSensorFSM.addTransition(FSM_States::STATE_INITIALIZING,   FSM_Events::SENSORS_INITIALIZED,        FSM_States::STATE_LISTENING);
    birdSensorFSM.addTransition(FSM_States::STATE_INITIALIZING,   FSM_Events::INITIALIZING_FAILED,        FSM_States::STATE_INITIALIZING_FAILED);
    birdSensorFSM.addTransition(FSM_States::STATE_LISTENING,      FSM_Events::BIRD_FOUND,                 FSM_States::STATE_GATHERING_DATA);
    birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA, FSM_Events::SEND_INTERVAL_NOT_REACHED,  FSM_States::STATE_LISTENING);
    birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA, FSM_Events::SEND_INTERVAL_REACHED,      FSM_States::STATE_SENDING);
    birdSensorFSM.addTransition(FSM_States::STATE_SENDING,        FSM_Events::SEND_SUCCEEDED,             FSM_States::STATE_LISTENING);
    birdSensorFSM.addTransition(FSM_States::STATE_SENDING,        FSM_Events::JOIN_FAILED,                FSM_States::STATE_NOT_CONNECTED);
    birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED,  FSM_Events::JOIN_SUCCESFULL,            FSM_States::STATE_SENDING);
    birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED,  FSM_Events::CONNECT_FAILED,             FSM_States::STATE_NOT_CONNECTED);
    birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED,  FSM_Events::CONNECTION_TIMEOUT,         FSM_States::STATE_LISTENING);

    lastTimeSent = millis();
}