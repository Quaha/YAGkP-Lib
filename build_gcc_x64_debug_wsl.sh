#!/bin/bash
BUILD_DIR="build"
DEPS_DIR="$(pwd)/deps"  # локальный префикс для GKlib и METIS

# Собираем GKlib
if [ ! -f "$DEPS_DIR/lib/libGKlib.a" ]; then
    echo "Building GKlib..."
    cd external/GKlib
    make config prefix="$DEPS_DIR"
    make
    make install
    cd ../..
fi

# Собираем METIS
if [ ! -f "$DEPS_DIR/lib/libmetis.a" ]; then
    echo "Building METIS..."
    cd external/METIS
    make config prefix="$DEPS_DIR" gklib_path="$DEPS_DIR"
    make
    make install
    cd ../..
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
cd ..

echo "Done!"