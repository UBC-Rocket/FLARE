/*State Machine Header*/
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

/*Includes------------------------------------------------------------*/

/*Constants------------------------------------------------------------*/
#define MIN_CHECKS 5

#define LAUNCH_THRESHOLD 12
#define LAUNCH_INTERRUPT_PIN 0

#define COAST_THRESHOLD 5
#define FINAL_DESCENT_THRESHOLD 200 //meters
#define LAND_HEIGHT_THRESHOLD 150 // meters
#define LANDING_THRESHOLD_HIGH 1.06
#define LANDING_THRESHOLD_LOW 0.94
#define LAND_CHECKS     20
#define APOGEE_CHECKS   5
#define COAST_CHECKS    10
#define MAIN_CHECKS     10

#define BASELINE_PRESSURE 1012 //TODO: calibrate

/*Variables------------------------------------------------------------*/
enum FlightStates {
  STANDBY,
  ARMED,
  POWERED_ASCENT,
  APOGEE_DETECT,
  INITIAL_DESCENT,
  FINAL_DESCENT,
  LANDED
};

/*Functions------------------------------------------------------------*/
void launchInterrupt();
void switchState(FlightStates new_state);
void stateMachine(float abs_accel, float altitude, float delta_altitude);

#endif
