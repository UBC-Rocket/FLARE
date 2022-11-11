#ifndef SCHEDULER_CLOCK_HPP_7BA322CFFDDE4E3EB2CBA99E36951D6A
#define SCHEDULER_CLOCK_HPP_7BA322CFFDDE4E3EB2CBA99E36951D6A

#include "HAL/time.h"
#include "RocketSchedule/schedule.hpp"
#include "env_config.h"

class SchedulerClock {
  public:
    typedef Hal::t_point time_point;
    typedef Hal::ms duration;

    static time_point now() { return Hal::now_ms(); }

#ifdef THIS_IS_NATIVE_CONFIGURATION

    /**
     * @brief sleeps for a given duration
     * @param dur duration in ms to sleep for
     */
    static void idle(duration dur) {
        env_callbacks();
        Hal::sleep_ms(dur.count());
    }
#endif
};

#endif
