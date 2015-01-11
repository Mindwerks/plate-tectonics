#ifndef NOISE_HPP
#define NOISE_HPP

#include <random>
#include "rectangle.hpp"

void createNoise(float* tmp, const WorldDimension& tmpDim, mt19937 _randsource, bool useSimplex = false);

#endif
