#!/usr/bin/env python3
"""
Compare two benchmark results and show performance differences.
"""

import json
import sys
from pathlib import Path


def format_time(seconds):
    """Format time in appropriate units."""
    if seconds >= 1.0:
        return f"{seconds:.3f}s"
    else:
        return f"{seconds*1000:.2f}ms"


def compare_benchmarks(baseline_file, new_file):
    """Compare two benchmark files and show differences."""
    with open(baseline_file) as f:
        baseline = json.load(f)
    with open(new_file) as f:
        new = json.load(f)
    
    print('=' * 80)
    print(f'BENCHMARK COMPARISON: {baseline_file} vs {new_file}')
    print('=' * 80)
    print()
    print(f"{'Configuration':<25} {'Baseline':<15} {'New':<15} {'Speedup':<12} {'Status'}")
    print('-' * 80)
    
    configs = sorted(baseline['results'].keys())
    speedups = []
    
    for config in configs:
        b_time = baseline['results'][config]['average']['total_time']
        n_time = new['results'][config]['average']['total_time']
        speedup = b_time / n_time
        speedups.append(speedup)
        
        b_str = format_time(b_time)
        n_str = format_time(n_time)
        
        # Determine status
        if speedup >= 1.05:
            status = "✓ Faster"
        elif speedup >= 1.02:
            status = "+ Improved"
        elif speedup >= 0.98:
            status = "≈ Same"
        elif speedup >= 0.95:
            status = "- Slower"
        else:
            status = "✗ Much slower"
        
        print(f"{config:<25} {b_str:<15} {n_str:<15} {speedup:.4f}×      {status}")
    
    print()
    print('=' * 80)
    print('SUMMARY')
    print('=' * 80)
    
    avg_speedup = sum(speedups) / len(speedups)
    improvement = (avg_speedup - 1) * 100
    
    print(f"Average speedup: {avg_speedup:.4f}× ({improvement:+.2f}%)")
    print()
    
    # Find best and worst
    best_idx = speedups.index(max(speedups))
    worst_idx = speedups.index(min(speedups))
    
    print(f"Best improvement:  {configs[best_idx]:<25} {speedups[best_idx]:.4f}×")
    print(f"Worst performance: {configs[worst_idx]:<25} {speedups[worst_idx]:.4f}×")
    print()
    
    # Time saved on largest config
    largest = '4096x4096_40steps'
    if largest in baseline['results'] and largest in new['results']:
        b_large = baseline['results'][largest]['average']['total_time']
        n_large = new['results'][largest]['average']['total_time']
        saved = b_large - n_large
        print(f"Time saved on {largest}:")
        print(f"  {format_time(b_large)} → {format_time(n_large)}")
        print(f"  Saved: {format_time(saved)} ({(b_large/n_large-1)*100:.1f}% faster)")


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <baseline.json> <new.json>")
        sys.exit(1)
    
    baseline_file = Path(sys.argv[1])
    new_file = Path(sys.argv[2])
    
    if not baseline_file.exists():
        print(f"Error: {baseline_file} not found")
        sys.exit(1)
    if not new_file.exists():
        print(f"Error: {new_file} not found")
        sys.exit(1)
    
    compare_benchmarks(baseline_file, new_file)
