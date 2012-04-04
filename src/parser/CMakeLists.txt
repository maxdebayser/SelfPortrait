SET(HEADERS
)

SET(SOURCES
	parser.cpp
)

SET(CLANG_LIBS
	libclang
)

SET(LIBS "${CLANG_LIBS}")


add_executable(parser ${HEADERS} ${SOURCES})

target_link_libraries(parser ${LIBS})

