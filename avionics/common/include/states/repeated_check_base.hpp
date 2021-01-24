#ifndef STATES_REPEATED_CHECK_BASE_HPP_019EF79B53A4468F971024AD9E0A0449
#define STATES_REPEATED_CHECK_BASE_HPP_019EF79B53A4468F971024AD9E0A0449
#include <type_traits>
#include <utility>

#include "state_interface.h"
namespace State {

/**
 * @brief Template class for states that have a single exit point, which they go
 * to after some condition is checked a fixed number of times. Treat this as an
 * interface class, with the following methods that can be overridden:
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
 * class Standby : RepeatedCheckBase<StateId::STANDBY, StateId::ASCENT, 5,
 * Standby> {
 *     bool accept(){ ... }
 * };
 */
template <StateId my_id, StateId next_id, int num_checks, class Derived>
class RepeatedCheckBase : public IState {
  public:
    StateId getNewState(StateInput const &input,
                        StateAuxilliaryInfo &) final override {
        if (accept(input)) {
            ++checks_;
            if (checks_ >= num_checks) {
                optionalOnExit();
                return next_id;
            }
        } else {
            checks_ = 0;
        }
        return my_id;
    }

    void onEntry() final override {
        checks_ = 0;
        optionalOnEntry();
    }

  private:
    //   Constructor is private to help force correct usage of the CRTP pattern
    RepeatedCheckBase() : checks_(0) {}
    friend class Derived;

    int checks_;

    /**
     * @brief Alias for underlying `accept` function.
     */
    bool accept(const StateInput &input) {
        return static_cast<Derived *>(this)->accept(input);
    }

    /**
     * The optional functions use SFINAE to detect whether extraEntry() and
     *      extraExit() exist.
     *
     * By the way CRTP works, this class is instantiated while Derived
     * is still an incomplete type: (below snippet from Wikipedia)
     *     class Derived : public Base<Derived> {
     *                            ^^^^^^^^^^^^^
     * So, checking needs to be deferred until Base gets instantiated; this
     * means we can't directly (immediately) use an SFINAE error to try to
     * detect extra* - the compiler doesn't know if that method exists yet,
     * since Derived is incomplete.
     *
     * To get around this, some indirection needs to be used, hence the helper
     * checking types. See stackoverflow.com/questions/13401716 for why dummy
     * types need to be used.
     */

    /**
     * Default version (false)
     */
    template <typename CheckedType, typename Dummy = void>
    class HasOnEntry : public std::false_type {};

    /**
     * Specialized variant (true)
     */
    template <typename CheckedType>
    class HasOnEntry<CheckedType, decltype(&CheckedType::extraOnEntry, void())>
        : public std::true_type {};

    /**
     * @brief No-op default version.
     */
    template <class Dummy = Derived>
    typename std::enable_if<!HasOnEntry<Dummy>::value>::type
    optionalExtraEntry() {}

    /**
     * @brief Used if extraOnEntry exists.
     */
    template <class Dummy = Derived>
    typename std::enable_if<HasOnEntry<Dummy>::value>::type
    optionalExtraEntry() {
        static_cast<Derived *>(this)->extraOnEntry();
    }

    /**
     * Default version (false)
     */
    template <typename CheckedType, typename Dummy = void>
    class HasOnExit : public std::false_type {};

    /**
     * Specialized variant (true)
     */
    template <typename CheckedType>
    class HasOnExit<CheckedType, decltype(&CheckedType::extraOnExit, void())>
        : public std::true_type {};

    /**
     * @brief No-op default version.
     */
    template <class Dummy = Derived>
    typename std::enable_if<!HasOnExit<Dummy>::value>::type
    optionalExtraExit() {}

    /**
     * @brief Used if extraOnEntry exists.
     */
    template <class Dummy = Derived>
    typename std::enable_if<HasOnExit<Dummy>::value>::type optionalExtraExit() {
        static_cast<Derived *>(this)->extraOnExit();
    }
};
} // namespace State

#endif
