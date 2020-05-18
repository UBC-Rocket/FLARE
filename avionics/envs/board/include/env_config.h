#ifndef ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7
#define ENV_CONFIG_H_B447E20B937446FCA6C66F5444A7D9A7

#include <HAL/port_impl.h>

// auto static SerialUSB       = Hal::Serial(Serial); //?
auto const static SerialGPS = Hal::Serial(Serial1);
auto const static SerialRadio = Hal::Serial(Serial2);
//Annoyingly trying to make SerialCamera const directly doesn't easily work because the underlying HardwareSerial can't be made const I think - still worth a shot in the future though
auto static SerialCamera = Hal::Serial(Serial3); 
auto const static IridiumSerial = Hal::Serial(Serial4);

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
