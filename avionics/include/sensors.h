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
#include <stdint.h>
#include <map>

#include "sensors-interface.h"
#include "statemachine.h"
#include "Adafruit_BNO055.h"
#include "options.h"

/*Constants------------------------------------------------------------*/
#define SerialUSB               Serial
#define SerialGPS               Serial1
#define SerialRadio             Serial2
#define IridiumSerial           Serial4
#define SPIThermo               SPI2

/*Variables------------------------------------------------------------*/


/*Functions------------------------------------------------------------*/

void initSensors(std::map<ISensor, SensorStatus, float*> sensors);

void displayStatus(std::map<ISensor, SensorStatus, float*> sensors);

// TODO: Remove dependency of sensors.h for MAX31855k.cpp/.h and GP20U7.cpp/.h

void pollSensors(unsigned long *timestamp,
        std::map<ISensor, SensorStatus, float*> sensors);

void logData(unsigned long *timestamp,
    std::map<ISensor, SensorStatus, float*> sensors, FlightStates state,
    float altitude, float baseline_pressure);

#endif
