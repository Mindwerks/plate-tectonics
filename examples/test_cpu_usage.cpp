/******************************************************************************
 *  Simple CPU stress test to verify we can actually use multiple cores
 *****************************************************************************/

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cmath>

// CPU-intensive work
double computePi(uint64_t iterations) {
    double sum = 0.0;
    for (uint64_t i = 0; i < iterations; ++i) {
        double x = (i + 0.5) / iterations;
        sum += 4.0 / (1.0 + x * x);
    }
    return sum / iterations;
}

int main() {
    std::cout << "CPU Usage Test - Verify Multi-Core Works\n";
    std::cout << "=========================================\n";
    std::cout << "Hardware threads: " << std::thread::hardware_concurrency() << "\n\n";

    const uint64_t total_work = 2000000000; // 2B iterations total

    // Test 1: Single thread
    std::cout << "Test 1: Single thread (watch Activity Monitor - should use ~1 core)\n";
    auto t1_start = std::chrono::high_resolution_clock::now();
    double result1 = computePi(total_work);
    auto t1_end = std::chrono::high_resolution_clock::now();
    auto t1_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1_end - t1_start).count();
    std::cout << "  Result: " << result1 << ", Time: " << t1_ms << " ms\n\n";

    // Test 2: Multi-threaded (divide work equally)
    const uint32_t num_threads = std::thread::hardware_concurrency();
    const uint64_t work_per_thread = total_work / num_threads;

    std::cout << "Test 2: " << num_threads << " threads, " << work_per_thread << " iterations each\n";
    std::cout << "        (watch Activity Monitor - should use ALL cores)\n";

    std::vector<std::thread> threads;
    std::vector<double> results(num_threads);

    auto t2_start = std::chrono::high_resolution_clock::now();
    for (uint32_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            results[i] = computePi(work_per_thread);
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    auto t2_end = std::chrono::high_resolution_clock::now();
    auto t2_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2_end - t2_start).count();

    double total = 0;
    for (double r : results) total += r;
    std::cout << "  Average result: " << total / num_threads << ", Time: " << t2_ms << " ms\n\n";

    // Results
    std::cout << "Results:\n";
    std::cout << "========\n";
    std::cout << "  Single thread: " << t1_ms << " ms\n";
    std::cout << "  Multi-thread:  " << t2_ms << " ms\n";
    std::cout << "  Speedup:       " << (double)t1_ms / t2_ms << "×\n\n";

    if (t2_ms < t1_ms * 0.5) {
        std::cout << "✓ Multi-threading WORKS! You should have seen all cores active.\n";
    } else {
        std::cout << "✗ Multi-threading NOT working properly!\n";
    }

    return 0;
}
