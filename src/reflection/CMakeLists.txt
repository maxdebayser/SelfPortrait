set(HEADERS
	attribute.h
	call_utils.h
	collection_utils.h
	conversion_cache.h
	class.h
	constructor.h
	function.h
	method.h
	reflection.h
	reflection_impl.h
	str_conversion.h
	str_utils.h
	typelist.h
	typeutils.h
	variant.h
	proxy.h
)

set(SOURCES
	class.cpp
	constructor.cpp
	function.cpp
	method.cpp
	reflection.cpp
	str_utils.cpp
	variant.cpp
        proxy.cpp
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fPIC")


add_library(selfportrait SHARED ${HEADERS} ${SOURCES})

install(TARGETS selfportrait LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY ./ DESTINATION include/SelfPortrait FILES_MATCHING PATTERN "*.h")
