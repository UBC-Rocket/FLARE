#include <string>

#include "CSVwrite.h"
#include "HAL/port_impl.h"
#include "buzzer.h"
#include "cameras.h"
#include "radio.h"
#include "stdio_controller.hpp"

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

// barometer data file
const std::string BAROMETER_DATA{"data/barometer.csv"};
constexpr char LOG_FILE_NAME[] = "datalog.csv";

int main(void) {
    RadioController radio(800);

    Buzzer buzzer;
    CSVWrite<NativeDumbCSVImpl> datalog;
    for (;;)
        ;
}
