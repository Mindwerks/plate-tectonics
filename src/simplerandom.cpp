/******************************************************************************
 *  plate-tectonics, a plate tectonics simulation library
 *  Copyright (C) 2010 Craig McQueen (http://craig.mcqueen.id.au)
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *
 *  This is code from the Simple Pseudo-random Number Generators
 *  Available on GitHub https://github.com/cmcqueen/simplerandom
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

#include "simplerandom.hpp"
#include <stddef.h>
#include "utils.hpp"

void simplerandom_cong_seed(SimpleRandomCong_t * p_cong, uint32_t seed);
void simplerandom_cong_mix(SimpleRandomCong_t * p_cong, const uint32_t * p_data, uint32_t num_data);
uint32_t simplerandom_cong_next(SimpleRandomCong_t * p_cong);

SimpleRandom::SimpleRandom(uint32_t seed)
{
    internal = new SimpleRandomCong_t();
    simplerandom_cong_seed(internal, seed);
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
    ASSERT(value >= 0.0f && value <= 1.0f, "Invalid float range");
    return value - 0.5f;
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
    /* We only use this parameter for type checking. */
    (void)p_cong;
    return 1u;
}

uint32_t simplerandom_cong_seed_array(SimpleRandomCong_t * p_cong, const uint32_t * p_seeds, uint32_t num_seeds, bool mix_extras)
{
    uint32_t    seed = 0;
    uint32_t      num_seeds_used = 0;

    if (num_seeds >= 1u && p_seeds != nullptr)
    {
        seed = p_seeds[0];
        num_seeds_used = 1u;
    }
    simplerandom_cong_seed(p_cong, seed);

    if (mix_extras && p_seeds != nullptr)
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
    (void)p_cong;
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
    if (p_data != nullptr)
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
