# -*- mode: cmake; -*-
project(libklio)
string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT_NAME)
cmake_minimum_required(VERSION "2.6" FATAL_ERROR)
if(POLICY CMP0011)
  cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

OPTION(ENABLE_LOGGING
  "Build the project with logging enabled"
  OFF)

# have the full monty in makefiles
set(CMAKE_VERBOSE_MAKEFILE true)

set(V_MAJOR 0)
set(V_MINOR 5)
set(V_PATCH 2)

# use ctest
include(CTest)
enable_testing()

# add a path where some libraries might be stored
set(CMAKE_ADDITIONAL_PATH "$ENV{CMAKE_ADDITIONAL_PATH}" CACHE PATH "Path where many locally installed libraries can be found")

OPTION(ENABLE_ROCKSDB
  "Build the project with rockskdb enabled"
  ON)
OPTION(ENABLE_MSG
  "Build the project with mysmartgrid enabled"
  ON)

################################################################
  ###################################
# No user settings below
###################################################################################################

# Where are the additional libraries installed? Note: provide includes
# path here, subsequent checks will resolve everything else
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_ADDITIONAL_PATH}/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_ADDITIONAL_PATH}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/modules ${CMAKE_MODULE_PATH})

include (CheckCompiler)
info_compiler()

include (UseCodeCoverage)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wno-non-virtual-dtor -g -fomit-frame-pointer")

set(CMAKE_CXX_FLAGS_EXPERIMENTAL "-O0 -g -ggdb -Wall  -W -Wshadow")
#set(CMAKE_CXX_FLAGS_EXPERIMENTAL "-O0 -g -ggdb -Wall -Werror -W -Wshadow")
set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -Wunused-variable")
set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -Wunused-parameter")
set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -Wunused-function")
set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -Wunused -Woverloaded-virtual")
#set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -Wno-system-headers")
set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -Wno-non-virtual-dtor")
set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -pg -fprofile-generate ")
set(CMAKE_CXX_FLAGS_EXPERIMENTAL "${CMAKE_CXX_FLAGS_EXPERIMENTAL} -fprofile-arcs -ftest-coverage")

# gprof and gcov support
set(CMAKE_CXX_FLAGS_PROFILE "-O0 -g -ggdb -W -Wreturn-type -Wno-shadow")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -Wunused-variable -Wunused-parameter")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -Wunused-function -Wunused")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -Woverloaded-virtual -Wno-system-headers")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -Wno-non-virtual-dtor")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RROFILE} -pg -fprofile-arcs")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_PROFILE} -ftest-coverage")

# Releases are made with the release build. Optimize code and strip binaries.
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-non-virtual-dtor")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-but-set-variable")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-variable")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-sign-compare")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-uninitialized")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-attributes")

if( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-deprecated-declarations")
else()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
endif()

# always set these flags
set(CMAKE_CXX_FLAGS "-Wall")

FIND_PACKAGE(Sqlite)
# We depend on boost >= 1.46.1 because of the updated filesystem
# library. The boost version detection is a mess, see 
# $ cmake --help-module FindBoost
# , section " The mess that is Boost_ADDITIONAL_VERSIONS (sorry?)"
SET(Boost_ADDITIONAL_VERSIONS "1.46.1" "1.47" "1.47.1" "1.48.0" "1.48"
  "1.49.0" "1.49" "1.50.0" "1.50")
set (Boost_USE_STATIC_LIBS ON)
SET(Boost_DETAILED_FAILURE_MSG true)
FIND_PACKAGE(Boost 1.46.1 
  COMPONENTS test_exec_monitor program_options filesystem system date_time)

# RocksDB
# check if the compiler version supports c++ 2011
if( ENABLE_ROCKSDB )
  if( ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.6")  
    include(FindRocksDB)
    if(ROCKSDB_FOUND)
      include_directories(${ROCKSDB_INCLUDE_DIR})
    else(ROCKSDB_FOUND)
      set(ROCKSDB_INCLUDE_DIR "")
      message(STATUS "RocksDB support is disabled because the library is not found. Install RocksDB or call cmake -DROCKSDB_HOME=path_to_rocksdb_install in order to enable it.")
      set(ENABLE_ROCKSDB False)
    endif(ROCKSDB_FOUND)
  else()
    set(ENABLE_ROCKSDB False)
  endif()
endif()

if( ENABLE_MSG )
  # libjson
  include(FindJson)
  if( NOT JSON_FOUND)
    message(STATUS "libjson is required.
            Install json or call cmake -DJSON_HOME=path_to_json_install")
    set(ENABLE_MSG False)
  else()
    include_directories(${JSON_INCLUDE_DIR})

    # Curl
    IF(WIN32)
      add_definitions(-DCURL_STATICLIB)
      include(FindCURL_WIN32)
    else(WIN32)
      #  add_definitions(-DCURL_STATICLIB)
      include(FindCurl)
      #include(FindGnutls)
    endif(WIN32)
    if( NOT CURL_FOUND )
      set(ENABLE_MSG False)
    endif()
    find_package(OpenSSL)
  endif( NOT JSON_FOUND)
endif()

# this file is installed and contains all necessary information about the installed libseda,
# it also loads the file with the exported targets
configure_file(${PROJECT_NAME}Config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)

include_directories(${CMAKE_BINARY_DIR})

# Check for some compiler stuff
include(CMakeBackwardCompatibilityCXX)

# Reflect the package structure
add_subdirectory(src)
add_subdirectory(libklio)
add_subdirectory(tests)

# add some files to the installation target
INSTALL(FILES README.md LICENSE 
    DESTINATION share/doc/libklio
  )
INSTALL(FILES share/libklio/date_time_zonespec.csv 
    DESTINATION share/libklio
  )
configure_file(share/libklio/date_time_zonespec.csv 
  ${CMAKE_BINARY_DIR}/
  )

install(EXPORT ${PROJECT_NAME}Exports
    DESTINATION lib/shared
    FILE ${PROJECT_NAME}Targets.cmake
    COMPONENT libraries)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  DESTINATION lib/shared
  COMPONENT libraries)

# CPACK packaging
INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_GENERATOR "DEB" "STGZ" "TGZ" "TBZ2")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "klio library")
SET(CPACK_PACKAGE_VENDOR "Fraunhofer ITWM")
set(CPACK_PACKAGE_CONTACT "Fraunhofer ITWM <support@mysmartgrid.de>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_NAME "libklio")
SET(CPACK_PACKAGE_VERSION_MAJOR ${V_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${V_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${V_PATCH})

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6 (>= 4.6.0), libc6 (>= 2.13)")

#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
SET(CPACK_PACKAGE_EXECUTABLES "klio-sensor" "Functions for handling sensors")
SET(CPACK_PACKAGE_EXECUTABLES "klio-store" "Tool for dealing with sensor data stores")
set(CPACK_STRIP_FILES "usr/bin/klio-export;usr/bin/klio-sensor;usr/bin/klio-store")
SET(CPACK_SOURCE_STRIP_FILES "")
SET(CPACK_SOURCE_STRIP_FILES TRUE)
set(CPACK_SECTION "net")
include(CPackTools)


### print some output for the user
message("")
message("***** Configuration *****")
message("  prefix: ${CMAKE_INSTALL_PREFIX}")
message("  boost include: ${Boost_INCLUDE_DIRS}")
message("  boost libraries shortcut: ${Boost_LIBRARIES}")
message("  boost program options lib: ${Boost_PROGRAM_OPTIONS_LIBRARY}")
message("  boost system lib: ${Boost_SYSTEM_LIBRARY}")
message("  boost filesystem lib: ${Boost_FILESYSTEM_LIBRARY}")
message("  json: -L${JSON_LIBRARY} -I${JSON_INCLUDE_DIR}")
message("  sqlite3 include: ${SQLITE3_INCLUDE_DIR}, lib: ${SQLITE3_LIBRARIES}")
message("  rocksdb include: ${ROCKSDB_INCLUDE_DIR}, lib: ${ROCKSDB_STATIC_LIBRARIES}")
message("  extended logging: ${ENABLE_LOGGING}")
message("  rocksdb: ${ENABLE_ROCKSDB}")
message("  mysmartgrid: ${ENABLE_MSG}")

