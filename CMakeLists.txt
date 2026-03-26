cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME YAGkP)
project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

# === Пути к зависимостям (собираются скриптом) ===
set(DEPS_DIR   ${CMAKE_SOURCE_DIR}/deps)
set(GKLIB_DIR  ${DEPS_DIR}/gklib)
set(METIS_DIR  ${DEPS_DIR}/metis)
set(SCOTCH_DIR ${DEPS_DIR}/scotch)

add_library(GKlib STATIC IMPORTED GLOBAL)
set_target_properties(GKlib PROPERTIES IMPORTED_LOCATION ${GKLIB_DIR}/lib/libGKlib.a)

add_library(metis STATIC IMPORTED GLOBAL)
set_target_properties(metis PROPERTIES IMPORTED_LOCATION ${METIS_DIR}/lib/libmetis.a)

add_library(scotch STATIC IMPORTED GLOBAL)
set_target_properties(scotch PROPERTIES IMPORTED_LOCATION ${SCOTCH_DIR}/lib/libscotch.a)

add_library(scotcherr STATIC IMPORTED GLOBAL)
set_target_properties(scotcherr PROPERTIES IMPORTED_LOCATION ${SCOTCH_DIR}/lib/libscotcherr.a)

# === KaHIP ===
set(KAHIP_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/KaHIP)
set(KAHIP_BUILD_DIR  ${CMAKE_BINARY_DIR}/kahip_build)
set(KAHIP_LIB        ${KAHIP_BUILD_DIR}/libkahip_static.a)

ExternalProject_Add(
    kahip_external
    SOURCE_DIR  ${KAHIP_SOURCE_DIR}
    BINARY_DIR  ${KAHIP_BUILD_DIR}
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    BUILD_BYPRODUCTS ${KAHIP_LIB}
    INSTALL_COMMAND ""
)

add_library(kahip STATIC IMPORTED GLOBAL)
set_target_properties(kahip PROPERTIES IMPORTED_LOCATION ${KAHIP_LIB})
add_dependencies(kahip kahip_external)

# === Твоя библиотека ===
file(GLOB_RECURSE SRC_FILES    CONFIGURE_DEPENDS src/*.cpp external/matrix/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/*.hpp external/matrix/*.hpp)

add_library(${PROJECT_NAME}_library
    ${SRC_FILES}
    ${HEADER_FILES}
)

target_include_directories(${PROJECT_NAME}_library
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external/matrix
        ${KAHIP_SOURCE_DIR}/interface
        ${METIS_DIR}/include
        ${SCOTCH_DIR}/include
)

find_package(OpenMP REQUIRED)

target_link_libraries(${PROJECT_NAME}_library
    PUBLIC
        OpenMP::OpenMP_CXX
        kahip
        metis
        GKlib
        scotch
        scotcherr
)

target_compile_options(${PROJECT_NAME}_library PRIVATE
    -O3 -march=native -funroll-loops -ffast-math
)

# === App ===
add_executable(${PROJECT_NAME}_app apps/main.cpp)
target_link_libraries(${PROJECT_NAME}_app PRIVATE ${PROJECT_NAME}_library)