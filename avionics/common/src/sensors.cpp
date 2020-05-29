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

// void initSensors(std::vector<std::reference_wrapper<ISensor>> &sensors,
//                  std::vector<std::reference_wrapper<IParachute>> &hardware,
//                  IBuzzer &buzzer) {
//     // if(powerbattery.getVoltage() <= LOW_BATTERY_VOLTAGE)
//     // { //TODO: Uncomment once the battery sensor is implemented
//     //     status->sensorNominal[BATTERY_STATUS_POSITION] = false;
//     //     if(powerbattery.getVoltage() <= MINIMUM_BATTERY_VOLTAGE){
//     //         status->overview = CRITICAL_FAILURE;
//     //         #ifdef TESTING
//     //         SerialUSB.println("DANGER: BATTERY AT UNACCEPTABLY LOW
//     //         VOLTAGE!"); #endif
//     //     }
//     //     else{
//     //         if(status->overview < NONCRITICAL_FAILURE){
//     //             status->overview = NONCRITICAL_FAILURE;
//     //         }
//     //         #ifdef TESTING
//     //         SerialUSB.println("WARNING: Battery at low voltage!");
//     //         #endif
//     //     }
//     // }

//     /*init hardware*/
//     for (auto hw : hardware) {
//         hw.get().init();
//     }
//
//     /*init sensors*/
//     for (auto sensor : sensors) {
//         sensor.get().initSensor();
//     }

//     /* transmit sensor report */
//     displayStatus(sensors, hardware, buzzer);
// }

void raiseToStatus(Status &currentStatus, Status incomingStatus) {
    if (incomingStatus > currentStatus) {
        currentStatus = incomingStatus;
    }
}

/**
 * @brief  Reports status with buzzer tune and LED.
 * @param  InitStatus *status - status of initialization.
 * @return void
 */
void displayStatus(SensorCollection &sensors,
                   std::vector<std::reference_wrapper<IParachute>> &hardware,
                   IBuzzer &buzzer) {
    // TODO: Make response dependent on e-match success
    SongTypes song;
    switch (getStatus()) {
    case Status::NOMINAL:
        song = SongTypes_SUCCESS;
        break;
    case Status::NON_CRITICAL_FAILURE:
        song = SongTypes_NONCRITFAIL;
        break;
    case Status::CRITICAL_FAILURE:
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
        buzzer.sing(SongTypes_CRITICALFAIL);
        Hal::sleep_ms(400);
    }
    return;
}

// TODO - ????
// #ifdef TESTING
// SerialUSB.println("Initialization complete! :D");
// #endif
// Hal::pinMode(Hal::LED_BUILTIN(), Hal::PinMode::OUTPUT);
// Hal::digitalWrite(Hal::LED_BUILTIN(), Hal::PinDigital::HIGH);

// #ifdef TESTING
// for (int i = 1; i <= 1; i++) {
// #else
// for (int i = 1; i <= 5; i++) {
// #endif
//     buzzer.sing(SongTypes_SUCCESS);
//     Hal::sleep_ms(400);
// }
// }

Status getStatus(SensorCollection &sensors,
                 std::vector<std::reference_wrapper<IParachute>> &hardware) {
    Status status = sensors.getStatus();

    for (auto hw : hardware) {
        if (hw.get().getStatus() == HardwareStatus::FAILURE) {
            status = Status::CRITICAL_FAILURE;
            return status;
        }
    }
    return status;
}

/**
 * @brief Polls all the sensors
 * @param timestamp pointer to store the timestamp value
 * @param sensors the sensors to poll
 */
// void pollSensors(Hal::t_point &timestamp,
//                  std::vector<std::reference_wrapper<ISensor>> &sensors) {
//     timestamp = Hal::now_ms();

//     for (auto sensor : sensors) {
//         sensor.get().readData();
//     }

//     // *battery_voltage = powerbattery.getVoltage();
// }
