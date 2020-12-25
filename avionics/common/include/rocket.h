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

    // Rocket also acts as radio command receiver
    void operator()(uint8_t command) {
        uint32_t new_time_int = millis();
        auto state = state_machine.getState();
        switch (command) {
        case 'P':
            radio.sendStatus(new_time_int, init_status, sensors, ignitors);
            break;
        case 'A':
            state_machine.arm();
            break;
        case 'C':
            radio.sendConfig(new_time_int);
            break;
        case 'D':
            state_machine.disarm();
            break;
        case 0x30:
            radio.sendBulkSensor(new_time_int, calc.altitude(),
                                 sensors.accelerometer, sensors.imuSensor,
                                 sensors.gps, static_cast<uint8_t>(state));
            break;
        case 0x04:
            radio.sendGPS(new_time_int, sensors.gps);
            break;
        case 0x10:
            radio.sendSingleSensor(new_time_int, 0x10,
                                   sensors.accelerometer.getData()[0]);
            break;

        case 0x11:
            radio.sendSingleSensor(new_time_int, 0x11,
                                   sensors.accelerometer.getData()[1]);
            break;
        case 0x12:
            radio.sendSingleSensor(new_time_int, 0x12,
                                   sensors.accelerometer.getData()[2]);
            break;
        case 0x13:
            radio.sendSingleSensor(new_time_int, 0x13,
                                   sensors.barometer.getData()[0]);
            break;
        case 0x14:
            radio.sendSingleSensor(new_time_int, 0x14,
                                   sensors.barometer.getData()[1]);
            break;
        case 0x15:
            radio.sendSingleSensor(new_time_int, 0x15,
                                   sensors.temperature.getData()[0]);
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
            radio.sendSingleSensor(new_time_int, 0x19,
                                   sensors.gps.getData()[0]);
            break;
        case 0x1A:
            radio.sendSingleSensor(new_time_int, 0x1A,
                                   sensors.gps.getData()[1]);
            break;
        case 0x1B:
            radio.sendSingleSensor(new_time_int, 0x1B,
                                   sensors.gps.getData()[2]);
            break;
        case 0x1C:
            radio.sendSingleSensor(new_time_int, 0x1C, calc.altitude());
            break;
        case 0x1D:
            radio.sendState(new_time_int, static_cast<uint8_t>(state));
            break;
        case 0x1E:
            break; // Voltage sensor not implemented
        case 0x1F:
            radio.sendSingleSensor(new_time_int, 0x1F, calc.getBaseAltitude());
            break;
        default:
            break;
        }
    }
};

constexpr char Rocket::LOG_FILE_NAME[];
