#include "CalcHelpers/barometric.hpp"
#include "gtest/gtest.h"

// Values taken from https://www.mide.com/air-pressure-at-altitude-calculator
TEST(Barometric, Tabulated) {
    ASSERT_NEAR(pressureToAltitude(1000), 110.88, 1);
    ASSERT_NEAR(pressureToAltitude(900), 988.50, 1);
    ASSERT_NEAR(pressureToAltitude(800), 1948.99, 1);
    ASSERT_NEAR(pressureToAltitude(700), 3012.18, 1);
    ASSERT_NEAR(pressureToAltitude(600), 4206.43, 1);
    ASSERT_NEAR(pressureToAltitude(500), 5574.44, 1);
}
