#ifndef NOISE_HPP
#define NOISE_HPP

#ifdef __MINGW32__ // this is to avoid a problem with the hypot function which is messed up by Python...
#undef __STRICT_ANSI__
#endif
#include <random>
#include "rectangle.hpp"

void createNoise(float* tmp, const WorldDimension& tmpDim, mt19937 _randsource, bool useSimplex = false);

#endif
