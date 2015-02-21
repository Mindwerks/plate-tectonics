cmake_minimum_required (VERSION 2.6)

add_subdirectory (googletest)

find_package(DevIL)

project (PlateTectonicsTests)
add_executable(PlateTectonicsTests test_acceptance.cpp test_heightmap.cpp test_plate.cpp test_rectangle.cpp test_sqrdmd.cpp test_randomness.cpp)

find_package(PNG REQUIRED)

include_directories("../src" "googletest/include" ${PNG_INCLUDE_DIR})

 # Standard linking to gtest stuff.
target_link_libraries(PlateTectonicsTests gtest gtest_main)

# Extra linking for the project.
target_link_libraries(PlateTectonicsTests PlateTectonics)

target_link_libraries(PlateTectonicsTests ${PNG_LIBRARIES})
