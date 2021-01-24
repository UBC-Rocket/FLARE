#include "states/ascent_to_apogee.h"

StateId State::AscentToApogee::getNewState(const StateInput &input,
                                           StateAuxilliaryInfo &) {

    if (input.velocity_vertical > MACH_LOCK_VELOCITY_) {
        if (++mach_checks >= MACH_LOCK_CHECKS_) {
            return mach_lock_id_;
        }
    } else {
        mach_checks = 0;
    }

    if (input.altitude < last_alt) {
        apogee_checks++;
    } else if (apogee_checks > 0) {
        apogee_checks--;
    }
    last_alt = input.altitude;

    if (apogee_checks >= APOGEE_CHECKS_) {
        drogue_ignitor_.fire();
        return post_apogee_id_;
    } else {
        return StateId::ASCENT_TO_APOGEE;
    }
}
