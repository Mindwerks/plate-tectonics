cmake_minimum_required (VERSION 2.6)

project (PlateTectonicsExample)
add_executable(PlateTectonicsExample example.cpp)

find_package(DevIL)

include_directories("../src" ${IL_INCLUDE_DIR})

# Extra linking for the project.
target_link_libraries(PlateTectonicsExample PlateTectonics ${IL_LIBRARIES} ${ILU_LIBRARIES})