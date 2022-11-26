#ifndef STATES_STANDBY
#define STATES_STANDBY

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

} // namespace State
#endif
