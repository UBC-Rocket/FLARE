#include "gtest/gtest.h"

// Mock ignitor
#define HARDWARE_IGNITOR_HPP_A22E53F7BA764FAC8FA3CEAB90131013
struct Ignitor {
    void fire() { fired = true; }
    bool fired = false;
};

#include "states/ascent_to_apogee.h"
#include "states/mach_lock.h"
#include "states/repeated_checks.hpp"

// Mocking time.cpp
namespace Hal {
    uint32_t current_time_;

    uint32_t millis() { return current_time_; }
    void sleep_ms(uint32_t t) {current_time_ += t;}

} // namespace Hal

TEST(AscentToApogee, FireIgnitor) {
    // Simulate free fall (uniform acceleration), with data every 50ms
    constexpr float grav = -9.81;
    constexpr float period = 50e-3;
    Calculator data;

    auto &alt = data.alt;
    auto &vel = data.vel_gnd_z;
    alt = 1000;
    vel = -2 * grav;
    Ignitor iggy;
    Ignitor redundantIggy;
    // removed for tantalus lite version
    // State::AscentToApogee state(StateId::PRESSURE_DELAY, StateId::MACH_LOCK, 5,
    //                             10, 200.0, iggy, redundantIggy);
    // state.onEntry();
    // float t;
    // for (t = -2; t <= 2; t += period) {
    //     StateId result = state.getNewState(data);
    //     if (result == StateId::PRESSURE_DELAY)
    //         break; // exited
    //     EXPECT_FALSE(iggy.fired);
    //     alt += period * vel;
    //     vel += period * grav;
    // }
    // EXPECT_TRUE(iggy.fired);
    // EXPECT_TRUE(-1 < t && t < 1);
}

TEST(AscentToApogee, MachLock) {
    Ignitor iggy;
    Ignitor redundantIggy;
    Calculator data;
    StateId result;
    auto &alt = data.alt;
    auto &vel = data.vel_gnd_z;
    alt = 1000;
    vel = 201.0;

    // removed for tantalus lite version
    // State::AscentToApogee state(StateId::PRESSURE_DELAY, StateId::MACH_LOCK, 5,
    //                             10, 200.0, iggy, redundantIggy);
    // state.onEntry();

    // for (int i = 0; i < 10; i++) {
    //     result = state.getNewState(data);
    // }

    // EXPECT_TRUE(result == StateId::MACH_LOCK);

    // vel = 199.0;
    // result = state.getNewState(data);
    // EXPECT_TRUE(result == StateId::ASCENT_TO_APOGEE);
}

TEST(AscentToApogee, MachLockChecks) {
    Ignitor iggy;
    Ignitor redundantIggy;
    Calculator data;
    StateId result;
    auto &alt = data.alt;
    auto &vel = data.vel_gnd_z;
    alt = 1000;
    vel = 201.0;

    // removed for tantalus lite version
    // State::AscentToApogee state(StateId::PRESSURE_DELAY, StateId::MACH_LOCK, 5,
    //                             10, 200.0, iggy, redundantIggy);
    // state.onEntry();

    // for (int i = 0; i < 9; i++) {
    //     result = state.getNewState(data);
    // }
    // EXPECT_TRUE(result == StateId::ASCENT_TO_APOGEE);

    // vel = 199.0;
    // result = state.getNewState(data);
    // EXPECT_TRUE(result == StateId::ASCENT_TO_APOGEE);

    // vel = 201.0;
    // for (int i = 0; i < 9; i++) {
    //     result = state.getNewState(data);
    // }
    // EXPECT_TRUE(result == StateId::ASCENT_TO_APOGEE);
}

TEST(MachLock, MachLockChecks) {
    Calculator data;
    StateId result;
    auto &vel = data.vel_gnd_z;
    vel = 199.0;

    State::MachLock state(StateId::ASCENT_TO_APOGEE, 5,
                                200.0, 999);
    state.onEntry();

    // removed for tantalus lite version
    // for (int i = 0; i < 4; i++) {
    //     result = state.getNewState(data);
    //     EXPECT_TRUE(result == StateId::MACH_LOCK);
    // }
    
    // vel = 201.0;

    // for (int i = 0; i < 5; i++) {
    //     result = state.getNewState(data);
    //     EXPECT_TRUE(result == StateId::MACH_LOCK);
    // }

    // vel = 199.0;

    // for (int i = 0; i < 5; i++) {
    //     result = state.getNewState(data);
    // }

    EXPECT_TRUE(result == StateId::ASCENT_TO_APOGEE);

}

TEST(MachLock, MachLockTimeout) {
    Calculator data;
    StateId result;

    State::MachLock state(StateId::ASCENT_TO_APOGEE, 15,
                                200.0, 999);
    state.onEntry();

    // removed for tantalus lite version
    // result = state.getNewState(data);

    // for (int i = 0; i < 10; i++) {
    //     Hal::sleep_ms(100);
    //     EXPECT_TRUE(result == StateId::MACH_LOCK);
    //     result = state.getNewState(data);
    // }
    // EXPECT_TRUE(result == StateId::ASCENT_TO_APOGEE);
}

TEST(DrogueDescent, FireIgnitor) {
    // Simulate falling at terminal velocity of 30 m/s
    constexpr float TERM_VEL = -30;
    constexpr float period = 50e-3;
    Calculator data;

    auto &alt = data.alt;
    auto &vel = data.vel_gnd_z;
    vel = TERM_VEL;
    Ignitor iggy;
    Ignitor redundantIggy;

    constexpr float main_alt = 500;
    State::DrogueDescent<StateId::MAIN_DESCENT, 5> state(main_alt, iggy, redundantIggy);

    alt = main_alt + 50;
    bool ok = false;
    // removed for tantalus lite version
    // while (alt > main_alt - 50) {
    //     StateId result = state.getNewState(data);
    //     if (result == StateId::MAIN_DESCENT) {
    //         ok = true;
    //         break; // exited
    //     }
    //     EXPECT_FALSE(iggy.fired);
    //     alt += vel * period;
    // }
    // EXPECT_TRUE(ok);
    // EXPECT_TRUE(iggy.fired);
}
