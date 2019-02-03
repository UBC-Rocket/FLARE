/*Cameras Header*/
#ifndef CAMERAS_H
#define CAMERAS_H

/*Includes------------------------------------------------------------*/
#include "sensors.h"

//#include <string>

/*Constants------------------------------------------------------------*/
// #define TESTING //enable or disable debug output
#define SerialCamera1           Serial3
#define SerialCamera2           Serial4

/*Variables------------------------------------------------------------*/

/*CAMERA initialization commands*/ //crc is zero initially

// const uint8_t checkStatus_command[] = {0xCC,0x11,0x0}; not sure yet


/*Functions------------------------------------------------------------*/
void power_cameras(); //a toggle switch
void start_record();
void stop_record();
//void check_status(); //crc stuff
uint8_t crc_high_first(uint8_t *ptr, uint8_t len);


#endif


