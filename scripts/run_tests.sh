#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if [ ! -f external/gtest/CMakeLists.txt ]; then
    echo "googletest submodule не инициализирован, делаю git submodule update..."
    git submodule update --init --recursive external/gtest
fi

if [ ! -d deps ]; then
    echo "deps/ не найден, запускаю build_linux.sh..."
    bash build_linux.sh
fi

BUILD_DIR="build_tests"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON

cmake --build . --target YAGkP_tests --parallel

echo
echo "=== Running tests ==="
ctest --output-on-failure