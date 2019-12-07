#include "states/powered_ascent.h"


StateId State::PoweredAscent::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static uint8_t burnout_checks = 0;

    if (input.acceleration(0) < 0){
        burnout_checks++;
    } else {
        burnout_checks = 0;
    }

    if (burnout_checks > M_BURNOUT_CHECKS){
        return m_post_ascent_id;
    } else {
        return StateId::POWERED_ASCENT;
    }
}

