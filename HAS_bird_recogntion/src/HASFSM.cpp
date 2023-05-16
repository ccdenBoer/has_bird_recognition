#include <HASFSM.h>
#include <SensorData.h>

SensorData      sensorData;
LoRaConnection  connection;

uint32_t        lastTimeSent;

float   lightIntensity;
float   temperature;
float   humidity;
bool    raining;
float   rainCoverage;
float   batteryPercentage;

void Start() {

}

void Initializing() {
    sensorData = SensorData();
    connection = LoRaConnection();

    sensorData.InitSensors();
    connection.InitConnection();
}

void Listening() {

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

    //Check send interval
    if ((millis() - lastTimeSent) >= (SEND_INTERVAL * 60 * 1000)) {
        //TODO: Raise send event
    } else {
        //TODO: Go back to measuring state
    }
}

void Sending() {

}

void NotConnected() {

}

void HASFiniteStateMachine::InitHASFSM() {
    //Initializing states
    this->birdSensorFSM.addState(FSM_States::STATE_START,           Start);
    this->birdSensorFSM.addState(FSM_States::STATE_INITIALIZING,    Initializing);
    this->birdSensorFSM.addState(FSM_States::STATE_LISTENING,       Listening);
    this->birdSensorFSM.addState(FSM_States::STATE_GATHERING_DATA,  GatheringData);
    this->birdSensorFSM.addState(FSM_States::STATE_SENDING,         Sending);
    this->birdSensorFSM.addState(FSM_States::STATE_NOT_CONNECTED,   NotConnected);

    //Initializing transistions
    this->birdSensorFSM.addTransition(FSM_States::STATE_START,          FSM_Events::EVENT_START,                FSM_States::STATE_INITIALIZING);
    this->birdSensorFSM.addTransition(FSM_States::STATE_INITIALIZING,   FSM_Events::SENSORS_INITIALIZED,        FSM_States::STATE_LISTENING);
    this->birdSensorFSM.addTransition(FSM_States::STATE_LISTENING,      FSM_Events::BIRD_FOUND,                 FSM_States::STATE_GATHERING_DATA);
    this->birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA, FSM_Events::SEND_INTERVAL_NOT_REACHED,  FSM_States::STATE_LISTENING);
    this->birdSensorFSM.addTransition(FSM_States::STATE_GATHERING_DATA, FSM_Events::SEND_INTERVAL_REACHED,      FSM_States::STATE_SENDING);
    this->birdSensorFSM.addTransition(FSM_States::STATE_SENDING,        FSM_Events::SEND_SUCCEEDED,             FSM_States::STATE_LISTENING);
    this->birdSensorFSM.addTransition(FSM_States::STATE_SENDING,        FSM_Events::JOIN_FAILED,                FSM_States::STATE_NOT_CONNECTED);
    this->birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED,  FSM_Events::JOIN_SUCCESFULL,            FSM_States::STATE_SENDING);
    this->birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED,  FSM_Events::CONNECT_FAILED,             FSM_States::STATE_NOT_CONNECTED);
    this->birdSensorFSM.addTransition(FSM_States::STATE_NOT_CONNECTED,  FSM_Events::CONNECTION_TIMEOUT,         FSM_States::STATE_LISTENING);

    lastTimeSent = millis();
}