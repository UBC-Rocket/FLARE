#pragma once

#undef abs // TODO This macro gets defined somewhere with Teensy and I'm not
           // sure where or whether there's a better way around it

#include <cmath> //std::abs

#include "CalcHelpers/barometric.hpp"
#include "CalcHelpers/exponential_moving_avg.h"
#include "calculations_interface.h"
#include "sensor_collection.h"
#include "sensors/barometer.h"
#include "state_input_struct.h"

#undef abs

// following recomendation for alpha = 2/(2N + 1)
constexpr float BAROMETER_MOVING_AVERAGE_ALPHA = 2.0f / (2 * 40 + 1);

class Calculator : public ICalculator {
  public:
    Calculator(SensorCollection &sensors) : baro_(sensors.barometer) {
        baro_.readData();
        base_alt_.reset(pressureToAltitude(*(baro_.getData())), 1,
                        BAROMETER_MOVING_AVERAGE_ALPHA);
        last_t_ = Hal::now_ms();
        last_alt_ = base_alt_.getAverage();
    }

    void calculateValues(StateId const state, StateInput &out_state_input,
                         Hal::t_point t_ms) {
        StateInput &out = out_state_input; // alias

        /* Update altitude average and calculate altitude*/
        const float alt = pressureToAltitude(*(baro_.getData()));
        const float diff = std::abs(base_alt_.getAverage() - alt);

        if ((state == StateId::STANDBY || state == StateId::ARMED) &&
            diff < base_alt_.getStandardDeviation() * 3) {
            base_alt_.addValue(alt);
        }
        out.altitude = alt - base_alt_.getAverage();

        constexpr int MILLISECONDS_PER_SECOND = 1000;
        out.velocity_vertical = (out.altitude - last_alt_) /
                                (t_ms - last_t_).count() *
                                MILLISECONDS_PER_SECOND;
        // t_ms and last_t_ are of millisconds

        last_alt_ = out.altitude; // in preparation for next loop
        last_t_ = t_ms;
    }

    /**
     * @brief Get baseline altitude (i.e. altitude of launch site above sea
     * level)
     * @return Base altitude, in metres
     */
    float getBaseAltitude() const { return base_alt_.getAverage(); }

    /**
     * @brief Estimate of current altitude
     * @return Altitude [metres]
     */
    float altitude() const { return last_alt_; }

  private:
    ExponentialMovingAvg<float> base_alt_;
    Barometer &baro_;
    float last_alt_;
    Hal::t_point last_t_;
};
