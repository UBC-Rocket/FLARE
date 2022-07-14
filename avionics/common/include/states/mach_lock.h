#pragma once

/*Includes------------------------------------------------------------*/
#include "hardware/ignitor.h"
#include "state_interface.h"
#include "HAL/time.h"

namespace State {

class MachLock : public IState {
  public:
    MachLock(StateId next_id, uint8_t const MACH_UNLOCK_CHECKS, 
            float MACH_UNLOCK_VELOCITY, uint32_t MACH_UNLOCK_TIME)
        : next_id_(next_id), MACH_UNLOCK_CHECKS_(MACH_UNLOCK_CHECKS),
          MACH_UNLOCK_VELOCITY_(MACH_UNLOCK_VELOCITY), MACH_UNLOCK_TIME_(MACH_UNLOCK_TIME) {}

    /*
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::ASCENT_TO_APOGEE; }

    /*
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(Calculator const &input, Hal::t_point const) {
        if (input.velocityGroundZ() < MACH_UNLOCK_VELOCITY_) {
            ++mach_checks_;
            if (mach_checks_ >= MACH_UNLOCK_CHECKS_) {
                return next_id_;
            }
        } else {
            mach_checks_ = 0;
        }
        if (Hal::millis() - initial_time_ > MACH_UNLOCK_TIME_) {
            return next_id_;
        }
        else {
            return StateId::MACH_LOCK;
        }
    }

    void onEntry() override {
        mach_checks_ = 0;
        initial_time_ = Hal::millis();
    }

  private:
    const StateId next_id_;
    const uint8_t MACH_UNLOCK_CHECKS_;
    const float MACH_UNLOCK_VELOCITY_;
    const uint32_t MACH_UNLOCK_TIME_;

    uint8_t mach_checks_{0};
    uint32_t initial_time_{0};

};

} // namespace State
