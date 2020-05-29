#pragma once

/**
 * CSV writing Class
 * Wrapper for SD.h which makes printing to csv easier
 */

/*Includes------------------------------------------------------------*/

#include "sensors-interface.h" // ISensor
#include "state_interface.h"   // StateId

/*
Requirements on Impl:
-Has init method that initializes the implementation. takes a char* (filename)
and returns boolean (true if success, false if failure) -Has templated print and
println functions that take in values to be printed. println should do the same
thing as print, except put a newline character at the end. -Has void flush(void)
method that flushes the buffer.
*/

// TODO: Add Impl class or integrate in this class.
// TODO: Figure out how to interface with SD Card

template <typename Impl> class CSVWrite {
  public:
    /**
     * @brief initializes the SD card connection
     * @return false if the sensor fails to initialize
     */
    bool init(const char *filename) { return m_datalog.init(filename); }

    /**
     * @brief prints t to the next csv column in order
     * @param s the data in string to print
     */
    template <typename T> void print(T s) {
        m_datalog.print(s);
        m_datalog.print(",");
    }

    /**
     * @brief prints s and ends the line
     * @param s the in string data to print
     */
    template <typename T> void println(T s) { m_datalog.println(s); }

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() { m_datalog.flush(); }

    /**
     * @brief Logs data on the SD card
     * @param timestamp pointer to store the timestamp value
     * @param sensors the sensors to log data from
     * @param state rocket flight state
     * @param altitude Calculated rocket altitude, after filtering
     * @param baseline_pressure Pressure used as "ground level"
     */
    void logData(unsigned long timestamp, SensorCollection &sensors,
                 StateId state, float altitude, float baseline_pressure) {

        /*
         * SD Card data layout:
         * Timestamp, Rocket flight state, Altitude, Baseline Pressure, Sensor
         * Data
         */
        print(timestamp);
        printState(state);
        print(altitude);
        print(baseline_pressure);

        // Print Sensor Data
        for (auto dat : sensors.getData()) {
            print(*dat);
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
        switch(state) {
            case StateId::STANDBY:
                print("Standby");
                break;
            case StateId::ARMED:
                print("Armed");
                break;
            case StateId::POWERED_ASCENT:
                print("Powered_Ascent");
                break;
            case StateId::PRE_AIR_START_COAST_TIMED:
                print("Pre_Air_Start_Coast_Timed");
                break;
            case StateId::ASCENT_TO_APOGEE:
                print("Ascent_To_Apogee");
                break;
            case StateId::MACH_LOCK:
                print("Mach_Lock");
                break;
            case StateId::PRESSURE_DELAY:
                print("Pressure_Delay");
                break;
            case StateId::DROGUE_DESCENT:
                print("Drogue_Descent");
                break;
            case StateId::MAIN_DESCENT:
                print("Main_Descent");
                break;
            case StateId::LANDED:
                print("Landed");
                break;
            case StateId::WINTER_CONTINGENCY:
                print("Winter_Contingency");
                break;
            default:
                print("Error_Unknown_State");
                break;
        }
    }
    // File m_datalog;
    Impl m_datalog;
};
