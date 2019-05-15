/* @file    commands.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  Instructs the system to respond to the relevant
 *  radio commands.  Also sends a response through the radio.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#include "commands.h"
#include "gpio.h"
#include "sensors.h"
#include "cameras.h"
#include "satcom.h"
#include "statemachine.h"
#include "buzzer.h"

#include <Arduino.h>

/** void doCommand(char, FlightStates *state, InitStatus *status)
  * @brief  Takes a radio command input and executes the command
  * @param  char command - radio command input
  * @param  FlightStates *state - current flight state
  * @param  InitStatus *status - Sensor status structure
  * @return void
  */
void doCommand(char command, FlightStates *state, InitStatus *status){
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
            digitalWrite(FLIGHT_LED,LOW);
            deployDrogue();
            #endif

        case STATUS:
            char statusReport1[RADIO_DATA_ARRAY_SIZE];
            char statusReport2[RADIO_DATA_ARRAY_SIZE];
            char statusReport3[RADIO_DATA_ARRAY_SIZE];
            generateStatusReport(status, statusReport1, statusReport2, statusReport3);
            sendRadioResponse(statusReport1);
            sendRadioResponse(statusReport2);
            sendRadioResponse(statusReport3);
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

/** void sendRadioResponse(const char*)
  * @brief  Takes a 5 byte char and sequentially sends it over the radio
  * @param  const char* response - 5 bytes worth of data to be sent via radio
  * @return void
  */
void sendRadioResponse(const char* response){
    //teensy should be little endian, which means least significant is stored first, make sure ground station decodes accordingly

    for(int i=0; i<5; i++)
    {
        SerialRadio.write(response[i]);
    }
}

/** void communicateThroughSerial(FlightStates *state, InitStatus *status)
  * @brief  Receives radio commands and evaluates their validity
  * @param  FlightStates *state - current flight state
  * @param  InitStatus *status - Sensor status structure
  * @return void
  */
void communicateThroughSerial(FlightStates * state, InitStatus *status)
{
    char command[RADIO_DATA_ARRAY_SIZE];
    char goodResponse[] = {'G','x','x','x','x'};
    const char badResponse[] = {'B','B','B','B','B'};

    for(int i = 0; i< RADIO_DATA_ARRAY_SIZE; i++){
           command[i] = SerialRadio.read();
    }

    bool correctCommand = checkCommand(command);

    #ifdef TESTING
    SerialUSB.print("Received Message: ");
    SerialUSB.println(command);
    #endif

    if(correctCommand){
        #ifdef TESTING
        SerialUSB.print("Good Command: ");
        SerialUSB.write(command[0]);
        SerialUSB.println();
        #endif
        //create the correct good response
        for(int i =1; i<5; i++){
            goodResponse[i] = command[1];
        }

        sendRadioResponse(goodResponse);
        doCommand(command[0], state, status);
    }

    else {
        #ifdef TESTING
        SerialUSB.print("Bad Command: ");
        SerialUSB.write(command[0]);
        SerialUSB.println();
        #endif

        sendRadioResponse(badResponse);
    }
}

/** bool checkCommand(char*)
  * @brief  Takes a 5 byte char and checks that all elements are the same
  * @param  char* radioCommand - 5 bytes worth of received radio data
  * @return bool - true if the 5 bytes are the same
  *                 false if the 5 bytes are not all the same
  */
//checks if all indexes are equal for radio commands
bool checkCommand(char *radioCommand)
 {
    const char a0 = radioCommand[0];

    for (int i = 1; i < 5; i++)
    {
        if (radioCommand[i] != a0)
            return false;
    }
    return true;
}
