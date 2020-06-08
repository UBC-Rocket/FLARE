#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>

/**
 * Kalman filter that only uses altitude measurements, and internally stores
 * vertical velocity and position as its state. Propagation model assumes a
 * constant gravitational acceleration downwards, and no other force.
 */
class AltitudeKf {
  public:
    // x0 - initial state
    // P0 - initial noise covariance
    Kf(Eigen::Vector2f x0, Eigen::Matrix2f P0) : m_x(x0), m_P(P0) {
        m_H << 1, 0;
    }

    // dt (change in time from last step) in seconds
    void predict(float dt) {
        m_F << 1, dt, 0, 1;
        m_B << -0.5 * M_GRAVITY_ACCEL * dt * dt, M_GRAVITY_ACCEL * dt;
        /*
        Error in position is  0.5 * a * t^2, since x = v0 * t + 0.5*a*t^2.
        Unjustifiably, let acceleration be uniformly distributed from -1 to 1
        m/s^2 (+ incorporated acceleration). Variance of acceleration = 2^2/12 =
        0.333, variance of position is 0.5 dt^2 * var(a).
        */
        m_Q << 0.333 * 0.5 * dt * dt, 0, 0, 0.333 * dt;

        m_x = m_F * m_x + m_B;
        m_P = m_F * m_P * m_F.transpose() + m_Q;
    }

    // Barometric altimeter has a nonlinear and complicated transformation from
    // sensor space to altitude. Easier probably to transform pressure reading
    // to altitude first, then use Hk as necessary to select specific fields
    // that can be measured.
    void update(float measuredAlt) {
        m_z << measuredAlt;
        m_R << M_BAROM_UNCERTAINTY;
        m_y = m_z - m_H * m_x;
        m_S = m_H * m_P * m_H.transpose() + m_R;
        m_K = m_P * m_H.transpose() * (m_S.inverse());
        // m_K = m_P * m_H.transpose() * (1 / m_S(0,0)); //TODO - figure out why
        // previous commented out line doesn't work
        m_x = m_x + m_K * m_y;

        // Use Joseph Form
        m_A = Eigen::Matrix2f::Identity() - m_K * m_H;
        m_P = m_A * m_P * m_A.transpose() + m_K * m_R * m_K.transpose();
    }

    /*
     * Does predict and update step together.
     * Returns altitude.
     */
    float predictUpdate(float dt, float measuredAlt) {
        predict(dt);
        update(measuredAlt);
        return m_x(0);
    }

    float getAltitude() { return m_x(0); }
    float getVelocity() { return m_x(1); }

  private:
    static constexpr float M_BAROM_UNCERTAINTY = 0.9;
    static constexpr float M_GRAVITY_ACCEL = 9.81;

    // In what is arguably an attempt at consistency - all of the terms used
    // during the predict/update cycle are kept as member variables; otherwise
    // we'd have some that need to be member variables (e.g. x, P, F) and some
    // that aren't, with no clear indication of what is a member variable and
    // what isn't
    Eigen::Matrix<float, 2, 1> m_x;
    Eigen::Matrix<float, 2, 2> m_P;
    Eigen::Matrix<float, 2, 2> m_F;
    Eigen::Matrix<float, 2, 1> m_B;
    Eigen::Matrix<float, 2, 2> m_Q;
    Eigen::Matrix<float, 1, 1> m_z;
    Eigen::Matrix<float, 1, 1> m_R;
    Eigen::Matrix<float, 1, 2> m_H;
    Eigen::Matrix<float, 1, 1> m_y;
    Eigen::Matrix<float, 1, 1> m_S;
    Eigen::Matrix<float, 2, 1> m_K;

    // Used in Joseph form as I - K * H
    Eigen::Matrix<float, 2, 2> m_A;
};
