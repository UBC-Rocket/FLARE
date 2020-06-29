#pragma once

#include "HAL/port_impl.h"
#include <cstdint>
#include <functional> //for std::reference_wrapper
#include <vector>     //for std::vector

#include "status_enums.h"

/**
 * @brief enum of all sensors
 */
enum class SensorType {
    ACCELEROMETER,
    BAROMETER,
    THERMOMETER,
    IMU,
    GPS,
    THERMOCOUPLE
};

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
