#pragma once

/*Includes------------------------------------------------------------*/
#include "hardware/ignitor.h"
#include "state_interface.h"
#include "HAL/time.h"

namespace State {

class AscentToApogee : public IState {
  public:
    AscentToApogee(
        StateId post_apogee_id, 
        StateId mach_lock_id,
        uint8_t const APOGEE_CHECKS, 
        uint8_t const MACH_LOCK_CHECKS,
        float MACH_LOCK_VELOCITY, 
        Ignitor &drogue_ignitor, 
        Ignitor &redundant_drogue_ignitor
    )
    :   APOGEE_CHECKS_(APOGEE_CHECKS), 
        MACH_LOCK_CHECKS_(MACH_LOCK_CHECKS),
        MACH_LOCK_VELOCITY_(MACH_LOCK_VELOCITY), 
        post_apogee_id_(post_apogee_id), 
        mach_lock_id_(mach_lock_id), 
        drogue_ignitor_(drogue_ignitor), 
        redundant_drogue_ignitor_(redundant_drogue_ignitor) {}

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
            if (++mach_checks_ >= MACH_LOCK_CHECKS_) {
                return mach_lock_id_;
            }
        } else {
            mach_checks_ = 0;
        }

        if (input.altitude() < last_alt_) {
            apogee_checks_++;
        } else if (apogee_checks_ > 0) {
            apogee_checks_--;
        }
        last_alt_ = input.altitude();

        if (apogee_checks_ >= APOGEE_CHECKS_) {
            drogue_ignitor_.fire();
            redundant_drogue_ignitor_.fire();
            return post_apogee_id_;
        } else {
            return StateId::ASCENT_TO_APOGEE;
        }
    }

    void onEntry() override {
        apogee_checks_ = 0;
        mach_checks_ = 0;
        // Awkward to pass in first value - 0-initialize works with checks.
        last_alt_ = 0;
    }

  private:
    const uint8_t APOGEE_CHECKS_;
    const uint8_t MACH_LOCK_CHECKS_;
    const float MACH_LOCK_VELOCITY_;
    const StateId post_apogee_id_;
    const StateId mach_lock_id_;
    Ignitor &drogue_ignitor_;
    Ignitor &redundant_drogue_ignitor_;

    uint8_t apogee_checks_{0};
    uint8_t mach_checks_{0};
    float last_alt_{0};
};

} // namespace State
