cmake_minimum_required(VERSION 3.14)
project(Sky_Pilot)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(include)
include_directories(src/utility)
include_directories(test)

add_executable(Sky_Pilot
        test/test_main.cpp
        include/catch.hpp
        )
