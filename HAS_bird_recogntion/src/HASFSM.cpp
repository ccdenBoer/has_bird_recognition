#include <HASFSM.h>
#include <SensorData.h>

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

    //TODO: Sent measurements to SDCard

    //Check send interval
    if ((millis() - lastTimeSent) >= (SEND_INTERVAL * 60 * 1000)) {
        birdSensorFSM.raiseEvent(SEND_INTERVAL_REACHED);
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
    birdSensorFSM.addState(FSM_States::STATE_START,           Start);
    birdSensorFSM.addState(FSM_States::STATE_INITIALIZING,    Initializing);
    birdSensorFSM.addState(FSM_States::STATE_LISTENING,       Listening);
    birdSensorFSM.addState(FSM_States::STATE_GATHERING_DATA,  GatheringData);
    birdSensorFSM.addState(FSM_States::STATE_SENDING,         Sending);
    birdSensorFSM.addState(FSM_States::STATE_NOT_CONNECTED,   NotConnected);

    //Initializing transistions
    birdSensorFSM.addTransition(FSM_States::STATE_START,          FSM_Events::EVENT_START,                FSM_States::STATE_INITIALIZING);
    birdSensorFSM.addTransition(FSM_States::STATE_INITIALIZING,   FSM_Events::SENSORS_INITIALIZED,        FSM_States::STATE_LISTENING);
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