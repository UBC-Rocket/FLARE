#ifndef FAKE_CLOCKS_HPP_CE5C3CAC8C614EB78253CD129A6DEB83
#define FAKE_CLOCKS_HPP_CE5C3CAC8C614EB78253CD129A6DEB83

#include <cstdint>
#include <exception>

typedef uint32_t TimePoint;
typedef uint32_t Duration;

class IFakeClock {
    // Pure virtual functions are a bit difficult, since a static version of
    // this is in the FakeClock; instead have some reasonable defaults
    //
    // Fake clocks should generally inherit from this class and then set
    // `FakeClock::impl` in their constructor.

  public:
    IFakeClock(TimePoint start_time) : now_val_(start_time) {}
    IFakeClock() : now_val_(0) {}

    virtual TimePoint now() { return now_val_; }
    virtual void idle(Duration dur) { now_val_ += dur; }
    void incrementTime(Duration dur) { now_val_ += dur; }

    virtual ~IFakeClock() {}

  protected:
    TimePoint now_val_;
};

class FakeClockWrapper {
  public:
    typedef Duration duration;
    typedef TimePoint time_point;

    static IFakeClock *impl;

    static time_point now() { return impl->now(); }
    static void idle(duration dur) { impl->idle(dur); }
};

/**
 * Exceptions to forcibly exit scheduler
 */
class ClockFinishedExc : public std::exception {};

/**
 * \brief Once the time reaches the specified stop_time, all methods will raise
 * ClockFinishedExc, which can then be caught outside of the scheduler.
 */
class StoppingClock : public IFakeClock {
  public:
    StoppingClock(TimePoint stop_time, TimePoint start_time)
        : IFakeClock(start_time), stop_time_(stop_time) {
        FakeClockWrapper::impl = this;
    }

    StoppingClock(TimePoint stop_time) : StoppingClock(stop_time, 0) {}

    TimePoint now() override {
        check_stop();
        return now_val_;
    }
    void idle(Duration dur) override {
        now_val_ += dur;
        check_stop();
    }
    void check_stop() {
        if (now_val_ >= stop_time_) {
            throw ClockFinishedExc();
        }
    }

  private:
    TimePoint stop_time_;
};

#endif
