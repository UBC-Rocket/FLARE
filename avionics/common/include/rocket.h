/**
 * Overarching object that carries different components.
 */
#pragma once

#include "CSVWriteImpl.h"
#include "CSVwrite.h"
#include "buzzer.h"
#include "calculations.h"
#include "cameras.h"
#include "radio.h"
#include "sensor_collection.h"
#include "state_machine.hpp"
#include "states_config.hpp"

#include "HAL/time.h"

struct Rocket {
  public:
    static constexpr char LOG_FILE_NAME[] = "datalog.csv";

    Rocket()
        : cam(Hal::SerialInst::Camera), datalog(LOG_FILE_NAME),
          init_status(collectStatus(sensors, ignitors)),
          calc(sensors, Hal::now_ms()), config(calc, ignitors, cam),
          state_machine(config.state_map, config.initial_state) {}

    // WARNING - MEMBER ORDER DEPENDENCY
    // https://isocpp.org/wiki/faq/ctors#order-dependency-in-members
    Buzzer buzzer;
    Camera cam;
    SensorCollection sensors;
    IgnitorCollection ignitors;
    CSVWrite<CSVWriteImpl> datalog;
    RocketStatus init_status;
    Calculator calc;
    StateMachineConfig config;
    StateMachine state_machine;
};
class CommandReceiver {
  private:
    Rocket &rocket_;

  public:
    CommandReceiver(Rocket &rocket) : rocket_(rocket) {}

    void run_command(Radio::command_t command) {
        // Sensor-related data should inform based on the last sensor poll time
        uint32_t sensor_poll_time =
            Hal::tpoint_to_uint(rocket_.sensors.last_poll_time());

        auto state = rocket_.state_machine.getState();
        switch (command) {
        case Radio::poll:
            Radio::sendStatus(Hal::millis(), rocket_.init_status,
                              rocket_.sensors, rocket_.ignitors);
            break;
        case Radio::arm:
            rocket_.state_machine.arm();
            rocket_.cam.start_record();
            break;

        case Radio::disarm:
            rocket_.state_machine.disarm();
            rocket_.cam.stop_record();
            break;
        case Radio::bulk_sensor:
            // removed for lite version
            // Radio::sendBulkSensor(
            //     sensor_poll_time, rocket_.calc.altitude(),
            //     rocket_.sensors.accelerometer, rocket_.sensors.imuSensor,
            //     rocket_.sensors.gps, static_cast<uint16_t>(state));
            break;
        case Radio::gps:
            // removed for lite version
            // Radio::sendGPS(sensor_poll_time, rocket_.sensors.gps);
            break;
        case Radio::acceleration_x:
            Radio::sendSingleSensor(sensor_poll_time, Radio::acceleration_x,
                                    rocket_.sensors.accelerometer.getData()[0]);
            break;

        case Radio::acceleration_y:
            Radio::sendSingleSensor(sensor_poll_time, Radio::acceleration_y,
                                    rocket_.sensors.accelerometer.getData()[1]);
            break;
        case Radio::acceleration_z:
            Radio::sendSingleSensor(sensor_poll_time, Radio::acceleration_z,
                                    rocket_.sensors.accelerometer.getData()[2]);
            break;
        case Radio::pressure:
            Radio::sendSingleSensor(sensor_poll_time, Radio::pressure,
                                    rocket_.sensors.barometer.getData()[0]);
            break;
        case Radio::barometer_temperature:
            Radio::sendSingleSensor(sensor_poll_time, Radio::barometer_temperature,
                                    rocket_.sensors.barometer.getData()[1]);
            break;
        case Radio::temperature:
            // removed for lite version
            // Radio::sendSingleSensor(sensor_poll_time, Radio::temperature,
            //                         rocket_.sensors.temperature.getData()[0]);
            break;
        case Radio::unused1:
            break;
            // Not implemented - still uncertain about IMU storage
        case Radio::unused2:
            break;
            // Not implemented - still uncertain about IMU storage
        case Radio::unused3:
            break;
            // Not implemented - still uncertain about IMU storage
        case Radio::latitude:
            // removed for lite version
            // Radio::sendSingleSensor(sensor_poll_time, Radio::latitude,
            //                         rocket_.sensors.gps.getData()[0]);
            break;
        case Radio::longitude:
            // removed for lite version
            // Radio::sendSingleSensor(sensor_poll_time, Radio::longitude,
            //                         rocket_.sensors.gps.getData()[1]);
            break;
        case Radio::gps_altitude:
            // removed for lite version
            // Radio::sendSingleSensor(sensor_poll_time, Radio::gps_altitude,
            //                         rocket_.sensors.gps.getData()[2]);
            break;
        case Radio::calculated_altitude:
            Radio::sendSingleSensor(sensor_poll_time, Radio::calculated_altitude,
                                    rocket_.calc.altitude());
            break;
        case Radio::send_state:
            Radio::sendState(sensor_poll_time, static_cast<uint16_t>(state));
            break;
        case Radio::voltage:
            // removed for lite version
            // Radio::sendSingleSensor(sensor_poll_time, Radio::voltage,
            //                         rocket_.sensors.battery.getData()[0]);
            break;
        case Radio::ground_altitude:
            Radio::sendSingleSensor(sensor_poll_time, Radio::ground_altitude,
                                    rocket_.calc.altitudeBase());
            break;
        default:
            // TODO: Log invalid command
            break;
        }
    }
};
