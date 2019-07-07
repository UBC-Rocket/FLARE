/*
 * SatCom Header
 *
 * @file    satcom.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Initializes the satcom module.  Sends a message over SatCom when
 * the SatComSend function is called.  This source utilizes the IridiumSBD.cpp
 * and .h files also included in this distro.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#ifndef SATCOM_H
#define SATCOM_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

/*Constants------------------------------------------------------------*/
#define NUM_SATCOM_SENDS_ON_LANDED 5
#define SATCOM_LANDED_TIME_INTERVAL 60000 //in ms

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
bool SatComSetup(void);
int SatComQuality(void);
void SatComSendGPS(unsigned long*, float*);
void SatComSendCharArray(char*);

#endif
