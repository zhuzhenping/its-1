add_definitions(-DCOMMON_EXPORTS)

string( REGEX REPLACE ".*/(.*)" "\\1" NAME ${CMAKE_CURRENT_SOURCE_DIR} )
file(GLOB SOURCES "${SRC_PATH}/${NAME}/*.cpp")

message(${SOURCES})

include_directories(${INC_PATH}/qt/linux/include)
add_library(${NAME} SHARED ${SOURCES})