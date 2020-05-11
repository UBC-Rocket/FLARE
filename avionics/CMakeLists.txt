cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-pthread")

project(Avionics_Native)

add_executable(TantalusStage1 envs/x86/src/main.cpp)

target_sources(TantalusStage1 PUBLIC
    common/src
    common/lib
    envs/x86/src
    envs/x86/lib
    rockets/tantalus_stage_1/src
    )
    
target_include_directories(TantalusStage1 PUBLIC 
    common/include
    common/lib
    envs/x86/include
    envs/x86/lib
    rockets/tantalus_stage_1/include
    
    )


