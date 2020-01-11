/*
 * GPS GP-20U7 Driver Header
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

/*
VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!

                 uuuuuuu
             uu$$$$$$$$$$$uu
          uu$$$$$$$$$$$$$$$$$uu
         u$$$$$$$$$$$$$$$$$$$$$u
        u$$$$$$$$$$$$$$$$$$$$$$$u
       u$$$$$$$$$$$$$$$$$$$$$$$$$u
       u$$$$$$$$$$$$$$$$$$$$$$$$$u
       u$$$$$$"   "$$$"   "$$$$$$u
       "$$$$"      u$u       $$$$"
        $$$u       u$u       u$$$
        $$$u      u$$$u      u$$$
         "$$$$uu$$$   $$$uu$$$$"
          "$$$$$$$"   "$$$$$$$"
            u$$$$$$$u$$$$$$$u
             u$"$"$"$"$"$"$u
  uuu        $$u$ $ $ $ $u$$       uuu
 u$$$$        $$$$$u$u$u$$$       u$$$$
  $$$$$uu      "$$$$$$$$$"     uu$$$$$$
u$$$$$$$$$$$uu    """""    uuuu$$$$$$$$$$
$$$$"""$$$$$$$$$$uuu   uu$$$$$$$$$"""$$$"
 """      ""$$$$$$$$$$$uu ""$"""
           uuuu ""$$$$$$$$$$uuu
  u$$$uuu$$$$$$$$$uu ""$$$$$$$$$$$uuu$$$
  $$$$$$$$$$""""           ""$$$$$$$$$$$"
   "$$$$$"                      ""$$$$""
     $$$"                         $$$$"

In order to successfully poll the GPS, the serial RX buffer size must be increased. This needs
to be done on the computer used for compilation. This can be done by navigating to the following
path in the Arduino contents folder:
On Mac: Got to the Applications folder, right click on the Arduino app, select Show Package Contents,
    then navigate to ‎⁨Contents⁩/⁨Java⁩/⁨hardware⁩/⁨teensy⁩/⁨avr⁩/⁨cores⁩/⁨teensy3⁩/serial1.c
On Windows: [user_drive]\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\serial1.c

On line 43 increase SERIAL1_RX_BUFFER_SIZE from 64 to 1024

THIS MUST BE DONE ON THE COMPUTER USED TO COMPILE THE CODE!!!

VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!
*/

/*Includes------------------------------------------------------------*/
#include "sensors/GPS.h"

void GPS::initSensor() {
    #ifdef TESTING
            SerialUSB.println("Initializing GPS");
        #endif
        m_serial_gps.begin(9600);  //baud rate 9600 for the GP-20U7
        while (!m_serial_gps) {}

    status = SensorStatus::NOMINAL;
}

void GPS::readData() {

    bool gpsSuccess = false;
    elapsedMillis timeout;
    while(m_serial_gps.available() && (timeout < GPS_TIMEOUT)) {
        char c = m_serial_gps.read();
        if (gps.encode(c)) {
            gpsSuccess = true;
            break;
        }
    }

    if(!gpsSuccess) {
        status = SensorStatus::FAILURE;
    }

    unsigned long fix_age;
    gps.f_get_position(&data[0], &data[1], &fix_age);
    data[2] = gps.f_altitude();

    status = SensorStatus::NOMINAL;
}

uint8_t GPS::dataLength() {
    return GPS_DATA_ARRAY_SIZE;
}

float *GPS::getData() {
    return data;
}
