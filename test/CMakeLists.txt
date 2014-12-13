cmake_minimum_required (VERSION 2.6)

add_subdirectory (googletest)

project (PlateTectonicsTests)
add_executable(PlateTectonicsTests test_acceptance.cpp test_heightmap.cpp test_plate.cpp test_rectangle.cpp)

include_directories("../src" "googletest/include")

 # Standard linking to gtest stuff.
target_link_libraries(PlateTectonicsTests gtest gtest_main)

# Extra linking for the project.
target_link_libraries(PlateTectonicsTests PlateTectonics)