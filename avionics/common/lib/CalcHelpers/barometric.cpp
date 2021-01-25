#include "CalcHelpers/barometric.hpp"

float pressureToAltitude(float const pres) {
    constexpr float Pb = 101325;
    constexpr float Tb = 288.15;
    constexpr float Lb = -0.0065;
    constexpr float R = 8.3144598;
    constexpr float g0 = 9.80665;
    constexpr float M = 0.0289644;

    return Tb / Lb * (std::pow(Pb / pres, R * Lb / (g0 * M)) - 1);
}
