#pragma once

/*Includes------------------------------------------------------------*/
#include "scheduler.hpp"
#include "state_input_struct.h"
#include "state_interface.h"

namespace State {

class Landed : public IState {
  public:
    Landed() {}

    /*
     * @brief Return the assigned enumeration code.
     * @return Enumeration code.
     */
    StateId getStateEnum(void) { return StateId::LANDED; }

    /*
     * @brief Return the next state, based on input data (mostly from filtered
     * sensor data)
     * @return State enumeration code, to be passed into the std::map between
     * codes and used states. Note that the returned code may be the same state.
     */
    StateId getNewState(const StateInput &, StateAuxilliaryInfo &) {
        return StateId::LANDED;
    }

    void onEntry() override {
        // TODO: Turn off cameras and camera cycling task
        constexpr static unsigned int LANDED_POLLING_INTERVAL = 5000;
        Scheduler::get_task(static_cast<int>(TaskID::ReadEvalLog)).period =
            Hal::ms(LANDED_POLLING_INTERVAL);
    }
};

} // namespace State
