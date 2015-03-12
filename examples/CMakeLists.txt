cmake_minimum_required (VERSION 2.6)

project (PlateTectonicsExamples)
add_executable(simulation simulation.cpp map_drawing.cpp)

find_package(PNG REQUIRED)

include_directories("../src" ${PNG_INCLUDE_DIRS})

target_link_libraries(simulation PlateTectonics ${PNG_LIBRARIES})
