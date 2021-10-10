#include "extra/altitude_avg.hpp"

namespace extra {
namespace estimator {

class AltitudeAvgOther : public AltitudeAvg {
    public AltitudeAvgOther(SensorCollection &sensors, Hal::t_point initial_time) : AltitudeAvg(sensors, initial_time);

    public void update(Hal::t_point t_ms) override;
};

} // namespace estimator
} // namespace extra
