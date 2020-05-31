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

// #include "HAL/time.h"

#include "buzzer.h"
#include "hw-interface.h"
// #include "options.h"
// #include "sensors-interface.h"
// #include "state_interface.h"
// #include "statemachine.h"

// #include "CSVwrite.h"

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/
enum class Status {
    NOMINAL = 0x00,
    NONCRITICAL_FAILURE = 0x01,
    CRITICAL_FAILURE = 0x03
};

void raiseToStatus(Status &currentStatus, Status incomingStatus);
// /*Functions------------------------------------------------------------*/

// void initSensors(std::vector<std::reference_wrapper<ISensor>> &sensors,
//                  std::vector<std::reference_wrapper<IParachute>>
//                  &hardware, IBuzzer &buzzer);
class SensorCollection;

void displayStatus(SensorCollection &sensors,
                   std::vector<std::reference_wrapper<IParachute>> &hardware,
                   IBuzzer &buzzer);

// Status getStatus(std::vector<std::reference_wrapper<ISensor>> &sensors,
//                  std::vector<std::reference_wrapper<IParachute>>
//                  &hardware);

// // TODO: Remove dependency of sensors.h for GP20U7.cpp/.h

// void pollSensors(Hal::t_point &timestamp,
//                  std::vector<std::reference_wrapper<ISensor>> &sensors);

// void logData(unsigned long timestamp,
//              std::vector<std::reference_wrapper<ISensor>> &sensors,
//              StateId state, float altitude, float baseline_pressure);
