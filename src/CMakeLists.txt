# copyright [2018] [wxblockchain]
# author: daniel.bian
# descrition: galaxy & singularity main CMakefiles.txt
# update-date:
#   2019-01-04  init version

###############################################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(its.component)

###############################################################################################################
STRING(TIMESTAMP BUILDTIME "%Y%m%d%H%M%S")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "its Console Trading System Component")
SET(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
SET(CPACK_PACKAGE_VENDOR "wxblockchain")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "2")

SET(CPACK_GENERATOR "RPM")
SET(CPACK_RPM_PACKAGE_RELEASE "${BUILDTIME}")
SET(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
SET(CPACK_RPM_PACKAGE_GROUP "${PROJECT_NAME}")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/opt/its/console")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/opt/its/console")
###############################################################################################################
# add_definitions(-std=c++11 -Wall -Wextra)
SET(CMAKE_BUILD_TYPE "Debug")
OPTION(BUILD_SHARED_LIBS "turn OFF for .a libs" ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall -pthread")

###############################################################################################################
FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

###############################################################################################################
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(network)
ADD_SUBDIRECTORY(datalib)
ADD_SUBDIRECTORY(ctp)
ADD_SUBDIRECTORY(account)
ADD_SUBDIRECTORY(strategy)
ADD_SUBDIRECTORY(server)
ADD_SUBDIRECTORY(main)

###############################################################################################################
# system info here
SET(SYS_INFO_TXT_IN "${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.txt")
SET(SYS_INFO_TXT    "${CMAKE_CURRENT_BINARY_DIR}/SysInfo.txt")
CONFIGURE_FILE(${SYS_INFO_TXT_IN} ${SYS_INFO_TXT})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_BINARY_DIR}/SysInfo.txt")

# set the execute script before and after install
#SET(CPACK_RPM_PRE_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/galaxy/scripts/pre_install.sh")
#SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/galaxy/scripts/post_install.sh")
#SET(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/galaxy/scripts/pre_uninstall.sh")
#SET(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/galaxy/scripts/post_uninstall.sh")

# 检查文件依赖
OPTION(SKIP_DEPENDENCY_CHECKING "SKIP_CHECK_ITS_DEPENDENCY, default OFF." OFF)
# IF(SKIP_DEPENDENCY_CHECKING)
#     MESSAGE(WARNING "DEPENDENCY CHECKING SKIPPED...")
# ELSE()
#     SET(CPACK_RPM_PACKAGE_REQUIRES "rfoo >= 1.3.1, pid >= 2.1.1, log4cplus2 == 2.0.0_RC1, supervisor >= 3.1.0")
# ENDIF()

SET(CMAKE_INSTALL_PREFIX /opt/its/console)
SET(CPACK_RPM_PACKAGE_AUTOREQ " no")
INCLUDE(CPack)

###############################################################################################################
