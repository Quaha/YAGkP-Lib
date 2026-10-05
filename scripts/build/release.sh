#!/usr/bin/env bash
# =============================================================================
# Release: только YAGkP, оптимизированная сборка
# =============================================================================
set -euo pipefail
# shellcheck source=../lib/common.sh
source "$(dirname "${BASH_SOURCE[0]}")/../lib/common.sh"
# shellcheck source=../lib/build.sh
source "$YAGKP_ROOT/scripts/lib/build.sh"

usage() {
	cat <<'USAGE'
Использование: bash scripts/build/release.sh [опции]

Оптимизированная сборка YAGkP: ядро и приложения, без сторонних библиотек.
Результат: build/release-<вариант>/

USAGE
	common_args_help
}

parse_common_args "$@"

build_yagkp release

print_built apps/partition/yagkp_partition