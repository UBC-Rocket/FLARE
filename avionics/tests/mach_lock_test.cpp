#include "gtest/gtest.h"
#include "states/mach_lock.h"
#include "HAL/time.h"
#include "state_machine.h"

TEST(mach_lock_test, TimeOut) {
    static uint32_t initial_time = Hal::millis();
    StateInput *input = new StateInput();
    StateAuxilliaryInfo *state_aux = new StateAuxilliaryInfo();
    StateId curr_state;
    State::MachLock mach_lock = State::MachLock(10, 150);

    input->ignitor_good = false;
    input->altitude = 0;
    input->velocity_vertical = 151;
    curr_state = mach_lock.getNewState(*input, *state_aux);
    input->velocity_vertical = 149;

    for (int i = 0; i < 10; i++) {
        ASSERT_TRUE(curr_state == StateId::MACH_LOCK);
        curr_state = mach_lock.getNewState(*input, *state_aux);
    }

    ASSERT_TRUE(curr_state == StateId::ASCENT_TO_APOGEE);

    mach_lock = State::MachLock(10, 150);
    input->velocity_vertical = 151;
    ASSERT_TRUE(curr_state == StateId::MACH_LOCK);

    while ((Hal::millis() - initial_time) < (uint32_t) 30001) {
        curr_state = mach_lock.getNewState(*input, *state_aux);
    }

    ASSERT_TRUE(curr_state == StateId::ASCENT_TO_APOGEE);
}
 