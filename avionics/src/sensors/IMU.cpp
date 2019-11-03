/*Includes------------------------------------------------------------*/
#include "sensors/IMU.h"
#include "options.h"

/*Variables------------------------------------------------------------*/
Adafruit_BNO055 IMU_Sensor(-1, IMU_ADDRESS);

SensorStatus IMU::initSensor() {
    #ifdef TESTING
        SerialUSB.println("Initializing IMU");
    #endif

    delay(7);
    #if defined NOSECONE
        if(!IMU_Sensor.begin(Adafruit_BNO055::OPERATION_MODE_NDOF)){
    #elif defined BODY
        if(!IMU_Sensor.begin(Adafruit_BNO055::OPERATION_MODE_AMG)){
    #endif
            return NONCRITICAL_FAILURE;

            #ifdef TESTING
                SerialUSB.println("ERROR: IMU initialization failed!");
            #endif
        }
    IMU_Sensor.setExtCrystalUse(true);

    return NOMINAL;
}

SensorStatus IMU::readData(float* data) {
    #ifdef TESTING
        SerialUSB.println("Polling IMU");
    #endif

    #if defined NOSECONE
        sensors_event_t event;
        IMU_Sensor.getEvent(&event);
        data[0] = event.orientation.heading;
        data[1] = event.orientation.roll;
        data[2] = event.orientation.pitch;

    #elif defined BODY
        imu::Vector<3> accelerometer_data, gyroscope_data, magnetometer_data;

        accelerometer_data = IMU_Sensor.getVector(
            Adafruit_BNO055::VECTOR_ACCELEROMETER);
        gyroscope_data = IMU_Sensor.getVector(
            Adafruit_BNO055::VECTOR_GYROSCOPE);
        magnetometer_data = IMU_Sensor.getVector(
            Adafruit_BNO055::VECTOR_MAGNETOMETER);

        data[0] = accelerometer_data[0];
        data[1] = accelerometer_data[1];
        data[2] = accelerometer_data[2];

        data[3] = gyroscope_data[0];
        data[4] = gyroscope_data[1];
        data[5] = gyroscope_data[2];

        data[6] = magnetometer_data[0];
        data[7] = magnetometer_data[1];
        data[8] = magnetometer_data[2];
    #endif // NOSECONE elif BODY

    return NOMINAL;
}