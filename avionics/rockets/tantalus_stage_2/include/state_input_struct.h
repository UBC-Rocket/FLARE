#pragma once

#include "Eigen/Core"
#include "Eigen/Geometry"

struct StateInput {
    float altitude;          // metres
    float velocity_vertical; // m/s, vertical
};

struct StateAuxilliaryInfo {};
