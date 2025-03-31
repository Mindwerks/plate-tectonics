cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project (PlateTectonicsExamples)
add_executable(simulation simulation.cpp map_drawing.cpp)

find_package(PNG REQUIRED)

include_directories("../src" ${PNG_INCLUDE_DIRS})

target_link_libraries(simulation PlateTectonics ${PNG_LIBRARIES})
