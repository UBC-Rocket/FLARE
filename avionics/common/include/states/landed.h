#pragma once

/*Includes------------------------------------------------------------*/
#include "cameras.h"
#include "scheduler.hpp"
#include "state_interface.h"

namespace State {

class Landed : public IState {
  public:
    Landed(Camera &cam) : cam_(cam) {}

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
    StateId getNewState(Calculator const &) { return StateId::LANDED; }

    void onEntry() override {
        constexpr static unsigned int LANDED_POLLING_INTERVAL = 5000;
        Scheduler::get_task(static_cast<int>(TaskID::ReadEvalLog)).period =
            Hal::ms(LANDED_POLLING_INTERVAL);

        Scheduler::unschedule(static_cast<int>(TaskID::RestartCamera));

        // NOTE: Status of camera is unknown at this point. Ensure that it is
        // turned off.
        cam_.stop_record();
    }

  private:
    Camera &cam_;
};

} // namespace State
