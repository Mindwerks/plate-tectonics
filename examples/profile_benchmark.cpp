/**
 * Simple benchmark for profiling.
 * Runs several plate tectonics simulations for profiling analysis.
 */

#include <iostream>
#include <chrono>
#include "../src/lithosphere.hpp"

int main() {
    std::cout << "Running profiling benchmark..." << std::endl;

    // Configuration for profiling
    const uint32_t width = 1024;
    const uint32_t height = 1024;
    const uint32_t num_plates = 10;
    const uint32_t steps = 40;
    const long seed = 42;
    const uint32_t num_cycles = 2;  // Number of restarts

    auto start = std::chrono::high_resolution_clock::now();

    // Run simulation
    lithosphere litho(seed, width, height, 0.65f, 60, 0.02f, 1000000, 0.33f, num_cycles, num_plates);

    std::cout << "Simulating " << steps << " steps on "
              << width << "x" << height << " grid with "
              << num_plates << " plates..." << std::endl;

    for (uint32_t step = 0; step < steps; ++step) {
        litho.update();
        if (step % 10 == 0) {
            std::cout << "  Step " << step << "/" << steps << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Completed in " << duration.count() << " ms" << std::endl;

    return 0;
}
