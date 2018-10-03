/*Main Arduino sketch*/

/*Includes------------------------------------------------------------*/
#include "SparkFun_LIS331.h"        //accelerometer
#include "SparkFun_MS5803_I2C.h"    //barometer
#include "SparkFunTMP102.h"         //temp sensor

#include <SD.h>

/*Constants------------------------------------------------------------*/
#define TESTING //enable or disable debug output

#define ACCELEROMETER_ADDRESS   0x18
#define TEMP_SENSOR_ADDRESS     0x48

#define ACCELEROMETER_SCALE     6

/*Variables------------------------------------------------------------*/
File datalog;

LIS331 accelerometer;
MS5803 barometer(ADDRESS_HIGH);
TMP102 temp_sensor(TEMP_SENSOR_ADDRESS);

/*Functions------------------------------------------------------------*/
void setup()
{
    bool status = true;

    /*init serial comms*/
    #ifdef TESTING
    Serial.begin(9600);
    while (!Serial) {}
    Serial.println("Initializing...");
    #endif

    /*init I2C bus*/
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 100000); //100kHz
    Wire.setDefaultTimeout(500000); //500ms

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
            datalog.write("SENSOR LOG DATA \n");
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
    int16_t x, y, z;
    float temp_b, press, temp;

    /*poll all the sensors*/
    accelerometer.readAxes(x, y, z);
    x = accelerometer.convertToG(ACCELEROMETER_SCALE, x);
    y = accelerometer.convertToG(ACCELEROMETER_SCALE, y);
    z = accelerometer.convertToG(ACCELEROMETER_SCALE, z);

    temp_b = barometer.getTemperature(CELSIUS, ADC_512);
    press = barometer.getPressure(ADC_4096);

    temp_sensor.wakeup();
    temp = temp_sensor.readTempC();
    temp_sensor.sleep();

    /*write data to SD card*/
    datalog.write("AX: \n");
    datalog.print(x);
    datalog.write("AY: \n");
    datalog.print(y);
    datalog.write("AZ: \n");
    datalog.print(z);
    datalog.write("Tb: \n");
    datalog.print(temp_b);
    datalog.write("P:  \n");
    datalog.print(press);
    datalog.write("T:  \n");
    datalog.print(temp);

    /*output data to serial*/
    #ifdef TESTING
    Serial.println("Acceleration X (G): ");
    Serial.print(x);
    Serial.println("Acceleration Y (G): ");
    Serial.print(y);
    Serial.println("Acceleration Z (G): ");
    Serial.print(z);
    Serial.println("Temperature (C):    ");
    Serial.print(temp_b);
    Serial.println("Pressure (mbar):    ");
    Serial.print(press);
    Serial.println("Temperature (C):    ");
    Serial.print(temp);
    Serial.println("");
    #endif

    delay(1000);
}
