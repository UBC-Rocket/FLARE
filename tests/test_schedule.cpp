#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <vector>

#include "schedule.hpp"

#include "fake_clocks.hpp"
#include "fake_tasks.hpp"
#include "task_tracking.hpp"

typedef ScheduleBase<FakeClock, 32> Schedule;

class ScheduleFixture : public ::testing::Test {
  protected:
    ScheduleFixture() {
        Schedule::clear();
        FakeClock::set_time(0);
    }
    ~ScheduleFixture() { Schedule::clear(); }
};

TEST_F(ScheduleFixture, EnableDisable) {
    Schedule::enableRepeat(0);
    Schedule::enableRepeat(1);
    EXPECT_TRUE(Schedule::checkRepeat(0));
    Schedule::disableRepeat(0);
    EXPECT_TRUE(Schedule::checkRepeat(1));
    EXPECT_FALSE(Schedule::checkRepeat(0));

    Schedule::enableRunEarly(0);
    Schedule::enableRunEarly(1);
    EXPECT_TRUE(Schedule::checkRunEarly(0));
    Schedule::disableRunEarly(0);
    EXPECT_TRUE(Schedule::checkRunEarly(1));
    EXPECT_FALSE(Schedule::checkRunEarly(0));
}

namespace {
using ::testing::_;
using ::testing::InSequence;
} // namespace

TEST_F(ScheduleFixture, CorrectOrder) {
    InSequence seq;
    MockTaskLogger logger;
    constexpr int PERIOD = 10;
    constexpr int NUM_TASKS = 10;

    // EXPECT_CALL(logger, logImpl(1, 1, _)).Times(NUM_TASKS);
    for (int id = 0; id < NUM_TASKS; id++) {
        int time = PERIOD * (id + 1);
        EXPECT_CALL(logger, logImpl(id, 1, time)).Times(1);
    }

    std::vector<SimpleTask> tasks;
    for (int i = 0; i < NUM_TASKS; ++i) {
        tasks.emplace_back(i, logger);
    }
    std::array<int, NUM_TASKS> task_order{9, 0, 2, 5, 4, 3, 7, 8, 6, 1};
    for (auto i : task_order) {
        Schedule::preregisterTask(
            i, Schedule::Task(SimpleTask::externAct, &(tasks[i]), PERIOD),
            false, false);
        Schedule::scheduleTask((i + 1) * PERIOD, i);
    }
    Schedule::run();
}
