#include "CalcHelpers/barometric.hpp"

/**
 * @brief Returns the altitude above sea-level given pressure.
 * See https://en.wikipedia.org/wiki/Barometric_formula
 * @param pascals Pressure, in [Pa]
 * @return Altitude above sea level [m]
 */
float pressureToAltitude(float const pres) {
    constexpr float Pb = 101325;
    constexpr float Tb = 288.15;
    constexpr float Lb = -0.0065;
    constexpr float R = 8.3144598;
    constexpr float g0 = 9.80665;
    constexpr float M = 0.0289644;

    return Tb / Lb * (std::pow(Pb / pres, R * Lb / (g0 * M)) - 1);
}
