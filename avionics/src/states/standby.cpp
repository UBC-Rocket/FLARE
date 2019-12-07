#include "states/standby.h"

State::Standby::Standby(StateId ascent_id) {
    m_ascent_id = ascent_id;
}

StateId State::Standby::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static uint8_t check_count
}

