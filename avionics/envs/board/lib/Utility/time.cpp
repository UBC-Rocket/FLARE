#include <Arduino.h>
#include <Utility/time.h>

namespace Utility{
    std::chrono::time_point<std::chrono::steady_clock> now_ms(){
        return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds(millis()));
    }

    /**
     * @brief Sleep for some number of milliseconds.
     * @param t Duration to sleep for, in milliseconds.
     */
    void sleep_ms(uint32_t t){
        delay(t);
    }

    /**
     * @brief Sleep for some number of microseconds.
     * @param t Duration to sleep for, in microseconds.
     */
    void sleep_us(uint32_t t){
        delayMicroseconds(t);
    }
}
