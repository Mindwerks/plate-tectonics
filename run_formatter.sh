#!/bin/bash

# Format all C++ source and header files in the project
echo "=== Formatting C++ code with clang-format ==="

# Find all .cpp, .hpp, .h, .cc files in src/ and include/ directories, excluding build/
find src -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" \) -exec echo "Formatting: {}" \; -exec clang-format -i {} \;

echo "Formatting complete!"
