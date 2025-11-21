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
#include "map_drawing.hpp"
#include "sqrdmd.hpp"
#include "gtest/gtest.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>

///
/// Regression test to ensure simulation output remains consistent
/// Uses seed 12345 and compares file checksums of generated images
///

namespace {

// Simple CRC32 implementation (no external dependencies)
uint32_t crc32(const unsigned char* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

// Helper function to compute CRC32 checksum of a file
uint32_t compute_file_checksum(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return 0;
    }

    // Read entire file into memory
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return crc32(buffer.data(), size);
}

// Helper functions from simulation.cpp
void produce_image_colors(float* heightmap, int width, int height, const char* filename) {
    writeImageColors((char*)filename, width, height, heightmap, "FOO");
}

void save_image(void* p, const char* filename, const int width, const int height) {
    const float* heightmap = platec_api_get_heightmap(p);
    float* copy = new float[width * height];
    memcpy(copy, heightmap, sizeof(float) * width * height);
    normalize(copy, width * height);
    produce_image_colors(copy, width, height, filename);
    delete[] copy;
}

} // anonymous namespace

TEST(Regression, SimulationSeed12345_OutputConsistency) {
    // This test ensures simulation output remains consistent across code changes
    // Uses raw heightmap data comparison instead of PNG files for platform independence

    const uint32_t seed = 12345;
    const uint32_t width = 600;
    const uint32_t height = 400;
    const size_t map_size = width * height;

    // Create simulation with same parameters as baseline
    void* p = platec_api_create(seed, width, height, 0.65, 60, 0.02, 1000000, 0.33, 2, 10);
    ASSERT_NE(p, nullptr) << "Failed to create simulation";

    // Get initial heightmap and compute checksum
    const float* initial_map = platec_api_get_heightmap(p);
    ASSERT_NE(initial_map, nullptr);
    uint32_t initial_crc32 = crc32(reinterpret_cast<const unsigned char*>(initial_map),
                                   map_size * sizeof(float));

    // Run simulation to completion
    while (platec_api_is_finished(p) == 0) {
        platec_api_step(p);
    }

    // Get final heightmap and compute checksum
    const float* final_map = platec_api_get_heightmap(p);
    ASSERT_NE(final_map, nullptr);
    uint32_t final_crc32 = crc32(reinterpret_cast<const unsigned char*>(final_map),
                                 map_size * sizeof(float));

    // Clean up
    platec_api_destroy(p);

    // Expected checksums from baseline run with seed 12345
    // These are checksums of the raw heightmap data
    // NOTE: These checksums will differ between architectures due to SIMD differences
    // On macOS ARM64 (NEON):
    const uint32_t EXPECTED_INITIAL_CRC32_ARM64 = 0x3E1ED204;
    const uint32_t EXPECTED_FINAL_CRC32_ARM64 = 0xD8F3679C;
    // On Linux x86-64 (AVX2/SSE):
    const uint32_t EXPECTED_INITIAL_CRC32_X86 = 0xBC72B08C;
    const uint32_t EXPECTED_FINAL_CRC32_X86 = 0x007415B5;

    // Check if we match either architecture's expected values
    bool initial_matches = (initial_crc32 == EXPECTED_INITIAL_CRC32_ARM64) ||
                          (initial_crc32 == EXPECTED_INITIAL_CRC32_X86);
    bool final_matches = (final_crc32 == EXPECTED_FINAL_CRC32_ARM64) ||
                        (final_crc32 == EXPECTED_FINAL_CRC32_X86);

    EXPECT_TRUE(initial_matches)
        << "Initial heightmap checksum does not match known baseline.\n"
        << "Expected (ARM64): 0x" << std::hex << EXPECTED_INITIAL_CRC32_ARM64 << "\n"
        << "Expected (x86):   0x" << std::hex << EXPECTED_INITIAL_CRC32_X86 << "\n"
        << "Got:              0x" << std::hex << initial_crc32 << "\n"
        << "This may indicate the simulation initial state has changed, or a new architecture.";

    EXPECT_TRUE(final_matches)
        << "Final heightmap checksum does not match known baseline.\n"
        << "Expected (ARM64): 0x" << std::hex << EXPECTED_FINAL_CRC32_ARM64 << "\n"
        << "Expected (x86):   0x" << std::hex << EXPECTED_FINAL_CRC32_X86 << "\n"
        << "Got:              0x" << std::hex << final_crc32 << "\n"
        << "This may indicate the simulation output has changed, or a new architecture.";
}
