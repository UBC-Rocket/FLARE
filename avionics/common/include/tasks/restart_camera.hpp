#ifndef RESTART_CAMERA_HPP_EA5200B75ECD4CC897C73B0CD1E8BA99
#define RESTART_CAMERA_HPP_EA5200B75ECD4CC897C73B0CD1E8BA99
#include "cameras.h"
#include "scheduler.hpp"

// REVIEW(akoen) these values have not been tested and may need to be rocket specific.
constexpr uint32_t CAMERA_POWEROFF_DURATION_MS = 2000;
constexpr uint32_t CAMERA_RESTART_INTERVAL_MS = 20000;
constexpr uint32_t CAMERA_FIRST_POWEROFF_DELAY_MS = 10000;

class RestartCamera {
private:
    Camera &cam_;
    bool camera_on_{true};

public:

    /**
     * @brief Construct a new Restart Camera object
     * @param cam 
     */
    RestartCamera(Camera &cam) : cam_(cam) {};

    static void togglePower(void *self) {
        reinterpret_cast<RestartCamera *>(self)->togglePower();
    }
private:
    /**
     * @brief toggles camera on/off
     */
    void togglePower();
};
#endif
