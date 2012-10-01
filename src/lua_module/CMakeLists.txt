set(HEADERS
)

set(SOURCES
	luamodule.cpp
)


set(LIBS
    reflection
    lua
    utils
)


add_library(luacppreflect SHARED ${HEADERS} ${SOURCES})
target_link_libraries(luacppreflect ${LIBS})
