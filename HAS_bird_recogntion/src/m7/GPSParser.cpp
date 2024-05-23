#include "GPSParser.h"

void GPSParser::readGPS(){
  while (gps.available( gpsPort )) {
    fix = gps.read();
    print_info();
  }
}

//Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars
void GPSParser::print_info(){
  trace_all( Serial, gps, fix );
}

void GPSParser::setup(){

  Serial.print( F("NMEA.INO: started\n") );
  Serial.print( F("  fix object size = ") );
  Serial.println( sizeof(gps.fix()) );
  Serial.print( F("  gps object size = ") );
  Serial.println( sizeof(gps) );
  Serial.println( F("Looking for GPS device on " GPS_PORT_NAME) );

  #ifndef NMEAGPS_RECOGNIZE_ALL
    #error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
  #endif

  #ifdef NMEAGPS_INTERRUPT_PROCESSING
    #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
  #endif

  #if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
      !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
      !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
      !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST )

    DEBUG_PORT.println( F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed.") );

  #else
    if (gps.merging == NMEAGPS::NO_MERGING) {
      Serial.print  ( F("\nWARNING: displaying data from ") );
      Serial.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      Serial.print  ( F(" sentences ONLY, and only if ") );
      Serial.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      Serial.println( F(" is enabled.\n"
                            "  Other sentences may be parsed, but their data will not be displayed.") );
    }
  #endif

  Serial.print  ( F("\nGPS quiet time is assumed to begin after a ") );
  Serial.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
  Serial.println( F(" sentence is received.\n"
                        "  You should confirm this with NMEAorder.ino\n") );

  trace_header( Serial );
  Serial.flush();

  GPS_SERIAL.begin( 9600 );

  readGPS();
}

bool GPSParser::getDateTime(char* dateTime){
  readGPS();
    if (fix.valid.date) {
        // format yy-mm-dd :: hh:mm:ss
        // easy format to sort filenames by
        sprintf(dateTime, "%d-%d-%d_%d:%d:%d", fix.dateTime.full_year(), fix.dateTime.month, fix.dateTime.day, fix.dateTime.hours, fix.dateTime.minutes, fix.dateTime.seconds);
        return true;
    } else {
        Serial.println("No valid date/time on fix");
        print_info();
        return false;
    }
}


bool GPSParser::getCoordinates(float location[2]){
  readGPS();
    if (fix.valid.location) {
        location[0] = fix.longitude();
        location[1] = fix.latitude();
        return true;
    } else {
        Serial.println("No valid location on fix");
        print_info();
        return false;
    }
}