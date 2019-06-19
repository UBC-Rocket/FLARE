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
#include "satcom.h"
#include "cameras.h"
#include "gpio.h"
#include "options.h"

#include <math.h>
#include <Arduino.h>
#include <SD.h>

/*Constants------------------------------------------------------------*/
#define STANDBY_LAUNCH_CHECKS 4  // same as armed launch checks for backup standby to launch
#define ARMED_LAUNCH_CHECKS 4
#define MACH_CHECKS 3
#define APOGEE_CHECKS   5
#define MACH_LOCK_CHECKS 20
#define MAIN_CHECKS     10
#define LAND_CHECKS     6

#define LAUNCH_THRESHOLD 50 // in meters
#define MACH_THRESHOLD 225 //in meters per second
#define MACH_LOCK_THRESHOLD 220 //in meters per second
#define FINAL_DESCENT_THRESHOLD 500 // 458 ~= 1500 feet for final
#define LAND_VELOCITY_THRESHOLD 4  // meters per LANDING_TIME_INTERVAL

#define SEA_PRESSURE 1013.25

#define APOGEE_DELAY 3000 //ms
#define LANDING_TIME_INTERVAL 10000 //ms  //datapoint delay when looking for landing event
#define TOGGLE_CAMERA_INTERVAL 200

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
    if(*curr_state != WINTER_CONTINGENCY) //don't want to switch out of WINTER_CONTINGENCY accidentally
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
    float *ground_altitude, float ground_alt_arr[], FlightStates *state) {
    static int launch_count = 0, armed_count = 0, mach_count = 0, mach_lock_count = 0, apogee_count = 0,
        main_count = 0, land_count = 0, camera_toggle_count = 0;
    static int base_alt_counter = 0;
    static uint32_t old_time_landed = millis(); //initialize on switching state
    static float old_altitude_landed = *altitude; //initialize on switching state
    static bool camera_toggle = true;

    switch (*state) {
        case STANDBY:
            if (*altitude > LAUNCH_THRESHOLD) {
                launch_count++;
                if (launch_count >= STANDBY_LAUNCH_CHECKS){
                    switchState(state, ASCENT);
                    digitalWrite(FLIGHT_LED, LOW);
                    launch_count = 0;
                    // turn on cameras
                    start_record();
                }
            }
            else{
                launch_count = 0;
                base_alt_counter++;
                if(base_alt_counter >= 20){
                    *baseline_pressure = groundAlt_update(&bar_data[0], ground_alt_arr);
                    *ground_altitude = 44330.0 * (1 - powf(*baseline_pressure / SEA_PRESSURE, 1 / 5.255));
                    base_alt_counter = 0;
                }
            }
            break;

        case ARMED:
            if (*altitude > LAUNCH_THRESHOLD) {
                armed_count++;
                if (armed_count >= ARMED_LAUNCH_CHECKS){
                    switchState(state, ASCENT);
                    digitalWrite(FLIGHT_LED, LOW);
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
                    #ifdef BODY
                        deployDrogue();
                    #endif
                    digitalWrite(FLIGHT_LED, HIGH);
                    switchState(state, PRESSURE_DELAY);
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

        case PRESSURE_DELAY:
            static unsigned long delay_start = millis();
            if((millis() - delay_start) >= APOGEE_DELAY)
            {
                switchState(state, INITIAL_DESCENT);
                digitalWrite(FLIGHT_LED, LOW);
            }
            break;

        case INITIAL_DESCENT:
            if (*altitude < FINAL_DESCENT_THRESHOLD) {
                main_count ++;
                if (main_count >= MAIN_CHECKS) {
                    #ifdef BODY
                        deployMain();
                    #endif
                    digitalWrite(FLIGHT_LED, HIGH);
                    old_time_landed = millis();
                    old_altitude_landed = *altitude;
                    switchState(state, FINAL_DESCENT);
                    main_count = 0;
                }
            }
            else {
                main_count = 0;
            }
            break;

        case FINAL_DESCENT:
            camera_toggle_count ++;
            if(millis() - old_time_landed >= LANDING_TIME_INTERVAL) {
                float delta_altitude_landed = *altitude - old_altitude_landed;
                if (delta_altitude_landed <= LAND_VELOCITY_THRESHOLD) { // Landed threshold based on velocity alone
                    land_count++;
                    if (land_count >= LAND_CHECKS) {
                        stop_record();
                        switchState(state, LANDED);
                        digitalWrite(FLIGHT_LED, LOW);
                        land_count = 0;
                    }
                }
                else {
                    land_count = 0;
                }
                old_time_landed = millis();
                old_altitude_landed = *altitude;
            }
            if((camera_toggle_count >= TOGGLE_CAMERA_INTERVAL) && (camera_toggle)){
                stop_record();
                delay(15);
                start_record(); //toggles the camera so we won't lose the video in case it gets damaged during landing.
                camera_toggle_count = 0;
                camera_toggle = false;
            }
            break;

        case LANDED:
            stop_record();
            break;

        case WINTER_CONTINGENCY:
            break;

        default:
            break;
    }
}
