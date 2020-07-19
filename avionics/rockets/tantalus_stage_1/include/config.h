#pragma once

/* TODO - MAKE THIS NOT A CONSTANT*/
#include "state_input_struct.h"

static StateInput state_input;
static StateAuxilliaryInfo state_aux;

/* Sensors */
#include "sensors/GPS.h"
#include "sensors/IMU.h"
#include "sensors/accelerometer.h"
#include "sensors/barometer.h"
#include "sensors/temperature.h"
#include "sensors_interface.h"

namespace SensorPositions {
constexpr uint8_t BAROMETER = 0;
constexpr uint8_t GPS = 1;
constexpr uint8_t ACCELEROMETER = 2;
constexpr uint8_t IMU = 3;
constexpr uint8_t TEMPERATURE = 4;
} // namespace SensorPositions
