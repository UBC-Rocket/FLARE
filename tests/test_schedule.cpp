#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <vector>

#include "RocketSchedule/fake_clocks.hpp"
#include "RocketSchedule/schedule.hpp"

#include "fake_tasks.hpp"
#include "schedule_defn.hpp"
#include "task_tracking.hpp"

namespace {
using ::testing::_;
using ::testing::Expectation;
using ::testing::ExpectationSet;
using ::testing::InSequence;
using ::testing::Sequence;
} // namespace

namespace RocketSchedule {
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

TEST_F(ScheduleFixture, RunEarly) {
    constexpr int REP = 0;
    constexpr int BLK = 1;
    MockTaskLogger logger;
    StoppingClock says_stop_at(44);

    LongTask longt(0, logger, 5);
    SimpleTask shortt(1, logger);

    Schedule::Task repeater(LongTask::externAct, &longt, 10);
    Schedule::Task blocker(SimpleTask::externAct, &shortt, 1);

    Schedule::registerTask(0, repeater, true, true);

    Schedule::preregisterTask(1, blocker, false, false);
    Schedule::preregisterTask(2, blocker, false, false);
    Schedule::preregisterTask(3, blocker, false, false);
    Schedule::preregisterTask(4, blocker, false, false);
    Schedule::preregisterTask(5, blocker, false, false);

    Schedule::scheduleTask(24, 1);
    Schedule::scheduleTask(30, 2);
    Schedule::scheduleTask(41, 3);
    Schedule::scheduleTask(42, 4);
    Schedule::scheduleTask(43, 5);

    InSequence seq;
    // After manually simulating
    //                         ID   cnt time
    EXPECT_CALL(logger, logImpl(REP, 1, 0));
    EXPECT_CALL(logger, logImpl(REP, 2, 5));
    EXPECT_CALL(logger, logImpl(REP, 3, 10));
    EXPECT_CALL(logger, logImpl(REP, 4, 15));
    EXPECT_CALL(logger, logImpl(BLK, 1, 24));
    EXPECT_CALL(logger, logImpl(REP, 5, 24));
    EXPECT_CALL(logger, logImpl(BLK, 2, 30));
    EXPECT_CALL(logger, logImpl(REP, 6, 30));
    EXPECT_CALL(logger, logImpl(REP, 7, 35));
    EXPECT_CALL(logger, logImpl(BLK, 3, 41));
    EXPECT_CALL(logger, logImpl(BLK, 4, 42));
    EXPECT_CALL(logger, logImpl(BLK, 5, 43));
    EXPECT_CALL(logger, logImpl(REP, 8, 43));

    try {
        Schedule::run();
    } catch (ClockFinishedExc &) {
    }
}

TEST_F(ScheduleFixture, RunLate) {
    MockTaskLogger logger;
    StoppingClock says_stop_at(56);

    LongTask hungry(0, logger, 30);
    Schedule::preregisterTask(
        0, Schedule::Task(LongTask::externAct, &hungry, 0), false, false);
    Schedule::scheduleTask(20, 0);

    SimpleTask before(1, logger);
    Schedule::preregisterTask(
        1, Schedule::Task(SimpleTask::externAct, &before, 0), false, false);
    Schedule::scheduleTask(19, 1);

    SimpleTask before_reps(2, logger);
    Schedule::preregisterTask(
        2, Schedule::Task(SimpleTask::externAct, &before_reps, 15), true,
        false);
    Schedule::scheduleTask(10, 2);

    SimpleTask midway_reps(3, logger);
    Schedule::preregisterTask(
        3, Schedule::Task(SimpleTask::externAct, &midway_reps, 10), true,
        false);
    Schedule::scheduleTask(23, 3);

    SimpleTask midway(4, logger);
    Schedule::preregisterTask(
        4, Schedule::Task(SimpleTask::externAct, &midway, 0), false, false);
    Schedule::scheduleTask(30, 4);

    SimpleTask midlate_reps(5, logger);
    Schedule::preregisterTask(
        5, Schedule::Task(SimpleTask::externAct, &midlate_reps, 10), true,
        false);
    Schedule::scheduleTask(45, 5);

    SimpleTask after(6, logger);
    Schedule::preregisterTask(
        6, Schedule::Task(SimpleTask::externAct, &after, 0), false, false);
    Schedule::scheduleTask(51, 6);

    // logimpl(ID, count, time)
    EXPECT_CALL(logger, logImpl(2, 1, 10));
    EXPECT_CALL(logger, logImpl(1, 1, 19));
    EXPECT_CALL(logger, logImpl(0, 1, 20));

    // Now long hungry task is running
    Sequence backlog;
    EXPECT_CALL(logger, logImpl(3, 1, 50)).InSequence(backlog);
    EXPECT_CALL(logger, logImpl(2, 2, 50)).InSequence(backlog);
    EXPECT_CALL(logger, logImpl(4, 1, 50)).InSequence(backlog);
    Expectation last_backlog =
        EXPECT_CALL(logger, logImpl(5, 1, 50)).InSequence(backlog);

    // Backlog cleared, now several tasks are immediately rescheduled
    EXPECT_CALL(logger, logImpl(3, 2, 50)).After(last_backlog);
    EXPECT_CALL(logger, logImpl(2, 3, 50)).After(last_backlog);

    EXPECT_CALL(logger, logImpl(6, 1, 51));
    EXPECT_CALL(logger, logImpl(5, 2, 55));

    try {
        Schedule::run();
    } catch (ClockFinishedExc &) {
    }
}

TEST_F(ScheduleFixture, DisableUnschedule) {
    MockTaskLogger logger;
    StoppingClock says_stop_at(150); // Should end before

    SimpleTask st0(0, logger);
    Schedule::registerTask(0, Schedule::Task(SimpleTask::externAct, &st0, 10));

    SimpleTask st1(1, logger);
    Schedule::registerTask(1, Schedule::Task(SimpleTask::externAct, &st1, 10));

    auto disabler =
        AdvancedTask(10, logger).unscheduleOtherAt(0, 1).disableRepeatAt(1, 1);
    Schedule::preregisterTask(
        10, Schedule::Task(AdvancedTask::externAct, &disabler, 0), false,
        false);
    Schedule::scheduleTask(15, 10);

    EXPECT_CALL(logger, logImpl(0, 1, 0));
    EXPECT_CALL(logger, logImpl(0, 2, 10));
    EXPECT_CALL(logger, logImpl(1, 1, 0));
    EXPECT_CALL(logger, logImpl(1, 2, 10));
    EXPECT_CALL(logger, logImpl(1, 3, 20));

    EXPECT_CALL(logger, logImpl(10, 1, 15));

    try {
        Schedule::run();
    } catch (ClockFinishedExc &) {
        ASSERT_TRUE(false) << "Timed out";
    }
}

TEST_F(ScheduleFixture, StopSelf) {
    MockTaskLogger logger;
    StoppingClock says_stop_at(150); // Should end before

    EXPECT_CALL(logger, logImpl(11, 1, 0));
    EXPECT_CALL(logger, logImpl(11, 2, 10));

    auto disable_self = AdvancedTask(11, logger).disableRepeatAt(11, 2);
    Schedule::registerTask(
        11, Schedule::Task(AdvancedTask::externAct, &disable_self, 10));

    try {
        Schedule::run();
    } catch (ClockFinishedExc &) {
        ASSERT_TRUE(false) << "Timed out";
    }
}

TEST_F(ScheduleFixture, Unschedule) {
    MockTaskLogger logger;

    constexpr int NUM_TASKS = 10;

    std::vector<SimpleTask> tasks;

    for (int i = 0; i < NUM_TASKS; ++i) {
        tasks.emplace_back(i, logger);
    }
    std::array<int, NUM_TASKS> shuffle{9, 0, 2, 5, 4, 3, 7, 8, 1, 6};
    // Each task starts at its ID, but inserted in random order
    for (int i = 0; i < NUM_TASKS; ++i) {
        int j = shuffle[i];
        Schedule::preregisterTask(
            j, Schedule::Task(SimpleTask::externAct, &(tasks[j]), 0), false,
            false);
        Schedule::scheduleTask(j + 1, j);
    }
    for (auto i : {0, 8, 4, 6, 2}) {
        Schedule::unschedule(i);
    }
    EXPECT_CALL(logger, logImpl(1, 1, 1 + 1));
    EXPECT_CALL(logger, logImpl(3, 1, 3 + 1));
    EXPECT_CALL(logger, logImpl(5, 1, 5 + 1));
    EXPECT_CALL(logger, logImpl(7, 1, 7 + 1));
    EXPECT_CALL(logger, logImpl(9, 1, 9 + 1));
    Schedule::run();
}

/**
 * Disrupt the existing task by adding a number of additional tasks
 */
TEST_F(ScheduleFixture, NewTasks) {
    MockTaskLogger logger;
    StoppingClock says_stop_at(50); // Should end before

    constexpr int NUM_ADDNL_TASKS = 10;

    std::vector<SimpleTask> addnl_tasks;

    for (int i = 0; i < NUM_ADDNL_TASKS; ++i) {
        addnl_tasks.emplace_back(i + 1, logger);
    }

    constexpr int period = 30;

    SpawningTask main_task(addnl_tasks, logger);
    Schedule::registerTask(
        0, Schedule::Task(SpawningTask::externAct, &main_task, period));

    EXPECT_CALL(logger, logImpl(0, 1, 0));
    EXPECT_CALL(logger, logImpl(0, 2, period));

    for (int i = 0; i < NUM_ADDNL_TASKS; ++i) {
        EXPECT_CALL(logger, logImpl(i + 1, 1, 0));
    }

    try {
        Schedule::run();
    } catch (ClockFinishedExc &) {
        // Expected
    }
}

} // namespace RocketSchedule
