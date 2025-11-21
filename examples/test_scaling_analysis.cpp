/******************************************************************************
 *  Scaling analysis: Test how speedup varies with resolution and steps
 *  Uses SAME SEED for fair comparison
 *****************************************************************************/

#include "../src/lithosphere.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <thread>
#include <numeric>

struct Result {
    uint32_t width;
    uint32_t height;
    uint32_t steps;
    long serial_ms;
    long parallel_ms;
    float speedup;
};

Result runTest(uint32_t width, uint32_t height, uint32_t steps) {
    const long SEED = 42;  // SAME SEED for fair comparison!
    const uint32_t num_plates = 10;
    const float sea_level = 0.65f;
    const uint32_t erosion_period = 60;
    const float folding_ratio = 0.02f;
    const uint32_t aggr_overlap_abs = 1000000;
    const float aggr_overlap_rel = 0.33f;
    const uint32_t cycle_count = 2;

    // Test SERIAL
    setenv("PLATE_SERIAL", "1", 1);
    unsetenv("PLATE_TIMING");

    auto serial_start = std::chrono::high_resolution_clock::now();
    {
        lithosphere litho(SEED, width, height, sea_level, erosion_period,
                          folding_ratio, aggr_overlap_abs, aggr_overlap_rel, cycle_count, num_plates);
        for (uint32_t step = 0; step < steps; ++step) {
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
        lithosphere litho(SEED, width, height, sea_level, erosion_period,
                          folding_ratio, aggr_overlap_abs, aggr_overlap_rel, cycle_count, num_plates);
        for (uint32_t step = 0; step < steps; ++step) {
            litho.update();
        }
    }
    auto parallel_end = std::chrono::high_resolution_clock::now();
    auto parallel_ms = std::chrono::duration_cast<std::chrono::milliseconds>(parallel_end - parallel_start).count();

    Result r;
    r.width = width;
    r.height = height;
    r.steps = steps;
    r.serial_ms = serial_ms;
    r.parallel_ms = parallel_ms;
    r.speedup = (float)serial_ms / parallel_ms;

    return r;
}

int main() {
    std::cout << "=" << std::string(78, '=') << "\n";
    std::cout << "SCALING ANALYSIS: Resolution and Step Count\n";
    std::cout << "=" << std::string(78, '=') << "\n\n";

    std::vector<Result> results;

    // Test 1: Fixed resolution, varying steps (linear scaling)
    std::cout << "TEST 1: STEP COUNT SCALING (fixed 1024×1024 resolution)\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::setw(8) << "Steps" << std::setw(12) << "Serial"
              << std::setw(12) << "Parallel" << std::setw(12) << "Speedup" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (uint32_t steps : {
                5, 10, 20, 40, 80, 100, 120, 160, 320
            }) {
        std::cout << std::setw(8) << steps << std::flush;
        Result r = runTest(1024, 1024, steps);
        results.push_back(r);
        std::cout << std::setw(12) << r.serial_ms << "ms"
                  << std::setw(11) << r.parallel_ms << "ms"
                  << std::setw(11) << std::fixed << std::setprecision(3) << r.speedup << "×\n";
    }

    // Analyze step scaling
    std::cout << "\nAnalysis: ";
    if (results.size() >= 2) {
        float speedup_first = results[0].speedup;
        float speedup_last = results[results.size()-1].speedup;
        float change_pct = ((speedup_last - speedup_first) / speedup_first) * 100;

        if (std::abs(change_pct) < 5) {
            std::cout << "Speedup is CONSTANT (~" << std::fixed << std::setprecision(2)
                      << (speedup_first + speedup_last)/2 << "×, variation "
                      << std::setprecision(1) << change_pct << "%)\n";
        } else if (change_pct > 0) {
            std::cout << "Speedup IMPROVES with more steps (+"
                      << std::setprecision(1) << change_pct << "%)\n";
        } else {
            std::cout << "Speedup DEGRADES with more steps ("
                      << std::setprecision(1) << change_pct << "%)\n";
        }
    }

    results.clear();

    // Test 2: Fixed steps, varying resolution (area scaling) - LARGE SCALE
    std::cout << "\n\nTEST 2: RESOLUTION SCALING (fixed 80 steps)\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::setw(12) << "Resolution" << std::setw(10) << "Area"
              << std::setw(12) << "Serial" << std::setw(12) << "Parallel"
              << std::setw(12) << "Speedup" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (uint32_t size : {
                256, 512, 1024, 2048, 4096, 8192
            }) {
        std::cout << std::setw(8) << size << "×" << size
                  << std::setw(9) << (size*size/1000) << "K" << std::flush;
        Result r = runTest(size, size, 80);
        results.push_back(r);
        std::cout << std::setw(11) << r.serial_ms << "ms"
                  << std::setw(11) << r.parallel_ms << "ms"
                  << std::setw(11) << std::fixed << std::setprecision(3) << r.speedup << "×\n";
    }

    // Analyze resolution scaling
    std::cout << "\nAnalysis: ";
    if (results.size() >= 2) {
        float speedup_first = results[0].speedup;
        float speedup_last = results[results.size()-1].speedup;
        float change_pct = ((speedup_last - speedup_first) / speedup_first) * 100;

        if (std::abs(change_pct) < 5) {
            std::cout << "Speedup is CONSTANT with resolution (~"
                      << std::fixed << std::setprecision(2)
                      << (speedup_first + speedup_last)/2 << "×)\n";
        } else if (change_pct > 5) {
            std::cout << "Speedup IMPROVES with larger resolution (+"
                      << std::setprecision(1) << change_pct << "%)\n";
            std::cout << "  → Better CPU cache utilization and/or amortized overhead\n";
        } else {
            std::cout << "Speedup DEGRADES with larger resolution ("
                      << std::setprecision(1) << change_pct << "%)\n";
            std::cout << "  → Memory bandwidth bottleneck or cache thrashing\n";
        }

        // Check if it's exponential (speedup ratio should be constant)
        if (results.size() >= 3) {
            std::vector<float> ratios;
            for (size_t i = 1; i < results.size(); ++i) {
                float ratio = results[i].speedup / results[i-1].speedup;
                ratios.push_back(ratio);
            }

            // Check if ratios are roughly constant (exponential growth)
            float avg_ratio = std::accumulate(ratios.begin(), ratios.end(), 0.0f) / ratios.size();

            float ratio_variance = 0;
            for (float r : ratios) {
                float diff = r - avg_ratio;
                ratio_variance += diff * diff;
            }
            ratio_variance /= ratios.size();

            if (ratio_variance < 0.01 && std::abs(avg_ratio - 1.0) > 0.05) {
                std::cout << "  → Growth pattern: EXPONENTIAL (ratio ~"
                          << std::fixed << std::setprecision(3) << avg_ratio << ")\n";
            }
        }
    }

    std::cout << "\n\n" << std::string(80, '=') << "\n";
    std::cout << "CONCLUSION\n";
    std::cout << std::string(80, '=') << "\n";
    std::cout << "Speedup characteristics:\n";
    std::cout << "  • Step count: ";

    // Re-check step scaling conclusion
    results.clear();
    for (uint32_t steps : {
                10, 40
            }) {
        results.push_back(runTest(1024, 1024, steps));
    }
    float step_change = ((results[1].speedup - results[0].speedup) / results[0].speedup) * 100;
    if (std::abs(step_change) < 5) {
        std::cout << "CONSTANT (independent of step count)\n";
    } else {
        std::cout << "VARIES " << std::showpos << std::setprecision(1) << step_change << "% over 4× step increase\n" << std::noshowpos;
    }

    std::cout << "  • Resolution: Analyze the table above for pattern\n";
    std::cout << "  • Hardware: " << std::thread::hardware_concurrency() << " logical cores\n";
    std::cout << "\nNote: Speedup depends on work distribution across threads.\n";
    std::cout << "Phase 1 (spatial partitioning): Benefits from larger resolutions\n";
    std::cout << "Phase 2 (batched atomics): Benefits are constant per cell\n";

    return 0;
}
