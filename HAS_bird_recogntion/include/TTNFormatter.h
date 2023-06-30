#ifndef TTN_FORMATTER_H
#define TTN_FORMATTER_H

#include "HASFSM.h"
#include <ArduinoJson.h>

class TTNFormatter{
private:
    
public:
    TTNFormatter(/* args */) {};
    ~TTNFormatter() {};

    message_t convertStringToMessage(char* messageString);
};


#endif