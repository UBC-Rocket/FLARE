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

#include "statemachine.h"
#include "Adafruit_BNO055.h"
#include "options.h"

/*Constants------------------------------------------------------------*/
#define SerialUSB               Serial
#define SerialGPS               Serial1
#define SerialRadio             Serial2
#define IridiumSerial           Serial4
#define SPIThermo               SPI2

#define ACC_DATA_ARRAY_SIZE     3
#define BAR_DATA_ARRAY_SIZE     2
#if defined NOSECONE
    #define IMU_DATA_ARRAY_SIZE     3
#elif defined BODY
    #define IMU_DATA_ARRAY_SIZE     9
#endif
#define GPS_DATA_ARRAY_SIZE     3
#define GPS_FIELD_LENGTH        20

#if defined THERMOCOUPLE
    #define NUM_SENSORS 9
#else
    #define NUM_SENSORS 8
#endif

#define FILE_STATUS_POSITION 0
#define BATTERY_STATUS_POSITION 1
#define ACCELEROMETER_STATUS_POSITION 2
#define BAROMETER_STATUS_POSITION 3
#define TEMPERATURE_STATUS_POSITION 4
#define IMU_STATUS_POSITION 5
#define EMATCH_STATUS_POSITION 6
#define SATCOM_STATUS_POSITION 7
#define THERMOCOUPLE_STATUS_POSITION 8
/*Variables------------------------------------------------------------*/

/*Error codes for initialization*/
enum OverallError {
    NOMINAL,
    NONCRITICAL_FAILURE,
    CRITICAL_FAILURE
};

struct InitStatus {
    OverallError overview;
    bool sensorNominal[NUM_SENSORS];
};

/*GPS initialization commands*/
const uint8_t GPS_reset_defaults[] =
    {0xA0, 0xA1, 0x00, 0x02, 0x04, 0x00, 0x04, 0x0D, 0x0A};
const uint8_t GPS_set_baud_rate[] =
    {0xA0, 0xA1, 0x00, 0x04, 0x05, 0x00, 0x00, 0x00, 0x05, 0x0D, 0x0A}; //4800
const uint8_t GPS_set_NMEA_message[] =
    {0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x09, 0x0D, 0x0A}; //GPGGA
const uint8_t GPS_set_update_rate[] =
    {0xA0, 0xA1, 0x00, 0x03, 0x0E, 0x01, 0x00, 0x0F, 0x0D, 0x0A}; //1 Hz


/*Functions------------------------------------------------------------*/
void initSensors(InitStatus* status);
float barSensorInit(void);

void displayStatus(InitStatus *status);
void pollSensors(unsigned long *timestamp, float *battery_voltage,
        float acc_data[], float bar_data[], float *temp_sensor_data,
        float IMU_data[], float GPS_data[], float *thermocouple_data);
void logData(unsigned long *timestamp, float *battery_voltage, float acc_data[],
        float bar_data[], float *temp_sensor_data, float IMU_data[],
        float GPS_data[], FlightStates state, float altitude,
        float baseline_pressure, float thermocouple_data);

#endif
