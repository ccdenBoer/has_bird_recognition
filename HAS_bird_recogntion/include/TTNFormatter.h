#ifndef TTN_FORMATTER_H
#define TTN_FORMATTER_H

#include "HASFSM.h"
#include <ArduinoJson.h>

namespace TTNFormatter{
    message_t convertStringToMessage(char* messageString);
	size_t convertPayloadToTTN(payload_t payload, uint8_t *buffer, size_t bufferSize);
	payload_t convertTTNtoPayload(uint8_t *payloadString);
};


#endif