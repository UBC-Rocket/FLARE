#include "sensor_collection.h"
#include "log.hpp"

constexpr char SensorCollection::LOG_FILE_HEADER[];

SensorCollection::SensorCollection()
    : barometer(BEGIN + BAROMETER_INDEX),
      accelerometer(BEGIN + ACCEL_INDEX) {
    // removed for lite version
    //   gps(Hal::SerialInst::GPS, BEGIN + GPS_INDEX),
    //   accelerometer(BEGIN + ACCEL_INDEX), 
    //   imuSensor(BEGIN + IMU_INDEX),
    //   battery(Pin::VOLTAGE_SENSOR, BEGIN + BATTERY_INDEX), 
    //   temperature(BEGIN + TEMP_INDEX) {
    
    updateStatus();
}

void SensorCollection::poll() {
    last_poll_time_ = Hal::now_ms();
    barometer.readData();
    // gps.readData(); // removed for lite version
    accelerometer.readData();
    // imuSensor.readData(); // removed for lite version
    // battery.readData();
    // temperature.readData();
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
        LOG_ERROR("Barometer failed");
        raiseToStatus(status_, RocketStatus::CRITICAL_FAILURE);
        *status_bitfield_ |= 0x80;
    }

    // removed for lite version
    // if (gps.getStatus() == SensorStatus::FAILURE) {
    //     LOG_WARN("GPS failed");
    //     raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
    //     *status_bitfield_ |= 0x40;
    // }
    if (accelerometer.getStatus() == SensorStatus::FAILURE) {
        LOG_WARN("Accelerometer failed");
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x20;
    }
    // removed for lite version
    // if (imuSensor.getStatus() == SensorStatus::FAILURE) {
    //     LOG_WARN("IMU failed");
    //     raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
    //     *status_bitfield_ |= 0x10;
    // }
    // if (temperature.getStatus() == SensorStatus::FAILURE) {
    //     LOG_WARN("Temperature sensor failed");
    //     raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
    //     *status_bitfield_ |= 0x08;
    // }
    // if (battery.getStatus() == SensorStatus::FAILURE) {
    //     LOG_WARN("Voltage sensor failed");
    //     raiseToStatus(status_, RocketStatus::CRITICAL_FAILURE);
    //     *status_bitfield_ |= 0x04;
    // } else if (battery.lowVoltageWarningSent) {
    //     LOG_WARN("Low battery voltage");
    //     raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
    //     *status_bitfield_ |= 0x04;
    // }
}
