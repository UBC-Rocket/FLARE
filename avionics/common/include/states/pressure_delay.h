#pragma once

/*Includes------------------------------------------------------------*/
#include "HAL/time.h"

#include "state_interface.h"

namespace State {

class PressureDelay : public IState {
  public:

    /**
     * @brief Construct a new Pressure Delay object
     * @param post_delay 
     * @param DELAY_TIME_MS 
     */
    PressureDelay(StateId post_delay, uint32_t const DELAY_TIME_MS)
        : DELAY_TIME_MS_(DELAY_TIME_MS), post_delay_(post_delay) {}

    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::PRESSURE_DELAY; }

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(Calculator const &);

    /**
     * @brief todo
     */
    void onEntry() { start_time_ = Hal::now_ms(); }

  private:
    uint32_t const DELAY_TIME_MS_;
    Hal::t_point start_time_;
    const StateId post_delay_;
};

} // namespace State
