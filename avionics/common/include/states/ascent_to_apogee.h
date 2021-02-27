#pragma once

/*Includes------------------------------------------------------------*/
#include "hardware/ignitor.h"
#include "state_interface.h"
#include "HAL/time.h"

namespace State {

class AscentToApogee : public IState {
  public:
    AscentToApogee(StateId post_apogee_id, StateId mach_lock_id,
                   uint8_t const APOGEE_CHECKS, uint8_t const MACH_LOCK_CHECKS,
                   float MACH_LOCK_VELOCITY, uint32_t MACH_LOCK_TIME, Ignitor &drogue_ignitor)
        : APOGEE_CHECKS_(APOGEE_CHECKS), MACH_LOCK_CHECKS_(MACH_LOCK_CHECKS),
          MACH_LOCK_VELOCITY_(MACH_LOCK_VELOCITY), MACH_LOCK_TIME_(MACH_LOCK_TIME),
          post_apogee_id_(post_apogee_id), mach_lock_id_(mach_lock_id),
          drogue_ignitor_(drogue_ignitor) {}

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
    StateId getNewState(Calculator const &input) {
        if (input.velocityGroundZ() > MACH_LOCK_VELOCITY_) {
            if (++mach_checks >= MACH_LOCK_CHECKS_) {
                return mach_timeout();
            }
        } else {
            mach_checks = 0;
        }

        if (input.altitude() < last_alt) {
            apogee_checks++;
        } else if (apogee_checks > 0) {
            apogee_checks--;
        }
        last_alt = input.altitude();

        if (apogee_checks >= APOGEE_CHECKS_) {
            drogue_ignitor_.fire();
            return post_apogee_id_;
        } else {
            return StateId::ASCENT_TO_APOGEE;
        }
    }

    void onEntry() override {
        apogee_checks = 0;
        mach_checks = 0;
        initial_time = 0;
        // Awkward to pass in first value - 0-initialize works with checks.
        last_alt = 0;
    }

    StateId mach_timeout() {
        if (mach_checks == MACH_LOCK_CHECKS_) {
            initial_time = Hal::millis();
            return mach_lock_id_;
        }
        else if (Hal::millis() - initial_time > MACH_LOCK_TIME_) {
            mach_checks = 0;
            return StateId::ASCENT_TO_APOGEE;
        }
        return mach_lock_id_;
    }

  private:
    const uint8_t APOGEE_CHECKS_;
    const uint8_t MACH_LOCK_CHECKS_;
    const float MACH_LOCK_VELOCITY_;
    const uint32_t MACH_LOCK_TIME_;
    const StateId post_apogee_id_;
    const StateId mach_lock_id_;
    Ignitor &drogue_ignitor_;

    uint8_t apogee_checks{0};
    uint8_t mach_checks{0};
    uint32_t initial_time{0};
    float last_alt{0};
};

} // namespace State
