/*Includes------------------------------------------------------------*/
#include "sensors/IMU.h"
#include "options.h"
#include "log.hpp"

/*Variables------------------------------------------------------------*/

IMU::IMU(float *const data) : SensorBase(data), bmi088{BMI088_ACC_ALT_ADDRESS, BMI088_GYRO_ALT_ADDRESS} {
#ifdef TESTING
    LOG_DEBUG("Initializing IMU");
#endif
    if (bmi088.isConnection()) {
        bmi088.initialize();
        status = SensorStatus::NOMINAL;
#ifdef TESTING
        LOG_DEBUG("BMI088 is connected");
#endif
    } else {
#ifdef TESTING
        LOG_DEBUG("BMI088 is not connected");
#endif
        status = SensorStatus::FAILURE;
    }
}

void IMU::readData() {
    bmi088.getAcceleration(&ax, &ay, &az);
    bmi088.getGyroscope(&gx, &gy, &gz);
    imuTemp = bmi088.getTemperature();

    status = SensorStatus::NOMINAL;
    
    data_[0] = ax;
    data_[1] = ay;
    data_[2] = az;
    data_[3] = gx;
    data_[4] = gy;
    data_[5] = gz;
    data_[6] = imuTemp;

#ifdef TESTING
    LOG_DEBUG("Polling IMU");
    FLOG_DEBUG("ax: ", data_[0]);
    FLOG_DEBUG("ay: ", data_[1]);
    FLOG_DEBUG("az: ", data_[2]);
    FLOG_DEBUG("gx: ", data_[3]);
    FLOG_DEBUG("gy: ", data_[4]);
    FLOG_DEBUG("gz: ", data_[5]);
    FLOG_DEBUG("imuTemp: ", data_[6]);
#endif

}
