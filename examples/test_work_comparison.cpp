// Test to compare work between serial and parallel versions
// Runs a single configuration with detailed output

#include <iostream>
#include <cstdlib>
#include "../src/lithosphere.hpp"

int main() {
    const uint32_t width = 1024;
    const uint32_t height = 1024;
    const uint32_t num_steps = 40;  // Run longer to see steady-state behavior

    std::cout << "Work Comparison Test (1024×1024, 40 steps)" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Test Serial
    std::cout << "\n=== SERIAL ===" << std::endl;
    setenv("PLATE_SERIAL", "1", 1);
    setenv("PLATE_TIMING", "1", 1);
    unsetenv("PLATE_PARALLEL");

    lithosphere lit_serial(123, width, height, 0.65f, 60, 0.02f, 1000000, 0.33f, 2, 10);
    for (uint32_t i = 0; i < num_steps; ++i) {
        lit_serial.update();
    }

    // Test Parallel
    std::cout << "\n=== PARALLEL ===" << std::endl;
    setenv("PLATE_PARALLEL", "1", 1);
    setenv("PLATE_TIMING", "1", 1);
    unsetenv("PLATE_SERIAL");

    lithosphere lit_parallel(123, width, height, 0.65f, 60, 0.02f, 1000000, 0.33f, 2, 10);
    for (uint32_t i = 0; i < num_steps; ++i) {
        lit_parallel.update();
    }

    std::cout << "\nTest complete!" << std::endl;
    return 0;
}
