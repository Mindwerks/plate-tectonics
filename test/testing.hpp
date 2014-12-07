#ifndef TESTING_HPP
#define TESTING_HPP

#include <cmath>

#define CHECKF_EQ(a,b) CHECK( std::abs((float)(a-b)) < 0.001f)

#endif