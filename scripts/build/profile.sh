#!/usr/bin/env bash
# =============================================================================
# Profile: только YAGkP, оптимизации как в release плюс данные для gprof
# =============================================================================
set -euo pipefail
# shellcheck source=../lib/common.sh
source "$(dirname "${BASH_SOURCE[0]}")/../lib/common.sh"
# shellcheck source=../lib/build.sh
source "$YAGKP_ROOT/scripts/lib/build.sh"

usage() {
	cat <<'USAGE'
Использование: bash scripts/build/profile.sh [опции]

Сборка YAGkP для профилирования gprof: ядро и приложения.
Результат: build/profile-<вариант>/

USAGE
	common_args_help
}

parse_common_args "$@"

build_yagkp profile

print_built apps/partition/yagkp_partition

app="${BUILD_DIR#"$YAGKP_ROOT"/}/apps/partition/yagkp_partition"
info ""
info "Профилирование:"
info "  $app --graph data/<граф>.mtx --k 32    # создаст gmon.out в текущем каталоге"
info "  gprof $app gmon.out > profile.txt"