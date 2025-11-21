# Scripts Directory

This directory contains utility scripts for testing and benchmarking the plate tectonics simulation.

## Available Scripts

### benchmark_performance.py

Performance benchmarking script that measures simulation performance across different resolutions and step counts.

**Features:**
- Tests three resolutions: 512×512, 1024×1024, and 4096×4096
- Tests three step counts: 10, 20, and 40 steps
- Measures creation time, step execution time, and heightmap retrieval time
- Supports multiple runs for statistical averaging
- Exports results to JSON format

**Usage:**

Basic benchmark (single run):
```bash
python scripts/benchmark_performance.py
```

Multiple runs with averaging:
```bash
python scripts/benchmark_performance.py --runs 3
```

Save results to JSON file:
```bash
python scripts/benchmark_performance.py --output baseline_results.json
```

Show individual run results:
```bash
python scripts/benchmark_performance.py --runs 3 --verbose
```

**Requirements:**
- The `platec` Python bindings must be installed
- Install with: `pip install -e pybindings/`

**Output:**
The script provides detailed timing information including:
- Creation time (initialization)
- Total step time (all simulation steps)
- Average/min/max step time
- Heightmap retrieval time
- Total execution time

Results can be exported to JSON for tracking performance improvements over time.

### verify_determinism.py

Determinism verification script that ensures the simulation produces identical results for the same seed and parameters.

**Features:**
- Verifies bit-exact reproducibility across multiple runs
- Tests different configurations (tiny, small, medium, large)
- Validates seed independence (different seeds → different results)
- Checks step consistency (intermediate states are reproducible)
- Provides SHA256 hashes for result comparison

**Usage:**

Quick verification test:
```bash
python scripts/verify_determinism.py --quick
```

Test specific configuration:
```bash
python scripts/verify_determinism.py --config medium --runs 5
```

Extensive testing (all configurations):
```bash
python scripts/verify_determinism.py --extensive
```

**Requirements:**
- The `platec` Python bindings must be installed
- Install with: `pip install -e pybindings/`

**Output:**
The script provides:
- Pass/fail status for each test
- SHA256 hash of heightmap results
- Diagnostic information if non-determinism is detected

**Why This Matters:**
- **Bug Reproduction:** Same seed → same results means bugs are reproducible
- **Performance Testing:** Ensures benchmarks measure actual performance, not randomness
- **Scientific Validity:** Critical for reproducible research
- **Regression Testing:** Detects unintended behavior changes

**Note:** The simulation is designed to be fully deterministic when using the same seed. 
All tests should pass. If any test fails, it indicates a serious issue that should be 
investigated immediately (e.g., uninitialized memory, platform-dependent behavior).

## Setting Up a Performance Baseline

To establish a performance baseline before optimization:

1. Ensure the Python bindings are installed:
   ```bash
   cd pybindings
   pip install -e .
   cd ..
   ```

2. Run the benchmark with multiple iterations:
   ```bash
   python scripts/benchmark_performance.py --runs 5 --output baseline.json
   ```

3. After making optimizations, run again:
   ```bash
   python scripts/benchmark_performance.py --runs 5 --output optimized.json
   ```

4. Compare the results to quantify improvements

## Determinism and Reproducibility

The plate tectonics simulation is fully deterministic - the same seed produces identical results every time. This is critical for:

### Before Performance Testing

Always verify determinism first:

```bash
# Quick check before benchmarking
python scripts/verify_determinism.py --quick

# Or comprehensive verification
python scripts/verify_determinism.py --extensive
```

This ensures your performance measurements are comparing actual algorithmic changes, not random variation.

### Workflow for Optimization

1. **Verify baseline determinism:**
   ```bash
   python scripts/verify_determinism.py --config medium --runs 5
   ```

2. **Establish performance baseline:**
   ```bash
   python scripts/benchmark_performance.py --runs 5 --output baseline.json
   ```

3. **Make your optimizations**

4. **Verify determinism is maintained:**
   ```bash
   python scripts/verify_determinism.py --extensive
   ```
   **All tests must still pass!**

5. **Measure performance improvement:**
   ```bash
   python scripts/benchmark_performance.py --runs 5 --output optimized.json
   ```

6. **Compare results to quantify speedup**

### For Bug Reports

When reporting bugs, use determinism to help developers reproduce:

```bash
# Find problematic seed
python scripts/verify_determinism.py --extensive

# If a specific seed causes issues, report it with:
# - Seed value
# - Resolution and steps
# - All simulation parameters
# - Expected vs actual behavior
```

For more details, see `docs/determinism.md`.
