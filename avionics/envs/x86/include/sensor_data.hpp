#ifndef SENSOR_DATA_SPOOF
#define SENSOR_DATA_SPOOF

#include <string>
#include <fstream>
#include <vector>
#include "HAL/time.h"

// class for linking data streams to different sensors
// Inputted data to DataSpoof is in form of csv with first column
// being time in ms, and the other columns being each entry of data
// NOTE: The time in ms of the data must be aligned with the used time

class DataSpoof {
public:
    DataSpoof(std::string dataFile, int dataLength) {
        dataStream = std::ifstream(dataFile);
        dlength = dataLength;
        data[dlength];
        std::string dummy;
        // throw away first line of csv
        std::getline(dataStream, dummy);
    }

    // returns next data point in stream
    float *getData() {
        // TODO: interpolation and possibly buffer here
        float dataTime;
        do {
            std::string dataStr;
            std::getline(dataStream, dataStr); 
            std::vector<std::string> dataFields = split_string(dataStr);

            dataTime = std::stof(dataFields[0]);

            for(int i = 1; i < dataFields.size(); i++) {
                data[i - 1] = std::stof(dataFields[i]);
            }
        // searches for data point one past current time in ms
        } while (dataTime < Hal::now_ms().time_since_epoch().count());

        return data;
    }

    int dataLength() {return dlength;}

    ~DataSpoof() {
        dataStream.close();
    }
private:
    std::ifstream dataStream;

    int dlength;
    float *data;

    // helper functino for splitting strings
    std::vector<std::string> split_string(std::string str) {
        std::stringstream ss(str);
        std::vector<std::string> out;
        while(ss.good()) {
            std::string substr;
            std::getline(ss, substr, ',');
            out.push_back(substr);
        }

        return out;
    }
};
#endif
