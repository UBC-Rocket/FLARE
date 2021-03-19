#ifndef TASK_TRACKING_HPP_98C0DA74F40E4BACB5E22CE0B467872C
#define TASK_TRACKING_HPP_98C0DA74F40E4BACB5E22CE0B467872C

#include "RocketSchedule/fake_clocks.hpp"
#include "gmock/gmock.h"

namespace RocketSchedule {

class ITaskLogger {
  public:
    virtual ~ITaskLogger() {}

    void log(int id, int call_count) {
        logImpl(id, call_count, FakeClockWrapper::now());
    }

    // Called after call_count incremented
    virtual void logImpl(int id, int call_count, TimePoint time) = 0;
};

class MockTaskLogger : public ITaskLogger {
  public:
    MOCK_METHOD(void, logImpl, (int, int, TimePoint), (override));
};

} // namespace RocketSchedule

#endif
