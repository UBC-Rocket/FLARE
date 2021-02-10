#ifndef STATE_INPUT_STRUCT_HPP_4A01F791ECBA4E3CB8BC12436C41ABC9
#define STATE_INPUT_STRUCT_HPP_4A01F791ECBA4E3CB8BC12436C41ABC9

#include "Eigen/Core"
#include "Eigen/Geometry"

struct StateInput {
    float altitude;          // metres
    float velocity_vertical; // m/s, vertical
};

struct StateAuxilliaryInfo {};

#endif
