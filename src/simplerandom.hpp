/*
 * This is code from the Simple Pseudo-random Number Generators
 * Contact : http://craig.mcqueen.id.au
 * Copyright : 2010 Craig McQueen
 * 
 * Available on GitHub https://github.com/cmcqueen/simplerandom
 */

#ifndef SIMPLE_RANDOM_HPP
#define SIMPLE_RANDOM_HPP

#include "utils.hpp"

typedef struct
{
    uint32_t        cong;
} SimpleRandomCong_t;


class SimpleRandom {
public:
	SimpleRandom(uint32_t seed);
	~SimpleRandom();
	uint32_t next();
	int32_t next_signed();
	double next_double();
	float next_float_signed();
	uint32_t maximum();
private:
	SimpleRandomCong_t* internal;
};

#endif
