cmake_minimum_required(VERSION 3.20)

project(
    ImperfectSR
    VERSION 1.0
    LANGUAGES CXX
)

# Check that the we are using a C++11-conforming compiler
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17
    support. Please use a C++17-compatible compiler.")
endif()

# Check that the prerequisite libraries are installed and visible
message(STATUS "Starting dependency check...")

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

message (STATUS "Finished dependency check.")


add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_definitions("-c")
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${DEP_INCLUDE_DIRS} ${COMMON_INCLUDES})
file(GLOB PROJECT_SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# generate example executable in tests directory
add_executable(example tests/example.cpp)
target_link_libraries(example sr)
# to create a new executable, just add a new line like:
# add_executable(my_main tests/my_main.cpp sr)

# Python bindings
# inspired by: https://github.com/pybind/scikit_build_example
pybind11_add_module(_core python/bindings.cpp ${PROJECT_SRC_FILES} WITH_SOABI)
include_directories(${DEP_INCLUDE_DIRS} ${COMMON_INCLUDES})
target_link_libraries(_core PRIVATE pybind11::headers)
install(TARGETS _core DESTINATION srfloat)