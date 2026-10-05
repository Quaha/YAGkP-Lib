#!/usr/bin/env bash
# =============================================================================
# Benchmark: YAGkP и сторонние библиотеки с одинаковыми флагами
# =============================================================================
set -euo pipefail
# shellcheck source=../lib/common.sh
source "$(dirname "${BASH_SOURCE[0]}")/../lib/common.sh"
# shellcheck source=../lib/build.sh
source "$YAGKP_ROOT/scripts/lib/build.sh"

usage() {
	cat <<'USAGE'
Использование: bash scripts/build/benchmark.sh [опции]

Сборка для сравнения: YAGkP и METIS / KaHIP / SCOTCH с одними и теми же
флагами (режим benchmark из scripts/lib/flags.sh). Сторонние библиотеки
собираются при первом запуске и при изменении флагов или компилятора.
Результат: build/benchmark-<вариант>/

USAGE
	common_args_help
	echo "  (--clean пересобирает только YAGkP; библиотеки: scripts/deps/all.sh --clean)"
}

parse_common_args "$@"

bash "$YAGKP_ROOT/scripts/deps/all.sh" "--$VARIANT" -j "$JOBS"

build_yagkp benchmark \
	-DYAGKP_BUILD_BENCHMARK=ON \
	-DYAGKP_DEPS_DIR="$YAGKP_ROOT/deps/$VARIANT"

print_built apps/partition/yagkp_partition apps/bench/yagkp_bench