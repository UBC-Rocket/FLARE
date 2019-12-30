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

#include "sensors-interface.h"
#include "hw-interface.h"
#include "statemachine.h"
#include "state_interface.h"
#include "options.h"

#include "sensors/accelerometer.h"
#include "sensors/barometer.h"
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/temperature.h"
#include "sensors/thermocouple.h"

#include "CSVwrite.h"

/*Constants------------------------------------------------------------*/
//TODO: put these somehwere
#define SerialUSB               Serial
#define SerialGPS               Serial1
#define SerialRadio             Serial2
#define IridiumSerial           Serial4
#define SPIThermo               SPI2

/*Variables------------------------------------------------------------*/
enum class Status {
    NOMINAL,
    NONCRITICAL_FAILURE,
    CRITICAL_FAILURE
};

/*Functions------------------------------------------------------------*/

void initSensors(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IHardware> > &hardware);

void displayStatus(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IHardware> > &hardware);

Status getStatus(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IHardware> > &hardware);

// TODO: Remove dependency of sensors.h for MAX31855k.cpp/.h and GP20U7.cpp/.h

void pollSensors(unsigned long *timestamp, std::vector<std::reference_wrapper<ISensor> > &sensors);

void logData(unsigned long timestamp, std::vector<std::reference_wrapper<ISensor> > &sensors,                    StateId state, float altitude, float baseline_pressure);

#endif
