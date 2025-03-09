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
#include "log.hpp"

GPS::GPS(Hal::CustomSerial &seri, float *const data) : SensorBase(data), gps(&seri.getSerial())
{
#ifdef TESTING
    LOG_DEBUG("Initializing GPS");
#endif
    gps.begin(9600); // 9600 NMEA is the default baud rate for Adafruit MTK GPS's
    // while (!serial_port_) {
    // }

    // while (1) {
    //     if (serial_port_.available()) {
    //         LOG_DEBUG("SERIAL AVAIL");
    //     } else {
    //         LOG_DEBUG("SERIAL FAIL");
    //     }
    // }

    // gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    // gps.sendCommand(PGCMD_ANTENNA);

    status = SensorStatus::NOMINAL;
}

void GPS::readData() {
    bool gpsSuccess = false;
    elapsedMillis timeout;
    // while (1) {
    //     char c = serial_port_.read();
    //     SerialLogger.print('[');
    //     SerialLogger.print(static_cast<int>(c));
    //     SerialLogger.print(']');
    //     SerialLogger.print(c);
    //     // if (gps.encode(c)) {
    //     //     gpsSuccess = true;
    //     //     break;
    //     // }
    // }

    // // SerialLogger.println("<EOT>");

    // TODO add timeout
    while (1) {
        char c = gps.read();
        SerialLogger.print(c);
        if (gps.newNMEAreceived() && gps.parse(gps.lastNMEA())) {
            gpsSuccess = true;
            SerialLogger.print("[GPS DATA]");
            // break;
        }
        // GPS.parse(GPS.lastNMEA())
        // if (c == '*') {
        //     if (val) {
        //         SerialLogger.print("Y");
        //     } else {
        //         SerialLogger.print("N");
        //     }
        // }

    }

    if (!gpsSuccess) {
        status = SensorStatus::FAILURE;
    }

#ifdef TESTING
    if (gpsSuccess) {
        LOG_DEBUG("GPS SUCCESS");
    } else {
        LOG_DEBUG("GPS FAIL");
    }
#endif

    // unsigned long fix_age;
    // gps.f_get_position(data_, data_ + 1, &fix_age);
    // data_[2] = gps.f_altitude();

    // #ifdef TESTING
    //     LOG_DEBUG("Polling GPS");
    //     FLOG_DEBUG("GPS Latitude: ", data_[0]);
    //     FLOG_DEBUG("GPS Longitude: ", data_[1]);
    //     FLOG_DEBUG("GPS Altitude: ", data_[2]);
    // #endif

    status = SensorStatus::NOMINAL;
}
