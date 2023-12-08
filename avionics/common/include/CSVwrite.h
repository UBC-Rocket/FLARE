#pragma once

/**
 * CSV writing Class
 * Wrapper for SD.h which makes printing to csv easier
 */

/*Includes------------------------------------------------------------*/

#include "sensor_collection.h" // Sensors
#include "state_interface.h"   // StateId

/*
Requirements on Impl:
-Has init method that initializes the implementation. takes a char* (filename)
and returns boolean (true if success, false if failure) -Has templated print and
println functions that take in values to be printed. println should do the same
thing as print, except put a newline character at the end. -Has void flush(void)
method that flushes the buffer.
*/

template <typename Impl> class CSVWrite {
  public:
    /**
     * @brief Constructor
     * @param filename Name of the file to log data to.
     */
    CSVWrite(const char *filename) {
        ok_ = m_datalog.init(filename);
        if (!ok_)
            return;

        // Header
        print("Timestamp (ms)");
        print("State");
        print("Altitude (m)");
        print("Baseline altitude (m)");
        m_datalog.print(SensorCollection::LOG_FILE_HEADER);
        m_datalog.print('\n');
    }

    /**
     * @brief Returns true if initialization was successful.
     */
    bool ok() { return ok_; }

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
     * @param baseline_alt Altitude (m) above sea level at the launch site.
     */
    void logData(unsigned long timestamp, SensorCollection &sensors,
                 StateId state, float altitude, float baseline_alt) {

        /*
         * SD Card data layout:
         * Timestamp, Rocket flight state, Altitude, Baseline Pressure, Sensor
         * Data
         */
        print(timestamp);
        print(state);
        print(altitude);
        print(baseline_alt);

        // Print Sensor Data
        // Separate GPS coordinate printing because they require more precision
        std::array<float, SensorCollection::SENSOR_DATA_LENGTH> sensorData = sensors.getData();
        for (int idx = 0; idx < SensorCollection::SENSOR_DATA_LENGTH; idx++) {
            if (idx == SensorCollection::GPS_LAT_INDEX || idx == SensorCollection::GPS_LON_INDEX) {
                print(sensorData[idx], 5);
            } else {
                print(sensorData[idx]);
            }
        }
        m_datalog.print('\n');
        flush();
    }

  private:
    /**
     * @brief prints t to the next csv column in order
     * @param s the data in string to print
     */
    template <typename T> void print(T s) {
        m_datalog.print(s);
        m_datalog.print(",");
    }

    /**
     * @brief prints float data with specified number of decimal places to next csv column in order
     * @param data the float data to print
     * @param decimalPlaces the number of decimal places
     */
    void print(float data, int decimalPlaces) {
        m_datalog.print(data, decimalPlaces);
        m_datalog.print(",");
    }

    /**
     * @brief prints s and ends the line
     * @param s the in string data to print
     */
    template <typename T> void println(T s) { m_datalog.println(s); }

    /**
     * @brief Specialization of print() for StateId.
     * @param state The state to print
     */
    void print(StateId state) {
        switch (state) {
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
    bool ok_;
};
