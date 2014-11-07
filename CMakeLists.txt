cmake_minimum_required(VERSION 2.8.4)
project(latin1-to-utf8)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W")
endif()
add_executable(latin1-to-utf8 main.c latin1-to-utf8.c)

enable_testing()

if(NOT DEFINED ENV{GTEST_ROOT})
    message(FATAL_ERROR "GTEST_ROOT is not defined. You must tell CMake where to find the gtest source")
endif()
add_subdirectory($ENV{GTEST_ROOT} gtest)
set(GTEST_INCLUDE_DIRS $ENV{GTEST_ROOT}/include $ENV{GTEST_ROOT})
set(GTEST_BOTH_LIBRARIES gtest gtest_main)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(all_tests all_tests.cc)
target_link_libraries(all_tests ${GTEST_BOTH_LIBRARIES})
add_test(AllTests all_tests)
