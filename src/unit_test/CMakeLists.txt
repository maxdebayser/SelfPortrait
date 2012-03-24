set(HEADERS
	variant_test.h
)

set(SOURCES
	variant_test.cpp
)

set(LIBS
	reflection
)

set(HEADER_FILES)
foreach(header ${HEADERS})
	list(APPEND HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach()

add_custom_command(
	OUTPUT runner.cpp
	COMMAND cxxtestgen --error-printer --have-eh --have-std -o runner.cpp "${HEADER_FILES}"
	DEPENDS ${HEADER_FILES}
)

add_executable(unit_test ${HEADERS} ${SOURCES} runner.cpp)

target_link_libraries(unit_test ${LIBS})

ADD_TEST(unit_test unit_test)
