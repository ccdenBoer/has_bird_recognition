#include <SDCardReaderAndWriter.h>

mbed::FATFileSystem fs("fs");

bool SDCardReaderAndWriter::InitSDCardReaderAndWriter() {
    if (fs.mount(&blockDevice)) {
        Serial.println("No filesystem found");
        return false;
    } else {
        return true;
    }
}

void SDCardReaderAndWriter::WriteToSDCard(int birdType, float birdAccuracy, float lightIntensity, float temp, float hum, int rainSurface, bool raining, int batteryPercentage, float lat, float lon, uint8_t validation) {
    //TODO: Generate name with timestamp
    char* fileName = "test.txt";
    
    //Open file or create new file if file doesn't exist
    FILE *filePointer = fopen(strcat("/fs/", fileName), "w");

    //Create JSON object
    StaticJsonDocument<AMOUNT_OF_ITEMS_TO_WRITE> doc;
    doc["birdType"] = birdType;
    doc["birdAccuracy"] = birdAccuracy;
    doc["lightIntensity"] = lightIntensity;
    doc["temperature"] = temp;
    doc["humidity"] = hum;
    doc["rainCoverage"] = rainSurface;
    doc["raining"] = raining;
    doc["batteryPercentage"] = batteryPercentage;
    doc["lattitude"] = lat;
    doc["longtitude"] = lon;
    doc["validation"] = validation;

    char output[256];
    serializeJson(doc, output);
    
    //Write JSON object to file
    fprintf(filePointer, output);

    //Close file
    fclose(filePointer);

}

char* SDCardReaderAndWriter::ReadFileData(char* fileName) {
    FILE* filePointer = fopen(fileName, "a");

    int end = fseek(filePointer, 0, SEEK_END);
    char buffer[end];
    
    /* Seek to the beginning of the file */
    fseek(filePointer, 0, SEEK_SET);

    fread(buffer, end + 1, 1, filePointer);
    printf("%s\n", buffer);
    fclose(filePointer);

    return buffer;
}