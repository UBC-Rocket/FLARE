#include "states/mach_lock.h"

StateId State::MachLock::getNewState(const StateInput &input,
                                     StateAuxilliaryInfo &) {
    if (input.velocity_vertical < M_MACH_UNLOCK_VELOCITY) {
        if (++unlock_checks >= M_MACH_UNLOCK_CHECKS) {
            return ascent_state_;
        }
    } else {
        unlock_checks = 0;
    }
    return StateId::MACH_LOCK;
}
