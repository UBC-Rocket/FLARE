/*State Machine Header*/
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

/*Includes------------------------------------------------------------*/
#include "groundaltitude.h"

/*Constants------------------------------------------------------------*/
#define STANDBY_LAUNCH_CHECKS 4  // same as armed launch checks for backup standby to launch
#define ARMED_LAUNCH_CHECKS 4
#define MACH_CHECKS 3
#define APOGEE_CHECKS   5
#define MACH_LOCK_CHECKS 20
#define MAIN_CHECKS     10
#define LAND_CHECKS     6

#define LAUNCH_THRESHOLD 50 // in meters
#define MACH_THRESHOLD 260 //in meters per second
#define MACH_LOCK_THRESHOLD 240 //in meters per second
#define FINAL_DESCENT_THRESHOLD 460 // 458 ~= 1500 feet for final
#define LAND_VELOCITY_THRESHOLD 4  // meters per LANDING_TIME_INTERVAL

#define SEA_PRESSURE 1013.25

#define APOGEE_DELAY 3000 //ms
#define LANDING_TIME_INTERVAL 10000 //ms  //datapoint delay when looking for landing event
#define TOGGLE_CAMERA_INTERVAL 200

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
