/*Sensors Header*/
#ifndef SENSORS_H
#define SENSORS_H

/*Includes------------------------------------------------------------*/
#include <stdint.h>

//#include <string.h>
#include "radio.h"
#include "statemachine.h"


/*Constants------------------------------------------------------------*/
#define NOSECONE //enable or disable nosecone-avionics specific functionality
//#define BODY //enable or disable body-avionics specific functionality
// #define TESTING //enable or disable debug output
//#define GROUND_TEST

#define SerialUSB               Serial
#define SerialGPS               Serial5
#define SerialRadio             Serial2

#define EARTHS_GRAVITY          9.80665

#define ACCELEROMETER_ADDRESS   0x18
#define TEMP_SENSOR_ADDRESS     0x48
#define IMU_ADDRESS             0x37
#define ACCELEROMETER_SCALE     6
#define BATTERY_SENSOR_PIN      9

#define ACC_DATA_ARRAY_SIZE     3
#define BAR_DATA_ARRAY_SIZE     2
#define IMU_DATA_ARRAY_SIZE     3
#define GPS_DATA_ARRAY_SIZE     3
#define GPS_FIELD_LENGTH        20

#define RADIO_DATA_ARRAY_SIZE   5

#define NUM_SENSORS 8
#define FILE_STATUS_POSITION 0
#define BATTERY_STATUS_POSITION 1
#define ACCELEROMETER_STATUS_POSITION 2
#define BAROMETER_STATUS_POSITION 3
#define TEMPERATURE_STATUS_POSITION 4
#define IMU_STATUS_POSITION 5
#define EMATCH_STATUS_POSITION 6
#define SATCOM_STATUS_POSITION 7

/*Variables------------------------------------------------------------*/

/*Error codes for initialization*/
enum OverallError {
    NOMINAL,
    NONCRITICAL_FAILURE,
    CRITICAL_FAILURE
};

struct InitStatus {
    OverallError overview;
    bool sensorNominal[NUM_SENSORS];
};

/*GPS initialization commands*/
const uint8_t GPS_reset_defaults[] = {0xA0, 0xA1, 0x00, 0x02, 0x04, 0x00, 0x04, 0x0D, 0x0A};
const uint8_t GPS_set_baud_rate[] = {0xA0, 0xA1, 0x00, 0x04, 0x05, 0x00, 0x00, 0x00, 0x05, 0x0D, 0x0A}; //4800
const uint8_t GPS_set_NMEA_message[] = {0xA0, 0xA1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0D, 0x0A}; //GPGGA
const uint8_t GPS_set_update_rate[] = {0xA0, 0xA1, 0x00, 0x03, 0x0E, 0x01, 0x00, 0x0F, 0x0D, 0x0A}; //1 Hz




/*Functions------------------------------------------------------------*/
void initSensors(InitStatus* status);
float barSensorInit(void);

void processRadioData(unsigned long*, float*, float*, float*, float*, float*, float*, FlightStates state, float altitude);
void sendRadioData(float data, char id);
void generateStatusReport(InitStatus *status, char *statusReport1, char *statusReport2, char *statusReport3);
void displayStatus(InitStatus *status);
void pollSensors(unsigned long*, float*, float*, float*, float*, float*, float*);
void logData(unsigned long *, float*, float*, float*, float*, float*, float*, FlightStates, float, float);
void processRadioData(unsigned long*, float*, float*, float*, float*, float*, float*, FlightStates state, float altitude);
void sendRadioData(float data, char id);


#endif
