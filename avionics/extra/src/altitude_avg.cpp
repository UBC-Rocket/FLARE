#undef abs
#include <cmath> //std::abs

#include "CalcHelpers/barometric.hpp"
#include "extra/altitude_avg.hpp"
#include "options.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif
#undef abs

namespace extra {
namespace estimator {

AltitudeAvg::AltitudeAvg(SensorCollection &sensors, Hal::t_point initial_time)
    : baro_(sensors.barometer),
      // Using the comma operator to execute readData() before continuing
      base_alt_((baro_.readData(), rawAltitude()), MOVING_AVERAGE_VARIANCE,
                BASE_MOVING_AVERAGE_ALPHA),
      agl_alt_(0), velocity_z_(0), last_agl_alt_(0), last_t_(initial_time) {
        #ifdef TESTING
        Serial.print("base alt (avg): ");
        Serial.println(base_alt_.getAverage());
        Serial.print("agl alt (avg): ");
        Serial.println(agl_alt_.getAverage());
        #endif
      }

void AltitudeAvg::update(StateId const state, Hal::t_point t_ms) {
    const float raw_alt = rawAltitude();

    // Update base alt if appropriate
    if (state == StateId::STANDBY || state == StateId::ARMED) {
        const float diff = std::abs(base_alt_.getAverage() - raw_alt);
        if (diff < base_alt_.getStandardDeviation() * 3) {
            base_alt_.addValue(raw_alt);
        }
    }

    // Update AGL altitude
    const float raw_agl_alt = raw_alt - base_alt_.getAverage();
    agl_alt_.addValue(raw_agl_alt);

    // Update velocity
    constexpr int MILLISECONDS_PER_SECOND = 1000;
    velocity_z_ = (altitude() - last_agl_alt_) / (t_ms - last_t_).count() *
                  MILLISECONDS_PER_SECOND;

    // Update last iteration info
    last_agl_alt_ = altitude();
    last_t_ = t_ms;
}

float AltitudeAvg::altitudeBase() const { return base_alt_.getAverage(); }

float AltitudeAvg::altitude() const { return agl_alt_.getAverage(); }

float AltitudeAvg::velocityGroundZ() const { return velocity_z_; }

float AltitudeAvg::rawAltitude() const {
    return pressureToAltitude(*(baro_.getData()));
}

} // namespace estimator
} // namespace extra
