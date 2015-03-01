cmake_minimum_required (VERSION 2.6)
project (PlateTectonics)
add_library(PlateTectonics src/sqrdmd.cpp src/heightmap.cpp src/lithosphere.cpp src/plate.cpp src/rectangle.cpp src/platecapi.cpp src/simplexnoise.cpp src/noise.cpp src/utils.cpp src/simplerandom.cpp src/plate_functions.cpp)

include_directories("src")

# -fp-model disable floating point optimization, for the sake of having
# reproducible results across platforms 
set(CMAKE_C_FLAGS "-std=c99 -O0")
set(CMAKE_CXX_FLAGS "-O0 -g -rdynamic")

option(WITH_EXAMPLES "compile also the example" OFF)
option(WITH_TESTS "compile also the tests" ON)

IF(WITH_TESTS)
	add_subdirectory (test)
ENDIF(WITH_TESTS)
IF(WITH_EXAMPLES)
	add_subdirectory (examples)
ENDIF(WITH_EXAMPLES)
