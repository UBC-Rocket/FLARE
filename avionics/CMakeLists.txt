cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Avionics_Native)

add_executable(TantalusStage1 envs/x86/src/main.cpp)

target_include_directories(TantalusStage1 PUBLIC 
    common/include
    envs/x86/include
    common/lib
    envs/x86/lib
    rockets/tantalus_stage_1/include
    )


