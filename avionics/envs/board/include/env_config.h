#ifndef ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7
#define ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7


#include <SPI.h>

auto static SerialUSB       = Hal::Serial(Serial);
auto static SerialGPS       = Hal::Serial(Serial1);
auto static SerialRadio     = Hal::Serial(Serial2);
auto static IridiumSerial   = Hal::Serial(Serial4);
auto static SerialCamera    = Hal::Serial(Serial3);

//TODO - make this
#define SPIThermo SPI2;

#endif
