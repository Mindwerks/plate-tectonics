cmake_minimum_required (VERSION 2.6)

project (PlateTectonicsExamples)
add_executable(PlateTectonicsSimulationExample simulation_example.cpp)
add_executable(PlateTectonicsSqrDmdExample sqrdmd_example.cpp)

if (WITH_EXAMPLE)
	find_package(DevIL)
endif (WITH_EXAMPLE)

include_directories("../src" ${IL_INCLUDE_DIR})

# Extra linking for the project.
target_link_libraries(PlateTectonicsSimulationExample PlateTectonics ${IL_LIBRARIES} ${ILU_LIBRARIES})

target_link_libraries(PlateTectonicsSqrDmdExample PlateTectonics ${IL_LIBRARIES} ${ILU_LIBRARIES})