#pragma once

#include "HAL/port_impl.h"
#include <cstdint>

#include "status_enums.h"

/**
 * @brief enum of all sensors
 */
enum class SensorType {
    ACCELEROMETER   = 0x02,
    BAROMETER       = 0x03,
    TEMPERATURE     = 0x04,
    IMU             = 0x01,
    GPS             = 0x00,
    THERMOCOUPLE    = 0x05,
    BATTERY         = 0x06
};

#define ACCELEROMETER_DATA_LENGTH   3
#define BAROMETER_DATA_LENGTH       2
#define GPS_DATA_LENGTH             3
#define IMU_DATA_LENGTH             4
#define TEMPERATURE_DATA_LENGTH     1
#define THERMOCOUPLE_DATA_LENGTH    1
#define BATTERY_DATA_LENGTH         1

template <uint8_t size> class SensorBase {
  public:
    SensorBase(float *const data) : data_(data) {}

    /**
     * @brief  Reads sensor data
     */
    virtual void readData() = 0;

    /**
     * @brief  Returns the length of the data array the sensor requires
     */
    static constexpr uint8_t dataLength() { return size; }

    /**
     * @brief  Returns data read during readData()
     * @return the data
     */
    const float *getData() { return data_; }

    /**
     * @brief returns the current status of the seNsor
     * @return the current sensor status
     */
    SensorStatus getStatus() { return status; }

  protected:
    SensorStatus status;
    float *const data_;
};
