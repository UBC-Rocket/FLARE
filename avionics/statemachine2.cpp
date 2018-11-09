/*State Machine Source*/

/*Includes------------------------------------------------------------*/
#include "statemachine.h"

#include <Arduino.h>
#include <SD.h>

/*Variables------------------------------------------------------------*/
File statelog;
FlightStates state = STANDBY;
volatile bool launch_interrupt_flag = false;


/*Functions------------------------------------------------------------*/

void launchInterrupt()
{
    launch_interrupt_flag = true;
}

void switchState(FlightStates new_state)
{
    state = new_state;
}

void stateMachine(float abs_accel, float altitude, float delta_altitude)
{
}