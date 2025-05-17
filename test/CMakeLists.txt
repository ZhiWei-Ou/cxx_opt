cmake_minimum_required(VERSION 3.10)
project(flag_test LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME} flag_test.cpp ../cxx_opt.cpp)
target_link_libraries(${PROJECT_NAME} gtest_main)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

