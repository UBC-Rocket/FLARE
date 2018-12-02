/* @file    statemachine.cpp
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

/*Includes------------------------------------------------------------*/
#include "statemachine.h"

#include <math.h>
#include <Arduino.h>
#include <SD.h>

/*Variables------------------------------------------------------------*/
File statelog;

/*Functions------------------------------------------------------------*/
/* void switchState(FlightStates *, FlightStates){}
 * @brief  Switches state machine state.
 * @param  FlightStates *curr_state - Current state machine state.
 * @param  FlightState new_state - State machine state to switch to.
 * @return void.
 */
void switchState(FlightStates *curr_state, FlightStates new_state){
    *curr_state = new_state;
}

/* void stateMachine(float*, float*, float*, float*, float*, float*, FlightStates *){}
 * @brief  The state machine of the rocket.  Implements current activities and tracks the rocket's flight state.
 * @param  float *altitude - Current altitude in meters/second
 * @param  float *delta_altitude - Change in altitude from the current altitude update.
 * @param  float *prev_delta_altitude - Previous change in altitude from the altitude update.
 * @param  float bar_data[] - received barometer sensor data array
 * @param  float *baseline_pressure - the baseline pressure of the rocket (calculated ground altitude)
 * @param  FlightState *state - State machine state.
 * @return void.
 */
void stateMachine(float *altitude, float *delta_altitude, float *prev_delta_altitude, float bar_data[], float *baseline_pressure,
    float *ground_altitude, FlightStates *state) {
    static int launch_count, armed_count, mach_count, mach_lock_count, apogee_count, main_count, land_count = 0;

    switch (*state) {
        case STANDBY:
            if (*altitude > LAUNCH_THRESHOLD) {
                launch_count++;
                if (launch_count >= LAUNCH_CHECKS){
                    switchState(state, ASCENT);
                    launch_count = 0;
                }
            }
            else{
                launch_count = 0;
                *baseline_pressure = groundAlt_update(&bar_data[0]);
                *ground_altitude = 44330.0 * (1 - powf(*baseline_pressure / SEA_PRESSURE, 1 / 5.255));
            }
            break;

        case ARMED:
            if (*altitude > LAUNCH_THRESHOLD) {
                armed_count++;
                if (armed_count >= LAUNCH_CHECKS){
                    switchState(state, ASCENT);
                    armed_count = 0;
                }
            }
            else{
                armed_count = 0;
            }
            break;

        case ASCENT:    // checks for Mach threshold + apogee
            if (*delta_altitude > MACH_THRESHOLD) {
                mach_count++;
                if (mach_count >= MACH_CHECKS) {
                    switchState(state, MACH_LOCK);
                    mach_count = 0;
                }
            }
            else {
                mach_count = 0;
            }
            if (*delta_altitude <= 0) {
                apogee_count ++;
                if (apogee_count >= APOGEE_CHECKS) {
                    //deploy drogue and payload
                    //delay to avoid pressure spikes
                    switchState(state, INITIAL_DESCENT);
                    apogee_count = 0;
                }
            }
            else {
                apogee_count = 0;
            }
            break;

        case MACH_LOCK: //checks for reduction in speed below mach threshold
            if ((*delta_altitude < MACH_LOCK_THRESHOLD) && (*delta_altitude < *prev_delta_altitude) ) {
                mach_lock_count++;
                if (mach_lock_count >= MACH_LOCK_CHECKS) {
                    switchState(state, ASCENT);
                    mach_lock_count = 0;
                }
            }
            else {
                mach_lock_count = 0;
            }
            break;

        case INITIAL_DESCENT:
            if (*altitude < FINAL_DESCENT_THRESHOLD) {
                main_count ++;
                if (main_count >= MAIN_CHECKS) {
                    //deploy main
                    switchState(state, FINAL_DESCENT);
                    main_count = 0;
                }
            }
            else {
                main_count = 0;
            }
            break;

        case FINAL_DESCENT:
            if (*altitude < LAND_HEIGHT_THRESHOLD && *delta_altitude < LAND_VELOCITY_THRESHOLD) {
                land_count++;
                if (land_count >= LAND_CHECKS) {
                    //turn off sensors except GPS
                    switchState(state, LANDED);
                    land_count = 0;
                }
            }
            else {
                land_count = 0;
            }
            break;

        case LANDED:
            break;

        default:
            break;
    }
}
