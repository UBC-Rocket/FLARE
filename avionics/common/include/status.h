/*
 * Sensors Header
 *
 * @file    sensors.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Main sensor initialization and polling file.
 * Implements sensor initialization and error checking as well as
 * sensor polling, sensor logging, and sending data over the radio.
 * Also performs rocket status-related work - startup buzzer, status report
 * generation, etc.
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
#include <stdint.h>

#include "buzzer.h"
#include "hw-interface.h"
#include "status_enums.h"

/*Functions------------------------------------------------------------*/

/**
 * @brief updates currentStatus to incomingStatus if incomingStatus > currentStatus
 * @param currentStatus 
 * @param incomingStatus 
 */
void raiseToStatus(RocketStatus &currentStatus, RocketStatus incomingStatus);

class SensorCollection;
class IgnitorCollection;

/**
 * @brief get current status of sensors and calls raiseToStatus
 * 
 * @param sensors used to get current status of sensors
 * @param ignitors can be used to update status of sensors
 * @return status of sensors
 */
RocketStatus collectStatus(SensorCollection &sensors,
                           IgnitorCollection &ignitors);

void displayStatus(RocketStatus status, IBuzzer &buzzer);
