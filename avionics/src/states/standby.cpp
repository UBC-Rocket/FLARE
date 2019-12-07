#include "states/standby.h"

StateId State::Standby::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static uint8_t check_count = 0;

    if (input.altitude > M_LAUNCH_THRESHOLD){
        check_count++;
    } else {
        check_count = 0;
    }

    if (check_count > M_STANDBY_CHECKS){
        return m_ascent_id;
    } else {
        return StateId::STANDBY;
    }
}

