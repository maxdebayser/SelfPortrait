cmake_minimum_required(VERSION 2.8)

project(IoCContainer)

#SET(CMAKE_CXX_COMPILER "g++-4.7")
SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") # Add c++11 functionality
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--build-id")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti") # Add c++11 functionality  
#add_definitions(-DNO_RTTI)

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

if (${LIB64} STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)
IF(SELFPORTRAIT_BUILD_PARSER)
    find_package(LLVM REQUIRED)
ENDIF()


IF(NOT (LUA_LIBRARY OR LUA_INCLUDEDIR OR LUA_SHORT_VERSION))
    find_package(Lua REQUIRED)
ENDIF()

IF(NOT LUA_LIBRARY)
    MESSAGE(FATAL_ERROR "Lua library not found")
ENDIF()

IF(NOT LUA_INCLUDEDIR)
    MESSAGE(FATAL_ERROR "Lua include dir not found")
ENDIF()

IF(NOT LUA_SHORT_VERSION)
    MESSAGE(FATAL_ERROR "Lua version not known (5.1 or 5.2)")
ENDIF()

if(LUA_SHORT_VERSION STREQUAL "5.2")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA52")
endif()

if(LUA_SHORT_VERSION STREQUAL "5.1")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA51")
endif()

find_package(CxxTest REQUIRED)

ENABLE_TESTING()
add_subdirectory(src)
