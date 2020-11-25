#include "stdio_controller.hpp"
#include <HAL/time.h>

namespace Hal {
/**
 * @brief Sleep for some number of milliseconds.
 * @param t Duration to sleep for, in milliseconds.
 */
void sleep_ms(uint32_t t) {
    if (t >= (std::numeric_limits<uint32_t>::max() / 1000))
        throw "The given sleep length will cause overflow.";

    uint32_t t_us = t * 1000;
    StdIoController::requestTimeUpdate(t_us);
}

/**
 * @brief Sleep for some number of microseconds.
 * @param t Duration to sleep for, in microseconds.
 */
void sleep_us(uint32_t t) {
    StdIoController::requestTimeUpdate(t);
}

t_point startup_t_point =
    std::chrono::time_point_cast<ms>(std::chrono::steady_clock::now());

void initialSystem() {
    startup_t_point =
        std::chrono::time_point_cast<ms>(std::chrono::steady_clock::now());
}

/**
 * @brief Retuns the current time in milliseconds relative to startup_t_point.
 */
t_point now_ms() {
    uint32_t now = StdIoController::requestTimeUpdate();
    t_point now_t = t_point(std::chrono::milliseconds(now));
    return t_point{now_t - startup_t_point};
}

uint32_t millis() {
    return static_cast<uint32_t>(now_ms().time_since_epoch().count());
}
} // namespace Hal
