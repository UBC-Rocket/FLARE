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
    
    /**
     * @brief Construct a new state object
     * @param main_altitude
     * @param main_ignitor
     */
    DrogueDescent(const float main_altitude, Ignitor &main_ignitor)
        : main_altitude_(main_altitude), ignitor_(main_ignitor) {}

  private:
    friend RepeatedCheckBase<StateId::DROGUE_DESCENT, next_id, num_checks,
                             DrogueDescent<next_id, num_checks>>;
    float main_altitude_;
    Ignitor &ignitor_;

    /**
     * @brief compares current altitude vs main_altitude_
     * @param input current altitude
     * @return true if current altitude < main_altitude_
     */
    bool accept(Calculator const &input) {
        return input.altitude() < main_altitude_;
    }

    /**
     * @brief additional logic ran when exiting a state
     *
     */
    void extraOnExit() { ignitor_.fire(); }
};

} 
#endif
