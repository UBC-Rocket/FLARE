/*Radio Header*/
#ifndef __RADIO_H__
#define __RADIO_H__

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include <stdint.h>

#include "statemachine.h"

/*Constants------------------------------------------------------------*/
/*radio data unique identifiers*/
const char UID_acc_acc_x  = 'X'; //Accelerometer - Acceleration X
const char UID_acc_acc_y  = 'Y'; //Accelerometer - Acceleration Y
const char UID_acc_acc_z  = 'Z'; //Accelerometer - Acceleration Z
const char UID_bar_pres  = 'P'; //Barometer - Pressure
const char UID_bar_temp  = '~'; //Barometer - Temperature
const char UID_temp_temp  = 'T'; //Temperature Sensor - Temperature
const char UID_IMU_yaw  = '@'; //IMU - Yaw
const char UID_IMU_roll  = '#'; //IMU - Roll
const char UID_IMU_pitch  = '$'; //IMU - Pitch
const char UID_GPS_lat  = 'L'; //GPS - Latitude
const char UID_GPS_long  = 'l'; //GPS - Longitude
const char UID_GPS_alt  = 'A'; //GPS - Altitude
const char UID_time  = 't'; //Time
const char UID_altitude = 'a'; //calculated altitude
const char UID_state = 's'; //state machine state
const char UID_batt = 'b';  //Battery voltage
const char UID_ground_altitude = 'g';//Ground Altitude
const char UID_status = 'S';  //Status

/*Functions------------------------------------------------------------*/
void sendRadioData(float data, char id);
void sendTierOne(unsigned long*, float*, float*, FlightStates state, float altitude);
void sendTierTwo(float*, float*, float*, float*);
void sendTierThree(float*, float*);
void bodyTierOne(float*, FlightStates state, float altitude, unsigned long*);
//void bodyTierTwo(float*); not needed
void noseconeTierOne(float*, unsigned long*, FlightStates state, float);
void noseconeTierTwo(float*, float*, float*, float*);

#endif