# Fetch GoogleTest from GitHub
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Make GoogleTest available
FetchContent_MakeAvailable(googletest)

project (PlateTectonicsTests)
add_executable(PlateTectonicsTests test_acceptance.cpp test_heightmap.cpp test_plate.cpp test_rectangle.cpp test_sqrdmd.cpp test_randomness.cpp test_portability.cpp test_bounds.cpp test_mass.cpp test_movement.cpp test_regression.cpp)

add_test(NAME PlateTectonicsTests COMMAND PlateTectonicsTests)

include_directories("../src" ${PNG_INCLUDE_DIR})

# Standard linking to gtest stuff.
target_link_libraries(PlateTectonicsTests gtest gtest_main)

# Extra linking for the project.
target_link_libraries(PlateTectonicsTests PlateTectonics)


