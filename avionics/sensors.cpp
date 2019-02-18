/*Sensors Source*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"
#include "battery.h"
#include "SparkFun_LIS331.h"        //accelerometer
#include "MS5803_01.h"              //barometer
#include "SparkFunTMP102.h"         //temp sensor
#include "Adafruit_BNO055.h"        //IMU
#include "Venus638FLPx.h"           //GPS

#include <Arduino.h>
#include <HardwareSerial.h>
#include <i2c_t3.h>
#include <SD.h>

/*Variables------------------------------------------------------------*/
File datalog;

Battery powerbattery(BATTERY_SENSOR_PIN);
LIS331 accelerometer;
MS_5803 barometer(1024);
TMP102 temp_sensor(TEMP_SENSOR_ADDRESS);
Adafruit_BNO055 IMU(IMU_ADDRESS);


/*Functions------------------------------------------------------------*/
/**
  * @brief  Initializes all the sensors
  * @param  None
  * @return bool - Status (true for success, false for failure)
  */
bool initSensors(void)
{
    bool status = true;

    /*init battery*/
    #ifdef TESTING
    SerialUSB.println("Initializing battery");
    #endif
    if(powerbattery.getVoltage() <= MINIMUM_BATTERY_VOLTAGE)
    {
       status = false;
        #ifdef TESTING
        SerialUSB.println("WARNING: Battery at low voltage!");
        #endif
    }
    #ifdef TESTING
    SerialUSB.print("Read voltage (V): ");
    SerialUSB.println(powerbattery.getVoltage());
    #endif


    /*init SD card*/
    #ifdef TESTING
    SerialUSB.println("Initializing SD card");
    #endif
    if (!SD.begin(BUILTIN_SDCARD)) {
        status = false;
        #ifdef TESTING
        SerialUSB.println("ERROR: SD card initialization failed!");
        #endif
    } else {
        datalog = SD.open("datalog.txt", FILE_WRITE);
        if (!datalog) {
            status = false;
            #ifdef TESTING
            SerialUSB.println("ERROR: Opening file failed!");
            #endif
        } else {
            datalog.write("SENSOR LOG DATA\n");
            datalog.write("Time (ms), State, Battery Voltage (V),Accelerometer - Acceleration X (g),Accelerometer - Acceleration Y (g),"
            "Accelerometer - Acceleration Z (g),Barometer - Pressure (mbar),Barometer - Temperature (C),"
            "Our - Baseline Pressure (mbar),Our - Altitude (m),Temperature Sensor - Temperature (C),"
            "IMU - Yaw (°),IMU - Roll (°),IMU - Pitch (°),GPS - latitude,GPS - longitude,GPS - altitude\n");
        }
    }

    /*init accerlerometer*/
    #ifdef TESTING
    SerialUSB.println("Initializing accelerometer");
    #endif
    accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    accelerometer.begin(LIS331::USE_I2C);

    /*init barometer*/
    #ifdef TESTING
    SerialUSB.println("Initializing barometer");
    #endif
    //barometer.reset();
    //barometer.begin();
    #ifdef TESTING
    if (!barometer.initializeMS_5803(true)) {
        return false;
    }
    #else
    if (!barometer.initializeMS_5803(false)) {
        return false;
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
    bool status_IMU = IMU.begin();
    delay(7); //TODO investigate this
    if(!status_IMU){
        SerialUSB.print("ERROR: IMU initialization failed!");
    }
    IMU.setExtCrystalUse(true);

    /*init GPS*/
    #ifdef TESTING
    SerialUSB.println("Initializing GPS");
    #endif
    SerialGPS.begin(4800);
    while (!SerialGPS) {}
    SerialGPS.write(GPS_reset_defaults, sizeof(GPS_reset_defaults));
    // SerialGPS.write(GPS_set_baud_rate, sizeof(GPS_set_baud_rate));
    SerialGPS.write(GPS_set_NMEA_message, sizeof(GPS_set_NMEA_message));
    SerialGPS.write(GPS_set_update_rate, sizeof(GPS_set_update_rate));

    /*init radio*/
    #ifdef TESTING
    SerialUSB.println("Initializing radio");
    #endif
    SerialRadio.begin(921600);
    while (!SerialRadio) {}

    return status;
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
  * @param  unsigned long timestamp - pointer to store the timestamp value
  * @param  float acc_data[] - array to store the accelerometer data
  * @param  float bar_data[] - array to store the barometer data
  * @param  float* temp_sensor_data - pointer to store the temperature sensor data
  * @param  float IMU_data[] - array to store the IMU data
  * @param  char GPS_data[][] - 2D array to store the GPS data
  * @return None
  */
void pollSensors(unsigned long *timestamp, float *battery_voltage, float acc_data[], float bar_data[],
                float *temp_sensor_data, float IMU_data[], float GPS_data[])
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
    #endif
    bool bar_flag = barometer.readSensor();

    #ifdef TESTING
    if(!bar_flag)
        SerialUSB.println("BAROMETER FAILED READING");
    #endif
    //bar_data[0] = barometer.getPressure(ADC_4096);
    //bar_data[1] = barometer.getTemperature(CELSIUS, ADC_512);
    bar_data[0] = barometer.pressure();
    bar_data[1] = barometer.temperature();


    #ifdef TESTING
    SerialUSB.println("Polling temperature sensor");
    #endif
    *temp_sensor_data = temp_sensor.readTempC();

    #ifdef TESTING
    SerialUSB.println("Polling IMU");
    #endif
    sensors_event_t event; //TODO what is this
    IMU.getEvent(&event);
    IMU_data[0] = event.orientation.x;
    IMU_data[1] = event.orientation.y;
    IMU_data[2] = event.orientation.z;

    #ifdef TESTING
    SerialUSB.println("Polling GPS");
    #endif
    if (updateGPS()) {
        getGPS(GPS_data);
    }
}

/**
  * @brief  Logs all the sensor data
  * @param  unsigned long timestamp - pointer to store the timestamp value
  * @param  float acc_data[] - array to store the accelerometer data
  * @param  float bar_data[] - array to store the barometer data
  * @param  float* temp_sensor_data - pointer to store the temperature sensor data
  * @param  float IMU_data[] - array to store the IMU data
  * @param  char GPS_data[][] - 2D array to store the GPS data
  * @return None
  */
void logData(unsigned long *timestamp, float *battery_voltage, float acc_data[], float bar_data[],
            float *temp_sensor_data, float IMU_data[], float GPS_data[],
            FlightStates state, float altitude, float baseline_pressure)
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
    for (unsigned int i = 0; i < GPS_DATA_ARRAY_SIZE; i++) {
        datalog.print(GPS_data[i]);
        datalog.print(",");
    }
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
    SerialUSB.print("IMU - Yaw:                          ");
    SerialUSB.println(IMU_data[0]);
    SerialUSB.print("IMU - Roll:                         ");
    SerialUSB.println(IMU_data[1]);
    SerialUSB.print("IMU - Pitch:                        ");
    SerialUSB.println(IMU_data[2]);
    SerialUSB.print("GPS latitude:                       ");
    SerialUSB.println(GPS_data[0]);
    SerialUSB.print("GPS longitude:                      ");
    SerialUSB.println(GPS_data[1]);
    SerialUSB.print("GPS altitude:                       ");
    SerialUSB.println(GPS_data[2]);
    SerialUSB.println("");
    #endif
}


void calculateValues(float acc_data[], float bar_data[], float* abs_accel,
                    float* prev_altitude, float* altitude, float* delta_altitude)
{
    // *abs_accel = sqrtf(powf(acc_data[0], 2) + powf(acc_data[1], 2) + powf(acc_data[2]), 2);
    // *prev_altitude = *altitude;
    // *altitude = 44330.0 * (1 - powf(bar_data[0] / BASELINE_PRESSURE, 1 / 5.255));
    // *delta_altitude = altitude - prev_altitude;
}





