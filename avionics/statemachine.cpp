/*State Machine Source*/

/*Includes------------------------------------------------------------*/
#include "statemachine.h"

#include <Arduino.h>
#include <SD.h>

/*Variables------------------------------------------------------------*/
File statelog;
FlightStates state = STANDBY;

/*Functions------------------------------------------------------------*/

void switchState(FlightStates new_state){
    state = new_state;
}

void stateMachine(float altitude, float delta_altitude, float base_altitude) {
    static int launch_count, armed_count, mach_count, mach_lock_count, apogee_count, main_count, land_count = 0;

    switch (state) {
        case STANDBY:
            if (altitude > (base_altitude + LAUNCH_THRESHOLD)) {
                launch_count++;
                if (launch_count >= LAUNCH_CHECKS){
                    switchState(ASCENT);
                    launch_count = 0;
                }
            }
            else{
                launch_count = 0;
            }
            break;

        case ARMED:
            if (altitude > (base_altitude + LAUNCH_THRESHOLD)) {
                armed_count++;
                if (armed_count >= LAUNCH_CHECKS){
                    switchState(ASCENT);
                    armed_count = 0;
                }
            }
            else{
                armed_count = 0;
            }
            break;

        case ASCENT:    // checks for Mach threshold + apogee
            if (delta_altitude > MACH_THRESHOLD) {
                mach_count++;
                if (mach_count >= MACH_CHECKS) {
                    switchState(MACH_LOCK);
                    mach_count = 0;
                }
            }
            else {
                mach_count = 0;
            }
            if (delta_altitude < 0) {
                apogee_count ++;
                if (apogee_count >= APOGEE_CHECKS) {
                    //deploy drogue and payload
                    //delay to avoid pressure spikes
                    switchState(INITIAL_DESCENT);
                    apogee_count = 0;
                }
            }
            else {
                apogee_count = 0;
            }
            break;

        case MACH_LOCK: //checks for reduction in speed below mach threshold
            if (delta_altitude < MACH_LOCK_THRESHOLD) {
                mach_lock_count++;
                if (mach_lock_count >= MACH_LOCK_CHECKS) {
                    switchState(ASCENT);
                    mach_lock_count = 0;
                }
            }
            else {
                mach_lock_count = 0;
            }
            break;

        case INITIAL_DESCENT:
            if (altitude < FINAL_DESCENT_THRESHOLD) {
                main_count ++;
                if (main_count >= MAIN_CHECKS) {
                    //deploy main
                    switchState(FINAL_DESCENT);
                    main_count = 0;
                }
            }
            else {
                main_count = 0;
            }
            break;

        case FINAL_DESCENT:
            if (altitude < (base_altitude + LAND_HEIGHT_THRESHOLD) && delta_altitude < LAND_VELOCITY_THRESHOLD) {
                land_count++;
                if (land_count >= LAND_CHECKS) {
                    //turn off sensors except GPS
                    switchState(LANDED);
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
