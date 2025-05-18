cmake_minimum_required(VERSION 3.10)

set(VERSION "0.0.1")
set(PROJECT_NAME "cxx_opt")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES CXX)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CTest)
enable_testing()
add_subdirectory(test)

