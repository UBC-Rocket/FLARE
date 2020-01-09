#include <HAL/time.h>

namespace Hal{
    std::chrono::time_point<std::chrono::steady_clock> now_ms(){
        return std::chrono::steady_clock::now();
    }

    /**
     * @brief Sleep for the given std::chrono::duration. Only available for x86 - Arduino framework doesn't implement enough of <chrono>.
     * @param t Duration to sleep for.
     */
    template<class Duration>
    void sleep(Duration t);{
        namespace c = std::chrono; //alias
        c::steady_clock::time_point start = c::steady_clock::now();
        c::steady_clock::time_point tgt = start + t; //Precalculate so checks can happen more often?
        while(c::steady_clock::now() < tgt);
    }

    /**
     * @brief Sleep for some number of milliseconds.
     * @param t Duration to sleep for, in milliseconds.
     */
    void sleep_ms(uint32_t t){
        sleep(std::chrono::milliseconds(t));
    }

    /**
     * @brief Sleep for some number of microseconds.
     * @param t Duration to sleep for, in microseconds.
     */
    void sleep_us(uint32_t t){
        sleep(std::chrono::microseconds(t));
    }

}
