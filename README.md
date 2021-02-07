[![Build Status](https://circleci.com/gh/UBC-Rocket/FLARE.svg?style=shield)](https://app.circleci.com/pipelines/github/UBC-Rocket/FLARE)

# IMPORTANT WARNINGS

**VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!**

                     uuuuuuu
                 uu$$$$$$$$$$$uu
              uu$$$$$$$$$$$$$$$$$uu
             u$$$$$$$$$$$$$$$$$$$$$u
            u$$$$$$$$$$$$$$$$$$$$$$$u
           u$$$$$$$$$$$$$$$$$$$$$$$$$u
           u$$$$$$$$$$$$$$$$$$$$$$$$$u
           u$$$$$$"   "$$$"   "$$$$$$u
           "$$$$"      u$u       $$$$"
            $$$u       u$u       u$$$
            $$$u      u$$$u      u$$$
             "$$$$uu$$$   $$$uu$$$$"
              "$$$$$$$"   "$$$$$$$"
                u$$$$$$$u$$$$$$$u
                 u$"$"$"$"$"$"$u
      uuu        $$u$ $ $ $ $u$$       uuu
     u$$$$        $$$$$u$u$u$$$       u$$$$
      $$$$$uu      "$$$$$$$$$"     uu$$$$$$
    u$$$$$$$$$$$uu    """""    uuuu$$$$$$$$$$
    $$$$"""$$$$$$$$$$uuu   uu$$$$$$$$$"""$$$"
     """      ""$$$$$$$$$$$uu ""$"""
               uuuu ""$$$$$$$$$$uuu
      u$$$uuu$$$$$$$$$uu ""$$$$$$$$$$$uuu$$$
      $$$$$$$$$$""""           ""$$$$$$$$$$$"
       "$$$$$"                      ""$$$$""
         $$$"                         $$$$"

In order to successfully poll the GPS, the serial RX buffer size must be increased. This needs
to be done on the computer used for compilation. This can be done by navigating to the following
path in the Arduino contents folder:

On Mac: Got to the Applications folder, right click on the Arduino app, select Show Package Contents,
    then navigate to ‎⁨Contents⁩/⁨Java⁩/⁨hardware⁩/⁨teensy⁩/⁨avr⁩/⁨cores⁩/⁨teensy3⁩/serial1.c
    
On Windows: [user_drive]\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\serial1.c

On line 43 increase SERIAL1_RX_BUFFER_SIZE from 64 to 1024

THIS MUST BE DONE ON THE COMPUTER USED TO COMPILE THE CODE!!!

**VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!**

***
***


# Definable stuff
All definable macros are located in "options.h".

### Nosecone/Body
For the 2018-2019 work year, the rocket has two avionics systems; one in the nosecone and one in the body.
The specific functions they can perform are as follows:

|Nosecone            | Body|
|---- | ----|
| Log all sensor data | Log all sensor data |
|                    | Communicate with SATCOM
|                    | Utilize GPS
| Uses `body` radio - key flight data is sent | Uses `nosecone` radio functions - less important data is sent|
|Runs state machine and controls deployment actuators (e.g. servos, ematch) | Runs state machine without control of actuators |
| | Accepts unique nosecone commands from radio _(not implemented as of Mar 2)_ |

As a hack, you can undefine NOSECONE to deactivate SATCOM (e.g. since SATCOM is blocking)

Note that precisely one of these options must be defined - otherwise, a custom error will be raised at compile.

### Servo/Pow
There are two systems in use for separating the rocket and deploying the drogue chute.

One system is a CO2 canister that is punctured by a servo actuator, and the other system is a black powder charge.

During nominal flight conditions, only define servo OR pow and not both depending on which drogue system is being used.

Note that precisely one of these options must be defined - otherwise, a custom error will be raised at compile.

### Testing
When defined, enables debugging output. All polled sensor values are printed to USB, and a 1 second delay is added to the main Arduino loop function. **DO NOT HAVE THIS DEFINED WHEN FLASHING TO FLIGHT PCBS**

### Ground Test
When defined, eanbles radio-activated deployment of the main and drogue parachutes. This is to allow for deployment tests while on the ground. GROUND_TEST also enables other features facilitating testing on the ground. **DO NOT HAVE THIS DEFINED WHEN FLASHING TO FLIGHT PCBS**

# LEDs
Pin 13 -> built-in LED (labeled LED3)
  The LED will be on if all sensors have been successfully initialized
  The LED will blink if there is a non-critical failure
  The LED will not turn on if there is a critical failure

Pin 20 -> Power status LED (labeled LED2)
  The LED will turn on if there is power to the microcontroller

Pin 21 -> Flight status LED (labeled LED1)
  The LED will turn on in ARMED, upon hitting Apogee, and upon deploying the main chute
  The LED will be off in the Standby, Ascent, Mach Lock, Initial Descent, and Landed states.

# Compilation of temporary things
>“Nothing is more permanent than a temporary solution.” -Russian proverb

## To-dos and other commented out code
*       sensors.cpp: InitSensors: TODO: Uncomment once the battery sensor is implemented
Do not uncomment as of April 21 2019 - the battery sensor is not implemented for Hanna 2019 (IREC as of yet uncertain)

*       commands.cpp: doCommand	: case STARTUP_BUZZER, case RECOVERY_BUZZER
Fine as is. These commands are not implemented for Hanna 2019, potentially will be implemented for ease of testing.

*       sensors.h: (define block)
TODO: label "Accelerometer Scale" and move the battery sensor pin defn to GPIO.h (Apr21,2019)

*

## Parameters
### battery.h:
As of April 21 2019 these are not implemented (battery voltage measurements not measured)

        #define MINIMUM_BATTERY_VOLTAGE 10
Battery voltage below which the rocket cannot reliably function throughout the flight, will raise a critical failure.

        #define LOW_BATTERY_VOLTAGE 11

Battery voltage below which the rocket will have a risk of failure, will raise a noncritical failure.

        #define R1 44170
        #define R2 15150

Measured resistor values in the voltage divider. More information in comments in file.

### calculations.h:
        #define PRESSURE_AVG_SET_SIZE   15
Number of values the moving average filter includes, during flight

### groundaltitude.h:
        #define GROUND_ALT_SIZE 60
Number of values the moving average filter includes, on the ground (waiting for launch)

### gpio.h
        (literally all of the pin assignments)
TRIPLE CHECK ALL OF THESE

There's a couple of other parameters in there that are useful/important to check

### IridiumSBD.h
        #define ISBD_DEFAULT_SENDRECEIVE_TIME   200  //300
        #define ISBD_STARTUP_MAX_TIME           15 //240
Modified from defaults to match our needs better.

### statemachine.h
        (all of the parameters given in the define block)
These should all be checked with relavent people in recovery, etc. before flashing.

