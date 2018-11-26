/*GPS Venus638FLPx Driver Source*/

/*Includes------------------------------------------------------------*/
#include "Venus638FLPx.h"
#include "sensors.h"

#include <Arduino.h>
#include <TinyGPS.h>

/*Variables------------------------------------------------------------*/
TinyGPS gps;

/*Functions------------------------------------------------------------*/
bool updateGPS(void)
{
    elapsedMillis timeout;
    while(SerialGPS.available() && (timeout < GPS_TIMEOUT)) {
        char c = SerialGPS.read();
        if (gps.encode(c)) {
            return true;
        }
    }

    return false;
}

void getGPS(float* GPS_data)
{
    unsigned long fix_age;
    gps.f_get_position(&GPS_data[0], &GPS_data[1], &fix_age);
    GPS_data[2] = gps.f_altitude();
}
