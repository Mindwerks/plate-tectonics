/******************************************************************************
 *  Test program to verify C++ parallelism is actually working
 *  This bypasses Python entirely to prove multi-threading works
 *****************************************************************************/

#include "../src/lithosphere.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <cstdlib>

struct TestConfig {
    uint32_t width;
    uint32_t height;
    uint32_t steps;
    std::string name;
};

void runTest(const TestConfig& config) {
    const uint32_t num_plates = 10;
    const float sea_level = 0.65f;
    const uint32_t erosion_period = 60;
    const float folding_ratio = 0.02f;
    const uint32_t aggr_overlap_abs = 1000000;
    const float aggr_overlap_rel = 0.33f;
    const uint32_t cycle_count = 2;

    std::cout << "\n" << config.name << " (" << config.width << "×" << config.height
              << ", " << config.steps << " steps)\n";
    std::cout << std::string(60, '=') << "\n";

    // Test SERIAL
    setenv("PLATE_SERIAL", "1", 1);
    unsetenv("PLATE_TIMING");

    auto serial_start = std::chrono::high_resolution_clock::now();
    {
        lithosphere litho(1, config.width, config.height, sea_level, erosion_period,
                          folding_ratio, aggr_overlap_abs, aggr_overlap_rel, cycle_count, num_plates);
        for (uint32_t step = 0; step < config.steps; ++step) {
            litho.update();
        }
    }
    auto serial_end = std::chrono::high_resolution_clock::now();
    auto serial_ms = std::chrono::duration_cast<std::chrono::milliseconds>(serial_end - serial_start).count();

    // Test PARALLEL
    unsetenv("PLATE_SERIAL");
    unsetenv("PLATE_TIMING");

    auto parallel_start = std::chrono::high_resolution_clock::now();
    {
        lithosphere litho(2, config.width, config.height, sea_level, erosion_period,
                          folding_ratio, aggr_overlap_abs, aggr_overlap_rel, cycle_count, num_plates);
        for (uint32_t step = 0; step < config.steps; ++step) {
            litho.update();
        }
    }
    auto parallel_end = std::chrono::high_resolution_clock::now();
    auto parallel_ms = std::chrono::duration_cast<std::chrono::milliseconds>(parallel_end - parallel_start).count();

    // Results
    float speedup = (float)serial_ms / parallel_ms;
    std::cout << "  Serial:   " << std::setw(6) << serial_ms << " ms\n";
    std::cout << "  Parallel: " << std::setw(6) << parallel_ms << " ms\n";
    std::cout << "  Speedup:  " << std::fixed << std::setprecision(2) << speedup << "×";

    if (speedup > 1.05) {
        std::cout << "  ✓ FASTER\n";
    } else if (speedup < 0.95) {
        std::cout << "  ✗ SLOWER\n";
    } else {
        std::cout << "  ≈ SAME\n";
    }
}

int main() {
    std::cout << "Testing C++ Parallelism at Multiple Resolutions\n";
    std::cout << "================================================\n";
    std::cout << "Hardware threads: " << std::thread::hardware_concurrency() << "\n";

    std::vector<TestConfig> configs = {
        {512, 512, 20, "Small"},
        {1024, 1024, 20, "Medium"},
        {2048, 2048, 10, "Large"},
        {4096, 4096, 5, "Very Large"}
    };

    for (const auto& config : configs) {
        runTest(config);
    }

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "NOTES:\n";
    std::cout << "- Parallel overhead is high for small resolutions\n";
    std::cout << "- Larger resolutions should show better speedup\n";
    std::cout << "- Watch Activity Monitor to see multi-core usage\n";

    return 0;
}
