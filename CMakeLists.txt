cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME YAGkP)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === KaHIP ===
include(ExternalProject)

set(KAHIP_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/KaHIP)
set(KAHIP_BUILD_DIR  ${CMAKE_BINARY_DIR}/kahip_build)
set(KAHIP_LIB ${KAHIP_BUILD_DIR}/libkahip_static.a)

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

# Импортируемая цель для удобного линкования
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
        ${KAHIP_SOURCE_DIR}/interface   # kaHIP_interface.h
)

find_package(OpenMP REQUIRED)

target_link_libraries(${PROJECT_NAME}_library
    PUBLIC
        OpenMP::OpenMP_CXX
        kahip
)

target_compile_options(${PROJECT_NAME}_library PRIVATE
    -O3 -march=native -funroll-loops -ffast-math
)

# === App ===
add_executable(${PROJECT_NAME}_app apps/main.cpp)
target_link_libraries(${PROJECT_NAME}_app PRIVATE ${PROJECT_NAME}_library)