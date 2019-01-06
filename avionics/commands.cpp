#include "commands.h"
#include "statemachine.h"
#include <Arduino.h>
#include "sensors.h"

void doCommand(char command, FlightStates *state){
    switch (command){
        case ARM:
        switchState(state, ARMED);
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
        delay(400);
        digitalWrite(LED_BUILTIN,LOW);
        break;
        case DROGUE: 
        //testing purposes  
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(400);
        digitalWrite(LED_BUILTIN,LOW);
        break;
    }

}

void sendRadioResponse(char* response){
    //teensy should be little endian, which means least significant is stored first, make sure ground station decodes accordingly 
    
      for(int i=0; i<4; i++)
      {
          SerialRadio.write(response[i]);
      }
}

