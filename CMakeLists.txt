cmake_minimum_required(VERSION 2.8.4)
project(l1u8recode)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra -Werror")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Werror -std=c++11")
endif()

include(version.cmake)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

configure_file(config.h.in config.h)
include_directories(${CMAKE_BINARY_DIR})

find_package(PkgConfig REQUIRED)

pkg_check_modules(POPT REQUIRED popt)
include_directories(${POPT_INCLUDE_DIRS})
link_directories(${POPT_LIBRARY_DIRS})

add_executable(l1u8recode main.cc KmpSearch.cc l1u8recode.cc)
target_link_libraries(l1u8recode popt)

set(CPACK_SET_DESTDIR ON)

set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "l1u8recode install prefix" FORCE)

install(TARGETS l1u8recode RUNTIME DESTINATION bin)

enable_testing()

if(NOT DEFINED ENV{GTEST_ROOT})
    message(FATAL_ERROR "GTEST_ROOT is not defined. You must tell CMake where to find the gtest source")
endif()
add_subdirectory($ENV{GTEST_ROOT} gtest)
set(GTEST_INCLUDE_DIRS $ENV{GTEST_ROOT}/include $ENV{GTEST_ROOT})
set(GTEST_BOTH_LIBRARIES gtest gtest_main)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(all_tests KmpSearch.cc kmp_tests.cc l1u8recode.cc l1u8recode_tests.cc)
target_link_libraries(all_tests ${GTEST_BOTH_LIBRARIES})
add_test(AllTests all_tests)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VENDOR  "Eliezio Oliveira <ebo@pobox.com>")
set(CPACK_PACKAGE_CONTACT "Eliezio Oliveira <ebo@pobox.com>")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_RPM_PACKAGE_RELEASE ${PACKAGE_RELEASE})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A smart ISO-8859-1 to UTF-8 recoder")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/eliezio/latin1-to-utf8")
set(CPACK_DEBIAN_PACKAGE_SECTION "text")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# CPACK_DEBIAN_PACKAGE_ARCHITECTURE should get set automatically by dpkg --print-architecture
# ... except that doesn't happen until CPack runs, and we need it here to set the package filename
# This is copied from CPackDeb.cmake.
find_program(DPKG_CMD dpkg)
if(DPKG_CMD)
  execute_process(COMMAND "${DPKG_CMD}" --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
  message(STATUS "Can not find dpkg in your path, default to i386.")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
endif()
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSION}-${PACKAGE_RELEASE}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
include(CPack)
