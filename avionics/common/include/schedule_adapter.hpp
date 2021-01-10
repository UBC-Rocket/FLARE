#include "HAL/time.h"
#include "RocketSchedule/schedule.hpp"
#include "env_config.h"

class SchedulerClock {
  public:
    typedef Hal::t_point time_point;
    typedef Hal::ms duration;

    static time_point now() { return Hal::now_ms(); }

#ifdef THIS_IS_NATIVE_CONFIGURATION
    static void idle(duration dur) { Hal::sleep_ms(dur.count()); }
#endif
};

typedef ScheduleBase<SchedulerClock, 16> Scheduler;
