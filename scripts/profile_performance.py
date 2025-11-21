#!/usr/bin/env python3
"""
Automated profiling script using multiple tools.
Requires: gperftools (brew install gperftools), hyperfine (brew install hyperfine)
Optional: py-spy (pip install py-spy)
"""

import subprocess
import os
import sys
import json
from datetime import datetime
from pathlib import Path

def run_command(cmd, description, check=False):
    """Run a command and print output."""
    print(f"\n{'='*70}")
    print(f"{description}")
    print(f"{'='*70}")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    print(result.stdout)
    if result.returncode != 0 and check:
        print(f"Error: {result.stderr}")
        return False
    return True

def profile_cpp():
    """Profile C++ library using macOS sample tool."""
    print("\n🔬 PROFILING C++ LIBRARY")
    
    # Build with profiling
    print("\n1. Building with profiling symbols...")
    os.chdir('build')
    success = run_command(
        'cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && make -j8',
        '1. Building with profiling symbols',
        check=True
    )
    
    if not success:
        print("❌ Build failed. Make sure you have CMake and build tools installed.")
        os.chdir('..')
        return False
    
    # Run with macOS sample profiler (no dependencies needed!)
    print("\n2. Running profiler (this will take about 10 seconds)...")
    print("   Using macOS built-in 'sample' profiler...")
    
    # Run the test in background and profile it
    result = subprocess.run(
        'timeout 10 ./test/PlateTectonicsTests &> /dev/null & PID=$!; sleep 1; sample $PID 5 -file ../profile_sample.txt; wait',
        shell=True,
        capture_output=True,
        text=True
    )
    
    os.chdir('..')
    
    # Analyze sample output
    if os.path.exists('profile_sample.txt'):
        print("\n3. Analyzing profile...")
        
        # Parse sample output to extract function statistics
        functions = {}
        current_call_stack = []
        
        with open('profile_sample.txt', 'r') as f:
            content = f.read()
            
            # Extract samples
            import re
            samples = re.findall(r'(\d+)\s+(.+?)(?:\s+\(in\s+.+?\))?$', content, re.MULTILINE)
            
            for count, func in samples:
                func = func.strip()
                if func and not func.startswith('['):
                    functions[func] = functions.get(func, 0) + int(count)
        
        # Write summary
        if functions:
            sorted_funcs = sorted(functions.items(), key=lambda x: x[1], reverse=True)
            total = sum(count for _, count in sorted_funcs)
            
            with open('profile_cpp.txt', 'w') as out:
                out.write("PROFILE ANALYSIS (Sample-based)\n")
                out.write("=" * 70 + "\n\n")
                out.write(f"Total samples: {total}\n\n")
                out.write(f"{'Samples':<10} {'%':<8} {'Cumulative %':<15} {'Function'}\n")
                out.write("-" * 70 + "\n")
                
                cumulative = 0
                for i, (func, count) in enumerate(sorted_funcs[:30]):
                    pct = (count / total * 100) if total > 0 else 0
                    cumulative += pct
                    out.write(f"{count:<10} {pct:<8.2f} {cumulative:<15.2f} {func}\n")
            
            print("\n📊 TOP 15 FUNCTIONS BY SAMPLES:")
            print(f"{'Samples':<10} {'%':<8} {'Cumulative %':<15} {'Function'}")
            print("-" * 70)
            
            cumulative = 0
            for i, (func, count) in enumerate(sorted_funcs[:15]):
                pct = (count / total * 100) if total > 0 else 0
                cumulative += pct
                print(f"{count:<10} {pct:<8.2f} {cumulative:<15.2f} {func[:45]}")
            
            return True
        else:
            print("⚠️  Could not parse profiling data. Raw output saved to profile_sample.txt")
            return True
    else:
        print("❌ No profile data generated.")
        return False

def profile_python():
    """Profile Python bindings with py-spy."""
    print("\n🐍 PROFILING PYTHON BINDINGS")
    
    # Check if py-spy is installed
    result = subprocess.run(['which', 'py-spy'], capture_output=True)
    if result.returncode != 0:
        print("⚠️  py-spy not installed. Install with: pip install py-spy")
        return False
    
    # Profile with py-spy
    print("\nThis will take about 30 seconds and requires sudo...")
    run_command(
        'sudo py-spy record -o profile_python.svg --duration 30 -- python scripts/benchmark_performance.py --runs 1',
        'Recording Python profile (requires sudo)'
    )
    
    if os.path.exists('profile_python.svg'):
        print("✅ Python flamegraph saved to profile_python.svg")
        print("   Open with: open profile_python.svg")
        return True
    return False

def benchmark_quick():
    """Quick benchmark comparison."""
    print("\n⚡ QUICK BENCHMARK")
    
    # Check if hyperfine is installed
    result = subprocess.run(['which', 'hyperfine'], capture_output=True)
    if result.returncode != 0:
        print("⚠️  hyperfine not installed. Install with: brew install hyperfine")
        return False
    
    run_command(
        'hyperfine --warmup 1 --runs 3 "python scripts/benchmark_performance.py --runs 1"',
        'Running quick benchmark with hyperfine'
    )
    return True

def instrument_code():
    """Show manual timing instrumentation example."""
    print("\n⏱️  MANUAL INSTRUMENTATION EXAMPLE")
    
    instrumentation_code = '''
// Add this Timer class to your C++ code for quick profiling:

#include <chrono>
#include <iostream>

class Timer {
    std::chrono::high_resolution_clock::time_point start;
    const char* name;
public:
    Timer(const char* n) : name(n) {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << name << ": " << duration.count() / 1000.0 << " ms\\n";
    }
};

// Usage in functions:
void plate::collide(...) {
    Timer timer("plate::collide");
    // ... function code ...
    // Timer will print elapsed time when it goes out of scope
}

void plate::erode(float lower_bound) {
    Timer timer("plate::erode");
    // ... function code ...
}

// Or time specific sections:
void someFunction() {
    {
        Timer timer("Section 1");
        // ... code section 1 ...
    }
    {
        Timer timer("Section 2");
        // ... code section 2 ...
    }
}
'''
    print(instrumentation_code)

def main():
    """Run all profiling tools."""
    print("=" * 70)
    print("PLATE TECTONICS PERFORMANCE PROFILING")
    print("=" * 70)
    print(f"Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    
    # Change to project root
    script_dir = Path(__file__).parent
    os.chdir(script_dir.parent)
    
    # Check for optional tools (all are optional now, using built-in macOS tools)
    tools = {
        'hyperfine': ('brew install hyperfine', False),
        'py-spy': ('pip install py-spy', False)
    }
    
    missing = []
    for tool, (install_cmd, required) in tools.items():
        result = subprocess.run(['which', tool], capture_output=True)
        if result.returncode != 0:
            missing.append(f"  ⚠️  OPTIONAL: {tool}: {install_cmd}")
    
    if missing:
        print("\n🔧 Optional tools (not required):")
        print('\n'.join(missing))
        print("\n✅ Using built-in macOS 'sample' tool for profiling\n")
    
    # Run profiling
    cpp_success = profile_cpp()
    
    if not cpp_success:
        print("\n❌ C++ profiling failed. Check the errors above.")
        return 1
    
    # Quick benchmark (optional)
    if subprocess.run(['which', 'hyperfine'], capture_output=True).returncode == 0:
        benchmark_quick()
    
    # Optional: Python profiling (requires sudo)
    if subprocess.run(['which', 'py-spy'], capture_output=True).returncode == 0:
        response = input("\n🐍 Profile Python bindings? (requires sudo) [y/N]: ")
        if response.lower() == 'y':
            profile_python()
    
    # Show instrumentation example
    response = input("\n⏱️  Show manual instrumentation code? [y/N]: ")
    if response.lower() == 'y':
        instrument_code()
    
    print("\n" + "=" * 70)
    print("PROFILING COMPLETE")
    print("=" * 70)
    print("\n📁 Output files:")
    if os.path.exists('profile_cpp.txt'):
        print("  ✅ profile_cpp.txt  - Text report with top functions")
    if os.path.exists('profile_sample.txt'):
        print("  ✅ profile_sample.txt - Raw sample data")
    if os.path.exists('profile_python.svg'):
        print("  ✅ profile_python.svg - Python flamegraph")
    
    print("\n📊 Next steps:")
    print("  1. Review profile_cpp.txt for hot functions")
    print("  2. Focus optimization on top 2-3 functions (>10% of samples)")
    print("  3. For more detailed profiling, use Xcode Instruments:")
    print("     instruments -t 'Time Profiler' -D profile.trace build/test/PlateTectonicsTests")
    print("  4. Read docs/profiling-guide.md for detailed help")
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
