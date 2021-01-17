#ifndef SCHEDULER_HPP_027EFDB5B9564B41A3DB637CF47A3960
#define SCHEDULER_HPP_027EFDB5B9564B41A3DB637CF47A3960
#include "scheduler_clock.hpp"

typedef ScheduleBase<SchedulerClock, 16> Scheduler;

/* Task IDs =========================================================== */
enum class TaskID {
    ReadEvalLog = 0,
    RadioTxBulk = 1,
    LEDBlinker = 2,
};
#endif
