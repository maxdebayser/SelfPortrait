SET(HEADERS
	test_functions.h
)

SET(SOURCES
	test_functions.cpp
	main.cpp
)

set(LIBS
	selfportrait
        ${LUA_LIBRARY}
	utils
)

add_executable(bench ${HEADERS} ${SOURCES})
target_link_libraries(bench ${LIBS})
