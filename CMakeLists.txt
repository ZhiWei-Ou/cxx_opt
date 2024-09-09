cmake_minimum_required(VERSION 3.5.0)

set(VERSION "0.0.1")
set(PROJECT_NAME "example")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES CXX)

add_executable(${PROJECT_NAME} example.cpp opt.h)
target_compile_options(${PROJECT_NAME} PRIVATE --std=c++11)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CTest)
enable_testing()
