#!/usr/bin/env bash
# =============================================================================
# KaHIP: только последовательный kaffpa (статическая библиотека kahip_static)
# =============================================================================
set -euo pipefail
# shellcheck source=../lib/common.sh
source "$(dirname "${BASH_SOURCE[0]}")/../lib/common.sh"
# shellcheck source=../lib/deps.sh
source "$YAGKP_ROOT/scripts/lib/deps.sh"

usage() {
	deps_usage kahip "Собирает KaHIP (kaffpa) из external/KaHIP."
}

parse_common_args "$@"
deps_begin kahip external/KaHIP

#   NONATIVEOPTIMIZATIONS=ON  KaHIP добавляет -march=native так, что наши флаги
#                             его не перекрывают; отключаем, -march задаёт flags.sh
#   NOMPI=ON, PARHIP=OFF      MPI нужен только ParHIP и kaffpaE, мы их не используем
#   CMAKE_DISABLE_FIND_PACKAGE_OpenMP=ON
#                             иначе KaHIP подключит OpenMP, если найдёт: для
#                             последовательного варианта он не нужен
#   LIB_METIS=OFF             иначе KaHIP подключит METIS, если найдёт его в системе
deps_configure \
	-DNONATIVEOPTIMIZATIONS=ON \
	-DNOMPI=ON \
	-DPARHIP=OFF \
	-DCMAKE_DISABLE_FIND_PACKAGE_OpenMP=ON \
	-DLIB_METIS=OFF
# Только библиотека: утилиты KaHIP не нужны, а собираются долго
cmake --build "$DEP_BUILD" --target kahip_static --parallel "$JOBS"

cp "$DEP_BUILD/libkahip_static.a" "$DEP_PREFIX/lib/"
cp "$DEP_SRC/interface/kaHIP_interface.h" "$DEP_PREFIX/include/"

deps_finish