cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project (PlateTectonicsExamples)
add_executable(simulation simulation.cpp map_drawing.cpp)
add_executable(profile_benchmark profile_benchmark.cpp)
add_executable(test_parallelism test_parallelism.cpp)
add_executable(test_cpu_usage test_cpu_usage.cpp)
add_executable(test_work_comparison test_work_comparison.cpp)
add_executable(test_scaling_analysis test_scaling_analysis.cpp)

find_package(PNG REQUIRED)

include_directories("../src" ${PNG_INCLUDE_DIRS})

target_link_libraries(simulation PlateTectonics ${PNG_LIBRARIES})
target_link_libraries(profile_benchmark PlateTectonics)
target_link_libraries(test_parallelism PlateTectonics)
target_link_libraries(test_work_comparison PlateTectonics)
target_link_libraries(test_scaling_analysis PlateTectonics)
# test_cpu_usage is standalone, no PlateTectonics dependency
target_link_libraries(profile_benchmark PlateTectonics)
