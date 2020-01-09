#ifndef HAL__TIME_H_717DBAFB60CA40BCBB6780A74CDAF02C
#define HAL__TIME_H_717DBAFB60CA40BCBB6780A74CDAF02C

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

    /**
     * @brief Get current time point; is somewhat more portable than Arduino's millis().
     * @return Current time_point, using std::chrono::steady_clock.
     */
    std::chrono::time_point<std::chrono::steady_clock> now_ms();
}

#endif

