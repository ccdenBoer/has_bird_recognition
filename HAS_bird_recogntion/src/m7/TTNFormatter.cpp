#include "TTNFormatter.h"

message_t TTNFormatter::convertStringToMessage(char *messageString)
{
    // Convert data
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, messageString);

    message_t message;

  	message.birdType = doc["birdType"];
  	message.birdAccuracy = (uint8_t)(doc["birdAccuracy"].as<float>() * 256.0f);
  	message.lightIntensity = doc["lightIntensity"];
  	message.temperature = doc["temperature"];
  	message.humidity = doc["humidity"];
  	message.raincoverage = doc["rainCoverage"];
  	message.raining = doc["raining"];
  	message.batteryPercentage = doc["batteryPercentage"];
  	message.lattitude = doc["lattitude"];
  	message.longtitude = doc["longtitude"];
  	message.validation = doc["validation"];

    return message;
}

size_t TTNFormatter::convertPayloadToTTN(payload_t payload, uint8_t *buffer, size_t bufferSize)
{
  uint8_t payloadSize = (sizeof(message_t) * payload.messageCount) + sizeof(uint16_t);
  if (bufferSize < payloadSize) {
	printf("bufferSize size too small\n");
	return 0;
  }

  buffer[0] = (uint8_t)payload.messageCount;
  buffer[1] = sizeof(message_t);
  for(int i = 0; i < payload.messageCount; i++) {
	auto *message = (message_t *) &payload.messages[i];
	memcpy(&buffer[2 + (i * sizeof(message_t))], message, sizeof(message_t));
  }
  return payloadSize;
}

payload_t TTNFormatter::convertTTNtoPayload(uint8_t *payloadString)
{
  payload_t payload;
  payload.messageCount = payloadString[0];
  payload.messages = (message_t *) &payloadString[1];
  return payload;
}

