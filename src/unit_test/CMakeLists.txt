set(HEADERS
	attribute_test.h
	class_test.h
	constructor_test.h
	method_test.h
	utilities_test.h
	variant_test.h
)

set(SOURCES
	attribute_test.cpp
	class_test.cpp
	constructor_test.cpp
	method_test.cpp
	utilities_test.cpp
	variant_test.cpp
)

set(LIBS
	reflection
)

add_definitions(-D_CXXTEST_HAVE_STD -D_CXXTEST_HAVE_EH)


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
