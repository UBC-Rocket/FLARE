/*Sensors Header*/
#ifndef SENSORS_H
#define SENSORS_H

/*Includes------------------------------------------------------------*/
#include "SparkFun_LIS331.h"        //accelerometer
#include "SparkFun_MS5803_I2C.h"    //barometer
#include "SparkFunTMP102.h"         //temp sensor
#include "MPU9250.h"                //IMU
#include "Venus638FLPx.h"           //GPS

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>

/*Constants------------------------------------------------------------*/
#define TESTING //enable or disable debug output

#define EARTHS_GRAVITY          9.80665

#define ACCELEROMETER_ADDRESS   0x18
#define TEMP_SENSOR_ADDRESS     0x48
#define IMU_ADDRESS             0x68
#define ACCELEROMETER_SCALE     6

#define ACC_DATA_ARRAY_SIZE     3
#define BAR_DATA_ARRAY_SIZE     2
#define IMU_DATA_ARRAY_SIZE     9
#define GPS_DATA_ARRAY_SIZE     3
#define GPS_SENTENCE_LENGTH     20

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
bool initSensors(void);
void pollSensors(float*, float*, float*, float*, char [][GPS_SENTENCE_LENGTH]);
void logData(float*, float*, float*, float*, char [][GPS_SENTENCE_LENGTH]);

#endif
