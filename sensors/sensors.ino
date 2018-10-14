/*Main Arduino Sketch*/

/*Includes------------------------------------------------------------*/
#include "SparkFun_LIS331.h"        //accelerometer
#include "SparkFun_MS5803_I2C.h"    //barometer
#include "SparkFunTMP102.h"         //temp sensor
#include "MPU9250.h"                //IMU

#include <i2c_t3.h>
#include <SD.h>

#include <stdio.h>

/*Constants------------------------------------------------------------*/
#define TESTING //enable or disable debug output
    
#define EARTHS_GRAVITY          9.80665

#define ACCELEROMETER_ADDRESS   0x18
#define TEMP_SENSOR_ADDRESS     0x48
#define IMU_ADDRESS             0x68
#define ACCELEROMETER_SCALE     6

/*Variables------------------------------------------------------------*/
File datalog;
char tempString[10];

LIS331 accelerometer;
MS5803 barometer(ADDRESS_HIGH);
TMP102 temp_sensor(TEMP_SENSOR_ADDRESS);
MPU9250 IMU(Wire, IMU_ADDRESS);

/*Functions------------------------------------------------------------*/
void setup()
{
    bool status = true;

    /*init serial comms*/
    Serial.begin(9600);
    while (!Serial) {}
    #ifdef TESTING
    Serial.println("Initializing...");
    #endif

    /*init I2C bus*/
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400); //400kHz
    Wire.setDefaultTimeout(500000); //500ms

    /*init serial bus*/
    Serial1.begin(9600);
    while (!Serial1) {}

    /*init SD card*/
    if (!SD.begin(BUILTIN_SDCARD)){
        status = false;
        #ifdef TESTING
        Serial.println("ERROR: SD card initialization failed!");
        #endif
    } else {
        datalog = SD.open("datalog.txt", FILE_WRITE);
        if(!datalog){
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
            "IMU - Magnetism Z (uT),IMU - Temperature (C)\n");
        }
    }

    /*init accerlerometer*/
    accelerometer.setI2CAddr(ACCELEROMETER_ADDRESS);
    accelerometer.begin(LIS331::USE_I2C);

    /*init barometer*/
    barometer.reset();
    barometer.begin();

    /*init temp sensor*/
    temp_sensor.begin();

    /*init IMU*/
    int error = 0;
    error = IMU.begin();
    if (error < 0) {
        status = false;
        #ifdef TESTING
        Serial.print("ERROR: IMU initialization failed! Error code ");
        Serial.println(error);
        #endif
    }

    /*if something went wrong spin infinitely, otherwise indicate completion*/
    if(!status){
        while(1){}
    } else {
        pinMode(LED_BUILTIN,OUTPUT);
        digitalWrite(LED_BUILTIN,HIGH);
        Serial.println("Initialization complete!");
    }
}

void loop()
{
    char sdPrintBuff[200] = {'\0'};


    int16_t x, y, z;
    float acc_data[3], bar_data[2], temp_data, IMU_data[10];

    /*poll all the sensors*/
    accelerometer.readAxes(x, y, z);
    acc_data[0] = accelerometer.convertToG(ACCELEROMETER_SCALE, x);
    acc_data[1] = accelerometer.convertToG(ACCELEROMETER_SCALE, y);
    acc_data[2] = accelerometer.convertToG(ACCELEROMETER_SCALE, z);

    bar_data[0] = barometer.getPressure(ADC_4096);
    bar_data[1] = barometer.getTemperature(CELSIUS, ADC_512);

    temp_sensor.wakeup();
    temp_data = temp_sensor.readTempC();
    temp_sensor.sleep();

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
    IMU_data[9] = IMU.getTemperature_C();

    /*write data to SD card, except every time they print something it goes into a buffer*/

    for (unsigned int i = 0; i < sizeof(acc_data)/sizeof(float); i++){
        sprintf(tempString, "%.2f", acc_data[i]);
        strcat(sdPrintBuff, tempString);
        strcat(sdPrintBuff, ",");
    }
    for (unsigned int i = 0; i < sizeof(bar_data)/sizeof(float); i++){
        sprintf(tempString, "%.2f", bar_data[i]);
        strcat(sdPrintBuff, tempString);
        strcat(sdPrintBuff, ",");
    }

    sprintf(tempString, "%.2f", temp_data);
    strcat(sdPrintBuff, tempString);
    strcat(sdPrintBuff, ",");

    for (unsigned int i = 0; i < sizeof(IMU_data)/sizeof(float); i++){
        sprintf(tempString,  "%.2f", IMU_data[i]);
        strcat(sdPrintBuff, tempString);
        strcat(sdPrintBuff, ",");
    }
    strcat(sdPrintBuff, "\n");

    datalog.print(sdPrintBuff);

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
    Serial.println(temp_data);
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
    Serial.print("IMU temperature (C):                ");
    Serial.println(IMU_data[9]);
    Serial.println("");
    #endif

    delay(1000);
}
