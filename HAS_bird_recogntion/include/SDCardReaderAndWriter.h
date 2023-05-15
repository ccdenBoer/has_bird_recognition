#ifndef SD_CARD_READER_AND_WRITER_H
#define SD_CARD_READER_AND_WRITER_H

#include <SDMMCBlockDevice.h>
#include <FATFileSystem.h>

mbed::FATFileSystem fs("fs");

class SDCardReaderAndWriter {
    public:
        SDCardReaderAndWriter(){};
        ~SDCardReaderAndWriter(){};

        bool InitSDCardReaderAndWriter();

        void WriteToSDCard();
        char* ReadFileData(char* fileName);

    private:
        SDMMCBlockDevice blockDevice;
};

#endif