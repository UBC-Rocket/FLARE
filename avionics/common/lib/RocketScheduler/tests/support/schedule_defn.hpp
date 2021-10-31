#ifndef SCHEDULE_DEFN_HPP_3975B8DC35FB4016A3863CA3FD00CE87
#define SCHEDULE_DEFN_HPP_3975B8DC35FB4016A3863CA3FD00CE87
#include "RocketSchedule/fake_clocks.hpp"
#include "RocketSchedule/schedule.hpp"

namespace RocketSchedule {
typedef ScheduleBase<FakeClockWrapper, 32> Schedule;
}

#endif
