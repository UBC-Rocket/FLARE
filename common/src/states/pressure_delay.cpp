#include "states/pressure_delay.h"

StateId State::PressureDelay::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static uint32_t start_time = millis();
    if (millis() - start_time > M_DELAY_TIME_MS) {
        return StateId::DROGUE_DESCENT;
    } else {
        return StateId::PRESSURE_DELAY;
    }
}

