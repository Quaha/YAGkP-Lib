#!/bin/bash
BUILD_DIR="build"

echo "Deleting build folder..."
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"

cd "$BUILD_DIR"
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
cd ..

echo "Done!"