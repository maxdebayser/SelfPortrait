set(HEADERS
	attribute_test.h
	class_test.h
	constructor_test.h
	function_test.h
	method_test.h
	parser_test.h
	proxy_test.h
	test_utils.h
	utilities_test.h
	variant_test.h
)

set(SOURCES
	attribute_test.cpp
	class_test.cpp
	constructor_test.cpp
	function_test.cpp
	method_test.cpp
	parser_test.cpp
	proxy_test.cpp
	test_utils.cpp
	utilities_test.cpp
	variant_test.cpp
)


set(LIBS
    reflection
    lua
	utils
)

SET(UNIT_SRC_DEF "\"${CMAKE_CURRENT_SOURCE_DIR}\"")
add_definitions(-DUNIT_SRC=${UNIT_SRC_DEF})


SET(UNIT_BIN_DEF "\"${CMAKE_CURRENT_BINARY_DIR}\"")
add_definitions(-DUNIT_BIN=${UNIT_BIN_DEF})


file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/parser_test_data")

add_definitions(-DCXXTEST_HAVE_STD -DCXXTEST_HAVE_EH)


set(HEADER_FILES)
foreach(header ${HEADERS})
	list(APPEND HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach()


add_custom_command(
	OUTPUT runner.cpp
	COMMAND cxxtestgen --error-printer --have-eh --have-std -o runner.cpp ${HEADER_FILES}
	DEPENDS ${HEADER_FILES}
)

add_executable(unit_test ${HEADERS} ${SOURCES} runner.cpp)


target_link_libraries(unit_test ${LIBS})

ADD_TEST(unit_test unit_test)
