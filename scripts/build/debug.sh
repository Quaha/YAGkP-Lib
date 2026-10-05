#!/usr/bin/env bash
# =============================================================================
# Debug: только YAGkP, без оптимизаций, с отладочной информацией и тестами
# =============================================================================
set -euo pipefail
# shellcheck source=../lib/common.sh
source "$(dirname "${BASH_SOURCE[0]}")/../lib/common.sh"
# shellcheck source=../lib/build.sh
source "$YAGKP_ROOT/scripts/lib/build.sh"

RUN_TESTS=1

usage() {
	cat <<'USAGE'
Использование: bash scripts/build/debug.sh [опции]

Отладочная сборка YAGkP: ядро, приложения и модульные тесты.
После сборки тесты запускаются. Результат: build/debug-<вариант>/

Опции режима:
  --no-tests     не запускать тесты после сборки

USAGE
	common_args_help
}

parse_extra_arg() {
	case "$1" in
		--no-tests) RUN_TESTS=0 ;;
		*) return 1 ;;
	esac
}

parse_common_args "$@"
init_submodule external/gtest

build_yagkp debug -DYAGKP_BUILD_TESTS=ON

if [ "$RUN_TESTS" -eq 1 ]; then
	step "Тесты"
	ctest --test-dir "$BUILD_DIR" --output-on-failure
fi

print_built apps/partition/yagkp_partition tests/yagkp_tests