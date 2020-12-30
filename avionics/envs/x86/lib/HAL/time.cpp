#include "stdio_controller.hpp"
#include <HAL/time.h>

namespace Hal {

uint16_t const US_PER_MS = 1000;

/**
 * @brief Sleep for some number of milliseconds. During debug, one of
 * Hal::sleep_ms() or Hal::sleep_us() is nessary to advance the simulated time.
 * @param t Duration to sleep for, in milliseconds.
 */
void sleep_ms(uint32_t t) {
    if (t >= (std::numeric_limits<uint32_t>::max() / US_PER_MS)) {
        throw std::invalid_argument(
            "The given sleep interval will cause integer overflow.");
    }

    uint32_t t_us = t * US_PER_MS;
    StdIoController::requestTimeUpdate(t_us);
}

/**
 * @brief Sleep for some number of microseconds. During debug, one of
 * Hal::sleep_ms() or Hal::sleep_us() is nessary to advance the simulated time.
 * @param t Duration to sleep for, in microseconds.
 */
void sleep_us(uint32_t t) { StdIoController::requestTimeUpdate(t); }

void initialSystem() {
    // NOTE Unused - time now handled by GS.
}

/**
 * @brief Retuns the current time in milliseconds relative to startup_t_point.
 */
t_point now_ms() {
    uint32_t now = StdIoController::requestTimeUpdate();
    t_point now_t = t_point(std::chrono::milliseconds(now));
    return now_t;
}

uint32_t millis() {
    return static_cast<uint32_t>(now_ms().time_since_epoch().count());
}

uint32_t tpoint_to_uint(t_point timestamp) {
    static_cast<uint32_t>(timestamp.time_since_epoch().count());
}
} // namespace Hal
