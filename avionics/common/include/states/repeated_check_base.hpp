#ifndef STATES_REPEATED_CHECK_BASE_HPP_019EF79B53A4468F971024AD9E0A0449
#define STATES_REPEATED_CHECK_BASE_HPP_019EF79B53A4468F971024AD9E0A0449
#include <type_traits>
#include <utility>

#include "state_interface.h"
namespace State {

/**
 * @brief Template class for states that have a single exit point, which they go
 * to after some condition is checked a fixed number of times. Treat this as an
 * interface class, with the following methods that can be overridden (must be
 * visible to the base class, so either make them public or declare the base
 * class as a friend):
 *
 * bool accept(const StateInput& input) - returns true if input is acceptable.
 * Pure virtual.
 *
 * bool extraOnEntry() - runs on entry. Overriding is optional.
 * bool extraOnExit() - runs on exit. Overriding is optional.
 *
 * Additionally, there are some template parameters:
 * \tparam my_id StateId of this state.
 * \tparam next_id The StateId to exit to
 * \tparam num_checks Number of checks to perform
 * \tparam Derived - derived class name - see below
 *
 * How to use:
 * Inherit from this class, passing your own derived class's name as Derived.
 * E.g.
 *
 * class Standby : public RepeatedCheckBase<StateId::STANDBY, StateId::ASCENT,
 *      5, Standby> {
 * public:
 *     bool accept(){ ... }
 * };
 */
template <StateId my_id, StateId next_id, int num_checks, class Derived>
class RepeatedCheckBase : public IState {
  public:
    StateId getNewState(Calculator const &input, Hal::t_point const) final override {
        if (accept(input)) {
            ++checks_;
            if (checks_ >= num_checks) {
                static_cast<Derived *>(this)->extraOnExit();
                return next_id;
            }
        } else {
            checks_ = 0;
        }
        return my_id;
    }

    StateId getStateEnum(void) final override { return my_id; }

    void onEntry() final override {
        checks_ = 0;
        static_cast<Derived *>(this)->extraOnEntry();
    }

  protected:
    // Automatically get overrided, since it's Derived that's calling these
    // methods.
    void extraOnEntry() {}
    void extraOnExit() {}

  private:
    //   Constructor is private to help force correct usage of the CRTP pattern
    RepeatedCheckBase() : checks_(0) {}
    friend Derived;

    int checks_;

    /**
     * @brief Alias for underlying `accept` function.
     */
    bool accept(Calculator const &input) {
        return static_cast<Derived *>(this)->accept(input);
    }
};
} // namespace State

#endif
