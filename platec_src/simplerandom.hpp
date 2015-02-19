/*
 * This is code from the Simple Pseudo-random Number Generators
 * Contact : http://craig.mcqueen.id.au
 * Copyright : 2010 Craig McQueen
 * 
 * Available on GitHub https://github.com/cmcqueen/simplerandom
 */

#ifndef SIMPLE_RANDOM_HPP
#define SIMPLE_RANDOM_HPP

#if _WIN32 || _WIN64
#include <Windows.h>
typedef UINT32 uint32_t;
#else
#include <cstdint>
#endif

typedef struct
{
    uint32_t        cong;
} SimpleRandomCong_t;


class SimpleRandom {
public:
	SimpleRandom(uint32_t seed);
	~SimpleRandom();
	uint32_t next();
private:
	SimpleRandomCong_t* internal;
};

#endif
