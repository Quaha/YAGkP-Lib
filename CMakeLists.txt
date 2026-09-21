cmake_minimum_required(VERSION 3.21)

project(YAGkP
    VERSION     0.1.0
    DESCRIPTION "Yet Another Graph k-Partition Library"
    LANGUAGES   C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # compile_commands.json для clangd / IDE

# =============================================================================
# Опции
#
#   yagkp            ядро библиотеки, без внешних зависимостей
#   yagkp_benchmark  обёртки METIS / KaHIP / SCOTCH (нужен scripts/build_deps.sh)
#   yagkp_app        CLI для замеров, собирается вместе с yagkp_benchmark
#   yagkp_tests      модульные тесты (подмодуль external/gtest)
# =============================================================================
option(YAGKP_BUILD_BENCHMARK "Собирать слой сравнения со сторонними библиотеками" OFF)
option(YAGKP_BUILD_TESTS     "Собирать модульные тесты"                           OFF)
option(YAGKP_NATIVE_FLAGS    "Оптимизация под текущий процессор (-march=native)"  OFF)

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
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external     # для "mmio/matrix.hpp"
)

target_compile_options(yagkp PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
)

# Сторонний код: его предупреждения не наши, глушим
set_source_files_properties(external/mmio/mmio.cpp PROPERTIES COMPILE_OPTIONS "-w")

# -march=native делает бинарь непереносимым между узлами кластера,
# поэтому только по явному запросу
if(YAGKP_NATIVE_FLAGS)
    target_compile_options(yagkp PUBLIC -march=native -funroll-loops)
endif()

# OpenMP пока не используется, но понадобится для параллельной версии
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(yagkp PUBLIC OpenMP::OpenMP_CXX)
endif()

# =============================================================================
# Слой замеров и CLI
# =============================================================================
if(YAGKP_BUILD_BENCHMARK)
    add_subdirectory(src/benchmark)

    add_executable(yagkp_app app/main.cpp)
    target_link_libraries(yagkp_app PRIVATE yagkp_benchmark)
endif()

# =============================================================================
# Тесты
# =============================================================================
if(YAGKP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
