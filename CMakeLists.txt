cmake_minimum_required(VERSION 2.8)

project(IoCContainer)

#SET(CMAKE_CXX_COMPILER "g++-4.7")
SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_FLAGS "-std=c++11") # Add c++11 functionality  

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)
find_package(LLVM REQUIRED)

ENABLE_TESTING()
add_subdirectory(src)
