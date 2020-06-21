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
#include "HAL/gpio.h"
#include "HAL/time.h"

#include "buzzer.h" //for buzzer response on startup
#include "options.h"
#include "radio.h"
#include "state_interface.h"
#include "status.h"

#undef HIGH // TODO - see if there's a better place to undef these codes from
            // Arduino
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
void displayStatus(SensorCollection &sensors,
                   std::vector<std::reference_wrapper<IIgnitor>> &ignitor,
                   IBuzzer &buzzer) {
    // TODO: Make response dependent on e-match success
    SongTypes song;
    switch (sensors.getStatus()) {
    case RocketStatus::NOMINAL:
        song = SongTypes_SUCCESS;
        break;
    case RocketStatus::NONCRITICAL_FAILURE:
        song = SongTypes_NONCRITFAIL;
        break;
    case RocketStatus::CRITICAL_FAILURE:
        song = SongTypes_CRITICALFAIL;
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
    Hal::pinMode(Hal::LED_BUILTIN(), Hal::PinMode::OUTPUT);
    Hal::digitalWrite(Hal::LED_BUILTIN(), Hal::PinDigital::HIGH);

    return;
}

RocketStatus
getStatus(SensorCollection &sensors,
          std::vector<std::reference_wrapper<IIgnitor>> &hardware) {
    RocketStatus status = sensors.getStatus();

    for (auto hw : hardware) {
        if (hw.get().getStatus() == HardwareStatus::FAILURE) {
            status = RocketStatus::CRITICAL_FAILURE;
            return status;
        }
    }
    return status;
}
