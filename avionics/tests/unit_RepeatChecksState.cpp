#include "gtest/gtest.h"

// Take advantage of the header guard to inject our own StateIds
#define STATE_ID_ENUM_HPP_765961F431844F469A0F1086D132018F
enum class StateId { kSimple, kExit };

#include "states/repeated_check_base.hpp"

constexpr int kNumChecks = 5;
class SimpleTestState
    : public State::RepeatedCheckBase<StateId::kSimple, StateId::kExit,
                                      kNumChecks, SimpleTestState> {

    friend State::RepeatedCheckBase<StateId::kSimple, StateId::kExit,
                                    kNumChecks, SimpleTestState>;
    bool accept(const StateInput &input) { return input.altitude > 10; };
};

TEST(RepeatedChecksState, Simple) {
    SimpleTestState state;
    StateInput input;
    StateAuxilliaryInfo aux;
    input.altitude = 5;

    for (int i = 0; i < kNumChecks * 10; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }

    input.altitude = 15;

    for (int i = 1; i < kNumChecks; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }
    EXPECT_EQ(state.getNewState(input, aux), StateId::kExit);

    // Suppose we re-enter state
    state.onEntry();
    for (int i = 1; i < kNumChecks; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }
    EXPECT_EQ(state.getNewState(input, aux), StateId::kExit);

    // Re-enter state again
    state.onEntry();
    input.altitude = 5;

    for (int i = 0; i < kNumChecks * 10; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }

    input.altitude = 15;

    for (int i = 1; i < kNumChecks; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }
    EXPECT_EQ(state.getNewState(input, aux), StateId::kExit);
}

TEST(RepeatedChecksState, Fluke) {
    SimpleTestState state;
    StateInput input;
    StateAuxilliaryInfo aux;
    input.altitude = 15;
    EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);

    input.altitude = 5;
    EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    input.altitude = 15;
    for (int i = 1; i < kNumChecks; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }
    EXPECT_EQ(state.getNewState(input, aux), StateId::kExit);

    state.onEntry();
    for (int i = 1; i < kNumChecks; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }
    input.altitude = 5; // break chain
    EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);

    input.altitude = 15;
    for (int i = 1; i < kNumChecks; ++i) {
        EXPECT_EQ(state.getNewState(input, aux), StateId::kSimple);
    }
    EXPECT_EQ(state.getNewState(input, aux), StateId::kExit);
}

class ExtraActionTestState
    : public State::RepeatedCheckBase<StateId::kSimple, StateId::kExit,
                                      kNumChecks, ExtraActionTestState> {

    friend State::RepeatedCheckBase<StateId::kSimple, StateId::kExit,
                                    kNumChecks, ExtraActionTestState>;
    bool accept(const StateInput &input) { return input.altitude > 10; };

    void extraOnEntry() { enterCalled = true; }
    void extraOnExit() { exitCalled = true; }

  public:
    ExtraActionTestState() : enterCalled(false), exitCalled(false) {}
    bool enterCalled;
    bool exitCalled;
};
TEST(RepeatedChecksState, ExtraActions) {
    ExtraActionTestState state;
    StateInput input;
    StateAuxilliaryInfo aux;
    state.onEntry();

    EXPECT_TRUE(state.enterCalled);
    state.enterCalled = false;

    input.altitude = 15;
    // almost but not quite exit
    for (int i = 1; i < kNumChecks; ++i) {
        state.getNewState(input, aux);
    }
    input.altitude = 5; // reset checks
    state.getNewState(input, aux);

    input.altitude = 15;
    for (int i = 1; i < kNumChecks; ++i) {
        state.getNewState(input, aux);
    }
    EXPECT_FALSE(state.exitCalled);
    state.getNewState(input, aux);

    EXPECT_TRUE(state.exitCalled);
    EXPECT_FALSE(state.enterCalled);
}
