#include "states/main_descent.h"

StateId State::MainDescent::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static float prev_altitude = input.altitude;
    static float prev_time = millis() / 1000.0; //seconds
    static float curr_time = millis() / 1000.0;
    static uint8_t num_checks = 0;

    if(curr_time - prev_time >= M_LANDED_TIME_INTERVAL) {
        if (abs((input.altitude - prev_altitude) / (curr_time - prev_time)) < M_LANDED_VELOCITY){
            num_checks++;
        } else {
            num_checks = 0;
        }

        if (num_checks >= M_LANDED_CHECKS) {
            return StateId::LANDED;
        } else {
            return StateId::MAIN_DESCENT;
        }
    }
}

