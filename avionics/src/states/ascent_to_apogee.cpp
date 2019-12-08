#include "states/ascent_to_apogee.h"

StateId State::AscentToApogee::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static int8_t check_count = 0;
    static float last_alt = input.altitude;

    if (input.altitude < last_alt){
        check_count++;
    } else if (check_count > 0) {
        check_count--;
    }
    last_alt = input.altitude;

    if (check_count >= M_APOGEE_CHECKS) {
        return StateId::PRESSURE_DELAY;
    } else {
        return StateId::ASCENT_TO_APOGEE;
    }
}

