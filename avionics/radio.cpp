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

#include "sensors.h"
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

//send pressure, gps, state, and altitude over radio
//Functions for testing purposes
// void sendTierOne(unsigned long *timestamp, float *GPS_data, float bar_data[], FlightStates state, float altitude){
//     float time = *timestamp;
//     sendRadioData(time, 't');
//     sendRadioData(bar_data[0], UID_bar_pres);
//     sendRadioData(GPS_data[0], UID_GPS_lat);
//     sendRadioData(GPS_data[1], UID_GPS_long);
//     sendRadioData(GPS_data[2], UID_GPS_alt);
//     sendRadioData( altitude, UID_altitude);
//     sendRadioData((float) state, UID_state);
// }

// //Send Acceleration, IMU data, and temperature
// //Functions for testing purposes
// void sendTierTwo(float acc_data[], float bar_data[], float *temp_sensor_data, float IMU_data[]){
//     sendRadioData(bar_data[1], UID_bar_temp);
//     sendRadioData(*temp_sensor_data, UID_temp_temp);
//     sendRadioData(IMU_data[0], UID_IMU_yaw);
//     sendRadioData(IMU_data[1], UID_IMU_roll);
//     sendRadioData(IMU_data[2], UID_IMU_pitch);
//     sendRadioData(acc_data[0], UID_acc_acc_x);
//     sendRadioData(acc_data[1], UID_acc_acc_y);
//     sendRadioData(acc_data[2], UID_acc_acc_z);
// }
// //Send data that only needs to be sent once, battery voltage, Ground Altitude
// //Functions for testing purposes
// void sendTierThree(float* battery_voltage, float* ground_altitude){
//     sendRadioData(*ground_altitude, UID_ground_altitude);
//     sendRadioData(*battery_voltage, UID_batt);
// }
/**
  * @brief  Send more essential data from the body over radio, pressure, state, altitude, timestamp
  * @param  float bar_data - baramoeter data array
  * @param  FlightStates state - current state
  * @param  float altitude - current calculated altitude
  * @param  unsigned long *timestamp - address for the timestamp
  * @return void
  */
void bodyTierOne(XBee* radio, ZBTxRequest* txPacket, float bar_data[], FlightStates state, float altitude, uint32_t *timestamp){

    static uint8_t payload[20];
    payload[0] = 0;
    memcpy(payload + 1, timestamp, sizeof(float));
    payload[5] = UID_bar_pres;
    memcpy(payload + 1 + 5, bar_data, sizeof(float));
    payload[10] = UID_altitude;
    memcpy(payload + 1 + 10, &altitude, sizeof(float));
    payload[15] = UID_state;
    memcpy(payload + 1 + 15, &state, sizeof(float));

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
void noseconeTierOne(XBee* radio, ZBTxRequest* txPacket, float* GPS_data,
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
    // sendRadioData(GPS_data[0], UID_GPS_lat);
    // sendRadioData(GPS_data[1], UID_GPS_long);
    // sendRadioData(GPS_data[2], UID_GPS_alt);
    // sendRadioData((float) state, UID_state);
    // sendRadioData( altitude, UID_altitude);

    /* Copied from tier2: */
    // sendRadioData(bar_data[1], UID_bar_temp);
    // sendRadioData(*temp_sensor_data, UID_temp_temp);
    // sendRadioData(IMU_data[0], UID_IMU_yaw);
    // sendRadioData(IMU_data[1], UID_IMU_roll);
    // sendRadioData(IMU_data[2], UID_IMU_pitch);
    // sendRadioData(acc_data[0], UID_acc_acc_x);
    // sendRadioData(acc_data[1], UID_acc_acc_y);
    // sendRadioData(acc_data[2], UID_acc_acc_z);
}
/**
  * @brief  Function to send the less essential data from the nosecone over radio,
  * barometer temperature, accelerometer, temperature, and IMU data
  * @param  float bar_data - barometer data array for temperature
  * @param  float acc_data - accelerometer data array
  * @param  float *temp_sensor_data - pointer for the temperature data
  * @param  float IMU_data - IMU data array
  * @return void
  */
// void noseconeTierTwo(float bar_data[], float acc_data[], float *temp_sensor_data, float IMU_data[]){
//     sendRadioData(bar_data[1], UID_bar_temp);
//     sendRadioData(*temp_sensor_data, UID_temp_temp);
//     sendRadioData(IMU_data[0], UID_IMU_yaw);
//     sendRadioData(IMU_data[1], UID_IMU_roll);
//     sendRadioData(IMU_data[2], UID_IMU_pitch);
//     sendRadioData(acc_data[0], UID_acc_acc_x);
//     sendRadioData(acc_data[1], UID_acc_acc_y);
//     sendRadioData(acc_data[2], UID_acc_acc_z);
// }

/**
  * @brief  Deals with all received messages from the radio driver, including commands and status.
  * @param  XBee radio
  * @param  char id - data identifier
  * @return void
  */
void resolveRadioRx(XBee* radio, ZBTxRequest* txPacket, FlightStates *state, InitStatus *status)
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
            doCommand(command, state, status, radio, txPacket);
        }
        // else if(radio.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {}
        // essentially an acknowledge of delivery status. may be useful later

        radio->readPacket();
    }
}

/** void doCommand(char, FlightStates *state, InitStatus *status)
  * @brief  Takes a radio command input and executes the command
  * @param  char command - radio command input
  * @param  FlightStates *state - current flight state
  * @param  InitStatus *status - Sensor status structure
  * @return void
  */
void doCommand(char command, FlightStates *state, InitStatus *status, XBee* radio, ZBTxRequest* txPacket){
    switch (command){
        case ARM:
            if(*state == STANDBY) { //Don't want to switch out of drogue deploy or something into Armed
                switchState(state, ARMED);
                digitalWrite(FLIGHT_LED, HIGH);
                // turn on cameras
                start_record();
            }

            break;

        case CAMERAS_ON:
            start_record();
            break;

        case CAMERAS_OFF:
            stop_record();
            break;

        case RESET:
            break;

        case MAIN:
            #ifdef GROUND_TEST  // Ground radio testing purposes
            digitalWrite(FLIGHT_LED, HIGH);
            deployMain();
            #endif
            break;

        case DROGUE:
            #ifdef GROUND_TEST
            //testing purposes!
            digitalWrite(FLIGHT_LED, LOW);
            deployDrogue();
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

        case DO_NOTHING:
            break;

        default:
            #ifdef TESTING
            SerialUSB.println("ERROR: COMMAND NOT RECOGNIZED");
            #endif
            break;
    }

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

// void sendRadioData(float data, char id){
//     //teensy should be little endian, which means least significant is stored first, make sure ground station decodes accordingly
//     u_int8_t b[4];
//     *(float*) b = data;
//     SerialRadio.write(id);
//     for(int i=0; i<4; i++)
//     {
//         SerialRadio.write(b[i]);
//     }
// }
