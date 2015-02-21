#include "utils.hpp"
#include <sstream>

namespace Platec {

std::string to_string(uint32_t value)
{	
	std::stringstream ss;
	ss << value;
	std::string str = ss.str();
	return str;
}

}