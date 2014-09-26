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

    struct LuaStateHolder {
        explicit LuaStateHolder(lua_State* L, const std::string& addLuaPath = "", const std::string& addCPath = "");
        LuaStateHolder(const std::string& addLuaPath = "", const std::string& addCPath = "");
        ~LuaStateHolder() {
            lua_close(m_L);
        }
        operator lua_State*() { return m_L; }
    private:
        lua_State* m_L;
    };

}


inline std::string binpath() {
    return UNIT_BIN;
}

inline std::string srcpath() {
    return UNIT_SRC;
}

#endif /* TEST_UTILS */
