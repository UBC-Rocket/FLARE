#pragma once

#include <SD.h>
#include "gpio.h"

/**
 * Another layer of wrapping between commands to write to file and actual file
 * writing - this one to deal with environment-specific stuff
 *
 */
class CSVWriteImpl {
  public:
    bool init(char const *filename) {
        bool success = SD.begin(static_cast<int>(Pin::SD_CHIP_SELECT));
        if (success) {
            m_datalog = SD.open(filename, FILE_WRITE);
        }

        if (!m_datalog) {
            return false;
        }

        return success;
    }

    /**
     * @brief writes t to the next csv column in order
     * @param t the data to write
     */
    template <typename T> void print(T t) {
        m_datalog.print(t);
    }

    /**
     * @brief prints float data with specified number of decimal places to next csv column in order
     * @param data the float data to print
     * @param decimalPlaces the number of decimal places
     */
    void print(float data, int decimalPlaces) {
        m_datalog.print(data, decimalPlaces);
    }

    /**
     * @brief writes t and ends the line
     * @param t the data to write
     */
    template <typename T> void println(T t) { m_datalog.println(t); }

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() { m_datalog.flush(); }

  private:
    File m_datalog;
};
