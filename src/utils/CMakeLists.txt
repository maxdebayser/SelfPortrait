set(HEADERS
	lua_utils.h
)

set(SOURCES
	lua_utils.cpp
)


add_library(utils STATIC ${HEADERS} ${SOURCES})
target_link_libraries(utils lua)
