#ifndef SCHEDULER_HPP_027EFDB5B9564B41A3DB637CF47A3960
#define SCHEDULER_HPP_027EFDB5B9564B41A3DB637CF47A3960
#include "RocketSchedule/fake_clocks.hpp"
#include "RocketSchedule/schedule.hpp"

/* Task IDs =========================================================== */
enum class TaskID {
    ReadEvalLog = 0,
    RadioTxBulk = 1,
    LEDBlinker = 2,
    RestartCamera = 3,
    kNum,
};

typedef ScheduleBase<RocketSchedule::FakeClockWrapper,
                     static_cast<int>(TaskID::kNum)>
    Scheduler;
#endif
