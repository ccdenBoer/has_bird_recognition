#ifndef LORA_CONNECTION_H
#define LORA_CONNECTION_H

#include <Arduino.h>
#include <iostream>

#define LORA_SERIAL             Serial2 //RX2 and TX2
#define LORA_TIMEOUT            100     //MS
#define MAX_BUFFER_LENGTH       256
#define MAX_RECONNECT_ATTEMPTS  20

enum device_mode { 
    LWABP = 0, 
    LWOTAA = 1
};

enum physical_type { 
    EU434 = 0, 
    EU868 = 1
};

enum data_rate { 
    DR0 = 0, 
    DR1 = 1, 
    DR2 = 2, 
    DR3 = 3
};

enum class_type { 
    CLASS_A = 0, 
    CLASS_C = 1 
};

enum power_mode {
    LOW_POWER = 0,
    MEDIUM_POWER = 1,
    HIGH_POWER = 2
};

enum otaa_join { 
    JOIN = 0, 
    FORCE 
};

class LoRaConnection
{
    private:
        char loraBuffer[256];

        void SendCommand(char* commandToSend);
        short ReadBuffer(char* buffer, short length, unsigned short timeout);

    public:
        LoRaConnection();
        ~LoRaConnection(){};

        void InitConnection();

        void InitialSetup();
        
        void SendKey(char* networkSessionKey, char* applicationSessionKey, char* applicationKey);

        void SetDeviceMode(device_mode mode);

        void SetDataRate(data_rate dataRate, physical_type physicalType);

        void SetChannel(unsigned char channel, float frequency);

        void SetClassType(class_type type);

        void SetPort(unsigned char port);

        bool SetOTAAJoin(otaa_join command, unsigned char timeout);

        void SetReceiveWindow(unsigned int window, unsigned char channel, float frequency);
        void SetReceiveWindow(unsigned int window, float frequency, data_rate dataRate);

        void EnableDutyCycle(bool enabled);
        void EnableDutyCycleJoin(bool enabled);

        void SetPowerMode(power_mode powerMode);
        void SetPowerMode(short power);

        bool SendPacket(char* buffer, unsigned char timeout);
        bool SendPacketCayenne(unsigned char *buffer, unsigned char length, unsigned char timeout);
};


#endif