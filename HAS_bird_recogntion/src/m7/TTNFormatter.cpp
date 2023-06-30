#include "TTNFormatter.h"

message_t TTNFormatter::convertStringToMessage(char *messageString)
{
    // Convert data
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, messageString);

    message_t tempPayload;

    printf("payload init\n");

    tempPayload.birdType = doc["birdType"];
    tempPayload.birdAccuracy = (uint8_t)(doc["birdAccuracy"].as<float>() * 256.0f);
    tempPayload.lightIntensity = doc["lightIntensity"];
    tempPayload.temperature = doc["temperature"];
    tempPayload.humidity = doc["humidity"];
    tempPayload.raincoverage = doc["rainCoverage"];
    tempPayload.raining = doc["raining"];
    tempPayload.batteryPercentage = doc["batteryPercentage"];
    tempPayload.lattitude = doc["lattitude"];
    tempPayload.longtitude = doc["longtitude"];
    tempPayload.validation = doc["validation"];

    return tempPayload;
}