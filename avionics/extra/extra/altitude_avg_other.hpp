#include "extra/altitude_avg.hpp"

namespace extra {
namespace estimator {

class AltitudeAvgOther : public AltitudeAvg {
    public void update(Hal::t_point t_ms) override;
}

} // namespace estimator
} // namespace extra
