#pragma once

/*Includes------------------------------------------------------------*/
#include "cameras.h"
#include "scheduler.hpp"
#include "state_interface.h"
#include "buzzer.h"

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
    StateId getNewState(Calculator const &) { return StateId::LANDED; }

    void onEntry() override {
      Scheduler::scheduleTask(Hal::now_ms() + Hal::ms(1), static_cast<int>(TaskID::BuzzerBeacon));
    }
};

} // namespace State
