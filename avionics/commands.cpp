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


void communicateThroughSerial(HardwareSerial SerialVar, FlightStates * state)
{
    char command[RADIO_DATA_ARRAY_SIZE];
    char recognitionRadio[RADIO_DATA_ARRAY_SIZE];
    char goodResponse[] = {'G','x','x','x','x'};
    const char badResponse[] = {'B','B','B','B','B'};


        #ifdef TESTING
        SerialUSB.print("Received Message: ");
        #endif
        while (SerialVar.available()){
            for(int i = 0; i< RADIO_DATA_ARRAY_SIZE; i++){
                command[i] = SerialVar.read();
            }
            bool correctCommand = checkForValidty(command);

            if(correctCommand){
                // radiolog.print(goodResponse);
                for(int i =1; i<5; i++)
                {
                    goodResponse[i] = command[0];
                }
                #ifdef TESTING

                SerialUSB.println(command);
                doCommand(command[0], state);

                #endif

                sendRadioResponse(goodResponse);
                SerialUSB.println(goodResponse);
            }
            else{
                //radiolog.print(badResponse);
                SerialUSB.println(command);
                SerialUSB.println(goodResponse);
                sendRadioResponse(badResponse);
            }
        }
}
/** void doCommand(char, FlightStates *state, InitStatus *status)
  * @brief  Takes a radio command input and executes the command
  * @param  char command - radio command input
  * @param  FlightStates *state - current flight state
  * @param  InitStatus *status - Sensor status structure
  * @return void
  */

void doCommand(char command, FlightStates * state){
    const char satComResponse[] = {'O','K','A','Y','!'};
    switch (command){
        case ARM:
            //if(*state == STANDBY) //Don't want to switch out of drogue deploy or something into Armed
            //  switchState(*state, ARMED);
            break;

        case CAMERAS_ON:
            //turn on the cameras
            break;

        //turn on the cameras
        case CAMERAS_OFF:
            //turn off the cameras
            break;

        case HALO:
            //play HALO
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
           // generateStatusReport(status, statusReport1, statusReport2, statusReport3); ASK
            sendRadioResponse(statusReport1);
            sendRadioResponse(statusReport2);
            sendRadioResponse(statusReport3);
            break;

        case SATCOM_TEST:
            sendSatComResponse(satComResponse);
            break;

        default:
            #ifdef TESTING
            SerialUSB.println("ERROR: COMMAND NOT RECOGNIZED");
            #endif
            break;

    }

}

/**
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

/**
  * @brief  Takes a 5 byte char and sequentially sends it over the satcom
  * @param  const char* response - 5 bytes worth of data to be sent via satcom
  * @return void
  */
void sendSatComResponse(const char* response){
     for(int i=0; i<5; i++)
      {
          SerialSatCom.write(response[i]);
      }

}

//checks if all indexes are equal for radio commands
bool checkForValidty(char *radioCommand)
 {
    const char a0 = radioCommand[0];

    for (int i = 1; i < 5; i++)
    {
        if (radioCommand[i] != a0)
            return false;
    }
    return true;
}

