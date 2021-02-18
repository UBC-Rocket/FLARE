#pragma once

#include "extra/altitude_avg.hpp"

typedef extra::estimator::AltitudeAvg Calculator;


// #undef abs // TODO This macro gets defined somewhere with Teensy and I'm not
//            // sure where or whether there's a better way around it

// #include <cmath> //std::abs

// #include <Eigen/Geometry> //quaternions

// #include "CalcHelpers/barometric.hpp"
// #include "calculations_interface.h"
// #include "sensor_collection.h"
// #include "sensors/IMU.h"
// #include "sensors/barometer.h"
// #include "state_input_struct.h"

// #undef abs

// // following recomendation for alpha = 2/(2N + 1)
// constexpr float BAROMETER_MOVING_AVERAGE_ALPHA = 2.0f / (2 * 40 + 1);

// class Calculator : public ICalculator {
//   public:
//     Calculator(SensorCollection &sensors)
//         : baro_(sensors.barometer), imu_(sensors.imuSensor) {
//         baro_.readData();
//         base_alt_.reset(pressureToAltitude(*(baro_.getData())), 1,
//                         BAROMETER_MOVING_AVERAGE_ALPHA);
//         last_t_ = Hal::now_ms();
//         altitude_ = base_alt_.getAverage();
//     }

//     void calculateValues(StateId const state, StateInput &out_state_input,
//                          Hal::t_point t_ms) {
//         StateInput &out = out_state_input; // alias

//         /* Update altitude average and calculate altitude*/
//         const float alt = pressureToAltitude(*(baro_.getData()));
//         const float diff = std::abs(base_alt_.getAverage() - alt);

//         if ((state == StateId::STANDBY || state == StateId::ARMED) &&
//             diff < base_alt_.getStandardDeviation() * 3) {
//             base_alt_.addValue(alt);
//         }
//         auto new_alt = alt - base_alt_.getAverage();

//         constexpr int MILLISECONDS_PER_SECOND = 1000;
//         velocity_z_ = (new_alt - altitude_) / (t_ms - last_t_).count() *
//                       MILLISECONDS_PER_SECOND;

//         altitude_ = new_alt;

//         // t_ms and last_t_ are of millisconds
//         last_t_ = t_ms;
//         out.altitude = altitude_; // in preparation for next loop
//         out.velocity_vertical = velocity_z_;

//         /* Do some math to figure out orientation & acceleration in relevant
//          * frames*/
//         // TODO - check state to only do these calculations when necessary
//         // There may be some really clever way to do this with pointers, or at
//         // least use comma initialization
//         out.accel_rocket(0) =
//             imu_.getData()[0]; //? TODO - make sure these are aligned
//         out.accel_rocket(1) = imu_.getData()[1]; //?
//         out.accel_rocket(2) = imu_.getData()[2]; //?

//         Eigen::Quaternionf meas_orient(1, 0, 0, 0);
//         // TODO - initialize measured orientation with imu_'s quaternion data
//         // instead of dummy data

//         // conjugate is equal to the inverse for unit quaternion
//         out.orientation = meas_orient * gnd_quat_.conjugate();
//         out.orientation.normalize();

//         // convert into quaternion for operation
//         const Eigen::Quaternionf xl_rocket_quat(
//             0, out.accel_rocket(0), out.accel_rocket(1),
//             out.accel_rocket(2)); // There may be some really clever way to do
//                                   // this with more pointers

//         // conjugate to rotate
//         const Eigen::Quaternionf xl_gnd_quat =
//             out.orientation.conjugate() * xl_rocket_quat * out.orientation;

//         // turn back into vector
//         out.accel_ground << xl_gnd_quat.x(), xl_gnd_quat.y(), xl_gnd_quat.z();
//     }

//     /**
//      * @brief Get baseline altitude (i.e. altitude of launch site above sea
//      * level)
//      * @return Base altitude, in metres
//      */
//     float getBaseAltitude() const { return base_alt_.getAverage(); }

//     /**
//      * @brief Estimate of current altitude
//      * @return Altitude [metres]
//      */
//     float altitude() const { return altitude_; }

//     /**
//      * @brief Estimate of current velocity in z direction (vertical)
//      * @return Velocity [m/s]
//      */
//     float velocity_z() const { return velocity_z_; }

//   private:
//     ExponentialMovingAvg<float> base_alt_;
//     Barometer &baro_;
//     float altitude_;
//     float velocity_z_;

//     Hal::t_point last_t_;

//     IMU &imu_;

//     Eigen::Quaternionf gnd_quat_;
// };
