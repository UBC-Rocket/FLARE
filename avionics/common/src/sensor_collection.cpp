#include "sensor_collection.h"
#include "log.hpp"
#include "options.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif

constexpr char SensorCollection::LOG_FILE_HEADER[];

SensorCollection::SensorCollection()
    : barometer(BEGIN + BAROMETER_INDEX),
      gps(Hal::SerialInst::GPS, BEGIN + GPS_INDEX),
      accelerometer(BEGIN + ACCEL_INDEX), 
      imuSensor(BEGIN + IMU_INDEX),
    //   battery(Pin::VOLTAGE_SENSOR, BEGIN + BATTERY_INDEX), // No voltage sensor for 2022/23
      temperature(BEGIN + TEMP_INDEX) {
    
    updateStatus();
}

void SensorCollection::poll() {
    last_poll_time_ = Hal::now_ms();
    barometer.readData();
    gps.readData();
    accelerometer.readData();
    imuSensor.readData();
    // battery.readData(); // No voltage sensor for 2022/23
    temperature.readData();
}

RocketStatus SensorCollection::getStatus(bool refresh) {
    if (refresh) {
        updateStatus();
    }
    return status_;
}

void SensorCollection::updateStatus() {
    status_ = RocketStatus::NOMINAL;
    status_bitfield_[0] = 0;
    if (barometer.getStatus() == SensorStatus::FAILURE) {
        #ifdef TESTING
        Serial.println("Barometer failed");
        #endif

        LOG_ERROR("Barometer failed");
        raiseToStatus(status_, RocketStatus::CRITICAL_FAILURE);
        *status_bitfield_ |= 0x80;
    }

    if (gps.getStatus() == SensorStatus::FAILURE) {
        #ifdef TESTING
        Serial.println("GPS failed");
        #endif

        LOG_WARN("GPS failed");
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x40;
    }
    if (accelerometer.getStatus() == SensorStatus::FAILURE) {
        #ifdef TESTING
        Serial.println("Accelerometer failed");
        #endif

        LOG_WARN("Accelerometer failed");
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x20;
    }
    if (imuSensor.getStatus() == SensorStatus::FAILURE) {
        #ifdef TESTING
        Serial.println("IMU failed");
        #endif

        LOG_WARN("IMU failed");
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x10;
    }
    if (temperature.getStatus() == SensorStatus::FAILURE) {
        #ifdef TESTING
        Serial.println("Temperature sensor failed");
        #endif

        LOG_WARN("Temperature sensor failed");
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x08;
    }
    // if (battery.getStatus() == SensorStatus::FAILURE) {
    //     #ifdef TESTING
    //     Serial.println("Voltage sensor failed");
    //     #endif

    //     LOG_WARN("Voltage sensor failed");
    //     raiseToStatus(status_, RocketStatus::CRITICAL_FAILURE);
    //     *status_bitfield_ |= 0x04;
    // } else if (battery.lowVoltageWarningSent) {
    //     LOG_WARN("Low battery voltage");
    //     raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
    //     *status_bitfield_ |= 0x04;
    // }
}
