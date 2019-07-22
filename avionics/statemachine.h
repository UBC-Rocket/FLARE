/*
 * State Machine Header
 *
 * @file    statemachine.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Main state machine of the rocket's flight states.
 * Implements state switching and the rocket's state machine.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

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
void stateMachine(float *altitude, float *delta_altitude,
    float *prev_delta_altitude, float bar_data[], float *baseline_pressure,
    float *ground_altitude, float ground_alt_arr[], FlightStates *state);


#endif
