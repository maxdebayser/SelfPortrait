set(HEADERS
	lua_utils.h
)

set(SOURCES
	lua_utils.cpp
)



SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")


add_library(utils STATIC ${HEADERS} ${SOURCES})
target_link_libraries(utils lua)
