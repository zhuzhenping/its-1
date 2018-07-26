cmake_minimum_required(VERSION 3.10)
project(its)

set(CMAKE_CXX_STANDARD 98)

set(BIN_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIB_PATH ${PROJECT_SOURCE_DIR}/lib)
set(INC_PATH ${PROJECT_SOURCE_DIR}/inc)
set(SRC_PATH ${PROJECT_SOURCE_DIR}/src)

set(LIBRARY_OUTPUT_PATH ${BIN_PATH}/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${BIN_PATH}/${CMAKE_BUILD_TYPE})

include_directories(${INC_PATH})
link_directories(${LIB_PATH} ${BIN_PATH})

add_subdirectory(prj)
