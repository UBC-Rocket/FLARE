#include "gtest/gtest.h"

#include "schedule.hpp"

TEST(EnableDisable, Scheduler) {
    Schedule::enableRepeat(0);
    EXPECT_TRUE(Schedule::checkRepeat(0));
    Schedule::disableRepeat(0);
    EXPECT_FALSE(Schedule::checkRepeat(0));
}
