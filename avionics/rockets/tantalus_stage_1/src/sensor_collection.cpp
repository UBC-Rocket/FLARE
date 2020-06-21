#include "sensor_collection.h"

SensorCollectionPtr getSensors() {
    std::unique_ptr<SensorCollection> tmp(new SensorCollection);
    return std::move(tmp);
}

SensorCollection::SensorCollection()
    : barometer(BEGIN + BAROMETER_INDEX),
      gps(Hal::SerialGPS, BEGIN + GPS_INDEX),
      accelerometer(BEGIN + ACCEL_INDEX), imuSensor(BEGIN + IMU_INDEX),
      temperature(BEGIN + TEMP_INDEX) {
    updateStatus();
}

void SensorCollection::poll(Hal::t_point &timestamp) {
    timestamp = Hal::now_ms();
    barometer.readData();
    gps.readData();
    accelerometer.readData();
    imuSensor.readData();
    temperature.readData();
}

Status SensorCollection::getStatus(bool refresh) {
    if (refresh) {
        updateStatus();
    }
    return status_;
}

void SensorCollection::updateStatus() {
    status_ = Status::NOMINAL;
    status_bitfield_[0] = 0;
    if (barometer.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, Status::CRITICAL_FAILURE);
        *status_bitfield_ |= 0x80;
    }

    if (gps.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x40;
    }
    if (accelerometer.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x20;
    }
    if (imuSensor.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x10;
    }
    if (temperature.getStatus() == SensorStatus::FAILURE) {
        raiseToStatus(status_, Status::NONCRITICAL_FAILURE);
        *status_bitfield_ |= 0x08;
    }
}
