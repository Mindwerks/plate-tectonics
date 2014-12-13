cmake_minimum_required (VERSION 2.6)
project (PlateTectonics)
add_library(PlateTectonics src/heightmap.cpp src/lithosphere.cpp src/plate.cpp)

include_directories("src")