#ifndef sensor-interface
#define sensor-interface

class ISensor
{
    public:

    /** 
      * @brief reads sensor data
      */
    virtual void read() = 0;

    /**
      * @brief returns read data
      * @return sensor data as length 3 array
      */ 
    float* getData() {
        return data;
    }

    private:

    //data
    float data[3];
};

#endif