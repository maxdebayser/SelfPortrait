include_directories(${IoCContainer_SOURCE_DIR}/src/reflection)
include_directories(${IoCContainer_SOURCE_DIR}/src/utils)

add_subdirectory(reflection)
add_subdirectory(parser)
add_subdirectory(lua_module)
add_subdirectory(unit_test)
add_subdirectory(utils)
