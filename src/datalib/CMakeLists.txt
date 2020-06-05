# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: comm library main CMakeLists.txt
# update-date:
#   2019-01-04  init version
###############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(datalib)

SET(CMAKE_CXX_STANDARD 11)

# 设置需要的boost 组件
set(BOOST_COMPONENTS system)
# 使用cmake find_package 查找boost库位置
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

###############################################################################################
# for general include
INCLUDE_DIRECTORIES("../../inc")
INCLUDE_DIRECTORIES("../../inc/qt/linux/include")

# for search lib pandora
LINK_DIRECTORIES("../../lib/boost")
LINK_DIRECTORIES("../../lib/qt")

###############################################################################################
# set the src file to compile
aux_source_directory(. DATALIB_SRC)


if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("############# Release #############")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3 -Wall -Wno-unused-function")
else()
    message("############# Debug #############")
    #ADD_DEFINITIONS(-D DEBUG_PRINT_ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O0 -Wall -ggdb -Wno-unused-function")
endif()

############################################################################################################################
# generate the library
ADD_LIBRARY(${PROJECT_NAME} SHARED ${DATALIB_SRC})
# 如果要生成同名的需要调用　SET_TARGET_PROPERTIES
# ADD_LIBRARY(${PROJECT_NAME}_static STATIC ${TOOL_SRC})

# SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
# 这里可以尝试设定版本, VERSION指动态库版本，SOVERSION指API版本
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

# link external lib
TARGET_LINK_LIBRARIES(${PROJECT_NAME} common)

#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

##
TARGET_LINK_LIBRARIES(${PROJECT_NAME} zmq)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})

############################################################################################################################
# installation
# install directory to the right path
INSTALL(DIRECTORY stg_trade DESTINATION "include/${PROJECT_NAME}"
                                            PATTERN "*.cpp" EXCLUDE)

# INSTALL(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static 
#         ARCHIVE DESTINATION "lib/${PROJECT_NAME}"
#         LIBRARY DESTINATION "lib/${PROJECT_NAME}")
# install library
INSTALL(TARGETS ${PROJECT_NAME} 
        ARCHIVE DESTINATION "lib/${PROJECT_NAME}"
        LIBRARY DESTINATION "lib/${PROJECT_NAME}")

############################################################################################################################

