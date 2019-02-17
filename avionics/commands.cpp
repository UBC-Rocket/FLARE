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


void doCommand(char command, FlightStates * state){
    switch (command){
        case ARM:
        //switchState(*state, ARMED);
        case CAMERAS_ON:
        //turn on the cameras
        case CAMERAS_OFF:
        //turn off the cameras
        case HALO:
        //play HALO
        case SATCOM:
        //switch to SATCOM
        case RESET:
        //not sure
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

