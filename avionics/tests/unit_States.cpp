#include "gtest/gtest.h"

// Mock ignitor
#define HARDWARE_IGNITOR_HPP_A22E53F7BA764FAC8FA3CEAB90131013
struct Ignitor {
    void fire() { fired = true; }
    bool fired = false;
};

// Mock StateId
#define STATE_ID_ENUM_HPP_765961F431844F469A0F1086D132018F
enum class StateId {
    ASCENT_TO_APOGEE,
    MACH_LOCK,
    PRESSURE_DELAY,
};

// Mock (or rather have a consistent) state input struct
#define STATE_INPUT_STRUCT_HPP_4A01F791ECBA4E3CB8BC12436C41ABC9
struct StateInput {
    float altitude;          // metres
    float velocity_vertical; // m/s, vertical
};

struct StateAuxilliaryInfo {};

#include "states/ascent_to_apogee.h"

TEST(AscentToApogee, FireIgnitor) {
    // Simulate free fall (uniform acceleration), with data every 50ms
    constexpr float grav = -9.81;
    constexpr float period = 50e-3;
    StateInput data;
    StateAuxilliaryInfo aux;
    auto &alt = data.altitude;
    auto &vel = data.velocity_vertical;
    alt = 1000;
    vel = -2 * grav;
    Ignitor iggy;
    State::AscentToApogee state(StateId::PRESSURE_DELAY, StateId::MACH_LOCK, 5,
                                10, 200.0, iggy);
    state.onEntry();
    float t;
    for (t = -2; t <= 2; t += period) {
        StateId result = state.getNewState(data, aux);
        if (result == StateId::PRESSURE_DELAY)
            break; // exited
        EXPECT_FALSE(iggy.fired);
        alt += period * vel;
        vel += period * grav;
    }
    EXPECT_TRUE(iggy.fired);
    EXPECT_TRUE(-1 < t && t < 1);
}
