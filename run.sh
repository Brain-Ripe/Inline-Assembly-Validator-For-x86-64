#!/bin/bash
# CD_LAB_EL/run.sh

if [ -z "$1" ]; then
    echo "Usage: ./run.sh <test_file>"
    echo "Example: ./run.sh testcases/test_all.c"
    exit 1
fi

# Path to the compiled plugin
PLUGIN="./build/AsmValidator.so"

if [ ! -f "$PLUGIN" ]; then
    echo "Error: Plugin not found. Run ./build.sh first."
    exit 1
fi

# Run the validator
clang -cc1 -load "$PLUGIN" -plugin asm-validator "$1"
