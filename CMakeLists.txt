cmake_minimum_required (VERSION 3.10)
project (PlateTectonics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizer support (AddressSanitizer, UndefinedBehaviorSanitizer)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_UBSAN)
    message(STATUS "UndefinedBehaviorSanitizer enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

# Detect SIMD capabilities based on target architecture
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
    set(TARGET_ARCH "ARM64")
    message(STATUS "Target architecture: ARM64 (NEON available)")
    # NEON is always available on ARM64
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+simd")
    
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64")
    set(TARGET_ARCH "X86_64")
    message(STATUS "Target architecture: x86-64")
    
    # Check for AVX2 support
    include(CheckCXXSourceRuns)
    set(CMAKE_REQUIRED_FLAGS "-mavx2")
    check_cxx_source_runs("
        #include <immintrin.h>
        int main() {
            __m256 a = _mm256_set1_ps(1.0f);
            return 0;
        }
    " HAVE_AVX2_SUPPORT)
    
    if(HAVE_AVX2_SUPPORT)
        message(STATUS "AVX2 support: YES")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
    else()
        message(STATUS "AVX2 support: NO - using SSE4.1")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
    endif()
else()
    message(STATUS "Target architecture: ${CMAKE_SYSTEM_PROCESSOR} - using scalar fallback")
endif()

# Optimization flags for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Platform-specific optimizations
if(TARGET_ARCH STREQUAL "ARM64")
    # Apple Silicon specific optimizations
    if(APPLE)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mcpu=apple-m1")
        message(STATUS "Apple Silicon optimizations enabled")
    endif()
elseif(TARGET_ARCH STREQUAL "X86_64")
    # Enable native arch optimizations for x86-64
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif()

# Enable link-time optimization for Release builds
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

add_library(PlateTectonics src/sqrdmd.cpp src/heightmap.cpp src/lithosphere.cpp src/plate.cpp src/rectangle.cpp src/platecapi.cpp src/simplexnoise.cpp src/noise.cpp src/utils.cpp src/simplerandom.cpp src/plate_functions.cpp src/bounds.cpp src/movement.cpp src/mass.cpp src/segments.cpp src/world_point.cpp src/geometry.cpp src/segment_creator.cpp src/segment_data.cpp src/simd_utils.cpp)

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
	add_subdirectory (test)
ENDIF(WITH_TESTS)
IF(WITH_EXAMPLES)
	add_subdirectory (examples)
ENDIF(WITH_EXAMPLES)
