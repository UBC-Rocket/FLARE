/*
 * Sensors Source
 *
 * @file    sensors.cpp
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

/*Includes------------------------------------------------------------*/
#include "HAL/pin_util.h"
#include "HAL/time.h"

#include "buzzer.h" //for buzzer response on startup
#include "ignitor_collection.h"
#include "options.h"
#include "radio.h"
#include "sensor_collection.h"
#include "status.h"

#undef HIGH // TODO - see if there's a better place to undef these codes from
#undef LOW  // Arduino
#undef OUTPUT
#undef LED_BUILTIN

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/
// CSVWrite datalog; //MOVED TO ENV_CONFIG

/*Functions------------------------------------------------------------*/
/**
 * @brief  Initializes all the sensors
 * @param  sensors The sensors to initialize
 * @return void
 */

void raiseToStatus(RocketStatus &currentStatus, RocketStatus incomingStatus) {
    if (incomingStatus > currentStatus) {
        currentStatus = incomingStatus;
    }
}

/**
 * @brief  Reports status with buzzer tune and LED.
 * @param SensorCollection &sensors - collection of sensors
 * @param &ignitor - collection of e-matches
 * @param &buzzer - buzzer
 * @return void
 */
void displayStatus(RocketStatus status, IBuzzer &buzzer) {
    // TODO: Make response dependent on e-match success
    SongTypes song;
    switch (status) {
    case RocketStatus::NOMINAL:
        song = SongTypes_SUCCESS;
        Hal::digitalWrite(Pin::STATUS_LED, Hal::PinDigital::VOLTAGE_HIGH);
        break;
    case RocketStatus::NONCRITICAL_FAILURE:
        song = SongTypes_NONCRITFAIL;
        // LED blinks - don't bother with specifing STATUS_LED
        break;
    case RocketStatus::CRITICAL_FAILURE:
        song = SongTypes_CRITICALFAIL;
        Hal::digitalWrite(Pin::STATUS_LED, Hal::PinDigital::VOLTAGE_LOW);
        break;
    default:
        // Not known - assume the worst
        song = SongTypes_CRITICALFAIL;
    }

#ifdef TESTING // Only plays song once if testing, because it's annoying
    for (int i = 1; i <= 1; i++) {
#else
    for (int i = 1; i <= 5; i++) {
#endif
        buzzer.sing(song);
        Hal::sleep_ms(400);
    }
    return;
}

/* clang-format off */ // Clang puts the function name on the next line :/
RocketStatus collectStatus(SensorCollection &sensors,
          IgnitorCollection &ignitors) {
    /* clang-format on */
    RocketStatus status = sensors.getStatus();
    raiseToStatus(status, ignitors.getStatus());
    return status;
}
