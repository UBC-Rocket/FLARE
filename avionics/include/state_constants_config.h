#ifndef STATES__STATE_CONSTANTS_CONFIG_H
#define STATES__STATE_CONSTANTS_CONFIG_H

/*Constants------------------------------------------------------------*/

constexpr int STANDBY_LAUNCH_CHECKS = 4;
constexpr int ARMED_LAUNCH_CHECKS = 4;

constexpr int MACH_LOCK_CHECKS = 2;

constexpr int APOGEE_CHECKS = 5;
constexpr int MACH_LOCK_CHECKS = 25;
constexpr int MAIN_CHECKS = 10;
constexpr int LAND_CHECKS = 6;
constexpr int LAUNCH_THRESHOLD = 25;
constexpr int MACH_THRESHOLD = 155;
constexpr int MACH_LOCK_THRESHOLD = 150;
constexpr int FINAL_DESCENT_THRESHOLD = 488;
constexpr int LAND_VELOCITY_THRESHOLD = 4;

constexpr int APOGEE_DELAY = 3000;
constexpr int LANDING_TIME_INTERVAL = 10000;
constexpr int TOGGLE_CAMERA_INTERVAL = 200;

#endif
