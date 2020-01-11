/*
 * Cameras Header
 *
 * @file    cameras.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description  File that controls commanding the camera modules.
 *  Camera module:  Runcam Split 2S
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#ifndef CAMERAS_H
#define CAMERAS_H

/*Includes------------------------------------------------------------*/
#include <hal/port.h>
#include <hal/time.h>
#include "sensors.h"

/*Constants------------------------------------------------------------*/
// #define SerialCamera        Serial3

/*Functions------------------------------------------------------------*/

class Camera{
public:
    Camera(Hal::ISerial &SerialCam): m_seri_cam(SerialCam) {
        m_seri_cam.begin(M_CAMERA_BAUD);
        while (!m_seri_cam);
        Hal::sleep_ms(2000);
        stop_record();
    }
    void power_cameras(); //a toggle switch
    void start_record();
    void stop_record();

private:
    uint8_t crc_calculator(uint8_t *command, uint8_t len);
    uint8_t crc8_dvb_s2(uint8_t crc, unsigned char a);

    Hal::ISerial &m_seri_cam;
    static constexpr auto M_CAMERA_BAUD = 115200;
};
#endif
