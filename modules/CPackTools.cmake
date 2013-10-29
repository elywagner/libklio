# -*- mode: cmake; -*-

function (get_revision_info REV_INFO)
  execute_process (
    COMMAND git rev-list origin/master..HEAD
    COMMAND wc -l
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE _out_git
    ERROR_VARIABLE _err_git
    RESULT_VARIABLE _res_git
    )
  if (${_res_git} EQUAL 0)
    string (STRIP "${_out_git}" _out_git)
    set (${REV_INFO} ${_out_git} PARENT_SCOPE)
  endif()
  message("git result: ${_res_git}")
  message("Projectrevision is: ${_out_git}")
endfunction()

get_revision_info (PROJECT_REVISION)
#if (NOT ${PROJECT_REVISION} EQUAL "")
#  message (FATAL_ERROR "could not discover revision info, please define -DPROJECT_REVISION by hand!")
#endif()
set (PROJECT_REVISION "${PROJECT_REVISION}" CACHE STRING "Source code revision" FORCE)

set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}.${PROJECT_REVISION}")

# set system architecture
if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
  if( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
  elseif( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
  elseif( ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "armv6l")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
  else()
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
  endif()
endif()

if(NOT CPACK_INSTALL_PREFIX)
	set(CPACK_INSTALL_PREFIX "/usr")
endif()

set(CPACK_SET_DESTDIR On)

set(CPACK_SYSTEM_NAME ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}-1_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

message(STATUS "CPACK_DEBIAN_PACKAGE_ARCHITECTURE: ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CPACK_SYSTEM_NAME: ${CPACK_SYSTEM_NAME}")

include(CPack)
