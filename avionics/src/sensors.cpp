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
#include "sensors.h"
#include "options.h"
#include "battery.h"
#include "satcom.h"                 //SATCOM
#include "buzzer.h"                 //for buzzer response on startup
#include "cameras.h"
#include "radio.h"
#include "statemachine.h"

#include <Arduino.h>

/*Constants------------------------------------------------------------*/
#define FILE_NAME "datalog.csv"

/*Variables------------------------------------------------------------*/
CSVWrite datalog;

/*Functions------------------------------------------------------------*/
/**
  * @brief  Initializes all the sensors
  * @param  sensors The sensors to initialize
  * @return void
  */

void initSensors(std::vector<ISensor> sensors, std::vector<IHardware> hardware)
{
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

    /*init SD card*/
    datalog.init(FILE_NAME);

    /* Init Cameras */
    SerialCamera.begin(CameraBaud);
    while (!SerialCamera) {}
    delay(2000);
    stop_record();

    /*init hardware*/
    for(IHardware &hw : hardware) {
        hw.init();
    }

    /*init sensors*/
    for(ISensor &sensor : sensors) {
        sensor.initSensor();
    }

    // TODO: figure out what to do with satcom
    #ifdef NOSECONE
        /*init satcom*/
        if (!SatComSetup()){
            if(status->overview < NONCRITICAL_FAILURE)
                status->overview = NONCRITICAL_FAILURE;
            status->sensorNominal[SATCOM_STATUS_POSITION] = false;
        }
    #endif // NOSECONE
    
    /* transmit sensor report */
    displayStatus(sensors, hardware);
}

/**
 * @brief  Reports status with buzzer tune and LED.
 * @param  InitStatus *status - status of initialization.
 * @return void
 */
void displayStatus(std::vector<ISensor> sensors,
    std::vector<IHardware> hardware) {
    //TODO: change this function to discern which sensors constitute a critical fail
    if (status->overview == CRITICAL_FAILURE) {
        #ifdef TESTING
            SerialUSB.println("Critical failure! >:-{");
        #endif

        #ifdef TESTING //Only plays song once if testing, because it's annoying
        for(int i = 1; i <= 1; i++){
        #else
        for(int i = 1; i <= 5; i++){
        #endif
            sing(SongTypes_CRITICALFAIL);
            delay(400);
        }
    }
    else if (status->overview == NONCRITICAL_FAILURE){
        #ifdef TESTING
            SerialUSB.println("Noncritical failure! :(");
        #endif
        pinMode(LED_BUILTIN, OUTPUT);

        #ifdef TESTING
        for(int i = 1; i <= 1; i++){
        #else
        for(int i = 1; i <= 5; i++){
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
        for(int i = 1; i <= 1; i++){
        #else
        for(int i = 1; i <= 5; i++){
        #endif
            sing(SongTypes_SUCCESS);
            delay(400);
        }
    }

    return;
}

/**
  * @brief Polls all the sensors
  * @param timestamp pointer to store the timestamp value
  * @param sensors the sensors to poll
  */
void pollSensors(unsigned long *timestamp, std::vector<ISensor> sensors) {
    *timestamp = millis();

    for(ISensor &sensor : sensors) {
        sensor.readData();
    }

    // *battery_voltage = powerbattery.getVoltage();
}

/**
  * @brief Logs data on the SD card
  * @param timestamp pointer to store the timestamp value
  * @param sensors the sensors to log data from
  * @param state rocket flight state
  * @param altitude Calculated rocket altitude, after filtering
  * @param baseline_pressure Pressure used as "ground level"
  */
void logData(unsigned long *timestamp, std::vector<ISensor> sensors,
        FlightStates state, float altitude, float baseline_pressure) {
    /*write data to SD card*/
    datalog.write(timestamp);
    for (ISensor &sensor : sensors) {
        float* data = sensor.getData;
        for (int i = 0; i < sensor.dataLength(); i++) {
            datalog.write(data[i]);
        }
    }
    datalog.writeln("");
    datalog.flush();
}
