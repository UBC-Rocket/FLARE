#undef abs
#include <cmath> //std::abs

#include "CalcHelpers/barometric.hpp"
#include "extra/altitude_avg.hpp"
#undef abs

namespace extra {
namespace estimator {

AltitudeAvg::AltitudeAvg(SensorCollection &sensors, Hal::t_point initial_time)
    : baro_(sensors.barometer),
      // Using the comma operator to execute readData() before continuing
      base_alt_((baro_.readData(), rawAltitude()), MOVING_AVERAGE_VARIANCE,
                BASE_MOVING_AVERAGE_ALPHA),
      agl_alt_(0), velocity_z_(0), last_agl_alt_(0), last_t_(initial_time) {}

virtual void AltitudeAvg::update(Hal::t_point t_ms) = 0;

float AltitudeAvg::altitudeBase() const { return base_alt_.getAverage(); }

float AltitudeAvg::altitude() const { return agl_alt_.getAverage(); }

float AltitudeAvg::velocityGroundZ() const { return velocity_z_; }

float AltitudeAvg::rawAltitude() const {
    return pressureToAltitude(*(baro_.getData()));
};

} // namespace estimator
} // namespace extra
