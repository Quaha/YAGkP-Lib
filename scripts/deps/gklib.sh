#!/usr/bin/env bash
# =============================================================================
# GKlib: вспомогательная библиотека METIS
# =============================================================================
set -euo pipefail
# shellcheck source=../lib/common.sh
source "$(dirname "${BASH_SOURCE[0]}")/../lib/common.sh"
# shellcheck source=../lib/deps.sh
source "$YAGKP_ROOT/scripts/lib/deps.sh"

usage() {
	deps_usage gklib "Собирает GKlib (нужна для METIS) из external/GKlib."
}

parse_common_args "$@"
deps_begin gklib external/GKlib

#   GKLIB_BUILD_APPS=OFF       только библиотека, без утилит
#   OPENMP=OFF                 последовательная версия
#   CMAKE_INSTALL_LIBDIR=lib   иначе на части систем библиотека уйдёт в lib64
cmake -S "$DEP_SRC" -B "$DEP_BUILD" \
	"${CMAKE_FLAG_ARGS[@]}" \
	-DCMAKE_INSTALL_PREFIX="$DEP_PREFIX" \
	-DCMAKE_INSTALL_LIBDIR=lib \
	-DGKLIB_BUILD_APPS=OFF \
	-DOPENMP=OFF \
	-DSHARED=OFF
cmake --build "$DEP_BUILD" --parallel "$JOBS"
cmake --install "$DEP_BUILD"

deps_finish