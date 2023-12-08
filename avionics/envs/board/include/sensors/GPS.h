/*
 * GPS Copernicus II DIP Driver Header
 *
 * @file    GPS.h
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

#pragma once

/*Includes------------------------------------------------------------*/
#include <array>

#include "sensors_interface.h"
#include <HAL/port_impl.h>
#include <TinyGPS.h>
#include <elapsedMillis.h>

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/

class GPS : public SensorBase<GPS_DATA_LENGTH> {
  public:
    GPS(Hal::CustomSerial &seri, float *const data);
    void readData();

  private:
    constexpr static int GPS_FIELD_LENGTH = 20;
    constexpr static int GPS_TIMEOUT = 100;

    Hal::CustomSerial &serial_port_;
    TinyGPS gps;

    /*GPS initialization commands*/
    const std::array<uint8_t, 9> GPS_reset_defaults;
    const std::array<uint8_t, 11> GPS_set_baud_rate;    // 4800
    const std::array<uint8_t, 16> GPS_set_NMEA_message; // GPGGA
    const std::array<uint8_t, 10> GPS_set_update_rate;  // 1 Hz
};
