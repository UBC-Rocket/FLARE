#ifndef PIN_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7
#define PIN_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7

//This file gets included in config.h, but only includes constants so it is safe to include in other places as well.
// Also use this file to define any Serial busses, etc.
#include <Arduino.h>
#include <SPI.h>

//I'm not actually certain what these types are so I'm going to avoid the question with auto
//Apparently SerialUSB is automatically defined by Teensy
// auto SerialUSB        = Serial;
auto static &SerialGPS        = Serial1;
auto static &SerialRadio      = Serial2;
auto static &IridiumSerial    = Serial4;
auto static &SPIThermo        = SPI2;
auto static &SerialCamera     = Serial3;

#endif
