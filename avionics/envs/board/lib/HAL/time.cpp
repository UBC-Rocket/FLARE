#include <Arduino.h>
#include <HAL/time.h>

namespace Hal {

t_point now_ms() { return t_point(std::chrono::milliseconds(millis())); }

uint32_t millis() { return ::millis(); }
/**
 * @brief Sleep for some number of milliseconds.
 * @param t Duration to sleep for, in milliseconds.
 */
void sleep_ms(uint32_t t) { delay(t); }

/**
 * @brief Sleep for some number of microseconds.
 * @param t Duration to sleep for, in microseconds.
 */
void sleep_us(uint32_t t) { delayMicroseconds(t); }

} // namespace Hal
