#ifndef HAS_FSM_H
#define HAS_FSM_H

#include <FSM.h>
#include <SensorData.h>
#include <LoRaConnection.h>

#define SEND_INTERVAL 15 //Minutes

enum FSM_States {
  STATE_START,
  STATE_INITIALIZING,
  STATE_LISTENING,
  STATE_GATHERING_DATA,
  STATE_SENDING,
  STATE_NOT_CONNECTED,
  STATE_TOTAL
};

enum FSM_Events {
  EVENT_START,
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

class HASFiniteStateMachine
{
    public:
      FSM birdSensorFSM = FSM(STATE_TOTAL, EVENTS_TOTAL);

      HASFiniteStateMachine() {};
      ~HASFiniteStateMachine() {};

      void InitHASFSM();
};

#endif