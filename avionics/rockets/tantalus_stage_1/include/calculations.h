#ifndef CALCULATIONS_H_62A4726C965C45AAA01550CD5DF18D99
#define CALCULATIONS_H_62A4726C965C45AAA01550CD5DF18D99

#include <cmath> //std::abs

#include "calculations_interface.h"
#include "state_input_struct.h"
#include "CalcHelpers/exponential_moving_avg.h"
#include "CalcHelpers/barometric.h"

#include "sensors/barometer.h"
#include "sensors/IMU.h"

//following recomendation for alpha = 2/(2N + 1)
constexpr float BAROMETER_MOVING_AVERAGE_ALPHA = 2.0f / (2 * 40 + 1);

class Calculator : public ICalculator{
public:
    Calculator(Barometer const *baro, IMU const *imu) : m_baro(baro), m_imu(imu)m_base_alt(baro->get_data()){}

    void calculateValues(StateId const state, StateInput &out_state_input) {
        const float alt = pressureToAltitude(baro->get_data());
        const float diff = std::abs(m_base_alt.getAverage() - alt);

        if((state == StateId::STANDBY || state == StateId::ARMED)
            && diff < m_base_alt.getStandardDeviation() * 3)
        ){
            m_base_alt.addValue(alt);
        }
        out_state_input.altitude = m_base_alt.getAverage();

    }

private:
    ExponentialMovingAvg<float> m_base_alt;
    Barometer *m_baro;
    IMU *m_imu;
}

#endif
