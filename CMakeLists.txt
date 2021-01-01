cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(rocketos)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wextra -pedantic")
else()
    set(CMAKE_CXX_FLAGS "-pthread -static-libstdc++ -static-libgcc -Wall -Wextra -pedantic")
endif()

add_executable(demo main.cpp)

# target_include_directories(demo PUBLIC
#   ${CMAKE_CURRENT_BINARY_DIR}
# )

###
### Setup Radio defines
###

###
### Google Test
###

# Download and unpack googletest at configure time
# -Wno-dev suppresses warnings - CMake gets confused when you do this, I guess
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -Wno-dev -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)
# Note 2020-12-20 - above add_subdirectory call throws CMake versioning errors.
# There's a recent (as of writing) pull request in googletest upstream
# https://github.com/google/googletest/pull/3094

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

enable_testing()

###
### Link all tests here
###
### Note: Compile project in build folder to keep unnecessarily files out from main directory
###

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(testSchedule tests/test_schedule.cpp )
target_link_libraries(testSchedule gtest_main)
# target_include_directories(testSchedule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(testSchedule PUBLIC include)
add_test(NAME TestSchedule COMMAND testSchedule)

# We can now call make test to run all the tests
