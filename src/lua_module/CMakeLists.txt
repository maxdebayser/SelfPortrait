set(HEADERS
        luamodule.h
)

set(SOURCES
	luamodule.cpp
)


set(LIBS
	selfportrait
    lua
    utils
)


add_library(luaselfportrait SHARED ${HEADERS} ${SOURCES})
target_link_libraries(luaselfportrait ${LIBS})

install(TARGETS luaselfportrait LIBRARY DESTINATION "lib${LIBSUFFIX}/lua/${LUA_SHORT_VERSION}")
install(DIRECTORY ./ DESTINATION include/SelfPortrait FILES_MATCHING PATTERN "*.h")
