#ifndef MAIN_TASKS_HPP_6FADFAE876EC4E6D8B5A9394585FE936
#define MAIN_TASKS_HPP_6FADFAE876EC4E6D8B5A9394585FE936
#include "radio.h"
#include "rocket.h"

/**
 * @brief Main sensor read, state estimation, run state machine, log data loop
 */
class ReadEvalLog {
  private:
    Rocket &rocket_;
    CommandReceiver command_receiver;

    constexpr static unsigned int NOMINAL_POLLING_TIME_INTERVAL = 50; // ms
    // MainDescent state updates this to 5000 ms - see main_descent.cpp

    void run() {
        auto &state_machine = rocket_.state_machine;
        auto &init_status = rocket_.init_status;
        auto &sensors = rocket_.sensors;
        auto &calc = rocket_.calc;
        auto &datalog = rocket_.datalog;

        // ensures that if state somehow gets accidentally changed, state
        // reverts to aborted
        if (init_status == RocketStatus::CRITICAL_FAILURE) {
            state_machine.abort();
        }

        StateId state = state_machine.getState();

        sensors.poll();
        calc.update(state, sensors.last_poll_time());
        state_machine.update(calc);
        datalog.logData(Hal::tpoint_to_uint(sensors.last_poll_time()), sensors,
                        state, calc.altitude(), calc.altitudeBase());

        Radio::forwardCommand(command_receiver);
    }

  public:
    ReadEvalLog(Rocket &rocket) : rocket_(rocket), command_receiver(rocket) {}
    static void run(void *self) {
        reinterpret_cast<ReadEvalLog *>(self)->run();
    }
};

/**
 * @brief Responsible for periodically sending bulk sensor data through radio
 */
class RadioTxBulk {
  public:
    static void run(void *self) {
        reinterpret_cast<RadioTxBulk *>(self)->run();
    }

    RadioTxBulk(Rocket &rkt) : rocket(rkt) {}
    static constexpr Hal::ms freq{500};

  private:
    void run() {
        StateId state = rocket.state_machine.getState();
        Radio::sendBulkSensor(
            Hal::tpoint_to_uint(rocket.sensors.last_poll_time()),
            rocket.calc.altitude(), rocket.sensors.accelerometer,
            rocket.sensors.imuSensor, rocket.sensors.gps,
            static_cast<uint16_t>(state));
    }

    Rocket &rocket;
};

#endif
