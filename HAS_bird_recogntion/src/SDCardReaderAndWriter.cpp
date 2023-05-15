#include <SDCardReaderAndWriter.h>

bool SDCardReaderAndWriter::InitSDCardReaderAndWriter() {
    if (fs.mount(&blockDevice)) {
        Serial.println("No filesystem found");
        return false;
    } else {
        return true;
    }
}

void SDCardReaderAndWriter::WriteToSDCard() {

}

char* SDCardReaderAndWriter::ReadFileData(char* fileName) {
    FILE* filePointer = fopen(fileName, "a");
    char buffer[100];
    
    /* Seek to the beginning of the file */
    fseek(filePointer, 0, SEEK_SET);

    return nullptr;
}