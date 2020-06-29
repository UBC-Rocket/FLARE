#include "states/pressure_delay.h"
#include <HAL/time.h>

StateId State::PressureDelay::getNewState(const StateInput &,
                                          StateAuxilliaryInfo &) {
    static auto start_time = Hal::now_ms();
    if (Hal::now_ms() - start_time >=
        std::chrono::milliseconds(M_DELAY_TIME_MS)) {
        return StateId::DROGUE_DESCENT;
    } else {
        return StateId::PRESSURE_DELAY;
    }
}
