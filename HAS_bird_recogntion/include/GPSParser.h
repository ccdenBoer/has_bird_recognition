#ifndef GPS_PARSER_H
#define GPS_PARSER_H

#include "Arduino.h"
#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>

#define GPS_SERIAL Serial1
#define GPS_PORT_NAME "Serial1"

static NMEAGPS  gps;
static gps_fix  fix;

class GPSParser {
  private:

  public:
    void readGPS();
    void print_info();
    void setup();
    bool getDateTime(char* dateTime);
    bool getCoordinates(float location[2]);

};

#endif