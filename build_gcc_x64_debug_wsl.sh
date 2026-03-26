#!/bin/bash
set -e

BUILD_DIR="build"
DEPS_DIR="$(pwd)/deps"

# === GKlib ===
if [ ! -f "$DEPS_DIR/gklib/lib/libGKlib.a" ]; then
    echo "Building GKlib..."
    cd external/GKlib
    make config prefix="$DEPS_DIR/gklib"
    make -j$(nproc)
    make install
    cd ../..
fi

# === METIS ===
if [ ! -f "$DEPS_DIR/metis/lib/libmetis.a" ]; then
    echo "Building METIS..."
    cd external/METIS
    make config prefix="$DEPS_DIR/metis" gklib_path="$DEPS_DIR/gklib"
    make -j$(nproc)
    make install
    cd ../..
fi

# === SCOTCH ===
if [ ! -f "$DEPS_DIR/scotch/lib/libscotch.a" ]; then
    echo "Building SCOTCH..."
    mkdir -p external/scotch/build
    cd external/scotch/build
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=/usr/bin/cc \
        -DCMAKE_INSTALL_PREFIX="$DEPS_DIR/scotch" \
        -DBUILD_SHARED_LIBS=OFF \
        -DSCOTCH_PTHREAD=OFF
    make -j$(nproc)
    make install
    cd ../../..
fi

# === Основной проект ===
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
cd ..

echo "Done!"