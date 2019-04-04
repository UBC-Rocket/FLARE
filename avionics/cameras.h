/*Cameras Header*/
#ifndef CAMERAS_H
#define CAMERAS_H

/*Includes------------------------------------------------------------*/
#include "sensors.h"

/*Constants------------------------------------------------------------*/
#define SerialCamera        Serial3
#define CameraBaud          115200

/*Functions------------------------------------------------------------*/
void power_cameras(); //a toggle switch
void start_record();
void stop_record();
uint8_t crc_calculator(uint8_t *command, uint8_t len);
uint8_t crc8_dvb_s2(uint8_t crc, unsigned char a);
#endif
