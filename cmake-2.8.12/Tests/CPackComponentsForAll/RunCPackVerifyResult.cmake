message(STATUS "=============================================================================")
message(STATUS "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)")
message(STATUS "")

if(NOT CPackComponentsForAll_BINARY_DIR)
  message(FATAL_ERROR "CPackComponentsForAll_BINARY_DIR not set")
endif()

if(NOT CPackGen)
  message(FATAL_ERROR "CPackGen not set")
endif()

message("CMAKE_CPACK_COMMAND = ${CMAKE_CPACK_COMMAND}")
if(NOT CMAKE_CPACK_COMMAND)
  message(FATAL_ERROR "CMAKE_CPACK_COMMAND not set")
endif()

if(NOT CPackComponentWay)
  message(FATAL_ERROR "CPackComponentWay not set")
endif()

set(expected_file_mask "")
# The usual default behavior is to expect a single file
# Then some specific generators (Archive, RPM, ...)
# May produce several numbers of files depending on
# CPACK_COMPONENT_xxx values
set(expected_count 1)
set(config_type $ENV{CMAKE_CONFIG_TYPE})
set(config_args )
if(config_type)
  set(config_args -C ${config_type})
endif()
set(config_verbose )

if(CPackGen MATCHES "ZIP")
    set(expected_file_mask "${CPackComponentsForAll_BINARY_DIR}/MyLib-*.zip")
    if (${CPackComponentWay} STREQUAL "default")
        set(expected_count 1)
    elseif (${CPackComponentWay} STREQUAL "OnePackPerGroup")
        set(expected_count 3)
    elseif (${CPackComponentWay} STREQUAL "IgnoreGroup")
        set(expected_count 4)
    elseif (${CPackComponentWay} STREQUAL "AllInOne")
        set(expected_count 1)
    endif ()
elseif (CPackGen MATCHES "RPM")
    set(config_verbose -D "CPACK_RPM_PACKAGE_DEBUG=1")
    set(expected_file_mask "${CPackComponentsForAll_BINARY_DIR}/MyLib-*.rpm")
    if (${CPackComponentWay} STREQUAL "default")
        set(expected_count 1)
    elseif (${CPackComponentWay} STREQUAL "OnePackPerGroup")
        set(expected_count 3)
    elseif (${CPackComponentWay} STREQUAL "IgnoreGroup")
        set(expected_count 4)
    elseif (${CPackComponentWay} STREQUAL "AllInOne")
        set(expected_count 1)
    endif ()
elseif (CPackGen MATCHES "DEB")
    set(expected_file_mask "${CPackComponentsForAll_BINARY_DIR}/MyLib-*.deb")
    if (${CPackComponentWay} STREQUAL "default")
        set(expected_count 1)
    elseif (${CPackComponentWay} STREQUAL "OnePackPerGroup")
        set(expected_count 3)
    elseif (${CPackComponentWay} STREQUAL "IgnoreGroup")
        set(expected_count 4)
    elseif (${CPackComponentWay} STREQUAL "AllInOne")
        set(expected_count 1)
    endif ()
endif()

if(CPackGen MATCHES "DragNDrop")
    set(expected_file_mask "${CPackComponentsForAll_BINARY_DIR}/MyLib-*.dmg")
    if (${CPackComponentWay} STREQUAL "default")
        set(expected_count 1)
    elseif (${CPackComponentWay} STREQUAL "OnePackPerGroup")
        set(expected_count 3)
    elseif (${CPackComponentWay} STREQUAL "IgnoreGroup")
        set(expected_count 4)
    elseif (${CPackComponentWay} STREQUAL "AllInOne")
        set(expected_count 1)
    endif ()
endif()

# clean-up previously CPack generated files
if(expected_file_mask)
  file(GLOB expected_file "${expected_file_mask}")
  if (expected_file)
    file(REMOVE ${expected_file})
  endif()
endif()

message("config_args = ${config_args}")
message("config_verbose = ${config_verbose}")
execute_process(COMMAND ${CMAKE_CPACK_COMMAND} ${config_verbose} -G ${CPackGen} ${config_args}
    RESULT_VARIABLE CPack_result
    OUTPUT_VARIABLE CPack_output
    ERROR_VARIABLE CPack_error
    WORKING_DIRECTORY ${CPackComponentsForAll_BINARY_DIR})

if (CPack_result)
  message(FATAL_ERROR "error: CPack execution went wrong!, CPack_output=${CPack_output}, CPack_error=${CPack_error}")
else ()
  message(STATUS "CPack_output=${CPack_output}")
endif()

# Now verify if the number of expected file is OK
# - using expected_file_mask and
# - expected_count
if(expected_file_mask)
  file(GLOB expected_file "${expected_file_mask}")

  message(STATUS "expected_count='${expected_count}'")
  message(STATUS "expected_file='${expected_file}'")
  message(STATUS "expected_file_mask='${expected_file_mask}'")

  if(NOT expected_file)
    message(FATAL_ERROR "error: expected_file=${expected_file} does not exist: CPackComponentsForAll test fails. (CPack_output=${CPack_output}, CPack_error=${CPack_error}")
  endif()

  list(LENGTH expected_file actual_count)
  message(STATUS "actual_count='${actual_count}'")
  if(NOT actual_count EQUAL expected_count)
    message(FATAL_ERROR "error: expected_count=${expected_count} does not match actual_count=${actual_count}: CPackComponents test fails. (CPack_output=${CPack_output}, CPack_error=${CPack_error})")
  endif()
endif()
