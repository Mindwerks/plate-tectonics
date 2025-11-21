/******************************************************************************
 *  PlaTec, a 2D terrain generator based on plate tectonics
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *  Copyright (C) 2025      SIMD Optimizations
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

#pragma once

#include <cstdint>

// Detect platform SIMD capabilities at compile time
#if defined(__ARM_NEON) || defined(__aarch64__)
    #define HAVE_NEON 1
    #include <arm_neon.h>
#endif

#if defined(__AVX2__)
    #define HAVE_AVX2 1
    #include <immintrin.h>
#elif defined(__SSE4_1__) || defined(__SSE4_2__)
    #define HAVE_SSE4 1
    #include <smmintrin.h>
#endif

namespace simd {

/**
 * Clamp all values in array to be non-negative (>= 0.0f)
 * Uses SIMD when available: NEON (ARM), AVX2 or SSE4.1 (x86-64)
 *
 * @param data Array of floats to clamp in-place
 * @param size Number of elements in array
 */
void clamp_non_negative(float* data, uint32_t size);

/**
 * Fill array with a constant value
 * Optimized for zero (uses memset) and SIMD for other values
 *
 * @param data Array to fill
 * @param size Number of elements
 * @param value Value to fill with
 */
void set_all(float* data, uint32_t size, float value);

/**
 * Copy array from source to destination
 * Uses memcpy for optimal performance
 *
 * @param dest Destination array
 * @param src Source array
 * @param size Number of elements to copy
 */
void copy_array(float* dest, const float* src, uint32_t size);

/**
 * Element-wise maximum of two arrays: dest[i] = max(src1[i], src2[i])
 * Uses SIMD when available
 *
 * @param dest Destination array
 * @param src1 First source array
 * @param src2 Second source array
 * @param size Number of elements
 */
void element_max(float* dest, const float* src1, const float* src2, uint32_t size);

/**
 * Compute dot product of two arrays: sum(a[i] * b[i])
 * Uses SIMD with FMA when available
 *
 * @param a First array
 * @param b Second array
 * @param size Number of elements
 * @return Dot product
 */
float dot_product(const float* a, const float* b, uint32_t size);

} // namespace simd
