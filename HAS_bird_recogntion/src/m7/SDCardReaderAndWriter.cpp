#include <SDCardReaderAndWriter.h>

SDMMCBlockDevice blockDevice;
mbed::FATFileSystem fs("sd-card");

bool SDCardReaderAndWriter::InitSDCardReaderAndWriter() {
    int err =  fs.mount(&blockDevice);

    if (err) {
        Serial.println("No filesystem found");
        fflush(stdout);
        err = fs.reformat(&blockDevice);
        return false;
    } else {
        Serial.println("Filesystem found");
        return true;
    }
}

void SDCardReaderAndWriter::WriteToSDCard(int birdType, float birdAccuracy, float lightIntensity, float temp, float hum, int rainSurface, bool raining, int batteryPercentage, float lat, float lon, uint8_t validation) {
    //TODO: Generate name with timestamp
    char* fileName = "/fs/test20.txt";

        //Open file or create new file if file doesn't exist
        FILE *filePointer = fopen("/fs/test20.txt", "w");

        if (filePointer == nullptr) {
            Serial.println("File not created");
            return;
        }

        Serial.println("File created");

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

        Serial.println("JSON Created");

        char output[1024];
        serializeJson(doc, output);

        Serial.println(output);

        //Write JSON object to file
        fprintf(filePointer, output);

        //Close file
        fclose(filePointer);
        Serial.println("File closed");

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