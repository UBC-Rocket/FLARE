#ifndef STATES_DROGUE_DESCENT
#define STATES_DROGUE_DESCENT

#include "cameras.h"
#include "hardware/ignitor.h"
#include "repeated_check_base.hpp"

namespace State {
template <StateId next_id, int num_checks>
class DrogueDescent
    : public RepeatedCheckBase<StateId::DROGUE_DESCENT, next_id, num_checks,
                               DrogueDescent<next_id, num_checks>> {
  public:
    DrogueDescent(const float main_altitude, Ignitor &main_ignitor)
        : main_altitude_(main_altitude), ignitor_(main_ignitor) {}

  private:
    friend RepeatedCheckBase<StateId::DROGUE_DESCENT, next_id, num_checks,
                             DrogueDescent<next_id, num_checks>>;
    float main_altitude_;
    Ignitor &ignitor_;
    bool accept(Calculator const &input) {
        return input.altitude() < main_altitude_;
    }

    void extraOnExit() { ignitor_.fire(); }
};

} 
#endif
