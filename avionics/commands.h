/*Commands Header*/
#ifndef COMMANDS_H
#define COMMANDS_H

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include <Arduino.h>

/*Constants------------------------------------------------------------*/

#define RADIO_DATA_ARRAY_SIZE  5

#define ARM 'r'
#define CAMERAS_ON 'C'
#define CAMERAS_OFF 'O'
#define HALO 'H'
#define SATCOM_TEST 'A'
#define RESET 'R'
#define PING 'p'
#define MAIN 'm'
#define DROGUE 'd'
#define STATUS 'S'


/*Variables------------------------------------------------------------*/


/*Functions------------------------------------------------------------*/
void communicateThroughSerial(HardwareSerial SerialVar,FlightStates * state, InitStatus *status);
void doCommand(char command,FlightStates * state );
void sendRadioResponse(const char* response);
void sendSatComResponse(const char* response);
bool checkForValidty(char *radioCommand);

#endif
