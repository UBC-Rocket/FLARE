#include "states/ascent_to_apogee.h"

StateId State::AscentToApogee::getNewState(const StateInput &input,
                                           StateAuxilliaryInfo &) {
    static uint8_t apogee_checks = 0;
    static uint8_t mach_checks = 0;
    static float last_alt = input.altitude;

    if (input.velocity_vertical > M_MACH_LOCK_VELOCITY) {
        if (++mach_checks >= M_MACH_LOCK_CHECKS) {
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

    if (apogee_checks >= M_APOGEE_CHECKS) {
        return post_apogee_id_;
    } else {
        return StateId::ASCENT_TO_APOGEE;
    }
}
