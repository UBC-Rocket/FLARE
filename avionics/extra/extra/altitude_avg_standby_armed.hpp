#include "extra/altitude_avg.hpp"

namespace extra {
namespace estimator {

class AltitudeAvgStandbyArmed : public AltitudeAvg {
    public:
        AltitudeAvgStandbyArmed(SensorCollection &sensors, Hal::t_point initial_time) : AltitudeAvg(sensors, initial_time);

        void update(Hal::t_point t_ms) override;
};

} // namespace estimator
} // namespace extra
