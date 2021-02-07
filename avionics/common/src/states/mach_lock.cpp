#include "states/mach_lock.h"
#include "HAL/time.h"

StateId State::MachLock::getNewState(const StateInput &input,
                                     StateAuxilliaryInfo &) {
    static uint8_t unlock_checks = 0;
    static uint32_t initial_time = Hal::millis();
    if (input.velocity_vertical < M_MACH_UNLOCK_VELOCITY) {
        if (++unlock_checks >= M_MACH_UNLOCK_CHECKS) {
            return StateId::ASCENT_TO_APOGEE;
        }
    } else {
        unlock_checks = 0;
    }

    if ((Hal::millis() - initial_time) > (uint32_t) 30000) {
        return StateId::ASCENT_TO_APOGEE;
    } 
    return StateId::MACH_LOCK;
}
