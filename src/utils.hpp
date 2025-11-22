/******************************************************************************
*  plate-tectonics, a plate tectonics simulation library
*  Copyright (C) 2012-2013 Lauri Viitanen
*  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, see http://www.gnu.org/licenses/
*****************************************************************************/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <stdlib.h>

#if _WIN32 || _WIN64
#include <Windows.h>
typedef UINT32 uint32_t;
typedef INT32 int32_t;
#else
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#endif

#ifndef UINT32_C
#define UINT32_C(val) val##ui32
#endif

// Define PI constant for mathematical operations
#ifndef PI
#define PI 3.14159265358979323846264338327950288f
#endif

namespace Platec {

std::string to_string(uint32_t value);
std::string to_string_f(float value);

}

// MK: I strongly feel that a release build should have this disabled,
// but I'm keeping it here because that is the wishes of FT
#define LOG_ASSERTS // Remove this to remove printing asserts in release mode

#ifndef NDEBUG
#define ASSERT(condition, message) \
do { \
	if (!(condition)) { \
		std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
			<< " line " << __LINE__ << " Message: " << (message) << std::endl; \
		exit(1); \
	} \
} while (false)
#elif defined(LOG_ASSERTS)
#define ASSERT(condition, message) \
do { \
	if (!(condition)) { \
		std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
			<< " line " << __LINE__ << " Message: " << (message) << std::endl; \
	} \
} while (false)
#else
#define ASSERT(condition, message)
#endif

#endif
