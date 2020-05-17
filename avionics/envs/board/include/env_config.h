#ifndef ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7
#define ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7

#include <HAL/port_impl.h>

// auto static SerialUSB       = Hal::Serial(Serial); //?
auto static SerialGPS = Hal::Serial(Serial1);
auto static SerialRadio = Hal::Serial(Serial2);
auto static IridiumSerial = Hal::Serial(Serial4);
auto static SerialCamera = Hal::Serial(Serial3);

#include "radio.h"

#include "cameras.h"
Camera cam(SerialCamera);

#include "CSVWriteImpl.h"
constexpr char LOG_FILE_NAME[] = "datalog.csv";
CSVWrite<CSVWriteImpl> datalog;

/* Buzzer */
#include "buzzer.h"
static Buzzer buzzer = Buzzer(MELODY_PIN);
#endif
