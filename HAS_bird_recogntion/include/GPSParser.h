#ifndef GPS_PARSER_H
#define GPS_PARSER_H

#include "Arduino.h"
#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <Timezone.h> 

#define GPS_SERIAL Serial1
#define GPS_PORT_NAME "Serial1"

static NMEAGPS  gps;
static gps_fix  fix;

class GPSParser {
  private:
    TimeChangeRule myDST = {"GMT", Second, Sun, Mar, 2, 120};    // Daylight time = UTC + 2 hours
    TimeChangeRule mySTD = {"GMT", First, Sun, Nov, 2, 60};     // Standard time = UTC + 1 hours
    Timezone myTZ = Timezone(myDST, mySTD); // initialized in setup
    TimeChangeRule *tcr;  // pointer to the time change rule, use to get TZ abbrev 

  public:
    void readGPS();
    void print_info();
    void setup();
    bool getDateTime(char* dateTime);
    bool getCoordinates(float location[2]);

};

#endif