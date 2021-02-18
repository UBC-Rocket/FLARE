#include "gtest/gtest.h"
#include <random>
#include <vector>

// // Mock time. Only used on averager construction
// namespace Hal {
// t_point now_ms() { return t_point(ms(-50)); }
// } // namespace Hal

// Mock sensor collections and barometer
class Barometer {
  public:
    void readData() {}
    double *getData() { return &pressure_; }

    double pressure_;
};

struct SensorCollection {
    Barometer barometer;
};

// Define header guards so actual classes don't appear
#define SENSOR_COLLECTION_HPP_C433414CA96E4AB8A2065CE9C7E10AA3
#define BAROMETER_HPP_E5BACDD72523463B88A01C9A5C9F31FE
#include "extra/altitude_avg.hpp"

#include "extra/../src/altitude_avg.cpp"

TEST(AltitudeAvg, Application) {
    SensorCollection sensors;
    Barometer &baro = sensors.barometer;

    // Corresponds to ~ 1600 m above sea level
    baro.pressure_ = 835.50;
    extra::estimator::AltitudeAvg estimator(sensors,
                                            Hal::t_point(Hal::ms(-50)));

    // Standard mersenne_twister_engine with consistent seed
    std::mt19937 gen(314159);
    std::uniform_real_distribution<> dis(835.00, 836.00);

    // Develop initial state
    int n = 0;
    for (; n < 1000; ++n) {
        baro.pressure_ = dis(gen);
        estimator.update(StateId::STANDBY, Hal::t_point(Hal::ms(n * 50)));
    }

    EXPECT_NEAR(estimator.altitudeBase(), 1597, 10);
    EXPECT_NEAR(estimator.altitude(), 0, 10);
    EXPECT_NEAR(estimator.velocityGroundZ(), 0, 10);

    // Base altitude shouldn't change in other states
    auto ground_alt = estimator.altitudeBase();

    for (; n < 1050; ++n) {
        baro.pressure_ = 835.20;
        estimator.update(StateId::LANDED, Hal::t_point(Hal::ms(n * 50)));
    }

    EXPECT_EQ(estimator.altitudeBase(), ground_alt);

    // Base altitude shouldn't change with drastic updates, but AGL alt should.
    // Simulate upwards motion @ 200 m/s, data rate 20 Hz
    std::vector<float> const data{835.24, 834.21, 833.18, 832.16, 831.13, 830.11,
                                  829.09, 828.09, 827.05, 826.03, 825.01, 824.00,
                                  822.98, 821.97, 820.95, 819.94, 818.93, 817.92,
                                  816.91, 815.90, 814.89};
    for (auto dat : data) {
        baro.pressure_ = dat;
        estimator.update(StateId::ASCENT_TO_APOGEE,
                         Hal::t_point(Hal::ms(n * 50)));
        ++n;
    }

    EXPECT_NEAR(estimator.altitudeBase(), 1597, 10);
    EXPECT_NEAR(estimator.velocityGroundZ(), 200, 20);
    EXPECT_NEAR(estimator.altitude(), 200, 20);
}
