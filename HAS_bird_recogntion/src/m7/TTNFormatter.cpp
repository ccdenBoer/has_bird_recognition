#include "TTNFormatter.h"

message_t TTNFormatter::convertStringToMessage(char *messageString)
{

    // Ensure messageString is not null
    if (messageString == nullptr) {
        printf("messageString is null\n");
        // Return a default message or handle error appropriately
        return message_t{};
    }

    // Convert data
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, messageString);
    if (error) {
        printf("deserializeJson() failed: %s\n", error.c_str());
        // Return a default message or handle error appropriately
        return message_t{};
    }

    const char* dateTime = doc["date/time"];
    if (!dateTime) {
        printf("date/time is missing in the JSON\n");
        // Return a default message or handle error appropriately
        return message_t{};
    }

    uint32_t date, time;
    
    // Extract the date part
    unsigned int year, month, day;
    sscanf(dateTime, "%4u-%2u-%2u", &year, &month, &day);
    // Reformat the date to yyyymmdd
    date = year * 10000 + month * 100 + day;

    // Extract the time part
    unsigned int hour, minute, second;
    sscanf(dateTime + 11, "%2u.%2u.%2u", &hour, &minute, &second);
    // Reformat the time to hhmmss
    time = hour * 10000 + minute * 100 + second;

    message_t message;

    message.birdList = doc["birdList"];
    message.birdType = doc["birdType"];
    message.birdAccuracy = static_cast<uint8_t>(doc["birdAccuracy"].as<float>() * 256.0f);
    message.date = date;
    message.time = time;
    message.lightIntensity = doc["lightIntensity"];
    message.temperature = doc["temperature"];
    message.humidity = doc["humidity"];
    message.rainLastHour = doc["rainLastHour"];
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

