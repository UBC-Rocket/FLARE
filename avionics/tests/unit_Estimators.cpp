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
    baro.pressure_ = 83550;
    extra::estimator::AltitudeAvg estimator(sensors,
                                            Hal::t_point(Hal::ms(-50)));

    // Standard mersenne_twister_engine with consistent seed
    std::mt19937 gen(314159);
    std::uniform_real_distribution<> dis(83500, 83600);

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
        baro.pressure_ = 83520;
        estimator.update(StateId::LANDED, Hal::t_point(Hal::ms(n * 50)));
    }

    EXPECT_EQ(estimator.altitudeBase(), ground_alt);

    // Base altitude shouldn't change with drastic updates, but AGL alt should.
    // Simulate upwards motion @ 200 m/s, data rate 20 Hz
    std::vector<float> const data{83524, 83421, 83318, 83216, 83113, 83011,
                                  82909, 82809, 82705, 82603, 82501, 82400,
                                  82298, 82197, 82095, 81994, 81893, 81792,
                                  81691, 81590, 81489};
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
