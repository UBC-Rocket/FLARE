#ifndef CSVWRITE_H
#define CSVWRITE_H

/**
  * CSV writing Class
  * Wrapper for SD.h which makes printing to csv easier
  */

/*Includes------------------------------------------------------------*/
#include <SD.h>

class CSVWrite {
    public:
    /**
     * @brief initializes the SD card connection
     * @return false if the sensor fails to initialize
     */
    bool init(const char* filename) {
        bool success = SD.begin(BUILTIN_SDCARD);
        if(success) {
            datalog = SD.open(filename, FILE_WRITE);
        }

        return success;
    }

    /**
     * @brief writes t to the next csv column in order
     * @param t the data to write
     */
    template<typename T>
    void write(T t) {
        datalog.print(t);
        datalog.printf(',');
    }

    /**
     * @brief writes t and ends the line
     * @param t the data to write
     */
    template<typename T>
    void writeln(T t) {
        datalog.println(t);
    }

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() {
        datalog.flush();
    }

    private:
    File datalog;
};
#endif
