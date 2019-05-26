/*State Machine Header*/
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

/*Includes------------------------------------------------------------*/
#include "groundaltitude.h"

/*Constants------------------------------------------------------------*/
// Polling time changes
#define LANDED_POLLING_TIME_INTERVAL 5000 //ms
#define NOMINAL_POLLING_TIME_INTERVAL 50  //ms

/*Variables------------------------------------------------------------*/
enum FlightStates {
  STANDBY, // 0
  ARMED,   // 1
  ASCENT,   //2
  MACH_LOCK,  //3
  PRESSURE_DELAY, //4
  INITIAL_DESCENT,  //5
  FINAL_DESCENT,    //6
  LANDED,          //7
  WINTER_CONTINGENCY //8 //For datalogging only
};

/*Functions------------------------------------------------------------*/
void switchState(FlightStates *curr_state, FlightStates new_state);
void stateMachine(float*, float*, float*, float*, float*, float*, float*, FlightStates*);


#endif
