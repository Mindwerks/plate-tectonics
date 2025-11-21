cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project (PlateTectonicsExamples)
add_executable(simulation simulation.cpp map_drawing.cpp)

# Prefer Homebrew/standard system paths over frameworks (e.g., Mono)
set(CMAKE_FIND_FRAMEWORK LAST)
find_package(PNG REQUIRED)

target_include_directories(simulation PRIVATE ../src ${PNG_INCLUDE_DIRS})
target_link_libraries(simulation PRIVATE PlateTectonics PNG::PNG)
