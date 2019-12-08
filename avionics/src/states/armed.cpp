#include "states/armed.h"

StateId State::Armed::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static uint8_t armed_count = 0;

    if (*altitude > M_LAUNCH_THRESHOLD) {
        armed_count++;
    } else {
        armed_count = 0;
    }

    if (armed_count >= M_ARMED_CHECKS){
        return m_ascent_id;
    } else {
        return StateId::ARMED;
    }
}

