#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

enum class SensorStatus {
    NOMINAL,
    NONCRITICAL_FAILURE,
    CRITICAL_FAILURE
};

/**
 * @brief enum of all sensors
 */
enum SensorType {
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
    virtual SensorStatus initSensor() = 0;

    /** 
      * @brief  Reads sensor data and store to float* data
      * @return SensorStatus
      */
    virtual SensorStatus readData(float* data) = 0;

    /**
     * @brief Returns the length of the data array the sensor requires
     */
    virtual uint8_t dataLength() = 0;
};
#endif