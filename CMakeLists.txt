cmake_minimum_required(VERSION 3.10)

set(VERSION "0.0.1")
set(PROJECT_NAME "cxx_opt")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES CXX)

add_library(
    ${PROJECT_NAME}
    cxx_opt.cpp
)
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    .
)
target_compile_features(
    ${PROJECT_NAME} PRIVATE
    cxx_std_11
)
target_compile_options(
    ${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
)
add_library(
    ${PROJECT_NAME}::${PROJECT_NAME} ALIAS
    ${PROJECT_NAME}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CTest)
enable_testing()
add_subdirectory(test)

