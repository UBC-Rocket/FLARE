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
#include "state_machine.h"

struct Rocket {
  public:
    static constexpr char LOG_FILE_NAME[] = "datalog.csv";

    Rocket()
        : cam(Hal::SerialInst::Camera), datalog(LOG_FILE_NAME),
          init_status(collectStatus(sensors, ignitors)),
          radio(Hal::SerialInst::Radio), calc(sensors) {}

    // WARNING - MEMBER ORDER DEPENDENCY
    // https://isocpp.org/wiki/faq/ctors#order-dependency-in-members
    Buzzer buzzer;
    Camera cam;
    SensorCollection sensors;
    IgnitorCollection ignitors;
    CSVWrite<CSVWriteImpl> datalog;
    RocketStatus init_status;
    RadioController radio;
    Calculator calc;
    StateMachine state_machine;
};
class CommandReceiver {
  private:
    Rocket &rocket_;

  public:
    CommandReceiver(Rocket &rocket) : rocket_(rocket) {}

    void run_command(uint8_t command) {
        // Sensor-related data should inform based on the last sensor poll time
        uint32_t sensor_poll_time =
            Hal::tpoint_to_uint(rocket_.sensors.last_poll_time());

        auto state = rocket_.state_machine.getState();
        switch (command) {
        case 'P':
            rocket_.radio.sendStatus(millis(), rocket_.init_status,
                                     rocket_.sensors, rocket_.ignitors);
            break;
        case 'A':
            rocket_.state_machine.arm();
            break;
        case 'C':
            rocket_.radio.sendConfig(millis());
            break;
        case 'D':
            rocket_.state_machine.disarm();
            break;
        case 0x30:
            rocket_.radio.sendBulkSensor(
                sensor_poll_time, rocket_.calc.altitude(),
                rocket_.sensors.accelerometer, rocket_.sensors.imuSensor,
                rocket_.sensors.gps, static_cast<uint8_t>(state));
            break;
        case 0x04:
            rocket_.radio.sendGPS(sensor_poll_time, rocket_.sensors.gps);
            break;
        case 0x10:
            rocket_.radio.sendSingleSensor(
                sensor_poll_time, 0x10,
                rocket_.sensors.accelerometer.getData()[0]);
            break;

        case 0x11:
            rocket_.radio.sendSingleSensor(
                sensor_poll_time, 0x11,
                rocket_.sensors.accelerometer.getData()[1]);
            break;
        case 0x12:
            rocket_.radio.sendSingleSensor(
                sensor_poll_time, 0x12,
                rocket_.sensors.accelerometer.getData()[2]);
            break;
        case 0x13:
            rocket_.radio.sendSingleSensor(
                sensor_poll_time, 0x13, rocket_.sensors.barometer.getData()[0]);
            break;
        case 0x14:
            rocket_.radio.sendSingleSensor(
                sensor_poll_time, 0x14, rocket_.sensors.barometer.getData()[1]);
            break;
        case 0x15:
            rocket_.radio.sendSingleSensor(
                sensor_poll_time, 0x15,
                rocket_.sensors.temperature.getData()[0]);
            break;
        case 0x16:
            break;
            // Not implemented - still uncertain about IMU storage
        case 0x17:
            break;
            // Not implemented - still uncertain about IMU storage
        case 0x18:
            break;
            // Not implemented - still uncertain about IMU storage
        case 0x19:
            rocket_.radio.sendSingleSensor(sensor_poll_time, 0x19,
                                           rocket_.sensors.gps.getData()[0]);
            break;
        case 0x1A:
            rocket_.radio.sendSingleSensor(sensor_poll_time, 0x1A,
                                           rocket_.sensors.gps.getData()[1]);
            break;
        case 0x1B:
            rocket_.radio.sendSingleSensor(sensor_poll_time, 0x1B,
                                           rocket_.sensors.gps.getData()[2]);
            break;
        case 0x1C:
            rocket_.radio.sendSingleSensor(sensor_poll_time, 0x1C,
                                           rocket_.calc.altitude());
            break;
        case 0x1D:
            rocket_.radio.sendState(sensor_poll_time,
                                    static_cast<uint8_t>(state));
            break;
        case 0x1E:
            break; // Voltage sensor not implemented
        case 0x1F:
            rocket_.radio.sendSingleSensor(sensor_poll_time, 0x1F,
                                           rocket_.calc.getBaseAltitude());
            break;
        default:
            break;
        }
    }
};

constexpr char Rocket::LOG_FILE_NAME[];
