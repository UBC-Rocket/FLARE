#pragma once

#include "extra/altitude_avg.hpp"
#include "extra/altitude_avg_other.hpp"
#include "extra/altitude_avg_standby_armed.hpp"

typedef extra::estimator::AltitudeAvg Calculator;
typedef extra::estimator::AltitudeAvgOther CalculatorOther;
typedef extra::estimator::AltitudeAvgStandbyArmed CalculatorStandbyArmed;
