cmake_minimum_required(VERSION 2.8.4)
project(l1u8recode)

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -Werror")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror -std=c++11")
endif()

add_executable(l1u8recode main.cc kmp.cc l1u8recode.cc)

enable_testing()

if(NOT DEFINED ENV{GTEST_ROOT})
    message(FATAL_ERROR "GTEST_ROOT is not defined. You must tell CMake where to find the gtest source")
endif()
add_subdirectory($ENV{GTEST_ROOT} gtest)
set(GTEST_INCLUDE_DIRS $ENV{GTEST_ROOT}/include $ENV{GTEST_ROOT})
set(GTEST_BOTH_LIBRARIES gtest gtest_main)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(kmp_tests kmp.cc kmp_tests.cc)
target_link_libraries(kmp_tests ${GTEST_BOTH_LIBRARIES})
add_test(KmpTests kmp_tests)

add_executable(l1u8recode_tests kmp.cc l1u8recode.cc l1u8recode_tests.cc)
target_link_libraries(l1u8recode_tests ${GTEST_BOTH_LIBRARIES})
add_test(L1U8RecodeTests l1u8recode_tests)
