cmake_minimum_required(VERSION 2.8.4)
project(l1u8recode)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra -Werror")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Werror -std=c++11")
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(POPT REQUIRED popt)
include_directories(${POPT_INCLUDE_DIRS})
link_directories(${POPT_LIBRARY_DIRS})

add_executable(l1u8recode main.cc KmpSearch.cc l1u8recode.cc)
target_link_libraries(l1u8recode popt)

install(TARGETS l1u8recode RUNTIME DESTINATION bin)

enable_testing()

if(NOT DEFINED ENV{GTEST_ROOT})
    message(FATAL_ERROR "GTEST_ROOT is not defined. You must tell CMake where to find the gtest source")
endif()
add_subdirectory($ENV{GTEST_ROOT} gtest)
set(GTEST_INCLUDE_DIRS $ENV{GTEST_ROOT}/include $ENV{GTEST_ROOT})
set(GTEST_BOTH_LIBRARIES gtest gtest_main)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(kmp_tests KmpSearch.cc kmp_tests.cc)
target_link_libraries(kmp_tests ${GTEST_BOTH_LIBRARIES})
add_test(KmpTests kmp_tests)

add_executable(l1u8recode_tests KmpSearch.cc l1u8recode.cc l1u8recode_tests.cc)
target_link_libraries(l1u8recode_tests ${GTEST_BOTH_LIBRARIES})
add_test(L1U8RecodeTests l1u8recode_tests)
