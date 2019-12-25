#ifndef STATE_INPUT_STRUCT_H
#define STATE_INPUT_STRUCT_H

#include "Eigen/Core"
#include "Eigen/Geometry"

struct StateInput {
    bool ignitor_good = false;
    float altitude; //metres
    float velocity_vertical; //metres, vertical
    Eigen::Vector3f accel_rocket; // m/s^2
    Eigen::Vector3f accel_ground; // m/s^2
    /*
    Orientation quaternion represents the rotation of the rocket, from the ground-based coordinate system. The quaternion is expected to be normalized by the time any states deal with it. */
    Eigen::Quaternionf orientation;
};

struct StateAuxilliaryInfo {
    bool in_abort = false;
};

#endif
