cmake_minimum_required(VERSION 3.10)

project(sha256 C)

add_library(sha256 STATIC sha256.c)

target_include_directories(sha256 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include(CheckIncludeFiles)
check_include_files(endian.h HAVE_ENDIAN_H)

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake_config.h.in"
    "${PROJECT_BINARY_DIR}/cmake_config.h"
)
target_include_directories(sha256 PRIVATE "${PROJECT_BINARY_DIR}")

add_executable(test_sha256 test_sha256.c)

target_link_libraries(test_sha256 PRIVATE sha256)

find_package(OpenSSL REQUIRED)
target_link_libraries(test_sha256 PRIVATE OpenSSL::Crypto)
