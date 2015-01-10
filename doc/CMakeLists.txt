find_program(GZIP gzip
  HINTS
  /bin
  /usr/bin
  /usr/local/bin REQUIRED)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/doc/l1u8recode.1.gz
  COMMAND ${GZIP} -9 -c l1u8recode.1 > ${CMAKE_BINARY_DIR}/doc/l1u8recode.1.gz
  MAIN_DEPENDENCY l1u8recode.1
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/doc")

add_custom_target(documentation ALL
  DEPENDS ${CMAKE_BINARY_DIR}/doc/l1u8recode.1.gz)

install(FILES ${CMAKE_BINARY_DIR}/doc/l1u8recode.1.gz DESTINATION ${INSTALL_MAN_DIR})
