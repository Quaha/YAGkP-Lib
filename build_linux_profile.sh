#!/bin/bash
set -e

BUILD_DIR="build_profile"

# Зависимости уже собраны в deps/ через build_linux.sh
if [ ! -d "deps" ]; then
    echo "Error: deps/ not found. Run build_linux.sh first."
    exit 1
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo \
         -DCMAKE_CXX_FLAGS="-pg -g" \
         -DCMAKE_EXE_LINKER_FLAGS="-pg"
cmake --build . --parallel
cd ..

echo "Profile build done!"
echo ""
echo "Usage:"
echo "  ./build_profile/YAGkP_app --graph data/add20.mtx --k 4 --algo yagkp --output benchmark/results/"
echo "  gprof ./build_profile/YAGkP_app gmon.out | less"