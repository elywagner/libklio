# -*- mode: cmake; -*-
#
#  Figure out the version of the used compiler
#  Variables set by this module
#  CMAKE_CXX_COMPILER_MAJOR  major version of compiler
#  CMAKE_CXX_COMPILER_MINR   minor version of compiler
#  CMAKE_CXX_COMPILER_PATCH  patch level (e.g. gcc 4.1.0)
#

# check the version of the compiler

if(NOT WIN32)
# extract the version of the compiler
if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
      OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)

  string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\1"
         CMAKE_CXX_COMPILER_MAJOR ${CMAKE_CXX_COMPILER_VERSION})
  string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*" "\\2"
         CMAKE_CXX_COMPILER_MINOR ${CMAKE_CXX_COMPILER_VERSION})
  set(CMAKE_CXX_COMPILER_PATCH "")
endif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
      OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)

  string(STRIP ${CMAKE_CXX_COMPILER_VERSION} CMAKE_CXX_COMPILER_VERSION)
  string(REGEX REPLACE "^([0-9]+).*$" "\\1"
         CMAKE_CXX_COMPILER_MAJOR ${CMAKE_CXX_COMPILER_VERSION})
  string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*$" "\\2"
         CMAKE_CXX_COMPILER_MINOR ${CMAKE_CXX_COMPILER_VERSION})
  string(REGEX REPLACE "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" "\\3"
         CMAKE_CXX_COMPILER_PATCH ${CMAKE_CXX_COMPILER_VERSION})
endif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
endif(NOT WIN32)

MARK_AS_ADVANCED(
  CMAKE_CXX_COMPILER_MAJOR
  CMAKE_CXX_COMPILER_MINOR
  CMAKE_CXX_COMPILER_PATCH
)


# just print the results if requested
function(info_compiler)
  message(STATUS "CMAKE_FORCE_CXX_COMPILER  = '${CMAKE_FORCE_CXX_COMPILER}'")
  message(STATUS "CMAKE_CXX_COMPILER        = '${CMAKE_CXX_COMPILER}'")
  message(STATUS "CMAKE_CXX_COMPILER_ID     = '${CMAKE_CXX_COMPILER_ID}'")
  message(STATUS "CMAKE_CXX_COMPILER_INIT   = '${CMAKE_CXX_COMPILER_INIT}'")
  message(STATUS "CMAKE_GENERATOR_CXX       = '${CMAKE_GENERATOR_CXX}'")
  message(STATUS "CMAKE_GNULD_IMAGE_VERSION = '${CMAKE_GNULD_IMAGE_VERSION}'")
  message(STATUS "CMAKE_CXX_COMPILER_VERSION= '${CMAKE_CXX_COMPILER_VERSION}'")
  message(STATUS "CMAKE_CXX_COMPILER_MAJOR  = '${CMAKE_CXX_COMPILER_MAJOR}'")
  message(STATUS "CMAKE_CXX_COMPILER_MINOR  = '${CMAKE_CXX_COMPILER_MINOR}'")
  message(STATUS "CMAKE_CXX_COMPILER_PATCH  = '${CMAKE_CXX_COMPILER_PATCH}'")
endfunction(info_compiler)
