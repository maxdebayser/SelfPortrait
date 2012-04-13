set(HEADERS
)

set(SOURCES
	luamodule.cpp
)


add_library(luacppreflect SHARED ${HEADERS} ${SOURCES})
target_link_libraries(luacppreflect reflection lua)
