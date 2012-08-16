SET(HEADERS
)

SET(SOURCES
	parser.cpp
)


SET(CLANG_LIBS
	clangFrontend
	clangDriver
    clangParse
    clangSema
    clangAnalysis
    clangAST
	clangLex
	clangBasic
	clangSerialization
	clangEdit
)
SET(CLANG_LIBS "${CLANG_LIBS} ${LLVM_ALL_LIBS}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXX_FLAGS} -fno-rtti")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LINKER_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LLVM_LINKER_FLAGS}")

SET(LIBS "${CLANG_LIBS}" dl pthread)

add_executable(parser ${HEADERS} ${SOURCES})

target_link_libraries(parser ${LIBS})
