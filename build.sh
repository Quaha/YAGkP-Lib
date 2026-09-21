#!/usr/bin/env bash
# =============================================================================
# Сборка YAGkP. Справка: bash build.sh --help
# =============================================================================
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

print_help() {
	cat <<'EOF'
Использование: bash build.sh [пресет] [опции]

Пресеты (не больше одного, по умолчанию --debug):
  -d, --debug       ядро + тесты, отладочная сборка
  -r, --release     ядро, оптимизированная сборка (-O3 -march=native)
  -b, --benchmark   ядро + METIS / KaHIP / SCOTCH + CLI для замеров
  -p, --profile     как --benchmark, но для gprof (-O2 -g -pg)

Опции:
  -t, --test        после сборки прогнать тесты
  -c, --clean       удалить build/<пресет> перед сборкой
      --deps        пересобрать сторонние библиотеки (только --benchmark / --profile)
  -j N              число потоков сборки (по умолчанию все ядра)
  -h, --help        показать эту справку и выйти, ничего не собирая

Примеры:
  bash build.sh                  # debug
  bash build.sh -d -t            # debug + тесты
  bash build.sh -b
  bash build.sh -r -c -j 8

Результат: build/<пресет>/
EOF
}

# -----------------------------------------------------------------------------
# Разбор аргументов. Сначала собираем всё, решения принимаем после:
# так --help срабатывает, даже если рядом стоят неверные аргументы.
# -----------------------------------------------------------------------------
presets=()
errors=()
show_help=0
run_tests=0
clean=0
rebuild_deps=0
jobs=""

while [ $# -gt 0 ]; do
	case "$1" in
		-h | --help)      show_help=1 ;;
		-d | --debug)     presets+=("debug") ;;
		-r | --release)   presets+=("release") ;;
		-b | --benchmark) presets+=("benchmark") ;;
		-p | --profile)   presets+=("profile") ;;
		-t | --test)      run_tests=1 ;;
		-c | --clean)     clean=1 ;;
		--deps)           rebuild_deps=1 ;;
		-j)
			if [ $# -lt 2 ] || ! [[ "$2" =~ ^[1-9][0-9]*$ ]]; then
				errors+=("-j: ожидается положительное число")
				if [ $# -ge 2 ]; then shift; fi
			else
				jobs="$2"
				shift
			fi
			;;
		*) errors+=("неизвестный аргумент: $1") ;;
	esac
	shift
done

if [ "$show_help" -eq 1 ]; then
	print_help
	exit 0
fi

echo "YAGkP build. Справка: bash build.sh --help"
echo

# Один и тот же пресет дважды (-d --debug) не ошибка, разные — ошибка.
# Конструкция ${arr[@]+...} нужна для старого bash (4.2 на CentOS 7):
# там пустой массив при set -u считается неопределённой переменной.
unique_presets=()
if [ "${#presets[@]}" -gt 0 ]; then
	unique_presets=($(printf '%s\n' "${presets[@]}" | sort -u))
fi
if [ "${#unique_presets[@]}" -gt 1 ]; then
	errors+=("указано несколько пресетов: ${unique_presets[*]}. Выберите один")
fi

if [ "${#errors[@]}" -gt 0 ]; then
	for e in "${errors[@]}"; do
		echo "Ошибка: $e" >&2
	done
	exit 1
fi

if [ "${#unique_presets[@]}" -eq 0 ]; then
	preset="debug"
	echo "Пресет не указан, собираю debug"
else
	preset="${unique_presets[0]}"
fi

needs_deps=0
if [ "$preset" = "benchmark" ] || [ "$preset" = "profile" ]; then
	needs_deps=1
fi

if [ "$rebuild_deps" -eq 1 ] && [ "$needs_deps" -eq 0 ]; then
	echo "Предупреждение: --deps нужен только для --benchmark / --profile, игнорирую"
fi

command -v cmake >/dev/null || { echo "Ошибка: cmake не найден" >&2; exit 1; }

build_dir="build/$preset"
cmake_args=()

# -----------------------------------------------------------------------------
# 1. Очистка
# -----------------------------------------------------------------------------
if [ "$clean" -eq 1 ]; then
	echo ">>> Удаляю $build_dir"
	rm -rf "$build_dir"
fi

# -----------------------------------------------------------------------------
# 2. Сторонние библиотеки
# -----------------------------------------------------------------------------
if [ "$needs_deps" -eq 1 ]; then
	if [ "$rebuild_deps" -eq 1 ]; then
		echo ">>> Удаляю deps/ для пересборки"
		rm -rf deps
	fi
	if [ ! -f deps/metis/lib/libmetis.a ] || [ ! -f deps/scotch/lib/libscotch.a ]; then
		echo ">>> Собираю сторонние библиотеки (один раз, несколько минут)"
		JOBS="${jobs:-$(nproc)}" bash scripts/build_deps.sh
	fi
fi

# -----------------------------------------------------------------------------
# 3. Тесты: в debug включены всегда, в остальных пресетах — по флагу
# -----------------------------------------------------------------------------
if [ "$run_tests" -eq 1 ] || [ "$preset" = "debug" ]; then
	if [ ! -f external/gtest/CMakeLists.txt ]; then
		echo ">>> Инициализирую подмодуль googletest"
		git submodule update --init external/gtest
	fi
	cmake_args+=("-DYAGKP_BUILD_TESTS=ON")
fi

# -----------------------------------------------------------------------------
# 4. Конфигурация и сборка
# -----------------------------------------------------------------------------
echo ">>> Конфигурация: $preset"
cmake --preset "$preset" ${cmake_args[@]+"${cmake_args[@]}"}

echo ">>> Сборка: $preset"
cmake --build --preset "$preset" --parallel "${jobs:-$(nproc)}"

# -----------------------------------------------------------------------------
# 5. Тесты
# -----------------------------------------------------------------------------
if [ "$run_tests" -eq 1 ]; then
	echo ">>> Тесты"
	ctest --test-dir "$build_dir" --output-on-failure
fi

echo
echo "Готово: $build_dir/"