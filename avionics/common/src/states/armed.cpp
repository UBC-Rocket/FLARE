#include "states/armed.h"

StateId State::Armed::getNewState(StateInput const &input,
                                  StateAuxilliaryInfo &) {
    static uint8_t armed_count = 0;

    if (input.altitude > M_LAUNCH_THRESHOLD) {
        armed_count++;
    } else {
        armed_count = 0;
    }

    if (armed_count >= M_ARMED_CHECKS) {
        return m_ascent_id;
    } else {
        return StateId::ARMED;
    }
}
