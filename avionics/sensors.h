/*Sensors Header*/
#ifndef SENSORS_H
#define SENSORS_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

/*Constants------------------------------------------------------------*/
#define TESTING //enable or disable debug output

#define SerialUSB               Serial
#define SerialGPS               Serial1
#define SerialRadio             Serial2

#define EARTHS_GRAVITY          9.80665

#define ACCELEROMETER_ADDRESS   0x18
#define TEMP_SENSOR_ADDRESS     0x48
#define IMU_ADDRESS             0x37
#define ACCELEROMETER_SCALE     6

#define ACC_DATA_ARRAY_SIZE     3
#define BAR_DATA_ARRAY_SIZE     2
#define IMU_DATA_ARRAY_SIZE     9
#define GPS_DATA_ARRAY_SIZE     3
#define GPS_FIELD_LENGTH        20

/*Variables------------------------------------------------------------*/

/*GPS initialization commands*/
const uint8_t GPS_reset_defaults[] = {0xA0, 0xA1, 0x00, 0x02, 0x04, 0x00, 0x04, 0x0D, 0x0A};
const uint8_t GPS_set_baud_rate[] = {0xA0, 0xA1, 0x00, 0x04, 0x05, 0x00, 0x05, 0x00, 0x00, 0x0D, 0x0A}; //115200
const uint8_t GPS_set_NMEA_message[] = {0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0D, 0x0A}; //GPGGA
const uint8_t GPS_set_update_rate[] = {0xA0, 0xA1, 0x00, 0x03, 0x0E, 0x01, 0x00, 0x0F, 0x0D, 0x0A}; //1 Hz

/*radio data unique identifiers*/
const char UID_acc_acc_x  = 'X'; //Accelerometer - Acceleration X
const char UID_acc_acc_y  = 'Y'; //Accelerometer - Acceleration Y
const char UID_acc_acc_z  = 'Z'; //Accelerometer - Acceleration Z
const char UID_bar_pres  = 'P'; //Barometer - Pressure
const char UID_bar_temp  = '~'; //Barometer - Temperature
const char UID_temp_temp  = 'T'; //Temperature Sensor - Temperature
const char UID_IMU_acc_x  = 'x'; //IMU - Acceleration X
const char UID_IMU_acc_y = 'y'; //IMU - Acceleration Y
const char UID_IMU_acc_z  = 'z'; //IMU - Acceleration Z
const char UID_IMU_gyro_x  = '@'; //IMU - Angular Velocity X
const char UID_IMU_gyro_y  = '#'; //IMU - Angular Velocity Y
const char UID_IMU_gyro_z  = '$'; //IMU - Angular Velocity Z
const char UID_IMU_mag_x  = '%'; //IMU - Magnetism X
const char UID_IMU_mag_y  = '^'; //IMU - Magnetism Y
const char UID_IMU_mag_z  = '&'; //IMU - Magnetism Z
const char UID_GPS_lat  = 'L'; //GPS - Latitude
const char UID_GPS_long  = 'l'; //GPS - Longitude
const char UID_GPS_alt  = 'A'; //GPS - Altitude
const char UID_time  = 't'; //Time

/*Functions------------------------------------------------------------*/
bool initSensors(void);
float barSensorInit(void);
void pollSensors(unsigned long*, float*, float*, float*, float*, char[][GPS_FIELD_LENGTH]);
void logData(unsigned long*, float*, float*, float*, float*, char[][GPS_FIELD_LENGTH]);

#endif
