#include "states/pressure_delay.h"
#include <HAL/time.h>

StateId State::PressureDelay::getNewState(Calculator const &) {
    if (Hal::now_ms() - start_time_ >= Hal::ms(DELAY_TIME_MS_)) {
        return post_delay_;
    } else {
        return StateId::PRESSURE_DELAY;
    }
}
