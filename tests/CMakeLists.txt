set(GTEST_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/gtest)

if(NOT EXISTS ${GTEST_SOURCE_DIR}/CMakeLists.txt)
    message(FATAL_ERROR
        "googletest не найден по пути ${GTEST_SOURCE_DIR}.\n"
        "Запустите: git submodule update --init --recursive")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)

add_subdirectory(${GTEST_SOURCE_DIR} ${CMAKE_BINARY_DIR}/gtest_build EXCLUDE_FROM_ALL)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp)

add_executable(YAGkP_tests ${TEST_SOURCES})

target_link_libraries(YAGkP_tests
    PRIVATE
        YAGkP_library
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(YAGkP_tests
    PROPERTIES
        LABELS "unit"
    DISCOVERY_TIMEOUT 30
)