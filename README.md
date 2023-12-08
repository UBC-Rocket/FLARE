[![Build Status](https://circleci.com/gh/UBC-Rocket/FLARE.svg?style=shield)](https://app.circleci.com/pipelines/github/UBC-Rocket/FLARE)

## Definable stuff
All definable macros are located in "options.h".

### Testing
When defined, enables debugging output. All polled sensor values are printed to USB. **DO NOT HAVE THIS DEFINED WHEN FLASHING TO FLIGHT PCBS**

### Ground Test
When defined, eanbles radio-activated deployment of the main and drogue parachutes. This is to allow for deployment tests while on the ground. GROUND_TEST also enables other features facilitating testing on the ground. **DO NOT HAVE THIS DEFINED WHEN FLASHING TO FLIGHT PCBS**

### gpio.h
(literally all of the pin assignments)

TRIPLE CHECK ALL OF THESE

There's a couple of other parameters in there that are useful/important to check

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
