/* @file    cameras.cpp
 * @author  UBC Rocket Avionics 2018/2019
 * @description  File that controls commanding the camera modules.
 *  Camera module:  Runcam Split 2S
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
#include "cameras.h"
#include <Arduino.h>

/*Functions------------------------------------------------------------*/
/**
  * @brief  toggle switch for powering the camera
  * @param  None
  * @return none/ should be changed to boolean if possible
  */
void power_cameras(){
    uint8_t power_command[] = {0xCC,0x01,0x01,0x0}; // LEAVE THESE HERE
    uint8_t crc = crc_calculator(power_command,3); //hypothetically only looks at the first three indexes
    power_command[3] = crc;
    //SerialUSB.println("the power command is %d", power_command[3]);
    SerialCamera.write(power_command,sizeof(power_command));
}

/**
  * @brief  starts recording on both cameras
  * @param  None
  * @return none/ should be changed to boolean if possible
  */
void start_record(){
    uint8_t startRecord_command[] = {0xCC,0x01,0x03,0x0}; // LEAVE THESE HERE
    uint8_t crc = crc_calculator(startRecord_command,3); //hypothetically only looks at the first three indexes
    startRecord_command[3] = crc;
    SerialCamera.write(startRecord_command,sizeof(startRecord_command));
}

/**
  * @brief  stops recording on both cameras
  * @param  None
  * @return none/ should be changed to boolean if possible
  */
void stop_record(){
    uint8_t stopRecord_command[] = {0xCC,0x01,0x04,0x0}; // LEAVE THESE HERE
    uint8_t crc = crc_calculator(stopRecord_command,3); //hypothetically only looks at the first three indexes
    stopRecord_command[3] = crc;
    SerialCamera.write(stopRecord_command,sizeof(stopRecord_command));
}

/**
  * @brief  calculates the crc for the specific command
  * @param  the first three bytes of the command (not sure about the unsigned char)
  * @return calculated crc
  */
//crc calculation,source: https://github.com/betaflight/betaflight/blob/5c5520ecf43bcd2c042828e08e7e11ab2342ccdd/src/main/common/crc.c?fbclid=IwAR3647Rv68ECXgWra1OktmKaQbQ1DhghM5o7r9s1hPTJkTfR6IW13qzt6LY#L60-L71
uint8_t crc_calculator(uint8_t *command, uint8_t len){
    uint8_t crc = 0;
    for (int i = 0 ; i < len ; i++){
        crc = crc8_dvb_s2 (crc, command[i]);
    }
    return crc;
}

//crc calculator helper
uint8_t crc8_dvb_s2(uint8_t crc, unsigned char a){
    crc ^= a;
    for (int ii = 0; ii < 8; ++ii) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0xD5;
        } else {
            crc = crc << 1;
        }
    }
    return crc;
}
