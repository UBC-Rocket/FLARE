#ifndef STATE_INPUT_STRUCT_HPP_4A01F791ECBA4E3CB8BC12436C41ABC9
#define STATE_INPUT_STRUCT_HPP_4A01F791ECBA4E3CB8BC12436C41ABC9

#include "Eigen/Core"
#include "Eigen/Geometry"

struct StateInput {
    bool ignitor_good = false;
    float altitude;               // metres
    float velocity_vertical;      // metres, vertical
    Eigen::Vector3f accel_rocket; // m/s^2, x pointing towards nose
    Eigen::Vector3f accel_ground; // m/s^2, x pointing up
    /*
    Orientation quaternion represents the rotation of the rocket, from the
    ground-based coordinate system. The quaternion is expected to be normalized
    by the time any states deal with it. */
    Eigen::Quaternionf orientation;
};

struct StateAuxilliaryInfo {
    bool in_abort = false;
};

#endif
