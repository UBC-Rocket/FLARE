#include "states/main_descent.h"
#include "HAL/time.h"

StateId State::MainDescent::getNewState(Calculator const &input) {
    typedef std::chrono::milliseconds ms;

    curr_time = Hal::now_ms();
    if (curr_time - prev_time >= LANDED_TIME_INTERVAL_) {
        auto dt_ms =
            std::chrono::duration_cast<ms>(curr_time - prev_time).count();
        if (abs(input.altitude() - prev_altitude) * 1000 / dt_ms <
            LANDED_VELOCITY_) {
            num_checks++;
        } else {
            num_checks = 0;
        }

        if (num_checks >= LANDED_CHECKS_) {
            return landed_state_;
        }

        prev_time = curr_time;
        prev_altitude = input.altitude();
    }

    return StateId::MAIN_DESCENT;
}
