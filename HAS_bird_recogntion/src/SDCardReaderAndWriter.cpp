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

    int end = fseek(filePointer, 0, SEEK_END);
    char buffer[end];
    
    /* Seek to the beginning of the file */
    fseek(filePointer, 0, SEEK_SET);

    fread(buffer, end + 1, 1, filePointer);
    printf("%s\n", buffer);
    fclose(filePointer);

    return buffer;
}