/*Radio Header*/
#ifndef __RADIO_H__
#define __RADIO_H__

/*Includes------------------------------------------------------------*/
#include <Arduino.h>
#include <stdint.h>

#include "statemachine.h"
#include "sensors.h"
#include "options.h"
#include "XBee.h"

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
const char UID_message = '"';

/* commands */
#define ARM 'r'
#define CAMERAS_ON 'C'
#define CAMERAS_OFF 'O'
#define RESET 'R'
#define PING 'p'
#define MAIN 'm'
#define DROGUE 'd'
#define STATUS 'S'
#define STARTUP_BUZZER 'B'
#define RECOVERY_BUZZER 'b'
#define DO_NOTHING '\0'

/* radio addressing */
#define GROUND_STATION_ADDR_MSB 0x0013A200 //Ground Station - Body
#define GROUND_STATION_ADDR_LSB 0x41678FC0

//Don't use the nose - it has a glued on capacitor which is more sketchy than
//the body
// #define GROUND_STATION_ADDR_MSB 0x0013A200 //Ground Station - Nose
// #define GROUND_STATION_ADDR_LSB 0x41678FB9


/* status bit flags */
#define BAROMETER_BIT_FLAG 0x02
#define IMU_BIT_FLAG 0x04
#ifdef BODY
    #define EMATCH_0_BIT_FLAG 0x08
    #define EMATCH_1_BIT_FLAG 0x10
#endif
#ifdef NOSECONE
    #define THERMOCOUPLE_BIT_FLAG 0x08
    #define SATCOM_BIT_FLAG 0x10
#endif
#define FILE_BIT_FLAG 0x20

/*Functions------------------------------------------------------------*/
void sendRadioBody(XBee* radio, ZBTxRequest* txPacket, float*, FlightStates state, float altitude, uint32_t*);
void sendRadioNosecone(XBee* radio, ZBTxRequest* txPacket, float* GPS_data,
                     float bar_data[], float acc_data[], float *temp_sensor_data, float IMU_data[]);

void resolveRadioRx(XBee* radio, ZBTxRequest* txPacket, FlightStates *state, InitStatus *status);
void radioStatus(XBee* radio, ZBTxRequest* txPacket, InitStatus *status);
void doCommand(char command, FlightStates *state, InitStatus *status, XBee* radio, ZBTxRequest* txPacket);
void sendMessage(XBee* radio, ZBTxRequest* txPacket, String* msg);
#endif
