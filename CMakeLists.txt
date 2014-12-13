cmake_minimum_required (VERSION 2.6)
project (PlateTectonics)
add_library(PlateTectonics src/sqrdmd.cpp src/heightmap.cpp src/lithosphere.cpp src/plate.cpp src/rectangle.cpp src/platecapi.cpp)

include_directories("src")
set(CMAKE_C_FLAGS "-std=c99")

add_subdirectory (test)
