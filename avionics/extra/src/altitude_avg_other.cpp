#include "extra/altitude_avg_other.hpp"

namespace extra {
namespace estimator {

public AltitudeAvgOther::AltitudeAvgOther(SensorCollection &sensors, Hal::t_point initial_time) : AltitudeAvg(sensors, initial_time) {};
    
public void update(Hal::t_point t_ms) override {
    const float raw_alt = rawAltitude();
    
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
};   


} // namespace estimator
} // namespace extra
