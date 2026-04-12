#!/bin/bash
set -e

BUILD_DIR="build"
DEPS_DIR="$(pwd)/deps"

# =============================================================================
# GKlib — helper library required by METIS
# Skipped if already built
# =============================================================================
if [ ! -f "$DEPS_DIR/gklib/lib/libGKlib.a" ] && [ ! -f "$DEPS_DIR/gklib/lib64/libGKlib.a" ]; then
    echo "Building GKlib..."
    cd external/GKlib
    make config prefix="$DEPS_DIR/gklib"
    make -j$(nproc)
    make install
    cd ../..
fi

# On some systems GKlib installs into lib64 instead of lib — create a symlink
if [ -d "$DEPS_DIR/gklib/lib64" ] && [ ! -d "$DEPS_DIR/gklib/lib" ]; then
    ln -sf lib64 "$DEPS_DIR/gklib/lib"
fi

# =============================================================================
# METIS — graph partitioning library
# Uses manual copy instead of "make install" to avoid install failures on
# some systems (e.g. cluster). Skipped if already built.
# =============================================================================
if [ ! -f "$DEPS_DIR/metis/lib/libmetis.a" ]; then
    echo "Building METIS..."
    cd external/METIS
    rm -rf build
    make config prefix="$DEPS_DIR/metis" gklib_path="$(pwd)/../../external/GKlib"
    make -j$(nproc) -C build/libmetis
    mkdir -p "$DEPS_DIR/metis/lib" "$DEPS_DIR/metis/include"
    cp build/libmetis/libmetis.a "$DEPS_DIR/metis/lib/"
    cp build/xinclude/metis.h    "$DEPS_DIR/metis/include/"
    cd ../..
fi

# =============================================================================
# SCOTCH — graph partitioning library
# Parallel (MPI-based) PT-SCOTCH is disabled to avoid MPI dependency.
# Skipped if already built.
# =============================================================================
if [ ! -f "$DEPS_DIR/scotch/lib/libscotch.a" ] && [ ! -f "$DEPS_DIR/scotch/lib64/libscotch.a" ]; then
    echo "Building SCOTCH..."
    mkdir -p external/scotch/build
    cd external/scotch/build
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=/usr/bin/cc \
        -DCMAKE_INSTALL_PREFIX="$DEPS_DIR/scotch" \
        -DBUILD_SHARED_LIBS=OFF \
        -DSCOTCH_PTHREAD=OFF \
        -DBUILD_PTSCOTCH=OFF
    make -j$(nproc)
    make install
    cd ../../..
fi

# On some systems SCOTCH installs into lib64 instead of lib — create a symlink
if [ -d "$DEPS_DIR/scotch/lib64" ] && [ ! -d "$DEPS_DIR/scotch/lib" ]; then
    ln -sf lib64 "$DEPS_DIR/scotch/lib"
fi

# =============================================================================
# Main project
# KaHIP is built automatically by CMake as an ExternalProject
# =============================================================================
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
cd ..

echo "Done!"