/*Sensors Header*/
#ifndef COMMANDS_H
#define COMMANDS_H

/*Includes------------------------------------------------------------*/
#include "sensors.h"

/*Constants------------------------------------------------------------*/

#define ARM 'r'
#define CAMERAS_ON 'C'
#define CAMERAS_OFF 'O'
#define HALO 'H'
#define SATCOM 'S'
#define RESET 'R'
#define PING 'p'
#define MAIN 'm'
#define DROGUE 'd'


/*Variables------------------------------------------------------------*/


/*Functions------------------------------------------------------------*/
void doCommand(char command,FlightStates * state );
void sendRadioResponse(const char* response);


#endif
