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
        auto &datalog = rocket_.datalog;
        Calculator *calc = &rocket_.calc;

        // ensures that if state somehow gets accidentally changed, state
        // reverts to aborted
        if (init_status == RocketStatus::CRITICAL_FAILURE) {
            state_machine.abort();
        }

        {
            const StateId state = state_machine.getState();
            sensors.poll();

            if(state == StateId::ARMED || state == StateId::STANDBY) {
                calc = &rocket_.calc_standby_armed;
            }
            else {
                calc = &rocket_.calc_other;
            }

            calc->update(sensors.last_poll_time());
            datalog.logData(Hal::tpoint_to_uint(sensors.last_poll_time()),
                            sensors, state, calc->altitude(),
                            calc->altitudeBase());
        }

        StateId old_state, new_state;
        std::tie(old_state, new_state) = state_machine.update(*calc);
        if (old_state != new_state) {
            // state transition has occurred
            if (new_state == StateId::MAIN_DESCENT) {
                // restart the camera periodically once in main descent
                Scheduler::scheduleTask(
                    Hal::now_ms() + Hal::ms(CAMERA_FIRST_POWEROFF_DELAY_MS),
                    static_cast<int>(TaskID::RestartCamera));

            } else if (new_state == StateId::LANDED) {
                // stop restarting the camera once in landed, and slow down
                // polling interval
                Scheduler::unschedule(static_cast<int>(TaskID::RestartCamera));

                // NOTE: Status of camera is unknown at this point. Ensure that
                // it is turned off.
                rocket_.cam.stop_record();

                constexpr static unsigned int LANDED_POLLING_INTERVAL = 5000;
                Scheduler::get_task(static_cast<int>(TaskID::ReadEvalLog))
                    .period = Hal::ms(LANDED_POLLING_INTERVAL);
            }
        }

        Radio::forwardCommand(command_receiver);
    }

  public:
    ReadEvalLog(Rocket &rocket) : rocket_(rocket), command_receiver(rocket) {}
    static void run(void *self) {
        reinterpret_cast<ReadEvalLog *>(self)->run();
    }
};

/**
 * @brief Responsible for periodically sending bulk sensor data through
 * radio
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
