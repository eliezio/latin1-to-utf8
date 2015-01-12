cmake_minimum_required(VERSION 2.8.4)
project(l1u8recode)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

if(NOT CMAKE_BUILD_TYPE)
    set(
        CMAKE_BUILD_TYPE
        Debug
        CACHE
        STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

message(STATUS "Source: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary: ${CMAKE_BINARY_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "In-source builds are not permitted. Make a separate folder for building:\n"
    "mkdir build; cd build; cmake ..\n"
    "Before that, remove the files that cmake just created:\n"
    "rm -rf CMakeCache.txt CMakeFiles")
endif()

set(INSTALL_DATA_DIR_BASE "share" CACHE STRING "Custom data installation directory without suffixes")
set(INSTALL_DOC_DIR_BASE "${INSTALL_DATA_DIR_BASE}/doc" CACHE STRING "Custom doc installation directory without suffixes")
set(INSTALL_DOC_DIR "${INSTALL_DOC_DIR_BASE}/${CMAKE_PROJECT_NAME}" CACHE STRING "Custom doc installation directory")
set(INSTALL_MAN_DIR "${INSTALL_DATA_DIR_BASE}/man/man1" CACHE STRING "Custom manpage installation directory without suffixes")

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION ${INSTALL_DOC_DIR} RENAME copyright)
install(FILES "${CMAKE_SOURCE_DIR}/README.org" DESTINATION ${INSTALL_DOC_DIR} RENAME README)

add_subdirectory(doc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=c++11")

find_package(Git)

execute_process(
  COMMAND "${GIT_EXECUTABLE}" describe --tags --always --abbrev=5
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE VERSION
  RESULT_VARIABLE EXECUTE_GIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
string(REGEX REPLACE "^v" "" VERSION "${VERSION}")
string(LENGTH "${VERSION}" VERSION_LEN)
math(EXPR VERSION_LEN "${VERSION_LEN} - 7")
string(SUBSTRING "${VERSION}" 0 ${VERSION_LEN} VERSION)

configure_file(config.h.in config.h)
include_directories(${CMAKE_BINARY_DIR})

find_package(PkgConfig REQUIRED)

pkg_check_modules(POPT REQUIRED popt)
include_directories(${POPT_INCLUDE_DIRS})
link_directories(${POPT_LIBRARY_DIRS})

add_executable(l1u8recode main.cc KmpSearch.cc l1u8recode.cc)
target_link_libraries(l1u8recode popt)

install(TARGETS l1u8recode RUNTIME DESTINATION bin)

enable_testing()

set(GTEST_ROOT "/usr/src/gtest")
add_subdirectory(${GTEST_ROOT} gtest)
set(GTEST_INCLUDE_DIRS ${GTEST_ROOT}/src)
set(GTEST_BOTH_LIBRARIES gtest gtest_main)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(all_tests KmpSearch.cc kmp_tests.cc l1u8recode.cc l1u8recode_tests.cc)
target_link_libraries(all_tests ${GTEST_BOTH_LIBRARIES})
add_test(AllTests all_tests)

# CPACK_DEBIAN_PACKAGE_ARCHITECTURE should get set automatically by dpkg --print-architecture
# ... except that doesn't happen until CPack runs, and we need it here to set the package filename
# This is copied from CPackDeb.cmake.
execute_process(COMMAND /usr/bin/dpkg --print-architecture
  OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND /usr/bin/lsb_release -sc
  OUTPUT_VARIABLE DISTRIBUTION_CODENAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)

include(ppa_config.cmake REQUIRED)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION "${VERSION}${PPA_DEBIAN_VERSION}~${DISTRIBUTION_CODENAME}")
set(CPACK_PACKAGE_CONTACT "Eliezio Oliveira <ebo@pobox.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/eliezio/latin1-to-utf8")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Smart recoding from ISO-8859-1 to UTF-8")
set(CPACK_PACKAGE_DESCRIPTION
  "Utility to recode files from ISO-8859-1 to UTF8 with two special capabilities:\n"
  "1. The recoding can be restricted to delimited contents of the input;\n"
  "2. It preserves characters already converted to UTF-8.")

set(CPACK_STRIP_FILES TRUE)

set(CPACK_DEBIAN_PACKAGE_SECTION "text")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_BUILD_DEPENDS python-minimal gzip cmake pkg-config libpopt-dev libgtest-dev)
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

include(CPack)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "any") # can be build on any system
include(UploadPPA)
