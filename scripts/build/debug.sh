#!/usr/bin/env bash
# =============================================================================
# Debug: только YAGkP, без оптимизаций, с отладочной информацией
# =============================================================================
set -euo pipefail
# shellcheck source=../lib/common.sh
source "$(dirname "${BASH_SOURCE[0]}")/../lib/common.sh"
# shellcheck source=../lib/build.sh
source "$YAGKP_ROOT/scripts/lib/build.sh"

usage() {
	cat <<'USAGE'
Использование: bash scripts/build/debug.sh [опции]

Отладочная сборка YAGkP: ядро и приложения.
Результат: build/debug-<вариант>/

USAGE
	common_args_help
}

parse_common_args "$@"

build_yagkp debug

print_built apps/partition/yagkp_partition