#!/usr/bin/env python3
"""
Performance benchmarking script for plate tectonics simulation.

This script measures the performance of building simulations at different
resolutions (512x512, 1024x1024, 4096x4096) with varying step counts
(10, 20, 40 steps).

Usage:
    python benchmark_performance.py
    python benchmark_performance.py --output results.json
    python benchmark_performance.py --runs 3
"""

import argparse
import json
import sys
import time
from datetime import datetime
from typing import Dict, List, Tuple

try:
    import platec
except ImportError:
    print("Error: platec module not found. Please ensure the pybindings are installed.")
    print("You can install it with: pip install -e pybindings/")
    sys.exit(1)


# Benchmark configurations
RESOLUTIONS = [
    (512, 512),
    (1024, 1024),
    (4096, 4096),
]

STEP_COUNTS = [10, 20, 40]

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


def run_benchmark(width: int, height: int, steps: int, params: Dict = None) -> Dict:
    """
    Run a single benchmark test.
    
    Args:
        width: Map width
        height: Map height
        steps: Number of simulation steps
        params: Simulation parameters (uses defaults if None)
    
    Returns:
        Dictionary containing timing results
    """
    if params is None:
        params = DEFAULT_PARAMS.copy()
    
    # Creation time
    start_create = time.perf_counter()
    p = platec.create(
        seed=params['seed'],
        width=width,
        height=height,
        sea_level=params['sea_level'],
        erosion_period=params['erosion_period'],
        folding_ratio=params['folding_ratio'],
        aggr_overlap_abs=params['aggr_overlap_abs'],
        aggr_overlap_rel=params['aggr_overlap_rel'],
        cycle_count=params['cycle_count'],
        num_plates=params['num_plates']
    )
    end_create = time.perf_counter()
    create_time = end_create - start_create
    
    # Step time
    step_times = []
    start_steps = time.perf_counter()
    for i in range(steps):
        step_start = time.perf_counter()
        platec.step(p)
        step_end = time.perf_counter()
        step_times.append(step_end - step_start)
    end_steps = time.perf_counter()
    total_step_time = end_steps - start_steps
    
    # Get heightmap time
    start_get = time.perf_counter()
    heightmap = platec.get_heightmap(p)
    end_get = time.perf_counter()
    get_time = end_get - start_get
    
    # Cleanup
    platec.destroy(p)
    
    total_time = create_time + total_step_time + get_time
    
    return {
        'create_time': create_time,
        'total_step_time': total_step_time,
        'avg_step_time': total_step_time / steps if steps > 0 else 0,
        'min_step_time': min(step_times) if step_times else 0,
        'max_step_time': max(step_times) if step_times else 0,
        'get_heightmap_time': get_time,
        'total_time': total_time,
        'step_times': step_times
    }


def format_time(seconds: float) -> str:
    """Format time in seconds to a human-readable string."""
    if seconds < 0.001:
        return f"{seconds * 1000000:.2f} µs"
    elif seconds < 1:
        return f"{seconds * 1000:.2f} ms"
    else:
        return f"{seconds:.3f} s"


def print_result(width: int, height: int, steps: int, result: Dict, run: int = None):
    """Print benchmark results in a formatted way."""
    run_str = f" (Run {run})" if run is not None else ""
    print(f"\n{'='*70}")
    print(f"Resolution: {width}x{height}, Steps: {steps}{run_str}")
    print(f"{'='*70}")
    print(f"  Creation time:      {format_time(result['create_time'])}")
    print(f"  Total step time:    {format_time(result['total_step_time'])}")
    print(f"  Average step time:  {format_time(result['avg_step_time'])}")
    print(f"  Min step time:      {format_time(result['min_step_time'])}")
    print(f"  Max step time:      {format_time(result['max_step_time'])}")
    print(f"  Get heightmap time: {format_time(result['get_heightmap_time'])}")
    print(f"  TOTAL TIME:         {format_time(result['total_time'])}")


def average_results(results: List[Dict]) -> Dict:
    """Calculate average results from multiple runs."""
    if not results:
        return {}
    
    num_runs = len(results)
    avg = {
        'create_time': sum(r['create_time'] for r in results) / num_runs,
        'total_step_time': sum(r['total_step_time'] for r in results) / num_runs,
        'avg_step_time': sum(r['avg_step_time'] for r in results) / num_runs,
        'min_step_time': sum(r['min_step_time'] for r in results) / num_runs,
        'max_step_time': sum(r['max_step_time'] for r in results) / num_runs,
        'get_heightmap_time': sum(r['get_heightmap_time'] for r in results) / num_runs,
        'total_time': sum(r['total_time'] for r in results) / num_runs,
    }
    return avg


def main():
    parser = argparse.ArgumentParser(
        description='Benchmark plate tectonics simulation performance'
    )
    parser.add_argument(
        '--output', '-o',
        type=str,
        help='Output file for JSON results (default: print to console only)'
    )
    parser.add_argument(
        '--runs', '-r',
        type=int,
        default=1,
        help='Number of runs per configuration (default: 1)'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Show results for each individual run'
    )
    
    args = parser.parse_args()
    
    print("=" * 70)
    print("PLATE TECTONICS PERFORMANCE BENCHMARK")
    print("=" * 70)
    print(f"Timestamp: {datetime.now().isoformat()}")
    print(f"Runs per configuration: {args.runs}")
    print(f"Resolutions: {', '.join(f'{w}x{h}' for w, h in RESOLUTIONS)}")
    print(f"Step counts: {', '.join(map(str, STEP_COUNTS))}")
    print()
    
    all_results = {}
    
    for width, height in RESOLUTIONS:
        for steps in STEP_COUNTS:
            config_key = f"{width}x{height}_{steps}steps"
            print(f"\nRunning benchmark: {config_key}")
            print("-" * 70)
            
            run_results = []
            for run in range(args.runs):
                print(f"  Run {run + 1}/{args.runs}...", end=' ', flush=True)
                result = run_benchmark(width, height, steps)
                run_results.append(result)
                print(f"Done ({format_time(result['total_time'])})")
                
                if args.verbose and args.runs > 1:
                    print_result(width, height, steps, result, run + 1)
            
            # Calculate and store average results
            if args.runs > 1:
                avg_result = average_results(run_results)
                print(f"\n  Average over {args.runs} runs:")
                print_result(width, height, steps, avg_result)
            else:
                avg_result = run_results[0]
                print_result(width, height, steps, avg_result)
            
            all_results[config_key] = {
                'width': width,
                'height': height,
                'steps': steps,
                'runs': args.runs,
                'average': avg_result,
                'all_runs': run_results if args.runs > 1 else None
            }
    
    # Summary
    print("\n" + "=" * 70)
    print("SUMMARY")
    print("=" * 70)
    print(f"{'Configuration':<25} {'Total Time':<15} {'Avg Step Time':<15}")
    print("-" * 70)
    for config_key, data in all_results.items():
        avg = data['average']
        print(f"{config_key:<25} {format_time(avg['total_time']):<15} {format_time(avg['avg_step_time']):<15}")
    
    # Save to file if requested
    if args.output:
        output_data = {
            'timestamp': datetime.now().isoformat(),
            'runs_per_config': args.runs,
            'parameters': DEFAULT_PARAMS,
            'results': all_results
        }
        
        with open(args.output, 'w') as f:
            json.dump(output_data, f, indent=2)
        
        print(f"\nResults saved to: {args.output}")


if __name__ == '__main__':
    main()
