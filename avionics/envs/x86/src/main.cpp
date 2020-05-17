
// barometer data file
#include <string>
const std::string BAROMETER_DATA{"data/barometer.csv"};

#include "HAL/port_impl.h"
#include "stdio_controller.hpp"

#include "CSVwrite.h"
class NativeDumbCSVImpl { // TODO - make this not full of no-ops
  public:
    bool init(char const *filename) { return true; }

    /**
     * @brief writes t to the next csv column in order
     * @param t the data to write
     */
    template <typename T> void print(T t) {}

    /**
     * @brief writes t and ends the line
     * @param t the data to write
     */
    template <typename T> void println(T t) {}

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() {}
};

constexpr char LOG_FILE_NAME[] = "datalog.csv";
CSVWrite<NativeDumbCSVImpl> datalog;

/* Buzzer */
#include "buzzer.h"
#include "radio.h"

#include "cameras.h"
#include "test_build.h"

// This wraps std::cout and std::cin, so this is a global for the same reason
// std::cout and std::cin are globals
namespace global {
StdIoController stdio_controller;
}
int main(void) {
    RadioController radio(800);
    auto SerialUSB = Hal::Serial(1);
    auto SerialGPS = Hal::Serial(2);
    auto SerialRadio = Hal::Serial(3);
    auto IridiumSerial = Hal::Serial(4);
    auto SerialCamera = Hal::Serial(5);

    NativeBuzzer buzzer;
    for (;;) {
    }
}
