#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <vector>

#include "schedule.hpp"

#include "fake_clocks.hpp"
#include "fake_tasks.hpp"
#include "task_tracking.hpp"

typedef ScheduleBase<FakeClockWrapper, 32> Schedule;

class ScheduleFixture : public ::testing::Test {
  protected:
    ScheduleFixture() {
        Schedule::clear();
        FakeClockWrapper::impl = &clock;
    }
    ~ScheduleFixture() { Schedule::clear(); }

  private:
    IFakeClock clock;
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
using ::testing::Expectation;
using ::testing::ExpectationSet;
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
    std::array<int, NUM_TASKS> task_order{9, 0, 2, 5, 4, 3, 7, 8, 1, 6};
    for (auto i : task_order) {
        Schedule::preregisterTask(
            i, Schedule::Task(SimpleTask::externAct, &(tasks[i]), PERIOD),
            false, false);
        Schedule::scheduleTask((i + 1) * PERIOD, i);
    }
    Schedule::run();
}

TEST_F(ScheduleFixture, EasyRepeat) {
    StoppingClock stopping_clock(50); // constructor does all the work
    MockTaskLogger logger;
    std::vector<int> times{7, 11, 13}; // prime numbers
    ExpectationSet expect_zeros;
    std::vector<SimpleTask> tasks;

    for (std::size_t id = 0; id < times.size(); ++id) {
        tasks.emplace_back(id, logger);
        expect_zeros += EXPECT_CALL(logger, logImpl(id, 1, 0)).Times(1);
    }
    for (std::size_t id = 0; id < times.size(); ++id) {
        // 2nd loop needed to prevent addresses from being invalidated
        Schedule::Task schedule_task(SimpleTask::externAct, &(tasks[id]),
                                     times[id]);
        Schedule::registerTask(id, schedule_task);
    }

    for (int t = 1; t < 50; t++) {
        for (std::size_t id = 0; id < times.size(); ++id) {
            auto &period = times[id];
            if (t % period == 0) {
                EXPECT_CALL(logger, logImpl(id, t / period + 1, t))
                    .Times(1)
                    .After(expect_zeros);
            }
        }
    }

    try {
        Schedule::run();
    } catch (ClockFinishedExc &) {
    }
}
