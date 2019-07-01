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
/* radio addressing */
#define GROUND_STATION_ADDR_MSB 0x0013A200 //Ground Station - Body
#define GROUND_STATION_ADDR_LSB 0x41678FC0

//Don't use the nose - it has a glued on capacitor which is more sketchy than
//the body
// #define GROUND_STATION_ADDR_MSB 0x0013A200 //Ground Station - Nose
// #define GROUND_STATION_ADDR_LSB 0x41678FB9

/*Functions------------------------------------------------------------*/
void sendRadioBody(XBee* radio, ZBTxRequest* txPacket, float*, FlightStates state, float *altitude, uint32_t*);
void sendRadioNosecone(XBee* radio, ZBTxRequest* txPacket, float* GPS_data,
                     float bar_data[], float acc_data[], float *temp_sensor_data, float IMU_data[]);

void resolveRadioRx(XBee* radio, ZBTxRequest* txPacket, float GPS_data[], FlightStates *state, InitStatus *status);
void radioStatus(XBee* radio, ZBTxRequest* txPacket, InitStatus *status);
void doCommand(char command, float GPS_data[], FlightStates *state, InitStatus *status, XBee* radio, ZBTxRequest* txPacket);
void sendMessage(XBee* radio, ZBTxRequest* txPacket, String* msg);
#endif
