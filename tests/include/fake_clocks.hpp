#ifndef FAKE_CLOCKS_HPP_CE5C3CAC8C614EB78253CD129A6DEB83
#define FAKE_CLOCKS_HPP_CE5C3CAC8C614EB78253CD129A6DEB83

#include <cstdint>

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

#endif
