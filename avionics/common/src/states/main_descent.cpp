#include <Arduino.h>
#include "states/main_descent.h"

StateId State::MainDescent::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static float prev_altitude = input.altitude;
    static uint32_t prev_time = millis() / 1000.0; //seconds
    static uint32_t curr_time = millis() / 1000.0;
    static uint8_t num_checks = 0;

    if(curr_time - prev_time >= M_LANDED_TIME_INTERVAL) {
        if (abs((input.altitude - prev_altitude) / (curr_time - prev_time)) < M_LANDED_VELOCITY){
            num_checks++;
        } else {
            num_checks = 0;
        }

        if (num_checks < M_LANDED_CHECKS) {
            return StateId::MAIN_DESCENT;
        }
        prev_time = curr_time;
    }

    return StateId::LANDED;
}

