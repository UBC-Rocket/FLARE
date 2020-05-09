#ifndef CSVWRITE_H_53A4CAED991D46F687680132192B5E5A
#define CSVWRITE_H_53A4CAED991D46F687680132192B5E5A

/**
  * CSV writing Class
  * Wrapper for SD.h which makes printing to csv easier
  */

/*Includes------------------------------------------------------------*/

#include "sensors-interface.h"  // ISensor
#include "state_interface.h"    // StateId

/*
Requirements on Impl:
-Has init method that initializes the implementation. takes a char* (filename) and returns boolean (true if success, false if failure)
-Has templated print and println functions that take in values to be printed. println should do the same thing as print, except put a newline character at the end.
-Has void flush(void) method that flushes the buffer.
*/

// TODO: Add Impl class or integrate in this class.
// TODO: Figure out how to interface with SD Card

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
     * @brief prints t to the next csv column in order
     * @param s the data in string to print
     */
    template<typename T>
    void print(T s) {
        m_datalog.print(s);
        m_datalog.print(",");
    }

    /**
     * @brief prints s and ends the line
     * @param s the in string data to print
     */
    template<typename T>
    void println(T s) {
        m_datalog.println(s);
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

        /*
         * SD Card data layout:
         * Timestamp, Rocket flight state, Altitude, Baseline Pressure, Sensor Data
         */

        // Print Timestamp
        print(timestamp);
        
        // Print Rocket flight state
        printState(state);

        // Print Altitude
        print(altitude);

        // Print Baseline Pressure
        print(baseline_pressure);

        // Print Sensor Data
        for (auto sensor : sensors) {
            float *data = sensor.get().getData();
            for (int i = 0; i < sensor.get().dataLength(); i++) {
                print(data[i]);
            }
        }
        print('\n');
        flush();
    }

    private:
    /**
     * @brief Prints out current flight state
     * @param state Flight state enum class
     */
    void printState(StateId state) {
        if(state == StateId::STANDBY) {
            print("Standby");
        }
        else if(state == StateId::ARMED) {
            print("Armed");
        }
        else if(state == StateId::POWERED_ASCENT) {
            print("Powered_Ascent");
        }
        else if(state == StateId::PRE_AIR_START_COAST_TIMED) {
            print("Pre_Air_Start_Coast_Timed");
        }
        else if(state == StateId::ASCENT_TO_APOGEE) {
            print("Ascent_To_Apogee");
        }
        else if(state == StateId::MACH_LOCK) {
            print("Mach_Lock");
        }
        else if(state == StateId::PRESSURE_DELAY) {
            print("Pressure_Delay");
        }
        else if(state == StateId::DROGUE_DESCENT) {
            print("Drogue_Descent");
        }
        else if(state == StateId::MAIN_DESCENT) {
            print("Main_Descent");
        }
        else if(state == StateId::LANDED) {
            print("Landed");
        }
        else if(state == StateId::WINTER_CONTINGENCY) {
            print("Winter_Contingency");
        }
        else {
            print("Error_Unknown_State");
        }
    }
    // File m_datalog;
    Impl m_datalog;
};
#endif
