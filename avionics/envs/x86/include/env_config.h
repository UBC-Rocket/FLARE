#ifndef ENV_CONFIG_H_A5EBD35B10114DF0B6FDCD24E3FE7DA8
#define ENV_CONFIG_H_A5EBD35B10114DF0B6FDCD24E3FE7DA8

#include "Hal/port_impl.h"
#include "stdio_controller.hpp"
StdIoController std_io_controller();


auto static SerialUSB       = Hal::NativeSerial(1);
auto static SerialGPS       = Hal::NativeSerial(2);
auto static SerialRadio     = Hal::NativeSerial(3);
auto static IridiumSerial   = Hal::NativeSerial(4);
auto static SerialCamera    = Hal::NativeSerial(5);

#include "CSVwrite.h"
class NativeDumbCSVImpl { //TODO - make this not full of no-ops
    bool init(char const *filename) {
        return true;
    }

    /**
     * @brief writes t to the next csv column in order
     * @param t the data to write
     */
    template<typename T>
    void write(T t) {
    }

    /**
     * @brief writes t and ends the line
     * @param t the data to write
     */
    template<typename T>
    void writeln(T t) {
    }

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() {
    }
};

constexpr char LOG_FILE_NAME[] = "datalog.csv";
CSVWrite<NativeDumbCSVImpl> datalog;

#endif
