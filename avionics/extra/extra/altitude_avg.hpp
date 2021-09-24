#ifndef EXTRA_ALTITUDE_AVG_HPP_FD593D19A99D494D953E99E48E3C89BF
#define EXTRA_ALTITUDE_AVG_HPP_FD593D19A99D494D953E99E48E3C89BF

#include "CalcHelpers/barometric.hpp"
#include "CalcHelpers/exponential_moving_avg.hpp"
#include "CalcHelpers/simple_moving_avg.hpp"
#include "HAL/time.h"
#include "sensor_collection.h"
#include "sensors/barometer.h"
#include "state_id_enum.hpp"

namespace extra {
namespace estimator {

class AltitudeAvg {
    // following recomendation for alpha = 2/(N + 1)
    constexpr static float BASE_MOVING_AVERAGE_ALPHA = 2.0 / (200 + 1);
    constexpr static float MOVING_AVERAGE_VARIANCE = 1; // [m]

  public:
    AltitudeAvg(SensorCollection &sensors, Hal::t_point initial_time);

    /**
     * @brief Update estimator with new information.
     */
    virtual void update(Hal::t_point t_ms);

    /**
     * @brief Get baseline altitude (i.e. altitude of launch site above sea
     * level)
     * @return Base altitude, in metres
     */
    float altitudeBase() const;

    /**
     * @brief Estimate of current altitude above ground level
     * @return Altitude [metres]
     */
    float altitude() const;

    /**
     * @brief Estimate of current velocity in z direction (vertical)
     * @return Velocity [m/s]
     */
    float velocityGroundZ() const;

    /**
     * @brief Get time that calc object was initialized
     * @return init_time [Hal::t_point]
     */
    Hal::t_point getInitTime() const;

  private:
    float rawAltitude() const;

    Barometer &baro_;
    ExponentialMovingAvg<float> base_alt_;
    SimpleMovingAvg<float, 5> agl_alt_; // AGL = above ground level

    float velocity_z_;

    float last_agl_alt_;
    Hal::t_point last_t_;
    Hal::t_point init_time;
};

} // namespace estimator
} // namespace extra

#endif
