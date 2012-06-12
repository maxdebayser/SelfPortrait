#ifndef TEST_UTILS
#define TEST_UTILS

#ifndef NO_RTTI
#define WITH_RTTI(EXPR) EXPR
#else
#define WITH_RTTI(EXPR)
#endif

#include "lua_utils.h"

namespace LuaUtils {

	int atPanicThrow(lua_State* L);

	int ts_fail(lua_State* L);
	int ts_warn(lua_State* L);
	int ts_trace(lua_State* L);

	struct LuaStateHolder {
		explicit LuaStateHolder(lua_State* L) : m_L(L) {
			lua_atpanic(m_L, atPanicThrow);
			luaL_openlibs(m_L);
			lua_pushcfunction(m_L, ts_fail);
			lua_setglobal(L, "TS_FAIL");
			lua_pushcfunction(m_L, ts_trace);
			lua_setglobal(L, "TS_TRACE");
			lua_pushcfunction(m_L, ts_warn);
			lua_setglobal(L, "TS_WARN");
		}
		LuaStateHolder() : LuaStateHolder(luaL_newstate()) {}
		~LuaStateHolder() {
			lua_close(m_L);
		}
		operator lua_State*() { return m_L; }
	private:
		lua_State* m_L;
	};

}

#endif /* TEST_UTILS */
