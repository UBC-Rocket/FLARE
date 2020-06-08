#pragma once
#include <chrono>

namespace Hal {

/**
 * @brief Sleep for some number of milliseconds.
 * @param t Duration to sleep for, in milliseconds.
 */
void sleep_ms(uint32_t t);

/**
 * @brief Sleep for some number of microseconds.
 * @param t Duration to sleep for, in microseconds.
 */
void sleep_us(uint32_t t);

typedef std::chrono::milliseconds ms;
typedef std::chrono::time_point<std::chrono::steady_clock, ms> t_point;

/**
 * @brief Get current time point; is somewhat more portable than Arduino's
 * millis().
 * @return Current time_point, using std::chrono::steady_clock.
 */
t_point now_ms();

void initialSystem();
} // namespace Hal
