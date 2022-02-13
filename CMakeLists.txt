cmake_minimum_required(VERSION 3.16)

project(
        dbbench
        DESCRIPTION "A helper library for benchmarking database systems"
        VERSION 0.1.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

option(DBBENCH_BUILD_EXAMPLES "Build examples" ON)

add_library(
        dbbench_core
        src/generator.cpp
        include/dbbench/generator.hpp
        include/dbbench/runner.hpp
)
target_include_directories(
        dbbench_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/dbbench>
)

add_library(
        dbbench_tatp
        src/benchmarks/tatp.cpp
        include/dbbench/benchmarks/tatp.hpp
)
target_include_directories(
        dbbench_tatp
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/dbbench>
)
target_link_libraries(dbbench_tatp dbbench_core)

if (DBBENCH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
