#ifndef STATES_ARMED
#define STATES_ARMED

#include "cameras.h"
#include "hardware/ignitor.h"
#include "repeated_check_base.hpp"

namespace State {
template <StateId next_id, int num_checks>
class Armed : public RepeatedCheckBase<StateId::ARMED, next_id, num_checks,
                                       Armed<next_id, num_checks>> {
  public:
    
    /**
     * @brief Construct a new state object
     * @param launch_threshold
     */
    Armed(const float launch_threshold) : launch_threshold_(launch_threshold) {}

  private:
    friend RepeatedCheckBase<StateId::ARMED, next_id, num_checks,
                             Armed<next_id, num_checks>>;
    float launch_threshold_;
    float last_alt_;

    /**
     * @brief compares altitude of parameter vs launch_threshold_
     * @param input altitude
     * @return true if current altitude > launch_threshold_
     */
    bool accept(Calculator const &input) {
      float altitude = input.altitude();
      bool isAccepted = altitude > launch_threshold_ && altitude > last_alt_;
      last_alt_ = altitude;
      return isAccepted;
    }

    void extraOnEntry() {
      last_alt_ = 0;
    }
};

} 
#endif
