#!/usr/bin/env bash
# =============================================================================
# Сборка сторонних библиотек (GKlib, METIS, SCOTCH) в deps/.
# Нужна только пресетам benchmark и profile. KaHIP собирает сам CMake.
# Уже собранные библиотеки пропускаются; для пересборки удалите deps/.
# =============================================================================
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEPS_DIR="$ROOT_DIR/deps"
JOBS="${JOBS:-$(nproc)}"

cd "$ROOT_DIR"
git submodule update --init external/GKlib external/METIS external/scotch external/KaHIP

# -----------------------------------------------------------------------------
# GKlib — вспомогательная библиотека для METIS
# -----------------------------------------------------------------------------
if [ ! -f "$DEPS_DIR/gklib/lib/libGKlib.a" ] && [ ! -f "$DEPS_DIR/gklib/lib64/libGKlib.a" ]; then
    echo ">>> GKlib"
    (
        cd external/GKlib
        make config prefix="$DEPS_DIR/gklib"
        make -j"$JOBS"
        make install
    )
fi
# на некоторых системах GKlib ставится в lib64
if [ -d "$DEPS_DIR/gklib/lib64" ] && [ ! -e "$DEPS_DIR/gklib/lib" ]; then
    ln -s lib64 "$DEPS_DIR/gklib/lib"
fi

# -----------------------------------------------------------------------------
# METIS. Копирование вручную вместо make install: на кластере install падает
# -----------------------------------------------------------------------------
if [ ! -f "$DEPS_DIR/metis/lib/libmetis.a" ]; then
    echo ">>> METIS"
    (
        cd external/METIS
        rm -rf build
        make config prefix="$DEPS_DIR/metis" gklib_path="$ROOT_DIR/external/GKlib"
        make -j"$JOBS" -C build/libmetis
        mkdir -p "$DEPS_DIR/metis/lib" "$DEPS_DIR/metis/include"
        cp build/libmetis/libmetis.a "$DEPS_DIR/metis/lib/"
        cp build/xinclude/metis.h    "$DEPS_DIR/metis/include/"
    )
fi

# -----------------------------------------------------------------------------
# SCOTCH: только последовательная библиотека на C.
#   BUILD_PTSCOTCH=OFF        без MPI
#   BUILD_FORTRAN=OFF         Fortran-интерфейс не нужен, а компилятора может не быть
#   BUILD_LIBSCOTCHMETIS=OFF  иначе SCOTCH положит свой metis.h и он
#   INSTALL_METIS_HEADERS=OFF   будет конкурировать с настоящим из deps/metis
#   THREADS=OFF               однопоточный, как и остальные участники сравнения
#   USE_ZLIB/LZMA/BZ2=OFF     иначе libscotch потребует -lz -llzma -lbz2 при линковке
# -----------------------------------------------------------------------------
if [ ! -f "$DEPS_DIR/scotch/lib/libscotch.a" ] && [ ! -f "$DEPS_DIR/scotch/lib64/libscotch.a" ]; then
    echo ">>> SCOTCH"
    (
        mkdir -p external/scotch/build
        cd external/scotch/build
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_C_COMPILER=/usr/bin/cc \
            -DCMAKE_INSTALL_PREFIX="$DEPS_DIR/scotch" \
            -DBUILD_SHARED_LIBS=OFF \
            -DBUILD_PTSCOTCH=OFF \
            -DBUILD_FORTRAN=OFF \
            -DBUILD_LIBESMUMPS=OFF \
            -DBUILD_LIBSCOTCHMETIS=OFF \
            -DINSTALL_METIS_HEADERS=OFF \
            -DTHREADS=OFF \
            -DUSE_ZLIB=OFF \
            -DUSE_LZMA=OFF \
            -DUSE_BZ2=OFF \
            -DENABLE_TESTS=OFF
        make -j"$JOBS"
        make install
    )
fi
if [ -d "$DEPS_DIR/scotch/lib64" ] && [ ! -e "$DEPS_DIR/scotch/lib" ]; then
    ln -s lib64 "$DEPS_DIR/scotch/lib"
fi

echo ">>> Сторонние библиотеки собраны в $DEPS_DIR"
