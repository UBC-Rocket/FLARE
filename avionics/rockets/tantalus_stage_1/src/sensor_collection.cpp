#include "sensor_collection.h"

SensorCollection::SensorCollection()
    : barometer(BEGIN + BAROMETER_INDEX),
      gps(Hal::SerialInst::GPS, BEGIN + GPS_INDEX),
      accelerometer(BEGIN + ACCEL_INDEX), imuSensor(BEGIN + IMU_INDEX),
      temperature(BEGIN + TEMP_INDEX) {
    updateStatus();
}

void SensorCollection::poll() {
    last_poll_time_ = Hal::now_ms();
    barometer.readData();
    gps.readData();
    accelerometer.readData();
    imuSensor.readData();
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
        raiseToStatus(status_, RocketStatus::CRITICAL_FAILURE);
        *status_bitfield_ |= 0x80;
    }

    if (gps.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x40;
    }
    if (accelerometer.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x20;
    }
    if (imuSensor.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x10;
    }
    if (temperature.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, RocketStatus::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x08;
    }
}
