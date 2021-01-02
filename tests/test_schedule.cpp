#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <vector>

#include "schedule.hpp"

class FakeClockBase {
  public:
    typedef uint32_t duration;
    typedef uint32_t time_point;

  protected:
    static time_point now_val;
};
typedef FakeClockBase::time_point TimePoint;
TimePoint FakeClockBase::now_val = 0;

class FakeClock : public FakeClockBase {
  public:
    static time_point now() { return now_val; }
    static void set_time(time_point new_time) { now_val = new_time; }
    static void idle(duration) { now_val++; }
};

class BusyWaitClock : public FakeClockBase {
  public:
    static time_point now() {
        // Needs to increment time; otherwise busy wait never exists
        return now_val++;
    }
};

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

class SimpleTask;
class ITaskLogger {
  public:
    virtual ~ITaskLogger() {}

    void log(SimpleTask *task);

    void log(int id, int call_count) {
        logImpl(id, call_count, FakeClock::now());
    }

    // Called after call_count incremented
    virtual void logImpl(int id, int call_count, TimePoint time) = 0;
};

class SimpleTask {
  public:
    SimpleTask(int id, ITaskLogger &log) : call_count(0), id_(id), log_(log) {}
    void action() {
        call_count++;
        log_.log(this);
    }
    int call_count;
    int id_;
    ITaskLogger &log_;
    static void externAct(void *me) { static_cast<SimpleTask *>(me)->action(); }
};

void ITaskLogger::log(SimpleTask *task) {
    { log(task->id_, task->call_count); }
}

class MockTaskLogger : public ITaskLogger {
  public:
    MOCK_METHOD(void, logImpl, (int, int, TimePoint), (override));
};

using ::testing::_;
using ::testing::InSequence;

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
