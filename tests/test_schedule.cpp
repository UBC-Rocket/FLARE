#include "gtest/gtest.h"

#include "schedule.hpp"

class FakeClock {
  public:
    typedef uint32_t duration;
    typedef uint32_t time_point;
    static time_point now_val;
    static time_point now() { return now_val; }
};

typedef ScheduleBase<FakeClock, 32> Schedule;

class ScheduleFixture : public ::testing::Test {
  protected:
    ScheduleFixture() { Schedule::clear(); }
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
