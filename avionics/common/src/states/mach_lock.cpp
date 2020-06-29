#include "states/mach_lock.h"

StateId State::MachLock::getNewState(const StateInput &input,
                                     StateAuxilliaryInfo &) {
    static uint8_t unlock_checks = 0;
    if (input.velocity_vertical < M_MACH_UNLOCK_VELOCITY) {
        if (++unlock_checks >= M_MACH_UNLOCK_CHECKS) {
            return StateId::ASCENT_TO_APOGEE;
        }
    } else {
        unlock_checks = 0;
    }
    return StateId::MACH_LOCK;
}
