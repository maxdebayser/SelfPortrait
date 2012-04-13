set(HEADERS
	attribute.h
	class.h
	constructor.h
	function.h
	method.h
	reflection.h
	reflection_impl.h
	str_conversion.h
	typelist.h
	typeutils.h
	variant.h
)

set(SOURCES
	reflection.cpp
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fPIC")


add_library(reflection STATIC ${HEADERS} ${SOURCES})
