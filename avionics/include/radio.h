/*
 * Radio Header
 *
 * @file    radio.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Implements radio send functions and prioritizing
 * sensor data to send over the radio.  This source utilizes the
 * XBee.cpp and .h files also included in this distro.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#ifndef __RADIO_H__
#define __RADIO_H__

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include <stdint.h>

#include "statemachine.h"
#include "sensors.h"
#include "options.h"
#include "XBee.h"

/*Constants------------------------------------------------------------*/
/* radio addressing */
#define GND_STN_ADDR_MSB 0x0013A200 //Ground Station - Body
#define GND_STN_ADDR_LSB 0x41678FC0

/*Functions------------------------------------------------------------*/
void sendRadioBody(XBee* radio, ZBTxRequest* txPacket, float*, FlightStates
        state, float *altitude, uint32_t*);
void sendRadioNosecone(XBee* radio, ZBTxRequest* txPacket, float* GPS_data,
        float bar_data[], float acc_data[], float *temp_sensor_data, float IMU_data[]);

void resolveRadioRx(XBee* radio, ZBTxRequest* txPacket, float GPS_data[],
        FlightStates *state, Status *status);
void radioStatus(XBee* radio, ZBTxRequest* txPacket, Status *status);
void doCommand(char command, float GPS_data[], FlightStates *state,
        Status *status, XBee* radio, ZBTxRequest* txPacket);
void sendMessage(XBee* radio, ZBTxRequest* txPacket, String* msg);
#endif
