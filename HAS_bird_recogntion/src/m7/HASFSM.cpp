#include <HASFSM.h>
#include <SensorData.h>

FSM birdSensorFSM = FSM(STATE_TOTAL, EVENTS_TOTAL);

SensorData              sensorData;
LoRaConnection          connection;
SDCardReaderAndWriter   sd;

uint32_t            lastTimeSent;

Available_Birds   lastRecognizedBird;
float             recognitionAccuracy;
float             lightIntensity;
float             temperature;
float             humidity;
bool              raining;
float             rainCoverage;
float             batteryPercentage;
float             lattitude;
float             longtitude;

void Start() {

}

void Initializing() {
    sensorData = SensorData();
    connection = LoRaConnection();

    sensorData.InitSensors();
    connection.InitConnection();

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

    while ((millis() - startListeningTime) < (LISTEN_TIME * 1000)) {
        //TODO: Take audio fragment
    }

    //TODO: Check for bird
    bool birdFound = true;
    //TODO: Update bird variable and accuracy

    //Update state if a bird was found
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

    //TODO: Get battery percentage
    batteryPercentage = 20; //TEMP VALUE

    //Validate
    uint8_t correctMeasurements = sensorData.ValidateSensorData(lightIntensity, temperature, humidity, rainCoverage, raining, batteryPercentage);

    //Sent measurements to SDCard
    sd.WriteToSDCard(lastRecognizedBird, recognitionAccuracy, lightIntensity, temperature, humidity, rainCoverage, raining, batteryPercentage, lattitude, longtitude, correctMeasurements);

    //Check send interval
    if ((millis() - lastTimeSent) >= (SEND_INTERVAL * 60 * 1000)) {
        birdSensorFSM.raiseEvent(SEND_INTERVAL_REACHED);
        lastTimeSent = millis();
    } else {
        birdSensorFSM.raiseEvent(SEND_INTERVAL_NOT_REACHED);
    }
}

void Sending() {

}

void NotConnected() {

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