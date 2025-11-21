/******************************************************************************
 *  PlaTec, a 2D terrain generator based on plate tectonics
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *  Copyright (C) 2025      SIMD Optimizations - Phase 1
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

#include "simd_utils.hpp"
#include <algorithm>
#include <cstring>

namespace simd {

// ============================================================================
// clamp_non_negative - SIMD optimized clamping to >= 0
// Simple vectorization without aggressive optimizations (Phase 1)
// ============================================================================

void clamp_non_negative(float* data, uint32_t len) {
    if (!data || len == 0)
        return;

#if defined(HAVE_NEON)
    // ARM NEON implementation (4 floats at a time)
    const float32x4_t zero = vdupq_n_f32(0.0f);
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        float32x4_t v = vld1q_f32(&data[i]);
        v = vmaxq_f32(v, zero);
        vst1q_f32(&data[i], v);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        if (data[i] < 0.0f)
            data[i] = 0.0f;
    }

#elif defined(HAVE_AVX2)
    // Intel AVX2 implementation (8 floats at a time)
    const __m256 zero = _mm256_setzero_ps();
    uint32_t i = 0;

    // Process 8 elements at a time
    for (; i + 8 <= len; i += 8) {
        __m256 v = _mm256_loadu_ps(&data[i]);
        v = _mm256_max_ps(v, zero);
        _mm256_storeu_ps(&data[i], v);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        if (data[i] < 0.0f)
            data[i] = 0.0f;
    }

#elif defined(HAVE_SSE4)
    // Intel SSE4.1 implementation (4 floats at a time)
    const __m128 zero = _mm_setzero_ps();
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        __m128 v = _mm_loadu_ps(&data[i]);
        v = _mm_max_ps(v, zero);
        _mm_storeu_ps(&data[i], v);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        if (data[i] < 0.0f)
            data[i] = 0.0f;
    }

#else
    // Scalar fallback
    for (uint32_t i = 0; i < len; ++i) {
        if (data[i] < 0.0f)
            data[i] = 0.0f;
    }
#endif
}

// ============================================================================
// set_all - SIMD optimized array fill
// Simple vectorization without aggressive optimizations (Phase 1)
// ============================================================================

void set_all(float* data, uint32_t len, float value) {
    if (!data || len == 0)
        return;

    // Special case: zero is best handled by memset
    if (value == 0.0f) {
        memset(data, 0, len * sizeof(float));
        return;
    }

#if defined(HAVE_NEON)
    // ARM NEON implementation (4 floats at a time)
    const float32x4_t v = vdupq_n_f32(value);
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        vst1q_f32(&data[i], v);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        data[i] = value;
    }

#elif defined(HAVE_AVX2)
    // Intel AVX2 implementation (8 floats at a time)
    const __m256 v = _mm256_set1_ps(value);
    uint32_t i = 0;

    // Process 8 elements at a time
    for (; i + 8 <= len; i += 8) {
        _mm256_storeu_ps(&data[i], v);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        data[i] = value;
    }

#elif defined(HAVE_SSE4)
    // Intel SSE4.1 implementation (4 floats at a time)
    const __m128 v = _mm_set1_ps(value);
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        _mm_storeu_ps(&data[i], v);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        data[i] = value;
    }

#else
    // Scalar fallback
    for (uint32_t i = 0; i < len; ++i) {
        data[i] = value;
    }
#endif
}

// ============================================================================
// copy_array - Optimized array copy
// ============================================================================

void copy_array(float* dest, const float* src, uint32_t len) {
    if (!dest || !src || len == 0)
        return;
    memcpy(dest, src, len * sizeof(float));
}

// ============================================================================
// element_max - SIMD optimized element-wise maximum
// ============================================================================

void element_max(float* dest, const float* src1, const float* src2, uint32_t len) {
    if (!dest || !src1 || !src2 || len == 0)
        return;

#if defined(HAVE_NEON)
    // ARM NEON implementation (4 floats at a time)
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        float32x4_t v1 = vld1q_f32(&src1[i]);
        float32x4_t v2 = vld1q_f32(&src2[i]);
        float32x4_t result = vmaxq_f32(v1, v2);
        vst1q_f32(&dest[i], result);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        dest[i] = std::max(src1[i], src2[i]);
    }

#elif defined(HAVE_AVX2)
    // Intel AVX2 implementation (8 floats at a time)
    uint32_t i = 0;

    // Process 8 elements at a time
    for (; i + 8 <= len; i += 8) {
        __m256 v1 = _mm256_loadu_ps(&src1[i]);
        __m256 v2 = _mm256_loadu_ps(&src2[i]);
        __m256 result = _mm256_max_ps(v1, v2);
        _mm256_storeu_ps(&dest[i], result);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        dest[i] = std::max(src1[i], src2[i]);
    }

#elif defined(HAVE_SSE4)
    // Intel SSE4.1 implementation (4 floats at a time)
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        __m128 v1 = _mm_loadu_ps(&src1[i]);
        __m128 v2 = _mm_loadu_ps(&src2[i]);
        __m128 result = _mm_max_ps(v1, v2);
        _mm_storeu_ps(&dest[i], result);
    }

    // Handle remaining elements
    for (; i < len; ++i) {
        dest[i] = std::max(src1[i], src2[i]);
    }

#else
    // Scalar fallback
    for (uint32_t i = 0; i < len; ++i) {
        dest[i] = std::max(src1[i], src2[i]);
    }
#endif
}

// ============================================================================
// dot_product - SIMD optimized dot product with FMA
// ============================================================================

float dot_product(const float* a, const float* b, uint32_t len) {
    if (!a || !b || len == 0)
        return 0.0f;

#if defined(HAVE_NEON)
    // ARM NEON implementation with FMA (4 floats at a time)
    float32x4_t sum_vec = vdupq_n_f32(0.0f);
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        float32x4_t v1 = vld1q_f32(&a[i]);
        float32x4_t v2 = vld1q_f32(&b[i]);
        sum_vec = vfmaq_f32(sum_vec, v1, v2); // sum += v1 * v2
    }

    // Horizontal sum of vector
    float sum = vaddvq_f32(sum_vec);

    // Handle remaining elements
    for (; i < len; ++i) {
        sum += a[i] * b[i];
    }

    return sum;

#elif defined(HAVE_AVX2)
    // Intel AVX2 implementation with FMA (8 floats at a time)
    __m256 sum_vec = _mm256_setzero_ps();
    uint32_t i = 0;

    // Process 8 elements at a time
    for (; i + 8 <= len; i += 8) {
        __m256 v1 = _mm256_loadu_ps(&a[i]);
        __m256 v2 = _mm256_loadu_ps(&b[i]);
        sum_vec = _mm256_fmadd_ps(v1, v2, sum_vec); // sum += v1 * v2
    }

    // Horizontal sum of vector
    __m128 sum_high = _mm256_extractf128_ps(sum_vec, 1);
    __m128 sum_low = _mm256_castps256_ps128(sum_vec);
    sum_low = _mm_add_ps(sum_low, sum_high);
    sum_low = _mm_hadd_ps(sum_low, sum_low);
    sum_low = _mm_hadd_ps(sum_low, sum_low);
    float sum = _mm_cvtss_f32(sum_low);

    // Handle remaining elements
    for (; i < len; ++i) {
        sum += a[i] * b[i];
    }

    return sum;

#elif defined(HAVE_SSE4)
    // Intel SSE4.1 implementation (4 floats at a time)
    // Note: SSE4.1 doesn't have FMA, so we use separate multiply and add
    __m128 sum_vec = _mm_setzero_ps();
    uint32_t i = 0;

    // Process 4 elements at a time
    for (; i + 4 <= len; i += 4) {
        __m128 v1 = _mm_loadu_ps(&a[i]);
        __m128 v2 = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(v1, v2);
        sum_vec = _mm_add_ps(sum_vec, prod);
    }

    // Horizontal sum of vector
    sum_vec = _mm_hadd_ps(sum_vec, sum_vec);
    sum_vec = _mm_hadd_ps(sum_vec, sum_vec);
    float sum = _mm_cvtss_f32(sum_vec);

    // Handle remaining elements
    for (; i < len; ++i) {
        sum += a[i] * b[i];
    }

    return sum;

#else
    // Scalar fallback
    float sum = 0.0f;
    for (uint32_t i = 0; i < len; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
#endif
}

} // namespace simd
