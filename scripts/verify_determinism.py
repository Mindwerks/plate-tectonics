#!/usr/bin/env python3
"""
Determinism verification script for plate tectonics simulation.

This script verifies that the simulation produces identical results when run
with the same seed and parameters. This is critical for:
- Reproducible bug reports
- Performance benchmarking consistency
- Regression testing
- Scientific reproducibility

Usage:
    python verify_determinism.py
    python verify_determinism.py --config 512x512 --runs 3
    python verify_determinism.py --extensive
"""

import argparse
import hashlib
import json
import sys
from typing import Dict, List, Tuple

try:
    import platec
except ImportError:
    print("Error: platec module not found. Please ensure the pybindings are installed.")
    print("You can install it with: pip install -e pybindings/")
    sys.exit(1)


# Test configurations
TEST_CONFIGS = {
    'tiny': {
        'width': 128,
        'height': 128,
        'steps': 5
    },
    'small': {
        'width': 256,
        'height': 256,
        'steps': 10
    },
    'medium': {
        'width': 512,
        'height': 512,
        'steps': 20
    },
    'large': {
        'width': 1024,
        'height': 1024,
        'steps': 40
    }
}

# Default simulation parameters
DEFAULT_PARAMS = {
    'seed': 42,
    'sea_level': 0.65,
    'erosion_period': 60,
    'folding_ratio': 0.02,
    'aggr_overlap_abs': 1000000,
    'aggr_overlap_rel': 0.33,
    'cycle_count': 2,
    'num_plates': 10
}


def compute_heightmap_hash(heightmap: List[float]) -> str:
    """
    Compute a hash of the heightmap for comparison.
    
    Args:
        heightmap: List of float values representing the heightmap
    
    Returns:
        SHA256 hash as hex string
    """
    # Convert floats to bytes for hashing
    # Using repr() to ensure exact float representation
    data = ''.join(repr(x) for x in heightmap).encode('utf-8')
    return hashlib.sha256(data).hexdigest()


def run_simulation(width: int, height: int, steps: int, seed: int = 42) -> Tuple[str, List[float]]:
    """
    Run a single simulation and return hash and heightmap.
    
    Args:
        width: Map width
        height: Map height
        steps: Number of simulation steps
        seed: Random seed
    
    Returns:
        Tuple of (hash, heightmap)
    """
    params = DEFAULT_PARAMS.copy()
    params['seed'] = seed
    
    # Create simulation
    p = platec.create(
        params['seed'],
        width,
        height,
        params['sea_level'],
        params['erosion_period'],
        params['folding_ratio'],
        params['aggr_overlap_abs'],
        params['aggr_overlap_rel'],
        params['cycle_count'],
        params['num_plates']
    )
    
    # Run simulation steps
    for _ in range(steps):
        platec.step(p)
    
    # Get final heightmap
    heightmap = platec.get_heightmap(p)
    
    # Cleanup
    platec.destroy(p)
    
    # Compute hash
    hm_hash = compute_heightmap_hash(heightmap)
    
    return hm_hash, heightmap


def verify_determinism(config_name: str, num_runs: int = 3) -> bool:
    """
    Verify determinism by running the same configuration multiple times.
    
    Args:
        config_name: Name of test configuration
        num_runs: Number of times to run
    
    Returns:
        True if all runs produce identical results
    """
    if config_name not in TEST_CONFIGS:
        print(f"Error: Unknown configuration '{config_name}'")
        return False
    
    config = TEST_CONFIGS[config_name]
    width = config['width']
    height = config['height']
    steps = config['steps']
    
    print(f"\nVerifying determinism: {config_name} ({width}×{height}, {steps} steps)")
    print(f"Running {num_runs} iterations with same seed...")
    
    hashes = []
    heightmaps = []
    
    for run in range(num_runs):
        print(f"  Run {run + 1}/{num_runs}...", end=' ', flush=True)
        hm_hash, heightmap = run_simulation(width, height, steps)
        hashes.append(hm_hash)
        heightmaps.append(heightmap)
        print(f"Hash: {hm_hash[:16]}...")
    
    # Check if all hashes are identical
    if len(set(hashes)) == 1:
        print(f"✓ PASS: All {num_runs} runs produced identical results")
        print(f"  Hash: {hashes[0]}")
        return True
    else:
        print(f"✗ FAIL: Runs produced different results!")
        print(f"  Unique hashes: {len(set(hashes))}")
        for i, h in enumerate(hashes):
            print(f"    Run {i + 1}: {h}")
        
        # Show some diagnostic info
        print("\n  Diagnostic Information:")
        for i in range(1, len(heightmaps)):
            diffs = sum(1 for j in range(len(heightmaps[0])) 
                       if heightmaps[0][j] != heightmaps[i][j])
            print(f"    Run 1 vs Run {i + 1}: {diffs} differing values "
                  f"({100.0 * diffs / len(heightmaps[0]):.2f}%)")
        
        return False


def verify_seed_independence(config_name: str = 'small') -> bool:
    """
    Verify that different seeds produce different results.
    
    Args:
        config_name: Name of test configuration
    
    Returns:
        True if different seeds produce different results
    """
    config = TEST_CONFIGS[config_name]
    width = config['width']
    height = config['height']
    steps = config['steps']
    
    print(f"\nVerifying seed independence: {config_name}")
    print(f"Running with different seeds...")
    
    seeds = [42, 123, 999, 12345]
    hashes = []
    
    for seed in seeds:
        print(f"  Seed {seed}...", end=' ', flush=True)
        hm_hash, _ = run_simulation(width, height, steps, seed)
        hashes.append(hm_hash)
        print(f"Hash: {hm_hash[:16]}...")
    
    # Check if all hashes are different
    if len(set(hashes)) == len(hashes):
        print(f"✓ PASS: All seeds produced unique results")
        return True
    else:
        print(f"✗ FAIL: Some seeds produced identical results!")
        print(f"  Unique hashes: {len(set(hashes))} out of {len(hashes)}")
        return False


def verify_step_consistency(config_name: str = 'tiny') -> bool:
    """
    Verify that simulation is consistent at intermediate steps.
    
    This checks that if we run to step N, then run a fresh simulation to step N,
    we get the same result.
    
    Args:
        config_name: Name of test configuration
    
    Returns:
        True if intermediate steps are consistent
    """
    config = TEST_CONFIGS[config_name]
    width = config['width']
    height = config['height']
    total_steps = config['steps']
    
    print(f"\nVerifying step consistency: {config_name}")
    
    params = DEFAULT_PARAMS.copy()
    
    # Run full simulation and capture intermediate states
    print(f"  Running full simulation ({total_steps} steps)...")
    p = platec.create(
        params['seed'],
        width,
        height,
        params['sea_level'],
        params['erosion_period'],
        params['folding_ratio'],
        params['aggr_overlap_abs'],
        params['aggr_overlap_rel'],
        params['cycle_count'],
        params['num_plates']
    )
    
    intermediate_hashes = []
    for step in range(total_steps):
        platec.step(p)
        hm = platec.get_heightmap(p)
        hm_hash = compute_heightmap_hash(hm)
        intermediate_hashes.append(hm_hash)
    
    platec.destroy(p)
    
    # Now verify a few intermediate steps
    test_steps = [total_steps // 4, total_steps // 2, total_steps]
    all_match = True
    
    for steps in test_steps:
        if steps > total_steps:
            continue
        
        print(f"  Verifying step {steps}...", end=' ', flush=True)
        hm_hash, _ = run_simulation(width, height, steps)
        
        if hm_hash == intermediate_hashes[steps - 1]:
            print(f"✓ Match")
        else:
            print(f"✗ Mismatch!")
            all_match = False
    
    if all_match:
        print(f"✓ PASS: All intermediate steps are consistent")
        return True
    else:
        print(f"✗ FAIL: Some intermediate steps are inconsistent")
        return False


def main():
    parser = argparse.ArgumentParser(
        description='Verify determinism of plate tectonics simulation'
    )
    parser.add_argument(
        '--config', '-c',
        choices=list(TEST_CONFIGS.keys()),
        default='small',
        help='Test configuration to use (default: small)'
    )
    parser.add_argument(
        '--runs', '-r',
        type=int,
        default=3,
        help='Number of runs for determinism check (default: 3)'
    )
    parser.add_argument(
        '--extensive', '-e',
        action='store_true',
        help='Run extensive tests on all configurations'
    )
    parser.add_argument(
        '--quick', '-q',
        action='store_true',
        help='Run quick test (tiny config, 2 runs)'
    )
    
    args = parser.parse_args()
    
    print("=" * 70)
    print("PLATE TECTONICS DETERMINISM VERIFICATION")
    print("=" * 70)
    
    all_passed = True
    
    if args.quick:
        # Quick test
        print("\nRunning quick verification test...")
        result = verify_determinism('tiny', 2)
        all_passed = all_passed and result
    
    elif args.extensive:
        # Extensive testing
        print("\nRunning extensive verification tests...")
        
        # Test all configurations
        for config_name in TEST_CONFIGS.keys():
            result = verify_determinism(config_name, args.runs)
            all_passed = all_passed and result
        
        # Test seed independence
        result = verify_seed_independence('small')
        all_passed = all_passed and result
        
        # Test step consistency
        result = verify_step_consistency('tiny')
        all_passed = all_passed and result
    
    else:
        # Single configuration test
        result = verify_determinism(args.config, args.runs)
        all_passed = all_passed and result
    
    # Summary
    print("\n" + "=" * 70)
    if all_passed:
        print("✓ ALL TESTS PASSED - Simulation is deterministic")
        print("=" * 70)
        return 0
    else:
        print("✗ SOME TESTS FAILED - Simulation may not be deterministic")
        print("=" * 70)
        print("\nPossible causes:")
        print("  - Uninitialized memory usage")
        print("  - Platform-dependent floating point behavior")
        print("  - Race conditions (if parallelized)")
        print("  - Use of non-seeded random sources")
        return 1


if __name__ == '__main__':
    sys.exit(main())
