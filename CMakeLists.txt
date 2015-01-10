cmake_minimum_required(VERSION 2.8.4)
project(l1u8recode)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra -Werror")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Werror -std=c++11")
endif()

set(CPACK_SET_DESTDIR ON)

set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "l1u8recode install prefix" FORCE)
set(INSTALL_DATA_DIR_BASE "${CMAKE_INSTALL_PREFIX}/share" CACHE STRING "Custom data installation directory without suffixes")
set(INSTALL_DOC_DIR_BASE "${INSTALL_DATA_DIR_BASE}/doc" CACHE STRING "Custom doc installation directory without suffixes")
set(INSTALL_DOC_DIR "${INSTALL_DOC_DIR_BASE}/${CMAKE_PROJECT_NAME}" CACHE STRING "Custom doc installation directory")
set(INSTALL_MAN_DIR "${INSTALL_DATA_DIR_BASE}/man/man1" CACHE STRING "Custom manpage installation directory without suffixes")

add_subdirectory(doc)

find_package(Git)

execute_process(
  COMMAND "${GIT_EXECUTABLE}" describe --tags --dirty --always
  WORKING_DIRECTORY "${vobsub2srt_SOURCE_DIR}"
  OUTPUT_VARIABLE VERSION
  RESULT_VARIABLE EXECUTE_GIT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET)
string(REGEX REPLACE "^v" "" VERSION "${VERSION}")

configure_file(config.h.in config.h)
include_directories(${CMAKE_BINARY_DIR})

find_package(PkgConfig REQUIRED)

pkg_check_modules(POPT REQUIRED popt)
include_directories(${POPT_INCLUDE_DIRS})
link_directories(${POPT_LIBRARY_DIRS})

add_executable(l1u8recode main.cc KmpSearch.cc l1u8recode.cc)
target_link_libraries(l1u8recode popt)

install(TARGETS l1u8recode RUNTIME DESTINATION bin)

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION ${INSTALL_DOC_DIR} RENAME copyright)
install(FILES "${CMAKE_SOURCE_DIR}/README.org" DESTINATION ${INSTALL_DOC_DIR} RENAME README)

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

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION "${VERSION}")
set(CPACK_PACKAGE_CONTACT "Eliezio Oliveira <ebo@pobox.com>")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/eliezio/latin1-to-utf8")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A smart ISO-8859-1 to UTF-8 recoder")
set(CPACK_PACKAGE_DESCRIPTION "A utility to recode files from ISO-8859-1 (latin1) to UTF8 that supports mixed encodings and/or formats.\n"
  "It's able to handle these two special cases:\n"
  "1. The recoding can be applyed to delimited portions of the input;\n"
  "2. Preserves characters already converted to UTF-8.")

set(CPACK_STRIP_FILES TRUE)

set(CPACK_DEBIAN_PACKAGE_SECTION "text")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpopt0 >= 1.14")
set(CPACK_DEBIAN_PACKAGE_BUILD_DEPENDS python-minimal cmake pkg-config libpopt-dev libgtest-dev)
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
set(PPA_DEBIAN_VERSION "ppa1")

include(ppa_config.cmake OPTIONAL)
set(DPUT_HOST "ppa:ebo/cvs2git")

include(CPack)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "any") # can be build on any system
include(UploadPPA)
