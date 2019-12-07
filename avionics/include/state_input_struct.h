#ifndef STATE_INPUT_STRUCT_H
#define STATE_INPUT_STRUCT_H

#include "Eigen/Core"

struct StateInput {
    bool ignitor_good = false;
    float altitude;
    Eigen::Vector3f acceleration;
};

struct StateAuxilliaryInfo {
    bool in_abort = false;
}

#endif
