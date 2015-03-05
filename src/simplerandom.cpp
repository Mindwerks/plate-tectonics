/*
 * This is code from the Simple Pseudo-random Number Generators
 * Contact : http://craig.mcqueen.id.au
 * Copyright : 2010 Craig McQueen
 * 
 * Available on GitHub https://github.com/cmcqueen/simplerandom
 */

#include "simplerandom.hpp"
#include <stdint.h>
#include <stddef.h>
#include "utils.hpp"

#ifndef UINT32_C
#define UINT32_C(val) val##ui32
#endif

void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed);
void simplerandom_cong_mix(SimpleRandomCong_t * p_cong, const uint32_t * p_data, uint32_t num_data);
uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong);

SimpleRandom::SimpleRandom(uint32_t seed)
{
    internal = new SimpleRandomCong_t();
    simplerandom_cong_seed(internal, seed);
}

SimpleRandom::SimpleRandom(SimpleRandom& other)
{
    internal = new SimpleRandomCong_t();
    internal->cong = other.internal->cong;
}

SimpleRandom::SimpleRandom(const SimpleRandom& other)
{
    internal = new SimpleRandomCong_t();
    internal->cong = other.internal->cong;
}

SimpleRandom::~SimpleRandom()
{
    delete internal;
}
    
uint32_t SimpleRandom::next()
{
    uint32_t res = simplerandom_cong_next(internal);
    
    return res;
}

double SimpleRandom::next_double()
{
    return ((double)next() / (double)maximum());
}

float SimpleRandom::next_float_signed()
{
    float value = next_double();
    p_assert(value >= 0.0f && value <= 1.0f, "(SimpleRandom::next_float_signed)");
    return next_double() - 0.5f;
}

int32_t SimpleRandom::next_signed()
{
    int32_t value = (int32_t)next();
    return value;
}

uint32_t SimpleRandom::maximum()
{
    return 4294967295;
}

uint32_t simplerandom_cong_num_seeds(const SimpleRandomCong_t * p_cong)
{
    (const void *)p_cong;   /* We only use this parameter for type checking. */

    return 1u;
}

uint32_t simplerandom_cong_seed_array(SimpleRandomCong_t * p_cong, const uint32_t * p_seeds, uint32_t num_seeds, bool mix_extras)
{
    uint32_t    seed = 0;
    uint32_t      num_seeds_used = 0;

    if (num_seeds >= 1u && p_seeds != NULL)
    {
        seed = p_seeds[0];
        num_seeds_used = 1u;
    }
    simplerandom_cong_seed(p_cong, seed);

    if (mix_extras && p_seeds != NULL)
    {
        simplerandom_cong_mix(p_cong, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed)
{
    p_cong->cong = seed;
    /* No sanitize is needed because for Cong, all state values are valid. */
}

void simplerandom_cong_sanitize(SimpleRandomCong_t * p_cong)
{
    /* All state values are valid for Cong. No sanitizing needed. */
    (const void *) p_cong;
}

uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong)
{
    uint32_t    cong;

    cong = UINT32_C(69069) * p_cong->cong + 12345u;
    p_cong->cong = cong;

    return cong;
}

void simplerandom_cong_mix(SimpleRandomCong_t * p_cong, const uint32_t * p_data, uint32_t num_data)
{
    if (p_data != NULL)
    {
        while (num_data)
        {
            --num_data;
            p_cong->cong ^= *p_data++;
            simplerandom_cong_sanitize(p_cong);
            simplerandom_cong_next(p_cong);
        }
    }
}
