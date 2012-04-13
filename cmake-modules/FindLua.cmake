INCLUDE(FindPkgConfig)

IF(PKG_CONFIG_FOUND)

	pkg_check_modules (LUA lua>=5.1)
	INCLUDE_DIRECTORIES(${LUA_INCLUDEDIR})

ELSE(PKG_CONFIG_FOUND)
	MESSAGE(WARNING " pkg-config not found, version detection will be much less reliable")
	find_program(LuaInterp lua)

	IF(${LuaInterp} STREQUAL "LuaInterp-NOTFOUND") 

		MESSAGE(WARNING " Lua interpreter not found. Can't make version detection. Compilation might not work ")
		
	ELSE(${LuaInterp} STREQUAL "LuaInterp-NOTFOUND") 

		execute_process(COMMAND ${LuaInterp} -v ERROR_VARIABLE LUA_OUTPUT)
		STRING(REGEX MATCH "Lua ([0-9]+.[0-9]+.[0-9]).*" DUMMY ${LUA_OUTPUT})
		SET(LUA_VERSION ${CMAKE_MATCH_1})

		IF (${LUA_VERSION} STRLESS "5.1")
			MESSAGE(FATAL_ERROR "Lua reports unsupported version ${LUA_VERSION}")
		ELSE (${LUA_VERSION} STRLESS "5.1")
			MESSAGE(STATUS "Found Lua version = ${LUA_VERSION}")
		ENDIF (${LUA_VERSION} STRLESS "5.1")

	ENDIF(${LuaInterp} STREQUAL "LuaInterp-NOTFOUND") 


	FIND_PATH(LUA_INCLUDE_DIR lua.h)

	FIND_LIBRARY(LUA_LIBRARY NAMES lua) 

	IF (LUA_INCLUDE_DIR AND LUA_LIBRARY)
	   SET(LUA_FOUND TRUE)
	ENDIF (LUA_INCLUDE_DIR AND LUA_LIBRARY)

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

