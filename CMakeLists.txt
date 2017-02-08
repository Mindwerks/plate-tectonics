cmake_minimum_required (VERSION 3.1)
project (PlateTectonics)
add_library(PlateTectonics src/sqrdmd.cpp src/heightmap.cpp src/lithosphere.cpp src/plate.cpp src/rectangle.cpp src/platecapi.cpp src/simplexnoise.cpp src/noise.cpp src/utils.cpp src/simplerandom.cpp src/plate_functions.cpp src/bounds.cpp src/movement.cpp src/mass.cpp src/segments.cpp src/world_point.cpp src/geometry.cpp src/segment_creator.cpp src/segment_data.cpp)

include_directories("src")

#
# The whole MSVC
#
if(MSVC)
	# Default to statically-linked runtime.
	if("${MSVC_RUNTIME}" STREQUAL "")
  		set(MSVC_RUNTIME "static")
	endif()
	# Set compiler options.
	set(variables
	  CMAKE_C_FLAGS_DEBUG
	  CMAKE_C_FLAGS_MINSIZEREL
	  CMAKE_C_FLAGS_RELEASE
	  CMAKE_C_FLAGS_RELWITHDEBINFO
	  CMAKE_CXX_FLAGS_DEBUG
	  CMAKE_CXX_FLAGS_MINSIZEREL
	  CMAKE_CXX_FLAGS_RELEASE
	  CMAKE_CXX_FLAGS_RELWITHDEBINFO
	)
	if(${MSVC_RUNTIME} STREQUAL "static")
	  message(STATUS
	    "MSVC -> forcing use of statically-linked runtime."
	  )
	  foreach(variable ${variables})
	    if(${variable} MATCHES "/MD")
	      string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
	    endif()
	  endforeach()
	else()
	  message(STATUS
	    "MSVC -> forcing use of dynamically-linked runtime."
	  )
	  foreach(variable ${variables})
	    if(${variable} MATCHES "/MT")
	      string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
	    endif()
	  endforeach()
	endif()
endif()
#
## -fp-model disable floating point optimization, for the sake of having
## reproducible results across platforms 
#IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
#ELSE()
    set (CMAKE_CXX_STANDARD 14)
#ENDIF()

option(WITH_EXAMPLES "compile also the example" OFF)
option(WITH_TESTS "compile also the tests" ON)

IF(WITH_TESTS)
	enable_testing()
	add_subdirectory (test)
ENDIF(WITH_TESTS)
IF(WITH_EXAMPLES)
	add_subdirectory (examples)
ENDIF(WITH_EXAMPLES)
