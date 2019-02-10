/*Sensors Header*/
#ifndef COMMANDS_H
#define COMMANDS_H

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include <Arduino.h>

/*Constants------------------------------------------------------------*/

#define DATA_ARRAY_SIZE   5

#define ARM 'r'
#define CAMERAS_ON 'C'
#define CAMERAS_OFF 'O'
#define HALO 'H'
#define SATCOM_TEST 'S'
#define RESET 'R'
#define PING 'p'
#define MAIN 'm'
#define DROGUE 'd'


/*Variables------------------------------------------------------------*/


/*Functions------------------------------------------------------------*/
void communicateThroughSerial(HardwareSerial SerialVar,FlightStates * state);
void doCommand(char command,FlightStates * state );
void sendRadioResponse(const char* response);
bool checkForValidty(char *radioCommand);

#endif