/*
** SelfPortrait API
** See Copyright Notice in lua_utils.h
*/

#ifndef TEST_UTILS
#define TEST_UTILS

#ifndef NO_RTTI
#define WITH_RTTI(EXPR) EXPR
#else
#define WITH_RTTI(EXPR)
#endif

#include "lua_utils.h"

#include "str_conversion.h"
#include <string>

namespace LuaUtils {

    int atPanicThrow(lua_State* L);

    int ts_fail(lua_State* L);
    int ts_warn(lua_State* L);
    int ts_trace(lua_State* L);

    void addTestFunctionsAndPaths(lua_State* L);
}


inline std::string binpath() {
    return UNIT_BIN;
}

inline std::string srcpath() {
    return UNIT_SRC;
}

#endif /* TEST_UTILS */
