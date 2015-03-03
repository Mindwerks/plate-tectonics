#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#define __STDC_CONSTANT_MACROS
#include <stdint.h>

#if _WIN32 || _WIN64
#include <Windows.h>
typedef UINT32 uint32_t;
typedef INT32 int32_t;
#else
#include <stdint.h>
#endif

namespace Platec {

std::string to_string(uint32_t value);

}

void p_assert(bool condition, const std::string& message);

#endif
