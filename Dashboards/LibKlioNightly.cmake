# -*- mode: cmake; -*-

set(ENV{https_proxy} "http://squid.itwm.fhg.de:3128/")
include(Tools.cmake)
my_ctest_setup()
include(CTestConfigLibKlio.cmake)
set(_ctest_type "Nightly")
# set(_ctest_type "Continuous")
# set(_ctest_type "Coverage")

set(URL "https://github.com/mysmartgrid/libklio.git")

set(CTEST_BASE_DIRECTORY "${KDE_CTEST_DASHBOARD_DIR}/${_projectNameDir}/${_ctest_type}")
set(CTEST_SOURCE_DIRECTORY "${CTEST_BASE_DIRECTORY}/${_srcDir}-${_git_branch}" )
set(CTEST_BINARY_DIRECTORY "${CTEST_BASE_DIRECTORY}/${_buildDir}-${CTEST_BUILD_NAME}")
set(CTEST_INSTALL_DIRECTORY "${CTEST_BASE_DIRECTORY}/install-${CTEST_BUILD_NAME}")
set(KDE_CTEST_VCS "git")
set(KDE_CTEST_VCS_REPOSITORY ${URL})

set(CMAKE_INSTALL_PREFIX "/usr")
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
#set(CTEST_BUILD_CONFIGURATION "Profiling")

configure_ctest_config(${KDE_CTEST_VCS_REPOSITORY} "CTestConfigLibKlio.cmake")
kde_ctest_setup()

FindOS(OS_NAME OS_VERSION)

set(ctest_config ${CTEST_BASE_DIRECTORY}/CTestConfig.cmake)
#######################################################################
ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

message("_git_branch ..: '${_git_branch}'")
message("compiler    ..: '${COMPILER_ID}'")

find_program(CTEST_GIT_COMMAND NAMES git)
set(CTEST_UPDATE_TYPE git)

set(CTEST_UPDATE_COMMAND  ${CTEST_GIT_COMMAND})
if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}/.git/HEAD")
  set(CTEST_CHECKOUT_COMMAND "${CTEST_GIT_COMMAND} clone ${URL} ${CTEST_SOURCE_DIRECTORY}")
endif(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}/.git/HEAD")

ctest_start(${_ctest_type})
ctest_update(SOURCE ${CTEST_SOURCE_DIRECTORY} RETURN_VALUE update_res)
message("===> Update returns: ${update_res}")
ctest_submit(PARTS Update)

execute_process(
  COMMAND ${CTEST_GIT_COMMAND} checkout  ${_git_branch}
  WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
  )

if( "${OS_NAME}-${OS_VERSION}" STREQUAL "Ubuntu-10.04" )
  set(BOOST_ROOT /homes/krueger/external_software/ubuntu_100403/${CMAKE_SYSTEM_PROCESSOR}/boost/1.46)
else( "${OS_NAME}-${OS_VERSION}" STREQUAL "Ubuntu-10.04" )
  set(BOOST_ROOT "")
endif( "${OS_NAME}-${OS_VERSION}" STREQUAL "Ubuntu-10.04" )

if(CMAKE_TOOLCHAIN_FILE)
  kde_ctest_write_initial_cache("${CTEST_BINARY_DIRECTORY}"
    CMAKE_TOOLCHAIN_FILE
    CMAKE_INSTALL_PREFIX
    )
  set(OS_NAME "openWRT")
  set(OS_VERSION "10.03.1")
  set(CMAKE_SYSTEM_PROCESSOR ${openwrt_arch})
else(CMAKE_TOOLCHAIN_FILE)
  kde_ctest_write_initial_cache("${CTEST_BINARY_DIRECTORY}"
    BOOST_ROOT
    CMAKE_INSTALL_PREFIX
)
endif(CMAKE_TOOLCHAIN_FILE)


ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}"  RETURN_VALUE resultConfigure)
message("====> Configure: ${resultConfigure}")

if( STAGING_DIR)
  include(${CTEST_BINARY_DIRECTORY}/CMakeCache.txt)
  set(ENV{STAGING_DIR}     ${OPENWRT_STAGING_DIR})
endif( STAGING_DIR)

ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE build_res)
message("====> BUILD: ${build_res}")

if( NOT CMAKE_TOOLCHAIN_FILE )
  ctest_test(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE test_res)
  message("====> TESTS: ${test_res}")
endif( NOT CMAKE_TOOLCHAIN_FILE )

ctest_submit(RETURN_VALUE res)

# package files
include(${CTEST_BINARY_DIRECTORY}/CPackConfig.cmake)
if( STAGING_DIR)
  set(ENV{PATH}            ${OPENWRT_STAGING_DIR}/host/bin:$ENV{PATH})
endif( STAGING_DIR)

if( NOT ${build_res} AND NOT ${update_res})
  # do the packaging
  execute_process(
    COMMAND cpack -G DEB
    WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}
    )
  # install for other packages
  execute_process(
    COMMAND make install
    WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}
    )
endif( NOT ${build_res} AND NOT ${update_res})

# upload files
if( NOT ${build_res} AND ${CTEST_PUSH_PACKAGES})
  message( "OS_NAME .....: ${OS_NAME}")
  message( "OS_VERSION ..: ${OS_VERSION}")
  message( "CMAKE_SYSTEM_PROCESSOR ..: ${CMAKE_SYSTEM_PROCESSOR}")

  if(CPACK_ARCHITECTUR)
    set(OPKG_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_ARCHITECTUR}")
    set(_package_file "${OPKG_FILE_NAME}.ipk")
  else(CPACK_ARCHITECTUR)
    set(_package_file "${CPACK_PACKAGE_FILE_NAME}.deb")
  endif(CPACK_ARCHITECTUR)
  message("==> Upload packages - ${_package_file}")
  set(_export_host ${CTEST_PACKAGE_SITE})
  set(_remote_dir "packages/${OS_NAME}/${OS_VERSION}/${CMAKE_SYSTEM_PROCESSOR}")
  execute_process(
    COMMAND ssh ${_export_host} mkdir -p ${_remote_dir}
    )
  execute_process(
    COMMAND scp -p ${_package_file} ${_export_host}:${_remote_dir}/${_package_file}
    WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY}/${subproject}
    )
endif( NOT ${build_res} AND ${CTEST_PUSH_PACKAGES})

message("DONE")
