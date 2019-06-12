/* @file    radio.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Implements radio send functions and prioritizing
 * sensor data to send over the radio
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
#include "radio.h"

#include "options.h"
#include "battery.h"
#include "SparkFun_LIS331.h"        //accelerometer
#include "MS5803_01.h"              //barometer
#include "SparkFunTMP102.h"         //temp sensor
#include "Adafruit_BNO055.h"        //IMU
#include "GP20U7.h"           //GPS
#include "gpio.h"
#include "cameras.h"
#include "satcom.h"
#include "statemachine.h"
#include "buzzer.h"

#include <Arduino.h>
#include <HardwareSerial.h>

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
#define GPS_PING 'g'
#define DO_NOTHING '\0'

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

/**
  * @brief  Send more essential data from the body over radio, pressure, state, altitude, timestamp
  * @param  float bar_data - baramoeter data array
  * @param  FlightStates state - current state
  * @param  float altitude - current calculated altitude
  * @param  unsigned long *timestamp - address for the timestamp
  * @return void
  */
void sendRadioBody(XBee* radio, ZBTxRequest* txPacket, float bar_data[], FlightStates state, float altitude, uint32_t *timestamp){
    uint32_t stateAsInt = static_cast<uint32_t>(state);
    static uint8_t payload[20];
    payload[0] = UID_time;
    memcpy(payload + 1, timestamp, sizeof(float));
    payload[5] = UID_bar_pres;
    memcpy(payload + 1 + 5, bar_data, sizeof(float));
    payload[10] = UID_altitude;
    memcpy(payload + 1 + 10, &altitude, sizeof(float));
    payload[15] = UID_state;
    memcpy(payload + 1 + 15, &stateAsInt, sizeof(float));

    txPacket->setPayloadLength(20);
    txPacket->setPayload(payload);

    radio->send(*txPacket);
}
/**
  * @brief  Sends more essential data over radio from the nosecone, and GPS data
  * @param  float GPS_data - gps data array
  * @param  unsigned long *timestamp - address for the timestamp
  * @return void
  */
void sendRadioNosecone(XBee* radio, ZBTxRequest* txPacket, float* GPS_data,
                     float bar_data[], float acc_data[], float *temp_sensor_data, float IMU_data[]){

    static uint8_t payload[55];

    payload[0] = UID_GPS_lat;
    memcpy(payload + 1, GPS_data, sizeof(float));
    payload[5] = UID_GPS_long;
    memcpy(payload + 1 + 5, GPS_data + 1, sizeof(float));
    payload[10] = UID_GPS_alt;
    memcpy(payload + 1 + 10, GPS_data + 2, sizeof(float));
    payload[15] = UID_bar_temp;
    memcpy(payload + 1 + 15, bar_data + 1, sizeof(float));
    payload[20] = UID_acc_acc_x;
    memcpy(payload + 1 + 20, acc_data, sizeof(float));
    payload[25] = UID_acc_acc_y;
    memcpy(payload + 1 + 25, acc_data + 1, sizeof(float));
    payload[30] = UID_acc_acc_z;
    memcpy(payload + 1 + 30, acc_data + 2, sizeof(float));
    payload[35] = UID_temp_temp;
    memcpy(payload + 1 + 35, temp_sensor_data, sizeof(float));
    payload[40] = UID_IMU_yaw;
    memcpy(payload + 1 + 40, IMU_data, sizeof(float));
    payload[45] = UID_IMU_roll;
    memcpy(payload + 1 + 45, IMU_data + 1, sizeof(float));
    payload[50] = UID_IMU_pitch;
    memcpy(payload + 1 + 50, IMU_data + 2, sizeof(float));

    txPacket->setPayloadLength(55);
    txPacket->setPayload(payload);

    radio->send(*txPacket);
}

/**
  * @brief  Deals with all received messages from the radio driver, including commands and status.
  * @param  XBee radio
  * @param  char id - data identifier
  * @return void
  */
void resolveRadioRx(float GPS_data[], XBee* radio, ZBTxRequest* txPacket, FlightStates *state, InitStatus *status)
{
    static ZBRxResponse rx = ZBRxResponse();
    static char command = '\0';

    radio->readPacket();
    while(radio->getResponse().isAvailable() || radio->getResponse().isError()) //goes through all radio packets in buffer
    {
        #ifdef TESTING
            //SerialUSB.println("Radio received something")
        #endif

        if(radio->getResponse().isError()) //will we use this?
        {
            // #ifdef TESTING
            //     SerialUSB.println("Radio error");
            // #endif
        }
        else if(radio->getResponse().getApiId() == ZB_RX_RESPONSE) //received command from radio
        {
            radio->getResponse().getZBRxResponse(rx);
            command = *(rx.getData());
            doCommand(command, GPS_data, state, status, radio, txPacket);
        }
        // else if(radio.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {}
        // essentially an acknowledge of delivery status. may be useful later

        radio->readPacket();
    }
}

/** void doCommand
  * @brief  Takes a radio command input and executes the command
  * @return void
  */
void doCommand(char command, float GPS_data[], FlightStates *state, InitStatus *status, XBee* radio, ZBTxRequest* txPacket){
    unsigned char payload[2] = {'G'};
    payload[1] = command;
    String msg;
            String latPrefix;
            String latVal;
            String longPrefix;
            String longVal;
            String altPrefix;
            String altVal;
    switch (command){
        case ARM:
            if(*state == STANDBY) { //Don't want to switch out of drogue deploy or something into Armed
                switchState(state, ARMED);
                digitalWrite(FLIGHT_LED, HIGH);
                // turn on cameras
                start_record();
            }
            else if(*state == ARMED) {
                msg = String("Already in ARMED");
                sendMessage(radio, txPacket, &msg);
            }
            else{
                msg = String("!! ALREADY IN FLIGHT !!");
                sendMessage(radio, txPacket, &msg);
            }

            break;

        case CAMERAS_ON:
            start_record();
            break;

        case CAMERAS_OFF:
            stop_record();
            break;

        case RESET:
            if(*state == ARMED) { //Don't want to switch out of drogue deploy or something into standby
                switchState(state, STANDBY);
                digitalWrite(FLIGHT_LED, LOW);
                // turn off cameras
                stop_record();
            }
            else if(*state == STANDBY) {
                msg = String("Already in STANDBY");
                sendMessage(radio, txPacket, &msg);
            }
            else{
                msg = String("!! ALREADY IN FLIGHT !!");
                sendMessage(radio, txPacket, &msg);
            }
            break;

        case MAIN:
            #ifdef GROUND_TEST  // Ground radio testing purposes
                digitalWrite(FLIGHT_LED, HIGH);
                deployMain();
            #else
                msg = String("!! MAIN PARACHUTE COMMAND RECEIVED, NO ACTION TAKEN !!");
                sendMessage(radio, txPacket, &msg);
            #endif
            break;

        case DROGUE:
            #ifdef GROUND_TEST
            //testing purposes!
            digitalWrite(FLIGHT_LED, LOW);
            deployDrogue();
            #else
                msg = String("!! DROGUE PARACHUTE COMMAND RECEIVED, NO ACTION TAKEN !!");
                sendMessage(radio, txPacket, &msg);
            #endif

        case STATUS:
            radioStatus(radio, txPacket, status);
            break;

        case STARTUP_BUZZER:
            // add the buzzer command
            break;

        case RECOVERY_BUZZER:
            // add the recovery buzzer command
            break;

        case PING:
            msg = String("Please help I'm stuck on a rocket");
            sendMessage(radio, txPacket, &msg);
            break;

        case GPS_PING:
            #ifdef NOSECONE
                latPrefix = String("Lat:");
                latVal = String(GPS_data[0], 3);
                longPrefix = String("Lng:");
                longVal = String(GPS_data[1], 3);
                altPrefix = String("Alt:");
                altVal = String(GPS_data[2],3);
                msg = String(latPrefix + latVal + longPrefix + longVal + altPrefix + altVal);
                sendMessage(radio, txPacket, &msg);
            #endif
            break;

        case DO_NOTHING:
            break;

        default:
            #ifdef TESTING
            SerialUSB.println("ERROR: COMMAND NOT RECOGNIZED");
            #endif
            payload[0] = 'B';
            break;
    }

    txPacket->setPayload(payload);
    txPacket->setPayloadLength(2);
    radio->send(*txPacket);
}

/*
 * @brief  Sends a radio message containing status information
 * @param XBee *radio - the class for the xbee module in use
 * @param ZBTxRequest *txPacket - reuse this class to save memory
 * @param  InitStatus *status - status of initialization
 * @return void
 */
void radioStatus(XBee* radio, ZBTxRequest* txPacket, InitStatus *status)
{
    #ifdef NOSECONE
    static char statusByte = 0x00; //NOSECONE INDICATOR
    #else
    static char statusByte = 0x01; //BODY INDICATOR
    #endif
    static uint8_t payload[3] = {UID_status};

    payload[1] = static_cast<uint8_t>(status->overview);

    if( !(status->sensorNominal[BAROMETER_STATUS_POSITION]) )
        statusByte |= BAROMETER_BIT_FLAG;
    if( !(status->sensorNominal[IMU_STATUS_POSITION]) )
        statusByte |= IMU_BIT_FLAG;
    #ifdef BODY
        if( !(status->sensorNominal[EMATCH_STATUS_POSITION]) )
            statusByte |= EMATCH_0_BIT_FLAG;
        // if( !(status->sensorNominal[EMATCH_1_STATUS_POSITION]) ) //Resolve once 2nd ematch is implemented
        //     statusByte |= EMATCH_1_BIT_FLAG;
    #else
        if( !(status->sensorNominal[THERMOCOUPLE_STATUS_POSITION]) )
            statusByte |= THERMOCOUPLE_BIT_FLAG;
        if( !(status->sensorNominal[SATCOM_STATUS_POSITION]) )
            statusByte |= SATCOM_BIT_FLAG;
    #endif
    if( !(status->sensorNominal[FILE_STATUS_POSITION]) )
        statusByte |= FILE_BIT_FLAG;


    txPacket->setPayload(payload);
    txPacket->setPayloadLength(3);

    radio->send(*txPacket);
}

/*
 * @brief  Sends a string message across radio
 * @param XBee *radio - the class for the xbee module in use
 * @param ZBTxRequest *txPacket - reuse this class to save memory
 * @param  String *msg - Arduino String containing the string to use
 * @return void
 */
void sendMessage(XBee* radio, ZBTxRequest* txPacket, String* msg)
{
    String payload = String(UID_message);
    payload.concat(*msg);
    txPacket->setPayloadLength(payload.length()+1);
    txPacket->setPayload( reinterpret_cast<unsigned char*>( const_cast<char*>( payload.c_str() ) ) );
    radio->send(*txPacket);
}
