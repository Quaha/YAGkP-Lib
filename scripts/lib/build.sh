# shellcheck shell=bash
# =============================================================================
# Общая логика скриптов сборки YAGkP (scripts/build/*.sh).
#
# Каталог сборки: build/<режим>-<вариант>, например build/release-seq.
# Флаги берутся из flags.sh, вариант par включает YAGKP_PARALLEL.
#
#   build_yagkp <режим> [-D...]   # конфигурация и сборка
#   echo "$BUILD_DIR"             # где лежит результат
# =============================================================================

# shellcheck source=flags.sh
source "$YAGKP_ROOT/scripts/lib/flags.sh"

# build_yagkp <режим> [дополнительные аргументы cmake...]
# Заполняет BUILD_DIR
build_yagkp() {
	local mode="$1"
	shift

	require_cmd cmake

	BUILD_DIR="$YAGKP_ROOT/build/$mode-$VARIANT"

	local parallel=OFF
	if [ "$VARIANT" = "par" ]; then
		parallel=ON
	fi

	load_flags "$mode"

	step "YAGkP: $mode ($VARIANT) -> ${BUILD_DIR#"$YAGKP_ROOT"/}"
	print_flags

	if [ "$CLEAN" -eq 1 ]; then
		info "Удаляю ${BUILD_DIR#"$YAGKP_ROOT"/}"
		rm -rf "$BUILD_DIR"
	fi

	cmake -S "$YAGKP_ROOT" -B "$BUILD_DIR" \
		"${CMAKE_FLAG_ARGS[@]}" \
		-DYAGKP_PARALLEL="$parallel" \
		"$@"
	cmake --build "$BUILD_DIR" --parallel "$JOBS"
}

# print_built <путь относительно BUILD_DIR>...: перечислить собранные файлы
print_built() {
	local rel
	ok "${BUILD_DIR#"$YAGKP_ROOT"/}"
	for rel in "$@"; do
		if [ -e "$BUILD_DIR/$rel" ]; then
			info "${BUILD_DIR#"$YAGKP_ROOT"/}/$rel"
		fi
	done
}