/*
 * Sensors Source
 *
 * @file    sensors.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Main sensor initialization and polling file.
 * Implements sensor initialization and error checking as well as
 * sensor polling, sensor logging, and sending data over the radio.
 * Also performs rocket status-related work - startup buzzer, status report
 * generation, etc.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include "options.h"
#include "battery.h"
#include "SparkFun_LIS331.h"        //accelerometer
#include "MS5803_01.h"              //barometer
#include "SparkFunTMP102.h"         //temp sensor
#include "Adafruit_BNO055.h"        //IMU
#include "GP20U7.h"                 //GPS
#include "gpio.h"                   //GPIO
#include "satcom.h"                 //SATCOM
#include "buzzer.h"                 //for buzzer response on startup
#include "cameras.h"
#include "MAX31855k.h"              //Thermocouple
#include "radio.h"
#include "statemachine.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>

/*Constants------------------------------------------------------------*/
#define ACCELEROMETER_ADDRESS   0x18
#define TEMP_SENSOR_ADDRESS     0x48
#define IMU_ADDRESS             0x28
#define ACCELEROMETER_SCALE     24
#define BATTERY_SENSOR_PIN      9

/*Variables------------------------------------------------------------*/
File datalog;

Battery powerbattery(BATTERY_SENSOR_PIN);
LIS331 accelerometer;
MS_5803 barometer(1024);
TMP102 temp_sensor(TEMP_SENSOR_ADDRESS);
Adafruit_BNO055 IMU(-1, IMU_ADDRESS);

#ifdef NOSECONE
    #ifdef TESTING
        // Self powered with debug output
        MAX31855k probe(THERMO_SELECT_PIN, NONE, NONE, true);
    #else
        // Self powered with no debug messages
        MAX31855k probe(THERMO_SELECT_PIN);
    #endif  // TESTING
#endif // NOSECONE


/*Functions------------------------------------------------------------*/
/**
  * @brief  Initializes all the sensors
  * @param  InitStatus *status - Status variable containing information on status.
  *                         Note: All values in *status will be overwritten; status
  *                         values can be then extracted.
  * @return void
  */

void initSensors(InitStatus *status)
{
    status->overview = NOMINAL;
    int i;
    for(i = 0; i < NUM_SENSORS; i++){
        status->sensorNominal[i] = true;
    }

    /*init battery*/
    #ifdef TESTING
        SerialUSB.println("Initializing battery");
    #endif

    // if(powerbattery.getVoltage() <= LOW_BATTERY_VOLTAGE)
    // { //TODO: Uncomment once the battery sensor is implemented
    //     status->sensorNominal[BATTERY_STATUS_POSITION] = false;
    //     if(powerbattery.getVoltage() <= MINIMUM_BATTERY_VOLTAGE){
    //         status->overview = CRITICAL_FAILURE;
    //         #ifdef TESTING
    //         SerialUSB.println("DANGER: BATTERY AT UNACCEPTABLY LOW VOLTAGE!");
    //         #endif
    //     }
    //     else{
    //         if(status->overview < NONCRITICAL_FAILURE){
    //             status->overview = NONCRITICAL_FAILURE;
    //         }
    //         #ifdef TESTING
    //         SerialUSB.println("WARNING: Battery at low voltage!");
    //         #endif
    //     }
    // }

    #ifdef TESTING
        SerialUSB.print("Read voltage (V): ");
        SerialUSB.println(powerbattery.getVoltage());
    #endif

    #ifdef BODY
    /* check ematch continuity */
    #ifdef TESTING
        SerialUSB.println("Checking ematch continuity");
    #endif

    if (!continuityCheck()){
        status->overview = CRITICAL_FAILURE;     //Critical failure if the ignitor is broken
        status->sensorNominal[EMATCH_STATUS_POSITION] = false;

        #ifdef TESTING
            SerialUSB.println("ERROR: ematch continuity");
        #endif
    }
    #endif //to IFDEF BODY

    /*init SD card*/
    #ifdef TESTING
        SerialUSB.println("Initializing SD card");
    #endif
    if (!SD.begin(BUILTIN_SDCARD)) {
        if(status->overview < NONCRITICAL_FAILURE)
            status->overview = NONCRITICAL_FAILURE;
        status->sensorNominal[FILE_STATUS_POSITION] = false;


        #ifdef TESTING
            SerialUSB.println("ERROR: SD card initialization failed!");
        #endif
    } else {
        datalog = SD.open("datalog.csv", FILE_WRITE);
        if (!datalog) {
            if(status->overview < NONCRITICAL_FAILURE)
                status->overview = NONCRITICAL_FAILURE;
            status->sensorNominal[FILE_STATUS_POSITION] = false;

            #ifdef TESTING
                SerialUSB.println("ERROR: Opening file failed!");
            #endif
        } else {
            #ifdef NOSECONE
                datalog.write("NOSECONE SENSOR LOG DATA\n");
                datalog.write("Time (ms), State, Battery Voltage (V),Accelerometer - Acceleration X (g),Accelerometer - Acceleration Y (g),"
                "Accelerometer - Acceleration Z (g),Barometer - Pressure (mbar),Barometer - Temperature (C),"
                "Our - Baseline Pressure (mbar),Our - Altitude (m),Temperature Sensor - Temperature (C),"
                "IMU - Heading (°),IMU - Roll (°),IMU - Pitch (°),GPS - latitude,GPS - longitude,GPS - altitude,SatCom,Thermocouple (C)\n");
            #endif
            #ifdef BODY
                datalog.write("BODY SENSOR LOG DATA\n");
                datalog.write("Time (ms), State, Battery Voltage (V),Accelerometer - Acceleration X (g),Accelerometer - Acceleration Y (g),"
                "Accelerometer - Acceleration Z (g),Barometer - Pressure (mbar),Barometer - Temperature (C),"
                "Our - Baseline Pressure (mbar),Our - Altitude (m),Temperature Sensor - Temperature (C),"
                "IMU - acceleration X (m/s^2),IMU - acceleration Y (m/s^2),IMU - acceleration Z (m/s^2),"
                "IMU - gyroscope X (rad/s),IMU - gyroscope Y (rad/s),IMU - gyroscope Z (rad/s),"
                "IMU - magnetometer X (mT),IMU - magnetometer Y (mT),IMU - magnetometer Z (mT),"
                "ematch\n");
            #endif
        }
    }

    /*init accerlerometer*/
    #ifdef TESTING
        SerialUSB.println("Initializing accelerometer");
    #endif
    accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    accelerometer.begin(LIS331::USE_I2C);
    accelerometer.setFullScale(LIS331::HIGH_RANGE);

    /*init barometer*/
    #ifdef TESTING
        SerialUSB.println("Initializing barometer");
    #endif
    #ifdef TESTING
    if (!barometer.initializeMS_5803(true)) { //because one is verbose
        status->overview = CRITICAL_FAILURE;
        status->sensorNominal[BAROMETER_STATUS_POSITION] = false;
    }
    #else
    if (!barometer.initializeMS_5803(false)) {
        status->overview = CRITICAL_FAILURE;
        status->sensorNominal[BAROMETER_STATUS_POSITION] = false;
    }
    #endif

    /*init temp sensor*/
    #ifdef TESTING
        SerialUSB.println("Initializing temperature sensor");
    #endif
    temp_sensor.begin();

    /*init IMU*/
    #ifdef TESTING
        SerialUSB.println("Initializing IMU");
    #endif

    delay(7);
    #if defined NOSECONE
        if(!IMU.begin(Adafruit_BNO055::OPERATION_MODE_NDOF)){
    #elif defined BODY
        if(!IMU.begin(Adafruit_BNO055::OPERATION_MODE_AMG)){
    #endif
            if(status->overview < NONCRITICAL_FAILURE)
                status->overview = NONCRITICAL_FAILURE;
            status->sensorNominal[IMU_STATUS_POSITION] = false;

            #ifdef TESTING
                SerialUSB.println("ERROR: IMU initialization failed!");
            #endif
        }
    IMU.setExtCrystalUse(true);

    /* Init Cameras */
    #ifdef TESTING
        SerialUSB.println("Initializing the camera");
    #endif
    SerialCamera.begin(CameraBaud);
    while (!SerialCamera) {}
    delay(2000);
    stop_record();

    #ifdef NOSECONE
        /*init GPS*/
        #ifdef TESTING
            SerialUSB.println("Initializing GPS");
        #endif
        SerialGPS.begin(9600);  //baud rate 9600 for the GP-20U7
        while (!SerialGPS) {}

    #endif



    #ifdef NOSECONE
        /*init satcom*/
        if (!SatComSetup()){
            if(status->overview < NONCRITICAL_FAILURE)
                status->overview = NONCRITICAL_FAILURE;
            status->sensorNominal[SATCOM_STATUS_POSITION] = false;
        }
    #endif // NOSECONE

    /* init Thermocouple */
    #ifdef NOSECONE
        #ifdef TESTING
            SerialUSB.println("Initializing thermocouple");
        #endif

        float thermo_temp = probe.readCJT();
        if (!isnan(thermo_temp)) {
            #ifdef TESTING
                SerialUSB.print("Cold Junction Temperature is [C]: ");
                SerialUSB.println(thermo_temp);
                SerialUSB.println("Thermocouple initialized");
            #endif
        }
        else{
            if(status->overview < NONCRITICAL_FAILURE)
                status->overview = NONCRITICAL_FAILURE;
            status->sensorNominal[THERMOCOUPLE_STATUS_POSITION] = false;
            #ifdef TESTING
                SerialUSB.println("Thermocouple failed to init");
            #endif
        }
    #endif // NOSECONE

    /* log initialization status for each sensor */
    // 'X' for N/A, 'G' for good, 'B' for bad
    datalog.write("X,X,"); //time, state;

    if(status->sensorNominal[BATTERY_STATUS_POSITION])
        datalog.write("G,");
    else
        datalog.write("B,");

    if(status->sensorNominal[ACCELEROMETER_STATUS_POSITION])
        datalog.write("G,G,G,");
    else
        datalog.write("B,B,B,");

    if(status->sensorNominal[BAROMETER_STATUS_POSITION]) //barom pressure & temperature
        datalog.write("G,G,");
    else
        datalog.write("B,B,");

    datalog.write("X,X,"); //Baseline pressure & altitude, no capability to test

    if(status->sensorNominal[TEMPERATURE_STATUS_POSITION])
        datalog.write("G,");
    else
        datalog.write("B,");

    if(status->sensorNominal[IMU_STATUS_POSITION]) { //Yaw, roll, pitch
        #ifdef BODY
            datalog.write("G,G,G,G,G,G,G,G,G,");
        #else
            datalog.write("G,G,G,");
        #endif
    }
    else {
        #ifdef BODY
            datalog.write("B,B,B,B,B,B,B,B,B,");
        #else
            datalog.write("B,B,B,");
        #endif
    }

    #ifdef NOSECONE
        datalog.write("X, X, X,");   //GPS no capability to test success
    #endif

    #ifdef BODY
        if(status->sensorNominal[EMATCH_STATUS_POSITION])   //ematch continuity
            datalog.write("G\n");
        else
            datalog.write("B\n");
    #endif

    #ifdef NOSECONE
        if(status->sensorNominal[SATCOM_STATUS_POSITION])
            datalog.write("G,");
        else
            datalog.write("B,");

        if(status->sensorNominal[THERMOCOUPLE_STATUS_POSITION])
            datalog.write("G\n");
        else
            datalog.write("B\n");

    #endif

    /* transmit sensor report */

    displayStatus(status);
    return;
}

/*
 * @brief  Reports status with buzzer tune and LED.
 * @param  InitStatus *status - status of initialization.
 * @return void
 */

void displayStatus(InitStatus *status){
    if (status->overview == CRITICAL_FAILURE) {
        #ifdef TESTING
            SerialUSB.println("Critical failure! >:-{");
        #endif

        #ifdef TESTING //Only plays song once if testing, because it's annoying
        for(int i = 1; i <= 1; i++){
        #else
        for(int i = 1; i <= 5; i++){
        #endif
            sing(SongTypes_CRITICALFAIL);
            delay(400);
        }
    }
    else if (status->overview == NONCRITICAL_FAILURE){
        #ifdef TESTING
            SerialUSB.println("Noncritical failure! :(");
        #endif
        pinMode(LED_BUILTIN, OUTPUT);

        #ifdef TESTING
        for(int i = 1; i <= 1; i++){
        #else
        for(int i = 1; i <= 5; i++){
        #endif
            sing(SongTypes_NONCRITFAIL);
            delay(400);
        }
    }
    else {
        #ifdef TESTING
            SerialUSB.println("Initialization complete! :D");
        #endif
        pinMode(LED_BUILTIN,OUTPUT);
        digitalWrite(LED_BUILTIN,HIGH);

        #ifdef TESTING
        for(int i = 1; i <= 1; i++){
        #else
        for(int i = 1; i <= 5; i++){
        #endif
            sing(SongTypes_SUCCESS);
            delay(400);
        }
    }

    return;
}

/**
  * @brief  Retrieves one pressure value from the barometer.
  * @return float - pressure data point from the barometer.
  */
float barSensorInit(void){
    barometer.readSensor();
    return barometer.pressure();
}

/**
  * @brief  Polls all the sensors
  * @param  unsigned long *timestamp - pointer to store the timestamp value
  * @param  float *battery_voltage - Stores battery voltage
  * @param  float acc_data[] - array to store the accelerometer data; refer to sensors.h for array sizes
  * @param  float bar_data[] - array to store the barometer data; refer to sensors.h for array sizes
  * @param  float* temp_sensor_data - pointer to store the temperature sensor data
  * @param  float IMU_data[] - array to store the IMU data; refer to sensors.h for array sizes
  * @param  char GPS_data[] - array to store the GPS data; refer to sensors.h for array sizes
  * @param  *thermocouple_data - pointer to store thermocouple data.
  * @return None
  */
void pollSensors(unsigned long *timestamp, float *battery_voltage, float acc_data[], float bar_data[],
                float *temp_sensor_data, float IMU_data[], float GPS_data[], float *thermocouple_data)
{
    int16_t x, y, z;

    *timestamp = millis();

    #ifdef TESTING
        SerialUSB.println("Measuring battery voltage");
    #endif
    *battery_voltage = powerbattery.getVoltage();

    #ifdef TESTING
        SerialUSB.println("Polling accelerometer");
    #endif
    accelerometer.readAxes(x, y, z);
    acc_data[0] = accelerometer.convertToG(ACCELEROMETER_SCALE, x);
    acc_data[1] = accelerometer.convertToG(ACCELEROMETER_SCALE, y);
    acc_data[2] = accelerometer.convertToG(ACCELEROMETER_SCALE, z);

    #ifdef TESTING
        SerialUSB.println("Polling barometer");
    bool bar_flag = barometer.readSensor();
    if(!bar_flag)
        SerialUSB.println("BAROMETER FAILED READING");

    #else

    barometer.readSensor();
    #endif

    bar_data[0] = barometer.pressure();
    bar_data[1] = barometer.temperature();


    #ifdef TESTING
        SerialUSB.println("Polling temperature sensor");
    #endif
    *temp_sensor_data = temp_sensor.readTempC();

    #ifdef TESTING
        SerialUSB.println("Polling IMU");
    #endif

    #if defined NOSECONE
        sensors_event_t event;
        IMU.getEvent(&event);
        IMU_data[0] = event.orientation.heading;
        IMU_data[1] = event.orientation.roll;
        IMU_data[2] = event.orientation.pitch;

    #elif defined BODY
        imu::Vector<3> accelerometer_data, gyroscope_data, magnetometer_data;

        accelerometer_data = IMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        gyroscope_data = IMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        magnetometer_data = IMU.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

        IMU_data[0] = accelerometer_data[0];
        IMU_data[1] = accelerometer_data[1];
        IMU_data[2] = accelerometer_data[2];

        IMU_data[3] = gyroscope_data[0];
        IMU_data[4] = gyroscope_data[1];
        IMU_data[5] = gyroscope_data[2];

        IMU_data[6] = magnetometer_data[0];
        IMU_data[7] = magnetometer_data[1];
        IMU_data[8] = magnetometer_data[2];
    #endif // NOSECONE elif BODY

    #ifdef NOSECONE
        #ifdef TESTING
            SerialUSB.println("Polling GPS");
        #endif
        if (updateGPS()) {
            getGPS(GPS_data);
        }
    #endif

    #ifdef NOSECONE
        #ifdef TESTING
            SerialUSB.println("Polling Thermocouple");
        #endif
        *thermocouple_data = probe.readTempC();
        #ifdef TESTING
            if (!isnan(*thermocouple_data)) {
                SerialUSB.print("Temp[C]=");
                SerialUSB.println(*thermocouple_data);
            }
            else
                SerialUSB.println("Thermocouple ERROR");
        #endif
    #endif

}

/**
  * @brief  Polls all the sensors
  * @param  unsigned long *timestamp - pointer to store the timestamp value
  * @param  float *battery_voltage - Stores battery voltage
  * @param  float acc_data[] - array to store the accelerometer data; refer to sensors.h for array sizes
  * @param  float bar_data[] - array to store the barometer data; refer to sensors.h for array sizes
  * @param  float* temp_sensor_data - pointer to store the temperature sensor data
  * @param  float IMU_data[] - array to store the IMU data; refer to sensors.h for array sizes
  * @param  char GPS_data[] - array to store the GPS data; refer to sensors.h for array sizes
  * @param  FlightStates state - rocket flight state
  * @param  float altitude - Calculated rocket altitude, after filtering
  * @param  float baseline_pressure - Pressure used as "ground level"
  * @param  *thermocouple_data - pointer to store thermocouple data.
  * @return None
  */
void logData(unsigned long *timestamp, float *battery_voltage, float acc_data[], float bar_data[],
            float *temp_sensor_data, float IMU_data[], float GPS_data[],
            FlightStates state, float altitude, float baseline_pressure, float thermocouple_data)
{
    /*write data to SD card*/
    #ifdef TESTING
        SerialUSB.println("Writing to SD card");
    #endif
    datalog.print(*timestamp);
    datalog.print(",");
    datalog.print(state);
    datalog.print(",");
    datalog.print(*battery_voltage);
    datalog.print(",");
    for (unsigned int i = 0; i < ACC_DATA_ARRAY_SIZE; i++) {
       datalog.print(acc_data[i]);
       datalog.print(",");
    }
    for (unsigned int i = 0; i < BAR_DATA_ARRAY_SIZE; i++) {
        datalog.print(bar_data[i]);
        datalog.print(",");
    }
    datalog.print(baseline_pressure);
    datalog.print(",");
    datalog.print(altitude);
    datalog.print(",");
    datalog.print(*temp_sensor_data);
    datalog.print(",");
    for (unsigned int i = 0; i < IMU_DATA_ARRAY_SIZE; i++) {
        datalog.print(IMU_data[i]);
        datalog.print(",");
    }
    #ifdef NOSECONE
        for (unsigned int i = 0; i < GPS_DATA_ARRAY_SIZE; i++) {
            datalog.print(GPS_data[i], 6);
            datalog.print(",");
        }
        datalog.print(",");
        datalog.print(thermocouple_data);
        datalog.print(",");
    #endif
    datalog.print("\n");
    datalog.flush();

    /*output data to serial*/
    #ifdef TESTING
        SerialUSB.print("Time (ms):                          ");
        SerialUSB.println(*timestamp);
        SerialUSB.print("State:                              ");
        SerialUSB.println(state);
        SerialUSB.print("Battery voltage (V):                ");
        SerialUSB.println(*battery_voltage);
        SerialUSB.print("Accelerometer acceleration X (g):   ");
        SerialUSB.println(acc_data[0]);
        SerialUSB.print("Accelerometer acceleration Y (g):   ");
        SerialUSB.println(acc_data[1]);
        SerialUSB.print("Accelerometer acceleration Z (g):   ");
        SerialUSB.println(acc_data[2]);
        SerialUSB.print("Barometer pressure (mbar):          ");
        SerialUSB.println(bar_data[0]);
        SerialUSB.print("Barometer temperature (C):          ");
        SerialUSB.println(bar_data[1]);
        SerialUSB.print("Baseline Pressure (mbar):           ");
        SerialUSB.println(baseline_pressure);
        SerialUSB.print("Altitude (m):                       ");
        SerialUSB.println(altitude);
        SerialUSB.print("Temperature sensor temperature (C): ");
        SerialUSB.println(*temp_sensor_data);

        #if defined NOSECONE
            SerialUSB.print("IMU - Heading:                      ");
            SerialUSB.println(IMU_data[0]);
            SerialUSB.print("IMU - Roll:                         ");
            SerialUSB.println(IMU_data[1]);
            SerialUSB.print("IMU - Pitch:                        ");
            SerialUSB.println(IMU_data[2]);

            SerialUSB.print("GPS latitude:                       ");
            SerialUSB.println(GPS_data[0], 6);
            SerialUSB.print("GPS longitude:                      ");
            SerialUSB.println(GPS_data[1], 6);
            SerialUSB.print("GPS altitude:                       ");
            SerialUSB.println(GPS_data[2], 3);

            SerialUSB.print("Thermocouple (C):                   ");
            SerialUSB.println(thermocouple_data);

        #elif defined BODY
            SerialUSB.print("IMU - accelerometer X (g):          ");
            SerialUSB.println(IMU_data[0]);
            SerialUSB.print("IMU - accelerometer Y (g):          ");
            SerialUSB.println(IMU_data[1]);
            SerialUSB.print("IMU - accelerometer Z (g):          ");
            SerialUSB.println(IMU_data[2]);
            SerialUSB.print("IMU - gyroscope X (rad/s):          ");
            SerialUSB.println(IMU_data[3]);
            SerialUSB.print("IMU - gyroscope Y (rad/s):          ");
            SerialUSB.println(IMU_data[4]);
            SerialUSB.print("IMU - gyroscope Z (rad/s):          ");
            SerialUSB.println(IMU_data[5]);
            SerialUSB.print("IMU - magnetometer X (mT):          ");
            SerialUSB.println(IMU_data[6]);
            SerialUSB.print("IMU - magnetometer Y (mT):          ");
            SerialUSB.println(IMU_data[7]);
            SerialUSB.print("IMU - magnetometer Z (mT):          ");
            SerialUSB.println(IMU_data[8]);
        #endif

        SerialUSB.println("");
    #endif
}
