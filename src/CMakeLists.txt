# Require a minimum version of cmake 
cmake_minimum_required(VERSION 3.20)
# set the name of the project
set(PROJECT_NAME_STR srlib)
project(${PROJECT_NAME_STR})

set(CMAKE_BUILD_TYPE Release)

file(GLOB PROJECT_SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/../include/)

include_directories(${PROJECT_INCLUDE_DIR} ${DEP_INCLUDE_DIRS})
add_library(sr SHARED ${PROJECT_SRC_FILES})

target_link_libraries(sr PRIVATE)

install(TARGETS sr DESTINATION lib)
install(DIRECTORY ${PROJECT_INCLUDE_DIR} DESTINATION include) 