cmake_minimum_required(VERSION 3.21)

project(YAGkP
    VERSION     0.1.0
    DESCRIPTION "Yet Another Graph k-Partition Library"
    LANGUAGES   C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # compile_commands.json для clangd / IDE

# =============================================================================
# Цели
#
#   yagkp            ядро библиотеки, без внешних зависимостей
#   yagkp_partition  CLI: разбиение только средствами YAGkP (apps/partition)
#   yagkp_bench      CLI: любой алгоритм с замером метрик (apps/bench)
#   yagkp_tests      модульные тесты (подмодуль external/gtest)
#
# Флаги компиляции здесь не задаются: их передают скрипты из scripts/
# через CMAKE_BUILD_TYPE и CMAKE_<LANG>_FLAGS_<TYPE>. Единый источник
# флагов для всех режимов и сторонних библиотек: scripts/lib/flags.sh
# =============================================================================
option(YAGKP_PARALLEL        "Параллельная версия (OpenMP)"                          OFF)
option(YAGKP_BUILD_APPS      "Собирать приложения из apps/"                          ON)
option(YAGKP_BUILD_BENCHMARK "Собирать yagkp_bench (нужны сторонние библиотеки)"     OFF)
option(YAGKP_BUILD_TESTS     "Собирать модульные тесты"                              OFF)

set(YAGKP_DEPS_DIR "${PROJECT_SOURCE_DIR}/deps" CACHE PATH
    "Каталог с собранными сторонними библиотеками (для YAGKP_BUILD_BENCHMARK)")

# -----------------------------------------------------------------------------
# Тип сборки. Profile: свой тип, его флаги приходят из scripts/lib/flags.sh
# -----------------------------------------------------------------------------
set(YAGKP_BUILD_TYPES Debug Release Profile)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Тип сборки" FORCE)
    message(STATUS "CMAKE_BUILD_TYPE не задан, использую Release")
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${YAGKP_BUILD_TYPES})

if(NOT CMAKE_BUILD_TYPE IN_LIST YAGKP_BUILD_TYPES)
    string(JOIN ", " _types ${YAGKP_BUILD_TYPES})
    message(FATAL_ERROR
        "Неизвестный CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Допустимые: ${_types}")
endif()

# =============================================================================
# Ядро
# =============================================================================
add_library(yagkp STATIC
    src/graph.cpp
    src/coarsening.cpp
    src/bipartitioner.cpp
    src/uncoarsening.cpp
    src/partitioner.cpp
    src/metrics.cpp
    external/mmio/mmio.cpp
)

target_include_directories(yagkp PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Сторонние заголовки как системные: их предупреждения не показываются
# (matrix.hpp, например, содержит #pragma omp, а включается в graph.hpp)
target_include_directories(yagkp SYSTEM PUBLIC
    ${PROJECT_SOURCE_DIR}/external   # для "mmio/matrix.hpp"
)

# Предупреждения: политика проекта, а не режима сборки, поэтому здесь
target_compile_options(yagkp PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
)

# Сторонний код: его предупреждения не наши, глушим
set_source_files_properties(external/mmio/mmio.cpp PROPERTIES COMPILE_OPTIONS "-w")

# -----------------------------------------------------------------------------
# Параллельная версия. Пока только заготовка: макрос YAGKP_PARALLEL и OpenMP
# подключаются, но код ядра их ещё не использует
# -----------------------------------------------------------------------------
if(YAGKP_PARALLEL)
    message(WARNING "YAGKP_PARALLEL=ON: параллельная версия ещё не реализована")
    find_package(OpenMP REQUIRED)
    target_link_libraries(yagkp PUBLIC OpenMP::OpenMP_CXX)
    target_compile_definitions(yagkp PUBLIC YAGKP_PARALLEL=1)
endif()

# =============================================================================
# Приложения
# =============================================================================
if(YAGKP_BUILD_APPS)
    add_subdirectory(apps)
endif()

# =============================================================================
# Тесты
# =============================================================================
if(YAGKP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# =============================================================================
# Сводка конфигурации
# =============================================================================
string(TOUPPER "${CMAKE_BUILD_TYPE}" _type)
string(STRIP "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${_type}}" _flags)
message(STATUS "")
message(STATUS "YAGkP ${PROJECT_VERSION}")
message(STATUS "  Build type   ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  CXX flags    ${_flags}")
message(STATUS "  Parallel     ${YAGKP_PARALLEL}")
message(STATUS "  Apps         ${YAGKP_BUILD_APPS}")
message(STATUS "  Benchmark    ${YAGKP_BUILD_BENCHMARK}")
message(STATUS "  Tests        ${YAGKP_BUILD_TESTS}")
message(STATUS "")