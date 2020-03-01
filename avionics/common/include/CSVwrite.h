#ifndef CSVWRITE_H_53A4CAED991D46F687680132192B5E5A
#define CSVWRITE_H_53A4CAED991D46F687680132192B5E5A

#include "sensors-interface.h" //for ISensor
#include "state_interface.h" //for StateId
/**
  * CSV writing Class
  * Wrapper for SD.h which makes printing to csv easier
  */

/*Includes------------------------------------------------------------*/

/*
Requirements on Impl:
-Has init method that initializes the implementation. takes a char* (filename) and returns boolean (true if success, false if failure)
-Has templated print and println functions that take in values to be printed. println should do the same thing as print, except put a newline character at the end.
-Has void flush(void) method that flushes the buffer.
*/

// TODO: Add Impl class or integrate in this class.

template <typename Impl>
class CSVWrite {
    public:
    /**
     * @brief initializes the SD card connection
     * @return false if the sensor fails to initialize
     */
    bool init(const char* filename) {
        return(m_datalog.init(filename));
        // bool success = SD.begin(BUILTIN_SDCARD);
        // if(success) {
        //     m_datalog = SD.open(filename, FILE_WRITE);
        // }

        // if(!m_datalog) {
        //     return false;
        // }

        // return success;
    }

    /**
     * @brief writes t to the next csv column in order
     * @param t the data to write
     */
    template<typename T>
    void write(T t) {
        m_datalog.print(t);
        m_datalog.print(",");
    }

    /**
     * @brief writes t and ends the line
     * @param t the data to write
     */
    template<typename T>
    void writeln(T t) {
        m_datalog.println(t);
    }

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() {
        m_datalog.flush();
    }

    /**
     * @brief Logs data on the SD card
     * @param timestamp pointer to store the timestamp value
     * @param sensors the sensors to log data from
     * @param state rocket flight state
     * @param altitude Calculated rocket altitude, after filtering
     * @param baseline_pressure Pressure used as "ground level"
     */
    void logData(unsigned long timestamp, SensorSet &sensors,
                StateId state, float altitude, float baseline_pressure) {

        /*write data to SD card*/
        write(timestamp);
        for (auto sensor : sensors) {
            float *data = sensor.get().getData();
            for (int i = 0; i < sensor.get().dataLength(); i++) {
                write(data[i]);
            }
        }
        writeln("");
        flush();
    }

    private:
    // File m_datalog;
    Impl m_datalog;
};
#endif
