#include <HASFSM.h>
#include <SensorData.h>

SensorData      sensorData;
LoRaConnection  connection;

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
    float   lightIntensity  = sensorData.GetLightIntensity();
    float   temperature     = sensorData.GetTemperature();
    float   humidity        = sensorData.GetHumidity();
    bool    raining         = sensorData.GetRainThreshold();
    float   rainCoverage    = sensorData.GetRainSurface();

    //TODO: Get battery percentage

    //TODO: Validate
    if (sensorData.ValidateSensorData(lightIntensity, temperature, humidity, rainCoverage)) {
        //TODO: Write data to SD-Card
    } else {
        //TODO: What to do when data is invalid
    }

    //TODO: Check send interval
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
}