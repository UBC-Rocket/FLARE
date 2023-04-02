/*Includes------------------------------------------------------------*/
#include "sensors/IMU.h"
#include "options.h"

/*Variables------------------------------------------------------------*/

IMU::IMU(float *const data) : SensorBase(data), bmi088{BMI088_ACC_ALT_ADDRESS, BMI088_GYRO_ALT_ADDRESS} {
#ifdef TESTING
    Serial.println("Initializing IMU");
#endif
    if (bmi088.isConnection()) {
        bmi088.initialize();
        status = SensorStatus::NOMINAL;
#ifdef TESTING
        Serial.println("BMI088 is connected");
#endif
    } else {
#ifdef TESTING
        Serial.println("BMI088 is not connected");
#endif
        status = SensorStatus::FAILURE;
    }
}

void IMU::readData() {
    // #if defined NOSECONE
    //     sensors_event_t event;
    //     IMU_Sensor.getEvent(&event);
    //     data[0] = event.orientation.heading;
    //     data[1] = event.orientation.roll;
    //     data[2] = event.orientation.pitch;

    // #elif defined BODY
    //     imu::Vector<3> accelerometer_data, gyroscope_data, magnetometer_data;

    //     accelerometer_data =
    //         IMU_Sensor.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    //     gyroscope_data =
    //     IMU_Sensor.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    //     magnetometer_data =
    //         IMU_Sensor.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

    //     data[0] = accelerometer_data[0];
    //     data[1] = accelerometer_data[1];
    //     data[2] = accelerometer_data[2];

    //     data[3] = gyroscope_data[0];
    //     data[4] = gyroscope_data[1];
    //     data[5] = gyroscope_data[2];

    //     data[6] = magnetometer_data[0];
    //     data[7] = magnetometer_data[1];
    //     data[8] = magnetometer_data[2];
    // #endif // NOSECONE elif BODY

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
    Serial.println("Polling IMU");
    Serial.print("ax:  ");
    Serial.println(data_[0]);
    Serial.print("ay: ");
    Serial.println(data_[1]);
    Serial.print("az: ");
    Serial.println(data_[2]);
    Serial.print("gx:  ");
    Serial.println(data_[3]);
    Serial.print("gy: ");
    Serial.println(data_[4]);
    Serial.print("gz: ");
    Serial.println(data_[5]);
    Serial.print("imuTemp: ");
    Serial.println(data_[6]);
#endif

}
