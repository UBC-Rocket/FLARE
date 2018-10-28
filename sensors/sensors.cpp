/*Sensors Source*/

/*Includes------------------------------------------------------------*/
#include "sensors.h"

/*Constants------------------------------------------------------------*/

/*Variables------------------------------------------------------------*/
File datalog;

LIS331 accelerometer;
MS5803 barometer(ADDRESS_HIGH);
TMP102 temp_sensor(TEMP_SENSOR_ADDRESS);
MPU9250 IMU(Wire, IMU_ADDRESS);
Venus638FLPx GPS(Serial1);

/*Functions------------------------------------------------------------*/

/*initialize all the sensors*/
bool initSensors(void)
{
    bool status = true;
    byte GPS_reset_defaults[] = {0xA0, 0xA1, 0x00, 0x02, 0x04, 0x00, 0x04, 0x0D, 0x0A};
    byte GPS_set_baud_rate[] = {0xA0, 0xA1, 0x00, 0x04, 0x05, 0x00, 0x05, 0x00, 0x00, 0x0D, 0x0A}; //115200
    byte GPS_set_NMEA_message[] = {0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0D, 0x0A}; //GPGGA
    byte GPS_set_update_rate[] = {0xA0, 0xA1, 0x00, 0x03, 0x0E, 0x10, 0x00, 0x1E, 0x0D, 0x0A}; //10 Hz

    /*init SD card*/
    #ifdef TESTING
    Serial.println("Initializing SD card");
    #endif
    if (!SD.begin(BUILTIN_SDCARD)) {
        status = false;
        #ifdef TESTING
        Serial.println("ERROR: SD card initialization failed!");
        #endif
    } else {
        datalog = SD.open("datalog.txt", FILE_WRITE);
        if (!datalog) {
            status = false;
            #ifdef TESTING
            Serial.println("ERROR: Opening file failed!");
            #endif
        } else {
            datalog.write("SENSOR LOG DATA\n");
            datalog.write("Accelerometer - Acceleration X (g),Accelerometer - Acceleration Y (g),"
            "Accelerometer - Acceleration Z (g),Barometer - Pressure (mbar),Barometer - Temperature (C),"
            "Temperature Sensor - Temperature (C),IMU - Acceleration X (g),IMU - Acceleration Y (g),"
            "IMU - Acceleration Z (g),IMU - Angular Velocity X (rad/s),IMU - Angular Velocity Y (rad/s),"
            "IMU - Angular Velocity Z (rad/s),IMU - Magnetism X (uT),IMU - Magnetism Y (uT),"
            "IMU - Magnetism Z (uT),GPS - Latitude (DDM),GPS - Longitude (DDM),GPS - Altitude (m)\n");
        }
    }

    /*init accerlerometer*/
    #ifdef TESTING
    Serial.println("Initializing accelerometer");
    #endif
    accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    accelerometer.begin(LIS331::USE_I2C);

    /*init barometer*/
    #ifdef TESTING
    Serial.println("Initializing barometer");
    #endif
    barometer.reset();
    barometer.begin();

    /*init temp sensor*/
    #ifdef TESTING
    Serial.println("Initializing temperature sensor");
    #endif
    temp_sensor.begin();

    /*init IMU*/
    #ifdef TESTING
    Serial.println("Initializing IMU");
    #endif
    int error = 0;
    error = IMU.begin();
    if (error < 0) {
        status = false;
        #ifdef TESTING
        Serial.print("ERROR: IMU initialization failed! Error code ");
        Serial.println(error);
        #endif
    }

    /*init GPS*/
    #ifdef TESTING
    Serial.println("Initializing GPS");
    #endif
    Serial1.begin(9600);
    while (!Serial1) {}
    Serial1.write(GPS_reset_defaults, sizeof(GPS_reset_defaults));
    Serial1.write(GPS_set_baud_rate, sizeof(GPS_set_baud_rate));
    Serial1.begin(115200);
    while (!Serial1) {}
    Serial1.write(GPS_set_NMEA_message, sizeof(GPS_set_NMEA_message));
    Serial1.write(GPS_set_update_rate, sizeof(GPS_set_update_rate));

    /*init radio*/
    #ifdef TESTING
    Serial.println("Initializing radio");
    #endif
    Serial2.begin(921600);
    while (!Serial2) {}

    return status;
}

/*poll all the sensors*/
void pollSensors(float acc_data[], float bar_data[],
                float *temp_sensor_data, float IMU_data[], char GPS_data[][GPS_SENTENCE_LENGTH])
{
    int16_t x, y, z;
    bool gps_status;

    #ifdef TESTING
    Serial.println("Polling accelerometer");
    #endif
    accelerometer.readAxes(x, y, z);
    acc_data[0] = accelerometer.convertToG(ACCELEROMETER_SCALE, x);
    acc_data[1] = accelerometer.convertToG(ACCELEROMETER_SCALE, y);
    acc_data[2] = accelerometer.convertToG(ACCELEROMETER_SCALE, z);

    #ifdef TESTING
    Serial.println("Polling barometer");
    #endif
    bar_data[0] = barometer.getPressure(ADC_4096);
    bar_data[1] = barometer.getTemperature(CELSIUS, ADC_512);

    #ifdef TESTING
    Serial.println("Polling temperature sensor");
    #endif
    temp_sensor.wakeup();
    *temp_sensor_data = temp_sensor.readTempC();
    temp_sensor.sleep();

    #ifdef TESTING
    Serial.println("Polling IMU");
    #endif
    IMU.readSensor();
    IMU_data[0] = IMU.getAccelX_mss() / EARTHS_GRAVITY; //convert to g
    IMU_data[1] = IMU.getAccelY_mss() / EARTHS_GRAVITY; //convert to g
    IMU_data[2] = IMU.getAccelZ_mss() / EARTHS_GRAVITY; //convert to g
    IMU_data[3] = IMU.getGyroX_rads();
    IMU_data[4] = IMU.getGyroY_rads();
    IMU_data[5] = IMU.getGyroZ_rads();
    IMU_data[6] = IMU.getMagX_uT();
    IMU_data[7] = IMU.getMagY_uT();
    IMU_data[8] = IMU.getMagZ_uT();

    #ifdef TESTING
    Serial.println("Polling GPS");
    #endif
    GPS.getGPS(&gps_status);
    if (gps_status) {
        GPS.getField(GPS_data[0], 2); //latitude
        GPS.getField(GPS_data[1], 4); //longitude
        GPS.getField(GPS_data[2], 9); //altitude
    }
}

/*log all the data*/
void logData(float acc_data[], float bar_data[],
            float *temp_sensor_data, float IMU_data[], char GPS_data[][GPS_SENTENCE_LENGTH])
{
    /*write data to SD card*/
    #ifdef TESTING
    Serial.println("Writing to SD card");
    #endif
    for (unsigned int i = 0; i < ACC_DATA_ARRAY_SIZE; i++) {
       datalog.print(acc_data[i]);
       datalog.print(",");
    }
    for (unsigned int i = 0; i < BAR_DATA_ARRAY_SIZE; i++) {
        datalog.print(bar_data[i]);
        datalog.print(",");
    }
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
    Serial.print("Accelerometer acceleration X (g):   ");
    Serial.println(acc_data[0]);
    Serial.print("Accelerometer acceleration Y (g):   ");
    Serial.println(acc_data[1]);
    Serial.print("Accelerometer acceleration Z (g):   ");
    Serial.println(acc_data[2]);
    Serial.print("Barometer pressure (mbar):          ");
    Serial.println(bar_data[0]);
    Serial.print("Barometer temperature (C):          ");
    Serial.println(bar_data[1]);
    Serial.print("Temperature sensor temperature (C): ");
    Serial.println(*temp_sensor_data);
    Serial.print("IMU acceleration X (g):             ");
    Serial.println(IMU_data[0]);
    Serial.print("IMU acceleration Y (g):             ");
    Serial.println(IMU_data[1]);
    Serial.print("IMU acceleration Z (g):             ");
    Serial.println(IMU_data[2]);
    Serial.print("IMU angular velocity X (rad/s):     ");
    Serial.println(IMU_data[3]);
    Serial.print("IMU angular velocity Y (rad/s):     ");
    Serial.println(IMU_data[4]);
    Serial.print("IMU angular velocity Z (rad/s):     ");
    Serial.println(IMU_data[5]);
    Serial.print("IMU magnetism X (uT):               ");
    Serial.println(IMU_data[6]);
    Serial.print("IMU magnetism X (uT):               ");
    Serial.println(IMU_data[7]);
    Serial.print("IMU magnetism X (uT):               ");
    Serial.println(IMU_data[8]);
    Serial.print("GPS latitude:                       ");
    Serial.println(GPS_data[0]);
    Serial.print("GPS longitude:                      ");
    Serial.println(GPS_data[1]);
    Serial.print("GPS altitude:                       ");
    Serial.println(GPS_data[2]);
    Serial.println("");
    #endif
}
