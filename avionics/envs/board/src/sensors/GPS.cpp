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
    gps.begin(9600); // baud rate of Adafruit Mini GPS PA1010D module
    // while (!serial_port_) {
    // }

    status = SensorStatus::NOMINAL;
}

void GPS::readData() {
    bool gpsSuccess = false;
    elapsedMillis timeout;
    while (gps.available()) {
        char c = gps.read();
        if (gps.newNMEAreceived() && gps.parse(gps.lastNMEA())) {
            gpsSuccess = true;
            SerialLogger.print("[GPS DATA]");
            data_[0] = gps.latitudeDegrees;
            data_[1] = gps.longitudeDegrees;
            data_[2] = gps.altitude;
            // break;
        }
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

    #ifdef TESTING
        LOG_DEBUG("Polling GPS");
        FLOG_DEBUG("GPS Latitude: ", data_[0]);
        FLOG_DEBUG("GPS Longitude: ", data_[1]);
        FLOG_DEBUG("GPS Altitude: ", data_[2]);
    #endif

    status = SensorStatus::NOMINAL;
}
