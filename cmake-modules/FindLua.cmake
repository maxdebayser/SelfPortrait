INCLUDE(FindPkgConfig)

IF(PKG_CONFIG_FOUND)
        pkg_check_modules (LUA lua>=5.1)
        if (LUA_VERSION VERSION_EQUAL "5.3")
                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA53" CACHE STRING "Lua version compiler flag")
                SET(LUA_SHORT_VERSION "5.3" CACHE STRING "Lua short version string")
        elseif (LUA_VERSION VERSION_EQUAL "5.2")
                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA52" CACHE STRING "Lua version compiler flag")
                SET(LUA_SHORT_VERSION "5.2" CACHE STRING "Lua short version string")
        else()
                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA51" CACHE STRING "Lua version compiler flag")
		SET(LUA_SHORT_VERSION "5.1" CACHE STRING "Lua short version string")
        endif()

        FIND_LIBRARY(LUA_LIBRARY NAMES lua)

ELSE(PKG_CONFIG_FOUND)
	MESSAGE(WARNING " pkg-config not found, version detection will be much less reliable")
	find_program(LuaInterp lua)

	IF(${LuaInterp} STREQUAL "LuaInterp-NOTFOUND") 

		MESSAGE(WARNING " Lua interpreter not found. Can't make version detection. Compilation might not work ")
		
	ELSE(${LuaInterp} STREQUAL "LuaInterp-NOTFOUND") 

                execute_process(COMMAND ${LuaInterp} -v ERROR_VARIABLE LUA_OUTPUT OUTPUT_VARIABLE LUA_OUTPUT)
                STRING(REGEX MATCH "Lua ([0-9]+.[0-9]+).*" DUMMY "${LUA_OUTPUT}" )
		SET(LUA_VERSION ${CMAKE_MATCH_1} CACHE STRING "Lua version string")

                IF (LUA_VERSION VERSION_LESS "5.1")
			MESSAGE(FATAL_ERROR "Lua reports unsupported version ${LUA_VERSION}")
                ELSE ()
			MESSAGE(STATUS "Found Lua version = ${LUA_VERSION}")
                        if (LUA_VERSION VERSION_EQUAL "5.3")
                                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA53" CACHE STRING "Lua version compiler flag")
                                SET(LUA_SHORT_VERSION "5.3" CACHE STRING "Lua short version string")
                        elseif (LUA_VERSION VERSION_EQUAL "5.2")
                                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA52" CACHE STRING "Lua version compiler flag")
                                SET(LUA_SHORT_VERSION "5.2" CACHE STRING "Lua short version string")
                        else()
                                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA51" CACHE STRING "Lua version compiler flag")
                                SET(LUA_SHORT_VERSION "5.1" CACHE STRING "Lua short version string")
                        endif()
                ENDIF ()

	ENDIF(${LuaInterp} STREQUAL "LuaInterp-NOTFOUND") 


        FIND_PATH(LUA_INCLUDEDIR lua.h)

	FIND_LIBRARY(LUA_LIBRARY NAMES lua) 

	IF (LUA_INCLUDEDIR AND LUA_LIBRARY)
	   SET(LUA_FOUND TRUE)
	ENDIF (LUA_INCLUDEDIR AND LUA_LIBRARY)

	IF (LUA_FOUND)
		IF (NOT Lua_FIND_QUIETLY)
			MESSAGE(STATUS "Found Lua: ${LUA_LIBRARY}")
		ENDIF (NOT Lua_FIND_QUIETLY)
	ELSE (LUA_FOUND)
		IF (Lua_FIND_REQUIRED)
			MESSAGE(FATAL_ERROR "Could not find Lua")
		ENDIF (Lua_FIND_REQUIRED)
	ENDIF (LUA_FOUND)

	INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})

ENDIF(PKG_CONFIG_FOUND)

