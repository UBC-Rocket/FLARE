#ifndef SENSOR_DATA
#define SENSOR_DATA

// class for linking data streams to different sensors

class DataSpoof {
public:
    // returns next data point in stream
    float *getData();
    int dataLength();
private:
    // some stream object pointing to a file here
};
#endif
