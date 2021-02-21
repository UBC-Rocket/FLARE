#pragma once
#include <cmath>

/**
 * @brief Returns the altitude above sea-level given pressure.
 * See https://en.wikipedia.org/wiki/Barometric_formula
 * @param pascals Pressure, in [Pa]
 * @return Altitude above sea level [m]
 */
float pressureToAltitude(float const pascals);
