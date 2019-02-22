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
#include <Arduino.h>
#include "sensors.h"


void doCommand(char command, FlightStates *state, InitStatus *status){
    switch (command){
        case ARM:
            //if(*state == STANDBY) //Don't want to switch out of drogue deploy or something into Armed
            //  switchState(*state, ARMED);
            break;

        case CAMERAS_ON:
            //turn on the cameras
            break;

        case CAMERAS_OFF:
            //turn off the cameras
            break;

        case HALO:
            //play HALO
            break;

        case SATCOM:
            //switch to SATCOM
            break;

        case RESET:
            //not sure
            break;

        case MAIN:
            #ifdef GROUND_TEST  // Ground radio testing purposes
            digitalWrite(LED_BUILTIN, HIGH);
            deployMain();
            #endif
            break;

        case DROGUE:
            #ifdef GROUND_TEST
            //testing purposes!
            digitalWrite(LED_BUILTIN,LOW);
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

        default:
            #ifdef TESTING
            SerialUSB.println("ERROR: COMMAND NOT RECOGNIZED");
            #endif
            break;
    }

}

void sendRadioResponse(const char* response){
    //teensy should be little endian, which means least significant is stored first, make sure ground station decodes accordingly

    for(int i=0; i<5; i++)
    {
        SerialRadio.write(response[i]);
    }
}

