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
#include <vector>

#include "buzzer.h"
#include "hw-interface.h"
#include "status_enums.h"

/*Functions------------------------------------------------------------*/
void raiseToStatus(RocketStatus &currentStatus, RocketStatus incomingStatus);

class SensorCollection;

void displayStatus(SensorCollection &sensors,
                   std::vector<std::reference_wrapper<IIgnitor>> &hardware,
                   IBuzzer &buzzer);
