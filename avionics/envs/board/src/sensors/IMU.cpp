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
    LOG_DEBUG(("ax: " + std::to_string(data_[0])).c_str());
    LOG_DEBUG(("ay: " + std::to_string(data_[1])).c_str());
    LOG_DEBUG(("az: " + std::to_string(data_[2])).c_str());
    LOG_DEBUG(("gx: " + std::to_string(data_[3])).c_str());
    LOG_DEBUG(("gy: " + std::to_string(data_[4])).c_str());
    LOG_DEBUG(("gz: " + std::to_string(data_[5])).c_str());
    LOG_DEBUG(("imuTemp: " + std::to_string(data_[6])).c_str());
#endif

}
