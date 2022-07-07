#ifndef STATES_REPEATED_CHECKS_HPP_5A521F841076442ABBD5520A3B85FC2B
#define STATES_REPEATED_CHECKS_HPP_5A521F841076442ABBD5520A3B85FC2B

#include "cameras.h"
#include "hardware/ignitor.h"
#include "repeated_check_base.hpp"

namespace State {
template <StateId next_id, int num_checks>
class Standby : public RepeatedCheckBase<StateId::STANDBY, next_id, num_checks,
                                         Standby<next_id, num_checks>> {
  public:
    Standby(const float launch_threshold, Camera &camera)
        : launch_threshold_(launch_threshold), camera_(camera) {}

  private:
    friend RepeatedCheckBase<StateId::STANDBY, next_id, num_checks,
                             Standby<next_id, num_checks>>;
    float launch_threshold_;
    Camera &camera_;
    bool accept(Calculator const &input) {
        return input.altitude() > launch_threshold_;
    }

    // Exiting directly is abnormal - means we didn't get arm signal.  Normally
    // rocket command parser addresses camera on/off
    void extraOnExit() { camera_.start_record(); }
};

template <StateId next_id, int num_checks>
class Armed : public RepeatedCheckBase<StateId::ARMED, next_id, num_checks,
                                       Armed<next_id, num_checks>> {
  public:
    Armed(const float launch_threshold) : launch_threshold_(launch_threshold) {}

  private:
    friend RepeatedCheckBase<StateId::ARMED, next_id, num_checks,
                             Armed<next_id, num_checks>>;
    float launch_threshold_;
    bool accept(Calculator const &input) {
        return input.altitude() > launch_threshold_;
    }
};

template <StateId next_id, int num_checks>
class DrogueDescent
    : public RepeatedCheckBase<StateId::DROGUE_DESCENT, next_id, num_checks,
                               DrogueDescent<next_id, num_checks>> {
  public:
    DrogueDescent(const float main_altitude, Ignitor &main_ignitor, Ignitor &redundant_main_ignitor)
        : main_altitude_(main_altitude), ignitor_(main_ignitor), redundant_ignitor_(redundant_main_ignitor) {}

  private:
    friend RepeatedCheckBase<StateId::DROGUE_DESCENT, next_id, num_checks,
                             DrogueDescent<next_id, num_checks>>;
    float main_altitude_;
    Ignitor &ignitor_;
    Ignitor &redundant_ignitor_;
    bool accept(Calculator const &input) {
        return input.altitude() < main_altitude_;
    }

    void extraOnExit() { 
      ignitor_.fire();
      redundant_ignitor_.fire();
    }
};

} // namespace State
#endif
