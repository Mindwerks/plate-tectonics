cmake_minimum_required (VERSION 2.6)
project (PlateTectonics)
add_library(PlateTectonics src/sqrdmd.cpp src/heightmap.cpp src/lithosphere.cpp src/plate.cpp src/rectangle.cpp src/platecapi.cpp src/simplexnoise.cpp)

include_directories("src")
set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-std=c++11")

option(WITH_EXAMPLE "compile also the example" OFF)

add_subdirectory (test)
IF(WITH_EXAMPLE)
	add_subdirectory (examples)
ENDIF(WITH_EXAMPLE)
