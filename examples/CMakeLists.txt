cmake_minimum_required (VERSION 2.6)

project (PlateTectonicsExamples)
add_executable(simulation simulation_example.cpp)
add_executable(simulation_rect simulation_rect_example.cpp)
add_executable(sqrdmd sqrdmd_example.cpp)
add_executable(simplex simplex_example.cpp)

if (WITH_EXAMPLE)
	find_package(DevIL)
endif (WITH_EXAMPLE)

include_directories("../src" ${IL_INCLUDE_DIR})

# Extra linking for the project.
target_link_libraries(simulation PlateTectonics ${IL_LIBRARIES} ${ILU_LIBRARIES})

target_link_libraries(simulation_rect PlateTectonics ${IL_LIBRARIES} ${ILU_LIBRARIES})

target_link_libraries(sqrdmd PlateTectonics ${IL_LIBRARIES} ${ILU_LIBRARIES})

target_link_libraries(simplex PlateTectonics ${IL_LIBRARIES} ${ILU_LIBRARIES})