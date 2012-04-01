set(HEADERS
	attribute.h
	class.h
	constructor.h
	function.h
	method.h
	reflection.h
	str_conversion.h
	typelist.h
	typeutils.h
	variant.h
)

set(SOURCES
	reflection.cpp
)


add_library(reflection STATIC ${HEADERS} ${SOURCES})
