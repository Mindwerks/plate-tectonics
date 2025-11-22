/******************************************************************************
 *  plate-tectonics, a plate tectonics simulation library
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
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

#include "platecapi.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>

///
/// Regression test to ensure simulation output remains consistent
/// Uses statistical comparison of heightmap data to detect meaningful changes
/// while being tolerant of minor floating-point differences across platforms
///

namespace {

// Statistical summary of heightmap data
struct HeightmapStats {
    float min;
    float max;
    float mean;
    float median;
    float std_dev;
    float q25;  // 25th percentile
    float q75;  // 75th percentile
};

// Compute statistical summary of heightmap
HeightmapStats compute_stats(const float* heightmap, size_t size) {
    HeightmapStats stats;

    // Copy data for sorting (to find median and quantiles)
    std::vector<float> sorted_data(heightmap, heightmap + size);
    std::sort(sorted_data.begin(), sorted_data.end());

    // Min/max
    stats.min = sorted_data.front();
    stats.max = sorted_data.back();

    // Median and quantiles
    stats.median = sorted_data[size / 2];
    stats.q25 = sorted_data[size / 4];
    stats.q75 = sorted_data[(3 * size) / 4];

    // Mean
    double sum = 0.0;
    for (size_t i = 0; i < size; i++) {
        sum += heightmap[i];
    }
    stats.mean = static_cast<float>(sum / size);

    // Standard deviation
    double variance = 0.0;
    for (size_t i = 0; i < size; i++) {
        double diff = heightmap[i] - stats.mean;
        variance += diff * diff;
    }
    stats.std_dev = static_cast<float>(std::sqrt(variance / size));

    return stats;
}

// Compare two stats with adaptive tolerance based on metric type
bool stats_match(const HeightmapStats& actual, const HeightmapStats& expected,
                 float central_tolerance, float extrema_tolerance) {
    auto close_enough = [](float a, float b, float rel_tol) {
        // Use relative tolerance: allow X% difference relative to the expected value
        // Also use absolute tolerance for values near zero to avoid division issues
        float abs_tolerance = std::max(0.05f, std::abs(b) * rel_tol);
        return std::abs(a - b) <= abs_tolerance;
    };

    // Min/max are single extreme values - more sensitive to platform differences
    // Use more generous tolerance
    bool extrema_ok = close_enough(actual.min, expected.min, extrema_tolerance) &&
                      close_enough(actual.max, expected.max, extrema_tolerance);

    // Central tendency metrics are more stable - use stricter tolerance
    bool central_ok = close_enough(actual.mean, expected.mean, central_tolerance) &&
                      close_enough(actual.median, expected.median, central_tolerance) &&
                      close_enough(actual.std_dev, expected.std_dev, central_tolerance) &&
                      close_enough(actual.q25, expected.q25, central_tolerance) &&
                      close_enough(actual.q75, expected.q75, central_tolerance);

    return extrema_ok && central_ok;
}

} // anonymous namespace

TEST(Regression, SimulationSeed12345_OutputConsistency) {
    // This test ensures simulation output remains consistent across code changes
    // Uses statistical comparison of heightmap data to detect meaningful changes
    // while tolerating minor floating-point differences across platforms/compilers

    const uint32_t seed = 12345;
    const uint32_t width = 600;
    const uint32_t height = 400;
    const size_t map_size = width * height;

    // Create simulation with same parameters as baseline
    void* p = platec_api_create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10);
    ASSERT_NE(p, nullptr) << "Failed to create simulation";

    // Get initial heightmap and compute statistics
    const float* initial_map = platec_api_get_heightmap(p);
    ASSERT_NE(initial_map, nullptr);
    HeightmapStats initial_stats = compute_stats(initial_map, map_size);

    // Run simulation to completion
    while (platec_api_is_finished(p) == 0) {
        platec_api_step(p);
    }

    // Get final heightmap and compute statistics
    const float* final_map = platec_api_get_heightmap(p);
    ASSERT_NE(final_map, nullptr);
    HeightmapStats final_stats = compute_stats(final_map, map_size);

    // Clean up
    platec_api_destroy(p);

    // Expected statistical properties from baseline runs with seed 12345
    // These should be consistent across platforms with minor floating-point tolerance
    // Baseline values obtained from macOS ARM64 (Apple Clang, NEON)

    HeightmapStats expected_initial = {
        0.1f,       // min
        2.0f,       // max
        0.689232f,  // mean
        0.1f,       // median
        0.779593f,  // std_dev
        0.1f,       // q25
        1.63843f    // q75
    };

    HeightmapStats expected_final = {
        0.0142494f,  // min
        11.2925f,    // max
        0.6208f,     // mean
        0.112179f,   // median
        0.943399f,   // std_dev
        0.0980789f,  // q25
        0.85813f     // q75
    };

    // Use adaptive tolerance to account for platform differences
    // while still catching major regressions
    // Central tendency metrics (mean, median, std_dev, quantiles) use strict tolerance
    // Extrema (min, max) use generous tolerance as they're more variable
    const float central_tolerance = 0.01f;   // 1% for mean, median, std_dev, quantiles
    const float extrema_tolerance = 0.15f;   // 15% for min/max

    bool initial_matches = stats_match(initial_stats, expected_initial,
                                       central_tolerance, extrema_tolerance);
    bool final_matches = stats_match(final_stats, expected_final,
                                     central_tolerance, extrema_tolerance);

    // Helper lambda to format difference with sign (fixed decimal notation)
    auto format_diff = [](float actual, float expected) -> std::string {
        float diff = actual - expected;
        char sign = (diff >= 0) ? '+' : '-';
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%c%.10f", sign, std::abs(diff));
        return std::string(buffer);
    };

    // Always print statistics for tracking simulation evolution over time
    std::cout << "\n=== Initial heightmap statistics ===\n";
    std::cout << "  min:     " << initial_stats.min << " (diff: "
              << format_diff(initial_stats.min, expected_initial.min) << ")\n";
    std::cout << "  max:     " << initial_stats.max << " (diff: "
              << format_diff(initial_stats.max, expected_initial.max) << ")\n";
    std::cout << "  mean:    " << initial_stats.mean << " (diff: "
              << format_diff(initial_stats.mean, expected_initial.mean) << ")\n";
    std::cout << "  median:  " << initial_stats.median << " (diff: "
              << format_diff(initial_stats.median, expected_initial.median) << ")\n";
    std::cout << "  std_dev: " << initial_stats.std_dev << " (diff: "
              << format_diff(initial_stats.std_dev, expected_initial.std_dev) << ")\n";
    std::cout << "  q25:     " << initial_stats.q25 << " (diff: "
              << format_diff(initial_stats.q25, expected_initial.q25) << ")\n";
    std::cout << "  q75:     " << initial_stats.q75 << " (diff: "
              << format_diff(initial_stats.q75, expected_initial.q75) << ")\n";
    std::cout << "  Status:  " << (initial_matches ? "✓ PASS" : "✗ FAIL") << "\n";

    std::cout << "\n=== Final heightmap statistics ===\n";
    std::cout << "  min:     " << final_stats.min << " (diff: "
              << format_diff(final_stats.min, expected_final.min) << ")\n";
    std::cout << "  max:     " << final_stats.max << " (diff: "
              << format_diff(final_stats.max, expected_final.max) << ")\n";
    std::cout << "  mean:    " << final_stats.mean << " (diff: "
              << format_diff(final_stats.mean, expected_final.mean) << ")\n";
    std::cout << "  median:  " << final_stats.median << " (diff: "
              << format_diff(final_stats.median, expected_final.median) << ")\n";
    std::cout << "  std_dev: " << final_stats.std_dev << " (diff: "
              << format_diff(final_stats.std_dev, expected_final.std_dev) << ")\n";
    std::cout << "  q25:     " << final_stats.q25 << " (diff: "
              << format_diff(final_stats.q25, expected_final.q25) << ")\n";
    std::cout << "  q75:     " << final_stats.q75 << " (diff: "
              << format_diff(final_stats.q75, expected_final.q75) << ")\n";
    std::cout << "  Status:  " << (final_matches ? "✓ PASS" : "✗ FAIL") << "\n";
    std::cout << std::endl;

    EXPECT_TRUE(initial_matches)
        << "Initial heightmap statistics differ significantly from baseline.\n"
        << "This may indicate a change in the simulation's initial state generation.\n"
        << "Review the output above to determine if this is expected.";

    EXPECT_TRUE(final_matches)
        << "Final heightmap statistics differ significantly from baseline.\n"
        << "This may indicate a change in the simulation output.\n"
        << "Review the output above to determine if this is expected.";
}
