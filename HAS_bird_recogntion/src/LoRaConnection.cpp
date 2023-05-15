#include <LoRaConnection.h>

/*
* Public methods declaration
*/
LoRaConnection::LoRaConnection() {
    memset(loraBuffer, 0, 256);
}

void LoRaConnection::InitConnection() {
    LORA_SERIAL.begin(9600); //Start the LoRa serial at a BaudRate of 9600
}

void LoRaConnection::SendKey(char* networkSessionKey, char* applicationSessionKey, char* applicationKey) {
    char command[64];
    
    if(networkSessionKey)
    {
        memset(command, 0, 64);
        sprintf(command, "AT+KEY=NWKSKEY,\"%s\"\r\n", networkSessionKey);
        this->SendCommand(command);
        delay(LORA_TIMEOUT);
    }

    if(applicationSessionKey)
    {
        memset(command, 0, 64);
        sprintf(command, "AT+KEY=APPSKEY,\"%s\"\r\n", applicationSessionKey);
        this->SendCommand(command);
        delay(LORA_TIMEOUT);
    }

    if(applicationKey)
    {
        memset(command, 0, 64);
        sprintf(command, "AT+KEY= APPKEY,\"%s\"\r\n", applicationKey);
        this->SendCommand(command);
        delay(LORA_TIMEOUT);
    }
}

void LoRaConnection::SetDeviceMode(device_mode mode) {
    switch (mode) {
    case device_mode::LWABP:
        this->SendCommand("AT+MODE=LWABP\r\n");
        break;
    case device_mode::LWOTAA:
        this->SendCommand("AT+MODE=LWOTAA\r\n");
        break;
    default:
        Serial.println("Device mode not found");
        break;
    }
}

void LoRaConnection::SetDataRate(data_rate dataRate, physical_type physicalType) {
    char command[32];

    switch(physicalType) {
        case physical_type::EU434:
            this->SendCommand("AT+DR=EU433\r\n");
            break;
        case physical_type::EU868:
            this->SendCommand("AT+DR=EU868\r\n");
            break;
        default:
            Serial.println("Physical type not found");
    }
    
    delay(LORA_TIMEOUT);
    
    memset(command, 0, 32);
    sprintf(command, "AT+DR=%d\r\n", dataRate);
    this->SendCommand(command);

    delay(LORA_TIMEOUT);
}

void LoRaConnection::SetChannel(unsigned char channel, float frequency) {
    char command[32];
    memset(command, 0, 32);

    sprintf(command, "AT+CH=%d,%d.%d\r\n", channel, (short)frequency, short(frequency * 10) % 10);
    this->SendCommand(command);

    delay(LORA_TIMEOUT);
}

void LoRaConnection::SetClassType(class_type type) {
    switch (type) {
        case class_type::CLASS_A:
            this->SendCommand("AT+CLASS=A\r\n");
            break;
        case class_type::CLASS_C:
            this->SendCommand("AT+CLASS=C\r\n");
            break;
        default:
            Serial.println("Class type not found");
    }

    delay(LORA_TIMEOUT);
}

void LoRaConnection::SetPort(unsigned char port) {
    char command[32];
    memset(command, 0, 32);

    sprintf(command, "AT+PORT=%d\r\n", port);
    this->SendCommand(command);

    delay(LORA_TIMEOUT);
}

void LoRaConnection::SetReceiveWindow(unsigned int window, unsigned char channel, float frequency) {
    char command[32];
    memset(command, 0, 32);

    sprintf(command, "AT+RXWIN%d=%d,%d.%d\r\n", window, channel, (short)frequency, short(frequency * 10) % 10);
    this->SendCommand(command);

    delay(LORA_TIMEOUT);
}

void LoRaConnection::SetReceiveWindow(unsigned int window, float frequency, data_rate dataRate) {
    char cmd[32];
    memset(cmd, 0, 32);

    sprintf(cmd, "AT+RXWIN%d=%d.%d,%d\r\n", window, (short)frequency, short(frequency * 10) % 10, dataRate);
    this->SendCommand(cmd);

    delay(LORA_TIMEOUT);
}

void LoRaConnection::EnableDutyCycle(bool enabled) {
    if (enabled) {
        this->SendCommand("AT+LW=DC, ON\r\n");
    } else {
        this->SendCommand("AT+LW=DC, OFF\r\n");
    } 

    delay(LORA_TIMEOUT);
}

void LoRaConnection::EnableDutyCycleJoin(bool enabled) {
    if (enabled) {
        this->SendCommand("AT+LW=JDC,ON\r\n");
    } else {
        this->SendCommand("AT+LW=JDC,OFF\r\n");
    }

    delay(LORA_TIMEOUT);
}

void LoRaConnection::SetPowerMode(power_mode powerMode) {
    switch (powerMode){
        case power_mode::LOW_POWER:
            this->SendCommand("AT+LOWPOWER\r\n");
            break;
        case power_mode::MEDIUM_POWER:
            this->SendCommand("AT+POWER=7\r\n");
            break;
        case power_mode::HIGH_POWER:
            this->SendCommand("AT+POWER=14\r\n");
            break;
        default:
            Serial.println("Power mode not found");
            break;
    }
}

void LoRaConnection::SetPowerMode(short power) {
    char command[32];
    memset(command, 0, 32);

    sprintf(command, "AT+POWER=%d\r\n", power);
    this->SendCommand(command);

    delay(LORA_TIMEOUT);
}

bool LoRaConnection::SendPacket(char* buffer, unsigned char timeout) {
    unsigned char bufferLength = strlen(buffer);
    
    while(SerialLoRa.available())SerialLoRa.read();
    
    this->SendCommand("AT+MSG=\"");
    for(unsigned char i = 0; i < bufferLength; i ++)SerialLoRa.write(buffer[i]);
    this->SendCommand("\"\r\n");
    
    memset(loraBuffer, 0, MAX_BUFFER_LENGTH);
    this->ReadBuffer(loraBuffer, MAX_BUFFER_LENGTH, timeout);
 
    if(strstr(loraBuffer, "+MSG: Done")) {
        return true;
    } else {
        return false;
    }
}

bool LoRaConnection::SendPacketCayenne(unsigned char *buffer, unsigned char length, unsigned char timeout) {
    char temp[2] = {0};
    
    while(SerialLoRa.available()) {
        SerialLoRa.read();
    }
    
    this->SendCommand("AT+MSGHEX=\"");

    for(unsigned char i = 0; i < length; i ++) {
        sprintf(temp,"%02x", buffer[i]);
        SerialLoRa.write(temp); 
    }

    this->SendCommand("\"\r\n");
    
    memset(loraBuffer, 0, MAX_BUFFER_LENGTH);
    this->ReadBuffer(loraBuffer, MAX_BUFFER_LENGTH, timeout);
 
    if(strstr(loraBuffer, "+MSGHEX: Done")) {
        return true;
    } else {
        return false;
    }
}

/*
* Private methods declaration
*/
void LoRaConnection::SendCommand(char* commandToSend) {
    LORA_SERIAL.print(commandToSend);
}

short LoRaConnection::ReadBuffer(char* buffer, short length, unsigned char timeout) {
    short i = 0;
    unsigned long timerStart, timerEnd;

    timerStart = millis();

    while(1)
    {
        if(i < length)
        {
            while(SerialLoRa.available())
            {
                char c = SerialLoRa.read();  
                buffer[i ++] = c;
            }  
        }
        
        timerEnd = millis();
        if(timerEnd - timerStart > 1000 * timeout)break;
    }
    
    return i;
}