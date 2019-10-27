#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

enum SensorStatus {
    NOMINAL,
    NONCRITICAL_FAILURE,
    CRITICAL_FAILURE
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
};
#endif