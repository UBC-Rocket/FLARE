/*
 * GPS Copernicus II DIP Driver Header
 *
 * @file    GPS.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  File that receives GPS serial data and encodes it
 * with a TinyGPS parsing library.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

/*Includes------------------------------------------------------------*/
#include "sensors/GPS.h"
#include "options.h"

GPS::GPS(Hal::CustomSerial &seri, float *const data)
    : SensorBase(data),
      serial_port_(seri), GPS_reset_defaults{0xA0, 0xA1, 0x00,
                                                         0x02, 0x04, 0x00,
                                                         0x04, 0x0D, 0x0A},
      GPS_set_baud_rate{0xA0, 0xA1, 0x00, 0x04, 0x05, 0x00,
                        0x00, 0x00, 0x05, 0x0D, 0x0A},
      GPS_set_NMEA_message{0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0D, 0x0A},
      GPS_set_update_rate{0xA0, 0xA1, 0x00, 0x03, 0x0E,
                          0x01, 0x00, 0x0F, 0x0D, 0x0A}

{
#ifdef TESTING
    Serial.println("Initializing GPS");
#endif
    serial_port_.begin(4800); // baud rate of Copernicus II DIP module
    while (!serial_port_) {
    }

    status = SensorStatus::NOMINAL;
}

void GPS::readData() {
    bool gpsSuccess = false;
    elapsedMillis timeout;
    while (serial_port_.available() && (timeout < GPS_TIMEOUT)) {
        char c = serial_port_.read();
        if (gps.encode(c)) {
            gpsSuccess = true;
            break;
        }
    }

    if (!gpsSuccess) {
        status = SensorStatus::FAILURE;
    }

    unsigned long fix_age;
    gps.f_get_position(data_, data_ + 1, &fix_age);
    data_[2] = gps.f_altitude();

    #ifdef TESTING
        Serial.println("Polling GPS");
        Serial.print("gps lat: "); Serial.println(data_[0], 10);
        Serial.print("gps lon: "); Serial.println(data_[1], 10);
        Serial.print("gps alt: "); Serial.println(data_[2], 10);
    #endif

    status = SensorStatus::NOMINAL;
}
