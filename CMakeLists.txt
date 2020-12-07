cmake_minimum_required(VERSION 3.10)
project(muduo_se C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

find_package(Threads)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(test)