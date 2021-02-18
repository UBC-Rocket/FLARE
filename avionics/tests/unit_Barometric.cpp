#include "CalcHelpers/barometric.hpp"
#include "gtest/gtest.h"

// Values taken from https://www.mide.com/air-pressure-at-altitude-calculator
TEST(Barometric, Tabulated) {
    ASSERT_NEAR(pressureToAltitude(100000), 110.88, 1);
    ASSERT_NEAR(pressureToAltitude(90000), 988.50, 1);
    ASSERT_NEAR(pressureToAltitude(80000), 1948.99, 1);
    ASSERT_NEAR(pressureToAltitude(70000), 3012.18, 1);
    ASSERT_NEAR(pressureToAltitude(60000), 4206.43, 1);
    ASSERT_NEAR(pressureToAltitude(50000), 5574.44, 1);
}
