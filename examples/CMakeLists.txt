cmake_minimum_required (VERSION 2.6)

project (PlateTectonicsExamples)
add_executable(simulation simulation_example.cpp map_drawing.cpp)
add_executable(simulation_rect simulation_rect_example.cpp map_drawing.cpp)
add_executable(sqrdmd sqrdmd_example.cpp map_drawing.cpp)
add_executable(simplex simplex_example.cpp map_drawing.cpp)
add_executable(noise noise_example.cpp map_drawing.cpp)


find_package(PNG REQUIRED)

include_directories("../src" ${PNG_INCLUDE_DIRS})

# Extra linking for the project.
target_link_libraries(simulation PlateTectonics ${PNG_LIBRARIES})

target_link_libraries(simulation_rect PlateTectonics ${PNG_LIBRARIES})

target_link_libraries(sqrdmd PlateTectonics ${PNG_LIBRARIES})

target_link_libraries(simplex PlateTectonics ${PNG_LIBRARIES})

target_link_libraries(noise PlateTectonics ${PNG_LIBRARIES})