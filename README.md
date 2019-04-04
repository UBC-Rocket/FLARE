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
  then navigate to ‎⁨Contents⁩/⁨Java⁩/⁨hardware⁩/⁨teensy⁩/⁨avr⁩/⁨cores⁩/⁨teensy3⁩/serial5.c

On Windows: [user_drive]\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\serial5.c

On line 45 increase SERIAL5_RX_BUFFER_SIZE from 64 to 1024

THIS MUST BE DONE ON THE COMPUTER USED TO COMPILE THE CODE!!!

**VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME! VERY IMPORTANT PLEASE READ ME!**

***
***


# Definable stuff
All definable things are located in "sensors.h".

### Nosecone/Body
For the 2018-2019 work year, the rocket has two avionics systems; one in the nosecone and one in the body.
The specific functions they can perform are as follows:

|Nosecone            | Body|
|---- | ----|
| Log all sensor data | Log all sensor data |
|                    | Communicate with SATCOM
|                    | Utilize GPS
| Uses `bodyTierX` radio functions| Uses `noseconeTierX` radio functions - a subset of data is sent |
|Runs state machine and controls deployment actuators (e.g. servos, ematch) | Runs state machine without control of actuators |
| | Accepts unique nosecone commands from radio _(not implemented as of Mar 2)_ |

As a hack, you can undefine NOSECONE to deactivate SATCOM (e.g. since SATCOM is blocking)

### Testing
When defined, enables debugging output. All polled sensor values are printed to USB, and a 1 second delay is added to the main Arduino loop function. **DO NOT HAVE THIS DEFINED WHEN FLASHING TO FLIGHT PCBS**

### Ground Test
When defined, eanbles radio-activated deployment of the main and drogue parachutes. This is to allow for deployment tests while on the ground. GROUND_TEST also enables other features facilitating testing on the ground (e.g. LED for rocket state) **DO NOT HAVE THIS DEFINED WHEN FLASHING TO FLIGHT PCBS**

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
