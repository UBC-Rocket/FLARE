#include "commands.h"
#include <Arduino.h>


void doCommand(char command, FlightStates *state, InitStatus *status){
    switch (command){
        case ARM:
            //if(*state == STANDBY) //Don't want to switch out of drogue deploy or something into Armed
            //  switchState(*state, ARMED);
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
            //testing purposes
            digitalWrite(LED_BUILTIN, HIGH);
            // delay(400);
            // digitalWrite(LED_BUILTIN,LOW);
            break;
        case DROGUE:
            //testing purposes
            // digitalWrite(LED_BUILTIN, HIGH);
            // delay(400);
            digitalWrite(LED_BUILTIN,LOW);
            break;

        case STATUS:
            char statusReport1[RADIO_DATA_ARRAY_SIZE];
            char statusReport2[RADIO_DATA_ARRAY_SIZE];
            generateStatusReport(status, statusReport1, statusReport2);
            sendRadioResponse(statusReport1);
            sendRadioResponse(statusReport2);
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

