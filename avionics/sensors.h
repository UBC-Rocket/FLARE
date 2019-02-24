/*Sensors Header*/
#ifndef SENSORS_H
#define SENSORS_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

//#include <string.h>

#include "statemachine.h"


/*Constants------------------------------------------------------------*/
//#define TESTING //enable or disable debug output
#define GROUND_TEST

#define SerialUSB               Serial
#define SerialGPS               Serial1
#define SerialRadio             Serial2

#define EARTHS_GRAVITY          9.80665

#define ACCELEROMETER_ADDRESS   0x18
#define TEMP_SENSOR_ADDRESS     0x48
#define IMU_ADDRESS             0x37
#define ACCELEROMETER_SCALE     6
#define BATTERY_SENSOR_PIN      9

#define ACC_DATA_ARRAY_SIZE     3
#define BAR_DATA_ARRAY_SIZE     2
#define IMU_DATA_ARRAY_SIZE     3
#define GPS_DATA_ARRAY_SIZE     3
#define GPS_FIELD_LENGTH        20

#define RADIO_DATA_ARRAY_SIZE   5

/*Variables------------------------------------------------------------*/

/*GPS initialization commands*/
const uint8_t GPS_reset_defaults[] = {0xA0, 0xA1, 0x00, 0x02, 0x04, 0x00, 0x04, 0x0D, 0x0A};
const uint8_t GPS_set_baud_rate[] = {0xA0, 0xA1, 0x00, 0x04, 0x05, 0x00, 0x00, 0x00, 0x05, 0x0D, 0x0A}; //4800
const uint8_t GPS_set_NMEA_message[] = {0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0D, 0x0A}; //GPGGA
const uint8_t GPS_set_update_rate[] = {0xA0, 0xA1, 0x00, 0x03, 0x0E, 0x01, 0x00, 0x0F, 0x0D, 0x0A}; //1 Hz



/*Functions------------------------------------------------------------*/
bool initSensors(void);
float barSensorInit(void);
void pollSensors(unsigned long*, float*, float*, float*, float*, float*, float*);
void logData(unsigned long *, float*, float*, float*, float*, float*, float*, FlightStates, float, float);

#endif
