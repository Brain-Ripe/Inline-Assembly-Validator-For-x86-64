#!/bin/bash
# CD_LAB_EL/build.sh

BUILD_DIR="build"

echo "--- Starting Build Process ---"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir $BUILD_DIR
fi

cd $BUILD_DIR
cmake ..
make

if [ $? -eq 0 ]; then
    echo "--- Build Success: ./build/AsmValidator.so is ready ---"
else
    echo "--- Build Failed ---"
    exit 1
fi
