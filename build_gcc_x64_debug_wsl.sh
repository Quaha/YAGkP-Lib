#!/bin/bash
BUILD_DIR="build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
cd ..

echo "Done!"