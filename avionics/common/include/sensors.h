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

#ifndef SENSORS_H
#define SENSORS_H

/*Includes------------------------------------------------------------*/
// #include <stdint.h>
#include <functional> //for std::reference_wrapper
#include <vector>
#include "HAL/time.h"

#include "buzzer.h"
#include "sensors-interface.h"
#include "hw-interface.h"
#include "statemachine.h"
#include "state_interface.h"
#include "options.h"

#include "CSVwrite.h"

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/
enum class Status {
    NOMINAL,
    NONCRITICAL_FAILURE,
    CRITICAL_FAILURE
};

/*Functions------------------------------------------------------------*/

void initSensors(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IParachute> > &hardware, IBuzzer &buzzer);

void displayStatus(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IParachute> > &hardware, IBuzzer &buzzer);

Status getStatus(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IParachute> > &hardware);

// TODO: Remove dependency of sensors.h for GP20U7.cpp/.h

void pollSensors(Hal::t_point &timestamp, std::vector<std::reference_wrapper<ISensor> > &sensors);

void logData(unsigned long timestamp, std::vector<std::reference_wrapper<ISensor> > &sensors,                    StateId state, float altitude, float baseline_pressure);

#endif
