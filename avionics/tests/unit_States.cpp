#include "gtest/gtest.h"

// Mock ignitor
#define HARDWARE_IGNITOR_HPP_A22E53F7BA764FAC8FA3CEAB90131013
struct Ignitor {
    void fire() { fired = true; }
    bool fired = false;
};

#include "states/ascent_to_apogee.h"
#include "states/repeated_checks.hpp"

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
    State::AscentToApogee state(StateId::PRESSURE_DELAY, StateId::MACH_LOCK, 5,
                                10, 200.0, iggy);
    state.onEntry();
    float t;
    for (t = -2; t <= 2; t += period) {
        StateId result = state.getNewState(data);
        if (result == StateId::PRESSURE_DELAY)
            break; // exited
        EXPECT_FALSE(iggy.fired);
        alt += period * vel;
        vel += period * grav;
    }
    EXPECT_TRUE(iggy.fired);
    EXPECT_TRUE(-1 < t && t < 1);
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

    constexpr float main_alt = 500;
    State::DrogueDescent<StateId::MAIN_DESCENT, 5> state(main_alt, iggy);

    alt = main_alt + 50;
    bool ok = false;
    while (alt > main_alt - 50) {
        StateId result = state.getNewState(data);
        if (result == StateId::MAIN_DESCENT) {
            ok = true;
            break; // exited
        }
        EXPECT_FALSE(iggy.fired);
        alt += vel * period;
    }
    EXPECT_TRUE(ok);
    EXPECT_TRUE(iggy.fired);
}
