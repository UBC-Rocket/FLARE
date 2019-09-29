cmake_minimum_required(VERSION 3.14)
project(Sky_Pilot)
include(FetchContent)

set(CMAKE_CXX_STANDARD 14)

include_directories(avionics)
include_directories(avionics/utility)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(googletest)

add_executable(Sky_Pilot
        #avionics/utility/Adafruit_Sensor.h
        #avionics/utility/imumaths.h
        #avionics/utility/matrix.h
        #avionics/utility/quaternion.h
        #avionics/utility/vector.h
        #avionics/Adafruit_BNO055.cpp
        #avionics/Adafruit_BNO055.h
        #avionics/avionics.ino
        #avionics/battery.cpp
        #avionics/battery.h
        #avionics/buzzer.cpp
        #avionics/buzzer.h
        #avionics/calculations.cpp
        #avionics/calculations.h
        #avionics/cameras.cpp
        #avionics/cameras.h
        #avionics/GP20U7.cpp
        #avionics/GP20U7.h
        #avionics/gpio.cpp
        #avionics/gpio.h
        #avionics/groundaltitude.cpp
        #avionics/groundaltitude.h
        #avionics/IridiumSBD.cpp
        #avionics/IridiumSBD.h
        #avionics/MAX31855k.cpp
        #avionics/MAX31855k.h
        #avionics/MS5803_01.cpp
        #avionics/MS5803_01.h
        #avionics/options.h
        #avionics/radio.cpp
        #avionics/radio.h
        #avionics/satcom.cpp
        #avionics/satcom.h
        #avionics/sensors.cpp
        #avionics/sensors.h
        #avionics/SparkFun_LIS331.cpp
        #avionics/SparkFun_LIS331.h
        #avionics/SparkFunTMP102.cpp
        #avionics/SparkFunTMP102.h
        #avionics/statemachine.cpp
        #avionics/statemachine.h
        #avionics/XBee.cpp
        #avionics/XBee.h
        avionics/maintest.cpp
        )

target_link_libraries(Sky_Pilot gtest)