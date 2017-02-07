add_subdirectory (googletest)


add_executable(PlateTectonicsTests test_acceptance.cpp test_heightmap.cpp test_plate.cpp test_rectangle.cpp test_sqrdmd.cpp test_randomness.cpp test_portability.cpp test_bounds.cpp test_mass.cpp test_movement.cpp)

add_test(NAME PlateTectonicsTests COMMAND PlateTectonicsTests)

include_directories("../src" "googletest/include" ${PNG_INCLUDE_DIR})

# Standard linking to gtest stuff.
target_link_libraries(PlateTectonicsTests gtest gtest_main)

# Extra linking for the project.
target_link_libraries(PlateTectonicsTests PlateTectonics)

