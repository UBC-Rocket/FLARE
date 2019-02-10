/*Includes------------------------------------------------------------*/
#include "cameras.h"
#include "sensors.h"
#include <Arduino.h>
// #include <HardwareSerial.h>
// #include <i2c_t3.h>
// #include <SD.h>





/*Functions------------------------------------------------------------*/
/**
  * @brief  toggle switch for powering the camera
  * @param  None
  * @return none/ should be changed to boolean if possible
  */
void power_cameras(){
    uint8_t power_command[] = {0xCC,0x01,0x01,0x0};
    uint8_t crc = crc_high_first(power_command,3); //hypothetically only looks at the first three indexes
    power_command[3] = crc;
    //SerialUSB.println("the power command is %u", power_command[3]);
    SerialCamera1.write(power_command,sizeof(power_command));
    //SerialCamera2.write(power_command,sizeof(power_command));
}
/**
  * @brief  starts recording on both cameras
  * @param  None
  * @return none/ should be changed to boolean if possible
  */
void start_record(){
    uint8_t startRecord_command[] = {0xCC,0x01,0x03,0x0};
    uint8_t crc = crc_high_first(startRecord_command,3); //hypothetically only looks at the first three indexes
    startRecord_command[3] = crc;
    SerialCamera1.write(startRecord_command,sizeof(startRecord_command));
    //SerialCamera2.write(startRecord_command,sizeof(startRecord_command));
}
/**
  * @brief  stops recording on both cameras
  * @param  None
  * @return none/ should be changed to boolean if possible
  */
void stop_record(){
    uint8_t stopRecord_command[] = {0xCC,0x01,0x04,0x0};
    uint8_t crc = crc_high_first(stopRecord_command,3); //hypothetically only looks at the first three indexes
    stopRecord_command[3] = crc;
    SerialCamera1.write(stopRecord_command,sizeof(stopRecord_command));
    //SerialCamera2.write(stopRecord_command,sizeof(stopRecord_command));
}
/**
  * @brief  calculates the crc for the specific command
  * @param  the first three bytes of the command (not sure about the unsigned char)
  * @return calculated crc
  */
//crc calculation,source: https://github.com/betaflight/betaflight/blob/5c5520ecf43bcd2c042828e08e7e11ab2342ccdd/src/main/common/crc.c?fbclid=IwAR3647Rv68ECXgWra1OktmKaQbQ1DhghM5o7r9s1hPTJkTfR6IW13qzt6LY#L60-L71
//should look into it more
uint8_t crc_high_first(uint8_t *ptr, uint8_t len)
{
    uint8_t i; 
    uint8_t crc = 0x00;
    while (len--) {
        crc ^= *ptr++;  
        for (i = 8; i > 0; --i) { 
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return (crc); 
}
// uint8_t crc8_dvb_s2(uint8_t crc, unsigned char a)
// {
//    crc ^= a;
//    for (int ii = 0; ii < 8; ++ii) {
//        if (crc & 0x80) {
//            crc = (crc << 1) ^ 0xD5;
//        } else {
//            crc = crc << 1;
//        }
//    }
//    return crc;
// }

// uint8_t crc8_dvb_s2_sbuf_append(uint8_t *buffer, uint8_t *start)
// {
//     uint8_t crc = 0;
//     const uint8_t* const end = buffer;
//     for (const uint8_t *ptr = start; ptr < end; ++ptr) {
//         crc = crc8_dvb_s2(crc, *ptr);
//     }
//     return crc
// }



