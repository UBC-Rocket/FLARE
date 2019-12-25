#ifndef STATES__STATE_CONSTANTS_CONFIG_H
#define STATES__STATE_CONSTANTS_CONFIG_H

/*Constants------------------------------------------------------------*/

constexpr uint8_t STANDBY_LAUNCH_CHECKS = 4;
constexpr uint8_t ARMED_LAUNCH_CHECKS = 4;
constexpr uint8_t MOTOR_BURNOUT_CHECKS = 4;
constexpr uint8_t MACH_LOCK_CHECKS = 10;
constexpr uint8_t MACH_UNLOCK_CHECKS = 10;
constexpr uint8_t APOGEE_CHECKS = 5;
constexpr uint8_t MAIN_DEPLOY_CHECKS = 5;

constexpr uint32_t POST_BURNOUT_COAST_TIME = 3000; //ms
constexpr float PRESTAGE_MAX_ACCEPT_ALTITUDE = 5000; //m
constexpr float PRESTAGE_MIN_ACCEPT_ALTITUDE = 1000; //m
constexpr float PRESTAGE_MAX_ACCEPT_VERTICAL_ACCEL = 20; //m/s^2
constexpr float PRESTAGE_MIN_ACCEPT_VERTICAL_ACCEL = -50; //m/s^2
constexpr float PRESTAGE_MAX_ACCEPT_ANGLE_FROM_VERTICAL = 1.57; //radians

constexpr float LAUNCH_THRESHOLD = 25; //m
constexpr float MACH_LOCK_VELOCITY_THRESHOLD = 155; //m/s
constexpr float MACH_UNLOCK_VELOCITY_THRESHOLD = 150; //m/s
constexpr float MAIN_DEPLOY_ALTITUDE = 488; //m - converted from 1500 ft

constexpr uint32_t LAND_CHECK_TIME_INTERVAL = 10000; //ms
constexpr uint8_t LAND_CHECKS = 6;
constexpr float LAND_VELOCITY_THRESHOLD = 4;

constexpr uint32_t APOGEE_PRESSURE_DELAY = 3000;
constexpr int LANDING_TIME_INTERVAL = 10000;
constexpr int TOGGLE_CAMERA_INTERVAL = 200;

#endif
