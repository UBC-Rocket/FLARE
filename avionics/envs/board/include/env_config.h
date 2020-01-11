#ifndef ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7
#define ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7


#include <SPI.h>
#include <HAL/port_impl.h>

// auto static SerialUSB       = Hal::Serial(Serial); //?
auto static SerialGPS       = Hal::Serial(Serial1);
auto static SerialRadio     = Hal::Serial(Serial2);
auto static IridiumSerial   = Hal::Serial(Serial4);
auto static SerialCamera    = Hal::Serial(Serial3);

//TODO - make this
#define SPIThermo SPI2;

#include "radio.h"
static RadioController radio = RadioController(Serial2); //TODO - create method in Hal::Serial that can extract the underlying HardwareSerial

#include "cameras.h"
Camera cam(SerialCamera);

#endif
