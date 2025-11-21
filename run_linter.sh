#!/bin/bash
# C++ Linter script for plate-tectonics project
# This script runs both clang-tidy and cppcheck on the codebase

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Set LLVM path (homebrew installation)
CLANG_TIDY="/opt/homebrew/opt/llvm/bin/clang-tidy"

echo -e "${GREEN}=== C++ Linting for plate-tectonics ===${NC}"
echo ""

# Check if compile_commands.json exists in build directory
BUILD_DIR="build"
if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    echo -e "${YELLOW}Warning: compile_commands.json not found in $BUILD_DIR/${NC}"
    echo -e "${YELLOW}Creating build directory and running cmake...${NC}"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cd ..
    echo ""
fi

# Create symlink to compile_commands.json in root for clang-tidy
if [ ! -L "compile_commands.json" ]; then
    ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json
fi

# Function to run clang-tidy
run_clang_tidy() {
    echo -e "${GREEN}Running clang-tidy...${NC}"
    
    # Find all C++ source files
    CPP_FILES=$(find src examples -name "*.cpp" -o -name "*.hpp" 2>/dev/null | grep -v "\.orig$")
    
    if [ -z "$CPP_FILES" ]; then
        echo -e "${RED}No C++ files found!${NC}"
        return 1
    fi
    
    # Run clang-tidy on each file
    for file in $CPP_FILES; do
        echo "  Checking: $file"
        $CLANG_TIDY "$file" -p . --quiet --checks='-misc-include-cleaner' 2>&1 || true
    done
    
    echo -e "${GREEN}clang-tidy complete!${NC}"
    echo ""
}

# Function to run cppcheck
run_cppcheck() {
    echo -e "${GREEN}Running cppcheck...${NC}"
    
    cppcheck --enable=all \
             --suppress=missingIncludeSystem \
             --suppress=unusedFunction \
             --suppress=unmatchedSuppression \
             --inline-suppr \
             --std=c++17 \
             --platform=native \
             --verbose \
             --quiet \
             -I src/ \
             src/ examples/ test/ 2>&1 | grep -v "Checking " | grep -v "^$" || true
    
    echo -e "${GREEN}cppcheck complete!${NC}"
    echo ""
}

# Main execution
case "${1:-all}" in
    clang-tidy)
        run_clang_tidy
        ;;
    cppcheck)
        run_cppcheck
        ;;
    all)
        run_clang_tidy
        run_cppcheck
        ;;
    *)
        echo "Usage: $0 [clang-tidy|cppcheck|all]"
        echo "  clang-tidy: Run only clang-tidy"
        echo "  cppcheck:   Run only cppcheck"
        echo "  all:        Run both (default)"
        exit 1
        ;;
esac

echo -e "${GREEN}=== Linting complete! ===${NC}"
