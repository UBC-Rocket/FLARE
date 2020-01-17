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
#include "HAL/time.h"
#include "HAL/gpio.h"

#include "sensors.h"
#include "options.h"
#include "buzzer.h" //for buzzer response on startup
#include "radio.h"
#include "state_interface.h"

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/
// CSVWrite datalog; //MOVED TO ENV_CONFIG

/*Functions------------------------------------------------------------*/
/**
  * @brief  Initializes all the sensors
  * @param  sensors The sensors to initialize
  * @return void
  */

void initSensors(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IParachute> > &hardware, Buzzer &buzzer) {
    // if(powerbattery.getVoltage() <= LOW_BATTERY_VOLTAGE)
    // { //TODO: Uncomment once the battery sensor is implemented
    //     status->sensorNominal[BATTERY_STATUS_POSITION] = false;
    //     if(powerbattery.getVoltage() <= MINIMUM_BATTERY_VOLTAGE){
    //         status->overview = CRITICAL_FAILURE;
    //         #ifdef TESTING
    //         SerialUSB.println("DANGER: BATTERY AT UNACCEPTABLY LOW VOLTAGE!");
    //         #endif
    //     }
    //     else{
    //         if(status->overview < NONCRITICAL_FAILURE){
    //             status->overview = NONCRITICAL_FAILURE;
    //         }
    //         #ifdef TESTING
    //         SerialUSB.println("WARNING: Battery at low voltage!");
    //         #endif
    //     }
    // }


    /*init hardware*/
    for (auto hw : hardware) {
        hw.get().init();
    }

    /*init sensors*/
    for (auto sensor : sensors) {
        sensor.get().initSensor();
    }

    // TODO: figure out what to do with satcom
    #ifdef NOSECONE
    /*init satcom*/
    if (!SatComSetup()) {
        if (status->overview < NONCRITICAL_FAILURE)
            status->overview = NONCRITICAL_FAILURE;
        status->sensorNominal[SATCOM_STATUS_POSITION] = false;
    }
    #endif // NOSECONE

    /* transmit sensor report */
    displayStatus(sensors, hardware, buzzer);
}

/**
 * @brief  Reports status with buzzer tune and LED.
 * @param  InitStatus *status - status of initialization.
 * @return void
 */
void displayStatus(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IParachute> > &hardware, Buzzer buzzer) {
    // TODO: change this function to discern which sensors constitute a critical fail
    /*
    if (status->overview == CRITICAL_FAILURE) {
        #ifdef TESTING
            SerialUSB.println("Critical failure! >:-{");
        #endif

        #ifdef TESTING //Only plays song once if testing, because it's annoying
        for(int i = 1; i <= 1; i++) {
        #else
        for(int i = 1; i <= 5; i++) {
        #endif
            sing(SongTypes_CRITICALFAIL);
            delay(400);
        }
    }
    else if (status->overview == NONCRITICAL_FAILURE) {
        #ifdef TESTING
            SerialUSB.println("Noncritical failure! :(");
        #endif
        pinMode(LED_BUILTIN, OUTPUT);

        #ifdef TESTING
        for(int i = 1; i <= 1; i++) {
        #else
        for(int i = 1; i <= 5; i++) {
        #endif
            sing(SongTypes_NONCRITFAIL);
            delay(400);
        }
    }
    else {
        #ifdef TESTING
            SerialUSB.println("Initialization complete! :D");
        #endif
        pinMode(LED_BUILTIN,OUTPUT);
        digitalWrite(LED_BUILTIN,HIGH);

        #ifdef TESTING
        for(int i = 1; i <= 1; i++) {
        #else
        for(int i = 1; i <= 5; i++) {
        #endif
            sing(SongTypes_SUCCESS);
            delay(400);
        }
    }

    return;
    */
    for (auto sensor : sensors) {
        if (sensor.get().getStatus() == SensorStatus::FAILURE) {
            #ifdef TESTING
            SerialUSB.println("Critical failure in one of a Sensor! >:-{");
            #endif

            #ifdef TESTING //Only plays song once if testing, because it's annoying
            for(int i = 1; i <= 1; i++) {
            #else
            for(int i = 1; i <= 5; i++) {
            #endif
                buzzer.sing(SongTypes_CRITICALFAIL);
                Hal::sleep_ms(400);
            }
            return;
        }
    }

    #ifdef TESTING
    SerialUSB.println("Initialization complete! :D");
    #endif
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);

    #ifdef TESTING
    for(int i = 1; i <= 1; i++) {
    #else
    for(int i = 1; i <= 5; i++) {
    #endif
        buzzer.sing(SongTypes_SUCCESS);
        Hal::sleep_ms(400);
    }
}

Status getStatus(std::vector<std::reference_wrapper<ISensor> > &sensors, std::vector<std::reference_wrapper<IParachute> > &hardware) {
    Status status = Status::NOMINAL;
    for (auto sensor : sensors) {
        if (sensor.get().getStatus() == SensorStatus::FAILURE) {
            status = Status::CRITICAL_FAILURE;
            return status;
        }
    }

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
void pollSensors(unsigned long *timestamp, std::vector<std::reference_wrapper<ISensor> > &sensors) {
    *timestamp = millis();

    for (auto sensor : sensors) {
        sensor.get().readData();
    }

    // *battery_voltage = powerbattery.getVoltage();
}

