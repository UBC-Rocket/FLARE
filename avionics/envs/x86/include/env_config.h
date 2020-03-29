#ifndef ENV_CONFIG_H_A5EBD35B10114DF0B6FDCD24E3FE7DA8
#define ENV_CONFIG_H_A5EBD35B10114DF0B6FDCD24E3FE7DA8

// barometer data file
#include <string>
const std::string BAROMETER_DATA{"barometer.csv"};

#include <fstream>
#include <iostream>

#include "HAL/port_impl.h"
#include "stdio_controller.hpp"
StdIoController std_io_controller;
std::mutex StdIoController::s_cout;

#include "radio.h"
NativeRadioController radio(800, std_io_controller);

auto static SerialUSB = Hal::NativeSerial(1, std_io_controller);
auto static SerialGPS = Hal::NativeSerial(2, std_io_controller);
auto static SerialRadio = Hal::NativeSerial(3, std_io_controller);
auto static IridiumSerial = Hal::NativeSerial(4, std_io_controller);
auto static SerialCamera = Hal::NativeSerial(5, std_io_controller);

#include "CSVwrite.h"
#include "SDcard.h"

constexpr char LOG_FILE_NAME[] = "datalog.csv";
CSVWrite<NativeSDcard> datalog;

/* Buzzer */
#include "buzzer.h"
static NativeBuzzer buzzer;

void setup(void);
void loop(void);

int main(void) {
    setup();
    for (;;) {
        loop();
    }
}

#endif
