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
    bool accept(Calculator const &input) { return input.altitude() > 10; };
};

TEST(RepeatedChecksState, Simple) {
    SimpleTestState state;
    Calculator input;
    input.alt = 5;

    // removed for tantalus lite version
    // for (int i = 0; i < kNumChecks * 10; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }

    // input.alt = 15;

    // for (int i = 1; i < kNumChecks; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }
    // EXPECT_EQ(state.getNewState(input), StateId::kExit);

    // // Suppose we re-enter state
    // state.onEntry();
    // for (int i = 1; i < kNumChecks; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }
    // EXPECT_EQ(state.getNewState(input), StateId::kExit);

    // // Re-enter state again
    // state.onEntry();
    // input.alt = 5;

    // for (int i = 0; i < kNumChecks * 10; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }

    // input.alt = 15;

    // for (int i = 1; i < kNumChecks; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }
    // EXPECT_EQ(state.getNewState(input), StateId::kExit);
}

TEST(RepeatedChecksState, Fluke) {
    SimpleTestState state;
    Calculator input;

    // removed for tantalus lite version
    // input.alt = 15;
    // EXPECT_EQ(state.getNewState(input), StateId::kSimple);

    // input.alt = 5;
    // EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // input.alt = 15;
    // for (int i = 1; i < kNumChecks; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }
    // EXPECT_EQ(state.getNewState(input), StateId::kExit);

    // state.onEntry();
    // for (int i = 1; i < kNumChecks; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }
    // input.alt = 5; // break chain
    // EXPECT_EQ(state.getNewState(input), StateId::kSimple);

    // input.alt = 15;
    // for (int i = 1; i < kNumChecks; ++i) {
    //     EXPECT_EQ(state.getNewState(input), StateId::kSimple);
    // }
    // EXPECT_EQ(state.getNewState(input), StateId::kExit);
}

class ExtraActionTestState
    : public State::RepeatedCheckBase<StateId::kSimple, StateId::kExit,
                                      kNumChecks, ExtraActionTestState> {

    friend State::RepeatedCheckBase<StateId::kSimple, StateId::kExit,
                                    kNumChecks, ExtraActionTestState>;
    bool accept(Calculator const &input) { return input.altitude() > 10; };

    void extraOnEntry() { enterCalled = true; }
    void extraOnExit() { exitCalled = true; }

  public:
    ExtraActionTestState() : enterCalled(false), exitCalled(false) {}
    bool enterCalled;
    bool exitCalled;
};
TEST(RepeatedChecksState, ExtraActions) {
    ExtraActionTestState state;
    Calculator input;

    state.onEntry();

    EXPECT_TRUE(state.enterCalled);
    state.enterCalled = false;

    // removed for tantalus lite version
    // input.alt = 15;
    // // almost but not quite exit
    // for (int i = 1; i < kNumChecks; ++i) {
    //     state.getNewState(input);
    // }
    // input.alt = 5; // reset checks
    // state.getNewState(input);

    // input.alt = 15;
    // for (int i = 1; i < kNumChecks; ++i) {
    //     state.getNewState(input);
    // }
    // EXPECT_FALSE(state.exitCalled);
    // state.getNewState(input);

    // EXPECT_TRUE(state.exitCalled);
    // EXPECT_FALSE(state.enterCalled);
}
