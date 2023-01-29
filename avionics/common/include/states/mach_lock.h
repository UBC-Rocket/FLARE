#pragma once

/*Includes------------------------------------------------------------*/
#include "hardware/ignitor.h"
#include "state_interface.h"
#include "HAL/time.h"

namespace State {

class MachLock : public IState {
  public:
    /**
     * @brief Construct a new Mach Lock object
     * @param next_id
     * @param MACH_UNLOCK_CHECKS number of mach_checks_ required before next
     * state
     * @param MACH_UNLOCK_VELOCITY maximum velocity before leaving MACH_LOCK
     * state
     * @param MACH_UNLOCK_TIME maximum amount of time before leaving MACH_LOCK
     * state' state
     */
    MachLock(StateId next_id, uint8_t const MACH_UNLOCK_CHECKS, 
            float MACH_UNLOCK_VELOCITY, uint32_t MACH_UNLOCK_TIME)
        : next_id_(next_id), MACH_UNLOCK_CHECKS_(MACH_UNLOCK_CHECKS),
          MACH_UNLOCK_VELOCITY_(MACH_UNLOCK_VELOCITY), MACH_UNLOCK_TIME_(MACH_UNLOCK_TIME) {}

    /**
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::ASCENT_TO_APOGEE; }

    /**
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @param input reference to input
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(Calculator const &input) {
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

    /**
     * @brief Initialize MachLock state
     * 
     */
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
