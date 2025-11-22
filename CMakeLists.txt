cmake_minimum_required (VERSION 3.10)
project (PlateTectonics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Treat warnings as errors
if(MSVC)
    add_compile_options(/W4 /WX)
    # Enable Address Sanitizer for better error detection
    add_compile_options(/fsanitize=address)
    add_link_options(/fsanitize=address)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

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

option(WITH_EXAMPLES "compile also the example" OFF)
option(WITH_TESTS "compile also the tests" ON)

IF(WITH_TESTS)
	enable_testing()
	add_subdirectory (test)
ENDIF(WITH_TESTS)
IF(WITH_EXAMPLES)
	add_subdirectory (examples)
ENDIF(WITH_EXAMPLES)
