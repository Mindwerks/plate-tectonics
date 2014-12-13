cmake_minimum_required (VERSION 2.6)

add_subdirectory (googletest)

project (PlateTectonicsTests)
add_executable(PlateTectonicsTests ../src/heightmap.cpp ../src/lithosphere.cpp ../src/plate.cpp test_acceptance.cpp test_heightmap.cpp test_plate.cpp test_rectangle.cpp)

include_directories("../src" "googletest/include")