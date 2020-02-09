#include <HAL/time.h>
#include "states/pressure_delay.h"

StateId State::PressureDelay::getNewState(const StateInput &input, StateAuxilliaryInfo &state_aux){
    static auto start_time = Hal::now_ms();
    if (Hal::now_ms() - start_time >= std::chrono::milliseconds(M_DELAY_TIME_MS)) {
        return StateId::DROGUE_DESCENT;
    } else {
        return StateId::PRESSURE_DELAY;
    }
}

