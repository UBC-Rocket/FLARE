#ifndef ENV_CONFIG_H_A5EBD35B10114DF0B6FDCD24E3FE7DA8
#define ENV_CONFIG_H_A5EBD35B10114DF0B6FDCD24E3FE7DA8

#include "stdio_controller.hpp"
StdIoController std_io_controller();


#include <SPI.h>

auto static SerialUSB       = Hal::NativeSerial(1);
auto static SerialGPS       = Hal::NativeSerial(2);
auto static SerialRadio     = Hal::NativeSerial(3);
auto static IridiumSerial   = Hal::NativeSerial(4);
auto static SerialCamera    = Hal::NativeSerial(5);

//TODO - make this
#define SPIThermo SPI2;

#endif
