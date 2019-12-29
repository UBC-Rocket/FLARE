#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <cstdint>
#include <vector> //for std::vector
#include <functional> //for std::reference_wrapper

enum class SensorStatus {
    NOMINAL,
    FAILURE
};

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

class ISensor {
public:
    /**
     * @brief  Initialize sensor
     * @return SensorStatus
     */
    virtual void initSensor() = 0;

    /**
     * @brief  Reads sensor data
     */
    virtual void readData() = 0;

    /**
     * @brief  Returns the length of the data array the sensor requires
     */
    virtual uint8_t dataLength() = 0;

    /**
     * @brief  Returns data read during readData()
     * @return the data
     */
    virtual float *getData() = 0;

    /**
     * @brief returns the current status of the snesor
     * @return the current sensor status
     */
    SensorStatus getStatus() {
        return status;
    }

protected:
    SensorStatus status;
};

typedef std::vector<std::reference_wrapper<ISensor>> SensorSet;

#endif
