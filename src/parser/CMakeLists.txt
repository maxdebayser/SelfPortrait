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
	LLVMMC
	LLVMSupport
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXX_FLAGS} -fno-rtti")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LINKER_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LLVM_LINKER_FLAGS}")

SET(LIBS "${CLANG_LIBS}" dl pthread)

add_executable(parser ${HEADERS} ${SOURCES})

target_link_libraries(parser ${LIBS})
