#ifndef SD_CARD_READER_AND_WRITER_H
#define SD_CARD_READER_AND_WRITER_H

#include <SDMMCBlockDevice.h>
#include <FATFileSystem.h>
#include <ArduinoJson.h>

#define AMOUNT_OF_ITEMS_TO_WRITE    JSON_OBJECT_SIZE(11)

class SDCardReaderAndWriter {
    public:
        SDCardReaderAndWriter(){};
        ~SDCardReaderAndWriter(){};

        bool InitSDCardReaderAndWriter();

        void WriteToSDCard(int birdType, float birdAccuracy, float lightIntensity, float temp, float hum, int rainSurface, bool raining, int batteryPercentage, float lat, float lon, uint8_t validation);
        char* ReadFileData(char* fileName);

    private:
};

#endif