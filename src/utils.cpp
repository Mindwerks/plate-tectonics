#include "utils.hpp"
#include <sstream>
#include <iostream>
#include <stdlib.h>

namespace Platec {

std::string to_string(uint32_t value)
{   
    std::stringstream ss;
    ss << value;
    std::string str = ss.str();
    return str;
}

}

void p_assert(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "Assertion failed: " << message << std::endl;
        exit(1);
    }
}